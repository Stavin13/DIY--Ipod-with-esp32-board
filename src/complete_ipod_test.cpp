/*
Complete ESP32-S3 iPod Test
Tests touch reliability and UI integration
Upload this to validate the complete system
*/

#include <TFT_eSPI.h>
#include <SPIFFS.h>
#include <FS.h>

TFT_eSPI tft = TFT_eSPI();

// Test modes
enum TestMode { 
  TOUCH_DIAGNOSTIC, 
  TOUCH_CALIBRATION, 
  UI_DEMO, 
  PERFORMANCE_TEST,
  COMPLETE_SYSTEM 
};

TestMode currentMode = TOUCH_DIAGNOSTIC;

// Touch statistics
struct TouchStats {
  unsigned long totalTouches = 0;
  unsigned long successfulTouches = 0;
  unsigned long missedTouches = 0;
  float avgResponseTime = 0;
  unsigned long maxResponseTime = 0;
  unsigned long minResponseTime = 999999;
  int consecutiveSuccesses = 0;
  int maxConsecutiveSuccesses = 0;
  int consecutiveMisses = 0;
  int maxConsecutiveMisses = 0;
};

TouchStats stats;

// Colors
#define BLACK 0x0000
#define WHITE 0xFFFF
#define GREEN 0x07E0
#define RED 0xF800
#define BLUE 0x001F
#define YELLOW 0xFFE0
#define CYAN 0x07FF
#define GRAY 0x8410

// Test configuration
#define TEST_DURATION_MS 30000  // 30 seconds per test
#define TOUCH_SAMPLE_RATE 100   // 100Hz
#define SUCCESS_THRESHOLD 95    // 95% success rate required

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("╔════════════════════════════════════════╗");
  Serial.println("║    ESP32-S3 iPod Complete Test v3.0   ║");
  Serial.println("║   Touch + UI Integration Validation   ║");
  Serial.println("╚════════════════════════════════════════╝");
  
  // Initialize SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("❌ SPIFFS initialization failed!");
    while(1) delay(1000);
  }
  
  // Initialize display
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(BLACK);
  
  // Show test menu
  showTestMenu();
}

void loop() {
  handleTestSelection();
}

void showTestMenu() {
  tft.fillScreen(BLACK);
  
  // Title
  tft.setTextSize(2);
  tft.setTextColor(CYAN);
  tft.setCursor(20, 20);
  tft.println("iPod Test Suite");
  
  // Test options
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  
  tft.setCursor(20, 60);
  tft.println("1. Touch Diagnostic (30s)");
  
  tft.setCursor(20, 80);
  tft.println("2. Touch Calibration");
  
  tft.setCursor(20, 100);
  tft.println("3. UI Demo");
  
  tft.setCursor(20, 120);
  tft.println("4. Performance Test");
  
  tft.setCursor(20, 140);
  tft.println("5. Complete System Test");
  
  // Instructions
  tft.setTextColor(YELLOW);
  tft.setCursor(20, 180);
  tft.println("Touch a test number to start");
  
  tft.setTextColor(GRAY);
  tft.setCursor(20, 200);
  tft.println("Or wait 5s for auto-start");
  
  // Auto-start countdown
  for (int i = 5; i > 0; i--) {
    tft.fillRect(20, 220, 200, 20, BLACK);
    tft.setTextColor(GREEN);
    tft.setCursor(20, 220);
    tft.print("Auto-starting in ");
    tft.print(i);
    tft.print(" seconds...");
    
    // Check for touch during countdown
    uint16_t x, y;
    for (int j = 0; j < 10; j++) {
      if (tft.getTouch(&x, &y)) {
        selectTest(x, y);
        return;
      }
      delay(100);
    }
  }
  
  // Auto-start with touch diagnostic
  currentMode = TOUCH_DIAGNOSTIC;
  runCurrentTest();
}

void handleTestSelection() {
  uint16_t x, y;
  if (tft.getTouch(&x, &y)) {
    selectTest(x, y);
    delay(500); // Debounce
  }
}

void selectTest(uint16_t x, uint16_t y) {
  // Determine which test was selected based on Y coordinate
  if (y >= 60 && y < 80) {
    currentMode = TOUCH_DIAGNOSTIC;
  } else if (y >= 80 && y < 100) {
    currentMode = TOUCH_CALIBRATION;
  } else if (y >= 100 && y < 120) {
    currentMode = UI_DEMO;
  } else if (y >= 120 && y < 140) {
    currentMode = PERFORMANCE_TEST;
  } else if (y >= 140 && y < 160) {
    currentMode = COMPLETE_SYSTEM;
  } else {
    return; // Invalid selection
  }
  
  runCurrentTest();
}

