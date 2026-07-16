/*
TTP229 8-Key iPod Interface with Gestures
Perfect for iPod controls - keys 1-8 only
*/

#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

// TTP229 Serial Pins
#define TTP_SDO 1
#define TTP_SCL 2

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

// UI State
enum Screen { HOME, LIBRARY, NOW_PLAYING, SETTINGS };
Screen currentScreen = HOME;
int selectedItem = 0;
int maxItems = 4;
bool isPlaying = false;
int volume = 50;
int currentSong = 0;

// Touch state
uint8_t lastTouch = 0;
unsigned long touchStartTime = 0;
int touchHistory[5] = {0, 0, 0, 0, 0};
int touchHistoryIndex = 0;

// Sample data
const char* menuItems[] = {"Music Library", "Now Playing", "Settings", "About"};
const char* songs[] = {
  "Bohemian Rhapsody", "Stairway to Heaven", "Hotel California", 
  "Imagine", "Sweet Child O' Mine", "Billie Jean"
};
const char* artists[] = {
  "Queen", "Led Zeppelin", "Eagles", 
  "John Lennon", "Guns N' Roses", "Michael Jackson"
};

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("╔════════════════════════════════════════╗");
  Serial.println("║   TTP229 8-Key iPod with Gestures     ║");
  Serial.println("╚════════════════════════════════════════╝");
  Serial.println();
  
  // Setup TTP229
  pinMode(TTP_SDO, INPUT);
  pinMode(TTP_SCL, OUTPUT);
  digitalWrite(TTP_SCL, HIGH);
  
  // Setup display
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(BLACK);
  
  showSplash();
  
  currentScreen = HOME;
  updateMaxItems();
  drawScreen();
  
  Serial.println("✅ 8-Key iPod Ready!");
  Serial.println();
  Serial.println("🎮 Controls:");
  Serial.println("   Key 1: Menu/Back");
  Serial.println("   Key 2: Up");
  Serial.println("   Key 3: Select/Play");
  Serial.println("   Key 4: Down");
  Serial.println("   Key 5: Previous");
  Serial.println("   Key 6: Next");
  Serial.println("   Key 7: Volume Down");
  Serial.println("   Key 8: Volume Up");
  Serial.println();
  Serial.println("📱 Gestures:");
  Serial.println("   Swipe 2→4: Navigate down");
  Serial.println("   Swipe 4→2: Navigate up");
  Serial.println("   Swipe 5→6: Next track");
  Serial.println();
}

void loop() {
  uint8_t touchStatus = readTTP229_8bit();
  
  if (touchStatus != lastTouch) {
    if (touchStatus != 0) {
      touchStartTime = millis();
      addToHistory(touchStatus);
      handleTouch(touchStatus);
    } else {
      // Touch released - check for gestures
      unsigned long duration = millis() - touchStartTime;
      detectGesture(duration);
      clearHistory();
    }
    lastTouch = touchStatus;
    delay(100); // Debounce
  }
  
  delay(10);
}

uint8_t readTTP229_8bit() {
  uint16_t value = 0;
  
  digitalWrite(TTP_SCL, HIGH);
  delayMicroseconds(100);
  
  // Read 16 bits but only use first 8
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
  
  // Invert and get only first 8 bits
  value = ~value;
  return (uint8_t)(value & 0xFF);
}

void addToHistory(uint8_t touch) {
  touchHistory[touchHistoryIndex] = touch;
  touchHistoryIndex = (touchHistoryIndex + 1) % 5;
}

void clearHistory() {
  for (int i = 0; i < 5; i++) {
    touchHistory[i] = 0;
  }
  touchHistoryIndex = 0;
}

void detectGesture(unsigned long duration) {
  // Count unique keys in history
  int uniqueKeys = 0;
  int keys[5] = {0};
  
  for (int i = 0; i < 5; i++) {
    if (touchHistory[i] != 0) {
      // Get first touched key
      for (int k = 0; k < 8; k++) {
        if ((touchHistory[i] >> k) & 0x01) {
          keys[uniqueKeys++] = k + 1;
          break;
        }
      }
    }
  }
  
  if (uniqueKeys >= 2) {
    // Check for swipe gestures
    int firstKey = keys[0];
    int lastKey = keys[uniqueKeys - 1];
    
    // Vertical swipes (2-4)
    if (firstKey == 2 && lastKey == 4) {
      Serial.println("🎯 Gesture: Swipe Down");
      navigateDown();
    } else if (firstKey == 4 && lastKey == 2) {
      Serial.println("🎯 Gesture: Swipe Up");
      navigateUp();
    }
    // Horizontal swipes (5-6)
    else if (firstKey == 5 && lastKey == 6) {
      Serial.println("🎯 Gesture: Swipe Right (Next)");
      nextTrack();
    } else if (firstKey == 6 && lastKey == 5) {
      Serial.println("🎯 Gesture: Swipe Left (Previous)");
      previousTrack();
    }
  }
}

