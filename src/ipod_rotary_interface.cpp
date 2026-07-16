/*
ESP32-S3 iPod with Rotary Encoder Interface
Classic iPod-style navigation with scroll wheel and buttons
*/

#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

// Rotary Encoder Pins (using available GPIOs)
#define ENCODER_CLK  0    // Rotary encoder clock
#define ENCODER_DT   1    // Rotary encoder data  
#define ENCODER_SW   2    // Rotary encoder switch (center button)

// Navigation Buttons (iPod-style)
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

// UI State
enum Screen { HOME, LIBRARY, NOW_PLAYING, SETTINGS };
Screen currentScreen = HOME;
int selectedItem = 0;
int maxItems = 4;
bool isPlaying = false;
int volume = 50;
int currentSong = 0;

// Encoder state
volatile int encoderPos = 0;
volatile bool encoderChanged = false;
int lastEncoderPos = 0;

// Button state
bool lastMenuState = HIGH;
bool lastPlayState = HIGH;
bool lastPrevState = HIGH;
bool lastNextState = HIGH;
unsigned long lastButtonTime = 0;

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
  Serial.println("║    ESP32-S3 iPod Rotary Interface     ║");
  Serial.println("║      Classic iPod Navigation          ║");
  Serial.println("╚════════════════════════════════════════╝");
  Serial.println();
  
  // Initialize display
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(BLACK);
  
  // Setup rotary encoder
  setupEncoder();
  
  // Setup buttons
  setupButtons();
  
  // Show splash
  showSplash();
  
  // Start with home screen
  currentScreen = HOME;
  updateMaxItems();
  drawScreen();
  
  Serial.println("🎮 iPod Interface Ready!");
  Serial.println("   Rotary Encoder: Scroll through items");
  Serial.println("   Center Button: Select item");
  Serial.println("   Menu Button: Go back");
  Serial.println("   Play Button: Play/Pause");
  Serial.println("   Prev/Next: Change tracks");
  Serial.println();
}

void loop() {
  handleEncoder();
  handleButtons();
  updateUI();
  delay(10);
}

void setupEncoder() {
  pinMode(ENCODER_CLK, INPUT_PULLUP);
  pinMode(ENCODER_DT, INPUT_PULLUP);
  pinMode(ENCODER_SW, INPUT_PULLUP);
  
  // Attach interrupts for smooth encoder reading
  attachInterrupt(digitalPinToInterrupt(ENCODER_CLK), encoderISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_DT), encoderISR, CHANGE);
  
  Serial.println("🔄 Rotary encoder configured:");
  Serial.print("   CLK: GPIO");
  Serial.println(ENCODER_CLK);
  Serial.print("   DT:  GPIO");
  Serial.println(ENCODER_DT);
  Serial.print("   SW:  GPIO");
  Serial.println(ENCODER_SW);
}

void setupButtons() {
  pinMode(BTN_MENU, INPUT_PULLUP);
  pinMode(BTN_PLAY, INPUT_PULLUP);
  pinMode(BTN_PREV, INPUT_PULLUP);
  pinMode(BTN_NEXT, INPUT_PULLUP);
  
  Serial.println("🔘 Navigation buttons configured:");
  Serial.print("   Menu: GPIO");
  Serial.println(BTN_MENU);
  Serial.print("   Play: GPIO");
  Serial.println(BTN_PLAY);
  Serial.print("   Prev: GPIO");
  Serial.println(BTN_PREV);
  Serial.print("   Next: GPIO");
  Serial.println(BTN_NEXT);
}

void IRAM_ATTR encoderISR() {
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();
  
  // Debounce
  if (interruptTime - lastInterruptTime > 5) {
    bool clkState = digitalRead(ENCODER_CLK);
    bool dtState = digitalRead(ENCODER_DT);
    
    if (clkState != dtState) {
      encoderPos++;
    } else {
      encoderPos--;
    }
    
    encoderChanged = true;
    lastInterruptTime = interruptTime;
  }
}

void handleEncoder() {
  if (encoderChanged) {
    encoderChanged = false;
    
    int change = encoderPos - lastEncoderPos;
    lastEncoderPos = encoderPos;
    
    if (change != 0) {
      Serial.print("🔄 Encoder: ");
      Serial.print(change > 0 ? "CW" : "CCW");
      Serial.print(" (");
      Serial.print(change);
      Serial.println(")");
      
      // Handle encoder movement based on current screen
      if (change > 0) {
        navigateDown();
      } else {
        navigateUp();
      }
    }
  }
  
  // Handle encoder button (center select)
  static bool lastSwState = HIGH;
  bool swState = digitalRead(ENCODER_SW);
  
  if (swState == LOW && lastSwState == HIGH) {
    delay(50); // Debounce
    if (digitalRead(ENCODER_SW) == LOW) {
      Serial.println("🔘 Encoder button pressed (SELECT)");
      handleSelect();
    }
  }
  lastSwState = swState;
}