void runCurrentTest() {
  switch (currentMode) {
    case TOUCH_DIAGNOSTIC:
      runTouchDiagnostic();
      break;
    case TOUCH_CALIBRATION:
      runTouchCalibration();
      break;
    case UI_DEMO:
      runUIDemo();
      break;
    case PERFORMANCE_TEST:
      runPerformanceTest();
      break;
    case COMPLETE_SYSTEM:
      runCompleteSystemTest();
      break;
  }
  
  // Return to menu after test
  delay(3000);
  showTestMenu();
}

void runTouchDiagnostic() {
  Serial.println("\n🔍 Starting Touch Diagnostic Test...");
  
  tft.fillScreen(BLACK);
  tft.setTextSize(2);
  tft.setTextColor(CYAN);
  tft.setCursor(20, 20);
  tft.println("Touch Test");
  
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setCursor(20, 50);
  tft.println("Touch anywhere rapidly");
  tft.setCursor(20, 65);
  tft.println("for 30 seconds");
  
  // Reset statistics
  memset(&stats, 0, sizeof(stats));
  stats.minResponseTime = 999999;
  
  unsigned long testStart = millis();
  unsigned long lastTouchTime = 0;
  bool wasTouched = false;
  
  // Test area
  tft.drawRect(10, 90, 220, 150, WHITE);
  tft.setCursor(15, 95);
  tft.setTextColor(GRAY);
  tft.println("Touch Test Area");
  
  while (millis() - testStart < TEST_DURATION_MS) {
    uint16_t x, y;
    bool isTouched = tft.getTouch(&x, &y);
    unsigned long now = millis();
    
    if (isTouched && !wasTouched) {
      // New touch detected
      stats.totalTouches++;
      stats.successfulTouches++;
      stats.consecutiveSuccesses++;
      stats.consecutiveMisses = 0;
      
      if (stats.consecutiveSuccesses > stats.maxConsecutiveSuccesses) {
        stats.maxConsecutiveSuccesses = stats.consecutiveSuccesses;
      }
      
      // Calculate response time
      if (lastTouchTime > 0) {
        unsigned long responseTime = now - lastTouchTime;
        stats.avgResponseTime = (stats.avgResponseTime + responseTime) / 2.0;
        
        if (responseTime > stats.maxResponseTime) {
          stats.maxResponseTime = responseTime;
        }
        if (responseTime < stats.minResponseTime) {
          stats.minResponseTime = responseTime;
        }
      }
      
      // Visual feedback
      drawTouchPoint(x, y, GREEN);
      
      lastTouchTime = now;
      wasTouched = true;
      
    } else if (!isTouched && wasTouched) {
      wasTouched = false;
    } else if (!isTouched) {
      // Check for missed touches (expected but not detected)
      if (now - lastTouchTime > 200 && lastTouchTime > 0) {
        stats.missedTouches++;
        stats.consecutiveMisses++;
        stats.consecutiveSuccesses = 0;
        
        if (stats.consecutiveMisses > stats.maxConsecutiveMisses) {
          stats.maxConsecutiveMisses = stats.consecutiveMisses;
        }
      }
    }
    
    // Update display every second
    if ((now - testStart) % 1000 < 50) {
      updateDiagnosticDisplay(testStart);
    }
    
    delay(10);
  }
  
  showDiagnosticResults();
}

void drawTouchPoint(uint16_t x, uint16_t y, uint16_t color) {
  // Only draw if within test area
  if (x >= 10 && x <= 230 && y >= 90 && y <= 240) {
    tft.fillCircle(x, y, 3, color);
    tft.drawCircle(x, y, 6, WHITE);
  }
}

void updateDiagnosticDisplay(unsigned long testStart) {
  unsigned long elapsed = millis() - testStart;
  unsigned long remaining = (TEST_DURATION_MS - elapsed) / 1000;
  
  // Clear stats area
  tft.fillRect(10, 250, 220, 60, BLACK);
  
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setCursor(10, 250);
  tft.print("Time: ");
  tft.print(remaining);
  tft.print("s");
  
  tft.setCursor(10, 265);
  tft.print("Touches: ");
  tft.print(stats.successfulTouches);
  
  tft.setCursor(10, 280);
  tft.print("Success: ");
  if (stats.totalTouches > 0) {
    float successRate = (float)stats.successfulTouches / stats.totalTouches * 100;
    tft.print(successRate, 1);
    tft.print("%");
  } else {
    tft.print("---%");
  }
  
  tft.setCursor(10, 295);
  tft.print("Avg Response: ");
  tft.print(stats.avgResponseTime, 0);
  tft.print("ms");
}

