/*
 * Complete iPod System - No SD Card Version
 * Works perfectly with just Display + Touch
 * Add SD card support later when ready
 * 
 * Hardware:
 * - Display: GPIO10(CS), GPIO9(DC), GPIO8(RST), GPIO11(MOSI), GPIO12(SCK), GPIO13(MISO)
 * - TTP229: GPIO1(SDO), GPIO2(SCL)
 */

#include <Arduino.h>
#include <TFT_eSPI.h>

// Pin Definitions
#define TTP_SDO 1
#define TTP_SCL 2

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

// Demo songs (no SD card needed)
const char* demoSongs[] = {
  "Bohemian Rhapsody", "Stairway to Heaven", "Hotel California",
  "Imagine", "Sweet Child O' Mine", "Billie Jean",
  "Smells Like Teen Spirit", "Hey Jude", "Purple Rain"
};

const char* demoArtists[] = {
  "Queen", "Led Zeppelin", "Eagles",
  "John Lennon", "Guns N' Roses", "Michael Jackson",
  "Nirvana", "The Beatles", "Prince"
};

const int totalSongs = 9;

// UI State
enum Screen { SPLASH, HOME, LIBRARY, NOW_PLAYING };
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
  Serial.println("║  iPod System - Demo Mode              ║");
  Serial.println("║  Display + Touch Working!              ║");
  Serial.println("╚════════════════════════════════════════╝\n");
  
  Serial.println("🔧 Initializing...\n");
  
  // Display
  Serial.print("📺 Display... ");
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(BLACK);
  Serial.println("✅");
  
  // Touch
  Serial.print("👆 TTP229 Touch... ");
  pinMode(TTP_SDO, INPUT);
  pinMode(TTP_SCL, OUTPUT);
  digitalWrite(TTP_SCL, HIGH);
  Serial.println("✅");
  
  Serial.println("💾 SD Card... Skipped (Demo Mode)");
  Serial.printf("🎵 Loaded %d demo songs\n\n", totalSongs);
  
  drawSplash();
  delay(2000);
  
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
  }
}

void navigateUp() {
  if (currentScreen == LIBRARY) {
    selectedItem = (selectedItem - 1 + totalSongs) % totalSongs;
    if (selectedItem < scrollOffset) scrollOffset = selectedItem;
    drawScreen();
  } else if (currentScreen == HOME) {
    selectedItem = (selectedItem - 1 + 2) % 2;
    drawScreen();
  }
}

void navigateDown() {
  if (currentScreen == LIBRARY) {
    selectedItem = (selectedItem + 1) % totalSongs;
    if (selectedItem >= scrollOffset + 6) scrollOffset = selectedItem - 5;
    drawScreen();
  } else if (currentScreen == HOME) {
    selectedItem = (selectedItem + 1) % 2;
    drawScreen();
  }
}

