/*
ESP32-S3 iPod UI - Complete Touch Interface
Modern iPod-style music player interface
*/

#include <TFT_eSPI.h>
#include <SPIFFS.h>

TFT_eSPI tft = TFT_eSPI();

// UI Configuration
#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 320
#define STATUS_BAR_HEIGHT 20
#define NAV_BAR_HEIGHT 40
#define CONTENT_HEIGHT (SCREEN_HEIGHT - STATUS_BAR_HEIGHT - NAV_BAR_HEIGHT)

// Colors (iPod-inspired)
#define COLOR_BG 0x0000          // Black background
#define COLOR_TEXT 0xFFFF        // White text
#define COLOR_ACCENT 0x001F      // Blue accent
#define COLOR_GRAY 0x8410        // Gray
#define COLOR_LIGHT_GRAY 0xC618  // Light gray
#define COLOR_DARK_GRAY 0x4208   // Dark gray
#define COLOR_GREEN 0x07E0       // Green
#define COLOR_RED 0xF800         // Red
#define COLOR_ORANGE 0xFD20      // Orange

// Screen states
enum Screen {
  SCREEN_HOME,
  SCREEN_LIBRARY,
  SCREEN_NOW_PLAYING,
  SCREEN_SETTINGS,
  SCREEN_ARTISTS,
  SCREEN_ALBUMS,
  SCREEN_SONGS,
  SCREEN_PLAYLISTS
};

// Current state
Screen currentScreen = SCREEN_HOME;
int selectedItem = 0;
int scrollOffset = 0;
bool isPlaying = false;
int currentSong = 0;
int volume = 75;
int progress = 30; // Progress percentage

// Touch handling
uint16_t lastTouchX = 0, lastTouchY = 0;
unsigned long lastTouchTime = 0;
bool touchPressed = false;

// Sample data
const char* homeMenuItems[] = {
  "Music Library",
  "Now Playing", 
  "Playlists",
  "Artists",
  "Albums",
  "Songs",
  "Settings"
};
const int homeMenuCount = 7;

const char* artists[] = {
  "The Beatles", "Queen", "Led Zeppelin", "Pink Floyd", 
  "The Rolling Stones", "AC/DC", "Nirvana", "Radiohead",
  "David Bowie", "The Who", "Metallica", "U2"
};
const int artistCount = 12;

const char* songs[] = {
  "Bohemian Rhapsody", "Stairway to Heaven", "Hotel California",
  "Imagine", "Sweet Child O' Mine", "Smells Like Teen Spirit",
  "Billie Jean", "Like a Rolling Stone", "Purple Haze",
  "Yesterday", "Good Vibrations", "Respect"
};
const int songCount = 12;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("ESP32-S3 iPod UI Starting...");
  
  // Initialize SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS initialization failed!");
  }
  
  // Initialize display
  tft.init();
  tft.setRotation(0); // Portrait
  tft.fillScreen(COLOR_BG);
  
  // Load touch calibration
  loadTouchCalibration();
  
  // Show splash screen
  showSplashScreen();
  
  // Initialize UI
  currentScreen = SCREEN_HOME;
  selectedItem = 0;
  
  Serial.println("iPod UI Ready!");
}

void loop() {
  handleTouch();
  updateDisplay();
  delay(50);
}

void handleTouch() {
  uint16_t x, y;
  
  if (tft.getTouch(&x, &y)) {
    if (!touchPressed) {
      touchPressed = true;
      lastTouchX = x;
      lastTouchY = y;
      lastTouchTime = millis();
      
      handleTouchPress(x, y);
    }
  } else {
    if (touchPressed) {
      touchPressed = false;
      handleTouchRelease();
    }
  }
}

void handleTouchPress(uint16_t x, uint16_t y) {
  Serial.print("Touch: (");
  Serial.print(x);
  Serial.print(", ");
  Serial.print(y);
  Serial.println(")");
  
  // Status bar touch (top 20 pixels)
  if (y < STATUS_BAR_HEIGHT) {
    handleStatusBarTouch(x, y);
    return;
  }
  
  // Navigation bar touch (bottom 40 pixels)
  if (y > SCREEN_HEIGHT - NAV_BAR_HEIGHT) {
    handleNavBarTouch(x, y);
    return;
  }
  
  // Content area touch
  handleContentTouch(x, y);
}

void handleStatusBarTouch(uint16_t x, uint16_t y) {
  // Status bar interactions (battery, time, etc.)
  if (x > 200) {
    // Battery area - show battery info
    showBatteryInfo();
  }
}

