/*
ESP32-S3 iPod UI - Simple Version
Basic touchscreen iPod interface for testing
*/

#include <TFT_eSPI.h>

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

// UI state
enum Screen { HOME, LIBRARY, NOW_PLAYING, SETTINGS };
Screen currentScreen = HOME;
int selectedItem = 0;
bool isPlaying = false;
int volume = 50;
int progress = 25;

// Sample data
const char* menuItems[] = {"Music Library", "Now Playing", "Settings", "About"};
const char* songs[] = {"Bohemian Rhapsody", "Stairway to Heaven", "Hotel California", "Imagine"};
const char* artists[] = {"Queen", "Led Zeppelin", "Eagles", "John Lennon"};

void setup() {
  Serial.begin(115200);
  Serial.println("iPod UI Starting...");
  
  // Initialize display
  tft.init();
  tft.setRotation(0); // Portrait
  tft.fillScreen(BLACK);
  
  // Show splash
  showSplash();
  
  // Start with home screen
  currentScreen = HOME;
  drawScreen();
}

void loop() {
  handleTouch();
  delay(50);
}

void showSplash() {
  tft.fillScreen(BLACK);
  
  // iPod logo
  tft.setTextSize(4);
  tft.setTextColor(WHITE);
  tft.setCursor(60, 120);
  tft.print("iPod");
  
  tft.setTextSize(1);
  tft.setTextColor(GRAY);
  tft.setCursor(90, 160);
  tft.print("ESP32-S3");
  
  delay(2000);
  tft.fillScreen(BLACK);
}

void handleTouch() {
  uint16_t x, y;
  
  if (tft.getTouch(&x, &y)) {
    Serial.print("Touch: ");
    Serial.print(x);
    Serial.print(", ");
    Serial.println(y);
    
    processTouch(x, y);
    delay(200); // Simple debounce
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
  }
}

