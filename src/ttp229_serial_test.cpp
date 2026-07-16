/*
TTP229 Serial Mode Test
For modules with SCL and SDO pins (not I2C)
*/

// TTP229 Serial Mode Pins
#define TTP_SDO 1   // Serial Data Out (GPIO1)
#define TTP_SCL 2   // Serial Clock (GPIO2)

uint16_t lastTouch = 0;

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("╔════════════════════════════════════════╗");
  Serial.println("║   TTP229 Serial Mode Test             ║");
  Serial.println("║   SCL + SDO (not I2C)                 ║");
  Serial.println("╚════════════════════════════════════════╝");
  Serial.println();
  
  // Setup pins
  pinMode(TTP_SDO, INPUT);   // Data input
  pinMode(TTP_SCL, OUTPUT);  // Clock output
  digitalWrite(TTP_SCL, HIGH);
  
  Serial.print("SDO (Data): GPIO");
  Serial.println(TTP_SDO);
  Serial.print("SCL (Clock): GPIO");
  Serial.println(TTP_SCL);
  Serial.println();
  Serial.println("🔴 Red light = Module powered ✅");
  Serial.println();
  Serial.println("Touch any key (1-16)...");
  Serial.println();
}

void loop() {
  uint16_t touchStatus = readTTP229Serial();
  
  // Only print when touch changes
  if (touchStatus != lastTouch) {
    if (touchStatus != 0) {
      Serial.print("👆 Touch detected! Raw: 0x");
      Serial.print(touchStatus, HEX);
      Serial.print(" - Keys: ");
      
      // Show which keys
      for (int i = 0; i < 16; i++) {
        if ((touchStatus >> i) & 0x01) {
          Serial.print(i + 1);
          Serial.print(" ");
        }
      }
      Serial.println();
    }
    lastTouch = touchStatus;
  }
  
  delay(50); // 20Hz polling
}

uint16_t readTTP229Serial() {
  uint16_t value = 0;
  
  // Wait for data ready (DV goes low)
  delayMicroseconds(100);
  
  // Read 16 bits
  for (int i = 0; i < 16; i++) {
    // Clock low
    digitalWrite(TTP_SCL, LOW);
    delayMicroseconds(2);
    
    // Read bit
    value <<= 1;
    if (digitalRead(TTP_SDO) == HIGH) {
      value |= 1;
    }
    
    // Clock high
    digitalWrite(TTP_SCL, HIGH);
    delayMicroseconds(2);
  }
  
  return value;
}