void handleNavBarTouch(uint16_t x, uint16_t y) {
  int navY = SCREEN_HEIGHT - NAV_BAR_HEIGHT;
  int buttonWidth = SCREEN_WIDTH / 4;
  
  if (x < buttonWidth) {
    // Back button
    goBack();
  } else if (x < buttonWidth * 2) {
    // Home button
    currentScreen = SCREEN_HOME;
    selectedItem = 0;
  } else if (x < buttonWidth * 3) {
    // Menu button
    showContextMenu();
  } else {
    // Now Playing button
    currentScreen = SCREEN_NOW_PLAYING;
  }
}

void handleContentTouch(uint16_t x, uint16_t y) {
  int contentY = y - STATUS_BAR_HEIGHT;
  int itemHeight = 40;
  int touchedItem = (contentY + scrollOffset) / itemHeight;
  
  switch (currentScreen) {
    case SCREEN_HOME:
      if (touchedItem < homeMenuCount) {
        selectedItem = touchedItem;
        selectHomeMenuItem(touchedItem);
      }
      break;
      
    case SCREEN_LIBRARY:
      handleLibraryTouch(touchedItem);
      break;
      
    case SCREEN_NOW_PLAYING:
      handleNowPlayingTouch(x, contentY);
      break;
      
    case SCREEN_ARTISTS:
      if (touchedItem < artistCount) {
        selectedItem = touchedItem;
        // Show artist's albums/songs
      }
      break;
      
    case SCREEN_SONGS:
      if (touchedItem < songCount) {
        selectedItem = touchedItem;
        playSong(touchedItem);
      }
      break;
  }
}

void selectHomeMenuItem(int item) {
  switch (item) {
    case 0: currentScreen = SCREEN_LIBRARY; break;
    case 1: currentScreen = SCREEN_NOW_PLAYING; break;
    case 2: currentScreen = SCREEN_PLAYLISTS; break;
    case 3: currentScreen = SCREEN_ARTISTS; break;
    case 4: currentScreen = SCREEN_ALBUMS; break;
    case 5: currentScreen = SCREEN_SONGS; break;
    case 6: currentScreen = SCREEN_SETTINGS; break;
  }
  selectedItem = 0;
  scrollOffset = 0;
}

void handleNowPlayingTouch(uint16_t x, uint16_t y) {
  // Play/Pause button (center)
  if (x > 80 && x < 160 && y > 200 && y < 240) {
    isPlaying = !isPlaying;
    return;
  }
  
  // Previous button (left)
  if (x > 40 && x < 80 && y > 200 && y < 240) {
    previousSong();
    return;
  }
  
  // Next button (right)
  if (x > 160 && x < 200 && y > 200 && y < 240) {
    nextSong();
    return;
  }
  
  // Progress bar
  if (y > 170 && y < 190) {
    progress = map(x, 20, 220, 0, 100);
    return;
  }
  
  // Volume control
  if (y > 240 && y < 260) {
    volume = map(x, 20, 220, 0, 100);
    return;
  }
}

void updateDisplay() {
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate < 100) return; // 10 FPS
  
  drawStatusBar();
  drawContent();
  drawNavBar();
  
  lastUpdate = millis();
}

void drawStatusBar() {
  // Clear status bar
  tft.fillRect(0, 0, SCREEN_WIDTH, STATUS_BAR_HEIGHT, COLOR_DARK_GRAY);
  
  // iPod logo/title
  tft.setTextColor(COLOR_TEXT);
  tft.setTextSize(1);
  tft.setCursor(5, 6);
  tft.print("iPod");
  
  // Time (center)
  tft.setCursor(100, 6);
  tft.print("12:34");
  
  // Battery (right)
  drawBattery(200, 4);
  
  // Separator line
  tft.drawFastHLine(0, STATUS_BAR_HEIGHT - 1, SCREEN_WIDTH, COLOR_GRAY);
}

void drawBattery(int x, int y) {
  // Battery outline
  tft.drawRect(x, y, 20, 12, COLOR_TEXT);
  tft.drawRect(x + 20, y + 3, 2, 6, COLOR_TEXT);
  
  // Battery level (75%)
  int level = 75;
  int fillWidth = map(level, 0, 100, 0, 18);
  uint16_t color = level > 20 ? COLOR_GREEN : COLOR_RED;
  tft.fillRect(x + 1, y + 1, fillWidth, 10, color);
  
  // Battery percentage
  tft.setCursor(x + 25, y + 2);
  tft.setTextSize(1);
  tft.print(level);
  tft.print("%");
}

