/*
ESP32-S3 iPod UI - Integrated with Reliable Touch
Complete iPod interface with improved touch handling
*/

#include <TFT_eSPI.h>
#include <SPIFFS.h>
#include <FS.h>

TFT_eSPI tft = TFT_eSPI();

// Screen dimensions
#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 320

// Colors
#define BLACK 0x0000
#define WHITE 0xFFFF
#define BLUE 0x001F
#define GRAY 0x8410
#define LIGHT_GRAY 0xC618
#define DARK_GRAY 0x4208
#define GREEN 0x07E0
#define RED 0xF800
#define CYAN 0x07FF
#define YELLOW 0xFFE0

// UI state
enum Screen { HOME, LIBRARY, NOW_PLAYING, SETTINGS, EQUALIZER };
Screen currentScreen = HOME;
int selectedItem = 0;
bool isPlaying = false;
int volume = 50;
int progress = 25;
int currentSong = 0;

// Touch handling variables
struct TouchState {
  bool isTouched = false;
  uint16_t x = 0, y = 0;
  unsigned long lastTouchTime = 0;
  unsigned long touchStartTime = 0;
  uint16_t startX = 0, startY = 0;
  bool gestureActive = false;
  int consecutiveReads = 0;
  int missedReads = 0;
};

TouchState touch;

// Touch configuration
#define TOUCH_DEBOUNCE_MS 50
#define MIN_CONSECUTIVE_READS 3
#define MAX_MISSED_READS 5
#define LONG_PRESS_TIME 800
#define SWIPE_THRESHOLD 40

// Sample data
const char* menuItems[] = {"Music Library", "Now Playing", "Settings", "Equalizer", "About"};
const char* songs[] = {
  "Bohemian Rhapsody", "Stairway to Heaven", "Hotel California", 
  "Imagine", "Sweet Child O' Mine", "Billie Jean"
};
const char* artists[] = {
  "Queen", "Led Zeppelin", "Eagles", 
  "John Lennon", "Guns N' Roses", "Michael Jackson"
};
const char* albums[] = {
  "A Night at the Opera", "Led Zeppelin IV", "Hotel California",
  "Imagine", "Appetite for Destruction", "Thriller"
};

// Calibration data
uint16_t calData[5];
bool calibrationLoaded = false;

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("╔════════════════════════════════════════╗");
  Serial.println("║     ESP32-S3 iPod UI - Integrated     ║");
  Serial.println("║    With Reliable Touch Handling       ║");
  Serial.println("╚════════════════════════════════════════╝");
  
  // Initialize SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("⚠️  SPIFFS initialization failed!");
  } else {
    Serial.println("✅ SPIFFS initialized successfully");
  }
  
  // Initialize display
  Serial.println("🖥️  Initializing display...");
  tft.init();
  tft.setRotation(0); // Portrait
  tft.fillScreen(BLACK);
  
  // Load or perform calibration
  if (loadCalibration()) {
    Serial.println("✅ Touch calibration loaded from memory");
    calibrationLoaded = true;
  } else {
    Serial.println("🎯 Starting touch calibration...");
    performCalibration();
  }
  
  // Show splash screen
  showSplash();
  
  // Start with home screen
  currentScreen = HOME;
  drawScreen();
  
  Serial.println("🚀 iPod UI ready! Touch to interact.");
}

void loop() {
  handleReliableTouch();
  updateUI();
  delay(10); // 100Hz update rate
}

void handleReliableTouch() {
  uint16_t x, y;
  bool currentlyTouched = tft.getTouch(&x, &y);
  
  if (currentlyTouched) {
    // Validate touch coordinates
    if (isValidTouch(x, y)) {
      touch.consecutiveReads++;
      touch.missedReads = 0;
      
      if (touch.consecutiveReads >= MIN_CONSECUTIVE_READS) {
        if (!touch.isTouched) {
          // New touch started
          touch.isTouched = true;
          touch.x = x;
          touch.y = y;
          touch.touchStartTime = millis();
          touch.startX = x;
          touch.startY = y;
          touch.gestureActive = false;
          
          Serial.print("👆 Touch started: (");
          Serial.print(x);
          Serial.print(", ");
          Serial.print(y);
          Serial.println(")");
          
          onTouchStart(x, y);
        } else {
          // Touch continues - update position
          touch.x = x;
          touch.y = y;
          
          // Check for gestures
          checkForGestures(x, y);
        }
        
        touch.lastTouchTime = millis();
      }
    } else {
      touch.missedReads++;
      if (touch.missedReads > MAX_MISSED_READS) {
        touch.consecutiveReads = 0;
      }
    }
  } else {
    // No touch detected
    touch.missedReads++;
    
    if (touch.missedReads > MAX_MISSED_READS) {
      if (touch.isTouched) {
        // Touch ended
        touch.isTouched = false;
        touch.consecutiveReads = 0;
        
        unsigned long touchDuration = millis() - touch.touchStartTime;
        
        Serial.print("👆 Touch ended after ");
        Serial.print(touchDuration);
        Serial.println("ms");
        
        if (!touch.gestureActive) {
          if (touchDuration > LONG_PRESS_TIME) {
            onLongPress(touch.x, touch.y);
          } else {
            onTouchEnd(touch.x, touch.y);
          }
        }
      }
    }
  }
}

