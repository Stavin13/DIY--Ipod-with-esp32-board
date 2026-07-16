/*
Advanced Touch Probe - Try different XPT2046 configurations
Some modules need different command sequences or power modes
*/

#define TOUCH_CS  6
#define TOUCH_CLK 7
#define TOUCH_DIN 5
#define TOUCH_DO  4

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("╔════════════════════════════════════════╗");
  Serial.println("║    Advanced Touch Probe v1.0          ║");
  Serial.println("║  Trying Alternative XPT2046 Configs   ║");
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
  
  Serial.println("🔧 TRYING DIFFERENT XPT2046 CONFIGURATIONS:");
  Serial.println();
  
  tryDifferentPowerModes();
  tryDifferentReferenceModes();
  tryDifferentSPISpeeds();
  tryTouchWithDelay();
  
  Serial.println("🏥 FINAL ASSESSMENT:");
  provideFinalAssessment();
}

void loop() {
  // Monitor with different configurations
  monitorWithBestConfig();
  delay(1000);
}

void tryDifferentPowerModes() {
  Serial.println("⚡ Testing different power modes...");
  
  // XPT2046 power mode bits:
  // Bit 0: PD0 (Power Down)
  // Bit 1: PD1 (Power Down)
  // 00 = Power down between conversions
  // 01 = Reference off, ADC on
  // 10 = Reference on, ADC off  
  // 11 = Always powered (best for touch)
  
  uint8_t xCommands[] = {0x90, 0x91, 0x92, 0x93};  // X with different power modes
  uint8_t yCommands[] = {0xD0, 0xD1, 0xD2, 0xD3};  // Y with different power modes
  String powerModes[] = {"PwrDown", "RefOff", "RefOn", "AlwaysOn"};
  
  for (int i = 0; i < 4; i++) {
    uint16_t x = sendTouchCommand(xCommands[i]);
    uint16_t y = sendTouchCommand(yCommands[i]);
    
    Serial.print("   ");
    Serial.print(powerModes[i]);
    Serial.print(": X=");
    Serial.print(x);
    Serial.print(" Y=");
    Serial.print(y);
    
    if (x > 100 || y > 100) {
      Serial.println(" ✅ Got response!");
    } else {
      Serial.println(" ❌ No response");
    }
  }
  Serial.println();
}

void tryDifferentReferenceModes() {
  Serial.println("🔋 Testing different reference modes...");
  
  // XPT2046 reference mode bits:
  // Bit 2: SER/DFR (Single-ended/Differential)
  // 0 = Differential (better noise immunity)
  // 1 = Single-ended (simpler)
  
  uint8_t xDifferential[] = {0x90, 0x91, 0x92, 0x93};  // X differential
  uint8_t xSingleEnded[] = {0x94, 0x95, 0x96, 0x97};   // X single-ended
  uint8_t yDifferential[] = {0xD0, 0xD1, 0xD2, 0xD3};  // Y differential  
  uint8_t ySingleEnded[] = {0xD4, 0xD5, 0xD6, 0xD7};   // Y single-ended
  
  Serial.println("   Differential mode:");
  for (int i = 0; i < 4; i++) {
    uint16_t x = sendTouchCommand(xDifferential[i]);
    uint16_t y = sendTouchCommand(yDifferential[i]);
    Serial.print("     Power mode ");
    Serial.print(i);
    Serial.print(": X=");
    Serial.print(x);
    Serial.print(" Y=");
    Serial.println(y);
  }
  
  Serial.println("   Single-ended mode:");
  for (int i = 0; i < 4; i++) {
    uint16_t x = sendTouchCommand(xSingleEnded[i]);
    uint16_t y = sendTouchCommand(ySingleEnded[i]);
    Serial.print("     Power mode ");
    Serial.print(i);
    Serial.print(": X=");
    Serial.print(x);
    Serial.print(" Y=");
    Serial.println(y);
  }
  Serial.println();
}

void tryDifferentSPISpeeds() {
  Serial.println("🔄 Testing different SPI timing...");
  
  // Try slower SPI communication
  Serial.println("   Slow SPI (10µs delays):");
  uint16_t x1 = sendTouchCommandSlow(0x90, 10);
  uint16_t y1 = sendTouchCommandSlow(0xD0, 10);
  Serial.print("     X=");
  Serial.print(x1);
  Serial.print(" Y=");
  Serial.println(y1);
  
  Serial.println("   Very slow SPI (50µs delays):");
  uint16_t x2 = sendTouchCommandSlow(0x90, 50);
  uint16_t y2 = sendTouchCommandSlow(0xD0, 50);
  Serial.print("     X=");
  Serial.print(x2);
  Serial.print(" Y=");
  Serial.println(y2);
  
  Serial.println("   Multiple samples:");
  for (int i = 0; i < 5; i++) {
    uint16_t x = sendTouchCommand(0x93);  // Always-on power mode
    uint16_t y = sendTouchCommand(0xD3);
    Serial.print("     Sample ");
    Serial.print(i + 1);
    Serial.print(": X=");
    Serial.print(x);
    Serial.print(" Y=");
    Serial.println(y);
    delay(100);
  }
  Serial.println();
}

