/*
TTP229 Full Diagnostic
Tests everything - shows raw pin states, tries all modes
*/

#define PIN1 1
#define PIN2 2

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("╔════════════════════════════════════════╗");
  Serial.println("║   TTP229 FULL DIAGNOSTIC               ║");
  Serial.println("╚════════════════════════════════════════╝");
  Serial.println();
  Serial.println("This will show EVERYTHING happening");
  Serial.println();
  
  // Test 1: Check if Serial Monitor is working
  Serial.println("✅ Serial Monitor is working!");
  Serial.println();
  
  // Test 2: Check pin states
  Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
  Serial.println("TEST 1: Raw Pin States");
  Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
  
  pinMode(PIN1, INPUT);
  pinMode(PIN2, INPUT);
  
  Serial.print("GPIO1 state: ");
  Serial.println(digitalRead(PIN1) ? "HIGH" : "LOW");
  Serial.print("GPIO2 state: ");
  Serial.println(digitalRead(PIN2) ? "HIGH" : "LOW");
  Serial.println();
  
  Serial.println("Touch a key and watch for changes...");
  Serial.println();
}

void loop() {
  static int counter = 0;
  counter++;
  
  // Every 10 loops (0.5 seconds), show status
  if (counter % 10 == 0) {
    Serial.print("Loop ");
    Serial.print(counter / 10);
    Serial.print(": ");
    
    // Read both pins
    pinMode(PIN1, INPUT);
    pinMode(PIN2, INPUT);
    
    int pin1 = digitalRead(PIN1);
    int pin2 = digitalRead(PIN2);
    
    Serial.print("GPIO1=");
    Serial.print(pin1);
    Serial.print(" GPIO2=");
    Serial.print(pin2);
    
    // Try serial read
    pinMode(PIN1, INPUT);
    pinMode(PIN2, OUTPUT);
    uint16_t serialVal = readSerial();
    
    Serial.print(" | Serial=0x");
    Serial.print(serialVal, HEX);
    
    if (serialVal != 0 && serialVal != 0xFFFF) {
      Serial.print(" ← TOUCH! Keys: ");
      for (int i = 0; i < 16; i++) {
        if ((serialVal >> i) & 0x01) {
          Serial.print(i + 1);
          Serial.print(" ");
        }
      }
    }
    
    Serial.println();
  }
  
  delay(50);
}

uint16_t readSerial() {
  uint16_t value = 0;
  
  digitalWrite(PIN2, HIGH);
  delayMicroseconds(100);
  
  for (int i = 0; i < 16; i++) {
    digitalWrite(PIN2, LOW);
    delayMicroseconds(5);
    
    value <<= 1;
    if (digitalRead(PIN1)) {
      value |= 1;
    }
    
    digitalWrite(PIN2, HIGH);
    delayMicroseconds(5);
  }
  
  return value;
}
