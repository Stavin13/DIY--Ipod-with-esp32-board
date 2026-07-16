/*
ESP32-S3 iPod with Haptic Feedback Interface
Physical buttons with DRV2605L haptic motor driver
Authentic iPod experience with tactile vibration feedback
*/

#include <TFT_eSPI.h>
#include <Wire.h>

TFT_eSPI tft = TFT_eSPI();

// DRV2605L Haptic Motor Driver
#define DRV2605L_ADDR 0x5A
#define DRV2605L_STATUS 0x00
#define DRV2605L_MODE 0x01
#define DRV2605L_RTPIN 0x02
#define DRV2605L_LIBRARY 0x03
#define DRV2605L_WAVESEQ1 0x04
#define DRV2605L_WAVESEQ2 0x05
#define DRV2605L_WAVESEQ3 0x06
#define DRV2605L_WAVESEQ4 0x07
#define DRV2605L_WAVESEQ5 0x08
#define DRV2605L_WAVESEQ6 0x09
#define DRV2605L_WAVESEQ7 0x0A
#define DRV2605L_WAVESEQ8 0x0B
#define DRV2605L_GO 0x0C
#define DRV2605L_OVERDRIVE 0x0D
#define DRV2605L_SUSTAINPOS 0x0E
#define DRV2605L_SUSTAINNEG 0x0F
#define DRV2605L_BREAK 0x10
#define DRV2605L_AUDIOCTRL 0x11
#define DRV2605L_AUDIOLVL 0x12
#define DRV2605L_AUDIOMAX 0x13
#define DRV2605L_RATEDV 0x16
#define DRV2605L_CLAMPV 0x17
#define DRV2605L_AUTOCALCOMP 0x18
#define DRV2605L_AUTOCALEMP 0x19
#define DRV2605L_FEEDBACK 0x1A
#define DRV2605L_CONTROL1 0x1B
#define DRV2605L_CONTROL2 0x1C
#define DRV2605L_CONTROL3 0x1D
#define DRV2605L_CONTROL4 0x1E
#define DRV2605L_VBAT 0x21
#define DRV2605L_LRARESON 0x22

// I2C Pins for ESP32-S3
#define I2C_SDA 21
#define I2C_SCL 22

// Haptic Button Pins
#define BTN_MENU     14   // Menu button (back/up)
#define BTN_PLAY     15   // Play/Pause button
#define BTN_PREV     16   // Previous track
#define BTN_NEXT     17   // Next track

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

// Haptic Effect Library (DRV2605L built-in effects)
enum HapticEffect {
  EFFECT_SHARP_CLICK = 1,      // Sharp click 100%
  EFFECT_DOUBLE_CLICK = 2,     // Double click 100%
  EFFECT_TRIPLE_CLICK = 3,     // Triple click 100%
  EFFECT_SOFT_BUMP = 10,       // Soft bump 100%
  EFFECT_DOUBLE_BUMP = 11,     // Double bump 100%
  EFFECT_PULSING_SHARP = 12,   // Pulsing sharp 100%
  EFFECT_SOFT_FUZZ = 15,       // Soft fuzz 60%
  EFFECT_STRONG_BUZZ = 16,     // Strong buzz 100%
  EFFECT_ALERT_750MS = 25,     // Alert 750ms
  EFFECT_BUZZ_100 = 47,        // Buzz 100%
  EFFECT_LONG_BUZZ = 94        // Long buzz for errors
};

// UI State
enum Screen { HOME, LIBRARY, NOW_PLAYING, SETTINGS };
Screen currentScreen = HOME;
int selectedItem = 0;
int maxItems = 4;
bool isPlaying = false;
int volume = 50;
int currentSong = 0;

// Button state tracking
struct ButtonState {
  bool lastState;
  unsigned long lastPressTime;
  bool pressed;
};

