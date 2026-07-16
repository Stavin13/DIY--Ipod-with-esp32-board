# 🔍 ILI9341 Display SD Slot Identification Guide

## 📌 Does Your Display Have a Built-in SD Slot?

Many ILI9341 displays come in different versions. Let's identify yours!

---

## 🎴 Common ILI9341 Module Types

### Type 1: WITH Built-in SD Slot ✅

**What it looks like:**

```
FRONT VIEW:                    BACK VIEW:
┌─────────────────┐           ┌─────────────────┐
│                 │           │  ┌───────────┐  │
│                 │           │  │ SD SLOT   │  │ ← MicroSD slot here!
│     2.8"        │           │  │  ▼ ▼ ▼    │  │
│    Screen       │           │  └───────────┘  │
│                 │           │                 │
│                 │           │  [Components]   │
└─────────────────┘           └─────────────────┘
  ││││││││││││││                ││││││││││││││
  Pin Headers                   Pin Headers
```

**Pin Layout (WITH SD):**
```
Main Pins:
┌─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┐
│ VCC │ GND │ CS  │ RST │ DC  │MOSI │ SCK │MISO │ LED │SD_CS│
└─────┴─────┴─────┴─────┴─────┴─────┴─────┴─────┴─────┴─────┘
                                                          ↑
                                                    Extra pin!
```

**Key Features:**
- ✅ MicroSD card slot on back
- ✅ Extra pin labeled `SD_CS`, `SDCS`, or `CS_SD`
- ✅ Usually 10 pins total (instead of 9)
- ✅ May have "SD" printed on PCB near slot

---

### Type 2: WITHOUT Built-in SD Slot ❌

**What it looks like:**

```
FRONT VIEW:                    BACK VIEW:
┌─────────────────┐           ┌─────────────────┐
│                 │           │                 │
│                 │           │  [Components]   │ ← No SD slot
│     2.8"        │           │                 │
│    Screen       │           │  [Chips]        │
│                 │           │                 │
│                 │           │  [Resistors]    │
└─────────────────┘           └─────────────────┘
  │││││││││││││                 │││││││││││││
  Pin Headers                   Pin Headers
```

**Pin Layout (NO SD):**
```
Main Pins Only:
┌─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┐
│ VCC │ GND │ CS  │ RST │ DC  │MOSI │ SCK │MISO │ LED │
└─────┴─────┴─────┴─────┴─────┴─────┴─────┴─────┴─────┘
                                                    ↑
                                              Only 9 pins
```

**Key Features:**
- ❌ No MicroSD card slot on back
- ❌ Only 9 pins (no SD_CS)
- ❌ Flat back with just components
- ❌ No "SD" markings

---

## 🔎 How to Check YOUR Display

### Step 1: Count the Pins
```
9 pins  = Probably NO SD slot
10 pins = Probably HAS SD slot
```

### Step 2: Look at the Back
Turn your display over and look for:

**WITH SD Slot:**
```
┌─────────────────────────┐
│  ┌─────────────┐        │
│  │   ▼ ▼ ▼ ▼   │ ← Slot │
│  │ [=========] │        │
│  └─────────────┘        │
│                         │
│  Text: "SD CARD"        │
│  or "TF CARD"           │
└─────────────────────────┘
```

**WITHOUT SD Slot:**
```
┌─────────────────────────┐
│                         │
│  [Chip] [Chip]          │
│                         │
│  [Resistors]            │
│                         │
│  Flat surface           │
└─────────────────────────┘
```

### Step 3: Check Pin Labels
Look at the pin header labels:

**WITH SD:**
```
VCC GND CS RST DC MOSI SCK MISO LED SD_CS
                                     ↑↑↑↑↑
                                   Extra pin!
```

**WITHOUT SD:**
```
VCC GND CS RST DC MOSI SCK MISO LED
                                ↑
                          Ends here
```

---

## 📸 MSP2806 Specific Information

Your **MSP2806** module can come in both versions:

