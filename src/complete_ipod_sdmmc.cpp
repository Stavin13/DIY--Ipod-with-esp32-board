/*
 * Complete iPod System - SD_MMC Version (No SPI Conflict!)
 * Uses ESP32's dedicated SD/MMC hardware instead of SPI
 * 
 * NEW SD Card Wiring (4-bit SD mode):
 * SD Card → ESP32-S3
 * ─────────────────
 * CLK  → GPIO14
 * CMD  → GPIO15
 * D0   → GPIO2
 * D1   → GPIO4  (optional for 4-bit)
 * D2   → GPIO12 (optional for 4-bit)
 * D3   → GPIO13 (optional for 4-bit)
 * VCC  → 3.3V
 * GND  → GND
 * 
 * OR use 1-bit mode (only 3 wires):
 * CLK  → GPIO14
 * CMD  → GPIO15
 * D0   → GPIO2
 * VCC  → 3.3V
 * GND  → GND
 */

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <SD_MMC.h>  // Use SD_MMC instead of SD!
#include <vector>

// Pin Definitions
#define TTP_SDO 1
#define TTP_SCL 2
#define TFT_CS 10

// Display
TFT_eSPI tft = TFT_eSPI();

// Colors
#define BLACK 0x0000
#define WHITE 0xFFFF
#define BLUE 0x001F
#define GRAY 0x8410
#define GREEN 0x07E0
#define RED 0xF800
#define CYAN 0x07FF
#define YELLOW 0xFFE0
#define PURPLE 0x780F
#define ORANGE 0xFD20

// Music file structure
struct MusicFile {
  String filename;
  String title;
  String artist;
  uint32_t size;
};

std::vector<MusicFile> musicLibrary;

// UI State
enum Screen { SPLASH, HOME, LIBRARY, NOW_PLAYING, SD_INFO };
Screen currentScreen = SPLASH;
int selectedItem = 0;
int scrollOffset = 0;
bool isPlaying = false;
int volume = 50;
int currentTrack = 0;

// Touch state
uint16_t lastTouch = 0;
unsigned long touchStartTime = 0;
bool longPressHandled = false;

// Function declarations
void initDisplay();
void initTTP229();
bool initSDCard();
void scanMusicFiles();
void scanDirectory(const char* dirname);
uint16_t readTTP229();
void handleTouch(uint16_t touch);
void drawScreen();
void drawSplash();
void drawHome();
void drawLibrary();
void drawNowPlaying();
void drawSDInfo();
void navigateUp();
void navigateDown();
void handleSelect();
void handleBack();
void togglePlayPause();
void nextTrack();
void previousTrack();
void volumeUp();
void volumeDown();

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n╔════════════════════════════════════════╗");
  Serial.println("║  Complete iPod System - SD_MMC        ║");
  Serial.println("║  No SPI Conflict!                      ║");
  Serial.println("╚════════════════════════════════════════╝\n");
  
  Serial.println("🔧 Initializing components...\n");
  
  initDisplay();
  initTTP229();
  
  drawSplash();
  delay(2000);
  
  bool sdOk = initSDCard();
  
  if (sdOk) {
    Serial.println("\n📂 Scanning for music files...");
    scanMusicFiles();
    Serial.printf("✅ Found %d music files\n\n", musicLibrary.size());
  }
  
  currentScreen = HOME;
  drawScreen();
  
  Serial.println("✅ System Ready!\n");
  printControls();
}

void loop() {
  uint16_t touch = readTTP229();
  
  if (touch != lastTouch) {
    if (touch != 0 && lastTouch == 0) {
      touchStartTime = millis();
      longPressHandled = false;
    } else if (touch == 0 && lastTouch != 0) {
      if (!longPressHandled) {
        handleTouch(lastTouch);
      }
    }
    lastTouch = touch;
    delay(50);
  }
  
  if (touch != 0 && !longPressHandled) {
    if (millis() - touchStartTime > 800) {
      Serial.println("🔙 Long Press - Back");
      handleBack();
      longPressHandled = true;
    }
  }
  
  delay(10);
}

void initDisplay() {
  Serial.print("📺 Display... ");
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(BLACK);
  Serial.println("✅");
}

void initTTP229() {
  Serial.print("👆 TTP229 Touch... ");
  pinMode(TTP_SDO, INPUT);
  pinMode(TTP_SCL, OUTPUT);
  digitalWrite(TTP_SCL, HIGH);
  Serial.println("✅");
}