ButtonState menuBtn = {HIGH, 0, false};
ButtonState playBtn = {HIGH, 0, false};
ButtonState prevBtn = {HIGH, 0, false};
ButtonState nextBtn = {HIGH, 0, false};

// Haptic feedback state
bool hapticEnabled = true;
uint8_t hapticIntensity = 80; // 0-100%

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
  delay(2000);
  
  Serial.println("╔════════════════════════════════════════╗");
  Serial.println("║    ESP32-S3 iPod Haptic Interface     ║");
  Serial.println("║     Physical Buttons + Vibration      ║");
  Serial.println("╚════════════════════════════════════════╝");
  Serial.println();
  
  // Initialize I2C
  Wire.begin(I2C_SDA, I2C_SCL);
  Wire.setClock(400000); // 400kHz I2C speed
  
  // Initialize display
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(BLACK);
  
  // Setup haptic motor driver
  setupHaptic();
  
  // Setup buttons
  setupButtons();
  
  // Show splash with haptic
  showSplash();
  
  // Start with home screen
  currentScreen = HOME;
  updateMaxItems();
  drawScreen();
  
  Serial.println("🎮 Haptic iPod Interface Ready!");
  Serial.println("   Menu Button: Navigate back");
  Serial.println("   Play Button: Play/Pause");
  Serial.println("   Prev/Next: Navigate & control tracks");
  Serial.println("   Haptic: Tactile feedback enabled");
  Serial.println();
}

void loop() {
  handleButtons();
  updateUI();
  delay(10);
}

void setupHaptic() {
  Serial.println("🔄 Initializing DRV2605L Haptic Driver...");
  
  // Check if DRV2605L is connected
  Wire.beginTransmission(DRV2605L_ADDR);
  if (Wire.endTransmission() != 0) {
    Serial.println("❌ DRV2605L not found! Check I2C connections.");
    hapticEnabled = false;
    return;
  }
  
  Serial.println("✅ DRV2605L found at address 0x5A");
  
  // Reset and configure DRV2605L
  writeRegister(DRV2605L_MODE, 0x00);          // Out of standby
  writeRegister(DRV2605L_RTPIN, 0x00);         // No real-time playback
  writeRegister(DRV2605L_LIBRARY, 1);          // Use ERM library
  writeRegister(DRV2605L_OVERDRIVE, 0x00);     // No overdrive
  writeRegister(DRV2605L_SUSTAINPOS, 0x00);    // Disable sustain
  writeRegister(DRV2605L_SUSTAINNEG, 0x00);    // Disable sustain
  writeRegister(DRV2605L_BREAK, 0x00);         // Disable brake
  writeRegister(DRV2605L_AUDIOCTRL, 0x00);     // Disable audio-to-haptic
  
  // Configure for LRA (Linear Resonant Actuator)
  writeRegister(DRV2605L_FEEDBACK, 0x80);      // LRA mode, auto-brake disabled
  writeRegister(DRV2605L_CONTROL1, 0x93);      // Drive time 2.4ms, sample time 300us
  writeRegister(DRV2605L_CONTROL2, 0xF5);      // Bidirectional input, brake stabilizer
  writeRegister(DRV2605L_CONTROL3, 0xA0);      // ERM open loop, NG threshold 2%
  
  // Set rated voltage and overdrive clamp (adjust for your LRA)
  writeRegister(DRV2605L_RATEDV, 0x3E);        // 1.2V rated (adjust as needed)
  writeRegister(DRV2605L_CLAMPV, 0x8C);        // 3.0V clamp (adjust as needed)
  
  // Run auto-calibration
  Serial.println("🔧 Running LRA auto-calibration...");
  runAutoCalibration();
  
  // Test haptic with startup effect
  playHapticEffect(EFFECT_DOUBLE_CLICK);
  
  Serial.println("✅ Haptic system initialized successfully!");
  Serial.print("   I2C SDA: GPIO");
  Serial.println(I2C_SDA);
  Serial.print("   I2C SCL: GPIO");
  Serial.println(I2C_SCL);
  Serial.print("   Intensity: ");
  Serial.print(hapticIntensity);
  Serial.println("%");
}

