# ESP32-S3-N16R8 Pinout & iPod Pin Usag

## ESP32-S3-N16R8 Complete Pinout

```
                    ESP32-S3-N16R8 DevKit-C
                    ┌─────────────────────┐
                    │                     │
                    │        USB-C        │
                    │                     │
    3V3  ●──────────┤ 3V3             EN  ├──────────● EN
    GND  ●──────────┤ GND            IO0  ├──────────● GPIO0 (Boot)
    TX   ●──────────┤ TX             IO1  ├──────────● GPIO1 🎵 ENCODER_CLK
    RX   ●──────────┤ RX             IO2  ├──────────● GPIO2 🎵 ENCODER_DT
    GPIO1●──────────┤ IO1            IO42 ├──────────● GPIO42
    GPIO2●──────────┤ IO2            IO41 ├──────────● GPIO41
    GPIO42●─────────┤ IO42           IO40 ├──────────● GPIO40
    GPIO41●─────────┤ IO41           IO39 ├──────────● GPIO39 🔋 POWER_ENABLE
    GPIO40●─────────┤ IO40           IO38 ├──────────● GPIO38 💾 SD_SCLK
    GPIO39●─────────┤ IO39           IO37 ├──────────● GPIO37 💾 SD_MISO
    GPIO38●─────────┤ IO38           IO36 ├──────────● GPIO36 💾 SD_MOSI
    GPIO37●─────────┤ IO37           IO35 ├──────────● GPIO35 💾 SD_CS
    GPIO36●─────────┤ IO36           IO0  ├──────────● GPIO0 (Boot)
    GPIO35●─────────┤ IO35           IO45 ├──────────● GPIO45
    GPIO0 ●─────────┤ IO0            IO48 ├──────────● GPIO48
    GPIO45●─────────┤ IO45           IO47 ├──────────● GPIO47
    GPIO48●─────────┤ IO48           IO21 ├──────────● GPIO21 🔋 CHARGE_STATUS
    GPIO47●─────────┤ IO47           IO14 ├──────────● GPIO14
    GPIO21●─────────┤ IO21           IO13 ├──────────● GPIO13 📺 TFT_MISO
    GPIO14●─────────┤ IO14           IO12 ├──────────● GPIO12 📺 TFT_SCLK
    GPIO13●─────────┤ IO13           IO11 ├──────────● GPIO11 📺 TFT_MOSI
    GPIO12●─────────┤ IO12           IO10 ├──────────● GPIO10 📺 TFT_CS
    GPIO11●─────────┤ IO11           IO9  ├──────────● GPIO9  📺 TFT_DC
    GPIO10●─────────┤ IO10           IO46 ├──────────● GPIO46
    GPIO9 ●─────────┤ IO9            IO3  ├──────────● GPIO3  👆 TOUCH_IRQ
    GPIO46●─────────┤ IO46           IO8  ├──────────● GPIO8  📺 TFT_RST
    GPIO3 ●─────────┤ IO3            IO18 ├──────────● GPIO18 🎧 USB_AUDIO_DP
    GPIO8 ●─────────┤ IO8            IO17 ├──────────● GPIO17 🎧 USB_AUDIO_EN
    GPIO18●─────────┤ IO18           IO16 ├──────────● GPIO16
    GPIO17●─────────┤ IO17           IO15 ├──────────● GPIO15
    GPIO16●─────────┤ IO16           IO7  ├──────────● GPIO7  👆 TOUCH_CLK
    GPIO15●─────────┤ IO15           IO6  ├──────────● GPIO6  👆 TOUCH_CS
    GPIO7 ●─────────┤ IO7            IO5  ├──────────● GPIO5  👆 TOUCH_DIN
    GPIO6 ●─────────┤ IO6            IO4  ├──────────● GPIO4  👆 TOUCH_DO
    GPIO5 ●─────────┤ IO5            GND  ├──────────● GND
    GPIO4 ●─────────┤ IO4            5V   ├──────────● 5V
                    │                     │
                    └─────────────────────┘
```

## iPod Project Pin Assignments (Pure Touch Interface)