void drawContent() {
  // Clear content area
  tft.fillRect(0, STATUS_BAR_HEIGHT, SCREEN_WIDTH, CONTENT_HEIGHT, COLOR_BG);
  
  switch (currentScreen) {
    case SCREEN_HOME:
      drawHomeScreen();
      break;
    case SCREEN_LIBRARY:
      drawLibraryScreen();
      break;
    case SCREEN_NOW_PLAYING:
      drawNowPlayingScreen();
      break;
    case SCREEN_SETTINGS:
      drawSettingsScreen();
      break;
    case SCREEN_ARTISTS:
      drawArtistsScreen();
      break;
    case SCREEN_SONGS:
      drawSongsScreen();
      break;
  }
}

void drawHomeScreen() {
  tft.setTextSize(2);
  tft.setTextColor(COLOR_TEXT);
  tft.setCursor(10, STATUS_BAR_HEIGHT + 10);
  tft.println("Music");
  
  // Menu items
  for (int i = 0; i < homeMenuCount; i++) {
    int y = STATUS_BAR_HEIGHT + 50 + (i * 30);
    
    // Highlight selected item
    if (i == selectedItem) {
      tft.fillRect(5, y - 2, SCREEN_WIDTH - 10, 26, COLOR_ACCENT);
    }
    
    tft.setTextSize(1);
    tft.setTextColor(i == selectedItem ? COLOR_BG : COLOR_TEXT);
    tft.setCursor(15, y + 5);
    tft.println(homeMenuItems[i]);
    
    // Arrow indicator
    if (i == selectedItem) {
      tft.setCursor(SCREEN_WIDTH - 20, y + 5);
      tft.print(">");
    }
  }
}

void drawNowPlayingScreen() {
  // Title
  tft.setTextSize(2);
  tft.setTextColor(COLOR_TEXT);
  tft.setCursor(10, STATUS_BAR_HEIGHT + 10);
  tft.println("Now Playing");
  
  // Album art placeholder
  int artSize = 120;
  int artX = (SCREEN_WIDTH - artSize) / 2;
  int artY = STATUS_BAR_HEIGHT + 50;
  
  tft.drawRect(artX, artY, artSize, artSize, COLOR_GRAY);
  tft.fillRect(artX + 2, artY + 2, artSize - 4, artSize - 4, COLOR_DARK_GRAY);
  
  // Music note symbol
  tft.setTextSize(4);
  tft.setTextColor(COLOR_GRAY);
  tft.setCursor(artX + 45, artY + 40);
  tft.print("♪");
  
  // Song info
  tft.setTextSize(1);
  tft.setTextColor(COLOR_TEXT);
  tft.setCursor(10, artY + artSize + 15);
  tft.println(songs[currentSong]);
  tft.setCursor(10, artY + artSize + 30);
  tft.setTextColor(COLOR_GRAY);
  tft.println(artists[currentSong % artistCount]);
  
  // Progress bar
  int progY = artY + artSize + 50;
  drawProgressBar(20, progY, 200, progress);
  
  // Time
  tft.setTextColor(COLOR_TEXT);
  tft.setCursor(20, progY + 15);
  tft.print("1:23");
  tft.setCursor(190, progY + 15);
  tft.print("3:45");
  
  // Control buttons
  int buttonY = progY + 35;
  drawControlButtons(buttonY);
  
  // Volume
  int volY = buttonY + 50;
  tft.setCursor(20, volY);
  tft.setTextColor(COLOR_GRAY);
  tft.print("Volume");
  drawVolumeBar(20, volY + 15, 200, volume);
}

void drawProgressBar(int x, int y, int width, int percent) {
  // Background
  tft.drawRect(x, y, width, 8, COLOR_GRAY);
  tft.fillRect(x + 1, y + 1, width - 2, 6, COLOR_DARK_GRAY);
  
  // Progress
  int progWidth = map(percent, 0, 100, 0, width - 2);
  tft.fillRect(x + 1, y + 1, progWidth, 6, COLOR_ACCENT);
}

void drawVolumeBar(int x, int y, int width, int percent) {
  // Background
  tft.drawRect(x, y, width, 6, COLOR_GRAY);
  tft.fillRect(x + 1, y + 1, width - 2, 4, COLOR_DARK_GRAY);
  
  // Volume level
  int volWidth = map(percent, 0, 100, 0, width - 2);
  uint16_t color = percent > 70 ? COLOR_RED : (percent > 30 ? COLOR_ORANGE : COLOR_GREEN);
  tft.fillRect(x + 1, y + 1, volWidth, 4, color);
}