void setupButtons() {
  pinMode(BTN_MENU, INPUT_PULLUP);
  pinMode(BTN_PLAY, INPUT_PULLUP);
  pinMode(BTN_PREV, INPUT_PULLUP);
  pinMode(BTN_NEXT, INPUT_PULLUP);
  
  Serial.println("🔘 Haptic buttons configured:");
  Serial.print("   Menu: GPIO");
  Serial.println(BTN_MENU);
  Serial.print("   Play: GPIO");
  Serial.println(BTN_PLAY);
  Serial.print("   Prev: GPIO");
  Serial.println(BTN_PREV);
  Serial.print("   Next: GPIO");
  Serial.println(BTN_NEXT);
}

void handleButtons() {
  unsigned long currentTime = millis();
  
  // Menu button
  handleButton(BTN_MENU, menuBtn, currentTime, []() {
    Serial.println("🔘 Menu button pressed");
    playHapticEffect(EFFECT_SOFT_BUMP);
    handleMenu();
  });
  
  // Play button  
  handleButton(BTN_PLAY, playBtn, currentTime, []() {
    Serial.println("🔘 Play button pressed");
    playHapticEffect(EFFECT_SHARP_CLICK);
    handlePlay();
  });
  
  // Previous button
  handleButton(BTN_PREV, prevBtn, currentTime, []() {
    Serial.println("🔘 Previous button pressed");
    playHapticEffect(EFFECT_SOFT_BUMP);
    handlePrevious();
  });
  
  // Next button
  handleButton(BTN_NEXT, nextBtn, currentTime, []() {
    Serial.println("🔘 Next button pressed");
    playHapticEffect(EFFECT_SOFT_BUMP);
    handleNext();
  });
}

void handleButton(int pin, ButtonState &btn, unsigned long currentTime, void (*callback)()) {
  bool currentState = digitalRead(pin);
  
  // Debouncing
  if (currentState != btn.lastState) {
    btn.lastPressTime = currentTime;
  }
  
  if ((currentTime - btn.lastPressTime) > 50) { // 50ms debounce
    if (currentState == LOW && !btn.pressed) {
      btn.pressed = true;
      callback();
    } else if (currentState == HIGH) {
      btn.pressed = false;
    }
  }
  
  btn.lastState = currentState;
}

