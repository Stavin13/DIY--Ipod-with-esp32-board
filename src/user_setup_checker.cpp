/*
User_Setup.h Configuration Checker
Verifies that User_Setup.h has the correct settings for touch
Upload this to check your TFT_eSPI configuration
*/

#include <TFT_eSPI.h>

void setup() {
  Serial.begin(115200);
  delay(3000);
  
  Serial.println("╔════════════════════════════════════════╗");
  Serial.println("║      User_Setup.h Configuration       ║");
  Serial.println("║            Checker v1.0               ║");
  Serial.println("╚════════════════════════════════════════╝");
  Serial.println();
  
  Serial.println("🔍 Checking User_Setup.h configuration...");
  Serial.println();
  
  // Check display driver
  Serial.println("📺 DISPLAY DRIVER:");
  #ifdef ILI9341_DRIVER
    Serial.println("   ✅ ILI9341_DRIVER is defined");
  #else
    Serial.println("   ❌ ILI9341_DRIVER is NOT defined");
    Serial.println("      Add: #define ILI9341_DRIVER");
  #endif
  
  // Check touch support
  Serial.println("\n👆 TOUCH SUPPORT:");
  #ifdef SUPPORT_TOUCH
    Serial.println("   ✅ SUPPORT_TOUCH is defined");
  #else
    Serial.println("   ❌ SUPPORT_TOUCH is NOT defined");
    Serial.println("      Add: #define SUPPORT_TOUCH");
    Serial.println("      THIS IS LIKELY WHY TOUCH ISN'T WORKING!");
  #endif
  
  // Check touch pins
  Serial.println("\n📌 TOUCH PINS:");
  #ifdef TOUCH_CS
    Serial.print("   ✅ TOUCH_CS is defined as GPIO");
    Serial.println(TOUCH_CS);
    if (TOUCH_CS != 6) {
      Serial.println("      ⚠️  Expected GPIO6 for your wiring");
    }
  #else
    Serial.println("   ❌ TOUCH_CS is NOT defined");
    Serial.println("      Add: #define TOUCH_CS 6");
  #endif
  
  // Check display pins
  Serial.println("\n🖥️  DISPLAY PINS:");
  #ifdef TFT_CS
    Serial.print("   TFT_CS: GPIO");
    Serial.println(TFT_CS);
  #endif
  #ifdef TFT_DC
    Serial.print("   TFT_DC: GPIO");
    Serial.println(TFT_DC);
  #endif
  #ifdef TFT_RST
    Serial.print("   TFT_RST: GPIO");
    Serial.println(TFT_RST);
  #endif
  #ifdef TFT_MOSI
    Serial.print("   TFT_MOSI: GPIO");
    Serial.println(TFT_MOSI);
  #endif
  #ifdef TFT_SCLK
    Serial.print("   TFT_SCLK: GPIO");
    Serial.println(TFT_SCLK);
  #endif
  #ifdef TFT_MISO
    Serial.print("   TFT_MISO: GPIO");
    Serial.println(TFT_MISO);
  #endif
  
  // Check SPI settings
  Serial.println("\n🔄 SPI SETTINGS:");
  #ifdef SPI_FREQUENCY
    Serial.print("   SPI_FREQUENCY: ");
    Serial.print(SPI_FREQUENCY / 1000000);
    Serial.println(" MHz");
  #endif
  #ifdef SPI_TOUCH_FREQUENCY
    Serial.print("   SPI_TOUCH_FREQUENCY: ");
    Serial.print(SPI_TOUCH_FREQUENCY / 1000000.0, 1);
    Serial.println(" MHz");
  #else
    Serial.println("   ⚠️  SPI_TOUCH_FREQUENCY not defined");
    Serial.println("      Add: #define SPI_TOUCH_FREQUENCY 2500000");
  #endif
  
  // Check color order
  Serial.println("\n🎨 COLOR SETTINGS:");
  #ifdef TFT_RGB_ORDER
    #if TFT_RGB_ORDER == TFT_BGR
      Serial.println("   ✅ TFT_RGB_ORDER set to TFT_BGR");
    #elif TFT_RGB_ORDER == TFT_RGB
      Serial.println("   ✅ TFT_RGB_ORDER set to TFT_RGB");
    #else
      Serial.println("   ⚠️  TFT_RGB_ORDER has unknown value");
    #endif
  #else
    Serial.println("   ⚠️  TFT_RGB_ORDER not defined");
  #endif
  
  // Summary and recommendations
  Serial.println("\n" + String("=").substring(0, 50));
  Serial.println("📋 SUMMARY:");
  
  bool hasIssues = false;
  
  #ifndef SUPPORT_TOUCH
    Serial.println("❌ CRITICAL: SUPPORT_TOUCH not defined");
    Serial.println("   This is why touch isn't working!");
    hasIssues = true;
  #endif
  
  #ifndef TOUCH_CS
    Serial.println("❌ CRITICAL: TOUCH_CS not defined");
    hasIssues = true;
  #endif
  
  #ifndef ILI9341_DRIVER
    Serial.println("❌ CRITICAL: ILI9341_DRIVER not defined");
    hasIssues = true;
  #endif
  
  if (!hasIssues) {
    Serial.println("✅ Configuration looks good!");
    Serial.println("   If touch still doesn't work, check hardware wiring.");
  } else {
    Serial.println("\n🛠️  TO FIX:");
    Serial.println("1. Copy User_Setup_Complete.h to TFT_eSPI/User_Setup.h");
    Serial.println("2. Or manually add missing #define statements");
    Serial.println("3. Restart Arduino IDE after changing User_Setup.h");
  }
  
  Serial.println("\n📍 Expected User_Setup.h location:");
  Serial.println("   Arduino/libraries/TFT_eSPI/User_Setup.h");
  Serial.println("   (or in your Arduino libraries folder)");
  
  Serial.println("\n🔄 After fixing User_Setup.h:");
  Serial.println("1. Restart Arduino IDE");
  Serial.println("2. Upload touch_hardware_diagnostic.cpp");
  Serial.println("3. Check if touch now works");
}

void loop() {
  // Nothing to do in loop
  delay(1000);
}