void drawControlButtons(int y) {
  // Previous button
  tft.drawCircle(60, y + 20, 18, COLOR_GRAY);
  tft.setTextSize(1);
  tft.setTextColor(COLOR_TEXT);
  tft.setCursor(55, y + 17);
  tft.print("<<");
  
  // Play/Pause button (larger)
  tft.drawCircle(120, y + 20, 25, COLOR_ACCENT);
  tft.setTextSize(2);
  tft.setCursor(isPlaying ? 115 : 113, y + 14);
  tft.print(isPlaying ? "||" : ">");
  
  // Next button
  tft.drawCircle(180, y + 20, 18, COLOR_GRAY);
  tft.setTextSize(1);
  tft.setCursor(175, y + 17);
  tft.print(">>");
}

void drawArtistsScreen() {
  // Title
  tft.setTextSize(2);
  tft.setTextColor(COLOR_TEXT);
  tft.setCursor(10, STATUS_BAR_HEIGHT + 10);
  tft.println("Artists");
  
  // Artist list
  for (int i = 0; i < artistCount && i < 8; i++) {
    int y = STATUS_BAR_HEIGHT + 50 + (i * 25);
    
    if (i == selectedItem) {
      tft.fillRect(5, y - 2, SCREEN_WIDTH - 10, 21, COLOR_ACCENT);
    }
    
    tft.setTextSize(1);
    tft.setTextColor(i == selectedItem ? COLOR_BG : COLOR_TEXT);
    tft.setCursor(15, y + 3);
    tft.println(artists[i]);
  }
}

void drawSongsScreen() {
  // Title
  tft.setTextSize(2);
  tft.setTextColor(COLOR_TEXT);
  tft.setCursor(10, STATUS_BAR_HEIGHT + 10);
  tft.println("Songs");
  
  // Song list
  for (int i = 0; i < songCount && i < 8; i++) {
    int y = STATUS_BAR_HEIGHT + 50 + (i * 25);
    
    if (i == selectedItem) {
      tft.fillRect(5, y - 2, SCREEN_WIDTH - 10, 21, COLOR_ACCENT);
    }
    
    tft.setTextSize(1);
    tft.setTextColor(i == selectedItem ? COLOR_BG : COLOR_TEXT);
    tft.setCursor(15, y + 3);
    tft.println(songs[i]);
    
    // Now playing indicator
    if (i == currentSong && isPlaying) {
      tft.setTextColor(COLOR_GREEN);
      tft.setCursor(SCREEN_WIDTH - 20, y + 3);
      tft.print("♪");
    }
  }
}

void drawLibraryScreen() {
  tft.setTextSize(2);
  tft.setTextColor(COLOR_TEXT);
  tft.setCursor(10, STATUS_BAR_HEIGHT + 10);
  tft.println("Library");
  
  const char* libraryItems[] = {"Artists", "Albums", "Songs", "Playlists", "Genres"};
  
  for (int i = 0; i < 5; i++) {
    int y = STATUS_BAR_HEIGHT + 50 + (i * 30);
    
    if (i == selectedItem) {
      tft.fillRect(5, y - 2, SCREEN_WIDTH - 10, 26, COLOR_ACCENT);
    }
    
    tft.setTextSize(1);
    tft.setTextColor(i == selectedItem ? COLOR_BG : COLOR_TEXT);
    tft.setCursor(15, y + 5);
    tft.println(libraryItems[i]);
  }
}

void drawSettingsScreen() {
  tft.setTextSize(2);
  tft.setTextColor(COLOR_TEXT);
  tft.setCursor(10, STATUS_BAR_HEIGHT + 10);
  tft.println("Settings");
  
  const char* settingsItems[] = {"Display", "Audio", "Touch", "About"};
  
  for (int i = 0; i < 4; i++) {
    int y = STATUS_BAR_HEIGHT + 50 + (i * 30);
    
    if (i == selectedItem) {
      tft.fillRect(5, y - 2, SCREEN_WIDTH - 10, 26, COLOR_ACCENT);
    }
    
    tft.setTextSize(1);
    tft.setTextColor(i == selectedItem ? COLOR_BG : COLOR_TEXT);
    tft.setCursor(15, y + 5);
    tft.println(settingsItems[i]);
  }
}

