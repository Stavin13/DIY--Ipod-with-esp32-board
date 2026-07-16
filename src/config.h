#ifndef CONFIG_H
#define CONFIG_H

// Hardware Configuration
#define BOARD_ESP32_S3

// =====================================================
//  ESP32-S3 iPod Configuration
//  Pure Touch Interface - No Buttons, No Encoder
//  Components: ESP32-S3 + MSP2806 Display + SD Module
// =====================================================

#ifndef CONFIG_H
#define CONFIG_H

// Hardware Configuration
#define BOARD_ESP32_S3

// Display Configuration - SPI1 Bus (ILI9341 controller)
#define TFT_WIDTH 240
#define TFT_HEIGHT 320
#define TFT_CS 10      // Chip Select
#define TFT_DC 9       // Data/Command
#define TFT_RST 8      // Reset
#define TFT_MOSI 11    // Master Out Slave In
#define TFT_SCLK 12    // Serial Clock
#define TFT_MISO 13    // Master In Slave Out (optional)

// Touch Configuration - SPI2 Bus (XPT2046 controller, same module as display)
#define TOUCH_CS 6     // Touch Chip Select
#define TOUCH_CLK 7    // Touch Clock
#define TOUCH_DIN 5    // Touch Data In
#define TOUCH_DO 4     // Touch Data Out
#define TOUCH_IRQ 3    // Touch Interrupt (optional)
#define TOUCH_ENABLED true

// SD Card Configuration - SPI3 Bus
#define SD_CS 35       // SD Chip Select
#define SD_MOSI 36     // SD Data Out
#define SD_MISO 37     // SD Data In
#define SD_SCLK 38     // SD Clock

// No Physical Input Devices - Pure Touch Interface
// GPIO0, GPIO1, GPIO2 available for future expansion

// USB-C Audio Configuration
#define USB_AUDIO_DP 18        // USB Data Plus
#define USB_AUDIO_DN 19        // USB Data Minus (built-in)
#define USB_VBUS_DETECT 20     // USB Power Detection
#define USB_AUDIO_ENABLE 17    // Audio Enable Control

// Power Management
#define CHARGE_STATUS 21       // Battery Charge Status
#define POWER_ENABLE 39        // Main Power Control
#define BATTERY_ADC 22         // Battery Voltage Monitor

// Bluetooth Configuration
#define BT_DEVICE_NAME "ESP32-iPod"

// Audio Settings
#define SAMPLE_RATE 44100
#define BITS_PER_SAMPLE 16
#define CHANNELS 2

// UI Configuration
#define STATUS_BAR_HEIGHT 20
#define NAV_BAR_HEIGHT 20
#define MAIN_CONTENT_HEIGHT (TFT_HEIGHT - STATUS_BAR_HEIGHT - NAV_BAR_HEIGHT)

// Colors (RGB565)
#define COLOR_BLACK 0x0000
#define COLOR_WHITE 0xFFFF
#define COLOR_BLUE 0x001F
#define COLOR_GRAY 0x8410
#define COLOR_LIGHT_GRAY 0xC618
#define COLOR_DARK_GRAY 0x4208

// File System
#define MAX_FILENAME_LENGTH 256
#define MAX_PATH_LENGTH 512
#define MUSIC_EXTENSIONS ".mp3,.wav,.flac,.m4a,.aac"

#endif // CONFIG_H