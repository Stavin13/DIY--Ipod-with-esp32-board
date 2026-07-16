/*
Touch Controller Probe - Direct XPT2046 Communication Test
Tests if the touch controller chip is actually responding
*/

#define TOUCH_CS  6
#define TOUCH_CLK 7
#define TOUCH_DIN 5
#define TOUCH_DO  4

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("╔════════════════════════════════════════╗");
  Serial.println("║    Touch Controller Probe v1.0        ║");
  Serial.println("║   Direct XPT2046 Communication Test   ║");
  Serial.println("╚════════════════════════════════════════╝");
  Serial.println();
  
  // Configure pins
  pinMode(TOUCH_CS, OUTPUT);
  pinMode(TOUCH_CLK, OUTPUT);
  pinMode(TOUCH_DIN, OUTPUT);
  pinMode(TOUCH_DO, INPUT);
  
  digitalWrite(TOUCH_CS, HIGH);
  digitalWrite(TOUCH_CLK, LOW);
  digitalWrite(TOUCH_DIN, LOW);
  
  Serial.println("🔍 TESTING TOUCH CONTROLLER COMMUNICATION:");
  Serial.println();
  
  testTouchControllerPresence();
  testAllTouchCommands();
  testTouchWithPressure();
  
  Serial.println("🏥 FINAL DIAGNOSIS:");
  provideFinalDiagnosis();
}

void loop() {
  // Continuous monitoring
  monitorTouchController();
  delay(500);
}

void testTouchControllerPresence() {
  Serial.println("📡 Testing if XPT2046 touch controller is present...");
  
  // Test multiple commands to see if we get any response
  uint16_t responses[8];
  uint8_t commands[] = {0x90, 0xD0, 0xB0, 0xC0, 0xA0, 0xE0, 0x80, 0xF0};
  String commandNames[] = {"X", "Y", "Z1", "Z2", "TEMP0", "TEMP1", "VBAT", "AUX"};
  
  for (int i = 0; i < 8; i++) {
    responses[i] = sendTouchCommand(commands[i]);
    Serial.print("   ");
    Serial.print(commandNames[i]);
    Serial.print(" (0x");
    Serial.print(commands[i], HEX);
    Serial.print("): ");
    Serial.print(responses[i]);
    Serial.print(" (0x");
    Serial.print(responses[i], HEX);
    Serial.println(")");
    delay(10);
  }
  
  Serial.println();
  
  // Analyze responses
  bool allZero = true;
  bool allMax = true;
  bool hasVariation = false;
  
  for (int i = 0; i < 8; i++) {
    if (responses[i] != 0) allZero = false;
    if (responses[i] != 0xFFF) allMax = false;
    if (i > 0 && responses[i] != responses[0]) hasVariation = true;
  }
  
  Serial.println("📊 RESPONSE ANALYSIS:");
  if (allZero) {
    Serial.println("   ❌ All responses are 0 - Touch controller not responding");
    Serial.println("      Possible causes:");
    Serial.println("      • Touch controller chip not populated on module");
    Serial.println("      • Touch controller not powered");
    Serial.println("      • SPI communication failure");
  } else if (allMax) {
    Serial.println("   ❌ All responses are 0xFFF - SPI lines stuck HIGH");
    Serial.println("      Possible causes:");
    Serial.println("      • T_DO pin not connected");
    Serial.println("      • Touch controller in wrong state");
  } else if (!hasVariation) {
    Serial.println("   ⚠️  All responses identical - Unusual behavior");
    Serial.println("      Touch controller may be present but not functioning normally");
  } else {
    Serial.println("   ✅ Touch controller is responding with varied data");
    Serial.println("      Controller appears to be functional");
  }
  Serial.println();
}

void testAllTouchCommands() {
  Serial.println("🎯 Testing all XPT2046 command variations...");
  
  // Test different reference modes and power settings
  uint8_t testCommands[] = {
    0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,  // X variations
    0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7   // Y variations
  };
  
  for (int i = 0; i < 16; i++) {
    uint16_t response = sendTouchCommand(testCommands[i]);
    Serial.print("   Command 0x");
    Serial.print(testCommands[i], HEX);
    Serial.print(": ");
    Serial.println(response);
    
    if (response > 0 && response < 0xFFF) {
      Serial.println("      ✅ Got valid response!");
    }
    delay(5);
  }
  Serial.println();
}

void testTouchWithPressure() {
  Serial.println("👆 Testing touch with manual pressure...");
  Serial.println("   Press firmly on the screen for 10 seconds...");
  
  for (int i = 0; i < 20; i++) {
    uint16_t x = sendTouchCommand(0x90);
    uint16_t y = sendTouchCommand(0xD0);
    uint16_t z1 = sendTouchCommand(0xB0);
    uint16_t z2 = sendTouchCommand(0xC0);
    
    Serial.print("   Sample ");
    Serial.print(i + 1);
    Serial.print(": X=");
    Serial.print(x);
    Serial.print(" Y=");
    Serial.print(y);
    Serial.print(" Z1=");
    Serial.print(z1);
    Serial.print(" Z2=");
    Serial.print(z2);
    
    if (x > 100 || y > 100 || z1 > 100 || z2 > 100) {
      Serial.println(" ✅ TOUCH DETECTED!");
    } else {
      Serial.println(" ❌ No touch");
    }
    
    delay(500);
  }
  Serial.println();
}

uint16_t sendTouchCommand(uint8_t command) {
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

void provideFinalDiagnosis() {
  Serial.println("==================");
  
  // Test a few commands to determine status
  uint16_t x = sendTouchCommand(0x90);
  uint16_t y = sendTouchCommand(0xD0);
  uint16_t z1 = sendTouchCommand(0xB0);
  
  if (x == 0 && y == 0 && z1 == 0) {
    Serial.println("❌ TOUCH CONTROLLER NOT FUNCTIONAL");
    Serial.println();
    Serial.println("🔧 SOLUTIONS:");
    Serial.println("1. Your MSP2806 module likely doesn't have a working touch controller");
    Serial.println("2. Consider these options:");
    Serial.println("   • Buy a different MSP2806 module with confirmed working touch");
    Serial.println("   • Use an external XPT2046 touch overlay");
    Serial.println("   • Switch to a capacitive touch display");
    Serial.println("   • Use the display without touch (buttons/encoder instead)");
    Serial.println();
    Serial.println("🛒 RECOMMENDED MODULES:");
    Serial.println("   • Look for modules explicitly labeled 'with touch'");
    Serial.println("   • Check seller reviews mentioning working touch");
    Serial.println("   • Consider ILI9341 modules from different suppliers");
    
  } else if (x == 0xFFF && y == 0xFFF) {
    Serial.println("⚠️  SPI COMMUNICATION ISSUE");
    Serial.println("   Touch controller may be present but T_DO pin issue");
    Serial.println("   Try different T_DO connection or check for shorts");
    
  } else {
    Serial.println("✅ TOUCH CONTROLLER IS WORKING!");
    Serial.println("   Hardware is functional - issue may be software configuration");
    Serial.println("   Try touch calibration or different touch settings");
  }
}

void monitorTouchController() {
  static int count = 0;
  count++;
  
  uint16_t x = sendTouchCommand(0x90);
  uint16_t y = sendTouchCommand(0xD0);
  
  Serial.print("Monitor ");
  Serial.print(count);
  Serial.print(": X=");
  Serial.print(x);
  Serial.print(" Y=");
  Serial.print(y);
  
  if (x > 100 || y > 100) {
    Serial.println(" ✅ ACTIVITY DETECTED!");
  } else {
    Serial.println(" ❌ No response");
  }
}