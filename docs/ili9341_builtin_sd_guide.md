# 🎴 ILI9341 Built-in SD Card Slot Guide

## 📌 What is This?

Many ILI9341 display modules (including MSP2806) have a **built-in microSD card slot** on the back. This is much better than using a separate SD module because:

✅ **Shares the same SPI bus** - No conflicts!  
✅ **Fewer wires** - Just one extra CS pin  
✅ **More reliable** - Designed to work together  
✅ **Saves space** - No separate module needed  

---

## 🔍 Step 1: Check Your Display

### Look at the Back of Your MSP2806 Display

```
Front (Screen Side):          Back (Component Side):
┌─────────────────┐          ┌─────────────────┐
│                 │          │  [SD Card Slot] │ ← Look for this!
│                 │          │   ┌─────────┐   │
│     Screen      │          │   │ ▼ ▼ ▼ ▼ │   │
│                 │          │   └─────────┘   │
│                 │          │                 │
└─────────────────┘          │  Pin Headers    │
  │││││││││││││││            │  │││││││││││││  │
  └─────────────┘            └─────────────────┘
```

### What to Look For:

1. **MicroSD Card Slot** - Small slot that fits a microSD card
2. **SD_CS Pin** - Extra pin labeled one of these:
   - `SD_CS`
   - `SDCS`
   - `CS_SD`
   - `SD-CS`
   - Sometimes just `CS` (separate from TFT_CS)

### Common ILI9341 Module Types:

**Type A: Separate SD_CS Pin**
```
Display Pins:
VCC  GND  CS  RST  DC  MOSI  SCK  MISO  LED  [SD_CS]
                                                 ↑
                                        Extra pin for SD!
```

**Type B: SD_CS on Pin Header**
```
Main Pins:          Extra Pins:
VCC  GND  CS        SD_CS
RST  DC   MOSI      (or SDCS)
SCK  MISO LED
```

**Type C: No Built-in SD**
```
Only these pins:
VCC  GND  CS  RST  DC  MOSI  SCK  MISO  LED
(No SD slot on back)
```

---

## 🔌 Step 2: Wiring

### If Your Display HAS Built-in SD Slot:

**Current Wiring (Already Connected):**
```
Display Pin    ESP32-S3 Pin
-----------    ------------
VCC            3.3V
GND            GND
CS             GPIO10
RST            GPIO8
DC             GPIO9
MOSI           GPIO11
SCK            GPIO12
MISO           GPIO13
LED            3.3V
```

**Add ONE More Wire:**
```
SD_CS          GPIO4  ← Add this!
```

### Visual Diagram:

```
ESP32-S3                    MSP2806 Display (Back View)
┌──────────┐               ┌─────────────────────────┐
│          │               │   [MicroSD Card Slot]   │
│  GPIO4 ──┼───────────────┼→ SD_CS pin              │
│  GPIO8 ──┼───────────────┼→ RST                    │
│  GPIO9 ──┼───────────────┼→ DC                     │
│  GPIO10 ─┼───────────────┼→ CS (TFT)               │
│  GPIO11 ─┼───────────────┼→ MOSI (shared)          │
│  GPIO12 ─┼───────────────┼→ SCK (shared)           │
│  GPIO13 ─┼───────────────┼→ MISO (shared)          │
│  3.3V ───┼───────────────┼→ VCC                    │
│  GND ────┼───────────────┼→ GND                    │
└──────────┘               └─────────────────────────┘
```

---

## 🧪 Step 3: Run the Test

### Upload the Test Code

1. Open `src/ili9341_builtin_sd_test.cpp`
2. Upload to your ESP32-S3
3. Open Serial Monitor (115200 baud)

### What the Test Does:

```
1. Tries GPIO4 first (most common)
   ├─ If works → Success! Use GPIO4
   └─ If fails → Try GPIO5

2. Tries GPIO5 (alternative)
   ├─ If works → Success! Use GPIO5
   └─ If fails → No built-in SD slot
```

### Expected Output (Success):

