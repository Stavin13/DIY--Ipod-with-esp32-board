/*
ESP32-S3 iPod with TTP229 Gesture Recognition
Swipe gestures, scroll wheel simulation, multi-touch
Advanced touch interface with gesture detection
*/

#include <TFT_eSPI.h>
#include <Wire.h>

TFT_eSPI tft = TFT_eSPI();

// TTP229 I2C Configuration
#define TTP229_ADDR 0x57
#define I2C_SDA 1
#define I2C_SCL 2

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
#define ORANGE 0xFD20
#define PURPLE 0x780F
#define PINK 0xF81F

// Gesture Types
enum GestureType {
  GESTURE_NONE,
  GESTURE_TAP,
  GESTURE_DOUBLE_TAP,
  GESTURE_LONG_PRESS,
  GESTURE_SWIPE_UP,
  GESTURE_SWIPE_DOWN,
  GESTURE_SWIPE_LEFT,
  GESTURE_SWIPE_RIGHT,
  GESTURE_SCROLL_CW,      // Clockwise scroll (like iPod wheel)
  GESTURE_SCROLL_CCW,     // Counter-clockwise scroll
  GESTURE_PINCH_IN,       // Two-finger pinch
  GESTURE_PINCH_OUT,      // Two-finger spread
  GESTURE_TWO_FINGER_TAP
};

// UI State
enum Screen { HOME, LIBRARY, NOW_PLAYING, SETTINGS, GESTURE_DEMO };
Screen currentScreen = HOME;
int selectedItem = 0;
int maxItems = 4;
bool isPlaying = false;
int volume = 50;
int currentSong = 0;

// Touch state
uint16_t touchStatus = 0;
uint16_t lastTouchStatus = 0;
unsigned long touchStartTime = 0;
unsigned long lastTapTime = 0;
int firstTouchKey = -1;
int lastTouchKey = -1;
bool gestureInProgress = false;

// Gesture detection parameters
#define TAP_THRESHOLD 200        // Max time for tap (ms)
#define LONG_PRESS_THRESHOLD 500 // Min time for long press (ms)
#define DOUBLE_TAP_THRESHOLD 300 // Max time between taps (ms)
#define SWIPE_MIN_KEYS 2         // Minimum keys for swipe
#define SCROLL_THRESHOLD 3       // Keys for scroll detection

// Touch history for gesture detection
#define TOUCH_HISTORY_SIZE 10
int touchHistory[TOUCH_HISTORY_SIZE];
int touchHistoryIndex = 0;
unsigned long touchHistoryTime[TOUCH_HISTORY_SIZE];

// Sample data
const char* menuItems[] = {"Music Library", "Now Playing", "Gesture Demo", "Settings"};
const char* songs[] = {
  "Bohemian Rhapsody", "Stairway to Heaven", "Hotel California", 
  "Imagine", "Sweet Child O' Mine", "Billie Jean",
  "Smells Like Teen Spirit", "Sweet Home Alabama"
};
const char* artists[] = {
  "Queen", "Led Zeppelin", "Eagles", 
  "John Lennon", "Guns N' Roses", "Michael Jackson",
  "Nirvana", "Lynyrd Skynyrd"
};

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("╔════════════════════════════════════════╗");
  Serial.println("║   ESP32-S3 iPod Gesture Interface     ║");
  Serial.println("║    TTP229 with Gesture Recognition    ║");
  Serial.println("╚════════════════════════════════════════╝");
  Serial.println();
  
  // Initialize I2C
  Wire.begin(I2C_SDA, I2C_SCL);
  Wire.setClock(100000);
  
  // Initialize display
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(BLACK);
  
  // Test TTP229
  testTTP229();
  
  // Show splash
  showSplash();
  
  // Initialize touch history
  for (int i = 0; i < TOUCH_HISTORY_SIZE; i++) {
    touchHistory[i] = -1;
    touchHistoryTime[i] = 0;
  }
  
  // Start with home screen
  currentScreen = HOME;
  updateMaxItems();
  drawScreen();
  
  Serial.println("🎮 Gesture iPod Interface Ready!");
  Serial.println();
  Serial.println("📱 Supported Gestures:");
  Serial.println("   • Tap: Select item");
  Serial.println("   • Double Tap: Quick action");
  Serial.println("   • Long Press: Context menu");
  Serial.println("   • Swipe Up/Down: Navigate");
  Serial.println("   • Swipe Left/Right: Previous/Next");
  Serial.println("   • Circular Scroll: iPod wheel simulation");
  Serial.println("   • Two-finger tap: Play/Pause");
  Serial.println();
}

