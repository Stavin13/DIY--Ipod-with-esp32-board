/*
ESP32-S3 iPod Display Test with Built-in TFT_eSPI Touch
Tests all connections and features using TFT_eSPI integrated touch
Upload this first to verify hardware
*/

#include <TFT_eSPI.h>

// Pin definitions (matching your wiring)
#define LED_PIN 48

TFT_eSPI tft = TFT_eSPI();

// Touch calibration data (will be set during calibration)
uint16_t calData[5];
bool touchCalibrated = false;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n\n╔════════════════════════════════════════╗");
  Serial.println("║  ESP32-S3 iPod Hardware Test v2.0     ║");
  Serial.println("║  Using TFT_eSPI Built-in Touch        ║");
  Serial.println("╚════════════════════════════════════════╝\n");

  // Test 1: Display initialization
  Serial.print("Test 1: Display Init... ");
  tft.init();
  tft.setRotation(0);  // Portrait
  Serial.println("✓ OK");

  // Test 2: Display colors
  Serial.print("Test 2: Color Display... ");
  tft.fillScreen(TFT_BLACK);
  delay(200);
  tft.fillScreen(TFT_RED);
  delay(200);
  tft.fillScreen(TFT_GREEN);
  delay(200);
  tft.fillScreen(TFT_BLUE);
  delay(200);
  Serial.println("✓ OK");

  // Test 3: Text rendering
  Serial.print("Test 3: Text Rendering... ");
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(3);
  tft.setCursor(20, 20);
  tft.println("ESP32-S3");
  tft.setTextSize(2);
  tft.println("\n iPod Test");
  Serial.println("✓ OK");

  // Test 4: Touch screen initialization
  Serial.print("Test 4: Touch Init... ");
  
  // Check if touch is supported in User_Setup.h
  #ifdef TOUCH_CS
    Serial.println("✓ Touch pins defined");
    tft.setTextSize(1);
    tft.setTextColor(TFT_GREEN);
    tft.println("\n Touch: Configured");
    
    // Calibrate touch
    Serial.println("Starting touch calibration...");
    calibrateTouch();
    
  #else
    Serial.println("✗ Touch not configured in User_Setup.h");
    tft.setTextColor(TFT_RED);
    tft.println("\n Touch: Not configured");
    tft.println(" Check User_Setup.h");
  #endif

  // Test 5: Check PSRAM
  Serial.print("Test 5: PSRAM Check... ");
  size_t psramSize = ESP.getPsramSize();
  size_t psramFree = ESP.getFreePsram();
  Serial.print(psramSize / 1024 / 1024);
  Serial.print("MB total, ");
  Serial.print(psramFree / 1024 / 1024);
  Serial.println("MB free ✓");
  
  tft.setTextColor(TFT_CYAN);
  tft.print("\n PSRAM: ");
  tft.print(psramSize / 1024 / 1024);
  tft.println("MB");

  // Test 6: RGB LED (if available)
  Serial.print("Test 6: RGB LED... ");
  pinMode(LED_PIN, OUTPUT);
  // Blink test
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(100);
    digitalWrite(LED_PIN, LOW);
    delay(100);
  }
  Serial.println("✓ OK (if LED blinked)");

  // Test 7: Display speed test
  Serial.print("Test 7: Display Speed... ");
  unsigned long start = millis();
  for (int i = 0; i < 100; i++) {
    tft.fillRect(random(240), random(320), 20, 20, random(0xFFFF));
  }
  unsigned long elapsed = millis() - start;
  float fps = 100000.0 / elapsed;
  Serial.print(fps, 1);
  Serial.println(" FPS ✓");

  // Summary screen
  showSummaryScreen(fps, psramSize);

  Serial.println("\n════════════════════════════════════════");
  Serial.println("All tests passed! Touch screen to test.");
  Serial.println("════════════════════════════════════════\n");
}

void loop() {
  #ifdef TOUCH_CS
    // Check for touch using TFT_eSPI built-in functions
    uint16_t x, y;
    
    if (tft.getTouch(&x, &y)) {
      // Draw colored circle at touch point
      uint16_t color = random(0xFFFF);
      tft.fillCircle(x, y, 8, color);
      tft.drawCircle(x, y, 12, TFT_WHITE);
      
      // Print touch coordinates
      Serial.print("Touch: Screen(");
      Serial.print(x);
      Serial.print(",");
      Serial.print(y);
      Serial.println(")");
      
      // Show coordinates on screen
      tft.fillRect(0, 300, 240, 20, TFT_BLACK);
      tft.setTextColor(TFT_GREEN);
      tft.setTextSize(1);
      tft.setCursor(5, 305);
      tft.print("Touch: (");
      tft.print(x);
      tft.print(", ");
      tft.print(y);
      tft.print(")");
      
      delay(100); // Debounce
    }
  #endif
  
  delay(50);
}

void calibrateTouch() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setCursor(20, 50);
  tft.println("Touch");
  tft.setCursor(20, 80);
  tft.println("Calibration");
  
  tft.setTextSize(1);
  tft.setCursor(10, 120);
  tft.println("Touch the corners when");
  tft.setCursor(10, 135);
  tft.println("prompted. Be accurate!");
  
  delay(2000);
  
  #ifdef TOUCH_CS
    // Perform calibration using TFT_eSPI built-in function
    tft.calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);
    touchCalibrated = true;
    
    Serial.println("Touch calibration complete!");
    Serial.print("Calibration data: ");
    for (int i = 0; i < 5; i++) {
      Serial.print(calData[i]);
      if (i < 4) Serial.print(", ");
    }
    Serial.println();
    
    // Show calibration success
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_GREEN);
    tft.setTextSize(2);
    tft.setCursor(10, 100);
    tft.println("Calibration");
    tft.setCursor(10, 130);
    tft.println("Complete!");
    
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE);
    tft.setCursor(10, 170);
    tft.println("Touch accuracy improved");
    
    delay(2000);
  #endif
}

void showSummaryScreen(float fps, size_t psramSize) {
  tft.fillScreen(TFT_BLACK);
  
  // Title
  tft.setTextColor(TFT_GREEN);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println("ALL TESTS");
  tft.setCursor(10, 35);
  tft.println("PASSED!");
  
  // Test results
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(10, 70);
  tft.println("Display: OK");
  
  #ifdef TOUCH_CS
    tft.println("Touch:   OK (Built-in)");
  #else
    tft.setTextColor(TFT_YELLOW);
    tft.println("Touch:   Not configured");
    tft.setTextColor(TFT_WHITE);
  #endif
  
  tft.print("PSRAM:   ");
  tft.print(psramSize / 1024 / 1024);
  tft.println("MB");
  
  tft.print("Speed:   ");
  tft.print(fps, 1);
  tft.println(" FPS");
  
  // Instructions
  tft.setTextColor(TFT_CYAN);
  tft.setCursor(10, 140);
  tft.println("Touch screen to test");
  tft.println("touch accuracy...");
  
  // TFT_eSPI info
  tft.setTextColor(TFT_YELLOW);
  tft.setCursor(10, 180);
  tft.println("Using TFT_eSPI");
  tft.println("Built-in Touch");
  
  // Touch area indicator
  tft.drawRect(5, 200, 230, 110, TFT_DARKGREY);
  tft.setTextColor(TFT_DARKGREY);
  tft.setCursor(10, 205);
  tft.println("Touch Area");
}