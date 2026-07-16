// Adafruit TFT Test - Alternative to TFT_eSPI
// This uses Adafruit libraries which are more stable with ESP32-S3

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <SPI.h>

// Pin definitions (matching your wiring)
#define TFT_CS    10
#define TFT_DC    9
#define TFT_RST   8
#define TFT_MOSI  11
#define TFT_SCLK  12
#define TFT_MISO  13

// Create display object
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST, TFT_MISO);

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("=== Adafruit ILI9341 Test ===");
  Serial.println("ESP32-S3 → Display Wiring:");
  Serial.println("GPIO10 → CS");
  Serial.println("GPIO9  → DC");
  Serial.println("GPIO8  → RST");
  Serial.println("GPIO11 → MOSI");
  Serial.println("GPIO12 → SCLK");
  Serial.println("GPIO13 → MISO");
  Serial.println("3.3V   → VCC & LED");
  Serial.println("GND    → GND");
  Serial.println("");
  
  // Initialize display
  Serial.println("Initializing display...");
  tft.begin();
  
  // Set rotation (0=portrait, 1=landscape)
  tft.setRotation(0);
  
  Serial.println("Display initialized!");
  
  // Test 1: Clear screen
  Serial.println("Test 1: Black screen");
  tft.fillScreen(ILI9341_BLACK);
  delay(1000);
  
  // Test 2: Color screens
  Serial.println("Test 2: Red screen");
  tft.fillScreen(ILI9341_RED);
  delay(1000);
  
  Serial.println("Test 3: Green screen");
  tft.fillScreen(ILI9341_GREEN);
  delay(1000);
  
  Serial.println("Test 4: Blue screen");
  tft.fillScreen(ILI9341_BLUE);
  delay(1000);
  
  // Test 5: Text
  Serial.println("Test 5: Text display");
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println("ESP32-S3");
  tft.println("iPod Project");
  
  tft.setTextColor(ILI9341_YELLOW);
  tft.setCursor(10, 60);
  tft.println("Display: OK");
  
  tft.setTextColor(ILI9341_GREEN);
  tft.setCursor(10, 90);
  tft.println("Adafruit Lib");
  
  // Test 6: Shapes
  Serial.println("Test 6: Drawing shapes");
  tft.drawRect(10, 130, 100, 50, ILI9341_RED);
  tft.fillRect(15, 135, 90, 40, ILI9341_BLUE);
  
  tft.drawCircle(180, 155, 30, ILI9341_YELLOW);
  tft.fillCircle(180, 155, 25, ILI9341_MAGENTA);
  
  // Test 7: Lines
  tft.drawLine(0, 200, 240, 200, ILI9341_WHITE);
  tft.drawLine(0, 210, 240, 210, ILI9341_CYAN);
  
  // Test 8: Color gradient
  Serial.println("Test 7: Color bars");
  for(int x = 0; x < 240; x += 40) {
    uint16_t colors[] = {
      ILI9341_RED, ILI9341_GREEN, ILI9341_BLUE, 
      ILI9341_YELLOW, ILI9341_MAGENTA, ILI9341_CYAN
    };
    tft.fillRect(x, 230, 40, 30, colors[x/40]);
  }
  
  // Test 9: Pixels
  for(int i = 0; i < 50; i++) {
    tft.drawPixel(random(240), random(320), ILI9341_WHITE);
  }
  
  Serial.println("All tests complete!");
  Serial.println("If you see colors and shapes, display is working perfectly!");
}

void loop() {
  // Animated blinking dot
  static unsigned long lastBlink = 0;
  static bool dotState = false;
  
  if (millis() - lastBlink > 500) {
    tft.fillCircle(120, 280, 5, dotState ? ILI9341_WHITE : ILI9341_BLACK);
    dotState = !dotState;
    lastBlink = millis();
    
    Serial.println(dotState ? "Dot ON" : "Dot OFF");
  }
}