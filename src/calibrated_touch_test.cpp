/*
Calibrated Touch Test - Fixed threshold and filtering
Prevents false touch detection and provides accurate touch sensing
*/

#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

#define TOUCH_CS  6
#define TOUCH_CLK 7
#define TOUCH_DIN 5
#define TOUCH_DO  4

// Touch configuration
#define TOUCH_THRESHOLD_MIN 500   // Minimum pressure to register touch
#define TOUCH_THRESHOLD_MAX 3800  // Maximum valid coordinate
#define DEBOUNCE_TIME 50          // Debounce time in ms
#define SAMPLES_FOR_TOUCH 3       // Consecutive samples needed

// Touch state
bool touchDetected = false;
uint16_t touchX = 0, touchY = 0;
unsigned long lastTouchTime = 0;
int consecutiveTouchSamples = 0;
int consecutiveNoTouchSamples = 0;

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("╔════════════════════════════════════════╗");
  Serial.println("║     Calibrated Touch Test v1.0        ║");
  Serial.println("║   Fixed Threshold & Filtering         ║");
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
  
  // Show interface
  drawInterface();
  
  Serial.println("🎯 CALIBRATED TOUCH DETECTION:");
  Serial.print("   Touch threshold: ");
  Serial.print(TOUCH_THRESHOLD_MIN);
  Serial.print(" - ");
  Serial.println(TOUCH_THRESHOLD_MAX);
  Serial.print("   Samples needed: ");
  Serial.println(SAMPLES_FOR_TOUCH);
  Serial.println("   Touch the screen to test...");
  Serial.println();
}

void loop() {
  checkTouchWithFiltering();
  updateDisplay();
  delay(20); // 50Hz update rate
}

