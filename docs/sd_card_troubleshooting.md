# 🔧 SD Card Module Troubleshooting Guide

## Card Works in Phone But Not in Module

This is a common issue! Here are the most likely causes and solutions:

---

## 🔍 **Quick Checks (Do These First)**

### 1. Card Insertion
```
❌ Common mistake: Card not fully inserted
✅ Solution: Push card in until you hear/feel a click
✅ Card should be flush with module
```

### 2. Wiring Check
```
Double-check every connection:

SD Module Pin → ESP32-S3 Pin
─────────────────────────────
VCC  → 3.3V  (NOT 5V!)
GND  → GND
CS   → GPIO5
MOSI → GPIO11
SCK  → GPIO12
MISO → GPIO13

Common mistakes:
❌ VCC connected to 5V (use 3.3V)
❌ MOSI/MISO swapped
❌ Loose jumper wires
❌ Wrong GPIO numbers
```

### 3. Power Supply
```
SD cards need stable power:
- Minimum: 3.3V @ 200mA
- Check: USB cable quality
- Try: Different USB port/power supply
- Avoid: Long/thin USB cables
```

---

## 🧪 **Diagnostic Steps**

### Step 1: Run Diagnostic Tool
Upload `sd_card_diagnostic.cpp` and check serial output:

```bash
pio run -t upload -t monitor
```

Look for these results:

**✅ Good:**
```
TEST 1: Pin Connections
✅ Pins configured
✅ MISO is HIGH (card present)

TEST 3: SD Card Init
✅ SUCCESS at 400kHz!
Card Type: SDHC
Total Size: 15360 MB
```

**❌ Problem:**
```
TEST 3: SD Card Init
❌ FAILED at 400kHz
```

### Step 2: Check MISO Pin
The MISO pin tells us if the card is responding:

```
MISO HIGH = Card is present and powered
MISO LOW  = Card not detected or wiring issue
```

If MISO is always LOW:
1. Card not inserted properly
2. VCC not connected
3. Card slot damaged
4. Wrong MISO pin

---

## 🔧 **Common Issues & Solutions**

### Issue 1: "SD Card initialization failed"

**Cause A: Wrong SPI Speed**
```cpp
// Try slower speed
SD.begin(SD_CS, SPI, 400000);  // 400kHz (very slow, very safe)

// Instead of:
SD.begin(SD_CS);  // Default 4MHz (might be too fast)
```

**Cause B: Card Format**
```
Your phone might format as exFAT (not supported)
Solution: Reformat as FAT32

Windows:
1. Right-click drive → Format
2. File system: FAT32
3. Allocation size: Default
4. Quick Format: Yes

Mac:
1. Disk Utility
2. Erase
3. Format: MS-DOS (FAT)
4. Scheme: Master Boot Record

Linux:
sudo mkfs.vfat -F 32 /dev/sdX1
```

**Cause C: Shared SPI Bus Conflict**
```cpp
// If display is also on SPI, initialize SD AFTER display
tft.init();        // Display first
delay(100);
SD.begin(SD_CS);   // SD card second
```

**Cause D: Power Issues**
```
SD cards draw 50-200mA during init
- Use good quality USB cable
- Try external power supply
- Add 10µF capacitor across VCC/GND on SD module
```

### Issue 2: "No SD card attached" (CARD_NONE)

**This means SD.begin() succeeded but can't detect card type**

Solutions:
1. **Reseat the card** - Remove and reinsert
2. **Clean contacts** - Use isopropyl alcohol on card contacts
3. **Try different card** - Test with another card
4. **Check card lock** - Some cards have physical lock switch

### Issue 3: Card detected but can't read files

**Cause: File system corruption**
```
Solution:
1. Backup card contents on computer
2. Format as FAT32
3. Copy files back
4. Test again
```

### Issue 4: Works sometimes, fails other times

**Cause: Loose connections or power fluctuations**

Solutions:
1. **Solder connections** instead of jumper wires
2. **Add capacitor** - 10µF across VCC/GND near SD module
3. **Shorter wires** - Keep wires under 10cm
4. **Better power supply** - Use 2A+ USB adapter

---

## 🔬 **Advanced Troubleshooting**

### Test 1: Manual SPI Communication
```cpp
SPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
digitalWrite(SD_CS, LOW);
uint8_t response = SPI.transfer(0xFF);
digitalWrite(SD_CS, HIGH);

Serial.printf("Response: 0x%02X\n", response);
// Should get 0xFF or 0x01, not 0x00
```

### Test 2: Check Card Voltage
```
Use multimeter:
- Measure VCC pin on SD module
- Should be 3.3V ±0.1V
- If lower: power supply issue
- If higher: wrong voltage!
```

### Test 3: Swap Modules
```
If you have another SD module:
1. Try different module with same card
2. Try same module with different card
3. This isolates hardware vs card issue
```

---

## 📊 **Error Code Reference**

| Error | Meaning | Solution |
|-------|---------|----------|
| `SD.begin() returns false` | Init failed | Check wiring, try slow speed |
| `cardType == CARD_NONE` | No card detected | Reseat card, check insertion |
| `cardSize == 0` | Can't read card | Reformat as FAT32 |
| `SD.open() fails` | File system error | Check format, try different card |

---

## 🎯 **Working Configuration**

Once working, you should see:
```
💾 SD Card... ✅
   Type: SDHC
   Size: 15360 MB
   Used: 245 MB / 15360 MB

📂 Scanning for music files...
   🎵 Song1.mp3
   🎵 Song2.mp3
✅ Found 2 music files
```

---

## 🛠️ **Hardware Fixes**

### Fix 1: Add Pull-up Resistor
Some cards need pull-up on MISO:
```
Add 10kΩ resistor between MISO and 3.3V
```

### Fix 2: Add Decoupling Capacitor
Stabilize power:
```
Add 10µF capacitor between VCC and GND
Place as close to SD module as possible
```

### Fix 3: Level Shifter (if using 5V)
If you must use 5V:
```
Add level shifter between ESP32 (3.3V) and SD module
But better to use 3.3V directly!
```

---

## 📝 **Checklist**

Before asking for help, verify:

**Hardware:**
- [ ] Card fully inserted (clicks in)
- [ ] VCC connected to 3.3V (not 5V)
- [ ] All 6 pins connected correctly
- [ ] No loose wires
- [ ] Good power supply (2A+ USB)

**Software:**
- [ ] Correct pin numbers in code
- [ ] SD.begin() called after SPI.begin()
- [ ] Tried slow speed (400kHz)
- [ ] Serial monitor shows error messages

**Card:**
- [ ] Works in phone/computer
- [ ] Formatted as FAT32
- [ ] 32GB or smaller
- [ ] Not write-protected
- [ ] Contacts are clean

---

## 💡 **Pro Tips**

1. **Start slow** - Use 400kHz speed first, increase later
2. **Test simple** - Try empty card first, add files later
3. **Check basics** - 90% of issues are wiring or power
4. **Use diagnostic** - Run `sd_card_diagnostic.cpp` first
5. **Be patient** - SD cards can be finicky, keep trying!

---

## 🆘 **Still Not Working?**

If you've tried everything:

1. **Post diagnostic output** - Share serial monitor output
2. **Photo of wiring** - Show your connections
3. **Card details** - Brand, size, type
4. **Module details** - Photo of SD module
5. **Power supply** - What are you using?

Most SD card issues are solved by:
- ✅ Fixing wiring (80% of cases)
- ✅ Using slower SPI speed (10% of cases)
- ✅ Reformatting card (5% of cases)
- ✅ Replacing defective module (5% of cases)

Good luck! 🍀
