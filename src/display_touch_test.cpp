// =====================================================
//  Complete Display + Touch Test for ESP32-S3 iPod
//  Works with both TFT_eSPI and Adafruit libraries
//  Choose one library by commenting/uncommenting
// =====================================================

// Choose your library (comment out the one you don't want)
#define USE_ADAFRUIT_LIBRARY    // Recommended - more stable
// #define USE_TFT_ESPI_LIBRARY    // Alternative - if Adafruit doesn't work

#ifdef USE_ADAFRUIT_LIBRARY
  #include <Adafruit_GFX.h>
  #include <Adafruit_ILI9341.h>
  #include <XPT2046_Touchscreen.h>
  #include <SPI.h>
  #include "adafruit_config.h"
  
  // Create display object
  Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST, TFT_MISO);
  
  // Create touch object
  XPT2046_Touchscreen touch(TOUCH_CS, TOUCH_IRQ);
  
#endif

#ifdef USE_TFT_ESPI_LIBRARY
  #include <TFT_eSPI.h>
  #include <XPT2046_Touchscreen.h>
  #include <SPI.h>
  
  // Create display object (uses User_Setup.h)
  TFT_eSPI tft = TFT_eSPI();
  
  // Create touch object
  XPT2046_Touchscreen touch(6, 3);  // CS=6, IRQ=3
  
#endif

// Test variables
int testPhase = 0;
unsigned long lastUpdate = 0;
bool touchPressed = false;
int touchX = 0, touchY = 0;

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("=== ESP32-S3 iPod Display + Touch Test ===");
  Serial.println("MSP2806 - ILI9341 Display + XPT2046 Touch");
  Serial.println("");
  
  // Print wiring information
  Serial.println("Wiring Check:");
  Serial.println("ESP32-S3 → MSP2806 Display");
  Serial.println("GPIO10 → CS (Display)");
  Serial.println("GPIO9  → DC (Display)");
  Serial.println("GPIO8  → RST (Display)");
  Serial.println("GPIO11 → SDA/MOSI (Display)");
  Serial.println("GPIO12 → SCL/SCLK (Display)");
  Serial.println("GPIO13 → SDO/MISO (Display) [optional]");
  Serial.println("GPIO6  → T_CS (Touch)");
  Serial.println("GPIO7  → T_CLK (Touch)");
  Serial.println("GPIO5  → T_DIN (Touch)");
  Serial.println("GPIO4  → T_DO (Touch)");
  Serial.println("GPIO3  → T_IRQ (Touch) [optional]");
  Serial.println("3.3V   → VCC + LED");
  Serial.println("GND    → GND");
  Serial.println("");
  
  // Initialize display
  Serial.println("Initializing display...");
  
#ifdef USE_ADAFRUIT_LIBRARY
  tft.begin();
  Serial.println("Adafruit ILI9341 initialized");
#endif

#ifdef USE_TFT_ESPI_LIBRARY
  tft.init();
  Serial.println("TFT_eSPI initialized");
#endif
  
  tft.setRotation(0);  // Portrait mode
  Serial.println("Display rotation set to portrait");
  
  // Initialize touch
  Serial.println("Initializing touch...");
  touch.begin();
  if (touch.tirqTouched()) {
    Serial.println("Touch controller detected!");
  } else {
    Serial.println("Touch controller not detected - check wiring");
  }
  
  // Clear screen
#ifdef USE_ADAFRUIT_LIBRARY
  tft.fillScreen(ILI9341_BLACK);
#endif
#ifdef USE_TFT_ESPI_LIBRARY
  tft.fillScreen(TFT_BLACK);
#endif
  
  Serial.println("Starting display tests...");
  Serial.println("Touch the screen to advance through tests");
  
  runDisplayTest();
}

void loop() {
  // Check for touch
  if (touch.touched()) {
    TS_Point p = touch.getPoint();
    
    // Map touch coordinates to screen coordinates
    touchX = map(p.x, 200, 3800, 0, 240);
    touchY = map(p.y, 200, 3800, 0, 320);
    
    // Ensure coordinates are within screen bounds
    touchX = constrain(touchX, 0, 239);
    touchY = constrain(touchY, 0, 319);
    
    if (!touchPressed) {
      touchPressed = true;
      Serial.println("Touch detected at (" + String(touchX) + ", " + String(touchY) + ")");
      
      // Advance to next test
      testPhase++;
      if (testPhase > 8) testPhase = 0;
      runDisplayTest();
    }
  } else {
    touchPressed = false;
  }
  
  // Auto-advance tests every 3 seconds if no touch
  if (millis() - lastUpdate > 3000) {
    testPhase++;
    if (testPhase > 8) testPhase = 0;
    runDisplayTest();
    lastUpdate = millis();
  }
  
  delay(50);
}

