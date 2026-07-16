/*
TTP229 Mode Test - Try both I2C and Serial modes
Some TTP229 modules default to serial mode, not I2C
*/

#include <Wire.h>

// Pin definitions to test
#define TEST_SDA 1
#define TEST_SCL 2

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("╔════════════════════════════════════════╗");
  Serial.println("║    TTP229 Mode Detection Test         ║");
  Serial.println("╚════════════════════════════════════════╝");
  Serial.println();
  Serial.println("🔴 Red light = Module has power ✅");
  Serial.println();
  
  // Test I2C Mode
  Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
  Serial.println("TEST 1: I2C Mode (Address 0x57)");
  Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
  testI2CMode();
  
  delay(1000);
  
  // Test Serial Mode
  Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
  Serial.println("TEST 2: Serial Mode (16-bit output)");
  Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
  testSerialMode();
  
  Serial.println();
  Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
  Serial.println("CONTINUOUS MONITORING");
  Serial.println("Touch any key to see which mode works...");
  Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
  Serial.println();
}

void loop() {
  // Try I2C
  Wire.requestFrom(0x57, 2);
  if (Wire.available() >= 2) {
    uint8_t low = Wire.read();
    uint8_t high = Wire.read();
    uint16_t value = (high << 8) | low;
    
    if (value != 0) {
      Serial.print("I2C: 0x");
      Serial.print(value, HEX);
      Serial.print(" - Keys: ");
      for (int i = 0; i < 16; i++) {
        if ((value >> i) & 0x01) {
          Serial.print(i + 1);
          Serial.print(" ");
        }
      }
      Serial.println();
    }
  }
  
  // Try Serial Mode
  pinMode(TEST_SDA, INPUT);
  pinMode(TEST_SCL, OUTPUT);
  
  uint16_t serialValue = readSerialMode();
  if (serialValue != 0) {
    Serial.print("SERIAL: 0x");
    Serial.print(serialValue, HEX);
    Serial.print(" - Keys: ");
    for (int i = 0; i < 16; i++) {
      if ((serialValue >> i) & 0x01) {
        Serial.print(i + 1);
        Serial.print(" ");
      }
    }
    Serial.println();
  }
  
  delay(100);
}

void testI2CMode() {
  Wire.begin(TEST_SDA, TEST_SCL);
  Wire.setClock(100000);
  
  Serial.print("Pins: SDA=GPIO");
  Serial.print(TEST_SDA);
  Serial.print(", SCL=GPIO");
  Serial.println(TEST_SCL);
  
  delay(100);
  
  // Try to detect at 0x57
  Wire.beginTransmission(0x57);
  byte error = Wire.endTransmission();
  
  if (error == 0) {
    Serial.println("✅ I2C device found at 0x57!");
    Serial.println("   Your TTP229 is in I2C mode");
    Serial.println("   Use the I2C code");
  } else {
    Serial.print("❌ No I2C device at 0x57 (Error: ");
    Serial.print(error);
    Serial.println(")");
    Serial.println("   Module might be in Serial mode");
  }
  Serial.println();
}

void testSerialMode() {
  pinMode(TEST_SDA, INPUT);   // SDO = Data Out
  pinMode(TEST_SCL, OUTPUT);  // SCL = Clock
  
  Serial.print("Pins: SDO=GPIO");
  Serial.print(TEST_SDA);
  Serial.print(", SCL=GPIO");
  Serial.println(TEST_SCL);
  
  // Try to read in serial mode
  uint16_t value = readSerialMode();
  
  if (value != 0xFFFF && value != 0x0000) {
    Serial.println("✅ Serial mode detected!");
    Serial.print("   Value: 0x");
    Serial.println(value, HEX);
    Serial.println("   Your TTP229 is in Serial mode");
    Serial.println("   Use the Serial code");
  } else {
    Serial.println("❌ No valid serial data");
    Serial.println("   Module might be in I2C mode");
  }
  Serial.println();
}

uint16_t readSerialMode() {
  uint16_t value = 0;
  
  // Clock in 16 bits
  for (int i = 0; i < 16; i++) {
    digitalWrite(TEST_SCL, LOW);
    delayMicroseconds(10);
    
    value <<= 1;
    if (digitalRead(TEST_SDA)) {
      value |= 1;
    }
    
    digitalWrite(TEST_SCL, HIGH);
    delayMicroseconds(10);
  }
  
  return value;
}
