/*
Touch Hardware Diagnostic - Deep Hardware Analysis
Systematically tests every aspect of touch functionality
Upload this to identify the exact cause of no touch response
*/

#include <TFT_eSPI.h>
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI();

// Pin definitions from your config
#define TOUCH_CS  6     // Touch Chip Select
#define TOUCH_CLK 7     // Touch Clock  
#define TOUCH_DIN 5     // Touch Data In
#define TOUCH_DO  4     // Touch Data Out
#define TOUCH_IRQ 3     // Touch Interrupt

// Colors
#define BLACK 0x0000
#define WHITE 0xFFFF
#define GREEN 0x07E0
#define RED 0xF800
#define YELLOW 0xFFE0
#define CYAN 0x07FF
#define GRAY 0x8410

// Test results
struct DiagnosticResults {
  bool displayWorking = false;
  bool touchPinsConfigured = false;
  bool spiCommunication = false;
  bool touchControllerResponding = false;
  bool touchDataValid = false;
  bool calibrationExists = false;
  bool tftEspiTouchEnabled = false;
  String errorMessage = "";
};

DiagnosticResults results;

void setup() {
  Serial.begin(115200);
  delay(3000); // Give time to open serial monitor
  
  Serial.println("╔════════════════════════════════════════╗");
  Serial.println("║    Touch Hardware Diagnostic v4.0     ║");
  Serial.println("║     Deep Hardware Analysis Tool       ║");
  Serial.println("╚════════════════════════════════════════╝");
  Serial.println();
  
  // Initialize display first
  Serial.println("🖥️  Step 1: Testing Display...");
  testDisplay();
  
  Serial.println("\n📌 Step 2: Testing Pin Configuration...");
  testPinConfiguration();
  
  Serial.println("\n🔌 Step 3: Testing SPI Communication...");
  testSPICommunication();
  
  Serial.println("\n🎯 Step 4: Testing Touch Controller...");
  testTouchController();
  
  Serial.println("\n📊 Step 5: Testing TFT_eSPI Touch...");
  testTFTESPITouch();
  
  Serial.println("\n🔍 Step 6: Raw Touch Data Analysis...");
  testRawTouchData();
  
  Serial.println("\n📋 Step 7: Configuration Analysis...");
  analyzeConfiguration();
  
  Serial.println("\n" + String("=").substring(0, 50));
  showFinalDiagnosis();
  
  // Show results on display
  displayResults();
}

void loop() {
  // Continuous touch monitoring
  monitorTouchContinuously();
  delay(100);
}

void testDisplay() {
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(BLACK);
  
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.setCursor(20, 20);
  tft.print("Touch Test");
  
  results.displayWorking = true;
  Serial.println("   ✅ Display initialized successfully");
}

void testPinConfiguration() {
  Serial.println("   📍 Checking pin assignments:");
  Serial.print("      TOUCH_CS:  GPIO");
  Serial.println(TOUCH_CS);
  Serial.print("      TOUCH_CLK: GPIO");
  Serial.println(TOUCH_CLK);
  Serial.print("      TOUCH_DIN: GPIO");
  Serial.println(TOUCH_DIN);
  Serial.print("      TOUCH_DO:  GPIO");
  Serial.println(TOUCH_DO);
  Serial.print("      TOUCH_IRQ: GPIO");
  Serial.println(TOUCH_IRQ);
  
  // Configure pins manually
  pinMode(TOUCH_CS, OUTPUT);
  pinMode(TOUCH_CLK, OUTPUT);
  pinMode(TOUCH_DIN, OUTPUT);
  pinMode(TOUCH_DO, INPUT);
  pinMode(TOUCH_IRQ, INPUT_PULLUP);
  
  digitalWrite(TOUCH_CS, HIGH);
  digitalWrite(TOUCH_CLK, LOW);
  digitalWrite(TOUCH_DIN, LOW);
  
  Serial.println("   ✅ Touch pins configured manually");
  
  // Test pin states
  Serial.println("   📊 Pin state test:");
  Serial.print("      CS (should be HIGH): ");
  Serial.println(digitalRead(TOUCH_CS) ? "HIGH ✅" : "LOW ❌");
  
  Serial.print("      IRQ (should be HIGH with pullup): ");
  Serial.println(digitalRead(TOUCH_IRQ) ? "HIGH ✅" : "LOW ❌");
  
  results.touchPinsConfigured = true;
}

