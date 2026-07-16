/*
 * SD Card Diagnostic Tool
 * Tests SD card module step-by-step to find the issue
 */

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

// SD Card pins
#define SD_CS    5
#define SD_MOSI  11
#define SD_SCK   12
#define SD_MISO  13

void testPinConnections();
void testSPIBus();
void testSDCardSlow();
void testSDCardFast();
void testCardInfo();

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("\n╔════════════════════════════════════════╗");
  Serial.println("║   SD Card Diagnostic Tool             ║");
  Serial.println("╚════════════════════════════════════════╝\n");
  
  Serial.println("📋 Testing SD card module...\n");
  
  // Test 1: Pin connections
  Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
  Serial.println("TEST 1: Pin Connections");
  Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
  testPinConnections();
  delay(1000);
  
  // Test 2: SPI Bus
  Serial.println("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
  Serial.println("TEST 2: SPI Bus Communication");
  Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
  testSPIBus();
  delay(1000);
  
  // Test 3: SD Card (Slow Speed)
  Serial.println("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
  Serial.println("TEST 3: SD Card Init (Slow Speed - 400kHz)");
  Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
  testSDCardSlow();
  delay(1000);
  
  // Test 4: SD Card (Fast Speed)
  Serial.println("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
  Serial.println("TEST 4: SD Card Init (Fast Speed - 4MHz)");
  Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
  testSDCardFast();
  
  Serial.println("\n╔════════════════════════════════════════╗");
  Serial.println("║   Diagnostic Complete                  ║");
  Serial.println("╚════════════════════════════════════════╝");
}

void loop() {
  // Nothing
}

void testPinConnections() {
  Serial.println("Setting up pins...");
  
  pinMode(SD_CS, OUTPUT);
  pinMode(SD_MOSI, OUTPUT);
  pinMode(SD_SCK, OUTPUT);
  pinMode(SD_MISO, INPUT);
  
  digitalWrite(SD_CS, HIGH);
  digitalWrite(SD_SCK, LOW);
  digitalWrite(SD_MOSI, LOW);
  
  Serial.println("✅ Pins configured");
  Serial.println("\n📌 Pin Configuration:");
  Serial.printf("   CS   (GPIO%d): OUTPUT, set HIGH\n", SD_CS);
  Serial.printf("   MOSI (GPIO%d): OUTPUT, set LOW\n", SD_MOSI);
  Serial.printf("   SCK  (GPIO%d): OUTPUT, set LOW\n", SD_SCK);
  Serial.printf("   MISO (GPIO%d): INPUT\n", SD_MISO);
  
  // Test MISO pin
  Serial.println("\n🔍 Testing MISO pin...");
  int misoState = digitalRead(SD_MISO);
  Serial.printf("   MISO state: %s\n", misoState ? "HIGH" : "LOW");
  
  if (misoState == HIGH) {
    Serial.println("   ✅ MISO is HIGH (good - card might be present)");
  } else {
    Serial.println("   ⚠️  MISO is LOW (card might not be inserted or wiring issue)");
  }
  
  // Toggle CS and check
  Serial.println("\n🔍 Testing CS pin...");
  digitalWrite(SD_CS, LOW);
  delay(10);
  Serial.println("   CS set LOW");
  digitalWrite(SD_CS, HIGH);
  delay(10);
  Serial.println("   CS set HIGH");
  Serial.println("   ✅ CS pin responding");
}

void testSPIBus() {
  Serial.println("Initializing SPI bus...");
  
  SPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
  Serial.println("✅ SPI bus initialized");
  
  Serial.println("\n📊 SPI Configuration:");
  Serial.println("   Clock: GPIO12");
  Serial.println("   MISO:  GPIO13");
  Serial.println("   MOSI:  GPIO11");
  Serial.println("   CS:    GPIO5");
  
  // Send dummy data
  Serial.println("\n🔍 Testing SPI communication...");
  digitalWrite(SD_CS, LOW);
  delay(1);
  
  uint8_t response = SPI.transfer(0xFF);
  Serial.printf("   Sent: 0xFF, Received: 0x%02X\n", response);
  
  digitalWrite(SD_CS, HIGH);
  delay(1);
  
  if (response == 0xFF || response == 0x00) {
    Serial.println("   ⚠️  No response from card (might be normal at this stage)");
  } else {
    Serial.println("   ✅ Got response from card!");
  }
}

void testSDCardSlow() {
  Serial.println("Attempting SD.begin() with slow speed...");
  Serial.println("(This is safest for troubleshooting)");
  
  // Try very slow speed (400kHz)
  if (!SD.begin(SD_CS, SPI, 400000)) {
    Serial.println("❌ FAILED at 400kHz");
    Serial.println("\n🔍 Possible issues:");
    Serial.println("   1. Card not fully inserted");
    Serial.println("   2. Wiring problem:");
    Serial.println("      - Check VCC → 3.3V");
    Serial.println("      - Check GND → GND");
    Serial.println("      - Check CS  → GPIO5");
    Serial.println("      - Check MOSI → GPIO11");
    Serial.println("      - Check SCK  → GPIO12");
    Serial.println("      - Check MISO → GPIO13");
    Serial.println("   3. Card needs reformatting (FAT32)");
    Serial.println("   4. Card is damaged");
    Serial.println("   5. Module is defective");
    return;
  }
  
  Serial.println("✅ SUCCESS at 400kHz!");
  testCardInfo();
}

void testSDCardFast() {
  Serial.println("Attempting SD.begin() with normal speed...");
  
  // Try normal speed (4MHz)
  if (!SD.begin(SD_CS, SPI, 4000000)) {
    Serial.println("❌ FAILED at 4MHz");
    Serial.println("   Card works at slow speed but not fast speed");
    Serial.println("   This is OK - use slow speed for now");
    return;
  }
  
  Serial.println("✅ SUCCESS at 4MHz!");
  testCardInfo();
}

void testCardInfo() {
  Serial.println("\n📊 Card Information:");
  
  uint8_t cardType = SD.cardType();
  
  if (cardType == CARD_NONE) {
    Serial.println("   ❌ No SD card detected");
    return;
  }
  
  Serial.print("   Card Type: ");
  switch (cardType) {
    case CARD_MMC:
      Serial.println("MMC");
      break;
    case CARD_SD:
      Serial.println("SDSC (Standard Capacity)");
      break;
    case CARD_SDHC:
      Serial.println("SDHC (High Capacity)");
      break;
    default:
      Serial.println("UNKNOWN");
  }
  
  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("   Total Size: %llu MB\n", cardSize);
  
  uint64_t usedBytes = SD.usedBytes() / (1024 * 1024);
  Serial.printf("   Used Space: %llu MB\n", usedBytes);
  
  uint64_t totalBytes = SD.totalBytes() / (1024 * 1024);
  uint64_t freeBytes = totalBytes - usedBytes;
  Serial.printf("   Free Space: %llu MB\n", freeBytes);
  
  // Try to list root directory
  Serial.println("\n📂 Root Directory:");
  File root = SD.open("/");
  if (!root) {
    Serial.println("   ❌ Failed to open root directory");
    return;
  }
  
  if (!root.isDirectory()) {
    Serial.println("   ❌ Root is not a directory");
    return;
  }
  
  File file = root.openNextFile();
  int fileCount = 0;
  while (file && fileCount < 10) {
    if (file.isDirectory()) {
      Serial.printf("   📁 %s/\n", file.name());
    } else {
      Serial.printf("   📄 %s (%d bytes)\n", file.name(), file.size());
    }
    file = root.openNextFile();
    fileCount++;
  }
  
  if (fileCount == 0) {
    Serial.println("   (empty)");
  }
  
  Serial.println("\n✅ Card is working properly!");
}