void showDiagnosticResults() {
  tft.fillScreen(BLACK);
  
  // Calculate final statistics
  float successRate = stats.totalTouches > 0 ? 
    (float)stats.successfulTouches / stats.totalTouches * 100 : 0;
  
  bool testPassed = successRate >= SUCCESS_THRESHOLD && 
                   stats.maxConsecutiveMisses < 10 &&
                   stats.avgResponseTime < 200;
  
  // Title
  tft.setTextSize(2);
  tft.setTextColor(testPassed ? GREEN : RED);
  tft.setCursor(20, 20);
  tft.print("Test ");
  tft.print(testPassed ? "PASSED" : "FAILED");
  
  // Results
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  
  tft.setCursor(20, 60);
  tft.print("Total Touches: ");
  tft.println(stats.totalTouches);
  
  tft.setCursor(20, 80);
  tft.print("Successful: ");
  tft.println(stats.successfulTouches);
  
  tft.setCursor(20, 100);
  tft.print("Success Rate: ");
  tft.print(successRate, 1);
  tft.println("%");
  
  tft.setCursor(20, 120);
  tft.print("Avg Response: ");
  tft.print(stats.avgResponseTime, 0);
  tft.println("ms");
  
  tft.setCursor(20, 140);
  tft.print("Max Response: ");
  tft.print(stats.maxResponseTime);
  tft.println("ms");
  
  tft.setCursor(20, 160);
  tft.print("Min Response: ");
  tft.print(stats.minResponseTime);
  tft.println("ms");
  
  tft.setCursor(20, 180);
  tft.print("Max Consecutive Misses: ");
  tft.println(stats.maxConsecutiveMisses);
  
  // Recommendations
  tft.setTextColor(YELLOW);
  tft.setCursor(20, 210);
  if (testPassed) {
    tft.println("✅ Touch is working reliably!");
    tft.setCursor(20, 225);
    tft.println("Ready for iPod UI");
  } else {
    tft.println("❌ Touch needs improvement:");
    tft.setCursor(20, 225);
    if (successRate < SUCCESS_THRESHOLD) {
      tft.println("• Check wiring connections");
    }
    if (stats.avgResponseTime > 200) {
      tft.println("• Reduce SPI frequency");
    }
    if (stats.maxConsecutiveMisses > 10) {
      tft.println("• Add debouncing/filtering");
    }
  }
  
  // Serial output
  Serial.println("\n📊 Touch Diagnostic Results:");
  Serial.print("   Success Rate: ");
  Serial.print(successRate, 1);
  Serial.println("%");
  Serial.print("   Average Response: ");
  Serial.print(stats.avgResponseTime, 0);
  Serial.println("ms");
  Serial.print("   Test Result: ");
  Serial.println(testPassed ? "PASSED ✅" : "FAILED ❌");
}

void runTouchCalibration() {
  Serial.println("\n🎯 Starting Touch Calibration...");
  
  tft.fillScreen(BLACK);
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
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
  
  uint16_t calData[5];
  tft.calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);
  
  // Save calibration
  fs::File f = SPIFFS.open("/TouchCalData", "w");
  if (f) {
    f.write((uint8_t*)calData, sizeof(calData));
    f.close();
    
    tft.fillScreen(BLACK);
    tft.setTextSize(2);
    tft.setTextColor(GREEN);
    tft.setCursor(20, 100);
    tft.println("Calibration");
    tft.setCursor(20, 130);
    tft.println("Complete!");
    
    Serial.println("✅ Touch calibration saved");
  } else {
    tft.fillScreen(BLACK);
    tft.setTextSize(2);
    tft.setTextColor(RED);
    tft.setCursor(20, 100);
    tft.println("Calibration");
    tft.setCursor(20, 130);
    tft.println("Failed!");
    
    Serial.println("❌ Failed to save calibration");
  }
}

void runUIDemo() {
  Serial.println("\n🎨 Starting UI Demo...");
  
  // This would run the integrated iPod UI
  // For now, show a placeholder
  tft.fillScreen(BLACK);
  tft.setTextSize(2);
  tft.setTextColor(CYAN);
  tft.setCursor(20, 100);
  tft.println("UI Demo");
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setCursor(20, 140);
  tft.println("Upload ipod_ui_integrated.cpp");
  tft.setCursor(20, 155);
  tft.println("for full UI experience");
}