void handleMenu() {
  if (currentScreen != HOME) {
    currentScreen = HOME;
    selectedItem = 0;
    updateMaxItems();
    drawScreen();
  } else {
    // Already at home, show settings
    currentScreen = SETTINGS;
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

void navigateUp() {
  selectedItem--;
  if (selectedItem < 0) {
    selectedItem = maxItems - 1; // Wrap around
  }
  drawScreen();
}

void navigateDown() {
  selectedItem++;
  if (selectedItem >= maxItems) {
    selectedItem = 0; // Wrap around
  }
  drawScreen();
}

void togglePlayPause() {
  isPlaying = !isPlaying;
  Serial.print("🎵 ");
  Serial.println(isPlaying ? "Playing" : "Paused");
  
  // Different haptic feedback for play vs pause
  if (isPlaying) {
    playHapticEffect(EFFECT_DOUBLE_CLICK);
  } else {
    playHapticEffect(EFFECT_SHARP_CLICK);
  }
  
  if (currentScreen == NOW_PLAYING) {
    drawScreen();
  }
}

void previousTrack() {
  currentSong = (currentSong - 1 + 6) % 6;
  playHapticEffect(EFFECT_TRIPLE_CLICK);
  Serial.print("⏮️ Previous track: ");
  Serial.println(songs[currentSong]);
  drawScreen();
}

void nextTrack() {
  currentSong = (currentSong + 1) % 6;
  playHapticEffect(EFFECT_TRIPLE_CLICK);
  Serial.print("⏭️ Next track: ");
  Serial.println(songs[currentSong]);
  drawScreen();
}

void updateMaxItems() {
  switch (currentScreen) {
    case HOME: maxItems = 4; break;
    case LIBRARY: maxItems = 6; break;
    case NOW_PLAYING: maxItems = 1; break;
    case SETTINGS: maxItems = 4; break;
  }
}

// Haptic Functions
void playHapticEffect(HapticEffect effect) {
  if (!hapticEnabled) return;
  
  // Clear any previous effects
  writeRegister(DRV2605L_WAVESEQ1, 0x00);
  writeRegister(DRV2605L_WAVESEQ2, 0x00);
  writeRegister(DRV2605L_WAVESEQ3, 0x00);
  writeRegister(DRV2605L_WAVESEQ4, 0x00);
  
  // Set the effect
  writeRegister(DRV2605L_WAVESEQ1, effect);
  
  // Trigger the effect
  writeRegister(DRV2605L_GO, 0x01);
  
  Serial.print("🔄 Haptic effect: ");
  Serial.println(effect);
}

void playCustomHapticPattern(uint8_t effect1, uint8_t effect2 = 0, uint8_t effect3 = 0) {
  if (!hapticEnabled) return;
  
  writeRegister(DRV2605L_WAVESEQ1, effect1);
  writeRegister(DRV2605L_WAVESEQ2, effect2);
  writeRegister(DRV2605L_WAVESEQ3, effect3);
  writeRegister(DRV2605L_WAVESEQ4, 0x00);
  
  writeRegister(DRV2605L_GO, 0x01);
}

void runAutoCalibration() {
  writeRegister(DRV2605L_MODE, 0x07); // Auto-calibration mode
  writeRegister(DRV2605L_GO, 0x01);   // Start calibration
  
  // Wait for calibration to complete
  delay(1000);
  
  uint8_t status = readRegister(DRV2605L_STATUS);
  if (status & 0x08) {
    Serial.println("✅ Auto-calibration completed successfully");
  } else {
    Serial.println("⚠️ Auto-calibration may have failed");
  }
  
  // Return to normal mode
  writeRegister(DRV2605L_MODE, 0x00);
}

void writeRegister(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(DRV2605L_ADDR);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
}

uint8_t readRegister(uint8_t reg) {
  Wire.beginTransmission(DRV2605L_ADDR);
  Wire.write(reg);
  Wire.endTransmission();
  
  Wire.requestFrom(DRV2605L_ADDR, 1);
  return Wire.read();
}

// Display Functions
void showSplash() {
  tft.fillScreen(BLACK);
  
  // iPod logo with haptic
  tft.setTextSize(4);
  tft.setTextColor(WHITE);
  tft.setCursor(60, 120);
  tft.print("iPod");
  
  playHapticEffect(EFFECT_PULSING_SHARP);
  delay(500);
  
  tft.setTextSize(1);
  tft.setTextColor(CYAN);
  tft.setCursor(70, 160);
  tft.print("Haptic Interface");
  
  tft.setTextColor(YELLOW);
  tft.setCursor(80, 180);
  tft.print("Feel the Beat!");
  
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
  }
  
  drawStatusBar();
}

void drawHome() {
  // Title with haptic indicator
  tft.setTextSize(3);
  tft.setTextColor(WHITE);
  tft.setCursor(20, 20);
  tft.print("iPod");
  
  // Haptic indicator
  tft.setTextSize(1);
  tft.setTextColor(hapticEnabled ? GREEN : RED);
  tft.setCursor(150, 30);
  tft.print(hapticEnabled ? "Haptic ON" : "Haptic OFF");
  
  // Menu items
  for (int i = 0; i < 4; i++) {
    int y = 70 + (i * 35);
    
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
}

void drawLibrary() {
  // Header
  drawHeader("Music Library");
  
  // Song list
  for (int i = 0; i < 6; i++) {
    int y = 80 + (i * 30);
    
    // Highlight selected item
    if (i == selectedItem) {
      tft.fillRoundRect(5, y - 3, 230, 25, 3, BLUE);
    }
    
    // Playing indicator
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
    
    // Playing indicator with haptic symbol
    if (i == currentSong && isPlaying) {
      tft.setTextColor(GREEN);
      tft.setCursor(200, y + 6);
      tft.print("♪~");
    }
    
    // Selection arrow
    if (i == selectedItem) {
      tft.setTextColor(WHITE);
      tft.setCursor(220, y + 6);
      tft.print(">");
    }
  }
}

void drawNowPlaying() {
  // Header
  drawHeader("Now Playing");
  
  // Album art placeholder with haptic indicator
  tft.drawRoundRect(60, 80, 120, 120, 10, WHITE);
  tft.fillRoundRect(62, 82, 116, 116, 8, DARK_GRAY);
  
  // Music note with vibration lines
  tft.setTextSize(6);
  tft.setTextColor(GRAY);
  tft.setCursor(105, 125);
  tft.print("♪");
  
  if (isPlaying && hapticEnabled) {
    // Vibration lines
    tft.setTextSize(1);
    tft.setTextColor(YELLOW);
    tft.setCursor(85, 115);
    tft.print("~");
    tft.setCursor(85, 135);
    tft.print("~");
    tft.setCursor(85, 155);
    tft.print("~");
  }
  
  // Song info
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setCursor(20, 210);
  tft.print(songs[currentSong]);
  
  tft.setTextColor(GRAY);
  tft.setCursor(20, 225);
  tft.print(artists[currentSong]);
  
  // Play/Pause indicator with haptic status
  tft.setTextSize(2);
  tft.setTextColor(isPlaying ? GREEN : RED);
  tft.setCursor(20, 250);
  tft.print(isPlaying ? "Playing" : "Paused");
  
  if (hapticEnabled) {
    tft.setTextSize(1);
    tft.setTextColor(YELLOW);
    tft.setCursor(150, 255);
    tft.print("Feel it!");
  }
}

void drawSettings() {
  drawHeader("Settings");
  
  const char* settings[] = {"Haptic Feedback", "Display", "Audio", "About"};
  
  for (int i = 0; i < 4; i++) {
    int y = 80 + (i * 30);
    
    if (i == selectedItem) {
      tft.fillRoundRect(10, y - 5, 220, 25, 5, BLUE);
    }
    
    tft.setTextSize(1);
    tft.setTextColor(WHITE);
    tft.setCursor(20, y);
    tft.print(settings[i]);
    
    // Show haptic status
    if (i == 0) {
      tft.setTextColor(hapticEnabled ? GREEN : RED);
      tft.setCursor(150, y);
      tft.print(hapticEnabled ? "ON" : "OFF");
    }
    
    if (i == selectedItem) {
      tft.setTextColor(WHITE);
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
  
  // Back indicator
  tft.setTextSize(1);
  tft.setTextColor(BLUE);
  tft.setCursor(10, 50);
  tft.print("Menu: Back");
  
  // Separator line
  tft.drawLine(0, 65, 240, 65, GRAY);
}

void drawStatusBar() {
  // Bottom status
  tft.drawLine(0, 300, 240, 300, GRAY);
  
  tft.setTextSize(1);
  tft.setTextColor(GRAY);
  tft.setCursor(10, 308);
  tft.print("Prev/Next: Navigate");
  
  tft.setCursor(150, 308);
  tft.print("Play: Select");
  
  // Haptic indicator
  if (hapticEnabled) {
    tft.setTextColor(YELLOW);
    tft.setCursor(210, 308);
    tft.print("~");
  }
}

void updateUI() {
  // Any dynamic updates
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate < 1000) return;
  
  lastUpdate = millis();
}