void drawNavBar() {
  int navY = SCREEN_HEIGHT - NAV_BAR_HEIGHT;
  
  // Background
  tft.fillRect(0, navY, SCREEN_WIDTH, NAV_BAR_HEIGHT, COLOR_DARK_GRAY);
  tft.drawFastHLine(0, navY, SCREEN_WIDTH, COLOR_GRAY);
  
  // Navigation buttons
  int buttonWidth = SCREEN_WIDTH / 4;
  
  // Back button
  tft.setTextSize(1);
  tft.setTextColor(COLOR_TEXT);
  tft.setCursor(buttonWidth/2 - 10, navY + 15);
  tft.print("Back");
  
  // Home button
  tft.setCursor(buttonWidth + buttonWidth/2 - 10, navY + 15);
  tft.print("Home");
  
  // Menu button
  tft.setCursor(2*buttonWidth + buttonWidth/2 - 10, navY + 15);
  tft.print("Menu");
  
  // Now Playing button
  tft.setCursor(3*buttonWidth + buttonWidth/2 - 15, navY + 15);
  tft.print("Playing");
  
  // Button separators
  for (int i = 1; i < 4; i++) {
    tft.drawFastVLine(i * buttonWidth, navY + 5, NAV_BAR_HEIGHT - 10, COLOR_GRAY);
  }
}

void showSplashScreen() {
  tft.fillScreen(COLOR_BG);
  
  // iPod logo
  tft.setTextSize(4);
  tft.setTextColor(COLOR_TEXT);
  tft.setCursor(60, 120);
  tft.println("iPod");
  
  tft.setTextSize(1);
  tft.setTextColor(COLOR_GRAY);
  tft.setCursor(80, 160);
  tft.println("ESP32-S3");
  
  // Loading animation
  for (int i = 0; i < 3; i++) {
    tft.fillCircle(100 + i * 20, 200, 5, COLOR_ACCENT);
    delay(200);
  }
  
  delay(1000);
}

// Utility functions
void goBack() {
  switch (currentScreen) {
    case SCREEN_LIBRARY:
    case SCREEN_NOW_PLAYING:
    case SCREEN_SETTINGS:
      currentScreen = SCREEN_HOME;
      break;
    case SCREEN_ARTISTS:
    case SCREEN_ALBUMS:
    case SCREEN_SONGS:
    case SCREEN_PLAYLISTS:
      currentScreen = SCREEN_LIBRARY;
      break;
  }
  selectedItem = 0;
}

void playSong(int songIndex) {
  currentSong = songIndex;
  isPlaying = true;
  progress = 0;
  currentScreen = SCREEN_NOW_PLAYING;
  
  Serial.print("Playing: ");
  Serial.println(songs[songIndex]);
}

void nextSong() {
  currentSong = (currentSong + 1) % songCount;
  progress = 0;
  Serial.print("Next: ");
  Serial.println(songs[currentSong]);
}

void previousSong() {
  currentSong = (currentSong - 1 + songCount) % songCount;
  progress = 0;
  Serial.print("Previous: ");
  Serial.println(songs[currentSong]);
}

void showContextMenu() {
  // Simple context menu
  tft.fillRect(50, 100, 140, 120, COLOR_DARK_GRAY);
  tft.drawRect(50, 100, 140, 120, COLOR_GRAY);
  
  tft.setTextSize(1);
  tft.setTextColor(COLOR_TEXT);
  tft.setCursor(60, 115);
  tft.println("Shuffle");
  tft.setCursor(60, 135);
  tft.println("Repeat");
  tft.setCursor(60, 155);
  tft.println("Equalizer");
  tft.setCursor(60, 175);
  tft.println("Sleep Timer");
  tft.setCursor(60, 195);
  tft.println("Cancel");
  
  delay(2000); // Auto-close after 2 seconds
}

void showBatteryInfo() {
  tft.fillRect(150, 25, 85, 60, COLOR_DARK_GRAY);
  tft.drawRect(150, 25, 85, 60, COLOR_GRAY);
  
  tft.setTextSize(1);
  tft.setTextColor(COLOR_TEXT);
  tft.setCursor(155, 35);
  tft.println("Battery");
  tft.setCursor(155, 50);
  tft.println("75% - 4h 23m");
  tft.setCursor(155, 65);
  tft.println("Charging");
  
  delay(2000);
}

void handleTouchRelease() {
  // Handle touch release events if needed
}

void handleLibraryTouch(int item) {
  selectedItem = item;
  switch (item) {
    case 0: currentScreen = SCREEN_ARTISTS; break;
    case 1: currentScreen = SCREEN_ALBUMS; break;
    case 2: currentScreen = SCREEN_SONGS; break;
    case 3: currentScreen = SCREEN_PLAYLISTS; break;
  }
}

bool loadTouchCalibration() {
  // Load touch calibration from SPIFFS if available
  File f = SPIFFS.open("/TouchCalData", "r");
  if (f) {
    uint16_t calData[5];
    f.read((uint8_t*)calData, sizeof(calData));
    f.close();
    tft.setTouch(calData);
    Serial.println("Touch calibration loaded");
    return true;
  }
  Serial.println("No touch calibration found - using defaults");
  return false;
}