void testSPICommunication() {
  Serial.println("   🔄 Testing manual SPI communication...");
  
  // Try to communicate with touch controller using bit-banging
  digitalWrite(TOUCH_CS, LOW);
  delayMicroseconds(10);
  
  // Send command to read X coordinate (0x90)
  uint8_t command = 0x90;
  for (int i = 7; i >= 0; i--) {
    digitalWrite(TOUCH_DIN, (command >> i) & 1);
    delayMicroseconds(1);
    digitalWrite(TOUCH_CLK, HIGH);
    delayMicroseconds(1);
    digitalWrite(TOUCH_CLK, LOW);
    delayMicroseconds(1);
  }
  
  // Try to read response
  uint16_t response = 0;
  for (int i = 0; i < 12; i++) {
    digitalWrite(TOUCH_CLK, HIGH);
    delayMicroseconds(1);
    response = (response << 1) | digitalRead(TOUCH_DO);
    digitalWrite(TOUCH_CLK, LOW);
    delayMicroseconds(1);
  }
  
  digitalWrite(TOUCH_CS, HIGH);
  
  Serial.print("   📡 Touch controller response: 0x");
  Serial.print(response, HEX);
  
  if (response > 0 && response < 0xFFF) {
    Serial.println(" ✅ (Valid response)");
    results.spiCommunication = true;
  } else if (response == 0) {
    Serial.println(" ❌ (No response - check wiring)");
  } else if (response == 0xFFF) {
    Serial.println(" ❌ (All HIGH - check connections)");
  } else {
    Serial.println(" ⚠️  (Unexpected response)");
  }
}

void testTouchController() {
  Serial.println("   🎯 Testing XPT2046 touch controller...");
  
  // Test multiple touch controller commands
  uint16_t xReading = readTouchRaw(0x90); // X coordinate
  uint16_t yReading = readTouchRaw(0xD0); // Y coordinate
  uint16_t z1Reading = readTouchRaw(0xB0); // Z1 (pressure)
  uint16_t z2Reading = readTouchRaw(0xC0); // Z2 (pressure)
  
  Serial.print("   📊 Raw readings:");
  Serial.print(" X=");
  Serial.print(xReading);
  Serial.print(" Y=");
  Serial.print(yReading);
  Serial.print(" Z1=");
  Serial.print(z1Reading);
  Serial.print(" Z2=");
  Serial.println(z2Reading);
  
  // Analyze readings
  if (xReading > 100 && xReading < 4000 && 
      yReading > 100 && yReading < 4000) {
    Serial.println("   ✅ Touch controller responding with valid data");
    results.touchControllerResponding = true;
    results.touchDataValid = true;
  } else if (xReading == 0 && yReading == 0) {
    Serial.println("   ❌ Touch controller not responding (all zeros)");
    results.errorMessage = "Touch controller not responding - check power/wiring";
  } else if (xReading == 0xFFF && yReading == 0xFFF) {
    Serial.println("   ❌ Touch controller stuck HIGH (wiring issue)");
    results.errorMessage = "Touch pins stuck HIGH - check connections";
  } else {
    Serial.println("   ⚠️  Touch controller responding but data may be invalid");
    results.touchControllerResponding = true;
  }
}

uint16_t readTouchRaw(uint8_t command) {
  digitalWrite(TOUCH_CS, LOW);
  delayMicroseconds(10);
  
  // Send command
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
  delayMicroseconds(10);
  
  return result;
}

void testTFTESPITouch() {
  Serial.println("   🔧 Testing TFT_eSPI touch integration...");
  
  #ifdef SUPPORT_TOUCH
    Serial.println("   ✅ SUPPORT_TOUCH is defined");
    results.tftEspiTouchEnabled = true;
    
    // Test TFT_eSPI touch functions
    uint16_t x, y;
    bool touchDetected = tft.getTouch(&x, &y);
    
    Serial.print("   📱 TFT_eSPI getTouch() result: ");
    if (touchDetected) {
      Serial.print("TOUCH DETECTED at (");
      Serial.print(x);
      Serial.print(", ");
      Serial.print(y);
      Serial.println(") ✅");
    } else {
      Serial.println("NO TOUCH ❌");
    }
    
    // Check touch configuration
    Serial.println("   ⚙️  Touch configuration:");
    #ifdef TOUCH_CS
      Serial.print("      TOUCH_CS defined as: ");
      Serial.println(TOUCH_CS);
    #else
      Serial.println("      ❌ TOUCH_CS not defined in User_Setup.h");
    #endif
    
    #ifdef SPI_TOUCH_FREQUENCY
      Serial.print("      SPI_TOUCH_FREQUENCY: ");
      Serial.print(SPI_TOUCH_FREQUENCY);
      Serial.println(" Hz");
    #else
      Serial.println("      ⚠️  SPI_TOUCH_FREQUENCY not defined");
    #endif
    
  #else
    Serial.println("   ❌ SUPPORT_TOUCH is NOT defined in User_Setup.h");
    results.errorMessage = "SUPPORT_TOUCH not enabled in User_Setup.h";
  #endif
}

