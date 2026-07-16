/*
ESP32-S3 iPod with TTP229 Capacitive Touch Interface
Touch-sensitive buttons through case material
Modern, sleek design with no physical buttons
*/

#include <TFT_eSPI.h>
#include <Wire.h>

TFT_eSPI tft = TFT_eSPI();

// TTP229 I2C Configuration
#define TTP229_ADDR 0x57
#define I2C_SDA 1
#define I2C_SCL 2

// Touch button mapping (TP0-TP15)
#define TOUCH_MENU     0   // TP0 - Menu/Back button
#define TOUCH_PLAY     1   // TP1 - Play/Pause button
#define TOUCH_PREV     2   // TP2 - Previous track
#define TOUCH_NEXT     3   // TP3 - Next track
#define TOUCH_VOL_UP   4   // TP4 - Volume up
#define TOUCH_VOL_DOWN 5   // TP5 - Volume down
#define TOUCH_SCROLL_UP   6   // TP6 - Scroll up
#define TOUCH_SCROLL_DOWN 7   // TP7 - Scroll down
#define TOUCH_SELECT   8   // TP8 - Select/OK

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

// UI State
enum Screen { HOME, LIBRARY, NOW_PLAYING, SETTINGS, VOLUME };
Screen currentScreen = HOME;
int selectedItem = 0;
int maxItems = 4;
bool isPlaying = false;
int volume = 50;
int currentSong = 0;

// Touch state
uint16_t touchStatus = 0;
uint16_t lastTouchStatus = 0;
unsigned long lastTouchTime = 0;
unsigned long touchHoldTime = 0;
bool touchHeld = false;

// Debouncing
#define DEBOUNCE_DELAY 50
#define HOLD_THRESHOLD 500

// Sample data
const char* menuItems[] = {"Music Library", "Now Playing", "Settings", "About"};
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
  Serial.println("║    ESP32-S3 iPod Touch Interface      ║");
  Serial.println("║      TTP229 Capacitive Touch          ║");
  Serial.println("╚════════════════════════════════════════╝");
  Serial.println();
  
  // Initialize I2C for TTP229
  Wire.begin(I2C_SDA, I2C_SCL);
  Wire.setClock(100000); // 100kHz for TTP229
  
  // Initialize display
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(BLACK);
  
  // Test TTP229 connection
  testTTP229();
  
  // Show splash
  showSplash();
  
  // Start with home screen
  currentScreen = HOME;
  updateMaxItems();
  drawScreen();
  
  Serial.println("🎮 Touch iPod Interface Ready!");
  Serial.println("   Touch zones:");
  Serial.println("   - Menu: Navigate back");
  Serial.println("   - Play: Play/Pause");
  Serial.println("   - Prev/Next: Navigate & control");
  Serial.println("   - Select: Choose item");
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
    Serial.println("✅ TTP229 found at address 0x57");
    Serial.print("   I2C SDA: GPIO");
    Serial.println(I2C_SDA);
    Serial.print("   I2C SCL: GPIO");
    Serial.println(I2C_SCL);
  } else {
    Serial.print("❌ TTP229 not found! I2C error: ");
    Serial.println(error);
    Serial.println("   Check connections and try again");
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
  unsigned long currentTime = millis();
  
  // Read touch status
  touchStatus = readTTP229();
  
  // Debouncing
  if (touchStatus != lastTouchStatus) {
    if (currentTime - lastTouchTime > DEBOUNCE_DELAY) {
      // Process touch changes
      for (int i = 0; i < 9; i++) { // Only check pads 0-8
        bool currentState = (touchStatus >> i) & 0x01;
        bool lastState = (lastTouchStatus >> i) & 0x01;
        
        if (currentState && !lastState) {
          // Touch pressed
          handleTouchPress(i);
          touchHoldTime = currentTime;
          touchHeld = false;
        } else if (!currentState && lastState) {
          // Touch released
          handleTouchRelease(i);
        }
      }
      
      lastTouchStatus = touchStatus;
      lastTouchTime = currentTime;
    }
  }
  
  // Check for touch hold
  if (touchStatus != 0 && !touchHeld) {
    if (currentTime - touchHoldTime > HOLD_THRESHOLD) {
      handleTouchHold();
      touchHeld = true;
    }
  }
}

