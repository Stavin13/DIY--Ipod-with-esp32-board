#ifndef SD_MANAGER_H
#define SD_MANAGER_H

#include <Arduino.h>
#include <SD.h>
#include <vector>
#include "../config.h"

struct MusicFile {
    String filename;
    String title;
    String artist;
    String album;
    uint32_t size;
    uint32_t duration;
};

struct Album {
    String name;
    String artist;
    std::vector<MusicFile> tracks;
};

struct Artist {
    String name;
    std::vector<Album> albums;
};

class SDManager {
private:
    bool initialized;
    std::vector<MusicFile> musicFiles;
    std::vector<Artist> artists;
    std::vector<Album> albums;
    
    bool isMusicFile(const String& filename);
    void scanDirectory(const String& path);
    void parseMusicFile(const String& filepath, MusicFile& file);
    void organizeLibrary();
    
public:
    SDManager();
    bool init();
    
    // Library management
    void scanLibrary();
    void refreshLibrary();
    
    // File access
    const std::vector<MusicFile>& getMusicFiles() const { return musicFiles; }
    const std::vector<Artist>& getArtists() const { return artists; }
    const std::vector<Album>& getAlbums() const { return albums; }
    
    // Search functions
    std::vector<MusicFile> searchByTitle(const String& query);
    std::vector<MusicFile> searchByArtist(const String& query);
    std::vector<MusicFile> searchByAlbum(const String& query);
    
    // File operations
    bool fileExists(const String& path);
    File openFile(const String& path);
    uint64_t getTotalSpace();
    uint64_t getUsedSpace();
    uint64_t getFreeSpace();
    
    // Playlist support (future)
    bool savePlaylist(const String& name, const std::vector<String>& files);
    std::vector<String> loadPlaylist(const String& name);
};

#endif // SD_MANAGER_H