void handleButtons() {
  unsigned long currentTime = millis();
  if (currentTime - lastButtonTime < 50) return; // Debounce
  
  // Menu button
  bool menuState = digitalRead(BTN_MENU);
  if (menuState == LOW && lastMenuState == HIGH) {
    Serial.println("🔘 Menu button pressed");
    handleMenu();
    lastButtonTime = currentTime;
  }
  lastMenuState = menuState;
  
  // Play button
  bool playState = digitalRead(BTN_PLAY);
  if (playState == LOW && lastPlayState == HIGH) {
    Serial.println("🔘 Play button pressed");
    handlePlay();
    lastButtonTime = currentTime;
  }
  lastPlayState = playState;
  
  // Previous button
  bool prevState = digitalRead(BTN_PREV);
  if (prevState == LOW && lastPrevState == HIGH) {
    Serial.println("🔘 Previous button pressed");
    handlePrevious();
    lastButtonTime = currentTime;
  }
  lastPrevState = prevState;
  
  // Next button
  bool nextState = digitalRead(BTN_NEXT);
  if (nextState == LOW && lastNextState == HIGH) {
    Serial.println("🔘 Next button pressed");
    handleNext();
    lastButtonTime = currentTime;
  }
  lastNextState = nextState;
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
      selectedItem = 0;
      updateMaxItems();
      drawScreen();
      break;
      
    case NOW_PLAYING:
      // Handle now playing selections (play/pause, etc.)
      if (selectedItem == 0) {
        togglePlayPause();
      }
      break;
      
    case SETTINGS:
      // Handle settings selections
      break;
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
  togglePlayPause();
}

void handlePrevious() {
  if (currentScreen == NOW_PLAYING) {
    currentSong = (currentSong - 1 + 6) % 6;
    drawScreen();
  } else {
    navigateUp();
  }
}

void handleNext() {
  if (currentScreen == NOW_PLAYING) {
    currentSong = (currentSong + 1) % 6;
    drawScreen();
  } else {
    navigateDown();
  }
}

void togglePlayPause() {
  isPlaying = !isPlaying;
  Serial.print("🎵 ");
  Serial.println(isPlaying ? "Playing" : "Paused");
  if (currentScreen == NOW_PLAYING) {
    drawScreen();
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

void showSplash() {
  tft.fillScreen(BLACK);
  
  // iPod logo
  tft.setTextSize(4);
  tft.setTextColor(WHITE);
  tft.setCursor(60, 120);
  tft.print("iPod");
  
  tft.setTextSize(1);
  tft.setTextColor(CYAN);
  tft.setCursor(70, 160);
  tft.print("Rotary Interface");
  
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
  
  drawScrollIndicator();
}

void drawHome() {
  // Title
  tft.setTextSize(3);
  tft.setTextColor(WHITE);
  tft.setCursor(20, 20);
  tft.print("iPod");
  
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
  
  drawStatusBar();
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
    
    // Playing indicator
    if (i == currentSong && isPlaying) {
      tft.setTextColor(GREEN);
      tft.setCursor(210, y + 6);
      tft.print("♪");
    }
    
    // Selection arrow
    if (i == selectedItem) {
      tft.setTextColor(WHITE);
      tft.setCursor(220, y + 6);
      tft.print(">");
    }
  }
  
  drawStatusBar();
}

void drawNowPlaying() {
  // Header
  drawHeader("Now Playing");
  
  // Album art placeholder
  tft.drawRoundRect(60, 80, 120, 120, 10, WHITE);
  tft.fillRoundRect(62, 82, 116, 116, 8, DARK_GRAY);
  
  // Music note
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
  
  // Play/Pause indicator
  tft.setTextSize(2);
  tft.setTextColor(isPlaying ? GREEN : RED);
  tft.setCursor(20, 250);
  tft.print(isPlaying ? "Playing" : "Paused");
  
  drawStatusBar();
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
    
    if (i == selectedItem) {
      tft.setCursor(200, y);
      tft.print(">");
    }
  }
  
  drawStatusBar();
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

void drawScrollIndicator() {
  // Show scroll position
  if (maxItems > 1) {
    int barHeight = 50;
    int barY = 150;
    int indicatorHeight = barHeight / maxItems;
    int indicatorY = barY + (selectedItem * indicatorHeight);
    
    // Scroll bar background
    tft.drawRect(230, barY, 8, barHeight, GRAY);
    
    // Scroll indicator
    tft.fillRect(231, indicatorY, 6, indicatorHeight, WHITE);
  }
}

void drawStatusBar() {
  // Bottom status
  tft.drawLine(0, 300, 240, 300, GRAY);
  
  tft.setTextSize(1);
  tft.setTextColor(GRAY);
  tft.setCursor(10, 308);
  tft.print("Rotary: Navigate");
  
  tft.setCursor(150, 308);
  tft.print("Select: OK");
}

void updateUI() {
  // Any dynamic updates can go here
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate < 1000) return;
  
  // Update any time-based elements
  lastUpdate = millis();
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
  tft.print("Rotary Interface v1.0");
  tft.setCursor(20, 140);
  tft.print("Classic iPod Navigation");
  
  tft.setTextColor(BLUE);
  tft.setCursor(20, 180);
  tft.print("Press Menu to return");
  
  // Wait for menu button
  while (digitalRead(BTN_MENU) == HIGH) {
    delay(50);
  }
  
  delay(200); // Debounce
  currentScreen = HOME;
  drawScreen();
}