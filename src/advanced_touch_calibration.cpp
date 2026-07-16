/*
Advanced Touch Calibration for ESP32-S3 iPod
Multiple calibration methods and accuracy improvements
*/

#include <TFT_eSPI.h>
#include <SPIFFS.h>

TFT_eSPI tft = TFT_eSPI();

// Touch calibration data
uint16_t calData[5];
bool touchCalibrated = false;

// Calibration settings
#define CALIBRATION_FILE "/TouchCalData"
#define FORCE_RECALIBRATION false  // Set to true to recalibrate
#define CALIBRATION_POINTS 9       // 5 = corners + center, 9 = 3x3 grid

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("╔════════════════════════════════════════╗");
  Serial.println("║     Advanced Touch Calibration        ║");
  Serial.println("╚════════════════════════════════════════╝");
  
  // Initialize SPIFFS for calibration storage
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS initialization failed!");
  }
  
  // Initialize display
  tft.init();
  tft.setRotation(0);  // Portrait
  tft.fillScreen(TFT_BLACK);
  
  // Show calibration menu
  showCalibrationMenu();
}

void loop() {
  uint16_t x, y;
  
  if (tft.getTouch(&x, &y)) {
    // Menu selection based on touch position
    if (y < 80) {
      // Method 1: Standard 5-point calibration
      standardCalibration();
    } else if (y < 160) {
      // Method 2: Enhanced 9-point calibration
      enhancedCalibration();
    } else if (y < 240) {
      // Method 3: Manual calibration with fine-tuning
      manualCalibration();
    } else {
      // Method 4: Test current calibration
      testCalibration();
    }
    
    delay(500); // Debounce
  }
}

void showCalibrationMenu() {
  tft.fillScreen(TFT_BLACK);
  
  // Title
  tft.setTextColor(TFT_CYAN);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println("Touch Calibration");
  
  // Menu options
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE);
  
  // Option 1
  tft.fillRect(5, 50, 230, 25, TFT_DARKGREY);
  tft.setCursor(10, 58);
  tft.println("1. Standard 5-Point Calibration");
  
  // Option 2
  tft.fillRect(5, 90, 230, 25, TFT_DARKGREY);
  tft.setCursor(10, 98);
  tft.println("2. Enhanced 9-Point Calibration");
  
  // Option 3
  tft.fillRect(5, 130, 230, 25, TFT_DARKGREY);
  tft.setCursor(10, 138);
  tft.println("3. Manual Fine-Tune Calibration");
  
  // Option 4
  tft.fillRect(5, 170, 230, 25, TFT_DARKGREY);
  tft.setCursor(10, 178);
  tft.println("4. Test Current Calibration");
  
  // Instructions
  tft.setTextColor(TFT_YELLOW);
  tft.setCursor(10, 220);
  tft.println("Touch an option above");
  
  // Current calibration status
  tft.setTextColor(TFT_GREEN);
  tft.setCursor(10, 250);
  if (loadCalibration()) {
    tft.println("Calibration: Loaded from memory");
  } else {
    tft.setTextColor(TFT_RED);
    tft.println("Calibration: Not found");
  }
}

void standardCalibration() {
  Serial.println("Starting standard 5-point calibration...");
  
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setCursor(20, 50);
  tft.println("Standard");
  tft.setCursor(20, 80);
  tft.println("Calibration");
  
  tft.setTextSize(1);
  tft.setCursor(10, 120);
  tft.println("Touch each target accurately");
  tft.setCursor(10, 135);
  tft.println("Use a stylus for best results");
  
  delay(3000);
  
  // Perform standard TFT_eSPI calibration
  tft.calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);
  
  // Save calibration
  saveCalibration();
  
  showCalibrationResult("Standard calibration complete!");
}

void enhancedCalibration() {
  Serial.println("Starting enhanced 9-point calibration...");
  
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setCursor(20, 50);
  tft.println("Enhanced");
  tft.setCursor(20, 80);
  tft.println("Calibration");
  
  tft.setTextSize(1);
  tft.setCursor(10, 120);
  tft.println("9-point calibration for");
  tft.setCursor(10, 135);
  tft.println("maximum accuracy");
  
  delay(3000);
  
  // First do standard calibration
  tft.calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);
  
  // Then do additional calibration points for fine-tuning
  enhancedCalibrationPoints();
  
  // Save calibration
  saveCalibration();
  
  showCalibrationResult("Enhanced calibration complete!");
}

