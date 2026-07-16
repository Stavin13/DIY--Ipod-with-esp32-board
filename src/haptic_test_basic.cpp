/*
ESP32-S3 DRV2605L Haptic Motor Driver - Basic Test
Test I2C communication and basic haptic effects
*/

#include <Wire.h>

// DRV2605L I2C Address and Registers
#define DRV2605L_ADDR 0x5A
#define DRV2605L_STATUS 0x00
#define DRV2605L_MODE 0x01
#define DRV2605L_RTPIN 0x02
#define DRV2605L_LIBRARY 0x03
#define DRV2605L_WAVESEQ1 0x04
#define DRV2605L_GO 0x0C
#define DRV2605L_FEEDBACK 0x1A
#define DRV2605L_CONTROL1 0x1B
#define DRV2605L_CONTROL2 0x1C
#define DRV2605L_CONTROL3 0x1D
#define DRV2605L_RATEDV 0x16
#define DRV2605L_CLAMPV 0x17

// I2C Pins for ESP32-S3
#define I2C_SDA 21
#define I2C_SCL 22

// Test button (optional)
#define TEST_BUTTON 14

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("╔════════════════════════════════════════╗");
  Serial.println("║      DRV2605L Haptic Driver Test      ║");
  Serial.println("║        ESP32-S3 + LRA Testing         ║");
  Serial.println("╚════════════════════════════════════════╝");
  Serial.println();
  
  // Initialize I2C
  Wire.begin(I2C_SDA, I2C_SCL);
  Wire.setClock(400000); // 400kHz
  
  Serial.print("🔌 I2C initialized - SDA: GPIO");
  Serial.print(I2C_SDA);
  Serial.print(", SCL: GPIO");
  Serial.println(I2C_SCL);
  
  // Setup test button
  pinMode(TEST_BUTTON, INPUT_PULLUP);
  Serial.print("🔘 Test button: GPIO");
  Serial.println(TEST_BUTTON);
  Serial.println();
  
  // Test DRV2605L connection
  testI2CConnection();
  
  // Initialize DRV2605L
  initializeDRV2605L();
  
  // Run basic tests
  runBasicTests();
  
  Serial.println("🎯 Test complete! Press button for manual haptic test.");
  Serial.println("   Or send commands via Serial Monitor:");
  Serial.println("   '1' = Sharp Click");
  Serial.println("   '2' = Double Click");
  Serial.println("   '3' = Triple Click");
  Serial.println("   '4' = Soft Bump");
  Serial.println("   '5' = Strong Buzz");
  Serial.println();
}

void loop() {
  // Check for button press
  static bool lastButtonState = HIGH;
  bool buttonState = digitalRead(TEST_BUTTON);
  
  if (buttonState == LOW && lastButtonState == HIGH) {
    delay(50); // Debounce
    if (digitalRead(TEST_BUTTON) == LOW) {
      Serial.println("🔘 Button pressed - Playing haptic effect!");
      playHapticEffect(1); // Sharp click
    }
  }
  lastButtonState = buttonState;
  
  // Check for serial commands
  if (Serial.available()) {
    char command = Serial.read();
    handleSerialCommand(command);
  }
  
  delay(10);
}

void testI2CConnection() {
  Serial.println("🔍 Testing I2C connection to DRV2605L...");
  
  Wire.beginTransmission(DRV2605L_ADDR);
  uint8_t error = Wire.endTransmission();
  
  if (error == 0) {
    Serial.println("✅ DRV2605L found at address 0x5A");
    
    // Read device status
    uint8_t status = readRegister(DRV2605L_STATUS);
    Serial.print("📊 Device status: 0x");
    Serial.println(status, HEX);
    
    if (status & 0x80) {
      Serial.println("⚠️ Device in standby mode");
    } else {
      Serial.println("✅ Device active");
    }
    
  } else {
    Serial.print("❌ DRV2605L not found! I2C error: ");
    Serial.println(error);
    Serial.println("   Check connections:");
    Serial.println("   - VCC to 3.3V");
    Serial.println("   - GND to GND");
    Serial.print("   - SDA to GPIO");
    Serial.println(I2C_SDA);
    Serial.print("   - SCL to GPIO");
    Serial.println(I2C_SCL);
    while(1); // Stop here if no device found
  }
  Serial.println();
}

void initializeDRV2605L() {
  Serial.println("🔧 Initializing DRV2605L...");
  
  // Take out of standby
  writeRegister(DRV2605L_MODE, 0x00);
  Serial.println("   ✓ Device out of standby");
  
  // Set to internal trigger mode
  writeRegister(DRV2605L_RTPIN, 0x00);
  Serial.println("   ✓ Internal trigger mode set");
  
  // Set library to ERM (Eccentric Rotating Mass)
  writeRegister(DRV2605L_LIBRARY, 1);
  Serial.println("   ✓ ERM library selected");
  
  // Configure for LRA (Linear Resonant Actuator)
  writeRegister(DRV2605L_FEEDBACK, 0x80); // LRA mode
  Serial.println("   ✓ LRA feedback mode enabled");
  
  // Set drive parameters
  writeRegister(DRV2605L_CONTROL1, 0x93); // Drive time
  writeRegister(DRV2605L_CONTROL2, 0xF5); // Sample time, blanking time
  writeRegister(DRV2605L_CONTROL3, 0xA0); // NG threshold
  Serial.println("   ✓ Drive parameters configured");
  
  // Set voltage levels (adjust for your LRA)
  writeRegister(DRV2605L_RATEDV, 0x3E);   // Rated voltage
  writeRegister(DRV2605L_CLAMPV, 0x8C);   // Overdrive clamp voltage
  Serial.println("   ✓ Voltage levels set");
  
  Serial.println("✅ DRV2605L initialization complete!");
  Serial.println();
}