void handleTouch(uint8_t touch) {
  // Check which key (only first one if multiple)
  for (int i = 0; i < 8; i++) {
    if ((touch >> i) & 0x01) {
      int key = i + 1;
      Serial.print("👆 Key ");
      Serial.println(key);
      
      switch (key) {
        case 1: handleMenu(); break;
        case 2: navigateUp(); break;
        case 3: handleSelect(); break;
        case 4: navigateDown(); break;
        case 5: previousTrack(); break;
        case 6: nextTrack(); break;
        case 7: volumeDown(); break;
        case 8: volumeUp(); break;
      }
      break;
    }
  }
}

void handleMenu() {
  if (currentScreen != HOME) {
    currentScreen = HOME;
    selectedItem = 0;
    updateMaxItems();
    drawScreen();
  }
}

void handleSelect() {
  switch (currentScreen) {
    case HOME:
      switch (selectedItem) {
        case 0: currentScreen = LIBRARY; break;
        case 1: currentScreen = NOW_PLAYING; break;
        case 2: currentScreen = SETTINGS; break;
        case 3: showAbout(); return;
      }
      selectedItem = 0;
      updateMaxItems();
      drawScreen();
      break;
      
    case LIBRARY:
      currentSong = selectedItem;
      currentScreen = NOW_PLAYING;
      isPlaying = true;
      drawScreen();
      break;
      
    case NOW_PLAYING:
      togglePlayPause();
      break;
  }
}

void navigateUp() {
  selectedItem--;
  if (selectedItem < 0) selectedItem = maxItems - 1;
  drawScreen();
}

void navigateDown() {
  selectedItem++;
  if (selectedItem >= maxItems) selectedItem = 0;
  drawScreen();
}

void togglePlayPause() {
  isPlaying = !isPlaying;
  Serial.print("🎵 ");
  Serial.println(isPlaying ? "Playing" : "Paused");
  drawScreen();
}

void previousTrack() {
  if (currentScreen == NOW_PLAYING) {
    currentSong = (currentSong - 1 + 6) % 6;
    Serial.println("⏮️ Previous track");
    drawScreen();
  }
}

void nextTrack() {
  if (currentScreen == NOW_PLAYING) {
    currentSong = (currentSong + 1) % 6;
    Serial.println("⏭️ Next track");
    drawScreen();
  }
}

void volumeDown() {
  if (volume > 0) {
    volume -= 10;
    Serial.print("🔉 Volume: ");
    Serial.println(volume);
    showVolumeOverlay();
  }
}

void volumeUp() {
  if (volume < 100) {
    volume += 10;
    Serial.print("🔊 Volume: ");
    Serial.println(volume);
    showVolumeOverlay();
  }
}

void updateMaxItems() {
  switch (currentScreen) {
    case HOME: maxItems = 4; break;
    case LIBRARY: maxItems = 6; break;
    case NOW_PLAYING: maxItems = 1; break;
    case SETTINGS: maxItems = 3; break;
  }
}

// Display functions
void showSplash() {
  tft.fillScreen(BLACK);
  
  tft.setTextSize(4);
  tft.setTextColor(WHITE);
  tft.setCursor(60, 100);
  tft.print("iPod");
  
  tft.setTextSize(1);
  tft.setTextColor(CYAN);
  tft.setCursor(60, 140);
  tft.print("8-Key Touch");
  
  tft.setTextColor(YELLOW);
  tft.setCursor(70, 160);
  tft.print("With Gestures!");
  
  delay(2000);
  tft.fillScreen(BLACK);
}

void drawScreen() {
  tft.fillScreen(BLACK);
  
  switch (currentScreen) {
    case HOME: drawHome(); break;
    case LIBRARY: drawLibrary(); break;
    case NOW_PLAYING: drawNowPlaying(); break;
    case SETTINGS: drawSettings(); break;
  }
  
  drawStatusBar();
}

