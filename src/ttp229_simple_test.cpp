/*
Simple TTP229 Test - Check if module is working
Tests basic touch detection without gestures
*/

#include <Wire.h>

// TTP229 Configuration
#define TTP229_ADDR 0x57
#define I2C_SDA 1
#define I2C_SCL 2

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("╔════════════════════════════════════════╗");
  Serial.println("║     TTP229 Simple Touch Test          ║");
  Serial.println("╚════════════════════════════════════════╝");
  Serial.println();
  
  // Initialize I2C
  Wire.begin(I2C_SDA, I2C_SCL);
  Wire.setClock(100000); // 100kHz
  
  Serial.print("I2C initialized - SDA: GPIO");
  Serial.print(I2C_SDA);
  Serial.print(", SCL: GPIO");
  Serial.println(I2C_SCL);
  Serial.println();
  
  // Test connection
  Serial.println("Testing TTP229 connection...");
  Wire.beginTransmission(TTP229_ADDR);
  uint8_t error = Wire.endTransmission();
  
  if (error == 0) {
    Serial.println("✅ TTP229 found at address 0x57");
  } else {
    Serial.print("❌ TTP229 NOT found! Error: ");
    Serial.println(error);
    Serial.println();
    Serial.println("Check wiring:");
    Serial.println("  VCC → 3.3V");
    Serial.println("  GND → GND");
    Serial.println("  SCL → GPIO2");
    Serial.println("  SDO → GPIO1");
    Serial.println();
    Serial.println("Trying anyway...");
  }
  Serial.println();
  Serial.println("Touch any key (1-16)...");
  Serial.println();
}

void loop() {
  // Read TTP229
  Wire.requestFrom(TTP229_ADDR, 2);
  
  if (Wire.available() >= 2) {
    uint8_t lowByte = Wire.read();
    uint8_t highByte = Wire.read();
    uint16_t touchStatus = (highByte << 8) | lowByte;
    
    // Print if any key is touched
    if (touchStatus != 0) {
      Serial.print("Touch detected! Raw value: 0x");
      Serial.print(touchStatus, HEX);
      Serial.print(" (");
      Serial.print(touchStatus, BIN);
      Serial.print(") - Keys: ");
      
      // Show which keys are pressed
      for (int i = 0; i < 16; i++) {
        if ((touchStatus >> i) & 0x01) {
          Serial.print(i + 1);
          Serial.print(" ");
        }
      }
      Serial.println();
    }
  } else {
    Serial.println("⚠️ No data from TTP229!");
  }
  
  delay(100); // 10Hz polling
}