void loop() {
  handleTouch();
  updateUI();
  delay(10);
}

void testTTP229() {
  Serial.println("🔍 Testing TTP229 connection...");
  
  Wire.beginTransmission(TTP229_ADDR);
  uint8_t error = Wire.endTransmission();
  
  if (error == 0) {
    Serial.println("✅ TTP229 module found!");
  } else {
    Serial.println("❌ TTP229 not found!");
  }
  Serial.println();
}

uint16_t readTTP229() {
  Wire.requestFrom(TTP229_ADDR, 2);
  
  if (Wire.available() >= 2) {
    uint8_t lowByte = Wire.read();
    uint8_t highByte = Wire.read();
    return (highByte << 8) | lowByte;
  }
  
  return 0;
}

void handleTouch() {
  touchStatus = readTTP229();
  unsigned long currentTime = millis();
  
  // Detect touch start
  if (touchStatus != 0 && lastTouchStatus == 0) {
    touchStartTime = currentTime;
    firstTouchKey = getFirstTouchedKey(touchStatus);
    gestureInProgress = true;
    
    // Add to history
    addToTouchHistory(firstTouchKey, currentTime);
  }
  
  // Detect touch changes during gesture
  if (touchStatus != lastTouchStatus && touchStatus != 0) {
    int currentKey = getFirstTouchedKey(touchStatus);
    if (currentKey != lastTouchKey) {
      addToTouchHistory(currentKey, currentTime);
      lastTouchKey = currentKey;
    }
  }
  
  // Detect touch end
  if (touchStatus == 0 && lastTouchStatus != 0) {
    unsigned long touchDuration = currentTime - touchStartTime;
    
    // Analyze gesture
    GestureType gesture = detectGesture(touchDuration, currentTime);
    
    if (gesture != GESTURE_NONE) {
      handleGesture(gesture);
    }
    
    // Reset
    gestureInProgress = false;
    clearTouchHistory();
  }
  
  lastTouchStatus = touchStatus;
}

int getFirstTouchedKey(uint16_t status) {
  for (int i = 0; i < 16; i++) {
    if ((status >> i) & 0x01) {
      return i + 1; // Return 1-16 (not 0-15)
    }
  }
  return -1;
}

int getTouchCount(uint16_t status) {
  int count = 0;
  for (int i = 0; i < 16; i++) {
    if ((status >> i) & 0x01) {
      count++;
    }
  }
  return count;
}

void addToTouchHistory(int key, unsigned long time) {
  touchHistory[touchHistoryIndex] = key;
  touchHistoryTime[touchHistoryIndex] = time;
  touchHistoryIndex = (touchHistoryIndex + 1) % TOUCH_HISTORY_SIZE;
}

void clearTouchHistory() {
  for (int i = 0; i < TOUCH_HISTORY_SIZE; i++) {
    touchHistory[i] = -1;
    touchHistoryTime[i] = 0;
  }
  touchHistoryIndex = 0;
}

