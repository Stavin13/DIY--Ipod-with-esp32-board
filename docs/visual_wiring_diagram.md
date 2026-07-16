# Visual Wiring Diagram - ESP32-S3 iPod

## Complete System Wiring

```
                    ┌─────────────────────────────────────────┐
                    │           ESP32-S3-N16R8                │
                    │                                         │
                    │  ┌─────┐                               │
                    │  │USB-C│                               │
                    │  └─────┘                               │
                    │                                         │
    ┌───────────────┼─ GPIO10 (TFT_CS)                       │
    │   ┌───────────┼─ GPIO9  (TFT_DC)                       │
    │   │   ┌───────┼─ GPIO8  (TFT_RST)                      │
    │   │   │   ┌───┼─ GPIO11 (TFT_MOSI)                     │
    │   │   │   │ ┌─┼─ GPIO12 (TFT_SCLK)                     │
    │   │   │   │ │ └─ GPIO13 (TFT_MISO)                     │
    │   │   │   │ │                                           │
    │   │   │   │ │   ┌───────┼─ GPIO6  (TOUCH_CS)           │
    │   │   │   │ │   │   ┌───┼─ GPIO7  (TOUCH_CLK)          │
    │   │   │   │ │   │   │ ┌─┼─ GPIO5  (TOUCH_DIN)          │
    │   │   │   │ │   │   │ │ └─ GPIO4  (TOUCH_DO)           │
    │   │   │   │ │   │   │ │                                 │
    │   │   │   │ │   │   │ │   ┌───────┼─ GPIO35 (SD_CS)    │
    │   │   │   │ │   │   │ │   │   ┌───┼─ GPIO36 (SD_MOSI)  │
    │   │   │   │ │   │   │ │   │   │ ┌─┼─ GPIO37 (SD_MISO)  │
    │   │   │   │ │   │   │ │   │   │ │ └─ GPIO38 (SD_SCLK)  │
    │   │   │   │ │   │   │ │   │   │ │                       │
    │   │   │   │ │   │   │ │   │   │ │   ┌───────┼─ GPIO1 (ENC_CLK) │
    │   │   │   │ │   │   │ │   │   │ │   │   ┌───┼─ GPIO2 (ENC_DT)  │
    │   │   │   │ │   │   │ │   │   │ │   │   │ ┌─┼─ GPIO0 (ENC_SW)  │
    │   │   │   │ │   │   │ │   │   │ │   │   │ │                     │
    │   │   │   │ │   │   │ │   │   │ │   │   │ │ ┌─────────┼─ 3.3V   │
    │   │   │   │ │   │   │ │   │   │ │   │   │ │ │     ┌───┼─ GND    │
    └───┼───┼───┼─┼───┼───┼─┼───┼───┼─┼───┼───┼─┼─┼─────┼───┼─────────┘
        │   │   │ │   │   │ │   │   │ │   │   │ │ │     │   │
        │   │   │ │   │   │ │   │   │ │   │   │ │ │     │   │
    ┌───▼───▼───▼─▼───▼───▼─▼───┐   │ │   │   │ │ │     │   │
    │  2.8" ILI9341 TFT Display │   │ │   │   │ │ │     │   │
    │  ┌─────────────────────┐   │   │ │   │   │ │ │     │   │
    │  │                     │   │   │ │   │   │ │ │     │   │
    │  │     240 x 320       │   │   │ │   │   │ │ │     │   │
    │  │    Color Display    │   │   │ │   │   │ │ │     │   │
    │  │                     │   │   │ │   │   │ │ │     │   │
    │  └─────────────────────┘   │   │ │   │   │ │ │     │   │
    │                            │   │ │   │   │ │ │     │   │
    │ CS DC RST MOSI SCLK MISO   │   │ │   │   │ │ │     │   │
    │ │  │  │   │    │    │      │   │ │   │   │ │ │     │   │
    │ │  │  │   │    │    │      │   │ │   │   │ │ │     │   │
    │ T_CS T_CLK T_DIN T_DO      │   │ │   │   │ │ │     │   │
    │ │    │     │     │         │   │ │   │   │ │ │     │   │
    └─┼────┼─────┼─────┼─────────┘   │ │   │   │ │ │     │   │
      │    │     │     │             │ │   │   │ │ │     │   │
      └────┼─────┼─────┼─────────────┘ │   │   │ │ │     │   │
           └─────┼─────┼───────────────┘   │   │ │ │     │   │
                 └─────┼───────────────────┘   │ │ │     │   │
                       └───────────────────────┘ │ │     │   │
                                                 │ │     │   │
                       ┌─────────────────────────┘ │     │   │
                       │ ┌─────────────────────────┘     │   │
                       │ │ ┌─────────────────────────────┘   │
                       │ │ │ ┌─────────────────────────────────┘
                       │ │ │ │
                   ┌───▼─▼─▼─▼───┐         ┌─────────────────┐
                   │ SD Card     │         │ Rotary Encoder  │
                   │ Module      │         │ (KY-040)        │
                   │             │         │                 │
                   │ CS MOSI     │         │ CLK DT SW       │
                   │ MISO SCLK   │         │ │   │  │        │
                   │ VCC GND     │         │ │   │  │        │
                   └─────────────┘         └─┼───┼──┼────────┘
                                             │   │  │
                                             └───┼──┼────────┐
                                                 └──┼──────┐ │
                                                    └────┐ │ │
                                                         │ │ │
                   ┌─────────────────────────────────────┘ │ │
                   │ ┌─────────────────────────────────────┘ │
                   │ │ ┌─────────────────────────────────────┘
                   │ │ │
               ┌───▼─▼─▼───┐
               │ Power &   │
               │ Battery   │
               │ Management│
               └───────────┘
```

