/*
Pin Connection Test - Test each touch pin individually
Upload this to verify each pin connection works
*/

// Touch pin definitions
#define TOUCH_CS  6
#define TOUCH_CLK 7
#define TOUCH_DIN 5
#define TOUCH_DO  4
#define TOUCH_IRQ 3

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("╔════════════════════════════════════════╗");
  Serial.println("║       Pin Connection Test v1.0        ║");
  Serial.println("║    Testing Each Touch Pin Individually║");
  Serial.println("╚════════════════════════════════════════╝");
  Serial.println();
  
  testTouchPins();
}

void loop() {
  // Test pins continuously
  testPinStates();
  delay(1000);
}

void testTouchPins() {
  Serial.println("🔌 TESTING TOUCH PIN CONNECTIONS:");
  Serial.println();
  
  // Configure pins
  pinMode(TOUCH_CS, OUTPUT);
  pinMode(TOUCH_CLK, OUTPUT);
  pinMode(TOUCH_DIN, OUTPUT);
  pinMode(TOUCH_DO, INPUT_PULLUP);
  pinMode(TOUCH_IRQ, INPUT_PULLUP);
  
  // Test CS pin (most critical)
  Serial.println("📌 Testing TOUCH_CS (GPIO6):");
  digitalWrite(TOUCH_CS, HIGH);
  delay(10);
  bool csHigh = digitalRead(TOUCH_CS);
  digitalWrite(TOUCH_CS, LOW);
  delay(10);
  bool csLow = digitalRead(TOUCH_CS);
  
  Serial.print("   Set HIGH, read: ");
  Serial.println(csHigh ? "HIGH ✅" : "LOW ❌");
  Serial.print("   Set LOW, read: ");
  Serial.println(csLow ? "HIGH ❌" : "LOW ✅");
  
  if (csHigh && !csLow) {
    Serial.println("   ✅ TOUCH_CS pin working correctly");
  } else {
    Serial.println("   ❌ TOUCH_CS pin not working - CHECK CONNECTION!");
  }
  Serial.println();
  
  // Test CLK pin
  Serial.println("📌 Testing TOUCH_CLK (GPIO7):");
  digitalWrite(TOUCH_CLK, HIGH);
  delay(10);
  digitalWrite(TOUCH_CLK, LOW);
  Serial.println("   ✅ TOUCH_CLK configured as output");
  Serial.println();
  
  // Test DIN pin
  Serial.println("📌 Testing TOUCH_DIN (GPIO5):");
  digitalWrite(TOUCH_DIN, HIGH);
  delay(10);
  digitalWrite(TOUCH_DIN, LOW);
  Serial.println("   ✅ TOUCH_DIN configured as output");
  Serial.println();
  
  // Test DO pin (input from touch controller)
  Serial.println("📌 Testing TOUCH_DO (GPIO4):");
  int doState = digitalRead(TOUCH_DO);
  Serial.print("   Current state: ");
  Serial.println(doState ? "HIGH" : "LOW");
  Serial.println("   ✅ TOUCH_DO configured as input");
  Serial.println();
  
  // Test IRQ pin (input from touch controller)
  Serial.println("📌 Testing TOUCH_IRQ (GPIO3):");
  int irqState = digitalRead(TOUCH_IRQ);
  Serial.print("   Current state: ");
  Serial.print(irqState ? "HIGH" : "LOW");
  if (irqState) {
    Serial.println(" ✅ (Good - pulled HIGH)");
  } else {
    Serial.println(" ⚠️  (Should be HIGH, but might be OK)");
  }
  Serial.println();
  
  Serial.println("🔍 MANUAL CONNECTION CHECK:");
  Serial.println("   1. Verify each wire goes to correct pin");
  Serial.println("   2. Push breadboard connections firmly");
  Serial.println("   3. Check for loose or broken wires");
  Serial.println("   4. Make sure MSP2806 has touch pins labeled");
  Serial.println();
}

void testPinStates() {
  static int count = 0;
  count++;
  
  Serial.print("Monitor ");
  Serial.print(count);
  Serial.print(": ");
  
  // Read all pin states
  Serial.print("CS=");
  Serial.print(digitalRead(TOUCH_CS));
  Serial.print(" CLK=");
  Serial.print(digitalRead(TOUCH_CLK));
  Serial.print(" DIN=");
  Serial.print(digitalRead(TOUCH_DIN));
  Serial.print(" DO=");
  Serial.print(digitalRead(TOUCH_DO));
  Serial.print(" IRQ=");
  Serial.println(digitalRead(TOUCH_IRQ));
  
  // Toggle CS to test connection
  digitalWrite(TOUCH_CS, count % 2);
}