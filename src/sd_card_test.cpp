/*
 * SD Card Module Test
 * Tests basic SD card functionality with your Micro SD Card Reader Module
 * 
 * Wiring:
 * SD Module → ESP32-S3
 * VCC  → 3.3V
 * GND  → GND
 * CS   → GPIO5
 * MOSI → GPIO11
 * SCK  → GPIO12
 * MISO → GPIO13
 */

#include <Arduino.h>
#include <SD.h>
#include <SPI.h>

// SD Card pins
#define SD_CS    5
#define SD_MOSI  11
#define SD_SCK   12
#define SD_MISO  13

void printCardInfo();
void testReadWrite();
void listDirectory(const char* dirname, uint8_t levels);
void scanMusicFiles();

void setup() {
  Serial.begin(115200);
  delay(2000);  // Wait for serial monitor
  
  Serial.println("\n╔════════════════════════════════════╗");
  Serial.println("║   SD Card Module Test              ║");
  Serial.println("║   Micro SD Card Reader Module      ║");
  Serial.println("╚════════════════════════════════════╝\n");
  
  // Initialize SPI
  Serial.println("🔧 Initializing SPI bus...");
  SPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
  Serial.println("✅ SPI initialized");
  
  // Initialize SD card
  Serial.println("\n💾 Initializing SD card...");
  Serial.println("   CS pin: GPIO5");
  
  if (!SD.begin(SD_CS)) {
    Serial.println("\n❌ SD Card initialization FAILED!");
    Serial.println("\n🔍 Troubleshooting:");
    Serial.println("   1. Is the SD card inserted?");
    Serial.println("   2. Check wiring:");
    Serial.println("      VCC  → 3.3V");
    Serial.println("      GND  → GND");
    Serial.println("      CS   → GPIO5");
    Serial.println("      MOSI → GPIO11");
    Serial.println("      SCK  → GPIO12");
    Serial.println("      MISO → GPIO13");
    Serial.println("   3. Is card formatted as FAT32?");
    Serial.println("   4. Try a different SD card");
    return;
  }
  
  Serial.println("✅ SD Card initialized successfully!\n");
  
  // Print card information
  printCardInfo();
  
  // Test read/write
  testReadWrite();
  
  // List root directory
  Serial.println("\n📂 Root Directory:");
  Serial.println("─────────────────────────────────────");
  listDirectory("/", 0);
  
  // Scan for music files
  scanMusicFiles();
  
  Serial.println("\n╔════════════════════════════════════╗");
  Serial.println("║   Test Complete!                   ║");
  Serial.println("╚════════════════════════════════════╝");
}

void loop() {
  // Nothing to do
}

void printCardInfo() {
  Serial.println("📊 SD Card Information:");
  Serial.println("─────────────────────────────────────");
  
  uint8_t cardType = SD.cardType();
  
  if (cardType == CARD_NONE) {
    Serial.println("❌ No SD card attached");
    return;
  }
  
  Serial.print("📇 Card Type: ");
  switch (cardType) {
    case CARD_MMC:
      Serial.println("MMC");
      break;
    case CARD_SD:
      Serial.println("SDSC (Standard Capacity)");
      break;
    case CARD_SDHC:
      Serial.println("SDHC (High Capacity)");
      break;
    default:
      Serial.println("UNKNOWN");
  }
  
  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("💾 Total Size: %llu MB\n", cardSize);
  
  uint64_t usedBytes = SD.usedBytes() / (1024 * 1024);
  Serial.printf("📊 Used Space: %llu MB\n", usedBytes);
  
  uint64_t totalBytes = SD.totalBytes() / (1024 * 1024);
  uint64_t freeBytes = totalBytes - usedBytes;
  Serial.printf("📂 Free Space: %llu MB\n", freeBytes);
  
  float usedPercent = (float)usedBytes / totalBytes * 100;
  Serial.printf("📈 Usage: %.1f%%\n", usedPercent);
}