## Connection Tables

### 🔌 Power Connections (All Modules)
```
ESP32-S3    →    All Modules
3.3V        →    VCC (TFT, Touch, SD, Encoder)
GND         →    GND (TFT, Touch, SD, Encoder)
```

### 📺 TFT Display Connections
```
ESP32-S3    Wire Color    ILI9341    Function
GPIO10   →  Orange     →  CS      →  Chip Select
GPIO9    →  Yellow     →  DC      →  Data/Command  
GPIO8    →  Green      →  RST     →  Reset
GPIO11   →  Blue       →  SDA     →  Serial Data (MOSI)
GPIO12   →  Purple     →  SCL     →  Serial Clock
GPIO13   →  Gray       →  SDO     →  Serial Data Out (MISO)
3.3V     →  Red        →  VCC     →  Power
3.3V     →  Red        →  LED     →  Backlight
GND      →  Black      →  GND     →  Ground
```

### 👆 Touch Connections (Same Display)
```
ESP32-S3    Wire Color    Touch     Function
GPIO6    →  White      →  T_CS   →  Touch Chip Select
GPIO7    →  Brown      →  T_CLK  →  Touch Clock
GPIO5    →  Pink       →  T_DIN  →  Touch Data In
GPIO4    →  Cyan       →  T_DO   →  Touch Data Out
GPIO3    →  Lime       →  T_IRQ  →  Touch Interrupt
```

### 💾 SD Card Connections
```
ESP32-S3    Wire Color    SD Card   Function
GPIO35   →  Orange     →  CS     →  Chip Select
GPIO36   →  Blue       →  MOSI   →  Master Out
GPIO37   →  Purple     →  MISO   →  Master In
GPIO38   →  Yellow     →  SCLK   →  Serial Clock
3.3V     →  Red        →  VCC    →  Power
GND      →  Black      →  GND    →  Ground
```

### 🎵 Rotary Encoder Connections
```
ESP32-S3    Wire Color    Encoder   Function
GPIO1    →  Green      →  CLK    →  Clock/A Phase
GPIO2    →  Blue       →  DT     →  Data/B Phase
GPIO0    →  Yellow     →  SW     →  Switch/Button
3.3V     →  Red        →  +      →  Power
GND      →  Black      →  GND    →  Ground
```

## Breadboard Layout Tips

### 🍞 Breadboard Sections:
1. **Top**: ESP32-S3 (center)
2. **Left**: TFT Display connections
3. **Right**: SD Card + Encoder
4. **Bottom**: Power rails (3.3V/GND)

### 📏 Wire Lengths:
- **Short (5cm)**: Power connections
- **Medium (10cm)**: SPI connections  
- **Long (15cm)**: Cross-breadboard connections

### 🎨 Color Coding:
- **Red**: 3.3V Power
- **Black**: Ground
- **Orange**: Chip Select pins
- **Blue**: MOSI/Data pins
- **Purple**: MISO/Data pins
- **Yellow**: Clock pins
- **Green**: Control pins

This layout keeps all SPI buses separate and organized for easy troubleshooting!