void tryTouchWithDelay() {
  Serial.println("⏱️  Testing with settling delays...");
  Serial.println("   Press and hold the screen firmly...");
  
  for (int i = 0; i < 10; i++) {
    // Power up the controller first
    sendTouchCommand(0x83);  // Power-up command
    delay(10);
    
    // Take multiple samples
    uint16_t x = sendTouchCommand(0x93);  // X with always-on power
    delay(5);
    uint16_t y = sendTouchCommand(0xD3);  // Y with always-on power
    delay(5);
    uint16_t z1 = sendTouchCommand(0xB3); // Z1 with always-on power
    delay(5);
    uint16_t z2 = sendTouchCommand(0xC3); // Z2 with always-on power
    
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
    
    if (x > 100 || y > 100 || z1 > 100) {
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

uint16_t sendTouchCommandSlow(uint8_t command, int delayUs) {
  digitalWrite(TOUCH_CS, LOW);
  delayMicroseconds(delayUs);
  
  // Send 8-bit command
  for (int i = 7; i >= 0; i--) {
    digitalWrite(TOUCH_DIN, (command >> i) & 1);
    delayMicroseconds(delayUs);
    digitalWrite(TOUCH_CLK, HIGH);
    delayMicroseconds(delayUs);
    digitalWrite(TOUCH_CLK, LOW);
    delayMicroseconds(delayUs);
  }
  
  // Read 12-bit response
  uint16_t result = 0;
  for (int i = 0; i < 12; i++) {
    digitalWrite(TOUCH_CLK, HIGH);
    delayMicroseconds(delayUs);
    result = (result << 1) | digitalRead(TOUCH_DO);
    digitalWrite(TOUCH_CLK, LOW);
    delayMicroseconds(delayUs);
  }
  
  digitalWrite(TOUCH_CS, HIGH);
  delayMicroseconds(delayUs);
  
  return result;
}

void provideFinalAssessment() {
  Serial.println("==================");
  
  // Test the most promising configuration
  uint16_t x = sendTouchCommand(0x93);  // X, always-on power
  uint16_t y = sendTouchCommand(0xD3);  // Y, always-on power
  uint16_t z2 = sendTouchCommand(0xC3); // Z2, always-on power
  
  Serial.println("📊 FINAL ASSESSMENT:");
  Serial.print("   Best config test: X=");
  Serial.print(x);
  Serial.print(" Y=");
  Serial.print(y);
  Serial.print(" Z2=");
  Serial.println(z2);
  
  if (x > 100 || y > 100) {
    Serial.println("   ✅ TOUCH IS WORKING with alternative configuration!");
    Serial.println("   🔧 Use commands 0x93 (X) and 0xD3 (Y) with always-on power");
  } else if (z2 > 1000) {
    Serial.println("   ⚠️  Touch controller works but touch surface not connected");
    Serial.println("   💡 Your MSP2806 has XPT2046 chip but touch lines aren't wired to screen");
    Serial.println("   🛒 Consider getting a different display module with working touch");
  } else {
    Serial.println("   ❌ Touch sensing not functional on this module");
    Serial.println("   📱 This MSP2806 variant doesn't support touch input");
  }
  
  Serial.println();
  Serial.println("🎯 RECOMMENDATIONS:");
  if (x == 0 && y == 0) {
    Serial.println("   1. Your MSP2806 module has touch controller chip but no touch sensing");
    Serial.println("   2. Touch pins exist but aren't connected to the screen surface");
    Serial.println("   3. This is common with budget display modules");
    Serial.println("   4. Solutions:");
    Serial.println("      • Buy a different MSP2806/ILI9341 module with confirmed touch");
    Serial.println("      • Add external resistive touch overlay");
    Serial.println("      • Use physical buttons/encoder for input");
    Serial.println("      • Switch to capacitive touch display");
  }
}

void monitorWithBestConfig() {
  static int count = 0;
  count++;
  
  // Use the best configuration found
  uint16_t x = sendTouchCommand(0x93);  // X, always-on power
  uint16_t y = sendTouchCommand(0xD3);  // Y, always-on power
  
  Serial.print("Monitor ");
  Serial.print(count);
  Serial.print(" (best config): X=");
  Serial.print(x);
  Serial.print(" Y=");
  Serial.print(y);
  
  if (x > 100 || y > 100) {
    Serial.println(" ✅ TOUCH!");
  } else {
    Serial.println(" ❌ No touch");
  }
}