bool isValidTouch(uint16_t x, uint16_t y) {
  // Filter out edge touches (often unreliable)
  if (x < 5 || x > 235 || y < 5 || y > 315) return false;
  
  // Filter out rapid position changes (noise)
  if (touch.isTouched) {
    int deltaX = abs((int)x - (int)touch.x);
    int deltaY = abs((int)y - (int)touch.y);
    
    if (deltaX > 50 || deltaY > 50) return false;
  }
  
  return true;
}

void checkForGestures(uint16_t x, uint16_t y) {
  if (touch.gestureActive) return;
  
  int deltaX = (int)x - (int)touch.startX;
  int deltaY = (int)y - (int)touch.startY;
  
  if (abs(deltaX) > SWIPE_THRESHOLD || abs(deltaY) > SWIPE_THRESHOLD) {
    touch.gestureActive = true;
    
    if (abs(deltaX) > abs(deltaY)) {
      // Horizontal swipe
      if (deltaX > 0) {
        onSwipeRight();
      } else {
        onSwipeLeft();
      }
    } else {
      // Vertical swipe
      if (deltaY > 0) {
        onSwipeDown();
      } else {
        onSwipeUp();
      }
    }
  }
}

void onTouchStart(uint16_t x, uint16_t y) {
  // Visual feedback
  drawTouchIndicator(x, y, GREEN);
}

void onTouchEnd(uint16_t x, uint16_t y) {
  // Process touch based on current screen
  if (millis() - touch.lastTouchTime < TOUCH_DEBOUNCE_MS) return;
  
  processTouch(x, y);
  
  // Clear touch indicator
  drawTouchIndicator(x, y, BLACK);
}

void onLongPress(uint16_t x, uint16_t y) {
  Serial.println("👆 Long press detected!");
  
  // Show context menu or special action
  showContextMenu(x, y);
}

void onSwipeLeft() {
  Serial.println("👈 Swipe LEFT");
  
  switch (currentScreen) {
    case NOW_PLAYING:
      nextSong();
      break;
    case LIBRARY:
      if (selectedItem < 5) selectedItem++;
      drawScreen();
      break;
  }
}

void onSwipeRight() {
  Serial.println("👉 Swipe RIGHT");
  
  switch (currentScreen) {
    case NOW_PLAYING:
      previousSong();
      break;
    case LIBRARY:
      if (selectedItem > 0) selectedItem--;
      drawScreen();
      break;
  }
}

void onSwipeUp() {
  Serial.println("👆 Swipe UP");
  
  switch (currentScreen) {
    case NOW_PLAYING:
      volume = min(100, volume + 10);
      drawNowPlaying();
      break;
    case HOME:
      if (selectedItem > 0) selectedItem--;
      drawScreen();
      break;
  }
}

void onSwipeDown() {
  Serial.println("👇 Swipe DOWN");
  
  switch (currentScreen) {
    case NOW_PLAYING:
      volume = max(0, volume - 10);
      drawNowPlaying();
      break;
    case HOME:
      if (selectedItem < 4) selectedItem++;
      drawScreen();
      break;
  }
}

void drawTouchIndicator(uint16_t x, uint16_t y, uint16_t color) {
  if (color == BLACK) {
    // Clear indicator - redraw the area
    drawScreen();
  } else {
    // Draw touch indicator
    tft.drawCircle(x, y, 8, color);
    tft.drawCircle(x, y, 12, WHITE);
  }
}

void processTouch(uint16_t x, uint16_t y) {
  switch (currentScreen) {
    case HOME:
      handleHomeTouch(x, y);
      break;
    case LIBRARY:
      handleLibraryTouch(x, y);
      break;
    case NOW_PLAYING:
      handleNowPlayingTouch(x, y);
      break;
    case SETTINGS:
      handleSettingsTouch(x, y);
      break;
    case EQUALIZER:
      handleEqualizerTouch(x, y);
      break;
  }
}

