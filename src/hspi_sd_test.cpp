/*
 * HSPI SD Card Test
 * Tests SD card on HSPI bus (GPIO 15, 35, 36, 37)
 * 
 * Wiring:
 * SD Card → ESP32-S3
 * CS   → GPIO15
 * MOSI → GPIO35
 * SCK  → GPIO36
 * MISO → GPIO37
 * VCC  → 3.3V
 * GND  → GND
 */

#include <Arduino.h>
#include <SD.h>
#include <SPI.h>

// HSPI pins for ESP32-S3
#define SD_CS   15
#define SD_MOSI 35
#define SD_SCK  36
#define SD_MISO 37

// Create HSPI instance
SPIClass * hspi = NULL;

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("\n╔════════════════════════════════════════╗");
  Serial.println("║   HSPI SD Card Test                    ║");
  Serial.println("╚════════════════════════════════════════╝\n");
  
  Serial.println("📌 Expected Wiring:");
  Serial.println("   CS   → GPIO15");
  Serial.println("   MOSI → GPIO35");
  Serial.println("   SCK  → GPIO36");
  Serial.println("   MISO → GPIO37");
  Serial.println("   VCC  → 3.3V");
  Serial.println("   GND  → GND\n");
  
  // Test 1: Initialize HSPI
  Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
  Serial.println("TEST 1: Initialize HSPI Bus");
  Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
  
  hspi = new SPIClass(HSPI);
  hspi->begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
  Serial.println("✅ HSPI initialized");
  
  // Test 2: Check pins
  Serial.println("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
  Serial.println("TEST 2: Pin Configuration");
  Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
  
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);
  Serial.println("✅ CS pin configured (GPIO15)");
  
  pinMode(SD_MISO, INPUT);
  int misoState = digitalRead(SD_MISO);
  Serial.printf("📍 MISO state: %s (GPIO37)\n", misoState ? "HIGH" : "LOW");
  
  // Test 3: SD Card Init (Very Slow)
  Serial.println("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
  Serial.println("TEST 3: SD Card Init (100kHz - Very Slow)");
  Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
  
  if (!SD.begin(SD_CS, *hspi, 100000)) {
    Serial.println("❌ FAILED at 100kHz");
    Serial.println("\n🔍 Troubleshooting:");
    Serial.println("   1. Double-check wiring:");
    Serial.println("      - CS   on GPIO15?");
    Serial.println("      - MOSI on GPIO35?");
    Serial.println("      - SCK  on GPIO36?");
    Serial.println("      - MISO on GPIO37?");
    Serial.println("   2. Card fully inserted?");
    Serial.println("   3. Card formatted as FAT32?");
    Serial.println("   4. Try different SD card?");
    Serial.println("   5. Check 3.3V power to card");
    return;
  }
  
  Serial.println("✅ SD Card detected!");
  
  // Test 4: Card Info
  Serial.println("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
  Serial.println("TEST 4: Card Information");
  Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
  
  uint8_t cardType = SD.cardType();
  
  if (cardType == CARD_NONE) {
    Serial.println("❌ No SD card attached");
    return;
  }
  
  Serial.print("📇 Card Type: ");
  switch (cardType) {
    case CARD_MMC:
      Serial.println("MMC");
      break;
    case CARD_SD:
      Serial.println("SDSC");
      break;
    case CARD_SDHC:
      Serial.println("SDHC");
      break;
    default:
      Serial.println("UNKNOWN");
  }
  
  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("💾 Total Size: %llu MB\n", cardSize);
  
  uint64_t usedBytes = SD.usedBytes() / (1024 * 1024);
  Serial.printf("📊 Used Space: %llu MB\n", usedBytes);
  
  uint64_t totalBytes = SD.totalBytes() / (1024 * 1024);
  uint64_t freeBytes = totalBytes - usedBytes;
  Serial.printf("📂 Free Space: %llu MB\n", freeBytes);
  
  // Test 5: List Files
  Serial.println("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
  Serial.println("TEST 5: List Root Directory");
  Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
  
  File root = SD.open("/");
  if (!root) {
    Serial.println("❌ Failed to open root directory");
    return;
  }
  
  if (!root.isDirectory()) {
    Serial.println("❌ Root is not a directory");
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
  
  // Test 6: Read/Write
  Serial.println("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
  Serial.println("TEST 6: Read/Write Test");
  Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
  
  Serial.print("📝 Writing test file... ");
  File testFile = SD.open("/hspi_test.txt", FILE_WRITE);
  if (!testFile) {
    Serial.println("❌ FAILED");
    return;
  }
  
  testFile.println("HSPI SD Card Test");
  testFile.printf("Timestamp: %lu ms\n", millis());
  testFile.close();
  Serial.println("✅ SUCCESS");
  
  Serial.print("📖 Reading test file... ");
  testFile = SD.open("/hspi_test.txt");
  if (!testFile) {
    Serial.println("❌ FAILED");
    return;
  }
  
  Serial.println("✅ SUCCESS");
  Serial.println("\n📄 File Contents:");
  while (testFile.available()) {
    Serial.write(testFile.read());
  }
  testFile.close();
  
  Serial.print("\n🗑️  Deleting test file... ");
  if (SD.remove("/hspi_test.txt")) {
    Serial.println("✅ SUCCESS");
  } else {
    Serial.println("❌ FAILED");
  }
  
  // Success!
  Serial.println("\n╔════════════════════════════════════════╗");
  Serial.println("║   ✅ ALL TESTS PASSED!                 ║");
  Serial.println("║   HSPI SD Card Working Perfectly!      ║");
  Serial.println("╚════════════════════════════════════════╝");
}

void loop() {
  // Nothing
}
