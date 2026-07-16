/*
I2C Scanner - Find all I2C devices
This will scan all possible I2C addresses and show what's connected
*/

#include <Wire.h>

// Try different pin combinations
void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("╔════════════════════════════════════════╗");
  Serial.println("║         I2C Scanner for ESP32-S3       ║");
  Serial.println("╚════════════════════════════════════════╝");
  Serial.println();
  
  // Test multiple pin combinations
  testI2CPins(1, 2);   // GPIO1=SDA, GPIO2=SCL
  testI2CPins(2, 1);   // GPIO2=SDA, GPIO1=SCL (swapped)
  testI2CPins(8, 9);   // Alternative pins
  testI2CPins(41, 42); // Alternative pins
  
  Serial.println();
  Serial.println("Scan complete!");
  Serial.println();
  Serial.println("If TTP229 found:");
  Serial.println("  1. Note the SDA/SCL pins that worked");
  Serial.println("  2. Note the I2C address");
  Serial.println("  3. Update your code with correct pins");
}

void loop() {
  // Nothing
}

void testI2CPins(int sda, int scl) {
  Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
  Serial.print("Testing SDA=GPIO");
  Serial.print(sda);
  Serial.print(", SCL=GPIO");
  Serial.println(scl);
  Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
  
  Wire.begin(sda, scl);
  Wire.setClock(100000);
  
  delay(100);
  
  int devicesFound = 0;
  
  Serial.println("Scanning I2C addresses 0x01 to 0x7F...");
  Serial.println();
  
  for (byte address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();
    
    if (error == 0) {
      Serial.print("✅ Device found at address 0x");
      if (address < 16) Serial.print("0");
      Serial.print(address, HEX);
      Serial.print(" (");
      Serial.print(address);
      Serial.print(")");
      
      // Identify known devices
      if (address == 0x57) {
        Serial.print(" ← TTP229 Touch Sensor!");
      } else if (address == 0x5A) {
        Serial.print(" ← DRV2605L Haptic Driver");
      } else if (address == 0x38 || address == 0x48) {
        Serial.print(" ← FT6236 Touch Controller");
      }
      
      Serial.println();
      devicesFound++;
    }
  }
  
  Serial.println();
  if (devicesFound == 0) {
    Serial.println("❌ No I2C devices found on these pins");
  } else {
    Serial.print("✅ Found ");
    Serial.print(devicesFound);
    Serial.println(" device(s)");
  }
  Serial.println();
  
  Wire.end();
  delay(500);
}