void testReadWrite() {
  Serial.println("\n📝 Testing Read/Write:");
  Serial.println("─────────────────────────────────────");
  
  // Test write
  Serial.print("Writing test file... ");
  File file = SD.open("/ipod_test.txt", FILE_WRITE);
  if (!file) {
    Serial.println("❌ FAILED");
    return;
  }
  
  file.println("iPod SD Card Test");
  file.println("ESP32-S3 + Micro SD Module");
  file.printf("Timestamp: %lu ms\n", millis());
  file.close();
  Serial.println("✅ SUCCESS");
  
  // Test read
  Serial.print("Reading test file... ");
  file = SD.open("/ipod_test.txt");
  if (!file) {
    Serial.println("❌ FAILED");
    return;
  }
  
  Serial.println("✅ SUCCESS");
  Serial.println("\n📄 File Contents:");
  while (file.available()) {
    Serial.write(file.read());
  }
  file.close();
  
  // Test delete
  Serial.print("\nDeleting test file... ");
  if (SD.remove("/ipod_test.txt")) {
    Serial.println("✅ SUCCESS");
  } else {
    Serial.println("❌ FAILED");
  }
}

void listDirectory(const char* dirname, uint8_t levels) {
  File root = SD.open(dirname);
  if (!root) {
    Serial.println("❌ Failed to open directory");
    return;
  }
  
  if (!root.isDirectory()) {
    Serial.println("❌ Not a directory");
    return;
  }
  
  File file = root.openNextFile();
  int fileCount = 0;
  int dirCount = 0;
  
  while (file) {
    // Print indentation
    for (uint8_t i = 0; i < levels; i++) {
      Serial.print("  ");
    }
    
    if (file.isDirectory()) {
      Serial.printf("📁 %s/\n", file.name());
      dirCount++;
      
      // Recursively list subdirectories (up to 2 levels)
      if (levels < 2) {
        String path = String(dirname) + "/" + file.name();
        listDirectory(path.c_str(), levels + 1);
      }
    } else {
      Serial.printf("📄 %s", file.name());
      
      // Show file size
      uint32_t size = file.size();
      if (size < 1024) {
        Serial.printf(" (%d B)\n", size);
      } else if (size < 1024 * 1024) {
        Serial.printf(" (%.1f KB)\n", size / 1024.0);
      } else {
        Serial.printf(" (%.1f MB)\n", size / 1024.0 / 1024.0);
      }
      
      fileCount++;
    }
    
    file = root.openNextFile();
  }
  
  if (levels == 0) {
    Serial.println("─────────────────────────────────────");
    Serial.printf("Total: %d files, %d folders\n", fileCount, dirCount);
  }
}

void scanMusicFiles() {
  Serial.println("\n🎵 Scanning for Music Files:");
  Serial.println("─────────────────────────────────────");
  
  int musicCount = 0;
  scanMusicDirectory("/", musicCount, 0);
  
  Serial.println("─────────────────────────────────────");
  if (musicCount == 0) {
    Serial.println("❌ No music files found");
    Serial.println("\n💡 Tip: Copy .mp3, .wav, or .flac files to SD card");
  } else {
    Serial.printf("✅ Found %d music files\n", musicCount);
  }
}

void scanMusicDirectory(const char* dirname, int& count, uint8_t level) {
  if (level > 3) return;  // Limit recursion depth
  
  File root = SD.open(dirname);
  if (!root || !root.isDirectory()) return;
  
  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      String path = String(dirname) + "/" + file.name();
      scanMusicDirectory(path.c_str(), count, level + 1);
    } else {
      String filename = file.name();
      filename.toLowerCase();
      
      if (filename.endsWith(".mp3") || 
          filename.endsWith(".wav") ||
          filename.endsWith(".flac") ||
          filename.endsWith(".aac") ||
          filename.endsWith(".m4a")) {
        
        // Print indentation
        for (uint8_t i = 0; i < level; i++) {
          Serial.print("  ");
        }
        
        Serial.printf("🎵 %s", file.name());
        
        // Show file size
        uint32_t size = file.size();
        if (size < 1024 * 1024) {
          Serial.printf(" (%.1f KB)\n", size / 1024.0);
        } else {
          Serial.printf(" (%.1f MB)\n", size / 1024.0 / 1024.0);
        }
        
        count++;
      }
    }
    
    file = root.openNextFile();
  }
}