GestureType detectGesture(unsigned long duration, unsigned long endTime) {
  int touchCount = getTouchCount(lastTouchStatus);
  
  // Two-finger gestures
  if (touchCount >= 2) {
    if (duration < TAP_THRESHOLD) {
      return GESTURE_TWO_FINGER_TAP;
    }
    // Could add pinch detection here
  }
  
  // Count unique keys in history
  int uniqueKeys = countUniqueKeysInHistory();
  
  // Long press
  if (duration >= LONG_PRESS_THRESHOLD && uniqueKeys == 1) {
    return GESTURE_LONG_PRESS;
  }
  
  // Tap or double tap
  if (duration < TAP_THRESHOLD && uniqueKeys <= 2) {
    if (endTime - lastTapTime < DOUBLE_TAP_THRESHOLD) {
      lastTapTime = 0; // Reset to prevent triple tap
      return GESTURE_DOUBLE_TAP;
    } else {
      lastTapTime = endTime;
      return GESTURE_TAP;
    }
  }
  
  // Swipe or scroll detection
  if (uniqueKeys >= SWIPE_MIN_KEYS) {
    // Detect scroll (circular motion)
    GestureType scroll = detectScrollGesture();
    if (scroll != GESTURE_NONE) {
      return scroll;
    }
    
    // Detect linear swipe
    return detectSwipeDirection();
  }
  
  return GESTURE_NONE;
}

int countUniqueKeysInHistory() {
  int count = 0;
  bool found[17] = {false}; // 1-16 keys
  
  for (int i = 0; i < TOUCH_HISTORY_SIZE; i++) {
    if (touchHistory[i] > 0 && touchHistory[i] <= 16) {
      if (!found[touchHistory[i]]) {
        found[touchHistory[i]] = true;
        count++;
      }
    }
  }
  
  return count;
}

GestureType detectSwipeDirection() {
  // Get first and last valid keys
  int firstKey = -1, lastKey = -1;
  
  for (int i = 0; i < TOUCH_HISTORY_SIZE; i++) {
    if (touchHistory[i] > 0) {
      if (firstKey == -1) firstKey = touchHistory[i];
      lastKey = touchHistory[i];
    }
  }
  
  if (firstKey == -1 || lastKey == -1) return GESTURE_NONE;
  
  // Convert to row/col (1-16 → 0-3, 0-3)
  int firstRow = (firstKey - 1) / 4;
  int firstCol = (firstKey - 1) % 4;
  int lastRow = (lastKey - 1) / 4;
  int lastCol = (lastKey - 1) % 4;
  
  int deltaRow = lastRow - firstRow;
  int deltaCol = lastCol - firstCol;
  
  // Determine primary direction
  if (abs(deltaRow) > abs(deltaCol)) {
    // Vertical swipe
    return (deltaRow > 0) ? GESTURE_SWIPE_DOWN : GESTURE_SWIPE_UP;
  } else if (abs(deltaCol) > abs(deltaRow)) {
    // Horizontal swipe
    return (deltaCol > 0) ? GESTURE_SWIPE_RIGHT : GESTURE_SWIPE_LEFT;
  }
  
  return GESTURE_NONE;
}

GestureType detectScrollGesture() {
  // Detect circular motion (iPod wheel style)
  // Check if touch path follows circular pattern
  
  int validKeys = 0;
  int keys[TOUCH_HISTORY_SIZE];
  
  for (int i = 0; i < TOUCH_HISTORY_SIZE; i++) {
    if (touchHistory[i] > 0) {
      keys[validKeys++] = touchHistory[i];
    }
  }
  
  if (validKeys < SCROLL_THRESHOLD) return GESTURE_NONE;
  
  // Define circular paths (clockwise and counter-clockwise)
  // Outer ring: 1→2→3→4→8→12→16→15→14→13→9→5→1
  // Middle positions for smoother detection
  
  int clockwisePaths[][4] = {
    {1, 2, 3, 4},    // Top row right
    {4, 8, 12, 16},  // Right column down
    {16, 15, 14, 13},// Bottom row left
    {13, 9, 5, 1},   // Left column up
    {2, 6, 10, 14},  // Middle vertical
    {5, 6, 7, 8}     // Middle horizontal
  };
  
  // Check for clockwise pattern
  for (int p = 0; p < 6; p++) {
    bool matchCW = true;
    bool matchCCW = true;
    
    for (int i = 0; i < 3 && i < validKeys - 1; i++) {
      // Check clockwise
      if (keys[i] != clockwisePaths[p][i]) matchCW = false;
      // Check counter-clockwise (reverse)
      if (keys[i] != clockwisePaths[p][3 - i]) matchCCW = false;
    }
    
    if (matchCW) return GESTURE_SCROLL_CW;
    if (matchCCW) return GESTURE_SCROLL_CCW;
  }
  
  return GESTURE_NONE;
}

