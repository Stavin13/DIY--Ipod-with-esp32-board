/*
 * ILI9341 Built-in SD Card Test
 * Tests SD card slot on ILI9341 display module
 * 
 * If your ILI9341 has an SD card slot, connect:
 * SD_CS → GPIO4 (or check your module's label)
 * 
 * All other pins (MOSI, SCK, MISO) are shared with display
 */

#include <Arduino.h>
#include <SD.h>
#include <SPI.h>

// Display pins (already connected)
#define TFT_CS   10
#define TFT_MOSI 11
#define TFT_SCK  12
#define TFT_MISO 13

// SD Card CS pin (on display module)
#define SD_CS 4  // Common for ILI9341 modules, might be 5 on some

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("\n╔════════════════════════════════════════╗");
  Serial.println("║   ILI9341 Built-in SD Card Test       ║");
  Serial.println("╚════════════════════════════════════════╝\n");
  
  Serial.println("📌 Testing SD card slot on display module");
  Serial.println("   SD_CS: GPIO4 (trying first)");
  Serial.println("   Shared SPI with display\n");
  
  // Make sure display CS is HIGH (deselected)
  pinMode(TFT_CS, OUTPUT);
  digitalWrite(TFT_CS, HIGH);
  delay(10);
  
  // Test with GPIO4 first (most common)
  Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
  Serial.println("TEST 1: SD Card on GPIO4");
  Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
  
  if (testSDCard(4)) {
    Serial.println("\n✅ SUCCESS! SD card works on GPIO4");
    return;
  }
  
  // Try GPIO5 if GPIO4 failed
  Serial.println("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
  Serial.println("TEST 2: SD Card on GPIO5");
  Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
  
  if (testSDCard(5)) {
    Serial.println("\n✅ SUCCESS! SD card works on GPIO5");
    return;
  }
  
  // Try GPIO14 (some modules use this)
  Serial.println("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
  Serial.println("TEST 3: SD Card on GPIO14");
  Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
  
  if (testSDCard(14)) {
    Serial.println("\n✅ SUCCESS! SD card works on GPIO14");
    return;
  }
  
  // All failed
  Serial.println("\n❌ SD card not detected on GPIO4, GPIO5, or GPIO14");
  Serial.println("\n� Possible reasons:");
  Serial.println("   1. Your ILI9341 module doesn't have a built-in SD slot");
  Serial.println("   2. SD card not inserted in the display module slot");
  Serial.println("   3. SD_CS pin is different (check module label)");
  Serial.println("   4. SD_CS wire not connected to ESP32");
  Serial.println("   5. SD card not formatted as FAT32");
  Serial.println("\n💡 Check the back of your display for:");
  Serial.println("   - MicroSD card slot (small slot for microSD)");
  Serial.println("   - Pin labeled SD_CS, SDCS, CS_SD, or SD-CS");
  Serial.println("\n📸 Take a photo of the back of your display");
  Serial.println("   and check the pin labels!");
  Serial.println("\n✅ ALTERNATIVE: Use ipod_no_sd.cpp for demo mode");
  Serial.println("   (Works without SD card, has 9 demo songs)");
}

void loop() {
  // Nothing
}

bool testSDCard(int csPin) {
  Serial.printf("Trying CS pin: GPIO%d\n", csPin);
  Serial.print("Initializing... ");
  
  // Initialize with slow speed for reliability
  if (!SD.begin(csPin, SPI, 400000)) {
    Serial.println("❌ Failed");
    return false;
  }
  
  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("❌ No card");
    SD.end();
    return false;
  }
  
  Serial.println("✅ Detected!");
  
  // Show card info
  Serial.print("📇 Type: ");
  if (cardType == CARD_MMC) Serial.println("MMC");
  else if (cardType == CARD_SD) Serial.println("SDSC");
  else if (cardType == CARD_SDHC) Serial.println("SDHC");
  
  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("💾 Size: %llu MB\n", cardSize);
  
  uint64_t usedBytes = SD.usedBytes() / (1024 * 1024);
  uint64_t totalBytes = SD.totalBytes() / (1024 * 1024);
  Serial.printf("📊 Used: %llu MB / %llu MB\n", usedBytes, totalBytes);
  
  // Test read/write
  Serial.print("📝 Testing write... ");
  File file = SD.open("/test.txt", FILE_WRITE);
  if (!file) {
    Serial.println("❌ Failed");
    return false;
  }
  file.println("ILI9341 SD Test");
  file.close();
  Serial.println("✅");
  
  Serial.print("📖 Testing read... ");
  file = SD.open("/test.txt");
  if (!file) {
    Serial.println("❌ Failed");
    return false;
  }
  while (file.available()) {
    Serial.write(file.read());
  }
  file.close();
  Serial.println("✅");
  
  SD.remove("/test.txt");
  
  return true;
}
