# 🔌 Complete ESP32-S3 iPod System Wiring Guide
## ESP32-S3 + TFT Display + SD Card + 16-Key Keypad

---

## 📦 **Components Required**

1. **ESP32-S3 DevKit** (main controller)
2. **MSP2806 Display** (2.8" ILI9341 TFT with touch)
3. **SD Card Module** (separate SPI module)
4. **TTP229 16-Key Touch Keypad** (capacitive touch)
5. **Breadboard** (for prototyping)
6. **Jumper Wires** (approximately 25 wires)
7. **USB-C Cable** (for power and programming)

---

## 🎯 **Quick Pin Summary**

```
╔═══════════════════════════════════════════════════════════════╗
║  Component        Pin Name    ESP32-S3 GPIO    Function       ║
╠═══════════════════════════════════════════════════════════════╣
║  TFT Display      CS          GPIO10           Chip Select    ║
║  (VSPI Bus)       DC          GPIO9            Data/Command   ║
║                   RST         GPIO8            Reset          ║
║                   MOSI        GPIO11           SPI Data Out   ║
║                   SCK         GPIO12           SPI Clock      ║
║                   MISO        GPIO13           SPI Data In    ║
║                   LED         3.3V             Backlight      ║
║                   VCC         3.3V             Power          ║
║                   GND         GND              Ground         ║
╠═══════════════════════════════════════════════════════════════╣
║  SD Card Module   CS          GPIO15           Chip Select    ║
║  (HSPI Bus)       MOSI        GPIO35           SPI Data Out   ║
║                   SCK         GPIO36           SPI Clock      ║
║                   MISO        GPIO37           SPI Data In    ║
║                   VCC         3.3V             Power          ║
║                   GND         GND              Ground         ║
╠═══════════════════════════════════════════════════════════════╣
║  TTP229 Keypad    SCL         GPIO2            I2C Clock      ║
║  (I2C Bus)        SDO         GPIO1            I2C Data       ║
║                   VCC         3.3V             Power          ║
║                   GND         GND              Ground         ║
╚═══════════════════════════════════════════════════════════════╝

Total GPIO Pins Used: 12
Total Wires: 25 (including power/ground)
```

---

## 🔌 **Detailed Connection Tables**

### **1. TFT Display (MSP2806) - 9 Wires**

```
Display Pin    →    ESP32-S3 Pin    Wire Color    Notes
───────────────────────────────────────────────────────────────
VCC            →    3.3V            Red           Power supply
GND            →    GND             Black         Ground
CS             →    GPIO10          Orange        Chip Select
RST            →    GPIO8           White         Reset (required!)
DC             →    GPIO9           Gray          Data/Command
SDA/MOSI       →    GPIO11          Blue          SPI Data Out
SCL/SCLK       →    GPIO12          Purple        SPI Clock
SDO/MISO       →    GPIO13          Brown         SPI Data In
LED            →    3.3V            Red           Backlight (always on)
```

**Important Notes:**
- Uses default VSPI bus (SPI2)
- RST pin MUST be connected (not optional)
- LED to 3.3V for full brightness
- MISO is optional but recommended

---

### **2. SD Card Module - 6 Wires**

```
SD Card Pin    →    ESP32-S3 Pin    Wire Color    Notes
───────────────────────────────────────────────────────────────
VCC            →    3.3V            Red           3.3V only!
GND            →    GND             Black         Ground
CS             →    GPIO15          Yellow        Chip Select
MOSI           →    GPIO35          Green         SPI Data Out
SCK            →    GPIO36          Blue          SPI Clock
MISO           →    GPIO37          White         SPI Data In
```

**Important Notes:**
- Uses HSPI bus (SPI3) - separate from display!
- Must use 3.3V (NOT 5V!)
- Different pins than display to avoid conflicts
- All 6 wires required for SD card operation

---

### **3. TTP229 16-Key Keypad - 4 Wires**

```
TTP229 Pin     →    ESP32-S3 Pin    Wire Color    Notes
───────────────────────────────────────────────────────────────
VCC            →    3.3V            Red           Power supply
GND            →    GND             Black         Ground
SCL            →    GPIO2           Yellow        I2C Clock
SDO            →    GPIO1           Green         I2C Data (SDA)
```

**Important Notes:**
- Uses I2C bus (Wire)
- I2C address: 0x57
- GPIO1/GPIO2 are standard I2C pins for ESP32-S3
- Built-in pull-up resistors (no external needed)

---

## 🎨 **Visual Wiring Diagram**

```
                         ESP32-S3 DevKit
                         
                           [USB-C Port]
              ┌────────────────────────────────────┐
              │                                    │
Power Rails:  │  3.3V ●─────────────────────┐      │
              │                             │      │
              │  GND  ●───────────────┐     │      │
              │                       │     │      │
I2C (Keypad): │  IO1  ●─────────┐     │     │      │
              │  IO2  ●───────┐ │     │     │      │
              │               │ │     │     │      │
Display:      │  IO8  ●─────┐ │ │     │     │      │
              │  IO9  ●───┐ │ │ │     │     │      │
              │  IO10 ●─┐ │ │ │ │     │     │      │
              │  IO11 ●─│─│─│─│─│─────│─────│───┐  │
              │  IO12 ●─│─│─│─│─│─────│─────│─┐ │  │
              │  IO13 ●─│─│─│─│─│─────│─────│─│─│─┐│
              │         │ │ │ │ │     │     │ │ │ ││
SD Card:      │  IO15 ●─│─│─│─│─│─────│─────│─│─│─││
              │  IO35 ●─│─│─│─│─│─────│─────│─│─│─││
              │  IO36 ●─│─│─│─│─│─────│─────│─│─│─││
              │  IO37 ●─│─│─│─│─│─────│─────│─│─│─││
              │         │ │ │ │ │     │     │ │ │ ││
              └─────────┼─┼─┼─┼─┼─────┼─────┼─┼─┼─┼┼
                        │ │ │ │ │     │     │ │ │ ││
                        │ │ │ │ │     │     │ │ │ ││
        ┌───────────────┘ │ │ │ │     │     │ │ │ ││
        │   ┌─────────────┘ │ │ │     │     │ │ │ ││
        │   │               │ │ │     │     │ │ │ ││
        ▼   ▼               ▼ ▼ ▼     ▼     ▼ ▼ ▼ ▼▼
    ┌─────────────┐   ┌──────────┐   ┌────────────────┐
    │  TTP229     │   │ MSP2806  │   │   SD Card      │
    │  16-Key     │   │ Display  │   │   Module       │
    │  Keypad     │   │ 2.8" TFT │   │                │
    ├─────────────┤   ├──────────┤   ├────────────────┤
    │ VCC  GND    │   │ VCC  GND │   │ VCC  GND       │
    │ SCL  SDO    │   │ CS   RST │   │ CS   MOSI      │
    │             │   │ DC   MOSI│   │ SCK  MISO      │
    │ [1][2][3][4]│   │ SCK  MISO│   │                │
    │ [5][6][7][8]│   │ LED      │   │                │
    │ [9][10][11] │   │          │   │                │
    │ [13][14][15]│   │          │   │                │
    └─────────────┘   └──────────┘   └────────────────┘
         I2C Bus         VSPI Bus         HSPI Bus
```

---

## 📐 **Breadboard Layout**

```
                    Breadboard Top View
                    
    Power Rails (Left Side):
    [+] ←─── ESP32-S3 3.3V (Red wire)
    [-] ←─── ESP32-S3 GND (Black wire)
    
    
    Component Placement:
    
    ┌────────────────────────────────────────────────────┐
    │                                                    │
    │  [+] Power Rail                                    │
    │  [-] Ground Rail                                   │
    │                                                    │
    │  ┌──────────┐         ┌──────────┐                │
    │  │ TTP229   │         │ SD Card  │                │
    │  │ Keypad   │         │ Module   │                │
    │  └──────────┘         └──────────┘                │
    │                                                    │
    │                                                    │
    │  ┌────────────────────────────────┐               │
    │  │      ESP32-S3 DevKit           │               │
    │  │         [USB-C]                │               │
    │  └────────────────────────────────┘               │
    │                                                    │
    │                                                    │
    │  ┌────────────────────────────────┐               │
    │  │     MSP2806 Display            │               │
    │  │     (2.8" TFT Screen)          │               │
    │  └────────────────────────────────┘               │
    │                                                    │
    └────────────────────────────────────────────────────┘
    
    Wire Routing Tips:
    - Keep I2C wires (keypad) short and twisted together
    - Route display wires on one side
    - Route SD card wires on opposite side
    - Use breadboard power rails for all VCC/GND connections
```

---

## 🔧 **Step-by-Step Wiring Instructions**

### **Phase 1: Power Distribution (5 minutes)**

1. **Connect ESP32-S3 to breadboard power rails:**
   ```
   ESP32-S3 3.3V → Breadboard + rail (Red wire)
   ESP32-S3 GND  → Breadboard - rail (Black wire)
   ```

2. **Verify power rails:**
   - Use multimeter to check 3.3V on + rail
   - Check continuity on - rail

---

### **Phase 2: TFT Display (10 minutes)**

3. **Connect display power:**
   ```
   Display VCC → Breadboard + rail (Red)
   Display GND → Breadboard - rail (Black)
   Display LED → Breadboard + rail (Red) - backlight
   ```

4. **Connect display SPI signals:**
   ```
   Display CS   → ESP32-S3 GPIO10 (Orange)
   Display RST  → ESP32-S3 GPIO8  (White)
   Display DC   → ESP32-S3 GPIO9  (Gray)
   Display MOSI → ESP32-S3 GPIO11 (Blue)
   Display SCK  → ESP32-S3 GPIO12 (Purple)
   Display MISO → ESP32-S3 GPIO13 (Brown)
   ```

5. **Test display:**
   - Power on ESP32-S3
   - Display backlight should illuminate
   - If no light, check LED connection

---

### **Phase 3: SD Card Module (5 minutes)**

6. **Connect SD card power:**
   ```
   SD VCC → Breadboard + rail (Red)
   SD GND → Breadboard - rail (Black)
   ```

7. **Connect SD card SPI signals (HSPI bus):**
   ```
   SD CS   → ESP32-S3 GPIO15 (Yellow)
   SD MOSI → ESP32-S3 GPIO35 (Green)
   SD SCK  → ESP32-S3 GPIO36 (Blue)
   SD MISO → ESP32-S3 GPIO37 (White)
   ```

8. **Insert SD card:**
   - Format as FAT32
   - Insert into module
   - Ensure card clicks into place

---

### **Phase 4: TTP229 Keypad (5 minutes)**

9. **Connect keypad power:**
   ```
   TTP229 VCC → Breadboard + rail (Red)
   TTP229 GND → Breadboard - rail (Black)
   ```

10. **Connect keypad I2C signals:**
    ```
    TTP229 SCL → ESP32-S3 GPIO2 (Yellow)
    TTP229 SDO → ESP32-S3 GPIO1 (Green)
    ```

11. **Verify keypad:**
    - Check for power LED (if present)
    - Touch pads should be clean and dry

---

## ✅ **Connection Verification Checklist**

### **Before Powering On:**

**Power Connections:**
- [ ] ESP32-S3 3.3V connected to breadboard + rail
- [ ] ESP32-S3 GND connected to breadboard - rail
- [ ] All component VCC pins connected to + rail
- [ ] All component GND pins connected to - rail
- [ ] No short circuits between + and - rails

**TFT Display (9 wires):**
- [ ] VCC → 3.3V
- [ ] GND → GND
- [ ] LED → 3.3V (backlight)
- [ ] CS → GPIO10
- [ ] RST → GPIO8
- [ ] DC → GPIO9
- [ ] MOSI → GPIO11
- [ ] SCK → GPIO12
- [ ] MISO → GPIO13

**SD Card Module (6 wires):**
- [ ] VCC → 3.3V (NOT 5V!)
- [ ] GND → GND
- [ ] CS → GPIO15
- [ ] MOSI → GPIO35
- [ ] SCK → GPIO36
- [ ] MISO → GPIO37
- [ ] SD card inserted and formatted (FAT32)

**TTP229 Keypad (4 wires):**
- [ ] VCC → 3.3V
- [ ] GND → GND
- [ ] SCL → GPIO2
- [ ] SDO → GPIO1

**Final Checks:**
- [ ] All connections secure (no loose wires)
- [ ] Correct GPIO pins used
- [ ] No crossed wires
- [ ] Multimeter check: 3.3V on power rails
- [ ] Visual inspection complete

---

## 🧪 **Testing Procedure**

### **Test 1: Power Test**
```
1. Connect USB-C to ESP32-S3
2. Check display backlight (should glow)
3. Check keypad power LED (if equipped)
4. Measure voltage on breadboard rails (should be 3.3V)
```

### **Test 2: Display Test**
```
Upload: src/tft_espi_test.cpp

Expected Output:
- Display shows color bars
- Text appears clearly
- No flickering or artifacts

If blank: Check RST, DC, CS connections
If white: Check MOSI, SCK connections
```

### **Test 3: SD Card Test**
```
Upload: src/hspi_sd_test.cpp

Expected Serial Output:
✅ SD Card initialized
   Type: SDHC
   Size: XXXXX MB
   Files found: X

If fails: Check CS, MOSI, SCK, MISO connections
If "Card Mount Failed": Check power and card format
```

### **Test 4: Keypad Test**
```
Upload: src/ttp229_simple_test.cpp

Expected Serial Output:
✅ TTP229 module found at 0x57
✅ I2C initialized on GPIO1/GPIO2

Touch any key:
👆 Key X pressed

If not found: Check SCL/SDO connections
If wrong keys: Verify I2C address (0x57)
```

### **Test 5: Complete System Test**
```
Upload: src/complete_ipod_hspi.cpp

Expected Result:
- Display shows iPod interface
- Touch keys to navigate
- SD card music files listed
- All components working together

This confirms everything is wired correctly!
```

---

## 🚨 **Troubleshooting Guide**

### **Display Issues**

**Problem: Display backlight off**
```
Check:
1. LED pin connected to 3.3V
2. VCC connected to 3.3V
3. GND connected properly
4. Try different USB cable (power issue)
```

**Problem: Display white/blank**
```
Check:
1. RST pin connected to GPIO8 (critical!)
2. DC pin connected to GPIO9
3. CS pin connected to GPIO10
4. MOSI/SCK connections
5. Upload correct test code
```

**Problem: Display garbled/artifacts**
```
Check:
1. Loose connections on SPI pins
2. Wires too long (keep < 15cm)
3. Power supply stable
4. Correct pin definitions in code
```

---

### **SD Card Issues**

**Problem: "Card Mount Failed"**
```
Check:
1. SD card formatted as FAT32
2. Card inserted correctly (clicks in)
3. VCC is 3.3V (NOT 5V!)
4. All 6 wires connected
5. CS pin on GPIO15
```

**Problem: "Card Not Found"**
```
Check:
1. HSPI pins correct (35, 36, 37)
2. Not using display SPI pins (11, 12, 13)
3. CS pin not floating
4. Card module has power
5. Try different SD card
```

**Problem: Files not reading**
```
Check:
1. Card formatted as FAT32 (not exFAT)
2. Files in root directory
3. Filenames 8.3 format or long name support
4. Card not corrupted
5. Sufficient power supply
```

---

### **Keypad Issues**

**Problem: "TTP229 not found"**
```
Check:
1. VCC connected to 3.3V (NOT 5V!)
2. GND connected properly
3. SCL on GPIO2 (not GPIO1)
4. SDO on GPIO1 (not GPIO2)
5. I2C address is 0x57
```

**Problem: Keys not responding**
```
Check:
1. Touch pads clean and dry
2. Firm touch required
3. Module has power LED on
4. I2C communication working (check serial)
5. No interference from other devices
```

**Problem: Wrong keys detected**
```
Check:
1. I2C address correct (0x57)
2. Module in correct mode (16-key)
3. Jumpers on module (if present)
4. Code key mapping correct
5. Serial output for actual key numbers
```

---

### **General Issues**

**Problem: ESP32-S3 won't boot**
```
Check:
1. No short circuits
2. Power supply adequate (500mA+)
3. USB cable good quality
4. No pins shorted to ground
5. Try without peripherals connected
```

**Problem: Intermittent failures**
```
Check:
1. All connections secure
2. Breadboard contacts clean
3. Wires not damaged
4. Power supply stable
5. No loose jumper wires
```

**Problem: Multiple components not working**
```
Check:
1. Power rails have 3.3V
2. Ground connections solid
3. No pin conflicts
4. Correct code uploaded
5. Serial monitor for error messages
```

---

## 💡 **Pro Tips**

### **For Reliable Connections:**
```
✅ Use solid-core wire for breadboard (22-24 AWG)
✅ Keep all wires under 15cm length
✅ Twist I2C wires together (reduces noise)
✅ Use breadboard power rails for distribution
✅ Double-check every connection before power-on
✅ Take photos of your wiring for reference
```

### **For Clean Wiring:**
```
✅ Use color-coded wires consistently
✅ Route wires neatly along breadboard edges
✅ Label wires with tape if needed
✅ Keep similar signals grouped together
✅ Avoid crossing wires when possible
```

### **For Best Performance:**
```
✅ Use quality jumper wires (not cheap ones)
✅ Ensure good breadboard contact
✅ Keep high-speed signals (SPI) short
✅ Separate power wires from signal wires
✅ Use common ground point
✅ Check connections with multimeter
```

### **For Easy Debugging:**
```
✅ Test one component at a time
✅ Use serial monitor for diagnostics
✅ Keep test code simple initially
✅ Document any wiring changes
✅ Mark working configurations
```

---

## 📊 **Bus Architecture**

```
ESP32-S3 Bus Usage:
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

I2C Bus (Wire):
  GPIO1  - SDA (TTP229 SDO)
  GPIO2  - SCL (TTP229 SCL)
  Speed: 100kHz (standard mode)
  Devices: TTP229 Keypad (0x57)

VSPI Bus (SPI2) - Default SPI:
  GPIO11 - MOSI (Display SDA)
  GPIO12 - SCK  (Display SCL)
  GPIO13 - MISO (Display SDO)
  GPIO10 - CS   (Display CS)
  Speed: 40MHz
  Devices: ILI9341 Display

HSPI Bus (SPI3) - Secondary SPI:
  GPIO35 - MOSI (SD Card MOSI)
  GPIO36 - SCK  (SD Card SCK)
  GPIO37 - MISO (SD Card MISO)
  GPIO15 - CS   (SD Card CS)
  Speed: 20MHz
  Devices: SD Card Module

Additional Pins:
  GPIO8  - Display Reset
  GPIO9  - Display Data/Command

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

Why This Configuration?
✅ No pin conflicts between components
✅ Separate SPI buses prevent interference
✅ Standard I2C pins for keypad
✅ All pins available on ESP32-S3
✅ Optimal performance for each bus
```

---

## 📦 **Bill of Materials**

### **Required Components:**
```
□ ESP32-S3 DevKit (1x)                    ~$10-15
□ MSP2806 Display 2.8" ILI9341 (1x)       ~$8-12
□ SD Card Module (SPI interface) (1x)     ~$2-5
□ TTP229 16-Key Touch Keypad (1x)         ~$5-8
□ Breadboard 830 points (1x)              ~$3-5
□ Jumper wires M-M 20cm (30x)             ~$3-5
□ USB-C cable (1x)                        ~$3-5
□ MicroSD card 8-32GB (1x)                ~$5-10

Total Cost: ~$40-65
```

### **Optional Tools:**
```
□ Multimeter (for testing)                ~$15-30
□ Wire stripper/cutter                    ~$10-15
□ Label maker or tape                     ~$5-10
□ Helping hands/PCB holder                ~$10-20
□ Soldering iron (for permanent build)    ~$20-40
```

---

## 🎯 **Quick Start Summary**

1. **Gather all components** and verify you have everything
2. **Set up breadboard** with power rails connected
3. **Wire display first** (9 wires) and test
4. **Wire SD card** (6 wires) and test
5. **Wire keypad** (4 wires) and test
6. **Upload complete system code** and enjoy!

Total wiring time: ~30 minutes
Total testing time: ~15 minutes

---

## 🚀 **Next Steps**

After successful wiring:

1. **Upload complete system:** `src/complete_ipod_hspi.cpp`
2. **Add music files** to SD card (MP3 format)
3. **Test all features:** navigation, playback, gestures
4. **Design enclosure:** See `docs/case_design.md`
5. **Create PCB:** See `docs/pcb_design_guide.md`

---

## 📝 **Configuration File**

Your `src/config.h` should match these pin assignments:

```cpp
// Display - VSPI Bus
#define TFT_CS 10
#define TFT_DC 9
#define TFT_RST 8
#define TFT_MOSI 11
#define TFT_SCLK 12
#define TFT_MISO 13

// SD Card - HSPI Bus
#define SD_CS 15
#define SD_MOSI 35
#define SD_SCK 36
#define SD_MISO 37

// TTP229 Keypad - I2C Bus
#define I2C_SDA 1
#define I2C_SCL 2
#define TTP229_ADDR 0x57
```

---

## ✨ **You're All Set!**

With these connections, you have a complete iPod system:
- ✅ 2.8" color touchscreen display
- ✅ SD card for music storage
- ✅ 16-key capacitive touch keypad
- ✅ Gesture recognition
- ✅ Full iPod interface

Total: **25 wires, 12 GPIO pins, 3 buses**

Now go build something awesome! 🎵🚀
