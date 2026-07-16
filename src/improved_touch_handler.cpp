/*
Improved Touch Handler - Fixes Intermittent Touch Issues
More reliable touch detection and debouncing
*/

#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

// Improved touch handling
struct TouchState {
  bool isPressed = false;
  bool wasPressed = false;
  uint16_t x = 0, y = 0;
  uint16_t lastX = 0, lastY = 0;
  unsigned long pressTime = 0;
  unsigned long releaseTime = 0;
  int consecutiveReads = 0;
  int missedReads = 0;
};

TouchState touch;

// Touch configuration
#define TOUCH_DEBOUNCE_MS 30        // Minimum time between touches
#define TOUCH_HOLD_MS 500           // Long press threshold
#define MIN_CONSECUTIVE_READS 2     // Minimum reads to confirm touch
#define MAX_MISSED_READS 3          // Maximum missed reads before release
#define TOUCH_MOVEMENT_THRESHOLD 15 // Minimum movement for drag

// UI State
enum Screen { HOME, LIBRARY, NOW_PLAYING };
Screen currentScreen = HOME;
int selectedItem = 0;
bool isPlaying = false;

void setup() {
  Serial.begin(115200);
  Serial.println("Improved Touch Handler Test");
  
  // Initialize display
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
  
  // Load touch calibration if available
  loadTouchCalibration();
  
  // Show initial screen
  drawScreen();
}

void loop() {
  updateTouch();
  handleTouchEvents();
  delay(10); // 100Hz update rate
}

void updateTouch() {
  uint16_t x, y;
  bool currentlyTouched = tft.getTouch(&x, &y);
  
  if (currentlyTouched) {
    touch.consecutiveReads++;
    touch.missedReads = 0;
    
    // Confirm touch after minimum consecutive reads
    if (touch.consecutiveReads >= MIN_CONSECUTIVE_READS) {
      if (!touch.isPressed) {
        // New touch detected
        touch.isPressed = true;
        touch.pressTime = millis();
        touch.x = x;
        touch.y = y;
        touch.lastX = x;
        touch.lastY = y;
        
        Serial.print("Touch START: (");
        Serial.print(x);
        Serial.print(", ");
        Serial.print(y);
        Serial.println(")");
        
        onTouchStart(x, y);
      } else {
        // Update touch position
        touch.x = x;
        touch.y = y;
        
        // Check for movement (drag)
        int deltaX = abs((int)x - (int)touch.lastX);
        int deltaY = abs((int)y - (int)touch.lastY);
        
        if (deltaX > TOUCH_MOVEMENT_THRESHOLD || deltaY > TOUCH_MOVEMENT_THRESHOLD) {
          onTouchMove(x, y, deltaX, deltaY);
          touch.lastX = x;
          touch.lastY = y;
        }
      }
    }
  } else {
    touch.consecutiveReads = 0;
    touch.missedReads++;
    
    // Release touch after maximum missed reads
    if (touch.isPressed && touch.missedReads >= MAX_MISSED_READS) {
      touch.isPressed = false;
      touch.releaseTime = millis();
      
      unsigned long touchDuration = touch.releaseTime - touch.pressTime;
      
      Serial.print("Touch END: Duration ");
      Serial.print(touchDuration);
      Serial.println("ms");
      
      onTouchEnd(touch.x, touch.y, touchDuration);
    }
  }
  
  touch.wasPressed = touch.isPressed;
}

void handleTouchEvents() {
  // This function is called from the touch event handlers
  // No additional processing needed here as events are handled in callbacks
}

void onTouchStart(uint16_t x, uint16_t y) {
  // Visual feedback - show touch point
  drawTouchFeedback(x, y, TFT_GREEN);
  
  // Haptic feedback simulation (could control a vibration motor)
  // digitalWrite(HAPTIC_PIN, HIGH);
  // delay(10);
  // digitalWrite(HAPTIC_PIN, LOW);
}

void onTouchMove(uint16_t x, uint16_t y, int deltaX, int deltaY) {
  // Handle drag gestures
  Serial.print("Touch MOVE: (");
  Serial.print(x);
  Serial.print(", ");
  Serial.print(y);
  Serial.print(") Delta: (");
  Serial.print(deltaX);
  Serial.print(", ");
  Serial.print(deltaY);
  Serial.println(")");
  
  // Could implement scrolling here
}

