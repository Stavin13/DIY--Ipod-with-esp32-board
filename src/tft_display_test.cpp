// TFT Display Test - Based on Your Exact Wiring
// Copy this to Arduino IDE and upload

#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

void setup() {
  Serial.begin(115200);
  Serial.println("=== TFT Display Test ===");
  Serial.println("Wiring Check:");
  Serial.println("ESP32-S3 → Display");
  Serial.println("GPIO10 → CS");
  Serial.println("GPIO9  → DC");
  Serial.println("GPIO8  → RST");
  Serial.println("GPIO11 → MOSI");
  Serial.println("GPIO12 → SCLK");
  Serial.println("GPIO13 → MISO");
  Serial.println("3.3V   → VCC");
  Serial.println("GND    → GND");
  Serial.println("");
  
  // Initialize TFT display
  Serial.println("Initializing TFT...");
  tft.init();
  
  // Set orientation (0=portrait, 1=landscape)
  tft.setRotation(0);
  
  // Clear screen to black
  Serial.println("Clearing screen...");
  tft.fillScreen(TFT_BLACK);
  
  // Test 1: Fill screen with color
  Serial.println("Test 1: Red screen");
  tft.fillScreen(TFT_RED);
  delay(1000);
  
  Serial.println("Test 2: Green screen");
  tft.fillScreen(TFT_GREEN);
  delay(1000);
  
  Serial.println("Test 3: Blue screen");
  tft.fillScreen(TFT_BLUE);
  delay(1000);
  
  // Test 4: Text display
  Serial.println("Test 4: Text display");
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println("ESP32-S3");
  tft.println("TFT Test");
  tft.println("");
  tft.setTextColor(TFT_YELLOW);
  tft.println("Display: OK");
  
  // Test 5: Shapes
  Serial.println("Test 5: Drawing shapes");
  tft.drawRect(10, 100, 100, 50, TFT_RED);
  tft.fillCircle(180, 125, 25, TFT_BLUE);
  tft.drawLine(0, 180, 240, 180, TFT_GREEN);
  
  // Test 6: Color bars
  Serial.println("Test 6: Color bars");
  for(int i = 0; i < 240; i += 40) {
    uint16_t colors[] = {TFT_RED, TFT_GREEN, TFT_BLUE, TFT_YELLOW, TFT_MAGENTA, TFT_CYAN};
    tft.fillRect(i, 200, 40, 30, colors[i/40]);
  }
  
  Serial.println("TFT Display Test Complete!");
  Serial.println("If you see colors and text, display is working!");
}

void loop() {
  // Animated test - blinking pixel
  static unsigned long lastBlink = 0;
  static bool pixelState = false;
  
  if (millis() - lastBlink > 500) {
    // Blink a pixel in center
    tft.drawPixel(120, 160, pixelState ? TFT_WHITE : TFT_BLACK);
    pixelState = !pixelState;
    lastBlink = millis();
  }
  
  // Display status every 5 seconds
  static unsigned long lastStatus = 0;
  if (millis() - lastStatus > 5000) {
    Serial.println("TFT still running... (blinking pixel should be visible)");
    lastStatus = millis();
  }
}