void runDisplayTest() {
  lastUpdate = millis();
  
  Serial.println("Running test phase " + String(testPhase));
  
  switch (testPhase) {
    case 0:
      testColorFill();
      break;
    case 1:
      testText();
      break;
    case 2:
      testShapes();
      break;
    case 3:
      testLines();
      break;
    case 4:
      testCircles();
      break;
    case 5:
      testRectangles();
      break;
    case 6:
      testColorBars();
      break;
    case 7:
      testTouchResponse();
      break;
    case 8:
      testIPodInterface();
      break;
  }
}

void testColorFill() {
#ifdef USE_ADAFRUIT_LIBRARY
  tft.fillScreen(ILI9341_RED);
  delay(500);
  tft.fillScreen(ILI9341_GREEN);
  delay(500);
  tft.fillScreen(ILI9341_BLUE);
  delay(500);
  tft.fillScreen(ILI9341_BLACK);
#endif

#ifdef USE_TFT_ESPI_LIBRARY
  tft.fillScreen(TFT_RED);
  delay(500);
  tft.fillScreen(TFT_GREEN);
  delay(500);
  tft.fillScreen(TFT_BLUE);
  delay(500);
  tft.fillScreen(TFT_BLACK);
#endif
  
  Serial.println("Color fill test complete");
}

void testText() {
#ifdef USE_ADAFRUIT_LIBRARY
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println("ESP32-S3");
  tft.println("iPod Project");
  tft.setTextColor(ILI9341_YELLOW);
  tft.println("");
  tft.println("Display: OK");
  tft.setTextColor(ILI9341_GREEN);
  tft.println("Touch: Testing");
  tft.setTextColor(ILI9341_CYAN);
  tft.println("");
  tft.println("Adafruit Lib");
#endif

#ifdef USE_TFT_ESPI_LIBRARY
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println("ESP32-S3");
  tft.println("iPod Project");
  tft.setTextColor(TFT_YELLOW);
  tft.println("");
  tft.println("Display: OK");
  tft.setTextColor(TFT_GREEN);
  tft.println("Touch: Testing");
  tft.setTextColor(TFT_CYAN);
  tft.println("");
  tft.println("TFT_eSPI Lib");
#endif
  
  Serial.println("Text test complete");
}

void testShapes() {
#ifdef USE_ADAFRUIT_LIBRARY
  tft.fillScreen(ILI9341_BLACK);
  tft.drawRect(10, 10, 100, 60, ILI9341_RED);
  tft.fillRect(15, 15, 90, 50, ILI9341_BLUE);
  tft.drawCircle(180, 40, 30, ILI9341_GREEN);
  tft.fillCircle(180, 40, 25, ILI9341_YELLOW);
  tft.drawTriangle(60, 100, 30, 150, 90, 150, ILI9341_MAGENTA);
  tft.fillTriangle(65, 105, 40, 145, 85, 145, ILI9341_CYAN);
#endif

#ifdef USE_TFT_ESPI_LIBRARY
  tft.fillScreen(TFT_BLACK);
  tft.drawRect(10, 10, 100, 60, TFT_RED);
  tft.fillRect(15, 15, 90, 50, TFT_BLUE);
  tft.drawCircle(180, 40, 30, TFT_GREEN);
  tft.fillCircle(180, 40, 25, TFT_YELLOW);
  tft.drawTriangle(60, 100, 30, 150, 90, 150, TFT_MAGENTA);
  tft.fillTriangle(65, 105, 40, 145, 85, 145, TFT_CYAN);
#endif
  
  Serial.println("Shapes test complete");
}

void testLines() {
#ifdef USE_ADAFRUIT_LIBRARY
  tft.fillScreen(ILI9341_BLACK);
  for (int i = 0; i < 240; i += 20) {
    tft.drawLine(0, 0, i, 319, ILI9341_RED);
    tft.drawLine(239, 0, 239-i, 319, ILI9341_GREEN);
  }
#endif

#ifdef USE_TFT_ESPI_LIBRARY
  tft.fillScreen(TFT_BLACK);
  for (int i = 0; i < 240; i += 20) {
    tft.drawLine(0, 0, i, 319, TFT_RED);
    tft.drawLine(239, 0, 239-i, 319, TFT_GREEN);
  }
#endif
  
  Serial.println("Lines test complete");
}

void testCircles() {
#ifdef USE_ADAFRUIT_LIBRARY
  tft.fillScreen(ILI9341_BLACK);
  for (int r = 10; r < 120; r += 10) {
    tft.drawCircle(120, 160, r, random(0xFFFF));
  }
#endif

#ifdef USE_TFT_ESPI_LIBRARY
  tft.fillScreen(TFT_BLACK);
  for (int r = 10; r < 120; r += 10) {
    tft.drawCircle(120, 160, r, random(0xFFFF));
  }
#endif
  
  Serial.println("Circles test complete");
}

void testRectangles() {
#ifdef USE_ADAFRUIT_LIBRARY
  tft.fillScreen(ILI9341_BLACK);
  for (int i = 0; i < 50; i++) {
    int x = random(240);
    int y = random(320);
    int w = random(50);
    int h = random(50);
    tft.fillRect(x, y, w, h, random(0xFFFF));
  }
#endif

#ifdef USE_TFT_ESPI_LIBRARY
  tft.fillScreen(TFT_BLACK);
  for (int i = 0; i < 50; i++) {
    int x = random(240);
    int y = random(320);
    int w = random(50);
    int h = random(50);
    tft.fillRect(x, y, w, h, random(0xFFFF));
  }
#endif
  
  Serial.println("Rectangles test complete");
}