void handleHomeTouch(uint16_t x, uint16_t y) {
  // Calculate which menu item was touched
  int item = (y - 60) / 40;
  
  if (item >= 0 && item < 5) {
    selectedItem = item;
    
    switch (item) {
      case 0: currentScreen = LIBRARY; break;
      case 1: currentScreen = NOW_PLAYING; break;
      case 2: currentScreen = SETTINGS; break;
      case 3: currentScreen = EQUALIZER; break;
      case 4: showAbout(); return;
    }
    
    drawScreen();
  }
}

void handleLibraryTouch(uint16_t x, uint16_t y) {
  // Back button (top-left)
  if (x < 60 && y < 40) {
    currentScreen = HOME;
    drawScreen();
    return;
  }
  
  // Song selection
  int song = (y - 80) / 40;
  if (song >= 0 && song < 6) {
    currentSong = song;
    currentScreen = NOW_PLAYING;
    drawScreen();
  }
}

void handleNowPlayingTouch(uint16_t x, uint16_t y) {
  // Back button
  if (x < 60 && y < 40) {
    currentScreen = HOME;
    drawScreen();
    return;
  }
  
  // Control buttons area
  if (y > 200 && y < 250) {
    if (x > 40 && x < 80) {
      // Previous button
      previousSong();
    } else if (x > 90 && x < 150) {
      // Play/Pause button
      togglePlayPause();
    } else if (x > 160 && x < 200) {
      // Next button
      nextSong();
    }
    return;
  }
  
  // Volume control
  if (y > 260 && y < 280) {
    volume = map(x, 20, 220, 0, 100);
    drawNowPlaying();
  }
  
  // Progress bar
  if (y > 180 && y < 200) {
    progress = map(x, 20, 220, 0, 100);
    drawNowPlaying();
  }
}

void handleSettingsTouch(uint16_t x, uint16_t y) {
  // Back button
  if (x < 60 && y < 40) {
    currentScreen = HOME;
    drawScreen();
    return;
  }
  
  // Settings items
  int item = (y - 80) / 35;
  if (item >= 0 && item < 4) {
    switch (item) {
      case 0: adjustBrightness(); break;
      case 1: performCalibration(); drawScreen(); break;
      case 2: showAudioSettings(); break;
      case 3: showAbout(); break;
    }
  }
}

void handleEqualizerTouch(uint16_t x, uint16_t y) {
  // Back button
  if (x < 60 && y < 40) {
    currentScreen = HOME;
    drawScreen();
  }
  
  // EQ sliders would be implemented here
}

void togglePlayPause() {
  isPlaying = !isPlaying;
  Serial.print("🎵 ");
  Serial.println(isPlaying ? "Playing" : "Paused");
  drawNowPlaying();
}

void nextSong() {
  currentSong = (currentSong + 1) % 6;
  Serial.print("⏭️  Next: ");
  Serial.println(songs[currentSong]);
  if (currentScreen == NOW_PLAYING) drawNowPlaying();
}

void previousSong() {
  currentSong = (currentSong - 1 + 6) % 6;
  Serial.print("⏮️  Previous: ");
  Serial.println(songs[currentSong]);
  if (currentScreen == NOW_PLAYING) drawNowPlaying();
}

void showSplash() {
  tft.fillScreen(BLACK);
  
  // iPod logo with animation
  for (int i = 0; i < 3; i++) {
    tft.setTextSize(4);
    tft.setTextColor(WHITE);
    tft.setCursor(60, 120);
    tft.print("iPod");
    
    tft.setTextSize(1);
    tft.setTextColor(GRAY);
    tft.setCursor(80, 160);
    tft.print("ESP32-S3 Edition");
    
    delay(300);
    
    tft.fillRect(60, 120, 120, 50, BLACK);
    delay(200);
  }
  
  // Final display
  tft.setTextSize(4);
  tft.setTextColor(WHITE);
  tft.setCursor(60, 120);
  tft.print("iPod");
  
  tft.setTextSize(1);
  tft.setTextColor(CYAN);
  tft.setCursor(70, 160);
  tft.print("Touch Interface Ready");
  
  delay(2000);
  tft.fillScreen(BLACK);
}