void onTouchEnd(uint16_t x, uint16_t y, unsigned long duration) {
  // Clear touch feedback
  drawTouchFeedback(x, y, TFT_BLACK);
  
  // Determine touch type
  if (duration < TOUCH_HOLD_MS) {
    // Short tap
    handleTap(x, y);
  } else {
    // Long press
    handleLongPress(x, y);
  }
}

void handleTap(uint16_t x, uint16_t y) {
  Serial.print("TAP at (");
  Serial.print(x);
  Serial.print(", ");
  Serial.print(y);
  Serial.println(")");
  
  // Process tap based on current screen
  switch (currentScreen) {
    case HOME:
      handleHomeTap(x, y);
      break;
    case LIBRARY:
      handleLibraryTap(x, y);
      break;
    case NOW_PLAYING:
      handleNowPlayingTap(x, y);
      break;
  }
}

void handleLongPress(uint16_t x, uint16_t y) {
  Serial.print("LONG PRESS at (");
  Serial.print(x);
  Serial.print(", ");
  Serial.print(y);
  Serial.println(")");
  
  // Show context menu or special action
  showContextMenu(x, y);
}

void handleHomeTap(uint16_t x, uint16_t y) {
  // Calculate which menu item was tapped
  if (y > 60 && y < 240) {
    int item = (y - 60) / 40;
    if (item >= 0 && item < 3) {
      selectedItem = item;
      
      switch (item) {
        case 0:
          currentScreen = LIBRARY;
          break;
        case 1:
          currentScreen = NOW_PLAYING;
          break;
        case 2:
          showSettings();
          return;
      }
      
      drawScreen();
    }
  }
}

void handleLibraryTap(uint16_t x, uint16_t y) {
  // Back button
  if (x < 60 && y < 40) {
    currentScreen = HOME;
    drawScreen();
    return;
  }
  
  // Song selection
  if (y > 80 && y < 200) {
    int song = (y - 80) / 30;
    if (song >= 0 && song < 4) {
      selectedItem = song;
      currentScreen = NOW_PLAYING;
      isPlaying = true;
      drawScreen();
    }
  }
}

void handleNowPlayingTap(uint16_t x, uint16_t y) {
  // Back button
  if (x < 60 && y < 40) {
    currentScreen = HOME;
    drawScreen();
    return;
  }
  
  // Play/Pause button
  if (x > 100 && x < 140 && y > 200 && y < 240) {
    isPlaying = !isPlaying;
    drawNowPlayingControls();
    return;
  }
  
  // Previous button
  if (x > 60 && x < 100 && y > 200 && y < 240) {
    selectedItem = (selectedItem - 1 + 4) % 4;
    drawScreen();
    return;
  }
  
  // Next button
  if (x > 140 && x < 180 && y > 200 && y < 240) {
    selectedItem = (selectedItem + 1) % 4;
    drawScreen();
    return;
  }
}

void drawTouchFeedback(uint16_t x, uint16_t y, uint16_t color) {
  if (color == TFT_BLACK) {
    // Clear feedback - redraw the area
    tft.fillCircle(x, y, 15, TFT_BLACK);
  } else {
    // Show feedback
    tft.drawCircle(x, y, 10, color);
    tft.drawCircle(x, y, 12, color);
  }
}

void showContextMenu(uint16_t x, uint16_t y) {
  // Simple context menu
  int menuX = constrain(x - 50, 10, 140);
  int menuY = constrain(y - 40, 10, 200);
  
  tft.fillRect(menuX, menuY, 100, 80, TFT_DARKGREY);
  tft.drawRect(menuX, menuY, 100, 80, TFT_WHITE);
  
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(menuX + 10, menuY + 10);
  tft.println("Options:");
  tft.setCursor(menuX + 10, menuY + 25);
  tft.println("Calibrate");
  tft.setCursor(menuX + 10, menuY + 40);
  tft.println("Settings");
  tft.setCursor(menuX + 10, menuY + 55);
  tft.println("Cancel");
  
  delay(2000); // Auto-close
  drawScreen(); // Redraw screen
}

