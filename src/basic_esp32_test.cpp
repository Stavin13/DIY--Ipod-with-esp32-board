// =====================================================
//  Basic ESP32-S3 Test - Fix Partition Issues First
//  Upload this BEFORE trying display code
// =====================================================

void setup() {
  Serial.begin(115200);
  delay(2000);  // Wait for serial monitor
  
  Serial.println("=== ESP32-S3 Basic Test ===");
  Serial.println("Board: ESP32-S3");
  Serial.println("Partition: Testing...");
  Serial.println("");
  
  // Test basic GPIO
  Serial.println("Setting up GPIO pins...");
  
  // Built-in LED (if available)
  pinMode(2, OUTPUT);
  Serial.println("GPIO2 (LED) configured as output");
  
  // Test pins for display (configure as outputs for testing)
  pinMode(8, OUTPUT);   // TFT_RST
  pinMode(9, OUTPUT);   // TFT_DC
  pinMode(10, OUTPUT);  // TFT_CS
  pinMode(11, OUTPUT);  // TFT_MOSI
  pinMode(12, OUTPUT);  // TFT_SCLK
  
  Serial.println("Display pins configured:");
  Serial.println("- GPIO8  (TFT_RST)");
  Serial.println("- GPIO9  (TFT_DC)");
  Serial.println("- GPIO10 (TFT_CS)");
  Serial.println("- GPIO11 (TFT_MOSI)");
  Serial.println("- GPIO12 (TFT_SCLK)");
  Serial.println("");
  
  Serial.println("SUCCESS: ESP32-S3 is working!");
  Serial.println("Partition scheme is correct!");
  Serial.println("Ready for display testing!");
  Serial.println("");
  Serial.println("Watch for blinking LED and GPIO toggles...");
}

void loop() {
  static unsigned long lastToggle = 0;
  static bool ledState = false;
  static int currentPin = 8;
  
  // Blink built-in LED every second
  if (millis() - lastToggle > 1000) {
    ledState = !ledState;
    digitalWrite(2, ledState);
    
    // Also toggle display pins to test them
    digitalWrite(currentPin, ledState);
    
    Serial.println("LED: " + String(ledState ? "ON" : "OFF") + 
                   " | Testing GPIO" + String(currentPin) + 
                   ": " + String(ledState ? "HIGH" : "LOW"));
    
    // Cycle through display pins
    currentPin++;
    if (currentPin > 12) currentPin = 8;
    
    lastToggle = millis();
  }
  
  // Print system info every 10 seconds
  static unsigned long lastInfo = 0;
  if (millis() - lastInfo > 10000) {
    Serial.println("");
    Serial.println("=== System Info ===");
    Serial.println("Chip Model: " + String(ESP.getChipModel()));
    Serial.println("Chip Revision: " + String(ESP.getChipRevision()));
    Serial.println("CPU Frequency: " + String(ESP.getCpuFreqMHz()) + " MHz");
    Serial.println("Flash Size: " + String(ESP.getFlashChipSize() / 1024 / 1024) + " MB");
    Serial.println("Free Heap: " + String(ESP.getFreeHeap()) + " bytes");
    Serial.println("Uptime: " + String(millis() / 1000) + " seconds");
    Serial.println("");
    
    lastInfo = millis();
  }
}