/*
TTP229 16-Key iPod Interface with Advanced Gestures
Full 16-channel support with iPod Classic-style controls
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
#define ORANGE 0xFD20

// Gesture detection
#define MAX_TOUCH_HISTORY 20
#define TAP_THRESHOLD 200
#define LONG_PRESS_THRESHOLD 500
#define SWIPE_MIN_KEYS 3
#define CIRCULAR_THRESHOLD 5

enum GestureType {
  GESTURE_NONE,
  GESTURE_TAP,
  GESTURE_LONG_PRESS,
  GESTURE_SWIPE_UP,
  GESTURE_SWIPE_DOWN,
  GESTURE_SWIPE_LEFT,
  GESTURE_SWIPE_RIGHT,
  GESTURE_CIRCULAR_CW,
  GESTURE_CIRCULAR_CCW,
  GESTURE_DOUBLE_TAP,
  GESTURE_PINCH_IN,
  GESTURE_PINCH_OUT
};

// UI State
enum Screen { HOME, LIBRARY, NOW_PLAYING, PLAYLISTS, SETTINGS, ARTISTS, ALBUMS };
Screen currentScreen = HOME;
int selectedItem = 0;
int maxItems = 5;
int scrollOffset = 0;
bool isPlaying = false;
int volume = 50;
int currentSong = 0;
int brightness = 80;

// Touch state
uint16_t lastTouch = 0;
unsigned long touchStartTime = 0;
unsigned long lastTapTime = 0;
int touchHistory[MAX_TOUCH_HISTORY];
int touchHistoryIndex = 0;
int touchCount = 0;
bool longPressTriggered = false;
int initialTouchKey = 0;

// Sample data
const char* menuItems[] = {"Music Library", "Now Playing", "Playlists", "Artists", "Settings"};
const char* songs[] = {
  "Bohemian Rhapsody", "Stairway to Heaven", "Hotel California", 
  "Imagine", "Sweet Child O' Mine", "Billie Jean", "Smells Like Teen Spirit",
  "Hey Jude", "Purple Rain", "Wonderwall", "Lose Yourself", "Thriller"
};
const char* artists[] = {
  "Queen", "Led Zeppelin", "Eagles", "John Lennon", "Guns N' Roses", 
  "Michael Jackson", "Nirvana", "The Beatles", "Prince", "Oasis", "Eminem", "Michael Jackson"
};
const char* playlists[] = {"Favorites", "Rock Classics", "Chill Vibes", "Workout Mix", "Road Trip"};

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("╔════════════════════════════════════════╗");
  Serial.println("║  TTP229 16-Key iPod with Gestures     ║");
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
  
  Serial.println("✅ 16-Key iPod Ready!");
  Serial.println();
  printControls();
}

void loop() {
  uint16_t touchStatus = readTTP229();
  
  if (touchStatus != lastTouch) {
    if (touchStatus != 0) {
      // Touch started
      if (lastTouch == 0) {
        touchStartTime = millis();
        longPressTriggered = false;
        // Store initial touch key
        for (int i = 0; i < 16; i++) {
          if ((touchStatus >> i) & 0x01) {
            initialTouchKey = i + 1;
            break;
          }
        }
      }
      addToHistory(touchStatus);
      touchCount++;
    } else {
      // Touch ended
      if (!longPressTriggered) {
        unsigned long duration = millis() - touchStartTime;
        GestureType gesture = detectGesture(duration);
        handleGesture(gesture);
      }
      clearHistory();
      touchCount = 0;
      initialTouchKey = 0;
    }
    lastTouch = touchStatus;
    delay(50); // Debounce
  }
  
  // Check for long press while holding
  if (touchStatus != 0 && !longPressTriggered) {
    unsigned long duration = millis() - touchStartTime;
    if (duration > LONG_PRESS_THRESHOLD && touchCount == 1) {
      // Still on same key = long press
      Serial.println("🎯 Gesture: LONG PRESS");
      handleMenu();
      longPressTriggered = true;
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
  
  // Invert (0xFFFF = no touch, bits LOW = touched)
  return ~value;
}

void addToHistory(uint16_t touch) {
  // Get first touched key
  for (int i = 0; i < 16; i++) {
    if ((touch >> i) & 0x01) {
      touchHistory[touchHistoryIndex] = i + 1;
      touchHistoryIndex = (touchHistoryIndex + 1) % MAX_TOUCH_HISTORY;
      break;
    }
  }
}

void clearHistory() {
  for (int i = 0; i < MAX_TOUCH_HISTORY; i++) {
    touchHistory[i] = 0;
  }
  touchHistoryIndex = 0;
}

GestureType detectGesture(unsigned long duration) {
  // Count unique keys
  int uniqueKeys = 0;
  int keys[MAX_TOUCH_HISTORY];
  
  for (int i = 0; i < MAX_TOUCH_HISTORY; i++) {
    if (touchHistory[i] != 0) {
      keys[uniqueKeys++] = touchHistory[i];
    }
  }
  
  if (uniqueKeys == 0) return GESTURE_NONE;
  
  // Single key gestures
  if (uniqueKeys == 1) {
    // Long press already handled in loop()
    if (duration < TAP_THRESHOLD) {
      // Check for double tap
      if (millis() - lastTapTime < 500) {
        lastTapTime = 0;
        return GESTURE_DOUBLE_TAP;
      }
      lastTapTime = millis();
      return GESTURE_TAP;
    }
    // Regular tap for longer durations (but not long enough for long press)
    return GESTURE_TAP;
  }
  
  // Multi-key gestures
  if (uniqueKeys >= SWIPE_MIN_KEYS) {
    int firstKey = keys[0];
    int lastKey = keys[uniqueKeys - 1];
    
    // Check for circular gesture (iPod wheel style)
    if (uniqueKeys >= CIRCULAR_THRESHOLD) {
      GestureType circular = detectCircular(keys, uniqueKeys);
      if (circular != GESTURE_NONE) return circular;
    }
    
    // Check for pinch gestures (corners moving in/out)
    GestureType pinch = detectPinch(keys, uniqueKeys);
    if (pinch != GESTURE_NONE) return pinch;
    
    // Linear swipes
    return detectSwipe(firstKey, lastKey, keys, uniqueKeys);
  }
  
  return GESTURE_NONE;
}

GestureType detectCircular(int* keys, int count) {
  // Map keys to positions (4x4 grid)
  // 1  2  3  4
  // 5  6  7  8
  // 9  10 11 12
  // 13 14 15 16
  
  int angleSum = 0;
  
  for (int i = 1; i < count; i++) {
    int k1 = keys[i-1];
    int k2 = keys[i];
    
    // Calculate angle change
    int row1 = (k1 - 1) / 4;
    int col1 = (k1 - 1) % 4;
    int row2 = (k2 - 1) / 4;
    int col2 = (k2 - 1) % 4;
    
    // Clockwise: right→down→left→up
    if (row1 == row2 && col2 > col1) angleSum++; // Right
    else if (col1 == col2 && row2 > row1) angleSum++; // Down
    else if (row1 == row2 && col2 < col1) angleSum++; // Left
    else if (col1 == col2 && row2 < row1) angleSum++; // Up
    
    // Counter-clockwise: left→down→right→up
    else if (row1 == row2 && col2 < col1) angleSum--; // Left
    else if (col1 == col2 && row2 > row1) angleSum--; // Down
    else if (row1 == row2 && col2 > col1) angleSum--; // Right
    else if (col1 == col2 && row2 < row1) angleSum--; // Up
  }
  
  if (angleSum >= CIRCULAR_THRESHOLD) return GESTURE_CIRCULAR_CW;
  if (angleSum <= -CIRCULAR_THRESHOLD) return GESTURE_CIRCULAR_CCW;
  
  return GESTURE_NONE;
}

GestureType detectPinch(int* keys, int count) {
  int firstKey = keys[0];
  int lastKey = keys[count - 1];
  
  // Pinch in: corners (1,4,13,16) → center (6,7,10,11)
  bool isCorner1 = (firstKey == 1 || firstKey == 4 || firstKey == 13 || firstKey == 16);
  bool isCenter1 = (lastKey == 6 || lastKey == 7 || lastKey == 10 || lastKey == 11);
  
  if (isCorner1 && isCenter1) return GESTURE_PINCH_IN;
  
  // Pinch out: center → corners
  bool isCenter2 = (firstKey == 6 || firstKey == 7 || firstKey == 10 || firstKey == 11);
  bool isCorner2 = (lastKey == 1 || lastKey == 4 || lastKey == 13 || lastKey == 16);
  
  if (isCenter2 && isCorner2) return GESTURE_PINCH_OUT;
  
  return GESTURE_NONE;
}

GestureType detectSwipe(int firstKey, int lastKey, int* keys, int count) {
  int rowDiff = ((lastKey - 1) / 4) - ((firstKey - 1) / 4);
  int colDiff = ((lastKey - 1) % 4) - ((firstKey - 1) % 4);
  
  // Vertical swipes (stronger vertical movement)
  if (abs(rowDiff) > abs(colDiff)) {
    if (rowDiff > 0) return GESTURE_SWIPE_DOWN;
    if (rowDiff < 0) return GESTURE_SWIPE_UP;
  }
  
  // Horizontal swipes (stronger horizontal movement)
  if (abs(colDiff) > abs(rowDiff)) {
    if (colDiff > 0) return GESTURE_SWIPE_RIGHT;
    if (colDiff < 0) return GESTURE_SWIPE_LEFT;
  }
  
  return GESTURE_NONE;
}

void handleGesture(GestureType gesture) {
  if (gesture == GESTURE_NONE) return;
  
  Serial.print("🎯 Gesture: ");
  
  switch (gesture) {
    case GESTURE_TAP:
      Serial.println("TAP");
      handleSelect();
      break;
      
    case GESTURE_DOUBLE_TAP:
      Serial.println("DOUBLE TAP");
      togglePlayPause();
      break;
      
    case GESTURE_LONG_PRESS:
      // Handled in loop() now
      Serial.println("LONG PRESS");
      handleMenu();
      break;
      
    case GESTURE_SWIPE_UP:
      Serial.println("SWIPE UP");
      navigateUp();
      break;
      
    case GESTURE_SWIPE_DOWN:
      Serial.println("SWIPE DOWN");
      navigateDown();
      break;
      
    case GESTURE_SWIPE_LEFT:
      Serial.println("SWIPE LEFT");
      previousTrack();
      break;
      
    case GESTURE_SWIPE_RIGHT:
      Serial.println("SWIPE RIGHT");
      nextTrack();
      break;
      
    case GESTURE_CIRCULAR_CW:
      Serial.println("CIRCULAR CLOCKWISE");
      volumeUp();
      break;
      
    case GESTURE_CIRCULAR_CCW:
      Serial.println("CIRCULAR COUNTER-CLOCKWISE");
      volumeDown();
      break;
      
    case GESTURE_PINCH_IN:
      Serial.println("PINCH IN");
      brightnessDown();
      break;
      
    case GESTURE_PINCH_OUT:
      Serial.println("PINCH OUT");
      brightnessUp();
      break;
  }
}

// Navigation functions
void handleMenu() {
  if (currentScreen != HOME) {
    currentScreen = HOME;
    selectedItem = 0;
    scrollOffset = 0;
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
        case 2: currentScreen = PLAYLISTS; break;
        case 3: currentScreen = ARTISTS; break;
        case 4: currentScreen = SETTINGS; break;
      }
      selectedItem = 0;
      scrollOffset = 0;
      updateMaxItems();
      drawScreen();
      break;
      
    case LIBRARY:
      currentSong = selectedItem;
      currentScreen = NOW_PLAYING;
      isPlaying = true;
      drawScreen();
      break;
      
    case PLAYLISTS:
      currentScreen = LIBRARY;
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
  
  // Scroll if needed
  if (selectedItem < scrollOffset) {
    scrollOffset = selectedItem;
  }
  
  drawScreen();
}

void navigateDown() {
  selectedItem++;
  if (selectedItem >= maxItems) selectedItem = 0;
  
  // Scroll if needed
  int visibleItems = 6;
  if (selectedItem >= scrollOffset + visibleItems) {
    scrollOffset = selectedItem - visibleItems + 1;
  }
  
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
    currentSong = (currentSong - 1 + 12) % 12;
    Serial.println("⏮️ Previous track");
    drawScreen();
  }
}

void nextTrack() {
  if (currentScreen == NOW_PLAYING) {
    currentSong = (currentSong + 1) % 12;
    Serial.println("⏭️ Next track");
    drawScreen();
  }
}

void volumeDown() {
  if (volume > 0) {
    volume -= 10;
    if (volume < 0) volume = 0;
    Serial.print("🔉 Volume: ");
    Serial.println(volume);
    showVolumeOverlay();
  }
}

void volumeUp() {
  if (volume < 100) {
    volume += 10;
    if (volume > 100) volume = 100;
    Serial.print("🔊 Volume: ");
    Serial.println(volume);
    showVolumeOverlay();
  }
}

void brightnessDown() {
  if (brightness > 20) {
    brightness -= 20;
    Serial.print("🔅 Brightness: ");
    Serial.println(brightness);
    showBrightnessOverlay();
  }
}

void brightnessUp() {
  if (brightness < 100) {
    brightness += 20;
    Serial.print("🔆 Brightness: ");
    Serial.println(brightness);
    showBrightnessOverlay();
  }
}

void updateMaxItems() {
  switch (currentScreen) {
    case HOME: maxItems = 5; break;
    case LIBRARY: maxItems = 12; break;
    case PLAYLISTS: maxItems = 5; break;
    case ARTISTS: maxItems = 12; break;
    case NOW_PLAYING: maxItems = 1; break;
    case SETTINGS: maxItems = 4; break;
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
  tft.print("16-Key Touch");
  
  tft.setTextColor(YELLOW);
  tft.setCursor(40, 160);
  tft.print("Advanced Gestures!");
  
  delay(2000);
  tft.fillScreen(BLACK);
}

void drawScreen() {
  tft.fillScreen(BLACK);
  
  switch (currentScreen) {
    case HOME: drawHome(); break;
    case LIBRARY: drawLibrary(); break;
    case NOW_PLAYING: drawNowPlaying(); break;
    case PLAYLISTS: drawPlaylists(); break;
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
  tft.setCursor(140, 30);
  tft.print("16-Key");
  
  for (int i = 0; i < 5; i++) {
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
  
  int visibleItems = 6;
  int startIdx = scrollOffset;
  int endIdx = min(startIdx + visibleItems, 12);
  
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
  
  // Scroll indicator
  if (maxItems > visibleItems) {
    int barHeight = (visibleItems * 180) / maxItems;
    int barY = 80 + (scrollOffset * 180) / maxItems;
    tft.fillRect(235, barY, 3, barHeight, GRAY);
  }
}

void drawPlaylists() {
  drawHeader("Playlists");
  
  for (int i = 0; i < 5; i++) {
    int y = 80 + (i * 30);
    
    if (i == selectedItem) {
      tft.fillRoundRect(5, y - 3, 230, 25, 3, BLUE);
    }
    
    tft.setTextSize(1);
    tft.setTextColor(WHITE);
    tft.setCursor(10, y);
    tft.print(playlists[i]);
    
    tft.setTextColor(GRAY);
    tft.setCursor(180, y);
    tft.print(">");
  }
}

void drawNowPlaying() {
  drawHeader("Now Playing");
  
  // Album art placeholder
  tft.drawRoundRect(60, 80, 120, 120, 10, WHITE);
  tft.fillRoundRect(62, 82, 116, 116, 8, GRAY);
  
  tft.setTextSize(6);
  tft.setTextColor(WHITE);
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
  
  // Play/Pause status
  tft.setTextSize(2);
  tft.setTextColor(isPlaying ? GREEN : RED);
  tft.setCursor(20, 250);
  tft.print(isPlaying ? "Playing" : "Paused");
  
  // Volume
  tft.setTextSize(1);
  tft.setTextColor(CYAN);
  tft.setCursor(150, 255);
  tft.print("Vol:");
  tft.print(volume);
}

void drawSettings() {
  drawHeader("Settings");
  
  const char* settings[] = {"Display", "Audio", "Brightness", "About"};
  
  for (int i = 0; i < 4; i++) {
    int y = 80 + (i * 30);
    
    if (i == selectedItem) {
      tft.fillRoundRect(10, y - 5, 220, 25, 5, BLUE);
    }
    
    tft.setTextSize(1);
    tft.setTextColor(WHITE);
    tft.setCursor(20, y);
    tft.print(settings[i]);
    
    if (i == 2) {
      tft.setTextColor(GRAY);
      tft.setCursor(150, y);
      tft.print(brightness);
      tft.print("%");
    }
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
  tft.print("Long Press: Back");
  
  tft.drawLine(0, 65, 240, 65, GRAY);
}

void drawStatusBar() {
  tft.drawLine(0, 300, 240, 300, GRAY);
  
  tft.setTextSize(1);
  tft.setTextColor(GRAY);
  tft.setCursor(10, 308);
  tft.print("Swipe:Nav Circle:Vol");
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

void showBrightnessOverlay() {
  int barWidth = (brightness * 200) / 100;
  
  tft.fillRect(20, 140, 200, 40, BLACK);
  tft.drawRect(20, 140, 200, 40, WHITE);
  tft.fillRect(22, 142, barWidth, 36, YELLOW);
  
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.setCursor(60, 155);
  tft.print("Bright:");
  tft.print(brightness);
  tft.print("%");
  
  delay(1000);
  drawScreen();
}

void printControls() {
  Serial.println("🎮 Gesture Controls:");
  Serial.println("   Tap: Select");
  Serial.println("   Double Tap: Play/Pause");
  Serial.println("   Long Press: Back/Menu");
  Serial.println("   Swipe Up: Navigate Up");
  Serial.println("   Swipe Down: Navigate Down");
  Serial.println("   Swipe Left: Previous Track");
  Serial.println("   Swipe Right: Next Track");
  Serial.println("   Circular CW: Volume Up");
  Serial.println("   Circular CCW: Volume Down");
  Serial.println("   Pinch In: Brightness Down");
  Serial.println("   Pinch Out: Brightness Up");
  Serial.println();
  Serial.println("📱 16-Key Layout:");
  Serial.println("   ┌────┬────┬────┬────┐");
  Serial.println("   │ 1  │ 2  │ 3  │ 4  │");
  Serial.println("   ├────┼────┼────┼────┤");
  Serial.println("   │ 5  │ 6  │ 7  │ 8  │");
  Serial.println("   ├────┼────┼────┼────┤");
  Serial.println("   │ 9  │ 10 │ 11 │ 12 │");
  Serial.println("   ├────┼────┼────┼────┤");
  Serial.println("   │ 13 │ 14 │ 15 │ 16 │");
  Serial.println("   └────┴────┴────┴────┘");
  Serial.println();
}