```
╔════════════════════════════════════════╗
║   ILI9341 Built-in SD Card Test       ║
╚════════════════════════════════════════╝

📌 Testing SD card slot on display module
   SD_CS: GPIO4 (trying first)
   Shared SPI with display

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
TEST 1: SD Card on GPIO4
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Trying CS pin: GPIO4
Initializing... ✅ Detected!
📇 Type: SDHC
💾 Size: 15200 MB
📊 Used: 374 MB / 15200 MB
📝 Testing write... ✅
📖 Testing read... ILI9341 SD Test
✅

✅ SUCCESS! SD card works on GPIO4
```

### Expected Output (No Built-in SD):

```
❌ SD card not detected on GPIO4 or GPIO5

🔍 Possible issues:
   1. Your ILI9341 module doesn't have SD slot
   2. SD card not inserted in display module
   3. SD_CS pin is different (check module label)
   4. Need to connect SD_CS wire to ESP32
```

---

## 📋 Troubleshooting

### Problem: "SD card not detected"

**Check 1: Is there an SD slot?**
- Look at the back of the display
- Some ILI9341 modules don't have SD slots

**Check 2: Is the SD card inserted?**
- Insert into the **display module**, not a separate SD module
- Push until it clicks

**Check 3: Is SD_CS connected?**
- Find the SD_CS pin on your display
- Connect it to GPIO4 on ESP32

**Check 4: Try different CS pins**
- Some modules use GPIO5 instead of GPIO4
- The test tries both automatically

**Check 5: SD card format**
- Must be FAT32 (not exFAT)
- 32GB or smaller (SDHC)

---

## ✅ If Built-in SD Works

### Update Your Complete System Code

Change the SD_CS pin in your code:

```cpp
// OLD (separate SD module):
#define SD_CS 5

// NEW (built-in SD on display):
#define SD_CS 4  // or GPIO5 if that's what worked
```

### Benefits:

✅ **No SPI conflicts** - Display and SD designed to work together  
✅ **Simpler wiring** - Just one extra wire  
✅ **More reliable** - Tested by manufacturer  
✅ **Saves space** - No separate SD module needed  

---

## ❌ If No Built-in SD Slot

### Option 1: Use Demo Mode (Recommended for Now)

Use `src/ipod_no_sd.cpp` which has:
- 9 demo songs built-in
- Full UI working
- No SD card needed
- Perfect for testing and development

### Option 2: Wait for PCB Design

When you design the PCB, you can:
- Use a dedicated SPI bus for SD card
- Add proper level shifters
- Design for reliability

### Option 3: Try SDMMC Mode (Advanced)

ESP32-S3 has a 4-bit SDMMC interface that's faster and more reliable than SPI. This requires different wiring and is better suited for PCB design.

---

## 🎯 Next Steps

### If Built-in SD Works:

1. ✅ Run `ili9341_builtin_sd_test.cpp`
2. ✅ Note which GPIO works (4 or 5)
3. ✅ Update `complete_ipod_system.cpp` with correct SD_CS pin
4. ✅ Test complete system with real music files
5. ✅ Add audio playback (I2S DAC)

### If No Built-in SD:

1. ✅ Use `ipod_no_sd.cpp` for now (demo mode)
2. ✅ Continue developing UI and features
3. ✅ Plan for PCB design with proper SD support
4. ✅ Consider SDMMC interface for final design

---

## 📸 Photo Guide

### What to Photograph:

1. **Back of your display** - Show the SD slot (if present)
2. **Pin labels** - Show all pin names clearly
3. **SD_CS pin** - Close-up of the SD_CS pin location

This will help identify your exact module type!

---

## 💡 Pro Tips

1. **Built-in SD is better** - Always use it if available
2. **GPIO4 is most common** - Try this first
3. **Check both sides** - Some modules have pins on both sides
4. **Read the labels** - Pin names are usually printed on PCB
5. **Demo mode works great** - Don't wait for SD to continue development

---

## 🔗 Related Files

- `src/ili9341_builtin_sd_test.cpp` - Test code (run this first!)
- `src/ipod_no_sd.cpp` - Demo mode without SD
- `src/complete_ipod_system.cpp` - Full system (needs SD_CS update)
- `docs/sd_card_troubleshooting.md` - General SD troubleshooting

---

**Ready to test? Upload `ili9341_builtin_sd_test.cpp` and let's see if your display has a built-in SD slot!** 🎴
