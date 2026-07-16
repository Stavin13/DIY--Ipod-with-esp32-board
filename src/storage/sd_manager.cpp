#include "sd_manager.h"

SDManager::SDManager() : initialized(false) {
}

bool SDManager::init() {
    if (!SD.begin(SD_CS)) {
        Serial.println("SD card initialization failed!");
        return false;
    }
    
    uint8_t cardType = SD.cardType();
    if (cardType == CARD_NONE) {
        Serial.println("No SD card attached");
        return false;
    }
    
    Serial.print("SD Card Type: ");
    if (cardType == CARD_MMC) {
        Serial.println("MMC");
    } else if (cardType == CARD_SD) {
        Serial.println("SDSC");
    } else if (cardType == CARD_SDHC) {
        Serial.println("SDHC");
    } else {
        Serial.println("UNKNOWN");
    }
    
    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);
    
    initialized = true;
    
    // Start initial library scan
    scanLibrary();
    
    return true;
}

void SDManager::scanLibrary() {
    if (!initialized) return;
    
    Serial.println("Scanning music library...");
    musicFiles.clear();
    artists.clear();
    albums.clear();
    
    scanDirectory("/");
    organizeLibrary();
    
    Serial.printf("Library scan complete: %d files found\n", musicFiles.size());
}

void SDManager::scanDirectory(const String& path) {
    File root = SD.open(path);
    if (!root || !root.isDirectory()) {
        return;
    }
    
    File file = root.openNextFile();
    while (file) {
        String filename = file.name();
        
        if (file.isDirectory()) {
            // Recursively scan subdirectories
            scanDirectory(path + "/" + filename);
        } else if (isMusicFile(filename)) {
            MusicFile musicFile;
            String fullPath = path + "/" + filename;
            parseMusicFile(fullPath, musicFile);
            musicFiles.push_back(musicFile);
        }
        
        file = root.openNextFile();
    }
    root.close();
}

bool SDManager::isMusicFile(const String& filename) {
    String extensions = MUSIC_EXTENSIONS;
    String lowerFilename = filename;
    lowerFilename.toLowerCase();
    
    // Check if filename ends with any supported extension
    int pos = 0;
    while (pos < extensions.length()) {
        int nextComma = extensions.indexOf(',', pos);
        if (nextComma == -1) nextComma = extensions.length();
        
        String ext = extensions.substring(pos, nextComma);
        if (lowerFilename.endsWith(ext)) {
            return true;
        }
        
        pos = nextComma + 1;
    }
    
    return false;
}

void SDManager::parseMusicFile(const String& filepath, MusicFile& file) {
    file.filename = filepath;
    
    // Extract basic info from filename
    int lastSlash = filepath.lastIndexOf('/');
    int lastDot = filepath.lastIndexOf('.');
    
    String basename = filepath.substring(lastSlash + 1, lastDot);
    
    // Try to parse "Artist - Title" format
    int dashPos = basename.indexOf(" - ");
    if (dashPos > 0) {
        file.artist = basename.substring(0, dashPos);
        file.title = basename.substring(dashPos + 3);
    } else {
        file.title = basename;
        file.artist = "Unknown Artist";
    }
    
    // Default album name from directory
    String dirPath = filepath.substring(0, lastSlash);
    int prevSlash = dirPath.lastIndexOf('/');
    if (prevSlash >= 0) {
        file.album = dirPath.substring(prevSlash + 1);
    } else {
        file.album = "Unknown Album";
    }
    
    // Get file size
    File f = SD.open(filepath);
    if (f) {
        file.size = f.size();
        f.close();
    } else {
        file.size = 0;
    }
    
    // Duration would need ID3 tag parsing or estimation
    file.duration = 0;
}

void SDManager::organizeLibrary() {
    // Group files by artist and album
    for (const auto& musicFile : musicFiles) {
        // Find or create artist
        Artist* artist = nullptr;
        for (auto& a : artists) {
            if (a.name == musicFile.artist) {
                artist = &a;
                break;
            }
        }
        
        if (!artist) {
            artists.push_back(Artist{musicFile.artist, {}});
            artist = &artists.back();
        }
        
        // Find or create album
        Album* album = nullptr;
        for (auto& alb : artist->albums) {
            if (alb.name == musicFile.album) {
                album = &alb;
                break;
            }
        }
        
        if (!album) {
            artist->albums.push_back(Album{musicFile.album, musicFile.artist, {}});
            album = &artist->albums.back();
        }
        
        // Add track to album
        album->tracks.push_back(musicFile);
    }
    
    // Also create a flat album list
    albums.clear();
    for (const auto& artist : artists) {
        for (const auto& album : artist.albums) {
            albums.push_back(album);
        }
    }
}

std::vector<MusicFile> SDManager::searchByTitle(const String& query) {
    std::vector<MusicFile> results;
    String lowerQuery = query;
    lowerQuery.toLowerCase();
    
    for (const auto& file : musicFiles) {
        String lowerTitle = file.title;
        lowerTitle.toLowerCase();
        if (lowerTitle.indexOf(lowerQuery) >= 0) {
            results.push_back(file);
        }
    }
    
    return results;
}

uint64_t SDManager::getTotalSpace() {
    return initialized ? SD.totalBytes() : 0;
}

uint64_t SDManager::getUsedSpace() {
    return initialized ? SD.usedBytes() : 0;
}

uint64_t SDManager::getFreeSpace() {
    return getTotalSpace() - getUsedSpace();
}