void runBasicTests() {
  Serial.println("🧪 Running basic haptic tests...");
  
  // Test 1: Sharp Click
  Serial.println("   Test 1: Sharp Click (Effect #1)");
  playHapticEffect(1);
  delay(1000);
  
  // Test 2: Double Click
  Serial.println("   Test 2: Double Click (Effect #2)");
  playHapticEffect(2);
  delay(1000);
  
  // Test 3: Soft Bump
  Serial.println("   Test 3: Soft Bump (Effect #10)");
  playHapticEffect(10);
  delay(1000);
  
  // Test 4: Strong Buzz
  Serial.println("   Test 4: Strong Buzz (Effect #16)");
  playHapticEffect(16);
  delay(1000);
  
  // Test 5: Custom sequence
  Serial.println("   Test 5: Custom Sequence (1-2-3)");
  playCustomSequence(1, 2, 3);
  delay(2000);
  
  Serial.println("✅ Basic tests complete!");
  Serial.println();
}

void playHapticEffect(uint8_t effect) {
  // Clear sequence
  writeRegister(DRV2605L_WAVESEQ1, 0x00);
  
  // Set effect
  writeRegister(DRV2605L_WAVESEQ1, effect);
  
  // Trigger
  writeRegister(DRV2605L_GO, 0x01);
  
  Serial.print("🔄 Playing effect #");
  Serial.println(effect);
}

void playCustomSequence(uint8_t effect1, uint8_t effect2, uint8_t effect3) {
  // Set sequence
  writeRegister(0x04, effect1); // WAVESEQ1
  writeRegister(0x05, effect2); // WAVESEQ2
  writeRegister(0x06, effect3); // WAVESEQ3
  writeRegister(0x07, 0x00);    // WAVESEQ4 (end)
  
  // Trigger
  writeRegister(DRV2605L_GO, 0x01);
  
  Serial.print("🔄 Playing sequence: ");
  Serial.print(effect1);
  Serial.print("-");
  Serial.print(effect2);
  Serial.print("-");
  Serial.println(effect3);
}

void handleSerialCommand(char command) {
  switch (command) {
    case '1':
      Serial.println("Command: Sharp Click");
      playHapticEffect(1);
      break;
    case '2':
      Serial.println("Command: Double Click");
      playHapticEffect(2);
      break;
    case '3':
      Serial.println("Command: Triple Click");
      playHapticEffect(3);
      break;
    case '4':
      Serial.println("Command: Soft Bump");
      playHapticEffect(10);
      break;
    case '5':
      Serial.println("Command: Strong Buzz");
      playHapticEffect(16);
      break;
    case 's':
      Serial.println("Command: Status Check");
      checkStatus();
      break;
    case 'h':
      Serial.println("Command: Help");
      printHelp();
      break;
    default:
      Serial.print("Unknown command: ");
      Serial.println(command);
      break;
  }
}

void checkStatus() {
  Serial.println("📊 DRV2605L Status Check:");
  
  uint8_t status = readRegister(DRV2605L_STATUS);
  Serial.print("   Status Register: 0x");
  Serial.println(status, HEX);
  
  if (status & 0x80) Serial.println("   - Device in standby");
  else Serial.println("   - Device active");
  
  if (status & 0x40) Serial.println("   - Diagnostic result available");
  if (status & 0x08) Serial.println("   - Auto-calibration done");
  if (status & 0x01) Serial.println("   - Over-temperature detected");
  
  uint8_t mode = readRegister(DRV2605L_MODE);
  Serial.print("   Mode Register: 0x");
  Serial.println(mode, HEX);
  
  uint8_t feedback = readRegister(DRV2605L_FEEDBACK);
  Serial.print("   Feedback Register: 0x");
  Serial.println(feedback, HEX);
  
  if (feedback & 0x80) Serial.println("   - LRA mode enabled");
  else Serial.println("   - ERM mode enabled");
}

void printHelp() {
  Serial.println("📖 Available Commands:");
  Serial.println("   '1' - Sharp Click (Effect #1)");
  Serial.println("   '2' - Double Click (Effect #2)");
  Serial.println("   '3' - Triple Click (Effect #3)");
  Serial.println("   '4' - Soft Bump (Effect #10)");
  Serial.println("   '5' - Strong Buzz (Effect #16)");
  Serial.println("   's' - Status Check");
  Serial.println("   'h' - This help message");
  Serial.println();
  Serial.println("🔘 Or press the test button for quick haptic test!");
}

void writeRegister(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(DRV2605L_ADDR);
  Wire.write(reg);
  Wire.write(value);
  uint8_t error = Wire.endTransmission();
  
  if (error != 0) {
    Serial.print("❌ I2C write error: ");
    Serial.println(error);
  }
}

uint8_t readRegister(uint8_t reg) {
  Wire.beginTransmission(DRV2605L_ADDR);
  Wire.write(reg);
  uint8_t error = Wire.endTransmission();
  
  if (error != 0) {
    Serial.print("❌ I2C read error: ");
    Serial.println(error);
    return 0;
  }
  
  Wire.requestFrom(DRV2605L_ADDR, 1);
  if (Wire.available()) {
    return Wire.read();
  } else {
    Serial.println("❌ No data received");
    return 0;
  }
}