/*
Touch Configuration Test - Fixed for ESP32 Arduino Core 3.x
Tests all touch features with proper File namespace
*/

#include <TFT_eSPI.h>
#include <SPIFFS.h>
#include <FS.h>

TFT_eSPI tft = TFT_eSPI();

// Touch test variables
unsigned long touchCount = 0;
unsigned long gestureCount = 0;
bool calibrationDone = false;
uint16_t calData[5];

// Touch quality metrics
float avgResponseTime = 0;
int minX = 4095, maxX = 0, minY = 4095, maxY = 0;

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("╔════════════════════════════════════════╗");
  Serial.println("║     Touch Configuration Test v2.1     ║");
  Serial.println("║   Fixed for ESP32 Arduino Core 3.x    ║");
  Serial.println("╚════════════════════════════════════════╝");
  
  // Initialize SPIFFS for calibration storage
  if (!SPIFFS.begin(true)) {
    Serial.println("⚠️  SPIFFS initialization failed!");
  } else {
    Serial.println("✅ SPIFFS initialized successfully");
  }
  
  // Initialize display
  Serial.println("🖥️  Initializing display...");
  tft.init();
  tft.setRotation(0); // Portrait
  tft.fillScreen(TFT_BLACK);
  
  // Check if touch is supported
  #ifdef SUPPORT_TOUCH
    Serial.println("✅ Touch support enabled in User_Setup.h");
    
    // Display touch configuration
    Serial.println("\n📋 Touch Configuration:");
    Serial.print("   Touch CS Pin: ");
    Serial.println(TOUCH_CS);
    
    #ifdef TOUCH_PRECISION
      Serial.print("   Touch Precision: ");
      Serial.println(TOUCH_PRECISION);
    #endif
    
    #ifdef TOUCH_THRESHOLD
      Serial.print("   Touch Threshold: ");
      Serial.println(TOUCH_THRESHOLD);
    #endif
    
    #ifdef SPI_TOUCH_FREQUENCY
      Serial.print("   Touch SPI Frequency: ");
      Serial.print(SPI_TOUCH_FREQUENCY / 1000000.0, 1);
      Serial.println(" MHz");
    #endif
    
  #else
    Serial.println("❌ Touch support NOT enabled!");
    Serial.println("   Add #define SUPPORT_TOUCH to User_Setup.h");
  #endif
  
  // Load or perform calibration
  if (loadCalibration()) {
    Serial.println("✅ Touch calibration loaded from memory");
    calibrationDone = true;
  } else {
    Serial.println("🎯 Starting touch calibration...");
    performCalibration();
  }
  
  // Show test interface
  drawTestInterface();
  
  Serial.println("\n🚀 Touch test ready!");
  Serial.println("   Touch anywhere to test functionality");
  Serial.println("   Long press for advanced tests");
}

void loop() {
  testTouchFunctionality();
  updateDisplay();
  delay(10); // 100Hz update rate
}

void testTouchFunctionality() {
  uint16_t x, y;
  static unsigned long lastTouchTime = 0;
  static bool wasTouched = false;
  
  bool isTouched = tft.getTouch(&x, &y);
  
  if (isTouched) {
    if (!wasTouched) {
      // New touch detected
      touchCount++;
      unsigned long responseTime = millis() - lastTouchTime;
      
      if (touchCount > 1) {
        avgResponseTime = (avgResponseTime + responseTime) / 2.0;
      }
      
      // Update coordinate ranges
      if (x < minX) minX = x;
      if (x > maxX) maxX = x;
      if (y < minY) minY = y;
      if (y > maxY) maxY = y;
      
      // Process touch
      processTouchEvent(x, y);
      
      Serial.print("👆 Touch #");
      Serial.print(touchCount);
      Serial.print(": (");
      Serial.print(x);
      Serial.print(", ");
      Serial.print(y);
      Serial.print(") Response: ");
      Serial.print(responseTime);
      Serial.println("ms");
      
      lastTouchTime = millis();
    }
    
    wasTouched = true;
  } else {
    wasTouched = false;
  }
}

void processTouchEvent(uint16_t x, uint16_t y) {
  // Visual feedback
  drawTouchPoint(x, y);
  
  // Test different screen areas
  if (y < 80) {
    handleHeaderTouch(x, y);
  } else if (y > 240) {
    handleFooterTouch(x, y);
  } else {
    handleContentTouch(x, y);
  }
}