void drawScreen() {
  tft.fillScreen(BLACK);
  
  switch (currentScreen) {
    case HOME:
      drawHome();
      break;
    case LIBRARY:
      drawLibrary();
      break;
    case NOW_PLAYING:
      drawNowPlaying();
      break;
    case SETTINGS:
      drawSettings();
      break;
    case EQUALIZER:
      drawEqualizer();
      break;
  }
}

void drawHome() {
  // Title with gradient effect
  tft.setTextSize(3);
  tft.setTextColor(WHITE);
  tft.setCursor(20, 20);
  tft.print("iPod");
  
  tft.setTextSize(1);
  tft.setTextColor(CYAN);
  tft.setCursor(120, 35);
  tft.print("ESP32-S3");
  
  // Menu items with improved styling
  for (int i = 0; i < 5; i++) {
    int y = 60 + (i * 40);
    
    // Highlight selected item
    if (i == selectedItem) {
      tft.fillRoundRect(10, y - 5, 220, 30, 5, BLUE);
    }
    
    tft.setTextSize(2);
    tft.setTextColor(WHITE);
    tft.setCursor(20, y);
    tft.print(menuItems[i]);
    
    // Arrow for selected item
    if (i == selectedItem) {
      tft.setCursor(200, y);
      tft.print(">");
    }
  }
  
  // Status bar
  drawStatusBar();
}

void drawLibrary() {
  // Header
  drawHeader("Music Library");
  
  // Song list with album art placeholders
  for (int i = 0; i < 6; i++) {
    int y = 80 + (i * 40);
    
    // Highlight currently playing song
    if (i == currentSong && isPlaying) {
      tft.fillRoundRect(5, y - 3, 230, 35, 3, DARK_GRAY);
    }
    
    // Album art placeholder
    tft.drawRect(10, y, 25, 25, GRAY);
    tft.fillRect(12, y + 2, 21, 21, DARK_GRAY);
    
    // Song info
    tft.setTextSize(1);
    tft.setTextColor(WHITE);
    tft.setCursor(45, y + 2);
    tft.print(songs[i]);
    
    tft.setTextColor(GRAY);
    tft.setCursor(45, y + 14);
    tft.print(artists[i]);
    
    // Playing indicator
    if (i == currentSong && isPlaying) {
      tft.setTextColor(GREEN);
      tft.setCursor(210, y + 8);
      tft.print("♪");
    }
  }
  
  drawStatusBar();
}

void drawNowPlaying() {
  // Header
  drawHeader("Now Playing");
  
  // Album art
  tft.drawRoundRect(60, 80, 120, 120, 10, WHITE);
  tft.fillRoundRect(62, 82, 116, 116, 8, DARK_GRAY);
  
  // Large music note
  tft.setTextSize(6);
  tft.setTextColor(GRAY);
  tft.setCursor(105, 125);
  tft.print("♪");
  
  // Song info
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setCursor(20, 210);
  tft.print(songs[currentSong]);
  
  tft.setTextColor(GRAY);
  tft.setCursor(20, 225);
  tft.print(artists[currentSong]);
  
  // Progress bar
  tft.setTextColor(GRAY);
  tft.setCursor(20, 245);
  tft.print("1:23");
  tft.setCursor(190, 245);
  tft.print("3:45");
  
  tft.drawRect(50, 247, 140, 6, WHITE);
  int progWidth = map(progress, 0, 100, 0, 138);
  tft.fillRect(51, 248, progWidth, 4, GREEN);
  
  // Control buttons
  drawControls();
  
  // Volume control
  tft.setTextColor(GRAY);
  tft.setCursor(20, 290);
  tft.print("Volume: ");
  tft.print(volume);
  tft.print("%");
  
  tft.drawRect(20, 305, 200, 8, WHITE);
  int volWidth = map(volume, 0, 100, 0, 198);
  tft.fillRect(21, 306, volWidth, 6, GREEN);
  
  drawStatusBar();
}

void drawControls() {
  int y = 265;
  
  // Previous button
  tft.drawCircle(60, y, 15, WHITE);
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setCursor(53, y - 3);
  tft.print("<<");
  
  // Play/Pause button
  tft.drawCircle(120, y, 20, WHITE);
  tft.setTextSize(2);
  tft.setCursor(isPlaying ? 113 : 111, y - 6);
  tft.print(isPlaying ? "||" : ">");
  
  // Next button
  tft.drawCircle(180, y, 15, WHITE);
  tft.setTextSize(1);
  tft.setCursor(173, y - 3);
  tft.print(">>");
}