void handleTouchPress(int pad) {
  Serial.print("👆 Touch: ");
  
  switch (pad) {
    case TOUCH_MENU:
      Serial.println("MENU");
      handleMenu();
      break;
      
    case TOUCH_PLAY:
      Serial.println("PLAY");
      handlePlay();
      break;
      
    case TOUCH_PREV:
      Serial.println("PREVIOUS");
      handlePrevious();
      break;
      
    case TOUCH_NEXT:
      Serial.println("NEXT");
      handleNext();
      break;
      
    case TOUCH_VOL_UP:
      Serial.println("VOLUME UP");
      handleVolumeUp();
      break;
      
    case TOUCH_VOL_DOWN:
      Serial.println("VOLUME DOWN");
      handleVolumeDown();
      break;
      
    case TOUCH_SCROLL_UP:
      Serial.println("SCROLL UP");
      navigateUp();
      break;
      
    case TOUCH_SCROLL_DOWN:
      Serial.println("SCROLL DOWN");
      navigateDown();
      break;
      
    case TOUCH_SELECT:
      Serial.println("SELECT");
      handleSelect();
      break;
  }
  
  // Visual feedback
  showTouchFeedback(pad);
}

void handleTouchRelease(int pad) {
  // Optional: Handle release events
}

void handleTouchHold() {
  // Handle long press (e.g., show volume control)
  if ((touchStatus >> TOUCH_VOL_UP) & 0x01) {
    Serial.println("🔊 Volume UP (hold)");
    for (int i = 0; i < 5; i++) {
      handleVolumeUp();
      delay(100);
    }
  } else if ((touchStatus >> TOUCH_VOL_DOWN) & 0x01) {
    Serial.println("🔉 Volume DOWN (hold)");
    for (int i = 0; i < 5; i++) {
      handleVolumeDown();
      delay(100);
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

void handlePlay() {
  if (currentScreen == NOW_PLAYING || currentScreen == LIBRARY) {
    togglePlayPause();
  } else if (currentScreen == HOME) {
    // Quick access to now playing
    currentScreen = NOW_PLAYING;
    selectedItem = 0;
    updateMaxItems();
    drawScreen();
  }
}

void handlePrevious() {
  switch (currentScreen) {
    case HOME:
    case LIBRARY:
    case SETTINGS:
      navigateUp();
      break;
    case NOW_PLAYING:
      previousTrack();
      break;
  }
}

void handleNext() {
  switch (currentScreen) {
    case HOME:
    case LIBRARY:
    case SETTINGS:
      navigateDown();
      break;
    case NOW_PLAYING:
      nextTrack();
      break;
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
      selectedItem = 0;
      updateMaxItems();
      drawScreen();
      break;
      
    case NOW_PLAYING:
      togglePlayPause();
      break;
  }
}

void handleVolumeUp() {
  if (volume < 100) {
    volume += 5;
    if (volume > 100) volume = 100;
    Serial.print("🔊 Volume: ");
    Serial.print(volume);
    Serial.println("%");
    showVolumeOverlay();
  }
}

void handleVolumeDown() {
  if (volume > 0) {
    volume -= 5;
    if (volume < 0) volume = 0;
    Serial.print("🔉 Volume: ");
    Serial.print(volume);
    Serial.println("%");
    showVolumeOverlay();
  }
}

void navigateUp() {
  selectedItem--;
  if (selectedItem < 0) {
    selectedItem = maxItems - 1;
  }
  drawScreen();
}

void navigateDown() {
  selectedItem++;
  if (selectedItem >= maxItems) {
    selectedItem = 0;
  }
  drawScreen();
}

void togglePlayPause() {
  isPlaying = !isPlaying;
  Serial.print("🎵 ");
  Serial.println(isPlaying ? "Playing" : "Paused");
  
  if (currentScreen == NOW_PLAYING) {
    drawScreen();
  }
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
  }
}

// Display Functions
void showSplash() {
  tft.fillScreen(BLACK);
  
  // iPod logo
  tft.setTextSize(4);
  tft.setTextColor(WHITE);
  tft.setCursor(60, 100);
  tft.print("iPod");
  
  tft.setTextSize(1);
  tft.setTextColor(CYAN);
  tft.setCursor(60, 140);
  tft.print("Touch Interface");
  
  tft.setTextColor(PURPLE);
  tft.setCursor(50, 160);
  tft.print("Capacitive Sensing");
  
  // Touch icon
  tft.setTextSize(3);
  tft.setTextColor(YELLOW);
  tft.setCursor(100, 200);
  tft.print("👆");
  
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
  }
  
  drawStatusBar();
  drawTouchIndicators();
}

void drawHome() {
  // Title
  tft.setTextSize(3);
  tft.setTextColor(WHITE);
  tft.setCursor(20, 20);
  tft.print("iPod");
  
  // Touch indicator
  tft.setTextSize(1);
  tft.setTextColor(CYAN);
  tft.setCursor(150, 30);
  tft.print("Touch");
  
  // Menu items
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
    
    if (i == currentSong && isPlaying) {
      tft.fillRoundRect(5, y - 3, 230, 25, 3, DARK_GRAY);
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
    
    if (i == selectedItem) {
      tft.setTextColor(WHITE);
      tft.setCursor(220, y + 6);
      tft.print(">");
    }
  }
}

void drawNowPlaying() {
  drawHeader("Now Playing");
  
  // Album art
  tft.drawRoundRect(60, 80, 120, 120, 10, WHITE);
  tft.fillRoundRect(62, 82, 116, 116, 8, DARK_GRAY);
  
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
  
  // Play/Pause
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
  tft.print("%");
}

void drawSettings() {
  drawHeader("Settings");
  
  const char* settings[] = {"Display", "Audio", "Touch Sensitivity", "About"};
  
  for (int i = 0; i < 4; i++) {
    int y = 80 + (i * 30);
    
    if (i == selectedItem) {
      tft.fillRoundRect(10, y - 5, 220, 25, 5, BLUE);
    }
    
    tft.setTextSize(1);
    tft.setTextColor(WHITE);
    tft.setCursor(20, y);
    tft.print(settings[i]);
    
    if (i == selectedItem) {
      tft.setCursor(200, y);
      tft.print(">");
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
  tft.print("Menu: Back");
  
  tft.drawLine(0, 65, 240, 65, GRAY);
}

void drawStatusBar() {
  tft.drawLine(0, 300, 240, 300, GRAY);
  
  tft.setTextSize(1);
  tft.setTextColor(GRAY);
  tft.setCursor(10, 308);
  tft.print("Touch to navigate");
}

void drawTouchIndicators() {
  // Show active touch zones
  if (touchStatus != 0) {
    tft.setTextSize(1);
    tft.setTextColor(YELLOW);
    tft.setCursor(180, 308);
    tft.print("Touch!");
  }
}

void showTouchFeedback(int pad) {
  // Brief visual feedback
  int x = 10 + (pad * 25);
  int y = 5;
  
  tft.fillCircle(x, y, 3, YELLOW);
  delay(100);
  tft.fillCircle(x, y, 3, BLACK);
}

void showVolumeOverlay() {
  // Volume bar overlay
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
  tft.print("TTP229 Touch Interface");
  tft.setCursor(20, 140);
  tft.print("Capacitive Sensing");
  
  tft.setTextColor(CYAN);
  tft.setCursor(20, 180);
  tft.print("Touch through case!");
  
  tft.setTextColor(BLUE);
  tft.setCursor(20, 220);
  tft.print("Touch Menu to return");
  
  // Wait for menu touch
  while (true) {
    uint16_t touch = readTTP229();
    if ((touch >> TOUCH_MENU) & 0x01) {
      delay(200);
      break;
    }
    delay(50);
  }
  
  currentScreen = HOME;
  drawScreen();
}

void updateUI() {
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate < 1000) return;
  
  lastUpdate = millis();
}