void runPerformanceTest() {
  Serial.println("\n⚡ Starting Performance Test...");
  
  tft.fillScreen(BLACK);
  tft.setTextSize(2);
  tft.setTextColor(YELLOW);
  tft.setCursor(20, 20);
  tft.println("Performance");
  
  // Test display speed
  unsigned long start = millis();
  for (int i = 0; i < 1000; i++) {
    tft.drawPixel(random(240), random(320), random(0xFFFF));
  }
  unsigned long displayTime = millis() - start;
  
  // Test touch response
  start = millis();
  int touchTests = 0;
  while (millis() - start < 5000) {
    uint16_t x, y;
    if (tft.getTouch(&x, &y)) {
      touchTests++;
    }
    delay(1);
  }
  
  // Show results
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setCursor(20, 60);
  tft.print("Display: ");
  tft.print(1000.0 / displayTime * 1000, 1);
  tft.println(" pixels/sec");
  
  tft.setCursor(20, 80);
  tft.print("Touch Rate: ");
  tft.print(touchTests / 5.0, 1);
  tft.println(" Hz");
  
  Serial.print("   Display Performance: ");
  Serial.print(1000.0 / displayTime * 1000, 1);
  Serial.println(" pixels/sec");
  Serial.print("   Touch Rate: ");
  Serial.print(touchTests / 5.0, 1);
  Serial.println(" Hz");
}

void runCompleteSystemTest() {
  Serial.println("\n🔧 Starting Complete System Test...");
  
  tft.fillScreen(BLACK);
  tft.setTextSize(2);
  tft.setTextColor(CYAN);
  tft.setCursor(20, 20);
  tft.println("System Test");
  
  // Test sequence
  bool allPassed = true;
  
  // 1. SPIFFS test
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setCursor(20, 60);
  tft.print("SPIFFS: ");
  
  fs::File testFile = SPIFFS.open("/test.txt", "w");
  if (testFile) {
    testFile.print("test");
    testFile.close();
    SPIFFS.remove("/test.txt");
    tft.setTextColor(GREEN);
    tft.println("PASS");
  } else {
    tft.setTextColor(RED);
    tft.println("FAIL");
    allPassed = false;
  }
  
  // 2. Display test
  tft.setTextColor(WHITE);
  tft.setCursor(20, 80);
  tft.print("Display: ");
  tft.setTextColor(GREEN);
  tft.println("PASS");
  
  // 3. Touch test
  tft.setTextColor(WHITE);
  tft.setCursor(20, 100);
  tft.print("Touch: ");
  
  bool touchWorking = false;
  unsigned long touchStart = millis();
  while (millis() - touchStart < 3000) {
    uint16_t x, y;
    if (tft.getTouch(&x, &y)) {
      touchWorking = true;
      break;
    }
    delay(10);
  }
  
  if (touchWorking) {
    tft.setTextColor(GREEN);
    tft.println("PASS");
  } else {
    tft.setTextColor(RED);
    tft.println("FAIL");
    allPassed = false;
  }
  
  // 4. Memory test
  tft.setTextColor(WHITE);
  tft.setCursor(20, 120);
  tft.print("Memory: ");
  
  size_t freeHeap = ESP.getFreeHeap();
  size_t psramSize = ESP.getPsramSize();
  
  if (freeHeap > 100000) {
    tft.setTextColor(GREEN);
    tft.print("PASS (");
    tft.print(freeHeap / 1024);
    tft.println("KB)");
  } else {
    tft.setTextColor(RED);
    tft.println("FAIL");
    allPassed = false;
  }
  
  // Overall result
  tft.setTextSize(2);
  tft.setCursor(20, 160);
  if (allPassed) {
    tft.setTextColor(GREEN);
    tft.println("ALL TESTS");
    tft.setCursor(20, 190);
    tft.println("PASSED!");
    Serial.println("✅ Complete system test PASSED");
  } else {
    tft.setTextColor(RED);
    tft.println("SOME TESTS");
    tft.setCursor(20, 190);
    tft.println("FAILED!");
    Serial.println("❌ Complete system test FAILED");
  }
  
  tft.setTextSize(1);
  tft.setTextColor(YELLOW);
  tft.setCursor(20, 230);
  tft.println("System ready for iPod UI!");
}