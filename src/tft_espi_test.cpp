// =====================================================
//  TFT_eSPI Test for ESP32-S3 iPod Project
//  Uses TFT_eSPI library - more optimized for ESP32
//  Requires User_Setup.h configuration
// =====================================================

#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include <SPI.h>

// Touch pins (TFT_eSPI handles display pins via User_Setup.h)
#define TOUCH_CS  6
#define TOUCH_IRQ 3

// Create objects
TFT_eSPI tft = TFT_eSPI();
XPT2046_Touchscreen touch(TOUCH_CS, TOUCH_IRQ);

// Test variables
int testPhase = 0;
unsigned long lastTest = 0;
bool touchActive = false;

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("=== TFT_eSPI ESP32-S3 iPod Test ===");
  Serial.println("Display: MSP2806 (ILI9341 + XPT2046)");
  Serial.println("Library: TFT_eSPI (Optimized)");
  Serial.println("");
  
  // Print configuration info
  Serial.println("Configuration Check:");
  Serial.println("- User_Setup.h should be configured");
  Serial.println("- ILI9341_DRIVER should be defined");
  Serial.println("- Pin assignments should match wiring");
  Serial.println("");
  
  // Initialize TFT
  Serial.println("Initializing TFT display...");
  tft.init();
  tft.setRotation(0);  // Portrait mode
  Serial.println("TFT initialized successfully!");
  
  // Initialize touch
  Serial.println("Initializing touch controller...");
  touch.begin();
  if (touch.tirqTouched()) {
    Serial.println("Touch controller detected!");
  } else {
    Serial.println("Touch controller not responding - check wiring");
  }
  Serial.println("");
  
  // Clear screen
  tft.fillScreen(TFT_BLACK);
  
  // Show startup message
  showStartupMessage();
  
  Serial.println("Starting automated tests...");
  Serial.println("Touch screen to advance manually");
}

void loop() {
  // Check for touch input
  if (touch.touched()) {
    TS_Point p = touch.getPoint();
    
    if (!touchActive) {
      touchActive = true;
      
      // Map touch coordinates to screen
      int x = map(p.x, 200, 3800, 0, 240);
      int y = map(p.y, 200, 3800, 0, 320);
      x = constrain(x, 0, 239);
      y = constrain(y, 0, 319);
      
      Serial.println("Touch detected at (" + String(x) + ", " + String(y) + ")");
      
      // Advance to next test
      nextTest();
    }
  } else {
    touchActive = false;
  }
  
  // Auto-advance every 3 seconds
  if (millis() - lastTest > 3000) {
    nextTest();
  }
  
  delay(50);
}

void nextTest() {
  testPhase++;
  if (testPhase > 9) testPhase = 0;
  
  runTest(testPhase);
  lastTest = millis();
}

void runTest(int phase) {
  Serial.println("Running test " + String(phase) + "...");
  
  switch (phase) {
    case 0: testBasicColors(); break;
    case 1: testText(); break;
    case 2: testShapes(); break;
    case 3: testLines(); break;
    case 4: testCircles(); break;
    case 5: testGradients(); break;
    case 6: testSprites(); break;
    case 7: testTouchResponse(); break;
    case 8: testIPodInterface(); break;
    case 9: testPerformance(); break;
  }
}

void showStartupMessage() {
  tft.fillScreen(TFT_BLACK);
  
  // Title
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(20, 50);
  tft.println("ESP32-S3");
  tft.setCursor(30, 80);
  tft.println("iPod Test");
  
  // Library info
  tft.setTextColor(TFT_YELLOW);
  tft.setTextSize(1);
  tft.setCursor(10, 120);
  tft.println("Library: TFT_eSPI");
  tft.setCursor(10, 135);
  tft.println("Display: ILI9341");
  tft.setCursor(10, 150);
  tft.println("Touch: XPT2046");
  
  // Status
  tft.setTextColor(TFT_GREEN);
  tft.setCursor(10, 180);
  tft.println("Status: Initializing...");
  
  // Instructions
  tft.setTextColor(TFT_CYAN);
  tft.setCursor(10, 220);
  tft.println("Touch screen to advance");
  tft.setCursor(10, 235);
  tft.println("Auto-advance: 3 seconds");
  
  delay(2000);
}

void testBasicColors() {
  Serial.println("Testing basic colors...");
  
  // Solid colors
  tft.fillScreen(TFT_RED);
  delay(300);
  tft.fillScreen(TFT_GREEN);
  delay(300);
  tft.fillScreen(TFT_BLUE);
  delay(300);
  tft.fillScreen(TFT_YELLOW);
  delay(300);
  tft.fillScreen(TFT_MAGENTA);
  delay(300);
  tft.fillScreen(TFT_CYAN);
  delay(300);
  tft.fillScreen(TFT_BLACK);
}