void testColorBars() {
#ifdef USE_ADAFRUIT_LIBRARY
  tft.fillScreen(ILI9341_BLACK);
  uint16_t colors[] = {ILI9341_RED, ILI9341_GREEN, ILI9341_BLUE, ILI9341_YELLOW, ILI9341_MAGENTA, ILI9341_CYAN};
  for (int i = 0; i < 6; i++) {
    tft.fillRect(i * 40, 50, 40, 220, colors[i]);
  }
#endif

#ifdef USE_TFT_ESPI_LIBRARY
  tft.fillScreen(TFT_BLACK);
  uint16_t colors[] = {TFT_RED, TFT_GREEN, TFT_BLUE, TFT_YELLOW, TFT_MAGENTA, TFT_CYAN};
  for (int i = 0; i < 6; i++) {
    tft.fillRect(i * 40, 50, 40, 220, colors[i]);
  }
#endif
  
  Serial.println("Color bars test complete");
}

void testTouchResponse() {
#ifdef USE_ADAFRUIT_LIBRARY
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println("Touch Test");
  tft.println("");
  tft.println("Touch anywhere");
  tft.println("on the screen");
  
  if (touchPressed) {
    tft.fillCircle(touchX, touchY, 10, ILI9341_RED);
    tft.setTextColor(ILI9341_GREEN);
    tft.setCursor(10, 200);
    tft.println("Touch at:");
    tft.println("X: " + String(touchX));
    tft.println("Y: " + String(touchY));
  }
#endif

#ifdef USE_TFT_ESPI_LIBRARY
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println("Touch Test");
  tft.println("");
  tft.println("Touch anywhere");
  tft.println("on the screen");
  
  if (touchPressed) {
    tft.fillCircle(touchX, touchY, 10, TFT_RED);
    tft.setTextColor(TFT_GREEN);
    tft.setCursor(10, 200);
    tft.println("Touch at:");
    tft.println("X: " + String(touchX));
    tft.println("Y: " + String(touchY));
  }
#endif
  
  Serial.println("Touch response test active");
}

void testIPodInterface() {
#ifdef USE_ADAFRUIT_LIBRARY
  tft.fillScreen(ILI9341_BLACK);
  
  // Status bar
  tft.fillRect(0, 0, 240, 20, ILI9341_BLUE);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(1);
  tft.setCursor(5, 6);
  tft.println("iPod  12:34  100%");
  
  // Now playing
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.setCursor(10, 40);
  tft.println("Now Playing");
  
  // Album art placeholder
  tft.drawRect(60, 70, 120, 120, ILI9341_WHITE);
  tft.setTextColor(ILI9341_GRAY);
  tft.setTextSize(1);
  tft.setCursor(100, 125);
  tft.println("Album");
  tft.setCursor(105, 135);
  tft.println("Art");
  
  // Song info
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(1);
  tft.setCursor(10, 210);
  tft.println("Song Title");
  tft.setCursor(10, 225);
  tft.println("Artist Name");
  tft.setCursor(10, 240);
  tft.println("Album Name");
  
  // Progress bar
  tft.drawRect(10, 260, 220, 10, ILI9341_WHITE);
  tft.fillRect(11, 261, 80, 8, ILI9341_BLUE);
  
  // Controls
  tft.setTextSize(2);
  tft.setCursor(50, 285);
  tft.println("<<  ||  >>");
#endif

#ifdef USE_TFT_ESPI_LIBRARY
  tft.fillScreen(TFT_BLACK);
  
  // Status bar
  tft.fillRect(0, 0, 240, 20, TFT_BLUE);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);
  tft.setCursor(5, 6);
  tft.println("iPod  12:34  100%");
  
  // Now playing
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setCursor(10, 40);
  tft.println("Now Playing");
  
  // Album art placeholder
  tft.drawRect(60, 70, 120, 120, TFT_WHITE);
  tft.setTextColor(TFT_DARKGREY);
  tft.setTextSize(1);
  tft.setCursor(100, 125);
  tft.println("Album");
  tft.setCursor(105, 135);
  tft.println("Art");
  
  // Song info
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);
  tft.setCursor(10, 210);
  tft.println("Song Title");
  tft.setCursor(10, 225);
  tft.println("Artist Name");
  tft.setCursor(10, 240);
  tft.println("Album Name");
  
  // Progress bar
  tft.drawRect(10, 260, 220, 10, TFT_WHITE);
  tft.fillRect(11, 261, 80, 8, TFT_BLUE);
  
  // Controls
  tft.setTextSize(2);
  tft.setCursor(50, 285);
  tft.println("<<  ||  >>");
#endif
  
  Serial.println("iPod interface test complete");
}