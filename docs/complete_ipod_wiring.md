# 🎵 Complete iPod Wiring Guide
## ESP32-S3 + Display + TTP229 + SD Card + Audio + Battery

---

## 📦 **Components You Need**

1. **ESP32-S3-N16R8** DevKit
2. **MSP2806 Display** (2.8" ILI9341)
3. **TTP229 16-Key Touch Module**
4. **SD Card Module** (SPI)
5. **PCM5102 DAC Module** (I2S audio)
6. **TP4056 USB-C Charging Module**
7. **3.7V LiPo Battery** (2000-3000mAh)
8. **3.5mm Headphone Jack**
9. **Jumper wires**

---

## 🔌 **Complete Pin Mapping**

### **Power Distribution**
```
TP4056 Module:
  USB-C Input → Charging
  B+ → Battery Positive
  B- → Battery Negative
  OUT+ → ESP32 5V/VIN
  OUT- → Common GND

Battery:
  3.7V LiPo (2000mAh+)
  Red → TP4056 B+
  Black → TP4056 B-
```

### **ESP32-S3 Connections**

#### **Display (MSP2806) - SPI**
```
Display Pin    ESP32-S3 Pin    Function
-----------    ------------    --------
VCC            3.3V            Power
GND            GND             Ground
CS             GPIO10          Chip Select
RST            GPIO8           Reset
DC             GPIO9           Data/Command
SDA/MOSI       GPIO11          SPI Data Out
SCL/SCLK       GPIO12          SPI Clock
SDO/MISO       GPIO13          SPI Data In
LED            3.3V            Backlight
```

#### **TTP229 Touch - Serial Mode**
```
TTP229 Pin     ESP32-S3 Pin    Function
----------     ------------    --------
VCC            3.3V            Power
GND            GND             Ground
SCL            GPIO2           Clock
SDO            GPIO1           Data
```

#### **SD Card Module - SPI (Shared with Display)**
```
SD Card Pin    ESP32-S3 Pin    Function
-----------    ------------    --------
VCC            3.3V            Power
GND            GND             Ground
CS             GPIO5           Chip Select
MOSI           GPIO11          SPI Data Out (shared)
SCK            GPIO12          SPI Clock (shared)
MISO           GPIO13          SPI Data In (shared)
```

#### **PCM5102 DAC - I2S Audio**
```
PCM5102 Pin    ESP32-S3 Pin    Function
-----------    ------------    --------
VCC            3.3V            Power
GND            GND             Ground
BCK            GPIO4           Bit Clock
DIN            GPIO3           Data In
LCK            GPIO6           Left/Right Clock
SCK            GND             System Clock (tie to GND)
FMT            GND             Format (I2S mode)
XMT            3.3V            Mute control (HIGH = unmuted)
```

#### **Headphone Jack**
```
PCM5102 → 3.5mm Jack:
  LOUT → Left Channel (Tip)
  ROUT → Right Channel (Ring)
  GND  → Ground (Sleeve)
```

---

## 📊 **GPIO Pin Summary**

```
ESP32-S3 Pin Usage:
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
GPIO1  - TTP229 SDO (Touch Data)
GPIO2  - TTP229 SCL (Touch Clock)
GPIO3  - PCM5102 DIN (Audio Data)
GPIO4  - PCM5102 BCK (Audio Bit Clock)
GPIO5  - SD Card CS (Chip Select)
GPIO6  - PCM5102 LCK (Audio L/R Clock)
GPIO8  - Display RST (Reset)
GPIO9  - Display DC (Data/Command)
GPIO10 - Display CS (Chip Select)
GPIO11 - SPI MOSI (Display + SD Card)
GPIO12 - SPI SCK (Display + SD Card)
GPIO13 - SPI MISO (Display + SD Card)
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Total: 13 GPIO pins used
```

---

## 🎨 **Visual Wiring Diagram**

```
                    ┌─────────────────────────────────┐
                    │       ESP32-S3 DevKit           │
                    │                                 │
    ┌───────────────┤ 3.3V (multiple connections)    │
    │               │ GND  (common ground)            │
    │               │                                 │
    │               │ GPIO1  ←─────────────────────┐  │
    │               │ GPIO2  ←───────────────────┐ │  │
    │               │ GPIO3  ─────────────────┐  │ │  │
    │               │ GPIO4  ───────────────┐ │  │ │  │
    │               │ GPIO5  ─────────────┐ │ │  │ │  │
    │               │ GPIO6  ───────────┐ │ │ │  │ │  │
    │               │ GPIO8  ─────────┐ │ │ │ │  │ │  │
    │               │ GPIO9  ───────┐ │ │ │ │ │  │ │  │
    │               │ GPIO10 ─────┐ │ │ │ │ │ │  │ │  │
    │               │ GPIO11 ───┐ │ │ │ │ │ │ │  │ │  │
    │               │ GPIO12 ─┐ │ │ │ │ │ │ │ │  │ │  │
    │               │ GPIO13 ┐│ │ │ │ │ │ │ │ │  │ │  │
    │               │        ││ │ │ │ │ │ │ │ │  │ │  │
    │               │ VIN ←──┼┼─┼─┼─┼─┼─┼─┼─┼─┼──┼─┼──┤
    │               └────────┼┼─┼─┼─┼─┼─┼─┼─┼─┼──┼─┼──┘
    │                        ││ │ │ │ │ │ │ │ │  │ │
    │  ┌─────────────────────┼┼─┼─┼─┼─┼─┼─┼─┼─┼──┘ │
    │  │  ┌──────────────────┼┼─┼─┼─┼─┼─┼─┼─┼─┘    │
    │  │  │  ┌───────────────┼┼─┼─┼─┼─┼─┼─┼─┘      │
    │  │  │  │  ┌────────────┼┼─┼─┼─┼─┼─┼─┘        │
    │  │  │  │  │  ┌─────────┼┼─┼─┼─┼─┼─┘          │
    │  │  │  │  │  │  ┌──────┼┼─┼─┼─┼─┘            │
    │  │  │  │  │  │  │  ┌───┼┼─┼─┼─┘              │
    │  │  │  │  │  │  │  │   ││ │ │                │
    │  │  │  │  │  │  │  │   ││ │ │                │
    ▼  ▼  ▼  ▼  ▼  ▼  ▼  ▼   ▼▼ ▼ ▼                ▼
┌──────────────────────┐  ┌──────────┐  ┌────────────────┐
│   MSP2806 Display    │  │ SD Card  │  │   TTP229 Touch │
│   (2.8" ILI9341)     │  │  Module  │  │   16-Key Pad   │
├──────────────────────┤  ├──────────┤  ├────────────────┤
│ VCC  GND  CS  RST DC │  │VCC GND CS│  │ VCC GND SCL SDO│
│ MOSI SCK MISO LED    │  │MOSI SCK  │  └────────────────┘
└──────────────────────┘  │MISO      │
                          └──────────┘

         ┌──────────────────────────┐
         │   PCM5102 DAC Module     │
         ├──────────────────────────┤
         │ VCC GND BCK DIN LCK      │
         │ SCK FMT XMT LOUT ROUT    │
         └──────────────────────────┘
                    │
                    ▼
         ┌──────────────────────────┐
         │   3.5mm Headphone Jack   │
         │   Tip Ring Sleeve        │
         └──────────────────────────┘

┌────────────────────────────────────────┐
│         Power System                   │
├────────────────────────────────────────┤
│  USB-C → [TP4056] → Battery (3.7V)    │
│              ↓                         │
│          OUT+ OUT-                     │
│              ↓                         │
│          ESP32 VIN                     │
└────────────────────────────────────────┘
```

---

## 🔧 **Step-by-Step Wiring**

### **Step 1: Power System**
```
1. Connect battery to TP4056:
   Battery Red → TP4056 B+
   Battery Black → TP4056 B-

2. Connect TP4056 to ESP32:
   TP4056 OUT+ → ESP32 VIN (or 5V)
   TP4056 OUT- → ESP32 GND

3. Create common ground rail on breadboard
   Connect all GND pins together
```

### **Step 2: Display (Already Working)**
```
Display → ESP32-S3:
  VCC → 3.3V
  GND → GND
  CS → GPIO10
  RST → GPIO8
  DC → GPIO9
  MOSI → GPIO11
  SCK → GPIO12
  MISO → GPIO13
  LED → 3.3V
```

### **Step 3: TTP229 Touch (Already Working)**
```
TTP229 → ESP32-S3:
  VCC → 3.3V
  GND → GND
  SCL → GPIO2
  SDO → GPIO1
```

### **Step 4: SD Card Module**
```
SD Card → ESP32-S3:
  VCC → 3.3V
  GND → GND
  CS → GPIO5
  MOSI → GPIO11 (shared with display)
  SCK → GPIO12 (shared with display)
  MISO → GPIO13 (shared with display)
```

### **Step 5: PCM5102 DAC**
```
PCM5102 → ESP32-S3:
  VCC → 3.3V
  GND → GND
  BCK → GPIO4
  DIN → GPIO3
  LCK → GPIO6
  SCK → GND (tie to ground)
  FMT → GND (I2S mode)
  XMT → 3.3V (unmute)

PCM5102 → Headphone Jack:
  LOUT → Jack Tip (Left)
  ROUT → Jack Ring (Right)
  GND → Jack Sleeve (Ground)
```

---

## ⚠️ **Important Notes**

### **SPI Bus Sharing**
The display and SD card share the same SPI bus (MOSI, SCK, MISO). They use different CS pins:
- Display CS: GPIO10
- SD Card CS: GPIO5

This is normal and works perfectly!

### **Power Considerations**
```
Component Power Draw:
- ESP32-S3: ~80-240mA (WiFi active)
- Display: ~100mA (backlight on)
- SD Card: ~50-100mA (reading)
- DAC: ~10mA
- TTP229: ~3mA
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Total: ~250-450mA typical

Recommended Battery: 2000-3000mAh
Battery Life: 4-8 hours
```

### **PCM5102 Configuration**
The PCM5102 has configuration pins:
- **SCK → GND**: Use ESP32 as master clock
- **FMT → GND**: I2S format (not left-justified)
- **XMT → 3.3V**: Unmute (always on)

Some PCM5102 modules have these pre-configured with solder bridges on the back.

---

## 💻 **Code Configuration**

Update `src/config.h`:

```cpp
// Display pins (already configured)
#define TFT_CS   10
#define TFT_RST  8
#define TFT_DC   9
#define TFT_MOSI 11
#define TFT_SCLK 12
#define TFT_MISO 13

// TTP229 Touch pins (already configured)
#define TTP_SDO  1
#define TTP_SCL  2

// SD Card pins
#define SD_CS    5
#define SD_MOSI  11  // Shared with display
#define SD_SCK   12  // Shared with display
#define SD_MISO  13  // Shared with display

// I2S Audio pins (PCM5102)
#define I2S_BCLK 4   // Bit clock
#define I2S_LRC  6   // Left/Right clock
#define I2S_DOUT 3   // Data out

// Battery monitoring (optional)
#define BATTERY_PIN 7  // ADC pin for voltage monitoring
```

---

## 🧪 **Testing Each Component**

### **1. Test SD Card**
```cpp
#include <SD.h>
#include <SPI.h>

void setup() {
  Serial.begin(115200);
  
  if (!SD.begin(5)) {  // GPIO5 = CS
    Serial.println("SD Card failed!");
    return;
  }
  
  Serial.println("SD Card OK!");
  Serial.print("Size: ");
  Serial.print(SD.cardSize() / 1024 / 1024);
  Serial.println(" MB");
}
```

### **2. Test Audio (I2S)**
```cpp
#include "driver/i2s.h"

void setup() {
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = 44100,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = I2S_COMM_FORMAT_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = 64
  };
  
  i2s_pin_config_t pin_config = {
    .bck_io_num = 4,    // BCK
    .ws_io_num = 6,     // LRC
    .data_out_num = 3,  // DIN
    .data_in_num = -1
  };
  
  i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_NUM_0, &pin_config);
  
  Serial.println("I2S Audio initialized!");
}
```

---

## 📝 **Shopping List (India)**

```
Component              Price (₹)    Link/Store
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
ESP32-S3-N16R8         800-1200     Robu.in, Amazon
MSP2806 Display        400-600      Already have
TTP229 16-Key          300-400      Already have
SD Card Module         50-100       Robu.in, local
PCM5102 DAC            300-500      Robu.in, Amazon
TP4056 USB-C           14-50        Already have
3.7V LiPo 2500mAh      300-500      Robu.in, Amazon
3.5mm Jack             10-20        Local electronics
Jumper Wires (40pcs)   50-100       Local/Amazon
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Total (new items):     ~₹700-1200
```

---

## 🎯 **Final Checklist**

Before powering on:

**Power:**
- [ ] Battery connected to TP4056 (correct polarity!)
- [ ] TP4056 OUT+ to ESP32 VIN
- [ ] All GND connected together

**Display:**
- [ ] All 9 wires connected
- [ ] CS on GPIO10

**Touch:**
- [ ] SCL on GPIO2
- [ ] SDO on GPIO1

**SD Card:**
- [ ] CS on GPIO5
- [ ] MOSI/SCK/MISO shared with display

**Audio:**
- [ ] BCK on GPIO4
- [ ] DIN on GPIO3
- [ ] LCK on GPIO6
- [ ] SCK tied to GND
- [ ] FMT tied to GND
- [ ] XMT tied to 3.3V

**Safety:**
- [ ] No short circuits
- [ ] Battery polarity correct
- [ ] All connections secure

---

## 🚀 **You're Ready!**

You now have a complete iPod with:
- ✅ Touch gesture control (16 keys)
- ✅ Color display
- ✅ SD card for music storage
- ✅ High-quality audio output
- ✅ Rechargeable battery
- ✅ USB-C charging

Enjoy your custom iPod! 🎵