void handleGesture(GestureType gesture) {
  Serial.print("🎯 Gesture detected: ");
  
  switch (gesture) {
    case GESTURE_TAP:
      Serial.println("TAP");
      handleTap();
      break;
      
    case GESTURE_DOUBLE_TAP:
      Serial.println("DOUBLE TAP");
      handleDoubleTap();
      break;
      
    case GESTURE_LONG_PRESS:
      Serial.println("LONG PRESS");
      handleLongPress();
      break;
      
    case GESTURE_SWIPE_UP:
      Serial.println("SWIPE UP");
      handleSwipeUp();
      break;
      
    case GESTURE_SWIPE_DOWN:
      Serial.println("SWIPE DOWN");
      handleSwipeDown();
      break;
      
    case GESTURE_SWIPE_LEFT:
      Serial.println("SWIPE LEFT");
      handleSwipeLeft();
      break;
      
    case GESTURE_SWIPE_RIGHT:
      Serial.println("SWIPE RIGHT");
      handleSwipeRight();
      break;
      
    case GESTURE_SCROLL_CW:
      Serial.println("SCROLL CLOCKWISE");
      handleScrollCW();
      break;
      
    case GESTURE_SCROLL_CCW:
      Serial.println("SCROLL COUNTER-CLOCKWISE");
      handleScrollCCW();
      break;
      
    case GESTURE_TWO_FINGER_TAP:
      Serial.println("TWO-FINGER TAP");
      handleTwoFingerTap();
      break;
      
    default:
      Serial.println("UNKNOWN");
      break;
  }
  
  // Visual feedback
  showGestureFeedback(gesture);
}

// Gesture handlers
void handleTap() {
  // Select current item
  if (currentScreen == HOME) {
    switch (selectedItem) {
      case 0: currentScreen = LIBRARY; break;
      case 1: currentScreen = NOW_PLAYING; break;
      case 2: currentScreen = GESTURE_DEMO; break;
      case 3: currentScreen = SETTINGS; break;
    }
    selectedItem = 0;
    updateMaxItems();
    drawScreen();
  } else if (currentScreen == LIBRARY) {
    currentSong = selectedItem;
    currentScreen = NOW_PLAYING;
    isPlaying = true;
    drawScreen();
  }
}

void handleDoubleTap() {
  // Quick play/pause
  togglePlayPause();
}

void handleLongPress() {
  // Go back to home
  if (currentScreen != HOME) {
    currentScreen = HOME;
    selectedItem = 0;
    updateMaxItems();
    drawScreen();
  }
}

void handleSwipeUp() {
  // Navigate up
  navigateUp();
}

void handleSwipeDown() {
  // Navigate down
  navigateDown();
}

void handleSwipeLeft() {
  // Previous track or go back
  if (currentScreen == NOW_PLAYING) {
    previousTrack();
  } else {
    handleLongPress(); // Go back
  }
}

void handleSwipeRight() {
  // Next track
  if (currentScreen == NOW_PLAYING) {
    nextTrack();
  }
}

void handleScrollCW() {
  // Scroll down (like iPod wheel)
  navigateDown();
}

void handleScrollCCW() {
  // Scroll up (like iPod wheel)
  navigateUp();
}

void handleTwoFingerTap() {
  // Play/pause
  togglePlayPause();
}

// Navigation functions
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
  if (currentScreen == NOW_PLAYING) drawScreen();
}

void previousTrack() {
  currentSong = (currentSong - 1 + 8) % 8;
  Serial.print("⏮️ Previous: ");
  Serial.println(songs[currentSong]);
  drawScreen();
}

void nextTrack() {
  currentSong = (currentSong + 1) % 8;
  Serial.print("⏭️ Next: ");
  Serial.println(songs[currentSong]);
  drawScreen();
}

