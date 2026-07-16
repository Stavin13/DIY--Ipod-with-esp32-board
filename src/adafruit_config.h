// =====================================================
//  Adafruit Library Configuration for ESP32-S3 iPod
//  Display: MSP2806 (2.8" ILI9341 with XPT2046 Touch)
//  Board: ESP32-S3-N16R8 DevKit
// =====================================================

#ifndef ADAFRUIT_CONFIG_H
#define ADAFRUIT_CONFIG_H

// =====================================================
// Display Pin Configuration (ILI9341)
// =====================================================
#define TFT_CS    10    // Chip Select
#define TFT_DC    9     // Data/Command
#define TFT_RST   8     // Reset
#define TFT_MOSI  11    // Master Out Slave In
#define TFT_SCLK  12    // Serial Clock
#define TFT_MISO  13    // Master In Slave Out (optional)

// =====================================================
// Touch Pin Configuration (XPT2046)
// =====================================================
#define TOUCH_CS   6    // Touch Chip Select
#define TOUCH_IRQ  3    // Touch Interrupt (optional)

// =====================================================
// Display Settings
// =====================================================
#define DISPLAY_WIDTH  240
#define DISPLAY_HEIGHT 320
#define DISPLAY_ROTATION 0  // 0=Portrait, 1=Landscape, 2=Portrait flipped, 3=Landscape flipped

// =====================================================
// SPI Settings
// =====================================================
#define SPI_FREQUENCY     20000000  // 20MHz - Safe for breadboard
#define TOUCH_FREQUENCY    2500000  // 2.5MHz - Touch controller

// =====================================================
// Color Definitions (RGB565)
// =====================================================
#define COLOR_BLACK       0x0000
#define COLOR_WHITE       0xFFFF
#define COLOR_RED         0xF800
#define COLOR_GREEN       0x07E0
#define COLOR_BLUE        0x001F
#define COLOR_YELLOW      0xFFE0
#define COLOR_MAGENTA     0xF81F
#define COLOR_CYAN        0x07FF
#define COLOR_GRAY        0x8410
#define COLOR_LIGHT_GRAY  0xC618
#define COLOR_DARK_GRAY   0x4208

// iPod-style colors
#define COLOR_IPOD_BG     0x0000    // Black background
#define COLOR_IPOD_TEXT   0xFFFF    // White text
#define COLOR_IPOD_ACCENT 0x001F    // Blue accent
#define COLOR_IPOD_GRAY   0x8410    // Gray for inactive items

// =====================================================
// Touch Calibration (adjust if touch is inaccurate)
// =====================================================
#define TOUCH_MIN_X  200
#define TOUCH_MAX_X  3800
#define TOUCH_MIN_Y  200
#define TOUCH_MAX_Y  3800

// =====================================================
// Performance Settings
// =====================================================
#define USE_DMA          false  // Set to true for better performance (uses more memory)
#define BUFFER_SIZE      240    // Line buffer size for drawing

#endif // ADAFRUIT_CONFIG_H