void handleSelect() {
  if (currentScreen == HOME) {
    if (selectedItem == 0) {
      currentScreen = LIBRARY;
      selectedItem = 0;
      scrollOffset = 0;
    } else {
      currentScreen = NOW_PLAYING;
    }
    drawScreen();
  } else if (currentScreen == LIBRARY) {
    currentTrack = selectedItem;
    currentScreen = NOW_PLAYING;
    isPlaying = true;
    Serial.printf("🎵 Selected: %s\n", demoSongs[currentTrack]);
    drawScreen();
  } else if (currentScreen == NOW_PLAYING) {
    togglePlayPause();
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
  if (currentScreen == NOW_PLAYING) {
    currentTrack = (currentTrack + 1) % totalSongs;
    Serial.printf("⏭️ Next: %s\n", demoSongs[currentTrack]);
    drawScreen();
  }
}

void previousTrack() {
  if (currentScreen == NOW_PLAYING) {
    currentTrack = (currentTrack - 1 + totalSongs) % totalSongs;
    Serial.printf("⏮️ Previous: %s\n", demoSongs[currentTrack]);
    drawScreen();
  }
}

void volumeUp() {
  if (volume < 100) {
    volume += 10;
    Serial.printf("🔊 Volume: %d%%\n", volume);
    showVolumeOverlay();
  }
}

void volumeDown() {
  if (volume > 0) {
    volume -= 10;
    Serial.printf("🔉 Volume: %d%%\n", volume);
    showVolumeOverlay();
  }
}

void showVolumeOverlay() {
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

void drawScreen() {
  tft.fillScreen(BLACK);
  
  switch (currentScreen) {
    case SPLASH: drawSplash(); break;
    case HOME: drawHome(); break;
    case LIBRARY: drawLibrary(); break;
    case NOW_PLAYING: drawNowPlaying(); break;
  }
}

void drawSplash() {
  tft.setTextSize(4);
  tft.setTextColor(WHITE);
  tft.setCursor(60, 100);
  tft.print("iPod");
  
  tft.setTextSize(1);
  tft.setTextColor(CYAN);
  tft.setCursor(60, 140);
  tft.print("Demo Mode");
  
  tft.setTextColor(YELLOW);
  tft.setCursor(30, 160);
  tft.print("Display + Touch Working!");
  
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
  tft.print("Demo");
  
  const char* menu[] = {"Music Library", "Now Playing"};
  
  for (int i = 0; i < 2; i++) {
    int y = 100 + (i * 50);
    
    if (i == selectedItem) {
      tft.fillRoundRect(10, y - 5, 220, 40, 5, BLUE);
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
  tft.printf("Songs: %d (Demo)", totalSongs);
  
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
  tft.printf("%d demo songs", totalSongs);
  
  tft.drawLine(0, 50, 240, 50, GRAY);
  
  int visibleItems = 6;
  int startIdx = scrollOffset;
  int endIdx = min(startIdx + visibleItems, totalSongs);
  
  for (int i = startIdx; i < endIdx; i++) {
    int y = 60 + ((i - startIdx) * 35);
    
    if (i == selectedItem) {
      tft.fillRoundRect(5, y - 3, 230, 32, 3, BLUE);
    }
    
    tft.setTextSize(1);
    tft.setTextColor(WHITE);
    tft.setCursor(10, y);
    tft.print(demoSongs[i]);
    
    tft.setTextColor(GRAY);
    tft.setCursor(10, y + 12);
    tft.print(demoArtists[i]);
    
    if (i == currentTrack && isPlaying) {
      tft.setTextColor(GREEN);
      tft.setCursor(200, y + 10);
      tft.print("♪");
    }
  }
  
  if (totalSongs > visibleItems) {
    int barHeight = (visibleItems * 210) / totalSongs;
    int barY = 60 + (scrollOffset * 210) / totalSongs;
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
  
  tft.drawRoundRect(60, 60, 120, 120, 10, WHITE);
  tft.fillRoundRect(62, 62, 116, 116, 8, GRAY);
  
  tft.setTextSize(6);
  tft.setTextColor(WHITE);
  tft.setCursor(105, 105);
  tft.print("♪");
  
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setCursor(10, 195);
  tft.print(demoSongs[currentTrack]);
  
  tft.setTextColor(GRAY);
  tft.setCursor(10, 210);
  tft.print(demoArtists[currentTrack]);
  
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
  tft.printf("Track %d/%d", currentTrack + 1, totalSongs);
  
  tft.setCursor(10, 275);
  tft.print("Demo Mode");
  
  tft.drawLine(0, 290, 240, 290, GRAY);
  tft.setCursor(10, 300);
  tft.print("4:Play 6/7:Prev/Next");
}

void printControls() {
  Serial.println("🎮 Touch Controls:");
  Serial.println("   Key 1: Menu    Key 2: Up      Key 3: Vol+");
  Serial.println("   Key 4: Play    Key 6: Prev    Key 7: Next");
  Serial.println("   Key 10: Down   Key 11: Select Key 15: Vol-");
  Serial.println();
  Serial.println("💡 This is Demo Mode - no SD card needed!");
  Serial.println("   Add SD card support later when ready.");
}