void checkTouchWithFiltering() {
  // Read raw coordinates
  uint16_t rawX = readTouchCoordinate(0x93); // X with always-on power
  uint16_t rawY = readTouchCoordinate(0xD3); // Y with always-on power
  uint16_t rawZ = readTouchCoordinate(0xB3); // Pressure
  
  // Calculate pressure (for resistive touch)
  uint16_t pressure = 0;
  if (rawZ > 0) {
    pressure = 4095 - rawZ; // Invert pressure reading
  }
  
  // Determine if this sample indicates touch
  bool sampleHasTouch = false;
  
  // Method 1: Check if coordinates are in valid range (not 0 or max)
  if (rawX > TOUCH_THRESHOLD_MIN && rawX < TOUCH_THRESHOLD_MAX &&
      rawY > TOUCH_THRESHOLD_MIN && rawY < TOUCH_THRESHOLD_MAX) {
    sampleHasTouch = true;
  }
  
  // Method 2: Check pressure if available
  if (pressure > 100 && pressure < 3000) {
    sampleHasTouch = true;
  }
  
  // Method 3: Reject obvious false readings
  if (rawX == 0 || rawX >= 4090 || rawY == 0 || rawY >= 4090) {
    sampleHasTouch = false;
  }
  
  // Update consecutive sample counters
  if (sampleHasTouch) {
    consecutiveTouchSamples++;
    consecutiveNoTouchSamples = 0;
  } else {
    consecutiveNoTouchSamples++;
    consecutiveTouchSamples = 0;
  }
  
  // Determine touch state based on consecutive samples
  if (!touchDetected && consecutiveTouchSamples >= SAMPLES_FOR_TOUCH) {
    // Touch started
    touchDetected = true;
    touchX = rawX;
    touchY = rawY;
    lastTouchTime = millis();
    
    Serial.print("👆 TOUCH STARTED! Raw: (");
    Serial.print(rawX);
    Serial.print(", ");
    Serial.print(rawY);
    Serial.print(") Pressure: ");
    Serial.println(pressure);
    
    // Map to screen coordinates
    uint16_t screenX = mapTouchX(rawX);
    uint16_t screenY = mapTouchY(rawY);
    
    Serial.print("   Screen coordinates: (");
    Serial.print(screenX);
    Serial.print(", ");
    Serial.print(screenY);
    Serial.println(")");
    
    // Draw touch point
    drawTouchPoint(screenX, screenY);
    
  } else if (touchDetected && consecutiveNoTouchSamples >= SAMPLES_FOR_TOUCH) {
    // Touch ended
    touchDetected = false;
    unsigned long touchDuration = millis() - lastTouchTime;
    
    Serial.print("👆 Touch ended after ");
    Serial.print(touchDuration);
    Serial.println("ms");
    Serial.println();
  }
  
  // Update coordinates if still touching
  if (touchDetected && sampleHasTouch) {
    touchX = rawX;
    touchY = rawY;
  }
  
  // Debug output (every 50 samples)
  static int debugCount = 0;
  debugCount++;
  if (debugCount % 50 == 0) {
    Serial.print("Debug ");
    Serial.print(debugCount);
    Serial.print(": Raw(");
    Serial.print(rawX);
    Serial.print(",");
    Serial.print(rawY);
    Serial.print(") P=");
    Serial.print(pressure);
    Serial.print(" Touch=");
    Serial.print(sampleHasTouch ? "Y" : "N");
    Serial.print(" Consecutive=");
    Serial.println(consecutiveTouchSamples);
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

uint16_t mapTouchX(uint16_t rawX) {
  // Map raw X coordinate to screen X (0-239)
  // Adjust these values based on your touch calibration
  return map(rawX, 500, 3800, 0, 239);
}

uint16_t mapTouchY(uint16_t rawY) {
  // Map raw Y coordinate to screen Y (0-319)  
  // Adjust these values based on your touch calibration
  return map(rawY, 500, 3800, 0, 319);
}

void drawInterface() {
  tft.setTextSize(2);
  tft.setTextColor(TFT_CYAN);
  tft.setCursor(20, 20);
  tft.println("Touch Test");
  
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(20, 60);
  tft.println("Calibrated touch detection");
  tft.setCursor(20, 75);
  tft.println("No false positives!");
  
  // Draw test area
  tft.drawRect(10, 100, 220, 150, TFT_WHITE);
  tft.setTextColor(0x8410);
  tft.setCursor(15, 105);
  tft.println("Touch Test Area");
  
  // Instructions
  tft.setTextColor(TFT_YELLOW);
  tft.setCursor(20, 270);
  tft.println("Touch anywhere to test");
  tft.setCursor(20, 285);
  tft.println("Watch Serial Monitor");
}

void drawTouchPoint(uint16_t x, uint16_t y) {
  // Only draw if within test area
  if (x >= 10 && x <= 230 && y >= 100 && y <= 250) {
    // Draw colorful touch indicator
    static uint16_t colors[] = {TFT_RED, TFT_GREEN, TFT_BLUE, TFT_YELLOW, TFT_CYAN, TFT_MAGENTA};
    static int colorIndex = 0;
    
    uint16_t color = colors[colorIndex];
    colorIndex = (colorIndex + 1) % 6;
    
    // Draw filled circle at touch point
    tft.fillCircle(x, y, 6, color);
    tft.drawCircle(x, y, 10, TFT_WHITE);
  }
  
  // Update coordinate display
  tft.fillRect(10, 300, 220, 20, TFT_BLACK);
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(10, 300);
  tft.print("Last touch: (");
  tft.print(x);
  tft.print(", ");
  tft.print(y);
  tft.print(")");
}

void updateDisplay() {
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate < 500) return; // Update every 500ms
  
  // Update status
  tft.fillRect(150, 270, 80, 30, TFT_BLACK);
  tft.setTextSize(1);
  tft.setTextColor(touchDetected ? TFT_GREEN : 0x8410);
  tft.setCursor(150, 270);
  tft.print("Status:");
  tft.setCursor(150, 285);
  tft.print(touchDetected ? "TOUCHING" : "Ready");
  
  lastUpdate = millis();
}