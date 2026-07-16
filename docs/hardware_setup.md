# Hardware Setup Guide

## Components Required

### Core Components
- ESP32-S3 DevKit (16MB Flash, 8MB PSRAM)
- 2.8" ILI9341 TFT Display (240x320, SPI)
- Capacitive Touch Controller (FT6236 or similar)
- MicroSD Card Module
- Rotary Encoder (KY-040)
- 18650 LiPo Battery (3500mAh)

## USB-C Audio + Charging System

### **Approach 1: USB-C DAC Dongle (Recommended)**
```
ESP32-S3 USB Host → USB-C DAC Dongle → Headphones/Speakers
ESP32-S3 ← USB-C Charging Port ← Power Adapter
```

**Advantages:**
- Simple implementation
- High-quality DAC (24-bit/96kHz)
- Plug-and-play compatibility
- Separate charging and audio ports

### **Approach 2: Single USB-C Port (Advanced)**
```
ESP32-S3 ← USB-C Controller (IP2723T) ← USB-C Port
         ↓                           ↓
    Audio DAC (PCM2912A)        Charging Circuit
```

**Advantages:**
- Single port for everything
- More compact design
- Professional implementation

**Disadvantages:**
- Complex circuit design
- Cannot charge while using wired audio
- Requires USB-C PD controller

## Recommended: Dual USB-C Design

For the best user experience, use **two USB-C ports**:

1. **USB-C Audio Port**: Connect USB-C DAC dongle or USB-C headphones
2. **USB-C Charging Port**: Dedicated charging (can use while listening)

### **Pin Connections (Dual USB-C)**
```
ESP32-S3    USB-C Audio    USB-C Charging
GPIO18  ->  D+ (Data)      -
GPIO19  ->  D- (Data)      -
GPIO20  ->  VBUS_DETECT    -
-       ->  -              VBUS (5V)
-       ->  -              GND
GPIO21  ->  -              CHG_STATUS
```

## Pin Connections

### Display + Touch Pins (Test Required)
```
ESP32-S3    ILI9341
GPIO10  ->  CS (Display)
GPIO9   ->  DC
GPIO8   ->  RST
GPIO11  ->  MOSI (SDI)
GPIO12  ->  SCLK (SCK)
GPIO13  ->  MISO (SDO)
3.3V    ->  VCC
GND     ->  GND
GPIO2   ->  LED (Backlight)

# Touch Pins (if functional - test required)
GPIO6   ->  T_CS
GPIO7   ->  T_CLK  
GPIO5   ->  T_DIN
GPIO4   ->  T_DO
GPIO3   ->  T_IRQ (optional)
```

**Note**: Label says "Touch: N" but pins are present. Test to confirm if touch controller is actually populated and functional.

### SD Card (SPI)
```
ESP32-S3    SD Card
GPIO35  ->  CS
GPIO36  ->  MOSI
GPIO37  ->  MISO
GPIO38  ->  SCLK
3.3V    ->  VCC
GND     ->  GND
```

### Rotary Encoder
```
ESP32-S3    Encoder
GPIO1   ->  CLK
GPIO2   ->  DT
GPIO3   ->  SW
3.3V    ->  VCC
GND     ->  GND
```

### USB-C Audio Port
```
ESP32-S3    USB-C Audio
GPIO18  ->  D+ (Data)
GPIO19  ->  D- (Data)  
GPIO20  ->  VBUS_DETECT
GPIO17  ->  AUDIO_ENABLE
5V      ->  VBUS
GND     ->  GND
```

### USB-C Charging Port
```
ESP32-S3    USB-C Charge
GPIO21  ->  CHG_STATUS
GPIO39  ->  POWER_ENABLE
5V      ->  VBUS (to TP4056)
GND     ->  GND
```

### Power Management
```
ESP32-S3    Power
GPIO22  ->  Battery ADC
3.3V    ->  System Power
GND     ->  System Ground
```

## Assembly Notes

1. **Power Supply**: Use 3.3V for logic, 5V for display backlight
2. **Decoupling**: Add 100nF capacitors near each IC
3. **Pull-ups**: I2C lines need 4.7kΩ pull-ups
4. **Audio**: Keep audio traces short and away from digital signals
5. **Shielding**: Consider copper pour for ground plane

## Case Design

- 3D printable case with access to:
  - **Two USB-C ports** (Audio + Charging)
  - SD card slot
  - Rotary encoder
  - Power button
- Display window with touch access (if functional)
- Battery compartment
- Ventilation for heat dissipation
- **No 3.5mm jack needed** - USB-C audio only

## Testing Checklist

- [ ] Power rails (3.3V, 5V)
- [ ] Display initialization
- [ ] Touch response
- [ ] SD card detection
- [ ] Rotary encoder rotation/click
- [ ] Audio output
- [ ] Battery charging
- [ ] USB-C connection
- [ ] Bluetooth pairing