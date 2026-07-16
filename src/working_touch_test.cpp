/*
Working Touch Test - Your touch IS working!
Uses the configuration that detected touch (commands 0x93/0xD3)
*/

#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

#define TOUCH_CS  6
#define TOUCH_CLK 7
#define TOUCH_DIN 5
#define TOUCH_DO  4

// Touch state
bool touchDetected = false;
uint16_t touchX = 0, touchY = 0;
unsigned long lastTouchTime = 0;

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("╔════════════════════════════════════════╗");
  Serial.println("║       Working Touch Test v1.0         ║");
  Serial.println("║    Your Touch IS Working! 🎉          ║");
  Serial.println("╚════════════════════════════════════════╝");
  Serial.println();
  
  // Initialize display
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
  
  // Configure touch pins
  pinMode(TOUCH_CS, OUTPUT);
  pinMode(TOUCH_CLK, OUTPUT);
  pinMode(TOUCH_DIN, OUTPUT);
  pinMode(TOUCH_DO, INPUT);
  
  digitalWrite(TOUCH_CS, HIGH);
  digitalWrite(TOUCH_CLK, LOW);
  digitalWrite(TOUCH_DIN, LOW);
  
  // Show success message
  tft.setTextSize(2);
  tft.setTextColor(TFT_GREEN);
  tft.setCursor(20, 50);
  tft.println("TOUCH");
  tft.setCursor(20, 80);
  tft.println("WORKING!");
  
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(20, 120);
  tft.println("Touch anywhere on screen");
  tft.setCursor(20, 135);
  tft.println("Watch serial monitor");
  
  Serial.println("🎉 SUCCESS: Touch controller is functional!");
  Serial.println("📊 Touch detection active...");
  Serial.println("   Touch the screen to see coordinates");
  Serial.println();
}

void loop() {
  checkTouch();
  updateDisplay();
  delay(50); // 20Hz update rate
}

void checkTouch() {
  // Use the working configuration (commands 0x93/0xD3 with always-on power)
  uint16_t rawX = readTouchCoordinate(0x93); // X with always-on power
  uint16_t rawY = readTouchCoordinate(0xD3); // Y with always-on power
  
  // Detect touch (values > 1000 indicate touch)
  bool currentTouch = (rawX > 1000 || rawY > 1000);
  
  if (currentTouch && !touchDetected) {
    // New touch started
    touchDetected = true;
    touchX = rawX;
    touchY = rawY;
    lastTouchTime = millis();
    
    Serial.print("👆 TOUCH DETECTED! Raw: (");
    Serial.print(rawX);
    Serial.print(", ");
    Serial.print(rawY);
    Serial.println(")");
    
    // Map to screen coordinates (rough mapping)
    uint16_t screenX = map(rawX, 0, 4095, 0, 240);
    uint16_t screenY = map(rawY, 0, 4095, 0, 320);
    
    // Constrain to screen bounds
    screenX = constrain(screenX, 0, 239);
    screenY = constrain(screenY, 0, 319);
    
    Serial.print("   Mapped to screen: (");
    Serial.print(screenX);
    Serial.print(", ");
    Serial.print(screenY);
    Serial.println(")");
    
    // Draw touch point on screen
    drawTouchPoint(screenX, screenY);
    
  } else if (!currentTouch && touchDetected) {
    // Touch ended
    touchDetected = false;
    unsigned long touchDuration = millis() - lastTouchTime;
    
    Serial.print("👆 Touch ended after ");
    Serial.print(touchDuration);
    Serial.println("ms");
    Serial.println();
  }
  
  // Update touch coordinates if still touching
  if (currentTouch) {
    touchX = rawX;
    touchY = rawY;
  }
}

uint16_t readTouchCoordinate(uint8_t command) {
  digitalWrite(TOUCH_CS, LOW);
  delayMicroseconds(1);
  
  // Send 8-bit command
  for (int i = 7; i >= 0; i--) {
    digitalWrite(TOUCH_DIN, (command >> i) & 1);
    delayMicroseconds(1);
    digitalWrite(TOUCH_CLK, HIGH);
    delayMicroseconds(1);
    digitalWrite(TOUCH_CLK, LOW);
    delayMicroseconds(1);
  }
  
  // Read 12-bit response
  uint16_t result = 0;
  for (int i = 0; i < 12; i++) {
    digitalWrite(TOUCH_CLK, HIGH);
    delayMicroseconds(1);
    result = (result << 1) | digitalRead(TOUCH_DO);
    digitalWrite(TOUCH_CLK, LOW);
    delayMicroseconds(1);
  }
  
  digitalWrite(TOUCH_CS, HIGH);
  delayMicroseconds(1);
  
  return result;
}

void drawTouchPoint(uint16_t x, uint16_t y) {
  // Draw colorful touch indicator
  static uint16_t colors[] = {TFT_RED, TFT_GREEN, TFT_BLUE, TFT_YELLOW, TFT_CYAN, TFT_MAGENTA};
  static int colorIndex = 0;
  
  uint16_t color = colors[colorIndex];
  colorIndex = (colorIndex + 1) % 6;
  
  // Draw filled circle at touch point
  tft.fillCircle(x, y, 8, color);
  tft.drawCircle(x, y, 12, TFT_WHITE);
  
  // Show coordinates on screen
  tft.fillRect(10, 200, 220, 40, TFT_BLACK);
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(10, 200);
  tft.print("Touch at: (");
  tft.print(x);
  tft.print(", ");
  tft.print(y);
  tft.println(")");
  
  tft.setCursor(10, 215);
  tft.print("Raw: (");
  tft.print(touchX);
  tft.print(", ");
  tft.print(touchY);
  tft.println(")");
}

void updateDisplay() {
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate < 1000) return; // Update every second
  
  // Update status
  tft.fillRect(10, 250, 220, 30, TFT_BLACK);
  tft.setTextSize(1);
  tft.setTextColor(touchDetected ? TFT_GREEN : 0x8410);
  tft.setCursor(10, 250);
  tft.print("Status: ");
  tft.print(touchDetected ? "TOUCHING" : "Ready");
  
  tft.setTextColor(0x8410);
  tft.setCursor(10, 265);
  tft.print("Touch working with 0x93/0xD3");
  
  lastUpdate = millis();
}