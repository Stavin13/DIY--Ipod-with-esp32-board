# 💾 SD Card - Separate SPI Bus Solution
## No More Conflicts with Display!

---

## 🔧 **The Solution: Use HSPI for SD Card**

ESP32-S3 has **two SPI buses**:
- **SPI (VSPI)** - Used by display (GPIO 10, 11, 12, 13)
- **HSPI** - Use for SD card (different pins!)

This way they don't interfere with each other!

---

## 🔌 **NEW SD Card Wiring**

### Move SD Card to HSPI Pins:

```
SD Card Module → ESP32-S3 (HSPI)
────────────────────────────────
VCC  → 3.3V
GND  → GND
CS   → GPIO15  (was GPIO5)
MOSI → GPIO35  (was GPIO11)
SCK  → GPIO36  (was GPIO12)
MISO → GPIO37  (was GPIO13)
```

### Display Stays the Same (VSPI):

```
Display → ESP32-S3 (VSPI)
─────────────────────────
VCC  → 3.3V
GND  → GND
CS   → GPIO10
RST  → GPIO8
DC   → GPIO9
MOSI → GPIO11
SCK  → GPIO12
MISO → GPIO13
LED  → 3.3V
```

### TTP229 Touch Stays the Same:

```
TTP229 → ESP32-S3
─────────────────
VCC  → 3.3V
GND  → GND
SCL  → GPIO2
SDO  → GPIO1
```

---

## 📊 **Complete Pin Map**

```
ESP32-S3 Pin Usage:
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
GPIO1  - TTP229 SDO (Touch Data)
GPIO2  - TTP229 SCL (Touch Clock)
GPIO8  - Display RST (Reset)
GPIO9  - Display DC (Data/Command)
GPIO10 - Display CS (Chip Select)
GPIO11 - Display MOSI (SPI Data Out) ◄─ VSPI
GPIO12 - Display SCK (SPI Clock)     ◄─ VSPI
GPIO13 - Display MISO (SPI Data In)  ◄─ VSPI
GPIO15 - SD Card CS (Chip Select)
GPIO35 - SD Card MOSI (SPI Data Out) ◄─ HSPI
GPIO36 - SD Card SCK (SPI Clock)     ◄─ HSPI
GPIO37 - SD Card MISO (SPI Data In)  ◄─ HSPI
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Total: 12 GPIO pins used
```

---

## 🎨 **Visual Wiring Diagram**

```
                    ┌─────────────────────────────────┐
                    │       ESP32-S3 DevKit           │
                    │                                 │
    ┌───────────────┤ 3.3V (multiple connections)    │
    │       ┌───────┤ GND  (common ground)            │
    │       │       │                                 │
    │       │       │ GPIO1  ←─────────────────────┐  │
    │       │       │ GPIO2  ←───────────────────┐ │  │
    │       │       │ GPIO8  ─────────────────┐  │ │  │
    │       │       │ GPIO9  ───────────────┐ │  │ │  │
    │       │       │ GPIO10 ─────────────┐ │ │  │ │  │
    │       │       │ GPIO11 ───────────┐ │ │ │  │ │  │
    │       │       │ GPIO12 ─────────┐ │ │ │ │  │ │  │
    │       │       │ GPIO13 ───────┐ │ │ │ │ │  │ │  │
    │       │       │               │ │ │ │ │ │  │ │  │
    │       │       │ GPIO15 ─────┐ │ │ │ │ │ │  │ │  │
    │       │       │ GPIO35 ───┐ │ │ │ │ │ │ │  │ │  │
    │       │       │ GPIO36 ─┐ │ │ │ │ │ │ │ │  │ │  │
    │       │       │ GPIO37 ┐│ │ │ │ │ │ │ │ │  │ │  │
    │       │       └────────┼┼─┼─┼─┼─┼─┼─┼─┼─┼──┼─┼──┘
    │       │                ││ │ │ │ │ │ │ │ │  │ │
    │       │  ┌─────────────┼┼─┼─┘ │ │ │ │ │ │  │ │
    │       │  │  ┌──────────┼┼─┼───┘ │ │ │ │ │  │ │
    │       │  │  │  ┌───────┼┼─┼─────┘ │ │ │ │  │ │
    │       │  │  │  │       ││ │       │ │ │ │  │ │
    ▼       ▼  ▼  ▼  ▼       ▼▼ ▼       ▼ ▼ ▼ ▼  ▼ ▼
┌──────────────────────┐  ┌──────────┐  ┌────────────────┐
│   MSP2806 Display    │  │ SD Card  │  │   TTP229 Touch │
│   (2.8" ILI9341)     │  │  Module  │  │   16-Key Pad   │
├──────────────────────┤  │  HSPI    │  ├────────────────┤
│ VCC  GND  CS  RST DC │  ├──────────┤  │ VCC GND SCL SDO│
│ MOSI SCK MISO LED    │  │VCC GND CS│  └────────────────┘
│      VSPI            │  │MOSI SCK  │
└──────────────────────┘  │MISO      │
                          └──────────┘
```

---

## 💻 **Code Changes**

The code automatically uses HSPI when you specify different pins:

```cpp
// SD Card on HSPI
#define SD_CS   15
#define SD_MOSI 35
#define SD_SCK  36
#define SD_MISO 37

// Initialize HSPI for SD card
SPIClass hspi(HSPI);
hspi.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);

// SD card will use HSPI automatically
SD.begin(SD_CS, hspi);
```

---

## ✅ **Benefits**

1. **No SPI conflicts** - Display and SD card on separate buses
2. **Full speed** - Both can run at maximum speed
3. **Reliable** - No timing issues or bus contention
4. **Simple** - Just rewire 4 wires to different pins

---

## 🔄 **Migration Steps**

### Step 1: Disconnect Old SD Wiring
Remove these connections:
- ❌ CS from GPIO5
- ❌ MOSI from GPIO11
- ❌ SCK from GPIO12
- ❌ MISO from GPIO13

### Step 2: Connect New SD Wiring
Add these connections:
- ✅ CS to GPIO15
- ✅ MOSI to GPIO35
- ✅ SCK to GPIO36
- ✅ MISO to GPIO37
- ✅ VCC to 3.3V (same)
- ✅ GND to GND (same)

### Step 3: Upload New Code
Use `complete_ipod_hspi.cpp` (I'll create this next)

---

## 📝 **Pin Availability Check**

These HSPI pins are safe to use:
- ✅ GPIO15 - Available
- ✅ GPIO35 - Available
- ✅ GPIO36 - Available
- ✅ GPIO37 - Available

None conflict with:
- Display (GPIO 8, 9, 10, 11, 12, 13)
- Touch (GPIO 1, 2)
- USB (GPIO 19, 20)
- Boot pins (GPIO 0)

---

## 🎯 **Expected Result**

After rewiring and uploading new code:

```
╔════════════════════════════════════════╗
║  Complete iPod System - HSPI           ║
║  Display + Touch + SD Working!         ║
╚════════════════════════════════════════╝

🔧 Initializing components...

📺 Display (VSPI)... ✅
👆 TTP229 Touch... ✅
💾 SD Card (HSPI)... ✅
   Type: SDHC
   Size: 15200 MB
   Used: 374 MB / 15200 MB

📂 Scanning for music files...
   🎵 Song1.mp3
   🎵 Song2.mp3
✅ Found 2 music files

✅ System Ready!
```

---

## 🚀 **Ready to Rewire!**

Just move those 4 SD card wires to the new GPIO pins and you're done!