void testText() {
  Serial.println("Testing text rendering...");
  
  tft.fillScreen(TFT_BLACK);
  
  // Different text sizes
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);
  tft.setCursor(10, 10);
  tft.println("Size 1: Small text");
  
  tft.setTextSize(2);
  tft.setCursor(10, 30);
  tft.println("Size 2: Medium");
  
  tft.setTextSize(3);
  tft.setCursor(10, 60);
  tft.println("Size 3");
  
  // Colored text
  tft.setTextSize(2);
  tft.setTextColor(TFT_RED);
  tft.setCursor(10, 110);
  tft.println("Red Text");
  
  tft.setTextColor(TFT_GREEN);
  tft.setCursor(10, 140);
  tft.println("Green Text");
  
  tft.setTextColor(TFT_BLUE);
  tft.setCursor(10, 170);
  tft.println("Blue Text");
  
  // Background text
  tft.setTextColor(TFT_YELLOW, TFT_RED);
  tft.setCursor(10, 200);
  tft.println("Background");
}

void testShapes() {
  Serial.println("Testing shapes...");
  
  tft.fillScreen(TFT_BLACK);
  
  // Rectangles
  tft.drawRect(10, 10, 80, 60, TFT_RED);
  tft.fillRect(15, 15, 70, 50, TFT_BLUE);
  
  // Circles
  tft.drawCircle(180, 40, 35, TFT_GREEN);
  tft.fillCircle(180, 40, 30, TFT_YELLOW);
  tft.fillCircle(180, 40, 20, TFT_MAGENTA);
  
  // Triangles
  tft.drawTriangle(50, 100, 20, 150, 80, 150, TFT_CYAN);
  tft.fillTriangle(55, 105, 30, 145, 75, 145, TFT_WHITE);
  
  // Rounded rectangles
  tft.drawRoundRect(120, 100, 100, 60, 10, TFT_ORANGE);
  tft.fillRoundRect(125, 105, 90, 50, 8, TFT_PINK);
}

void testLines() {
  Serial.println("Testing lines...");
  
  tft.fillScreen(TFT_BLACK);
  
  // Radial lines
  int centerX = 120;
  int centerY = 160;
  
  for (int angle = 0; angle < 360; angle += 15) {
    float rad = angle * PI / 180;
    int x = centerX + cos(rad) * 80;
    int y = centerY + sin(rad) * 80;
    
    uint16_t color = tft.color565(
      255 * (sin(rad * 2) + 1) / 2,
      255 * (cos(rad * 3) + 1) / 2,
      255 * (sin(rad * 5) + 1) / 2
    );
    
    tft.drawLine(centerX, centerY, x, y, color);
  }
}

void testCircles() {
  Serial.println("Testing circles...");
  
  tft.fillScreen(TFT_BLACK);
  
  // Concentric circles
  for (int r = 10; r < 120; r += 10) {
    uint16_t color = tft.color565(
      255 - (r * 2),
      r * 2,
      128 + (r / 2)
    );
    tft.drawCircle(120, 160, r, color);
  }
  
  // Filled circles
  for (int i = 0; i < 20; i++) {
    int x = random(240);
    int y = random(320);
    int r = random(20) + 5;
    uint16_t color = random(0xFFFF);
    tft.fillCircle(x, y, r, color);
  }
}

void testGradients() {
  Serial.println("Testing gradients...");
  
  tft.fillScreen(TFT_BLACK);
  
  // Horizontal gradient
  for (int x = 0; x < 240; x++) {
    uint16_t color = tft.color565(x, 0, 255 - x);
    tft.drawFastVLine(x, 20, 60, color);
  }
  
  // Vertical gradient
  for (int y = 0; y < 80; y++) {
    uint16_t color = tft.color565(0, y * 3, 255 - (y * 3));
    tft.drawFastHLine(20, 100 + y, 200, color);
  }
  
  // Radial gradient
  for (int r = 0; r < 60; r++) {
    uint16_t color = tft.color565(255 - (r * 4), r * 4, 128);
    tft.drawCircle(120, 240, r, color);
  }
}

void testSprites() {
  Serial.println("Testing sprites/fast drawing...");
  
  tft.fillScreen(TFT_BLACK);
  
  // Fast rectangles
  for (int i = 0; i < 100; i++) {
    int x = random(200);
    int y = random(280);
    int w = random(40) + 10;
    int h = random(40) + 10;
    uint16_t color = random(0xFFFF);
    
    tft.fillRect(x, y, w, h, color);
    delay(20);
  }
}

