// Simple TFT Test - Copy to Arduino IDE
// This will help us debug the white screen issue

#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

void setup() {
  Serial.begin(115200);
  Serial.println("=== Simple TFT Test ===");
  
  // Initialize display
  Serial.println("1. Initializing TFT...");
  tft.init();
  
  // Test different rotations
  Serial.println("2. Testing rotations...");
  for(int rotation = 0; rotation < 4; rotation++) {
    Serial.printf("   Rotation %d\n", rotation);
    tft.setRotation(rotation);
    
    // Fill with different colors
    tft.fillScreen(TFT_RED);
    delay(500);
    tft.fillScreen(TFT_GREEN);
    delay(500);
    tft.fillScreen(TFT_BLUE);
    delay(500);
  }
  
  // Set back to portrait
  tft.setRotation(0);
  
  // Test 3: Color inversion
  Serial.println("3. Testing color inversion...");
  tft.fillScreen(TFT_BLACK);
  delay(1000);
  tft.invertDisplay(true);   // Invert colors
  delay(1000);
  tft.invertDisplay(false);  // Normal colors
  
  // Test 4: Simple graphics
  Serial.println("4. Testing graphics...");
  tft.fillScreen(TFT_BLACK);
  
  // Draw colored rectangles
  tft.fillRect(0, 0, 80, 80, TFT_RED);
  tft.fillRect(80, 0, 80, 80, TFT_GREEN);
  tft.fillRect(160, 0, 80, 80, TFT_BLUE);
  
  // Draw text
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(10, 100);
  tft.println("TFT Test");
  tft.println("Working!");
  
  Serial.println("5. Test complete!");
  Serial.println("What do you see on the display?");
}

void loop() {
  // Cycle through colors every 2 seconds
  static unsigned long lastChange = 0;
  static int colorIndex = 0;
  
  if (millis() - lastChange > 2000) {
    uint16_t colors[] = {TFT_RED, TFT_GREEN, TFT_BLUE, TFT_YELLOW, TFT_MAGENTA, TFT_CYAN, TFT_WHITE, TFT_BLACK};
    String colorNames[] = {"RED", "GREEN", "BLUE", "YELLOW", "MAGENTA", "CYAN", "WHITE", "BLACK"};
    
    Serial.printf("Showing: %s\n", colorNames[colorIndex].c_str());
    tft.fillScreen(colors[colorIndex]);
    
    colorIndex = (colorIndex + 1) % 8;
    lastChange = millis();
  }
}