void testRawTouchData() {
  Serial.println("   📊 Analyzing raw touch data patterns...");
  
  // Take multiple samples
  int validSamples = 0;
  int totalSamples = 10;
  
  for (int i = 0; i < totalSamples; i++) {
    uint16_t x = readTouchRaw(0x90);
    uint16_t y = readTouchRaw(0xD0);
    
    Serial.print("   Sample ");
    Serial.print(i + 1);
    Serial.print(": X=");
    Serial.print(x);
    Serial.print(" Y=");
    Serial.print(y);
    
    if (x > 100 && x < 4000 && y > 100 && y < 4000) {
      Serial.println(" ✅");
      validSamples++;
    } else {
      Serial.println(" ❌");
    }
    
    delay(100);
  }
  
  float successRate = (float)validSamples / totalSamples * 100;
  Serial.print("   📈 Valid sample rate: ");
  Serial.print(successRate, 1);
  Serial.println("%");
  
  if (successRate > 80) {
    Serial.println("   ✅ Touch data quality is good");
  } else if (successRate > 50) {
    Serial.println("   ⚠️  Touch data quality is marginal");
  } else {
    Serial.println("   ❌ Touch data quality is poor");
  }
}

void analyzeConfiguration() {
  Serial.println("   🔍 Analyzing User_Setup.h configuration...");
  
  // Check critical defines
  Serial.println("   📋 Critical configuration check:");
  
  #ifdef ILI9341_DRIVER
    Serial.println("      ✅ ILI9341_DRIVER defined");
  #else
    Serial.println("      ❌ ILI9341_DRIVER not defined");
  #endif
  
  #ifdef SUPPORT_TOUCH
    Serial.println("      ✅ SUPPORT_TOUCH defined");
  #else
    Serial.println("      ❌ SUPPORT_TOUCH not defined - THIS IS THE PROBLEM!");
    results.errorMessage = "Add #define SUPPORT_TOUCH to User_Setup.h";
  #endif
  
  #ifdef TOUCH_CS
    Serial.print("      ✅ TOUCH_CS defined as GPIO");
    Serial.println(TOUCH_CS);
    
    if (TOUCH_CS != 6) {
      Serial.println("      ⚠️  TOUCH_CS should be GPIO6 for your wiring");
    }
  #else
    Serial.println("      ❌ TOUCH_CS not defined");
  #endif
  
  // Check pin conflicts
  Serial.println("   ⚠️  Checking for pin conflicts:");
  if (TOUCH_CS == 10) {
    Serial.println("      ❌ TOUCH_CS conflicts with TFT_CS!");
  }
  
  // Check SPI configuration
  Serial.println("   🔄 SPI configuration:");
  #ifdef USE_HSPI_PORT
    Serial.println("      ✅ Using HSPI port");
  #else
    Serial.println("      ⚠️  SPI port not explicitly defined");
  #endif
}

void showFinalDiagnosis() {
  Serial.println("🏥 FINAL DIAGNOSIS:");
  Serial.println("==================");
  
  if (!results.tftEspiTouchEnabled) {
    Serial.println("❌ PRIMARY ISSUE: SUPPORT_TOUCH not enabled");
    Serial.println("   SOLUTION: Add #define SUPPORT_TOUCH to User_Setup.h");
    Serial.println();
  }
  
  if (!results.spiCommunication) {
    Serial.println("❌ HARDWARE ISSUE: No SPI communication with touch controller");
    Serial.println("   SOLUTIONS:");
    Serial.println("   1. Check all touch pin connections");
    Serial.println("   2. Verify 3.3V power to touch controller");
    Serial.println("   3. Check for loose breadboard connections");
    Serial.println("   4. Try different jumper wires");
    Serial.println();
  }
  
  if (results.touchControllerResponding && !results.touchDataValid) {
    Serial.println("⚠️  CALIBRATION ISSUE: Touch controller works but needs calibration");
    Serial.println("   SOLUTION: Run touch calibration");
    Serial.println();
  }
  
  if (results.errorMessage.length() > 0) {
    Serial.println("🔧 SPECIFIC ERROR: " + results.errorMessage);
    Serial.println();
  }
  
  // Provide specific solutions
  Serial.println("🛠️  RECOMMENDED ACTIONS:");
  Serial.println("1. Copy User_Setup_Complete.h to TFT_eSPI/User_Setup.h");
  Serial.println("2. Verify this wiring:");
  Serial.println("   ESP32-S3  →  MSP2806");
  Serial.println("   GPIO6     →  T_CS");
  Serial.println("   GPIO7     →  T_CLK");
  Serial.println("   GPIO5     →  T_DIN");
  Serial.println("   GPIO4     →  T_DO");
  Serial.println("   GPIO3     →  T_IRQ");
  Serial.println("3. Check 3.3V power connections");
  Serial.println("4. Try pressing firmly on the screen while testing");
  Serial.println();
}