void drawScreen() {
  tft.fillScreen(TFT_BLACK);
  
  switch (currentScreen) {
    case HOME:
      drawHomeScreen();
      break;
    case LIBRARY:
      drawLibraryScreen();
      break;
    case NOW_PLAYING:
      drawNowPlayingScreen();
      break;
  }
}

void drawHomeScreen() {
  // Title
  tft.setTextSize(3);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(20, 20);
  tft.print("iPod");
  
  // Menu items
  const char* items[] = {"Music Library", "Now Playing", "Settings"};
  
  for (int i = 0; i < 3; i++) {
    int y = 60 + (i * 40);
    
    if (i == selectedItem) {
      tft.fillRect(10, y - 5, 220, 30, TFT_BLUE);
    }
    
    tft.setTextSize(2);
    tft.setTextColor(TFT_WHITE);
    tft.setCursor(20, y);
    tft.print(items[i]);
  }
  
  // Touch quality indicator
  tft.setTextSize(1);
  tft.setTextColor(TFT_GREEN);
  tft.setCursor(10, 280);
  tft.print("Touch: Enhanced Mode");
}

void drawLibraryScreen() {
  // Back button
  tft.setTextSize(1);
  tft.setTextColor(TFT_CYAN);
  tft.setCursor(10, 10);
  tft.print("< Back");
  
  // Title
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(20, 40);
  tft.print("Library");
  
  // Songs
  const char* songs[] = {"Song 1", "Song 2", "Song 3", "Song 4"};
  
  for (int i = 0; i < 4; i++) {
    int y = 80 + (i * 30);
    
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE);
    tft.setCursor(20, y);
    tft.print(songs[i]);
    
    if (i == selectedItem && isPlaying) {
      tft.setTextColor(TFT_GREEN);
      tft.setCursor(180, y);
      tft.print("♪");
    }
  }
}

void drawNowPlayingScreen() {
  // Back button
  tft.setTextSize(1);
  tft.setTextColor(TFT_CYAN);
  tft.setCursor(10, 10);
  tft.print("< Back");
  
  // Title
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(20, 40);
  tft.print("Now Playing");
  
  // Album art placeholder
  tft.drawRect(70, 80, 100, 100, TFT_WHITE);
  tft.setTextSize(3);
  tft.setTextColor(TFT_GREY);
  tft.setCursor(110, 120);
  tft.print("♪");
  
  // Song info
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(20, 200);
  tft.print("Song ");
  tft.print(selectedItem + 1);
  
  drawNowPlayingControls();
}

void drawNowPlayingControls() {
  // Clear control area
  tft.fillRect(0, 220, 240, 60, TFT_BLACK);
  
  // Control buttons with better touch targets
  int y = 230;
  
  // Previous (larger touch area)
  tft.drawRect(55, y - 5, 50, 30, TFT_DARKGREY);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(70, y);
  tft.print("<<");
  
  // Play/Pause (larger touch area)
  tft.drawRect(95, y - 5, 50, 30, isPlaying ? TFT_GREEN : TFT_BLUE);
  tft.setCursor(isPlaying ? 110 : 112, y);
  tft.print(isPlaying ? "||" : ">");
  
  // Next (larger touch area)
  tft.drawRect(135, y - 5, 50, 30, TFT_DARKGREY);
  tft.setCursor(150, y);
  tft.print(">>");
}

void showSettings() {
  tft.fillScreen(TFT_BLACK);
  
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(20, 40);
  tft.print("Settings");
  
  tft.setTextSize(1);
  tft.setCursor(20, 80);
  tft.print("Touch Calibration");
  tft.setCursor(20, 100);
  tft.print("Display Brightness");
  tft.setCursor(20, 120);
  tft.print("Audio Settings");
  
  tft.setTextColor(TFT_CYAN);
  tft.setCursor(20, 200);
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

bool loadTouchCalibration() {
  // Placeholder for loading calibration
  // In a real implementation, this would load from SPIFFS
  return false;
}