void updateMaxItems() {
  switch (currentScreen) {
    case HOME: maxItems = 4; break;
    case LIBRARY: maxItems = 8; break;
    case NOW_PLAYING: maxItems = 1; break;
    case SETTINGS: maxItems = 4; break;
    case GESTURE_DEMO: maxItems = 1; break;
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
  tft.setCursor(50, 140);
  tft.print("Gesture Interface");
  
  tft.setTextColor(PINK);
  tft.setCursor(60, 160);
  tft.print("Swipe & Scroll");
  
  // Gesture icons
  tft.setTextSize(2);
  tft.setTextColor(YELLOW);
  tft.setCursor(40, 200);
  tft.print("👆 ↕️ ↔️ 🔄");
  
  delay(2500);
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
    case GESTURE_DEMO:
      drawGestureDemo();
      break;
  }
  
  drawStatusBar();
}

void drawHome() {
  tft.setTextSize(3);
  tft.setTextColor(WHITE);
  tft.setCursor(20, 20);
  tft.print("iPod");
  
  tft.setTextSize(1);
  tft.setTextColor(PINK);
  tft.setCursor(140, 30);
  tft.print("Gesture");
  
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
  
  int startIdx = max(0, selectedItem - 3);
  int endIdx = min(8, startIdx + 6);
  
  for (int i = startIdx; i < endIdx; i++) {
    int y = 80 + ((i - startIdx) * 30);
    
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
  tft.fillRoundRect(62, 82, 116, 116, 8, DARK_GRAY);
  
  tft.setTextSize(6);
  tft.setTextColor(GRAY);
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
}

void drawSettings() {
  drawHeader("Settings");
  
  const char* settings[] = {"Display", "Audio", "Gestures", "About"};
  
  for (int i = 0; i < 4; i++) {
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

void drawGestureDemo() {
  drawHeader("Gesture Demo");
  
  tft.setTextSize(1);
  tft.setTextColor(CYAN);
  tft.setCursor(20, 80);
  tft.print("Try these gestures:");
  
  tft.setTextColor(WHITE);
  tft.setCursor(20, 100);
  tft.print("Tap: Select");
  tft.setCursor(20, 115);
  tft.print("Double Tap: Play/Pause");
  tft.setCursor(20, 130);
  tft.print("Long Press: Back");
  tft.setCursor(20, 145);
  tft.print("Swipe Up/Down: Navigate");
  tft.setCursor(20, 160);
  tft.print("Swipe Left/Right: Track");
  tft.setCursor(20, 175);
  tft.print("Circular: Scroll");
  tft.setCursor(20, 190);
  tft.print("Two Fingers: Play/Pause");
  
  tft.setTextColor(YELLOW);
  tft.setCursor(20, 220);
  tft.print("Touch the keypad!");
}

void drawHeader(const char* title) {
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.setCursor(20, 20);
  tft.print(title);
  
  tft.setTextSize(1);
  tft.setTextColor(BLUE);
  tft.setCursor(10, 50);
  tft.print("Long press: Back");
  
  tft.drawLine(0, 65, 240, 65, GRAY);
}

void drawStatusBar() {
  tft.drawLine(0, 300, 240, 300, GRAY);
  
  tft.setTextSize(1);
  tft.setTextColor(GRAY);
  tft.setCursor(10, 308);
  tft.print("Gesture control");
  
  if (gestureInProgress) {
    tft.setTextColor(YELLOW);
    tft.setCursor(180, 308);
    tft.print("Touch!");
  }
}

void showGestureFeedback(GestureType gesture) {
  // Brief visual feedback
  const char* gestureNames[] = {
    "", "TAP", "2TAP", "HOLD", "↑", "↓", "←", "→", "⟳", "⟲", "⊙", "⊕", "✌"
  };
  
  if (gesture > 0 && gesture < 13) {
    tft.fillRect(80, 5, 80, 20, BLACK);
    tft.setTextSize(1);
    tft.setTextColor(YELLOW);
    tft.setCursor(90, 10);
    tft.print(gestureNames[gesture]);
    delay(500);
    tft.fillRect(80, 5, 80, 20, BLACK);
  }
}

void updateUI() {
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate < 1000) return;
  lastUpdate = millis();
}