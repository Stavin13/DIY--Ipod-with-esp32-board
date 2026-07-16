// =====================================================
//  TFT_eSPI User Setup for ESP32-S3 iPod Project
//  Display: MSP2806 (2.8" ILI9341 with XPT2046 Touch)
//  Board: ESP32-S3-N16R8 DevKit
// =====================================================

#define USER_SETUP_INFO "ESP32-S3 iPod MSP2806"

// =====================================================
// Section 1: Display Driver Selection
// =====================================================
#define ILI9341_DRIVER      // 2.8" display driver

// =====================================================
// Section 2: Display Color Order
// =====================================================
#define TFT_RGB_ORDER TFT_BGR   // Try TFT_RGB if colors are wrong

// =====================================================
// Section 3: ESP32-S3 Pin Configuration
// Matches your exact wiring from config.h
// =====================================================

// Display SPI pins
#define TFT_MISO  13    // Master In Slave Out (optional)
#define TFT_MOSI  11    // Master Out Slave In (SDA)
#define TFT_SCLK  12    // Serial Clock (SCL)
#define TFT_CS    10    // Chip Select
#define TFT_DC    9     // Data/Command (RS)
#define TFT_RST   8     // Reset

// Backlight control (if you want software control)
// #define TFT_BL    -1    // Not used - hardwired to 3.3V
// #define TFT_BACKLIGHT_ON HIGH

// =====================================================
// Section 4: Touch Controller Configuration
// Enhanced touch calibration settings
// =====================================================
#define TOUCH_CS  6     // Touch Chip Select

// Enable TFT_eSPI touch support with enhanced settings
#define SUPPORT_TOUCH
#define CALIBRATION_FILE "/TouchCalData"  // Store calibration in SPIFFS
#define REPEAT_CAL false  // Set to true to force recalibration

// Touch calibration improvements
#define TOUCH_PRECISION 10    // Higher = more precise (1-20)
#define TOUCH_THRESHOLD 600   // Touch pressure threshold
#define TOUCH_Z_THRESHOLD 400 // Z-axis pressure threshold

// SPI settings for touch (separate from display)
#define SPI_TOUCH_FREQUENCY   2500000   // 2.5MHz for stable touch

// =====================================================
// Section 5: Display Resolution
// =====================================================
#define TFT_WIDTH  240
#define TFT_HEIGHT 320

// =====================================================
// Section 6: Font Loading
// =====================================================
#define LOAD_GLCD   // Font 1. Original Adafruit 8 pixel font needs ~1820 bytes in FLASH
#define LOAD_FONT2  // Font 2. Small 16 pixel high font, needs ~3534 bytes in FLASH, 96 characters
#define LOAD_FONT4  // Font 4. Medium 26 pixel high font, needs ~5848 bytes in FLASH, 96 characters
#define LOAD_FONT6  // Font 6. Large 48 pixel high font, needs ~2666 bytes in FLASH, only characters 1234567890:-.apm
#define LOAD_FONT7  // Font 7. 7 segment 48 pixel high font, needs ~2438 bytes in FLASH, only characters 1234567890:-.
#define LOAD_FONT8  // Font 8. Large 75 pixel high font needs ~3256 bytes in FLASH, only characters 1234567890:-.
#define LOAD_GFXFF  // FreeFonts. Include access to the 48 Adafruit_GFX free fonts FF1 to FF48 and custom fonts

// Comment out the #define below to stop the SPIFFS filing system and smooth font code being loaded
// this will save ~20kbytes of FLASH
#define SMOOTH_FONT

// =====================================================
// Section 7: SPI Frequency Settings
// Start with conservative speeds for breadboard
// =====================================================
#define SPI_FREQUENCY        20000000   // 20MHz - Safe for breadboard
#define SPI_READ_FREQUENCY   10000000   // 10MHz - For reading operations
#define SPI_TOUCH_FREQUENCY   2500000   // 2.5MHz - For touch controller

// =====================================================
// Section 8: ESP32-S3 Specific Settings
// =====================================================
// Use VSPI port (SPI3) for display
#define USE_HSPI_PORT

// =====================================================
// Section 9: Optional Features
// =====================================================
// Uncomment to enable DMA (faster but uses more memory)
// #define SPI_18BIT_DRIVER

// Uncomment for better performance with large graphics
// #define DMA_CHANNEL 1

// =====================================================
// END OF USER SETUP
// =====================================================