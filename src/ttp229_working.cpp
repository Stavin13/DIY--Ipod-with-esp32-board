/*
TTP229 Serial Mode - WORKING VERSION
Inverted logic: 0xFFFF = no touch, bits go LOW when touched
*/

#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

// TTP229 Serial Pins
#define TTP_SDO 1
#define TTP_SCL 2

// Colors
#define BLACK 0x0000
#define WHITE 0xFFFF
#define BLUE 0x001F
#define GREEN 0x07E0
#define RED 0xF800
#define CYAN 0x07FF
#define YELLOW 0xFFE0

uint16_t lastTouch = 0xFFFF;
int selectedItem = 0;
int maxItems = 4;

const char* menuItems[] = {"Music Library", "Now Playing", "Settings", "About"};

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("╔════════════════════════════════════════╗");
  Serial.println("║   TTP229 iPod Interface - WORKING!    ║");
  Serial.println("╚════════════════════════════════════════╝");
  Serial.println();
  
  // Setup TTP229
  pinMode(TTP_SDO, INPUT);
  pinMode(TTP_SCL, OUTPUT);
  digitalWrite(TTP_SCL, HIGH);
  
  // Setup display
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(BLACK);
  
  // Show splash
  tft.setTextSize(4);
  tft.setTextColor(WHITE);
  tft.setCursor(60, 120);
  tft.print("iPod");
  
  tft.setTextSize(1);
  tft.setTextColor(CYAN);
  tft.setCursor(70, 160);
  tft.print("Touch Working!");
  
  delay(2000);
  
  // Draw menu
  drawMenu();
  
  Serial.println("✅ TTP229 Working!");
  Serial.println("   Key mapping:");
  Serial.println("   1 = Up");
  Serial.println("   2 = Select");
  Serial.println("   3 = Down");
  Serial.println("   4 = Back");
  Serial.println();
}

void loop() {
  uint16_t touchStatus = readTTP229();
  
  // Invert bits (0xFFFF = no touch, 0 = touched)
  touchStatus = ~touchStatus;
  
  if (touchStatus != lastTouch && touchStatus != 0) {
    handleTouch(touchStatus);
    lastTouch = touchStatus;
    delay(200); // Debounce
  } else if (touchStatus == 0) {
    lastTouch = 0;
  }
  
  delay(10);
}

uint16_t readTTP229() {
  uint16_t value = 0;
  
  digitalWrite(TTP_SCL, HIGH);
  delayMicroseconds(100);
  
  for (int i = 0; i < 16; i++) {
    digitalWrite(TTP_SCL, LOW);
    delayMicroseconds(5);
    
    value <<= 1;
    if (digitalRead(TTP_SDO)) {
      value |= 1;
    }
    
    digitalWrite(TTP_SCL, HIGH);
    delayMicroseconds(5);
  }
  
  return value;
}

void handleTouch(uint16_t touch) {
  // Check which key was pressed
  for (int i = 0; i < 16; i++) {
    if ((touch >> i) & 0x01) {
      Serial.print("👆 Key ");
      Serial.print(i + 1);
      Serial.println(" pressed");
      
      // Map keys to actions
      switch (i + 1) {
        case 1: // Up
          navigateUp();
          break;
        case 2: // Select
          selectItem();
          break;
        case 3: // Down
          navigateDown();
          break;
        case 4: // Back
          goBack();
          break;
      }
      
      break; // Only handle first key
    }
  }
}

void navigateUp() {
  selectedItem--;
  if (selectedItem < 0) selectedItem = maxItems - 1;
  Serial.println("⬆️ Navigate Up");
  drawMenu();
}

void navigateDown() {
  selectedItem++;
  if (selectedItem >= maxItems) selectedItem = 0;
  Serial.println("⬇️ Navigate Down");
  drawMenu();
}

void selectItem() {
  Serial.print("✅ Selected: ");
  Serial.println(menuItems[selectedItem]);
  
  // Flash selection
  tft.fillScreen(GREEN);
  delay(200);
  drawMenu();
}

void goBack() {
  Serial.println("⬅️ Back");
  selectedItem = 0;
  drawMenu();
}

void drawMenu() {
  tft.fillScreen(BLACK);
  
  // Title
  tft.setTextSize(3);
  tft.setTextColor(WHITE);
  tft.setCursor(20, 20);
  tft.print("iPod");
  
  tft.setTextSize(1);
  tft.setTextColor(YELLOW);
  tft.setCursor(150, 30);
  tft.print("Touch!");
  
  // Menu items
  for (int i = 0; i < 4; i++) {
    int y = 70 + (i * 35);
    
    if (i == selectedItem) {
      tft.fillRoundRect(10, y - 5, 220, 30, 5, BLUE);
    }
    
    tft.setTextSize(2);
    tft.setTextColor(WHITE);
    tft.setCursor(20, y);
    tft.print(menuItems[i]);
    
    if (i == selectedItem) {
      tft.setCursor(200, y);
      tft.print(">");
    }
  }
  
  // Instructions
  tft.setTextSize(1);
  tft.setTextColor(CYAN);
  tft.setCursor(10, 250);
  tft.print("1=Up 2=OK 3=Down 4=Back");
}