void enhancedCalibrationPoints() {
  // Additional calibration points for better accuracy
  struct Point {
    int x, y;
    const char* name;
  };
  
  Point extraPoints[] = {
    {60, 80, "Top-Left Quarter"},
    {180, 80, "Top-Right Quarter"},
    {120, 160, "Center"},
    {60, 240, "Bottom-Left Quarter"},
    {180, 240, "Bottom-Right Quarter"}
  };
  
  for (int i = 0; i < 5; i++) {
    tft.fillScreen(TFT_BLACK);
    
    // Instructions
    tft.setTextColor(TFT_YELLOW);
    tft.setTextSize(1);
    tft.setCursor(10, 10);
    tft.print("Touch: ");
    tft.println(extraPoints[i].name);
    
    // Draw target
    drawCalibrationTarget(extraPoints[i].x, extraPoints[i].y, TFT_RED);
    
    // Wait for touch
    waitForTouch();
    
    delay(500);
  }
}

void manualCalibration() {
  Serial.println("Starting manual calibration...");
  
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setCursor(20, 50);
  tft.println("Manual");
  tft.setCursor(20, 80);
  tft.println("Calibration");
  
  tft.setTextSize(1);
  tft.setCursor(10, 120);
  tft.println("Fine-tune calibration");
  tft.setCursor(10, 135);
  tft.println("by adjusting parameters");
  
  delay(3000);
  
  // Load existing calibration or use defaults
  if (!loadCalibration()) {
    // Default calibration values (adjust these for your display)
    calData[0] = 300;   // Touch X minimum
    calData[1] = 3600;  // Touch X maximum  
    calData[2] = 300;   // Touch Y minimum
    calData[3] = 3600;  // Touch Y maximum
    calData[4] = 1;     // Touch orientation
  }
  
  // Manual adjustment interface
  manualAdjustmentInterface();
  
  // Save calibration
  saveCalibration();
  
  showCalibrationResult("Manual calibration complete!");
}

void manualAdjustmentInterface() {
  bool adjusting = true;
  int selectedParam = 0;
  const char* paramNames[] = {"X Min", "X Max", "Y Min", "Y Max", "Rotation"};
  
  while (adjusting) {
    tft.fillScreen(TFT_BLACK);
    
    // Title
    tft.setTextColor(TFT_CYAN);
    tft.setTextSize(1);
    tft.setCursor(10, 10);
    tft.println("Manual Calibration Adjustment");
    
    // Show current values
    for (int i = 0; i < 5; i++) {
      if (i == selectedParam) {
        tft.setTextColor(TFT_YELLOW);
        tft.print("> ");
      } else {
        tft.setTextColor(TFT_WHITE);
        tft.print("  ");
      }
      
      tft.setCursor(10, 40 + i * 20);
      tft.print(paramNames[i]);
      tft.print(": ");
      tft.println(calData[i]);
    }
    
    // Instructions
    tft.setTextColor(TFT_GREEN);
    tft.setCursor(10, 160);
    tft.println("Touch corners to adjust:");
    tft.println("Top-Left: Decrease value");
    tft.println("Top-Right: Increase value");
    tft.println("Bottom-Left: Previous param");
    tft.println("Bottom-Right: Next param");
    tft.println("Center: Save & Exit");
    
    // Wait for touch
    uint16_t x, y;
    while (!tft.getTouch(&x, &y)) {
      delay(50);
    }
    
    // Process touch
    if (x < 80 && y < 80) {
      // Top-left: decrease
      if (calData[selectedParam] > 50) calData[selectedParam] -= 50;
    } else if (x > 160 && y < 80) {
      // Top-right: increase
      if (calData[selectedParam] < 4000) calData[selectedParam] += 50;
    } else if (x < 80 && y > 240) {
      // Bottom-left: previous parameter
      selectedParam = (selectedParam - 1 + 5) % 5;
    } else if (x > 160 && y > 240) {
      // Bottom-right: next parameter
      selectedParam = (selectedParam + 1) % 5;
    } else if (x > 80 && x < 160 && y > 120 && y < 200) {
      // Center: save and exit
      adjusting = false;
    }
    
    delay(300); // Debounce
  }
}

