/*
Touch Diagnostic Tool - Fix Intermittent Touch Issues
Helps identify and resolve touch problems
*/

#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

// Touch diagnostic variables
unsigned long touchCount = 0;
unsigned long missedTouches = 0;
unsigned long lastTouchTime = 0;
bool touchActive = false;
uint16_t lastX = 0, lastY = 0;

// Touch quality metrics
float touchSuccessRate = 0;
int consecutiveMisses = 0;
int maxConsecutiveMisses = 0;

// Diagnostic settings
#define TOUCH_DEBOUNCE_MS 50
#define TOUCH_TIMEOUT_MS 100
#define MIN_TOUCH_PRESSURE 300
#define MAX_TOUCH_PRESSURE 4000

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("=== Touch Diagnostic Tool ===");
  Serial.println("Analyzing touch reliability...");
  
  // Initialize display
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
  
  // Show diagnostic interface
  drawDiagnosticInterface();
  
  Serial.println("Touch anywhere to start diagnostics");
  Serial.println("Watch for patterns in touch behavior");
}

void loop() {
  runTouchDiagnostics();
  updateDiagnosticDisplay();
  delay(10); // High frequency checking
}

void runTouchDiagnostics() {
  uint16_t x, y;
  
  // Method 1: TFT_eSPI built-in touch
  bool touched1 = tft.getTouch(&x, &y);
  
  // Method 2: Raw touch reading (if available)
  bool touched2 = checkRawTouch();
  
  // Method 3: Pressure-based detection
  bool touched3 = checkTouchPressure();
  
  if (touched1 || touched2 || touched3) {
    if (!touchActive) {
      touchActive = true;
      touchCount++;
      lastTouchTime = millis();
      lastX = x;
      lastY = y;
      consecutiveMisses = 0;
      
      processTouchEvent(x, y);
      
      Serial.print("Touch #");
      Serial.print(touchCount);
      Serial.print(": (");
      Serial.print(x);
      Serial.print(", ");
      Serial.print(y);
      Serial.print(") Methods: ");
      Serial.print(touched1 ? "1" : "-");
      Serial.print(touched2 ? "2" : "-");
      Serial.print(touched3 ? "3" : "-");
      Serial.println();
    }
  } else {
    if (touchActive) {
      // Check if touch was released or lost
      if (millis() - lastTouchTime > TOUCH_TIMEOUT_MS) {
        touchActive = false;
        Serial.println("Touch released");
      } else {
        // Touch might be intermittent
        missedTouches++;
        consecutiveMisses++;
        if (consecutiveMisses > maxConsecutiveMisses) {
          maxConsecutiveMisses = consecutiveMisses;
        }
      }
    }
  }
  
  // Calculate success rate
  if (touchCount > 0) {
    touchSuccessRate = (float)(touchCount * 100) / (touchCount + missedTouches);
  }
}

bool checkRawTouch() {
  // This would implement raw SPI communication with touch controller
  // For now, return false as we're using TFT_eSPI
  return false;
}

bool checkTouchPressure() {
  // Alternative pressure-based detection
  // This is a placeholder - actual implementation would read pressure values
  return false;
}

void processTouchEvent(uint16_t x, uint16_t y) {
  // Draw touch point
  uint16_t color = TFT_GREEN;
  
  // Color code based on touch quality
  if (consecutiveMisses > 5) color = TFT_RED;
  else if (consecutiveMisses > 2) color = TFT_YELLOW;
  
  tft.fillCircle(x, y, 8, color);
  tft.drawCircle(x, y, 12, TFT_WHITE);
  
  // Show touch number
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(x + 15, y - 5);
  tft.print(touchCount);
}

void drawDiagnosticInterface() {
  // Title
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(10, 10);
  tft.println("Touch Test");
  
  // Instructions
  tft.setTextSize(1);
  tft.setTextColor(TFT_CYAN);
  tft.setCursor(10, 40);
  tft.println("Touch anywhere on screen");
  tft.setCursor(10, 55);
  tft.println("Colors indicate quality:");
  
  // Color legend
  tft.fillCircle(20, 75, 5, TFT_GREEN);
  tft.setCursor(35, 72);
  tft.setTextColor(TFT_WHITE);
  tft.println("Good touch");
  
  tft.fillCircle(20, 90, 5, TFT_YELLOW);
  tft.setCursor(35, 87);
  tft.println("Some issues");
  
  tft.fillCircle(20, 105, 5, TFT_RED);
  tft.setCursor(35, 102);
  tft.println("Poor quality");
  
  // Test area
  tft.drawRect(5, 120, 230, 150, TFT_DARKGREY);
  tft.setTextColor(TFT_DARKGREY);
  tft.setCursor(10, 125);
  tft.println("Touch Test Area");
}

void updateDiagnosticDisplay() {
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate < 500) return; // Update every 500ms
  
  // Clear stats area
  tft.fillRect(0, 280, 240, 40, TFT_BLACK);
  
  // Show statistics
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(5, 285);
  tft.print("Touches: ");
  tft.print(touchCount);
  
  tft.setCursor(5, 300);
  tft.print("Success: ");
  tft.print(touchSuccessRate, 1);
  tft.print("%");
  
  tft.setCursor(120, 285);
  tft.print("Missed: ");
  tft.print(missedTouches);
  
  tft.setCursor(120, 300);
  tft.print("Max Miss: ");
  tft.print(maxConsecutiveMisses);
  
  lastUpdate = millis();
}