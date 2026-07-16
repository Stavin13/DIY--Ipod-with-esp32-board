// Simple TFT Display Test - Copy to Arduino IDE

#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

void setup() {
  Serial.begin(115200);
  Serial.println("TFT Test Starting...");
  
  // Initialize display
  tft.init();
  tft.setRotation(0);  // Portrait mode
  
  // Turn on backlight (if connected to GPIO2)
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);
  
  // Clear screen and show test
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.drawString("TFT Working!", 10, 50);
  
  // Draw some shapes
  tft.drawRect(10, 100, 100, 50, TFT_RED);
  tft.fillCircle(120, 125, 25, TFT_BLUE);
  tft.drawLine(0, 200, 240, 200, TFT_GREEN);
  
  Serial.println("TFT initialized successfully!");
}

void loop() {
  // Change colors every second
  static unsigned long lastChange = 0;
  static int colorIndex = 0;
  
  if (millis() - lastChange > 1000) {
    uint16_t colors[] = {TFT_RED, TFT_GREEN, TFT_BLUE, TFT_YELLOW, TFT_MAGENTA, TFT_CYAN};
    
    tft.fillCircle(120, 250, 20, colors[colorIndex]);
    colorIndex = (colorIndex + 1) % 6;
    
    lastChange = millis();
  }
}