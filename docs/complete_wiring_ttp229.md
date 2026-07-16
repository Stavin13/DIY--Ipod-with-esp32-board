# Complete Wiring Guide - ESP32-S3 + TTP229 + Display

## 🔌 **Complete System Connections**

### **Components:**
1. ESP32-S3 DevKit
2. TTP229 16-Key Touch Module
3. MSP2806 Display (2.8" ILI9341)

---

## 📋 **Quick Connection Summary**

### **TTP229 Touch Module (4 wires)**
```
TTP229 Pin    →    ESP32-S3 Pin
----------         ------------
VCC           →    3.3V
GND           →    GND
SCL           →    GPIO2 (I2C Clock)
SDO           →    GPIO1 (I2C Data/SDA)
```

### **MSP2806 Display (9 wires)**
```
Display Pin   →    ESP32-S3 Pin
-----------        ------------
VCC           →    3.3V
GND           →    GND
CS            →    GPIO10
RST           →    GPIO8
DC            →    GPIO9
SDA/MOSI      →    GPIO11
SCL/SCLK      →    GPIO12
SDO/MISO      →    GPIO13
LED           →    3.3V
```

---

## 🎨 **Visual Wiring Diagram**

```
                    ESP32-S3 DevKit
                    
                      [USB-C Port]
         ┌─────────────────────────────────┐
         │                                 │
         │  3.3V ●─────────────────┐       │
         │   GND ●───────────┐     │       │
         │                   │     │       │
         │  IO21 ●───────┐   │     │       │
         │  IO22 ●─────┐ │   │     │       │
         │             │ │   │     │       │
         │   IO8 ●───┐ │ │   │     │       │
         │   IO9 ●─┐ │ │ │   │     │       │
         │  IO10 ●─│─│─│─│───│─────│───┐   │
         │  IO11 ●─│─│─│─│───│─────│─┐ │   │
         │  IO12 ●─│─│─│─│───│─────│─│─│─┐ │
         │  IO13 ●─│─│─│─│───│─────│─│─│─│─│
         │         │ │ │ │   │     │ │ │ │ │
         └─────────│─│─│─│───│─────│─│─│─│─│
                   │ │ │ │   │     │ │ │ │ │
                   │ │ │ │   │     │ │ │ │ │
    ┌──────────────│─│─│─│───│─────┘ │ │ │ │
    │              │ │ │ │   │       │ │ │ │
    │  TTP229      │ │ │ │   │       │ │ │ │
    │  Module      │ │ │ │   │       │ │ │ │
    │              │ │ │ │   │       │ │ │ │
    │  ┌────────┐  │ │ │ │   │       │ │ │ │
    │  │ Header │  │ │ │ │   │       │ │ │ │
    │  │  Pins  │  │ │ │ │   │       │ │ │ │
    │  └────────┘  │ │ │ │   │       │ │ │ │
    │   │ │ │ │    │ │ │ │   │       │ │ │ │
    │  VCC│ │ │    │ │ │ │   │       │ │ │ │
    │  GND│ │ │    │ │ │ │   │       │ │ │ │
    │  SCL│ │ │    │ │ │ │   │       │ │ │ │
    │  SDO│ │ │    │ │ │ │   │       │ │ │ │
    │     │ │ │    │ │ │ │   │       │ │ │ │
    └─────┼─┼─┼────┘ │ │ │   │       │ │ │ │
          │ │ │      │ │ │   │       │ │ │ │
          │ │ └──────┘ │ │   │       │ │ │ │
          │ └──────────┘ │   │       │ │ │ │
          │              │   │       │ │ │ │
          └──────────────┘   │       │ │ │ │
                             │       │ │ │ │
                             │       │ │ │ │
    ┌────────────────────────┘       │ │ │ │
    │                                │ │ │ │
    │  MSP2806 Display               │ │ │ │
    │  (2.8" ILI9341)                │ │ │ │
    │                                │ │ │ │
    │  ┌──────────────────────┐      │ │ │ │
    │  │                      │      │ │ │ │
    │  │    240 x 320 px      │      │ │ │ │
    │  │      Display         │      │ │ │ │
    │  │                      │      │ │ │ │
    │  └──────────────────────┘      │ │ │ │
    │                                │ │ │ │
    │  Pin Header:                   │ │ │ │
    │  VCC ──────────────────────────┘ │ │ │
    │  GND ────────────────────────────┘ │ │
    │  CS  ──────────────────────────────┘ │
    │  RST ────────────────────────────────┘
    │  DC  ──────────────────────────────┐
    │  SDA ────────────────────────────┐ │
    │  SCL ──────────────────────────┐ │ │
    │  SDO ────────────────────────┐ │ │ │
    │  LED ──────────────────────┐ │ │ │ │
    │                            │ │ │ │ │
    └────────────────────────────┼─┼─┼─┼─┼─┐
                                 │ │ │ │ │ │
                    To 3.3V ─────┘ │ │ │ │ │
                    To GPIO9 ───────┘ │ │ │
                    To GPIO11 ─────────┘ │ │
                    To GPIO12 ───────────┘ │
                    To GPIO13 ─────────────┘
```

---

## 📸 **Breadboard Layout**

```
                    Breadboard View
                    
    Power Rails:
    [+] ←─── ESP32-S3 3.3V
    [-] ←─── ESP32-S3 GND
    
    
    TTP229 Module:                    ESP32-S3:
    ┌──────────────┐                 ┌──────────┐
    │   [Header]   │                 │          │
    │   VCC ●──────┼─────────────────┤ 3.3V     │
    │   GND ●──────┼─────────────────┤ GND      │
    │   SCL ●──────┼─────────────────┤ GPIO22   │
    │   SDO ●──────┼─────────────────┤ GPIO21   │
    │              │                 │          │
    │  [1][2][3][4]│                 │          │
    │  [5][6][7][8]│                 │          │
    │  [9][10][11] │                 │          │
    │  [13][14][15]│                 │          │
    └──────────────┘                 └──────────┘
    
    
    Display Module:                   ESP32-S3:
    ┌──────────────┐                 ┌──────────┐
    │  [Display]   │                 │          │
    │   VCC ●──────┼─────────────────┤ 3.3V     │
    │   GND ●──────┼─────────────────┤ GND      │
    │   CS  ●──────┼─────────────────┤ GPIO10   │
    │   RST ●──────┼─────────────────┤ GPIO8    │
    │   DC  ●──────┼─────────────────┤ GPIO9    │
    │   SDA ●──────┼─────────────────┤ GPIO11   │
    │   SCL ●──────┼─────────────────┤ GPIO12   │
    │   SDO ●──────┼─────────────────┤ GPIO13   │
    │   LED ●──────┼─────────────────┤ 3.3V     │
    └──────────────┘                 └──────────┘
```

---

## 🔧 **Step-by-Step Wiring Instructions**

### **Step 1: Power Connections**
```
1. Connect ESP32-S3 3.3V to breadboard + rail
2. Connect ESP32-S3 GND to breadboard - rail
3. Connect TTP229 VCC to + rail
4. Connect TTP229 GND to - rail
5. Connect Display VCC to + rail
6. Connect Display GND to - rail
7. Connect Display LED to + rail (backlight)
```

### **Step 2: TTP229 I2C Connections**
```
1. TTP229 SCL → ESP32-S3 GPIO2 (yellow wire)
2. TTP229 SDO → ESP32-S3 GPIO1 (green wire)
```

### **Step 3: Display SPI Connections**
```
1. Display CS   → ESP32-S3 GPIO10 (orange wire)
2. Display RST  → ESP32-S3 GPIO8  (white wire)
3. Display DC   → ESP32-S3 GPIO9  (gray wire)
4. Display SDA  → ESP32-S3 GPIO11 (blue wire)
5. Display SCL  → ESP32-S3 GPIO12 (purple wire)
6. Display SDO  → ESP32-S3 GPIO13 (brown wire)
```

---

## 📊 **Pin Assignment Table**

### **ESP32-S3 Pin Usage**
```
GPIO Pin    Function        Connected To
--------    --------        ------------
GPIO1       I2C SDA         TTP229 SDO
GPIO2       I2C SCL         TTP229 SCL
GPIO8       Display RST     MSP2806 RST
GPIO9       Display DC      MSP2806 DC
GPIO10      Display CS      MSP2806 CS
GPIO11      SPI MOSI        MSP2806 SDA
GPIO12      SPI CLK         MSP2806 SCL
GPIO13      SPI MISO        MSP2806 SDO
3.3V        Power           TTP229 VCC, Display VCC, Display LED
GND         Ground          TTP229 GND, Display GND
```

### **Available GPIO Pins (for future expansion)**
```
GPIO0, GPIO3, GPIO4, GPIO5, GPIO6, GPIO7
GPIO14, GPIO15, GPIO16, GPIO17, GPIO18, GPIO19, GPIO20
GPIO35, GPIO36, GPIO37, GPIO38, GPIO39, GPIO40, GPIO41, GPIO42
GPIO43, GPIO44, GPIO45, GPIO46, GPIO47, GPIO48

Note: GPIO21 and GPIO22 are used for USB on ESP32-S3
```

---

## 🎨 **Color-Coded Wire Recommendations**

```
Power:
  Red    → 3.3V connections
  Black  → GND connections

TTP229 (I2C):
  Yellow → SCL (GPIO2)
  Green  → SDA (GPIO1)

Display (SPI):
  Orange → CS (GPIO10)
  White  → RST (GPIO8)
  Gray   → DC (GPIO9)
  Blue   → MOSI (GPIO11)
  Purple → CLK (GPIO12)
  Brown  → MISO (GPIO13)
```

---

## ✅ **Connection Checklist**

### **Before Powering On:**
```
□ TTP229 VCC connected to 3.3V
□ TTP229 GND connected to GND
□ TTP229 SCL connected to GPIO2
□ TTP229 SDO connected to GPIO1

□ Display VCC connected to 3.3V
□ Display GND connected to GND
□ Display LED connected to 3.3V
□ Display CS connected to GPIO10
□ Display RST connected to GPIO8
□ Display DC connected to GPIO9
□ Display SDA connected to GPIO11
□ Display SCL connected to GPIO12
□ Display SDO connected to GPIO13

□ No short circuits between power and ground
□ All connections are secure
□ Correct GPIO pins used
```

---

## 🔍 **Testing Connections**

### **Test 1: Power Test**
```
1. Connect USB-C to ESP32-S3
2. Check TTP229 power LED (if present)
3. Check Display backlight (should glow)
4. If no backlight, check LED connection
```

### **Test 2: TTP229 I2C Test**
```
1. Upload: src/ttp229_keypad_test.cpp
2. Open Serial Monitor (115200 baud)
3. Should see: "✅ TTP229 module found!"
4. Touch any key (1-16)
5. Should see: "👆 Key X pressed"
```

### **Test 3: Display Test**
```
1. Upload: src/simple_tft_test.cpp
2. Display should show colors
3. If blank: Check RST, DC, CS connections
4. If white: Check SPI connections (MOSI, CLK)
```

### **Test 4: Complete System Test**
```
1. Upload: src/ttp229_gesture_ipod.cpp
2. Display should show iPod interface
3. Touch keys to navigate
4. Try gestures (swipe, scroll)
5. Check Serial Monitor for gesture detection
```

---

## 🚨 **Troubleshooting**

### **TTP229 Not Detected**
```
Problem: "❌ TTP229 not found!"
Check:
1. VCC connected to 3.3V (not 5V!)
2. GND connected properly
3. SCL on GPIO2 (not GPIO1)
4. SDO on GPIO1 (not GPIO2)
5. Module has power LED on (if equipped)
```

### **Display Not Working**
```
Problem: Blank or white screen
Check:
1. LED connected to 3.3V (backlight)
2. VCC connected to 3.3V
3. All SPI pins connected correctly
4. RST not floating (must be connected)
5. Try different USB cable (power issue)
```

### **Touch Not Responding**
```
Problem: Keys don't register
Check:
1. I2C address correct (0x57)
2. Pull-up resistors (usually built-in)
3. Touch pads clean and dry
4. Firmware uploaded correctly
5. Serial Monitor shows I2C communication
```

---

## 💡 **Pro Tips**

### **For Reliable Connections:**
```
✅ Use solid core wire for breadboard
✅ Keep wires short (< 15cm)
✅ Twist I2C wires together (reduce noise)
✅ Use breadboard power rails
✅ Double-check all connections before power-on
```

### **For Clean Wiring:**
```
✅ Use color-coded wires
✅ Route wires neatly
✅ Label wires with tape
✅ Take photos for reference
✅ Document any changes
```

### **For Best Performance:**
```
✅ Keep TTP229 away from display
✅ Use short I2C wires
✅ Ensure good ground connection
✅ Use quality jumper wires
✅ Check for loose connections
```

---

## 📦 **Required Materials**

```
Hardware:
□ ESP32-S3 DevKit (1x)
□ TTP229 16-Key Touch Module (1x)
□ MSP2806 Display 2.8" (1x)
□ Breadboard (1x)
□ Jumper wires M-M (20x)
□ USB-C cable (1x)

Optional:
□ Wire stripper
□ Multimeter
□ Label tape
□ Cable ties
```

---

## 🎯 **Quick Start**

1. **Wire everything** following the diagram above
2. **Double-check** all connections with checklist
3. **Upload test code** `src/ttp229_keypad_test.cpp`
4. **Verify** TTP229 detection in Serial Monitor
5. **Upload gesture code** `src/ttp229_gesture_ipod.cpp`
6. **Try gestures** - swipe, scroll, tap!

That's it! You now have a **complete gesture-controlled iPod** with your TTP229 module! 🎵

The wiring is simple - just **13 wires total** (4 for TTP229, 9 for display), and you're ready to go! 🚀