void drawSettings() {
  drawHeader("Settings");
  
  const char* settings[] = {"Display Brightness", "Touch Calibration", "Audio Settings", "About"};
  
  for (int i = 0; i < 4; i++) {
    int y = 80 + (i * 35);
    
    tft.setTextSize(1);
    tft.setTextColor(WHITE);
    tft.setCursor(20, y);
    tft.print(settings[i]);
    
    tft.setTextColor(GRAY);
    tft.setCursor(200, y);
    tft.print(">");
  }
  
  drawStatusBar();
}

void drawEqualizer() {
  drawHeader("Equalizer");
  
  // EQ visualization placeholder
  tft.setTextSize(1);
  tft.setTextColor(GRAY);
  tft.setCursor(20, 100);
  tft.print("Equalizer controls");
  tft.setCursor(20, 120);
  tft.print("coming soon...");
  
  drawStatusBar();
}

void drawHeader(const char* title) {
  // Back button
  tft.setTextSize(1);
  tft.setTextColor(BLUE);
  tft.setCursor(10, 10);
  tft.print("< Back");
  
  // Title
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.setCursor(20, 40);
  tft.print(title);
  
  // Separator line
  tft.drawLine(0, 65, 240, 65, GRAY);
}

void drawStatusBar() {
  // Bottom status bar
  tft.drawLine(0, 300, 240, 300, GRAY);
  
  tft.setTextSize(1);
  tft.setTextColor(GRAY);
  tft.setCursor(10, 308);
  tft.print("Touch: ");
  tft.print(touch.isTouched ? "Active" : "Ready");
  
  tft.setCursor(150, 308);
  tft.print("Songs: 6");
}

void updateUI() {
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate < 1000) return;
  
  // Update any dynamic elements
  if (currentScreen == NOW_PLAYING && isPlaying) {
    progress = (progress + 1) % 100;
    
    // Update progress bar only
    tft.drawRect(50, 247, 140, 6, WHITE);
    int progWidth = map(progress, 0, 100, 0, 138);
    tft.fillRect(51, 248, progWidth, 4, GREEN);
  }
  
  lastUpdate = millis();
}

// Calibration and utility functions
void performCalibration() {
  tft.fillScreen(BLACK);
  
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.setCursor(20, 50);
  tft.println("Touch");
  tft.setCursor(20, 80);
  tft.println("Calibration");
  
  tft.setTextSize(1);
  tft.setCursor(10, 120);
  tft.println("Touch each target accurately");
  tft.setCursor(10, 135);
  tft.println("Use a stylus for best results");
  
  delay(3000);
  
  tft.calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);
  saveCalibration();
  calibrationLoaded = true;
  
  Serial.println("✅ Touch calibration complete!");
}

bool loadCalibration() {
  fs::File f = SPIFFS.open("/TouchCalData", "r");
  if (f) {
    f.read((uint8_t*)calData, sizeof(calData));
    f.close();
    tft.setTouch(calData);
    return true;
  }
  return false;
}

void saveCalibration() {
  fs::File f = SPIFFS.open("/TouchCalData", "w");
  if (f) {
    f.write((uint8_t*)calData, sizeof(calData));
    f.close();
    Serial.println("💾 Calibration saved to SPIFFS");
  }
}

void showContextMenu(uint16_t x, uint16_t y) {
  // Context menu implementation
  tft.fillRoundRect(x - 50, y - 30, 100, 60, 5, DARK_GRAY);
  tft.drawRoundRect(x - 50, y - 30, 100, 60, 5, WHITE);
  
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setCursor(x - 40, y - 20);
  tft.print("Recalibrate");
  tft.setCursor(x - 40, y - 5);
  tft.print("Settings");
  tft.setCursor(x - 40, y + 10);
  tft.print("About");
  
  delay(2000);
  drawScreen();
}

void adjustBrightness() {
  // Brightness adjustment placeholder
  Serial.println("🔆 Brightness adjustment");
}

void showAudioSettings() {
  // Audio settings placeholder
  Serial.println("🔊 Audio settings");
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
  tft.print("Version 2.0");
  tft.setCursor(20, 140);
  tft.print("TFT_eSPI Display");
  tft.setCursor(20, 160);
  tft.print("Reliable Touch Interface");
  tft.setCursor(20, 180);
  tft.print("Gesture Support");
  
  tft.setTextColor(BLUE);
  tft.setCursor(20, 220);
  tft.print("Touch anywhere to return");
  
  // Wait for touch
  while (true) {
    uint16_t x, y;
    if (tft.getTouch(&x, &y)) {
      delay(200); // Debounce
      currentScreen = HOME;
      drawScreen();
      break;
    }
    delay(50);
  }
}