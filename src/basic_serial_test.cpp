/*
Basic Serial Test - Minimal ESP32-S3 Communication Test
Upload this first to verify basic serial communication
*/

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("ESP32-S3 Serial Test");
  Serial.println("If you see this, serial communication works!");
  Serial.print("Free heap: ");
  Serial.println(ESP.getFreeHeap());
  Serial.print("Chip model: ");
  Serial.println(ESP.getChipModel());
  Serial.print("CPU frequency: ");
  Serial.println(ESP.getCpuFreqMHz());
}

void loop() {
  static int counter = 0;
  counter++;
  
  Serial.print("Loop count: ");
  Serial.println(counter);
  
  delay(1000);
}