void testTouchResponse() {
  Serial.println("Testing touch response...");
  
  tft.fillScreen(TFT_BLACK);
  
  // Instructions
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println("Touch Test");
  
  tft.setTextSize(1);
  tft.setCursor(10, 50);
  tft.println("Touch anywhere on screen");
  tft.setCursor(10, 65);
  tft.println("Red circles show touch points");
  
  // Draw touch grid
  tft.setTextColor(TFT_DARKGREY);
  for (int x = 0; x < 240; x += 40) {
    tft.drawFastVLine(x, 100, 200, TFT_DARKGREY);
  }
  for (int y = 100; y < 300; y += 40) {
    tft.drawFastHLine(0, y, 240, TFT_DARKGREY);
  }
  
  // Show coordinates if touched
  if (touch.touched()) {
    TS_Point p = touch.getPoint();
    int x = map(p.x, 200, 3800, 0, 240);
    int y = map(p.y, 200, 3800, 0, 320);
    x = constrain(x, 0, 239);
    y = constrain(y, 0, 319);
    
    // Draw touch point
    tft.fillCircle(x, y, 8, TFT_RED);
    tft.drawCircle(x, y, 12, TFT_WHITE);
    
    // Show coordinates
    tft.fillRect(10, 300, 220, 20, TFT_BLACK);
    tft.setTextColor(TFT_GREEN);
    tft.setCursor(10, 305);
    tft.println("Touch: (" + String(x) + ", " + String(y) + ")");
  }
}

void testIPodInterface() {
  Serial.println("Testing iPod interface...");
  
  tft.fillScreen(TFT_BLACK);
  
  // Status bar
  tft.fillRect(0, 0, 240, 20, TFT_BLUE);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);
  tft.setCursor(5, 6);
  tft.print("iPod");
  tft.setCursor(100, 6);
  tft.print("12:34");
  tft.setCursor(200, 6);
  tft.print("100%");
  
  // Now Playing
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setCursor(10, 35);
  tft.println("Now Playing");
  
  // Album art
  tft.drawRoundRect(50, 70, 140, 140, 10, TFT_WHITE);
  tft.fillRoundRect(55, 75, 130, 130, 8, TFT_DARKGREY);
  
  // Music note symbol
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(4);
  tft.setCursor(110, 130);
  tft.print("♪");
  
  // Song info
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(10, 230);
  tft.println("Bohemian Rhapsody");
  tft.setCursor(10, 245);
  tft.println("Queen");
  tft.setCursor(10, 260);
  tft.println("A Night at the Opera");
  
  // Progress bar
  tft.drawRoundRect(10, 280, 220, 8, 3, TFT_WHITE);
  tft.fillRoundRect(11, 281, 80, 6, 2, TFT_BLUE);
  
  // Time
  tft.setCursor(10, 295);
  tft.println("2:15");
  tft.setCursor(200, 295);
  tft.println("5:55");
  
  // Control buttons
  tft.drawCircle(60, 315, 15, TFT_WHITE);   // Previous
  tft.drawCircle(120, 315, 20, TFT_WHITE);  // Play/Pause
  tft.drawCircle(180, 315, 15, TFT_WHITE);  // Next
  
  // Button symbols
  tft.setTextSize(1);
  tft.setCursor(55, 312);
  tft.print("<<");
  tft.setCursor(115, 310);
  tft.print("||");
  tft.setCursor(175, 312);
  tft.print(">>");
}

void testPerformance() {
  Serial.println("Testing performance...");
  
  unsigned long startTime = millis();
  
  // Clear screen test
  for (int i = 0; i < 10; i++) {
    tft.fillScreen(random(0xFFFF));
  }
  
  unsigned long clearTime = millis() - startTime;
  
  // Rectangle drawing test
  startTime = millis();
  tft.fillScreen(TFT_BLACK);
  
  for (int i = 0; i < 1000; i++) {
    int x = random(200);
    int y = random(280);
    int w = random(40);
    int h = random(40);
    uint16_t color = random(0xFFFF);
    tft.fillRect(x, y, w, h, color);
  }
  
  unsigned long rectTime = millis() - startTime;
  
  // Show results
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setCursor(10, 50);
  tft.println("Performance");
  
  tft.setTextSize(1);
  tft.setCursor(10, 100);
  tft.println("Screen clear (10x): " + String(clearTime) + "ms");
  tft.setCursor(10, 120);
  tft.println("Rectangles (1000x): " + String(rectTime) + "ms");
  
  tft.setTextColor(TFT_GREEN);
  tft.setCursor(10, 150);
  tft.println("TFT_eSPI is optimized!");
  tft.setCursor(10, 165);
  tft.println("Fast SPI, DMA support");
  
  Serial.println("Performance results:");
  Serial.println("- Screen clear: " + String(clearTime) + "ms");
  Serial.println("- 1000 rectangles: " + String(rectTime) + "ms");
}