bool initSDCard() {
  Serial.print("💾 SD Card (SD_MMC)... ");
  
  // Use 1-bit mode for simplicity (only needs 3 data wires)
  // Set to false for 4-bit mode (faster but needs more wires)
  if (!SD_MMC.begin("/sdcard", true)) {  // true = 1-bit mode
    Serial.println("❌ FAILED");
    Serial.println("   SD_MMC uses different pins than SPI!");
    Serial.println("   Rewire SD card to:");
    Serial.println("   CLK → GPIO14");
    Serial.println("   CMD → GPIO15");
    Serial.println("   D0  → GPIO2");
    Serial.println("   (Or keep SPI wiring and skip SD for now)");
    return false;
  }
  
  uint8_t cardType = SD_MMC.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("❌ No card detected");
    return false;
  }
  
  Serial.println("✅");
  
  Serial.print("   Type: ");
  if (cardType == CARD_MMC) Serial.println("MMC");
  else if (cardType == CARD_SD) Serial.println("SDSC");
  else if (cardType == CARD_SDHC) Serial.println("SDHC");
  
  uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
  Serial.printf("   Size: %llu MB\n", cardSize);
  
  uint64_t usedBytes = SD_MMC.usedBytes() / (1024 * 1024);
  uint64_t totalBytes = SD_MMC.totalBytes() / (1024 * 1024);
  Serial.printf("   Used: %llu MB / %llu MB\n", usedBytes, totalBytes);
  
  return true;
}

void scanMusicFiles() {
  musicLibrary.clear();
  scanDirectory("/");
}

void scanDirectory(const char* dirname) {
  File root = SD_MMC.open(dirname);
  if (!root || !root.isDirectory()) return;
  
  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      String path = String(dirname) + "/" + file.name();
      scanDirectory(path.c_str());
    } else {
      String filename = file.name();
      filename.toLowerCase();
      
      if (filename.endsWith(".mp3") || filename.endsWith(".wav") || 
          filename.endsWith(".flac") || filename.endsWith(".m4a")) {
        
        MusicFile music;
        music.filename = String(dirname) + "/" + file.name();
        
        String basename = file.name();
        int dotPos = basename.lastIndexOf('.');
        if (dotPos > 0) {
          basename = basename.substring(0, dotPos);
        }
        
        int dashPos = basename.indexOf(" - ");
        if (dashPos > 0) {
          music.artist = basename.substring(0, dashPos);
          music.title = basename.substring(dashPos + 3);
        } else {
          music.title = basename;
          music.artist = "Unknown";
        }
        
        music.size = file.size();
        musicLibrary.push_back(music);
        
        Serial.printf("   🎵 %s\n", music.title.c_str());
      }
    }
    file = root.openNextFile();
  }
}

uint16_t readTTP229() {
  uint16_t value = 0;
  
  digitalWrite(TTP_SCL, HIGH);
  delayMicroseconds(100);
  
  for (int i = 0; i < 16; i++) {
    digitalWrite(TTP_SCL, LOW);
    delayMicroseconds(5);
    
    value <<= 1;
    if (digitalRead(TTP_SDO)) {
      value |= 1;
    }
    
    digitalWrite(TTP_SCL, HIGH);
    delayMicroseconds(5);
  }
  
  return ~value;
}

void handleTouch(uint16_t touch) {
  int key = 0;
  for (int i = 0; i < 16; i++) {
    if ((touch >> i) & 0x01) {
      key = i + 1;
      break;
    }
  }
  
  if (key == 0) return;
  
  key = 17 - key;  // Fix inverted keys
  
  Serial.printf("👆 Key %d pressed\n", key);
  
  switch (key) {
    case 2:  navigateUp(); break;
    case 10: navigateDown(); break;
    case 6:  previousTrack(); break;
    case 7:  nextTrack(); break;
    case 11: handleSelect(); break;
    case 3:  volumeUp(); break;
    case 15: volumeDown(); break;
    case 1:  handleBack(); break;
    case 4:  togglePlayPause(); break;
    case 8:  currentScreen = SD_INFO; drawScreen(); break;
  }
}

// ... (rest of the functions are the same as complete_ipod_system.cpp)
// I'll add them in the next part to keep this under 50 lines

void printControls() {
  Serial.println("🎮 Controls:");
  Serial.println("   Key 1: Menu    Key 2: Up      Key 3: Vol+");
  Serial.println("   Key 4: Play    Key 6: Prev    Key 7: Next");
  Serial.println("   Key 8: SD Info Key 10: Down   Key 11: Select");
  Serial.println("   Key 15: Vol-   Long Press: Back");
}
