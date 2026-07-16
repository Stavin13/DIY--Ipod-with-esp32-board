// =====================================================
//  Simple Display Test for Arduino IDE
//  ESP32-S3 iPod Project - MSP2806 Display
//  Install libraries first via Library Manager
// =====================================================

// Required Libraries (install via Arduino IDE Library Manager):
// 1. Adafruit GFX Library by Adafruit
// 2. Adafruit ILI9341 by Adafruit  
// 3. XPT2046_Touchscreen by Paul Stoffregen
// 4. Adafruit BusIO by Adafruit

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <XPT2046_Touchscreen.h>
#include <SPI.h>

// Pin definitions for ESP32-S3 → MSP2806
#define TFT_CS    10    // Display Chip Select
#define TFT_DC    9     // Display Data/Command
#define TFT_RST   8     // Display Reset
#define TFT_MOSI  11    // Display Data Out
#define TFT_SCLK  12    // Display Clock
#define TFT_MISO  13    // Display Data In (optional)

#define TOUCH_CS  6     // Touch Chip Select
#define TOUCH_IRQ 3     // Touch Interrupt (optional)

// Create display and touch objects
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST, TFT_MISO);
XPT2046_Touchscreen touch(TOUCH_CS, TOUCH_IRQ);

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("=== ESP32-S3 iPod Display Test ===");
  Serial.println("MSP2806 - 2.8\" ILI9341 + Touch");
  Serial.println("");
  
  // Print wiring guide
  Serial.println("Wiring Check:");
  Serial.println("ESP32-S3 → MSP2806");
  Serial.println("GPIO10 → CS");
  Serial.println("GPIO9  → DC");
  Serial.println("GPIO8  → RST");
  Serial.println("GPIO11 → SDA/MOSI");
  Serial.println("GPIO12 → SCL/SCLK");
  Serial.println("GPIO13 → SDO/MISO");
  Serial.println("GPIO6  → T_CS");
  Serial.println("GPIO7  → T_CLK");
  Serial.println("GPIO5  → T_DIN");
  Serial.println("GPIO4  → T_DO");
  Serial.println("3.3V   → VCC + LED");
  Serial.println("GND    → GND");
  Serial.println("");
  
  // Initialize display
  Serial.println("Initializing display...");
  tft.begin();
  tft.setRotation(0);  // Portrait mode
  Serial.println("Display initialized!");
  
  // Initialize touch
  Serial.println("Initializing touch...");
  touch.begin();
  Serial.println("Touch initialized!");
  
  // Test display
  runDisplayTests();
}

void loop() {
  // Check for touch
  if (touch.touched()) {
    TS_Point p = touch.getPoint();
    
    // Map touch coordinates to screen
    int x = map(p.x, 200, 3800, 0, 240);
    int y = map(p.y, 200, 3800, 0, 320);
    
    // Draw touch point
    tft.fillCircle(x, y, 5, ILI9341_RED);
    
    Serial.println("Touch at: (" + String(x) + ", " + String(y) + ")");
    delay(100);
  }
  
  delay(50);
}

void runDisplayTests() {
  Serial.println("Running display tests...");
  
  // Test 1: Color fill
  Serial.println("Test 1: Color screens");
  tft.fillScreen(ILI9341_BLACK);
  delay(500);
  tft.fillScreen(ILI9341_RED);
  delay(500);
  tft.fillScreen(ILI9341_GREEN);
  delay(500);
  tft.fillScreen(ILI9341_BLUE);
  delay(500);
  
  // Test 2: Text
  Serial.println("Test 2: Text display");
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println("ESP32-S3");
  tft.println("iPod Test");
  
  tft.setTextColor(ILI9341_YELLOW);
  tft.setCursor(10, 60);
  tft.println("Display: OK");
  
  tft.setTextColor(ILI9341_GREEN);
  tft.setCursor(10, 90);
  tft.println("Touch: Ready");
  
  tft.setTextColor(ILI9341_CYAN);
  tft.setCursor(10, 120);
  tft.println("Libraries: OK");
  
  delay(2000);
  
  // Test 3: Shapes
  Serial.println("Test 3: Shapes");
  tft.fillScreen(ILI9341_BLACK);
  
  // Rectangle
  tft.drawRect(10, 10, 100, 60, ILI9341_RED);
  tft.fillRect(15, 15, 90, 50, ILI9341_BLUE);
  
  // Circle
  tft.drawCircle(180, 40, 30, ILI9341_GREEN);
  tft.fillCircle(180, 40, 25, ILI9341_YELLOW);
  
  // Lines
  tft.drawLine(0, 100, 240, 100, ILI9341_WHITE);
  tft.drawLine(120, 0, 120, 320, ILI9341_WHITE);
  
  delay(2000);
  
  // Test 4: Color bars
  Serial.println("Test 4: Color bars");
  tft.fillScreen(ILI9341_BLACK);
  
  uint16_t colors[] = {
    ILI9341_RED, ILI9341_GREEN, ILI9341_BLUE, 
    ILI9341_YELLOW, ILI9341_MAGENTA, ILI9341_CYAN
  };
  
  for (int i = 0; i < 6; i++) {
    tft.fillRect(i * 40, 50, 40, 220, colors[i]);
  }
  
  delay(2000);
  
  // Test 5: iPod interface preview
  Serial.println("Test 5: iPod interface");
  drawIPodInterface();
  
  Serial.println("All tests complete!");
  Serial.println("Touch the screen to see touch response");
}

void drawIPodInterface() {
  tft.fillScreen(ILI9341_BLACK);
  
  // Status bar
  tft.fillRect(0, 0, 240, 20, ILI9341_BLUE);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(1);
  tft.setCursor(5, 6);
  tft.print("iPod");
  tft.setCursor(100, 6);
  tft.print("12:34");
  tft.setCursor(200, 6);
  tft.print("100%");
  
  // Title
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.setCursor(10, 40);
  tft.println("Now Playing");
  
  // Album art placeholder
  tft.drawRect(60, 80, 120, 120, ILI9341_WHITE);
  tft.setTextColor(ILI9341_DARKGREY);
  tft.setTextSize(1);
  tft.setCursor(105, 130);
  tft.println("Album");
  tft.setCursor(110, 145);
  tft.println("Art");
  
  // Song info
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(1);
  tft.setCursor(10, 220);
  tft.println("Song Title Here");
  tft.setCursor(10, 235);
  tft.println("Artist Name");
  tft.setCursor(10, 250);
  tft.println("Album Name");
  
  // Progress bar
  tft.drawRect(10, 270, 220, 8, ILI9341_WHITE);
  tft.fillRect(11, 271, 80, 6, ILI9341_BLUE);
  
  // Time
  tft.setCursor(10, 285);
  tft.println("1:23");
  tft.setCursor(200, 285);
  tft.println("3:45");
  
  // Controls
  tft.setTextSize(2);
  tft.setCursor(60, 300);
  tft.println("<<  ||  >>");
}