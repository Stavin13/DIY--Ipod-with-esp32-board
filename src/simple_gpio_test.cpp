// Simple GPIO Test - Test ESP32-S3 pins before display
// This will help verify your ESP32-S3 is working correctly

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("=== ESP32-S3 GPIO Test ===");
  Serial.println("Testing display pins...");
  
  // Define display pins
  int pins[] = {8, 9, 10, 11, 12, 13};  // RST, DC, CS, MOSI, SCLK, MISO
  String pinNames[] = {"RST(8)", "DC(9)", "CS(10)", "MOSI(11)", "SCLK(12)", "MISO(13)"};
  
  // Set pins as outputs (except MISO)
  for(int i = 0; i < 6; i++) {
    if(i != 5) {  // Skip MISO (input)
      pinMode(pins[i], OUTPUT);
      Serial.println("Pin " + pinNames[i] + " set as OUTPUT");
    } else {
      pinMode(pins[i], INPUT);
      Serial.println("Pin " + pinNames[i] + " set as INPUT");
    }
  }
  
  Serial.println("\nStarting pin toggle test...");
  Serial.println("Watch with multimeter or LED:");
}

void loop() {
  static unsigned long lastToggle = 0;
  static bool pinState = false;
  static int currentPin = 0;
  
  if (millis() - lastToggle > 1000) {
    // Skip MISO pin (input only)
    if(currentPin == 5) currentPin++;
    if(currentPin >= 6) currentPin = 0;
    
    // Toggle current pin
    if(currentPin != 5) {  // Don't toggle MISO
      int pins[] = {8, 9, 10, 11, 12, 13};
      String pinNames[] = {"RST(8)", "DC(9)", "CS(10)", "MOSI(11)", "SCLK(12)", "MISO(13)"};
      
      digitalWrite(pins[currentPin], pinState ? HIGH : LOW);
      Serial.println("Pin " + pinNames[currentPin] + " = " + (pinState ? "HIGH" : "LOW"));
      
      pinState = !pinState;
      
      // Move to next pin every 2 toggles
      if(!pinState) {
        currentPin++;
        if(currentPin == 5) currentPin++;  // Skip MISO
        if(currentPin >= 6) {
          currentPin = 0;
          Serial.println("--- Cycle complete ---");
        }
      }
    }
    
    lastToggle = millis();
  }
}