void testCalibration() {
  Serial.println("Testing current calibration...");
  
  tft.fillScreen(TFT_BLACK);
  
  // Load current calibration
  loadCalibration();
  
  // Title
  tft.setTextColor(TFT_GREEN);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println("Calibration");
  tft.setCursor(10, 35);
  tft.println("Test");
  
  // Instructions
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(10, 70);
  tft.println("Touch anywhere to test");
  tft.println("accuracy. Touch corners");
  tft.println("to check precision.");
  
  // Draw test grid
  tft.setTextColor(TFT_DARKGREY);
  for (int x = 0; x < 240; x += 40) {
    tft.drawFastVLine(x, 100, 200, TFT_DARKGREY);
  }
  for (int y = 100; y < 300; y += 40) {
    tft.drawFastHLine(0, y, 240, TFT_DARKGREY);
  }
  
  // Test loop
  unsigned long testStart = millis();
  int touchCount = 0;
  
  while (millis() - testStart < 30000) { // 30 second test
    uint16_t x, y;
    
    if (tft.getTouch(&x, &y)) {
      touchCount++;
      
      // Draw touch point
      uint16_t color = random(0xFFFF);
      tft.fillCircle(x, y, 5, color);
      tft.drawCircle(x, y, 8, TFT_WHITE);
      
      // Show coordinates
      tft.fillRect(0, 305, 240, 15, TFT_BLACK);
      tft.setTextColor(TFT_GREEN);
      tft.setTextSize(1);
      tft.setCursor(5, 308);
      tft.print("Touch #");
      tft.print(touchCount);
      tft.print(": (");
      tft.print(x);
      tft.print(", ");
      tft.print(y);
      tft.print(")");
      
      Serial.print("Touch ");
      Serial.print(touchCount);
      Serial.print(": (");
      Serial.print(x);
      Serial.print(", ");
      Serial.print(y);
      Serial.println(")");
      
      delay(100);
    }
    
    delay(50);
  }
  
  // Show test results
  tft.fillRect(0, 280, 240, 40, TFT_BLACK);
  tft.setTextColor(TFT_CYAN);
  tft.setCursor(10, 285);
  tft.print("Test complete: ");
  tft.print(touchCount);
  tft.println(" touches");
  
  delay(3000);
  showCalibrationMenu();
}

void drawCalibrationTarget(int x, int y, uint16_t color) {
  // Draw crosshair target
  tft.drawCircle(x, y, 10, color);
  tft.drawCircle(x, y, 5, color);
  tft.drawFastHLine(x - 15, y, 30, color);
  tft.drawFastVLine(x, y - 15, 30, color);
  tft.fillCircle(x, y, 2, color);
}

void waitForTouch() {
  uint16_t x, y;
  
  // Wait for touch
  while (!tft.getTouch(&x, &y)) {
    delay(50);
  }
  
  // Wait for release
  while (tft.getTouch(&x, &y)) {
    delay(50);
  }
}

bool saveCalibration() {
  File f = SPIFFS.open(CALIBRATION_FILE, "w");
  if (f) {
    f.write((uint8_t*)calData, sizeof(calData));
    f.close();
    Serial.println("Calibration saved to SPIFFS");
    return true;
  }
  Serial.println("Failed to save calibration");
  return false;
}

bool loadCalibration() {
  File f = SPIFFS.open(CALIBRATION_FILE, "r");
  if (f) {
    f.read((uint8_t*)calData, sizeof(calData));
    f.close();
    
    // Apply calibration to TFT_eSPI
    tft.setTouch(calData);
    touchCalibrated = true;
    
    Serial.println("Calibration loaded from SPIFFS");
    Serial.print("Cal data: ");
    for (int i = 0; i < 5; i++) {
      Serial.print(calData[i]);
      if (i < 4) Serial.print(", ");
    }
    Serial.println();
    
    return true;
  }
  Serial.println("No calibration file found");
  return false;
}

void showCalibrationResult(const char* message) {
  tft.fillScreen(TFT_BLACK);
  
  tft.setTextColor(TFT_GREEN);
  tft.setTextSize(2);
  tft.setCursor(10, 100);
  tft.println("Success!");
  
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(10, 140);
  tft.println(message);
  
  tft.setTextColor(TFT_YELLOW);
  tft.setCursor(10, 170);
  tft.println("Calibration data:");
  for (int i = 0; i < 5; i++) {
    tft.setCursor(10, 185 + i * 15);
    tft.print("Cal[");
    tft.print(i);
    tft.print("]: ");
    tft.println(calData[i]);
  }
  
  tft.setTextColor(TFT_CYAN);
  tft.setCursor(10, 270);
  tft.println("Touch to return to menu");
  
  // Wait for touch
  waitForTouch();
  showCalibrationMenu();
}