/*
 * Complete iPod System - Integrated Test
 * ESP32-S3 + ILI9341 Display + TTP229 16-Key + SD Card + Audio
 * 
 * Hardware:
 * - Display: GPIO10(CS), GPIO9(DC), GPIO8(RST), GPIO11(MOSI), GPIO12(SCK), GPIO13(MISO)
 * - TTP229: GPIO1(SDO), GPIO2(SCL)
 * - SD Card: GPIO5(CS), GPIO11(MOSI), GPIO12(SCK), GPIO13(MISO) - shared SPI
 * - Audio: I2S output ready for DAC module
 */

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <SD.h>
#include <SPI.h>
#include <vector>

// Pin Definitions
#define TTP_SDO 1
#define TTP_SCL 2
#define SD_CS 5
#define SD_MOSI 11
#define SD_SCK 12
#define SD_MISO 13
#define TFT_CS 10  // Display CS pin

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
  Serial.println("║  Complete iPod System Test            ║");
  Serial.println("║  Display + Touch + SD + Audio Ready    ║");
  Serial.println("╚════════════════════════════════════════╝\n");
  
  // Initialize components
  Serial.println("🔧 Initializing components...\n");
  
  initDisplay();
  initTTP229();
  
  drawSplash();
  delay(2000);
  
  // Important: Small delay before SD init (shared SPI bus)
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
      // Touch started
      touchStartTime = millis();
      longPressHandled = false;
    } else if (touch == 0 && lastTouch != 0) {
      // Touch ended
      if (!longPressHandled) {
        handleTouch(lastTouch);
      }
    }
    lastTouch = touch;
    delay(50);
  }
  
  // Check for long press
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
  
  // Let TFT_eSPI initialize SPI automatically
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
  Serial.print("💾 SD Card... ");
  
  // Make sure display CS is HIGH (deselected)
  pinMode(TFT_CS, OUTPUT);
  digitalWrite(TFT_CS, HIGH);
  delay(10);
  
  // Initialize SD card CS pin
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);
  delay(10);
  
  // Try slow speed first (more reliable with shared SPI)
  Serial.print("trying 400kHz... ");
  if (!SD.begin(SD_CS, SPI, 400000, "/sd", 5, false)) {  // 5 = max files, false = don't format
    Serial.println("failed");
    Serial.print("   Trying 100kHz... ");
    
    // Try even slower
    delay(100);
    if (!SD.begin(SD_CS, SPI, 100000, "/sd", 5, false)) {
      Serial.println("failed");
      Serial.println("   ❌ SD card not working with display");
      Serial.println("   💡 Card works alone but conflicts on shared SPI");
      Serial.println("   ⚠️  Continuing without SD card...");
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
        
        // Extract title from filename
        String basename = file.name();
        int dotPos = basename.lastIndexOf('.');
        if (dotPos > 0) {
          basename = basename.substring(0, dotPos);
        }
        
        // Try to parse "Artist - Title" format
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
  // Find which key was pressed
  int key = 0;
  for (int i = 0; i < 16; i++) {
    if ((touch >> i) & 0x01) {
      key = i + 1;
      break;
    }
  }
  
  if (key == 0) return;
  
  // Fix inverted keys: key 1 is actually 16, key 16 is actually 1
  key = 17 - key;  // Invert: 1→16, 2→15, 3→14, ... 16→1
  
  Serial.printf("👆 Key %d pressed\n", key);
  
  // Key mapping (4x4 grid):
  // 1  2  3  4
  // 5  6  7  8
  // 9  10 11 12
  // 13 14 15 16
  
  switch (key) {
    case 2:  // Up
      navigateUp();
      break;
    case 10: // Down
      navigateDown();
      break;
    case 6:  // Left
      previousTrack();
      break;
    case 7:  // Right
      nextTrack();
      break;
    case 11: // Center/Select
      handleSelect();
      break;
    case 3:  // Volume Up
      volumeUp();
      break;
    case 15: // Volume Down
      volumeDown();
      break;
    case 1:  // Menu
      handleBack();
      break;
    case 4:  // Play/Pause
      togglePlayPause();
      break;
    case 8:  // SD Info
      currentScreen = SD_INFO;
      drawScreen();
      break;
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
    
    // Show volume overlay
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
    
    // Show volume overlay
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
  tft.print("Complete System Test");
  
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
  tft.print("Complete");
  
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
  
  // Status
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
    
    // Truncate long titles
    String title = musicLibrary[i].title;
    if (title.length() > 28) {
      title = title.substring(0, 25) + "...";
    }
    tft.print(title);
    
    tft.setTextColor(GRAY);
    tft.setCursor(10, y + 12);
    tft.print(musicLibrary[i].artist);
    
    // File size
    tft.setCursor(10, y + 22);
    float sizeMB = musicLibrary[i].size / (1024.0 * 1024.0);
    tft.printf("%.1f MB", sizeMB);
    
    if (i == currentTrack && isPlaying) {
      tft.setTextColor(GREEN);
      tft.setCursor(200, y + 10);
      tft.print("♪");
    }
  }
  
  // Scroll indicator
  if (musicLibrary.size() > visibleItems) {
    int barHeight = (visibleItems * 210) / musicLibrary.size();
    int barY = 60 + (scrollOffset * 210) / musicLibrary.size();
    tft.fillRect(235, barY, 3, barHeight, CYAN);
  }
  
  // Footer
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
  
  // Album art placeholder
  tft.drawRoundRect(60, 60, 120, 120, 10, WHITE);
  tft.fillRoundRect(62, 62, 116, 116, 8, GRAY);
  
  tft.setTextSize(6);
  tft.setTextColor(WHITE);
  tft.setCursor(105, 105);
  tft.print("♪");
  
  // Song info
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
  
  // Status
  tft.setTextSize(2);
  tft.setTextColor(isPlaying ? GREEN : RED);
  tft.setCursor(10, 235);
  tft.print(isPlaying ? "Playing" : "Paused");
  
  // Volume
  tft.setTextSize(1);
  tft.setTextColor(CYAN);
  tft.setCursor(150, 240);
  tft.print("Vol:");
  tft.print(volume);
  tft.print("%");
  
  // Track info
  tft.setTextColor(GRAY);
  tft.setCursor(10, 260);
  tft.printf("Track %d/%d", currentTrack + 1, musicLibrary.size());
  
  // File info
  tft.setCursor(10, 275);
  float sizeMB = musicLibrary[currentTrack].size / (1024.0 * 1024.0);
  tft.printf("%.1f MB", sizeMB);
  
  // Controls
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
    tft.print("Check:");
    tft.setCursor(20, 145);
    tft.print("- Card inserted?");
    tft.setCursor(20, 160);
    tft.print("- Wiring correct?");
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
  
  // Usage bar
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
  
  // Music files
  tft.setTextColor(CYAN);
  tft.setCursor(10, 200);
  tft.print("Music Files:");
  tft.setTextColor(WHITE);
  tft.setCursor(100, 200);
  tft.printf("%d", musicLibrary.size());
  
  // File types
  tft.setTextColor(GRAY);
  tft.setCursor(10, 220);
  tft.print("Supported:");
  tft.setCursor(10, 235);
  tft.print(".mp3 .wav .flac .m4a");
  
  // Footer
  tft.drawLine(0, 280, 240, 280, GRAY);
  tft.setTextColor(GRAY);
  tft.setCursor(10, 290);
  tft.print("Press 1 for Menu");
}

void printControls() {
  Serial.println("🎮 Touch Controls (16-Key Layout):");
  Serial.println("   ┌────┬────┬────┬────┐");
  Serial.println("   │ 1  │ 2  │ 3  │ 4  │  1:Menu  2:Up  3:Vol+  4:Play");
  Serial.println("   ├────┼────┼────┼────┤");
  Serial.println("   │ 5  │ 6  │ 7  │ 8  │  6:Prev  7:Next  8:SD Info");
  Serial.println("   ├────┼────┼────┼────┤");
  Serial.println("   │ 9  │ 10 │ 11 │ 12 │  10:Down  11:Select");
  Serial.println("   ├────┼────┼────┼────┤");
  Serial.println("   │ 13 │ 14 │ 15 │ 16 │  15:Vol-");
  Serial.println("   └────┴────┴────┴────┘");
  Serial.println();
  Serial.println("💡 Long Press any key = Back to Menu");
  Serial.println();
}