void drawHome() {
  tft.setTextSize(3);
  tft.setTextColor(WHITE);
  tft.setCursor(20, 20);
  tft.print("iPod");
  
  tft.setTextSize(1);
  tft.setTextColor(PURPLE);
  tft.setCursor(150, 30);
  tft.print("8-Key");
  
  for (int i = 0; i < 4; i++) {
    int y = 70 + (i * 35);
    
    if (i == selectedItem) {
      tft.fillRoundRect(10, y - 5, 220, 30, 5, BLUE);
    }
    
    tft.setTextSize(2);
    tft.setTextColor(WHITE);
    tft.setCursor(20, y);
    tft.print(menuItems[i]);
    
    if (i == selectedItem) {
      tft.setCursor(200, y);
      tft.print(">");
    }
  }
}

void drawLibrary() {
  drawHeader("Music Library");
  
  for (int i = 0; i < 6; i++) {
    int y = 80 + (i * 30);
    
    if (i == selectedItem) {
      tft.fillRoundRect(5, y - 3, 230, 25, 3, BLUE);
    }
    
    tft.setTextSize(1);
    tft.setTextColor(WHITE);
    tft.setCursor(10, y);
    tft.print(songs[i]);
    
    tft.setTextColor(GRAY);
    tft.setCursor(10, y + 12);
    tft.print(artists[i]);
    
    if (i == currentSong && isPlaying) {
      tft.setTextColor(GREEN);
      tft.setCursor(200, y + 6);
      tft.print("♪");
    }
  }
}

void drawNowPlaying() {
  drawHeader("Now Playing");
  
  tft.drawRoundRect(60, 80, 120, 120, 10, WHITE);
  tft.fillRoundRect(62, 82, 116, 116, 8, GRAY);
  
  tft.setTextSize(6);
  tft.setTextColor(WHITE);
  tft.setCursor(105, 125);
  tft.print("♪");
  
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setCursor(20, 210);
  tft.print(songs[currentSong]);
  
  tft.setTextColor(GRAY);
  tft.setCursor(20, 225);
  tft.print(artists[currentSong]);
  
  tft.setTextSize(2);
  tft.setTextColor(isPlaying ? GREEN : RED);
  tft.setCursor(20, 250);
  tft.print(isPlaying ? "Playing" : "Paused");
  
  tft.setTextSize(1);
  tft.setTextColor(CYAN);
  tft.setCursor(150, 255);
  tft.print("Vol:");
  tft.print(volume);
}

void drawSettings() {
  drawHeader("Settings");
  
  const char* settings[] = {"Display", "Audio", "About"};
  
  for (int i = 0; i < 3; i++) {
    int y = 80 + (i * 30);
    
    if (i == selectedItem) {
      tft.fillRoundRect(10, y - 5, 220, 25, 5, BLUE);
    }
    
    tft.setTextSize(1);
    tft.setTextColor(WHITE);
    tft.setCursor(20, y);
    tft.print(settings[i]);
  }
}

void drawHeader(const char* title) {
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.setCursor(20, 20);
  tft.print(title);
  
  tft.setTextSize(1);
  tft.setTextColor(BLUE);
  tft.setCursor(10, 50);
  tft.print("Key 1: Back");
  
  tft.drawLine(0, 65, 240, 65, GRAY);
}

void drawStatusBar() {
  tft.drawLine(0, 300, 240, 300, GRAY);
  
  tft.setTextSize(1);
  tft.setTextColor(GRAY);
  tft.setCursor(10, 308);
  tft.print("2/4:Nav 3:OK 5/6:Track");
}

void showVolumeOverlay() {
  int barWidth = (volume * 200) / 100;
  
  tft.fillRect(20, 140, 200, 40, BLACK);
  tft.drawRect(20, 140, 200, 40, WHITE);
  tft.fillRect(22, 142, barWidth, 36, GREEN);
  
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.setCursor(80, 155);
  tft.print(volume);
  tft.print("%");
  
  delay(1000);
  drawScreen();
}

void showAbout() {
  tft.fillScreen(BLACK);
  
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.setCursor(20, 60);
  tft.print("About iPod");
  
  tft.setTextSize(1);
  tft.setTextColor(GRAY);
  tft.setCursor(20, 100);
  tft.print("ESP32-S3 Music Player");
  tft.setCursor(20, 120);
  tft.print("TTP229 8-Key Touch");
  tft.setCursor(20, 140);
  tft.print("With Gesture Support");
  
  tft.setTextColor(BLUE);
  tft.setCursor(20, 180);
  tft.print("Press Key 1 to return");
  
  delay(3000);
  currentScreen = HOME;
  drawScreen();
}
