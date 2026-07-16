/*
 * Complete iPod System - HSPI Version
 * SD Card on separate SPI bus - NO CONFLICTS!
 * 
 * NEW SD Card Wiring (HSPI):
 * SD Card → ESP32-S3
 * ─────────────────
 * CS   → GPIO15
 * MOSI → GPIO35
 * SCK  → GPIO36
 * MISO → GPIO37
 * VCC  → 3.3V
 * GND  → GND
 * 
 * Display stays on VSPI (GPIO 10, 11, 12, 13)
 * Touch stays same (GPIO 1, 2)
 */

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <SD.h>
#include <SPI.h>
#include <vector>

// Pin Definitions
#define TTP_SDO 1
#define TTP_SCL 2

// SD Card on HSPI (separate bus!)
#define SD_CS   15
#define SD_MOSI 35
#define SD_SCK  36
#define SD_MISO 37

#define TFT_CS 10

// Create HSPI instance for SD card
SPIClass hspi(HSPI);

// Display on default VSPI
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

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n╔════════════════════════════════════════╗");
  Serial.println("║  Complete iPod System - HSPI          ║");
  Serial.println("║  SD Card on Separate SPI Bus!         ║");
  Serial.println("╚════════════════════════════════════════╝\n");
  
  Serial.println("🔧 Initializing components...\n");
  
  initDisplay();
  initTTP229();
  
  drawSplash();
  delay(2000);
  
  // Small delay before SD init
  delay(100);
  
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
  Serial.print("📺 Display (VSPI)... ");
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
  Serial.print("💾 SD Card (HSPI)... ");
  
  // Initialize HSPI with custom pins
  hspi.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
  
  // Initialize SD card on HSPI bus
  if (!SD.begin(SD_CS, hspi, 4000000)) {  // 4MHz
    Serial.println("❌ FAILED at 4MHz");
    Serial.print("   Trying slower (400kHz)... ");
    
    if (!SD.begin(SD_CS, hspi, 400000)) {
      Serial.println("❌ FAILED");
      Serial.println("   Check new wiring:");
      Serial.println("   CS   → GPIO15");
      Serial.println("   MOSI → GPIO35");
      Serial.println("   SCK  → GPIO36");
      Serial.println("   MISO → GPIO37");
      return false;
    }
  }
  
  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("❌ No card detected");
    return false;
  }
  
  Serial.println("✅");
  
  Serial.print("   Type: ");
  if (cardType == CARD_MMC) Serial.println("MMC");
  else if (cardType == CARD_SD) Serial.println("SDSC");
  else if (cardType == CARD_SDHC) Serial.println("SDHC");
  
  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("   Size: %llu MB\n", cardSize);
  
  uint64_t usedBytes = SD.usedBytes() / (1024 * 1024);
  uint64_t totalBytes = SD.totalBytes() / (1024 * 1024);
  Serial.printf("   Used: %llu MB / %llu MB\n", usedBytes, totalBytes);
  
  return true;
}

void scanMusicFiles() {
  musicLibrary.clear();
  scanDirectory("/");
}