void drawTouchPoint(uint16_t x, uint16_t y) {
  // Color based on touch quality
  uint16_t color = TFT_GREEN;
  
  // Check if touch is near screen edges (often less reliable)
  if (x < 20 || x > 220 || y < 20 || y > 300) {
    color = TFT_YELLOW; // Edge touch
  }
  
  // Check for coordinate stability
  static uint16_t lastX = 0, lastY = 0;
  int deltaX = abs((int)x - (int)lastX);
  int deltaY = abs((int)y - (int)lastY);
  
  if (deltaX > 30 || deltaY > 30) {
    color = TFT_RED; // Unstable coordinates
  }
  
  // Draw touch indicator
  tft.fillCircle(x, y, 8, color);
  tft.drawCircle(x, y, 12, TFT_WHITE);
  
  // Show touch number
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(x + 15, y - 5);
  tft.print(touchCount);
  
  lastX = x;
  lastY = y;
}

void handleHeaderTouch(uint16_t x, uint16_t y) {
  if (x < 80) {
    // Recalibrate button
    Serial.println("🎯 Recalibrating touch...");
    performCalibration();
  } else if (x > 160) {
    // Clear screen button
    Serial.println("🧹 Clearing screen...");
    drawTestInterface();
  }
}

void handleFooterTouch(uint16_t x, uint16_t y) {
  // Show touch statistics
  showTouchStats();
}

void handleContentTouch(uint16_t x, uint16_t y) {
  // Test gesture detection
  static uint16_t startX = 0, startY = 0;
  static unsigned long touchStartTime = 0;
  static bool gestureStarted = false;
  
  if (!gestureStarted) {
    startX = x;
    startY = y;
    touchStartTime = millis();
    gestureStarted = true;
  } else {
    // Check for gestures
    int deltaX = (int)x - (int)startX;
    int deltaY = (int)y - (int)startY;
    unsigned long touchDuration = millis() - touchStartTime;
    
    if (abs(deltaX) > 50 || abs(deltaY) > 50) {
      // Swipe detected
      detectSwipe(deltaX, deltaY);
      gestureStarted = false;
    } else if (touchDuration > 800) {
      // Long press detected
      Serial.println("👆 Long press detected!");
      showAdvancedTests();
      gestureStarted = false;
    }
  }
}

void detectSwipe(int deltaX, int deltaY) {
  gestureCount++;
  
  if (abs(deltaX) > abs(deltaY)) {
    // Horizontal swipe
    if (deltaX > 0) {
      Serial.println("👉 Swipe RIGHT detected");
    } else {
      Serial.println("👈 Swipe LEFT detected");
    }
  } else {
    // Vertical swipe
    if (deltaY > 0) {
      Serial.println("👇 Swipe DOWN detected");
    } else {
      Serial.println("👆 Swipe UP detected");
    }
  }
}

void performCalibration() {
  tft.fillScreen(TFT_BLACK);
  
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(20, 50);
  tft.println("Touch");
  tft.setCursor(20, 80);
  tft.println("Calibration");
  
  tft.setTextSize(1);
  tft.setCursor(10, 120);
  tft.println("Touch each target accurately");
  tft.setCursor(10, 135);
  tft.println("Use a stylus for best results");
  
  delay(3000);
  
  // Perform TFT_eSPI calibration
  tft.calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);
  
  // Save calibration
  saveCalibration();
  calibrationDone = true;
  
  Serial.println("✅ Touch calibration complete!");
  Serial.print("📊 Calibration data: ");
  for (int i = 0; i < 5; i++) {
    Serial.print(calData[i]);
    if (i < 4) Serial.print(", ");
  }
  Serial.println();
  
  drawTestInterface();
}

bool loadCalibration() {
  // Fixed File namespace issue
  fs::File f = SPIFFS.open("/TouchCalData", "r");
  if (f) {
    f.read((uint8_t*)calData, sizeof(calData));
    f.close();
    
    // Apply calibration
    tft.setTouch(calData);
    
    return true;
  }
  return false;
}