### MSP2806 Version A (WITH SD):
- Model: MSP2806-SD or MSP2806-TF
- Has microSD slot on back
- 10-pin header
- SD_CS usually on GPIO4

### MSP2806 Version B (NO SD):
- Model: MSP2806 (basic)
- No SD slot
- 9-pin header
- Need separate SD module

---

## 🎯 Quick Identification Checklist

Check these in order:

1. **Count pins:**
   - [ ] 9 pins → Probably no SD
   - [ ] 10 pins → Probably has SD

2. **Look at back:**
   - [ ] See SD slot → Has SD!
   - [ ] No slot → No SD

3. **Check labels:**
   - [ ] See "SD_CS" pin → Has SD!
   - [ ] No SD_CS pin → No SD

4. **Look for text:**
   - [ ] "SD CARD" or "TF CARD" printed → Has SD!
   - [ ] No SD text → No SD

---

## 🔌 If You HAVE Built-in SD:

### Wiring:
```
Display SD_CS → ESP32 GPIO4
(All other pins already connected)
```

### Test:
```
Upload: src/ili9341_builtin_sd_test.cpp
```

### Benefits:
- ✅ No SPI conflicts
- ✅ Simpler wiring
- ✅ More reliable
- ✅ Saves space

---

## ❌ If You DON'T HAVE Built-in SD:

### Option 1: Demo Mode (Recommended)
```
Upload: src/ipod_no_sd.cpp
- 9 demo songs
- Full UI
- No SD needed
```

### Option 2: Separate SD Module (Not Recommended)
- Has SPI conflicts
- Less reliable
- More wiring
- Better to wait for PCB design

### Option 3: Wait for PCB
- Design proper SD support
- Use SDMMC interface
- Professional solution

---

## 📷 Photo Examples

### What to Photograph:

**Photo 1: Back of Display**
- Show the entire back
- Include any SD slot
- Show all components

**Photo 2: Pin Headers**
- Close-up of pin labels
- Show all pin names clearly
- Include any SD_CS pin

**Photo 3: Side View**
- Show if there's an SD slot opening
- Show thickness of module

---

## 🆘 Still Not Sure?

### Try the Test Anyway!

Even if you're not sure, run the test:
```
src/ili9341_builtin_sd_test.cpp
```

The test will:
1. Try GPIO4 (most common)
2. Try GPIO5 (alternative)
3. Try GPIO14 (some modules)
4. Tell you if SD slot exists

**No harm in trying!** The test is safe and won't damage anything.

---

## 💡 Pro Tips

1. **Most ILI9341 modules DON'T have SD slots**
   - It's an optional feature
   - More common on expensive modules

2. **SD slot is usually on the BACK**
   - Never on the front (screen side)
   - Sometimes on the side edge

3. **"TF Card" = MicroSD Card**
   - Same thing, different name
   - TF = TransFlash (old name for microSD)

4. **Demo mode is perfectly fine**
   - Don't wait for SD to continue
   - `ipod_no_sd.cpp` works great!

5. **PCB design is the real solution**
   - Breadboard SD is always tricky
   - PCB will be much more reliable

---

## 🎯 Decision Tree

```
Do you see an SD slot on the back?
│
├─ YES → Connect SD_CS to GPIO4
│         Run ili9341_builtin_sd_test.cpp
│         │
│         ├─ Works? → Use complete_ipod_system.cpp
│         └─ Fails? → Check wiring, try GPIO5
│
└─ NO → Use ipod_no_sd.cpp (demo mode)
         Continue development
         Plan for PCB design
```

---

## 🔗 Next Steps

1. ✅ Identify your display type (WITH or WITHOUT SD)
2. ✅ If WITH SD: Run `ili9341_builtin_sd_test.cpp`
3. ✅ If WITHOUT SD: Use `ipod_no_sd.cpp`
4. ✅ Continue with UI and feature development
5. ✅ Plan PCB design for final product

---

**Take a close look at your display now!** 🔍

Does it have an SD slot? Let's find out! 🎴