void displayResults() {
  tft.fillScreen(BLACK);
  
  // Title
  tft.setTextSize(2);
  tft.setTextColor(CYAN);
  tft.setCursor(10, 10);
  tft.println("Touch Diagnostic");
  
  // Results
  tft.setTextSize(1);
  int y = 50;
  
  tft.setTextColor(results.displayWorking ? GREEN : RED);
  tft.setCursor(10, y);
  tft.print("Display: ");
  tft.println(results.displayWorking ? "OK" : "FAIL");
  y += 20;
  
  tft.setTextColor(results.touchPinsConfigured ? GREEN : RED);
  tft.setCursor(10, y);
  tft.print("Pin Config: ");
  tft.println(results.touchPinsConfigured ? "OK" : "FAIL");
  y += 20;
  
  tft.setTextColor(results.spiCommunication ? GREEN : RED);
  tft.setCursor(10, y);
  tft.print("SPI Comm: ");
  tft.println(results.spiCommunication ? "OK" : "FAIL");
  y += 20;
  
  tft.setTextColor(results.touchControllerResponding ? GREEN : RED);
  tft.setCursor(10, y);
  tft.print("Touch IC: ");
  tft.println(results.touchControllerResponding ? "OK" : "FAIL");
  y += 20;
  
  tft.setTextColor(results.tftEspiTouchEnabled ? GREEN : RED);
  tft.setCursor(10, y);
  tft.print("TFT_eSPI: ");
  tft.println(results.tftEspiTouchEnabled ? "OK" : "FAIL");
  y += 20;
  
  // Main issue
  tft.setTextColor(YELLOW);
  tft.setCursor(10, y + 20);
  tft.println("Main Issue:");
  
  tft.setTextColor(WHITE);
  tft.setCursor(10, y + 40);
  if (!results.tftEspiTouchEnabled) {
    tft.println("SUPPORT_TOUCH not");
    tft.setCursor(10, y + 55);
    tft.println("enabled in User_Setup.h");
  } else if (!results.spiCommunication) {
    tft.println("Check touch wiring");
    tft.setCursor(10, y + 55);
    tft.println("and power connections");
  } else {
    tft.println("Hardware OK");
    tft.setCursor(10, y + 55);
    tft.println("Try calibration");
  }
  
  // Instructions
  tft.setTextColor(GRAY);
  tft.setCursor(10, 280);
  tft.println("Check Serial Monitor");
  tft.setCursor(10, 295);
  tft.println("for detailed diagnosis");
}

void monitorTouchContinuously() {
  static int testCount = 0;
  testCount++;
  
  // Test both raw and TFT_eSPI methods
  uint16_t rawX = readTouchRaw(0x90);
  uint16_t rawY = readTouchRaw(0xD0);
  
  uint16_t tftX, tftY;
  bool tftTouch = tft.getTouch(&tftX, &tftY);
  
  // Only print if there's activity or every 50 tests
  if (rawX > 500 || rawY > 500 || tftTouch || testCount % 50 == 0) {
    Serial.print("Monitor ");
    Serial.print(testCount);
    Serial.print(": Raw(");
    Serial.print(rawX);
    Serial.print(",");
    Serial.print(rawY);
    Serial.print(") TFT(");
    if (tftTouch) {
      Serial.print(tftX);
      Serial.print(",");
      Serial.print(tftY);
    } else {
      Serial.print("none");
    }
    Serial.println(")");
    
    if (tftTouch) {
      // Show touch on display
      tft.fillCircle(tftX, tftY, 5, GREEN);
      delay(100);
      tft.fillCircle(tftX, tftY, 5, BLACK);
    }
  }
}