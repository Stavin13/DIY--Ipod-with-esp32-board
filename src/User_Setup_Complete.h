// =====================================================
//  TFT_eSPI User Setup for ESP32-S3 iPod Project
//  Display: MSP2806 (2.8" ILI9341 with XPT2046 Touch)
//  Board: ESP32-S3-N16R8 DevKit
//  Complete Touch Sensor Configuration
// =====================================================

#define USER_SETUP_INFO "ESP32-S3 iPod MSP2806 with Touch"

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

// Backlight control (hardwired to 3.3V)
// #define TFT_BL    -1    // Not used - hardwired to 3.3V
// #define TFT_BACKLIGHT_ON HIGH

// =====================================================
// Section 4: Touch Controller Configuration (XPT2046)
// Complete touch sensor setup for reliable operation
// =====================================================

// Touch controller pins
#define TOUCH_CS  6     // Touch Chip Select (T_CS)
// Note: Touch uses shared MOSI(11), MISO(13), SCLK(12) with display
// Additional touch pins: T_CLK(7), T_DIN(5), T_DO(4), T_IRQ(3)

// Enable TFT_eSPI built-in touch support
#define SUPPORT_TOUCH

// Touch calibration settings
#define CALIBRATION_FILE "/TouchCalData"  // Store calibration in SPIFFS
#define REPEAT_CAL false  // Set to true to force recalibration every boot

// Touch sensitivity and precision
#define TOUCH_PRECISION 12    // Higher = more precise (1-20), 12 is good balance
#define TOUCH_THRESHOLD 600   // Touch pressure threshold (lower = more sensitive)
#define TOUCH_Z_THRESHOLD 400 // Z-axis pressure threshold

// XPT2046 specific configuration
#define XPT2046_SPI 1         // Use SPI port 1 for touch
#define XPT2046_IRQ 3         // Touch interrupt pin (GPIO3, optional but recommended)

// Touch coordinate mapping (fine-tune these if touch is offset)
#define TOUCH_MAP_X1 300      // Touch X minimum raw value
#define TOUCH_MAP_X2 3700     // Touch X maximum raw value
#define TOUCH_MAP_Y1 300      // Touch Y minimum raw value  
#define TOUCH_MAP_Y2 3700     // Touch Y maximum raw value

// Touch orientation and mirroring
#define TOUCH_ORIENTATION 1   // 0-7, adjust if touch doesn't match display
// 0 = normal, 1 = rotate 90°, 2 = rotate 180°, 3 = rotate 270°
// 4 = mirror X, 5 = mirror Y, 6 = mirror X + rotate 90°, 7 = mirror Y + rotate 90°

// Touch timing and debouncing
#define TOUCH_DEBOUNCE 15     // Debounce time in milliseconds
#define TOUCH_REPEAT_DELAY 150 // Delay before touch repeat (ms)

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
// Optimized for breadboard stability and touch reliability
// =====================================================
#define SPI_FREQUENCY        20000000   // 20MHz - Safe for breadboard display
#define SPI_READ_FREQUENCY   10000000   // 10MHz - For reading operations
#define SPI_TOUCH_FREQUENCY   2500000   // 2.5MHz - For XPT2046 touch controller (stable)

// =====================================================
// Section 8: ESP32-S3 Specific Settings
// =====================================================
// Use HSPI port (SPI2) for display - ESP32-S3 compatible
#define USE_HSPI_PORT

// =====================================================
// Section 9: Advanced Touch Features
// =====================================================

// Touch filtering and noise reduction
#define TOUCH_FILTER_ENABLED    // Enable coordinate filtering
#define TOUCH_AVERAGE_SAMPLES 3 // Number of samples to average (1-5)
#define TOUCH_MAX_JITTER 15     // Maximum allowed coordinate jitter (pixels)

// Touch gesture support
#define TOUCH_GESTURE_ENABLED   // Enable basic gesture detection
#define TOUCH_SWIPE_THRESHOLD 30 // Minimum distance for swipe detection
#define TOUCH_LONG_PRESS_TIME 800 // Long press time in milliseconds

// Touch validation
#define TOUCH_MIN_PRESSURE 200  // Minimum pressure to register touch
#define TOUCH_MAX_PRESSURE 4000 // Maximum pressure (filter out noise)

// =====================================================
// Section 10: Performance and Memory Options
// =====================================================

// Uncomment to enable DMA (faster but uses more memory)
// #define SPI_18BIT_DRIVER

// Uncomment for better performance with large graphics
// #define DMA_CHANNEL 1

// Enable SPIFFS for touch calibration storage
#define SPIFFS_SUPPORT

// Memory optimization for ESP32-S3
#define PSRAM_ENABLE           // Use PSRAM if available
#define BUFFER_SIZE 240        // Line buffer size for drawing

// =====================================================
// Section 11: Debug and Diagnostics
// =====================================================

// Uncomment for touch debugging (shows raw coordinates in serial)
// #define TOUCH_DEBUG

// Uncomment for SPI debugging
// #define SPI_DEBUG

// Touch calibration points (5-point calibration)
#define CALIBRATION_POINTS 5   // Standard 5-point calibration

// =====================================================
// Section 12: Compatibility Settings
// =====================================================

// Ensure compatibility with TFT_eSPI touch functions
#define TFT_TOUCH_DEVICE_XPT2046

// Color depth (16-bit for best performance)
#define COLOR_DEPTH 16

// Screen orientation support
#define SUPPORT_ROTATION

// =====================================================
// END OF USER SETUP
// =====================================================

/*
WIRING SUMMARY for ESP32-S3 + MSP2806:

Display (ILI9341):
ESP32-S3    MSP2806
GPIO10  →   CS
GPIO9   →   DC
GPIO8   →   RST
GPIO11  →   SDA/MOSI
GPIO12  →   SCL/SCLK
GPIO13  →   SDO/MISO (optional)
3.3V    →   VCC
3.3V    →   LED
GND     →   GND

Touch (XPT2046):
ESP32-S3    MSP2806
GPIO6   →   T_CS
GPIO7   →   T_CLK
GPIO5   →   T_DIN
GPIO4   →   T_DO
GPIO3   →   T_IRQ (optional)
3.3V    →   VCC (shared)
GND     →   GND (shared)

Note: Touch controller shares MOSI, MISO, SCLK with display
but has separate chip select (T_CS) and additional pins.
*/