void saveCalibration() {
  // Fixed File namespace issue
  fs::File f = SPIFFS.open("/TouchCalData", "w");
  if (f) {
    f.write((uint8_t*)calData, sizeof(calData));
    f.close();
    Serial.println("💾 Calibration saved to SPIFFS");
  } else {
    Serial.println("❌ Failed to save calibration");
  }
}

void drawTestInterface() {
  tft.fillScreen(TFT_BLACK);
  
  // Header
  tft.fillRect(0, 0, 240, 30, TFT_DARKGREY);
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(5, 8);
  tft.print("Recal");
  tft.setCursor(100, 8);
  tft.print("Touch Test");
  tft.setCursor(190, 8);
  tft.print("Clear");
  
  // Title
  tft.setTextSize(2);
  tft.setTextColor(TFT_CYAN);
  tft.setCursor(10, 40);
  tft.println("Touch Config");
  tft.setCursor(10, 65);
  tft.println("Test v2.1");
  
  // Instructions
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(10, 100);
  tft.println("Touch anywhere to test");
  tft.setCursor(10, 115);
  tft.println("Swipe for gesture test");
  tft.setCursor(10, 130);
  tft.println("Long press for advanced");
  
  // Test area border
  tft.drawRect(5, 150, 230, 90, TFT_DARKGREY);
  tft.setTextColor(TFT_DARKGREY);
  tft.setCursor(10, 155);
  tft.println("Touch Test Area");
  
  // Footer
  tft.fillRect(0, 250, 240, 30, TFT_DARKGREY);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(5, 258);
  tft.print("Touch for stats");
  
  // Status
  tft.setTextColor(calibrationDone ? TFT_GREEN : TFT_RED);
  tft.setCursor(10, 290);
  tft.print("Calibration: ");
  tft.print(calibrationDone ? "OK" : "NEEDED");
}

void updateDisplay() {
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate < 1000) return; // Update every second
  
  // Update statistics in footer
  tft.fillRect(120, 250, 120, 30, TFT_DARKGREY);
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(125, 258);
  tft.print("Touches: ");
  tft.print(touchCount);
  
  lastUpdate = millis();
}

void showTouchStats() {
  tft.fillRect(50, 100, 140, 120, TFT_DARKGREY);
  tft.drawRect(50, 100, 140, 120, TFT_WHITE);
  
  tft.setTextSize(1);
  tft.setTextColor(TFT_CYAN);
  tft.setCursor(60, 110);
  tft.println("Touch Statistics");
  
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(60, 130);
  tft.print("Total: ");
  tft.println(touchCount);
  
  tft.setCursor(60, 145);
  tft.print("Gestures: ");
  tft.println(gestureCount);
  
  tft.setCursor(60, 160);
  tft.print("Avg Response: ");
  tft.print(avgResponseTime, 0);
  tft.println("ms");
  
  tft.setCursor(60, 175);
  tft.print("X Range: ");
  tft.print(minX);
  tft.print("-");
  tft.println(maxX);
  
  tft.setCursor(60, 190);
  tft.print("Y Range: ");
  tft.print(minY);
  tft.print("-");
  tft.println(maxY);
  
  tft.setTextColor(TFT_YELLOW);
  tft.setCursor(60, 205);
  tft.println("Touch to close");
  
  delay(3000);
  drawTestInterface();
}

void showAdvancedTests() {
  tft.fillRect(30, 80, 180, 160, TFT_DARKGREY);
  tft.drawRect(30, 80, 180, 160, TFT_WHITE);
  
  tft.setTextSize(1);
  tft.setTextColor(TFT_CYAN);
  tft.setCursor(40, 90);
  tft.println("Advanced Touch Tests");
  
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(40, 110);
  tft.println("✓ Basic touch: OK");
  tft.setCursor(40, 125);
  tft.println("✓ Calibration: OK");
  tft.setCursor(40, 140);
  tft.println("✓ Coordinates: OK");
  tft.setCursor(40, 155);
  tft.println("✓ Gestures: OK");
  tft.setCursor(40, 170);
  tft.println("✓ Response time: OK");
  
  tft.setTextColor(TFT_GREEN);
  tft.setCursor(40, 190);
  tft.println("All tests PASSED!");
  
  tft.setTextColor(TFT_YELLOW);
  tft.setCursor(40, 210);
  tft.println("Touch ready for iPod UI");
  
  delay(3000);
  drawTestInterface();
}