void handleHomeTouch(uint16_t x, uint16_t y) {
  // Calculate which menu item was touched
  int item = (y - 60) / 40;
  
  if (item >= 0 && item < 4) {
    selectedItem = item;
    
    switch (item) {
      case 0: currentScreen = LIBRARY; break;
      case 1: currentScreen = NOW_PLAYING; break;
      case 2: currentScreen = SETTINGS; break;
      case 3: showAbout(); return;
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
  int song = (y - 80) / 35;
  if (song >= 0 && song < 4) {
    selectedItem = song;
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
  
  // Play/Pause button (center area)
  if (x > 80 && x < 160 && y > 200 && y < 240) {
    isPlaying = !isPlaying;
    drawNowPlaying();
    return;
  }
  
  // Previous button (left)
  if (x > 40 && x < 80 && y > 200 && y < 240) {
    selectedItem = (selectedItem - 1 + 4) % 4;
    drawNowPlaying();
    return;
  }
  
  // Next button (right)
  if (x > 160 && x < 200 && y > 200 && y < 240) {
    selectedItem = (selectedItem + 1) % 4;
    drawNowPlaying();
    return;
  }
  
  // Volume control
  if (y > 260 && y < 280) {
    volume = map(x, 20, 220, 0, 100);
    drawNowPlaying();
  }
}

void handleSettingsTouch(uint16_t x, uint16_t y) {
  // Back button
  if (x < 60 && y < 40) {
    currentScreen = HOME;
    drawScreen();
  }
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
  }
}

void drawHome() {
  // Title
  tft.setTextSize(3);
  tft.setTextColor(WHITE);
  tft.setCursor(20, 20);
  tft.print("iPod");
  
  // Menu items
  for (int i = 0; i < 4; i++) {
    int y = 60 + (i * 40);
    
    // Highlight selected item
    if (i == selectedItem) {
      tft.fillRect(10, y - 5, 220, 30, BLUE);
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
  
  // Status info
  tft.setTextSize(1);
  tft.setTextColor(GRAY);
  tft.setCursor(20, 280);
  tft.print("4 Songs • Touch to navigate");
}

void drawLibrary() {
  // Back button
  tft.setTextSize(1);
  tft.setTextColor(BLUE);
  tft.setCursor(10, 10);
  tft.print("< Back");
  
  // Title
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.setCursor(20, 40);
  tft.print("Music Library");
  
  // Song list
  for (int i = 0; i < 4; i++) {
    int y = 80 + (i * 35);
    
    // Highlight if currently playing
    if (i == selectedItem && currentScreen == NOW_PLAYING) {
      tft.fillRect(10, y - 3, 220, 25, DARK_GRAY);
    }
    
    tft.setTextSize(1);
    tft.setTextColor(WHITE);
    tft.setCursor(20, y);
    tft.print(songs[i]);
    
    tft.setTextColor(GRAY);
    tft.setCursor(20, y + 12);
    tft.print(artists[i]);
    
    // Playing indicator
    if (i == selectedItem && isPlaying) {
      tft.setTextColor(GREEN);
      tft.setCursor(200, y + 6);
      tft.print("♪");
    }
  }
}

void drawNowPlaying() {
  // Back button
  tft.setTextSize(1);
  tft.setTextColor(BLUE);
  tft.setCursor(10, 10);
  tft.print("< Back");
  
  // Title
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.setCursor(20, 40);
  tft.print("Now Playing");
  
  // Album art placeholder
  tft.drawRect(70, 80, 100, 100, WHITE);
  tft.fillRect(72, 82, 96, 96, DARK_GRAY);
  
  // Music note
  tft.setTextSize(4);
  tft.setTextColor(GRAY);
  tft.setCursor(110, 115);
  tft.print("♪");
  
  // Song info
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setCursor(20, 200);
  tft.print(songs[selectedItem]);
  
  tft.setTextColor(GRAY);
  tft.setCursor(20, 215);
  tft.print(artists[selectedItem]);
  
  // Control buttons
  drawControls();
  
  // Volume
  tft.setTextColor(GRAY);
  tft.setCursor(20, 250);
  tft.print("Volume: ");
  tft.print(volume);
  tft.print("%");
  
  // Volume bar
  tft.drawRect(20, 265, 200, 10, WHITE);
  int volWidth = map(volume, 0, 100, 0, 198);
  tft.fillRect(21, 266, volWidth, 8, GREEN);
}

void drawControls() {
  int y = 230;
  
  // Previous button
  tft.drawCircle(60, y, 15, WHITE);
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setCursor(55, y - 3);
  tft.print("<<");
  
  // Play/Pause button
  tft.drawCircle(120, y, 20, WHITE);
  tft.setTextSize(2);
  tft.setCursor(isPlaying ? 115 : 113, y - 6);
  tft.print(isPlaying ? "||" : ">");
  
  // Next button
  tft.drawCircle(180, y, 15, WHITE);
  tft.setTextSize(1);
  tft.setCursor(175, y - 3);
  tft.print(">>");
}

void drawSettings() {
  // Back button
  tft.setTextSize(1);
  tft.setTextColor(BLUE);
  tft.setCursor(10, 10);
  tft.print("< Back");
  
  // Title
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.setCursor(20, 40);
  tft.print("Settings");
  
  // Settings items
  const char* settings[] = {"Display Brightness", "Touch Calibration", "Audio Settings", "About"};
  
  for (int i = 0; i < 4; i++) {
    int y = 80 + (i * 30);
    
    tft.setTextSize(1);
    tft.setTextColor(WHITE);
    tft.setCursor(20, y);
    tft.print(settings[i]);
    
    tft.setTextColor(GRAY);
    tft.setCursor(200, y);
    tft.print(">");
  }
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
  tft.print("Version 1.0");
  tft.setCursor(20, 140);
  tft.print("TFT_eSPI Display");
  tft.setCursor(20, 160);
  tft.print("Touch Interface");
  
  tft.setTextColor(BLUE);
  tft.setCursor(20, 200);
  tft.print("Touch anywhere to return");
  
  // Wait for touch
  while (true) {
    uint16_t x, y;
    if (tft.getTouch(&x, &y)) {
      currentScreen = HOME;
      drawScreen();
      break;
    }
    delay(50);
  }
}