void scanDirectory(const char* dirname) {
  File root = SD.open(dirname);
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

void navigateUp() {
  if (currentScreen == LIBRARY && musicLibrary.size() > 0) {
    selectedItem--;
    if (selectedItem < 0) selectedItem = musicLibrary.size() - 1;
    if (selectedItem < scrollOffset) scrollOffset = selectedItem;
    drawScreen();
  } else if (currentScreen == HOME) {
    selectedItem = (selectedItem - 1 + 3) % 3;
    drawScreen();
  }
}

void navigateDown() {
  if (currentScreen == LIBRARY && musicLibrary.size() > 0) {
    selectedItem++;
    if (selectedItem >= musicLibrary.size()) selectedItem = 0;
    if (selectedItem >= scrollOffset + 6) scrollOffset = selectedItem - 5;
    drawScreen();
  } else if (currentScreen == HOME) {
    selectedItem = (selectedItem + 1) % 3;
    drawScreen();
  }
}

void handleSelect() {
  switch (currentScreen) {
    case HOME:
      if (selectedItem == 0) {
        currentScreen = LIBRARY;
        selectedItem = 0;
        scrollOffset = 0;
      } else if (selectedItem == 1) {
        currentScreen = NOW_PLAYING;
      } else if (selectedItem == 2) {
        currentScreen = SD_INFO;
      }
      drawScreen();
      break;
      
    case LIBRARY:
      if (musicLibrary.size() > 0) {
        currentTrack = selectedItem;
        currentScreen = NOW_PLAYING;
        isPlaying = true;
        Serial.printf("🎵 Selected: %s\n", musicLibrary[currentTrack].title.c_str());
        drawScreen();
      }
      break;
      
    case NOW_PLAYING:
      togglePlayPause();
      break;
  }
}

void handleBack() {
  if (currentScreen != HOME) {
    currentScreen = HOME;
    selectedItem = 0;
    drawScreen();
  }
}

void togglePlayPause() {
  isPlaying = !isPlaying;
  Serial.printf("🎵 %s\n", isPlaying ? "Playing" : "Paused");
  drawScreen();
}

void nextTrack() {
  if (currentScreen == NOW_PLAYING && musicLibrary.size() > 0) {
    currentTrack = (currentTrack + 1) % musicLibrary.size();
    Serial.printf("⏭️ Next: %s\n", musicLibrary[currentTrack].title.c_str());
    drawScreen();
  }
}

void previousTrack() {
  if (currentScreen == NOW_PLAYING && musicLibrary.size() > 0) {
    currentTrack = (currentTrack - 1 + musicLibrary.size()) % musicLibrary.size();
    Serial.printf("⏮️ Previous: %s\n", musicLibrary[currentTrack].title.c_str());
    drawScreen();
  }
}

void volumeUp() {
  if (volume < 100) {
    volume += 10;
    Serial.printf("🔊 Volume: %d%%\n", volume);
    
    tft.fillRect(40, 140, 160, 40, BLACK);
    tft.drawRect(40, 140, 160, 40, WHITE);
    int barWidth = (volume * 150) / 100;
    tft.fillRect(45, 145, barWidth, 30, GREEN);
    tft.setTextSize(2);
    tft.setTextColor(WHITE);
    tft.setCursor(90, 155);
    tft.print(volume);
    tft.print("%");
    delay(800);
    drawScreen();
  }
}

void volumeDown() {
  if (volume > 0) {
    volume -= 10;
    Serial.printf("🔉 Volume: %d%%\n", volume);
    
    tft.fillRect(40, 140, 160, 40, BLACK);
    tft.drawRect(40, 140, 160, 40, WHITE);
    int barWidth = (volume * 150) / 100;
    tft.fillRect(45, 145, barWidth, 30, GREEN);
    tft.setTextSize(2);
    tft.setTextColor(WHITE);
    tft.setCursor(90, 155);
    tft.print(volume);
    tft.print("%");
    delay(800);
    drawScreen();
  }
}

void drawScreen() {
  tft.fillScreen(BLACK);
  
  switch (currentScreen) {
    case SPLASH: drawSplash(); break;
    case HOME: drawHome(); break;
    case LIBRARY: drawLibrary(); break;
    case NOW_PLAYING: drawNowPlaying(); break;
    case SD_INFO: drawSDInfo(); break;
  }
}

void drawSplash() {
  tft.setTextSize(4);
  tft.setTextColor(WHITE);
  tft.setCursor(60, 100);
  tft.print("iPod");
  
  tft.setTextSize(1);
  tft.setTextColor(CYAN);
  tft.setCursor(40, 140);
  tft.print("HSPI - No Conflicts!");
  
  tft.setTextColor(YELLOW);
  tft.setCursor(30, 160);
  tft.print("Display+Touch+SD+Audio");
  
  tft.setTextColor(GREEN);
  tft.setCursor(70, 180);
  tft.print("ESP32-S3");
}

void drawHome() {
  tft.setTextSize(3);
  tft.setTextColor(WHITE);
  tft.setCursor(20, 20);
  tft.print("iPod");
  
  tft.setTextSize(1);
  tft.setTextColor(PURPLE);
  tft.setCursor(140, 30);
  tft.print("HSPI");
  
  const char* menu[] = {"Music Library", "Now Playing", "SD Card Info"};
  
  for (int i = 0; i < 3; i++) {
    int y = 80 + (i * 40);
    
    if (i == selectedItem) {
      tft.fillRoundRect(10, y - 5, 220, 35, 5, BLUE);
    }
    
    tft.setTextSize(2);
    tft.setTextColor(WHITE);
    tft.setCursor(20, y);
    tft.print(menu[i]);
    
    if (i == selectedItem) {
      tft.setCursor(200, y);
      tft.print(">");
    }
  }
  
  tft.setTextSize(1);
  tft.setTextColor(GRAY);
  tft.setCursor(10, 280);
  tft.printf("Songs: %d", musicLibrary.size());
  
  tft.setCursor(10, 295);
  tft.print("Vol: ");
  tft.print(volume);
  tft.print("%");
  
  tft.setCursor(10, 310);
  tft.print("Key 1:Menu 11:Select");
}

void drawLibrary() {
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.setCursor(20, 15);
  tft.print("Library");
  
  tft.setTextSize(1);
  tft.setTextColor(GRAY);
  tft.setCursor(20, 35);
  tft.printf("%d songs", musicLibrary.size());
  
  tft.drawLine(0, 50, 240, 50, GRAY);
  
  if (musicLibrary.size() == 0) {
    tft.setTextSize(1);
    tft.setTextColor(RED);
    tft.setCursor(20, 100);
    tft.print("No music files found!");
    
    tft.setTextColor(YELLOW);
    tft.setCursor(20, 120);
    tft.print("Add .mp3/.wav files");
    tft.setCursor(20, 135);
    tft.print("to SD card");
    return;
  }
  
  int visibleItems = 6;
  int startIdx = scrollOffset;
  int endIdx = min(startIdx + visibleItems, (int)musicLibrary.size());
  
  for (int i = startIdx; i < endIdx; i++) {
    int y = 60 + ((i - startIdx) * 35);
    
    if (i == selectedItem) {
      tft.fillRoundRect(5, y - 3, 230, 32, 3, BLUE);
    }
    
    tft.setTextSize(1);
    tft.setTextColor(WHITE);
    tft.setCursor(10, y);
    
    String title = musicLibrary[i].title;
    if (title.length() > 28) {
      title = title.substring(0, 25) + "...";
    }
    tft.print(title);
    
    tft.setTextColor(GRAY);
    tft.setCursor(10, y + 12);
    tft.print(musicLibrary[i].artist);
    
    tft.setCursor(10, y + 22);
    float sizeMB = musicLibrary[i].size / (1024.0 * 1024.0);
    tft.printf("%.1f MB", sizeMB);
    
    if (i == currentTrack && isPlaying) {
      tft.setTextColor(GREEN);
      tft.setCursor(200, y + 10);
      tft.print("♪");
    }
  }
  
  if (musicLibrary.size() > visibleItems) {
    int barHeight = (visibleItems * 210) / musicLibrary.size();
    int barY = 60 + (scrollOffset * 210) / musicLibrary.size();
    tft.fillRect(235, barY, 3, barHeight, CYAN);
  }
  
  tft.drawLine(0, 280, 240, 280, GRAY);
  tft.setTextSize(1);
  tft.setTextColor(GRAY);
  tft.setCursor(10, 290);
  tft.print("2/10:Nav 11:Play 1:Back");
}

void drawNowPlaying() {
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.setCursor(20, 15);
  tft.print("Now Playing");
  
  tft.drawLine(0, 40, 240, 40, GRAY);
  
  if (musicLibrary.size() == 0) {
    tft.setTextSize(1);
    tft.setTextColor(RED);
    tft.setCursor(40, 120);
    tft.print("No songs loaded");
    return;
  }
  
  tft.drawRoundRect(60, 60, 120, 120, 10, WHITE);
  tft.fillRoundRect(62, 62, 116, 116, 8, GRAY);
  
  tft.setTextSize(6);
  tft.setTextColor(WHITE);
  tft.setCursor(105, 105);
  tft.print("♪");
  
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setCursor(10, 195);
  String title = musicLibrary[currentTrack].title;
  if (title.length() > 30) {
    title = title.substring(0, 27) + "...";
  }
  tft.print(title);
  
  tft.setTextColor(GRAY);
  tft.setCursor(10, 210);
  tft.print(musicLibrary[currentTrack].artist);
  
  tft.setTextSize(2);
  tft.setTextColor(isPlaying ? GREEN : RED);
  tft.setCursor(10, 235);
  tft.print(isPlaying ? "Playing" : "Paused");
  
  tft.setTextSize(1);
  tft.setTextColor(CYAN);
  tft.setCursor(150, 240);
  tft.print("Vol:");
  tft.print(volume);
  tft.print("%");
  
  tft.setTextColor(GRAY);
  tft.setCursor(10, 260);
  tft.printf("Track %d/%d", currentTrack + 1, musicLibrary.size());
  
  tft.setCursor(10, 275);
  float sizeMB = musicLibrary[currentTrack].size / (1024.0 * 1024.0);
  tft.printf("%.1f MB", sizeMB);
  
  tft.drawLine(0, 290, 240, 290, GRAY);
  tft.setCursor(10, 300);
  tft.print("4:Play 6/7:Prev/Next");
}

void drawSDInfo() {
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.setCursor(20, 15);
  tft.print("SD Card");
  
  tft.drawLine(0, 40, 240, 40, GRAY);
  
  uint8_t cardType = SD.cardType();
  
  if (cardType == CARD_NONE) {
    tft.setTextSize(1);
    tft.setTextColor(RED);
    tft.setCursor(40, 100);
    tft.print("No SD card detected!");
    
    tft.setTextColor(YELLOW);
    tft.setCursor(20, 130);
    tft.print("Check HSPI wiring:");
    tft.setCursor(20, 145);
    tft.print("CS→15 MOSI→35");
    tft.setCursor(20, 160);
    tft.print("SCK→36 MISO→37");
    return;
  }
  
  tft.setTextSize(1);
  tft.setTextColor(CYAN);
  tft.setCursor(10, 60);
  tft.print("Card Type:");
  tft.setTextColor(WHITE);
  tft.setCursor(100, 60);
  if (cardType == CARD_MMC) tft.print("MMC");
  else if (cardType == CARD_SD) tft.print("SDSC");
  else if (cardType == CARD_SDHC) tft.print("SDHC");
  
  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  tft.setTextColor(CYAN);
  tft.setCursor(10, 80);
  tft.print("Total Size:");
  tft.setTextColor(WHITE);
  tft.setCursor(100, 80);
  tft.printf("%llu MB", cardSize);
  
  uint64_t usedBytes = SD.usedBytes() / (1024 * 1024);
  tft.setTextColor(CYAN);
  tft.setCursor(10, 100);
  tft.print("Used:");
  tft.setTextColor(WHITE);
  tft.setCursor(100, 100);
  tft.printf("%llu MB", usedBytes);
  
  uint64_t totalBytes = SD.totalBytes() / (1024 * 1024);
  uint64_t freeBytes = totalBytes - usedBytes;
  tft.setTextColor(CYAN);
  tft.setCursor(10, 120);
  tft.print("Free:");
  tft.setTextColor(WHITE);
  tft.setCursor(100, 120);
  tft.printf("%llu MB", freeBytes);
  
  tft.setTextColor(CYAN);
  tft.setCursor(10, 150);
  tft.print("Usage:");
  
  int barWidth = (usedBytes * 200) / totalBytes;
  tft.drawRect(10, 165, 220, 20, WHITE);
  tft.fillRect(12, 167, barWidth, 16, GREEN);
  
  float usedPercent = (float)usedBytes / totalBytes * 100;
  tft.setTextColor(WHITE);
  tft.setCursor(90, 170);
  tft.printf("%.1f%%", usedPercent);
  
  tft.setTextColor(CYAN);
  tft.setCursor(10, 200);
  tft.print("Music Files:");
  tft.setTextColor(WHITE);
  tft.setCursor(100, 200);
  tft.printf("%d", musicLibrary.size());
  
  tft.setTextColor(GRAY);
  tft.setCursor(10, 220);
  tft.print("Bus: HSPI (No conflict!)");
  
  tft.drawLine(0, 280, 240, 280, GRAY);
  tft.setTextColor(GRAY);
  tft.setCursor(10, 290);
  tft.print("Press 1 for Menu");
}

void printControls() {
  Serial.println("🎮 Touch Controls:");
  Serial.println("   Key 1: Menu    Key 2: Up      Key 3: Vol+");
  Serial.println("   Key 4: Play    Key 6: Prev    Key 7: Next");
  Serial.println("   Key 8: SD Info Key 10: Down   Key 11: Select");
  Serial.println("   Key 15: Vol-   Long Press: Back");
  Serial.println();
  Serial.println("📌 SD Card Wiring (HSPI - Separate Bus):");
  Serial.println("   CS   → GPIO15");
  Serial.println("   MOSI → GPIO35");
  Serial.println("   SCK  → GPIO36");
  Serial.println("   MISO → GPIO37");
  Serial.println();
  Serial.println("📺 Display stays on VSPI (GPIO 10,11,12,13)");
  Serial.println("👆 Touch stays same (GPIO 1,2)");
}