### 📺 TFT Display with Built-in Touch (ILI9341 + XPT2046) - Single Module
```
ESP32-S3    Function    Display Pin    Description
GPIO10  →   TFT_CS   →  CS            Display Chip Select
GPIO9   →   TFT_DC   →  DC/RS         Display Data/Command
GPIO8   →   TFT_RST  →  RST           Display Reset
GPIO11  →   TFT_MOSI →  SDA/MOSI      Display Data Out
GPIO12  →   TFT_SCLK →  SCL/SCLK      Display Clock
GPIO13  →   TFT_MISO →  SDO/MISO      Display Data In (optional)
GPIO6   →   TOUCH_CS →  T_CS          Touch Chip Select
GPIO7   →   TOUCH_CLK→  T_CLK         Touch Clock
GPIO5   →   TOUCH_DIN→  T_DIN         Touch Data In
GPIO4   →   TOUCH_DO →  T_DO          Touch Data Out
GPIO3   →   TOUCH_IRQ→  T_IRQ         Touch Interrupt (optional)
3.3V    →   VCC      →  VCC           Power
3.3V    →   LED      →  LED           Backlight
GND     →   GND      →  GND           Ground
```

### 💾 SD Card Module - SPI3
```
ESP32-S3    Function    SD Module     Description
GPIO35  →   SD_CS    →  CS           Chip Select
GPIO36  →   SD_MOSI  →  MOSI/CMD     Data Out
GPIO37  →   SD_MISO  →  MISO/DAT0    Data In
GPIO38  →   SD_SCLK  →  SCLK/CLK     Clock
3.3V    →   VCC      →  VCC          Power
GND     →   GND      →  GND          Ground
```

### 🎧 USB-C Audio & Power
```
ESP32-S3    Function           Description
GPIO18  →   USB_AUDIO_DP    →  USB Data Plus
GPIO19  →   USB_AUDIO_DN    →  USB Data Minus (built-in)
GPIO17  →   USB_AUDIO_EN    →  Audio Enable Control
GPIO20  →   USB_VBUS_DET    →  USB Power Detection
```

### 🔋 Power Management
```
ESP32-S3    Function           Description
GPIO39  →   POWER_ENABLE    →  Main Power Control
GPIO21  →   CHARGE_STATUS   →  Battery Charge Status
GPIO22  →   BATTERY_ADC     →  Battery Voltage Monitor
```

## Pin Usage Summary

### Used Pins (18 total):
- **GPIO3**: Touch IRQ (optional)
- **GPIO4**: Touch DO
- **GPIO5**: Touch DIN
- **GPIO6**: Touch CS
- **GPIO7**: Touch CLK
- **GPIO8**: TFT Reset
- **GPIO9**: TFT DC
- **GPIO10**: TFT CS
- **GPIO11**: TFT MOSI
- **GPIO12**: TFT SCLK
- **GPIO13**: TFT MISO (optional)
- **GPIO17**: USB Audio Enable
- **GPIO18**: USB Audio DP
- **GPIO19**: USB Audio DN (built-in)
- **GPIO20**: USB VBUS Detect
- **GPIO21**: Charge Status
- **GPIO22**: Battery ADC
- **GPIO35**: SD CS
- **GPIO36**: SD MOSI
- **GPIO37**: SD MISO
- **GPIO38**: SD SCLK
- **GPIO39**: Power Enable

### Available Pins (27 remaining):
- **GPIO0-2**: Available for future expansion
- **GPIO14-16**: Available for I2C devices, LEDs, sensors
- **GPIO40-48**: Available for additional features
- **Massive expansion potential**: Accelerometer, gyroscope, ambient light sensor, wireless charging, etc.

## Important Notes

### ⚠️ Special Pins:
- **GPIO0**: Boot pin - hold LOW during boot to enter download mode
- **GPIO19**: Built-in USB D- (can't be changed)
- **GPIO20**: Built-in USB D+ (can't be changed)
- **GPIO43, GPIO44**: Built-in UART (TX/RX)

### 🔧 Component Count:
- **ESP32-S3**: Main controller
- **TFT Display with Touch**: Single integrated module (your MSP2806)
- **SD Card Module**: Music storage
- **USB-C Components**: Power and audio
- **Battery System**: Power management

### 💡 Pure Touch Interface Benefits:
- **Zero physical buttons** - Completely touch-controlled
- **Single display module** - Built-in touch eliminates separate touch controller
- **Maximum simplicity** - Minimal component count
- **Modern UX** - Full gesture support (swipe, tap, long press)
- **Clean design** - No external buttons or encoders
- **Maximum GPIO availability** - 27 pins free for expansion

This pinout gives you the ultimate minimalist touchscreen iPod - just ESP32-S3, your existing display with built-in touch, SD card, and USB-C. Maximum simplicity with maximum functionality!