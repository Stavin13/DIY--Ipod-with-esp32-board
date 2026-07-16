# 🎴 Quick SD Card Test - ILI9341 Built-in Slot

## 🎯 Goal
Test if your MSP2806 (ILI9341) display has a built-in SD card slot.

---

## ⚡ Quick Steps

### 1️⃣ Check Your Display
Look at the **BACK** of your display for:
- 📍 MicroSD card slot
- 📍 Pin labeled `SD_CS` or `SDCS`

### 2️⃣ Insert SD Card
- Put your 16GB microSD card into the **display module** (not a separate SD module)
- Make sure it's FAT32 formatted

### 3️⃣ Connect SD_CS Wire
```
Display SD_CS pin → ESP32 GPIO4
```

### 4️⃣ Upload Test
- Open: `src/ili9341_builtin_sd_test.cpp`
- Upload to ESP32-S3
- Open Serial Monitor (115200 baud)

---

## ✅ Success Looks Like:
```
✅ SUCCESS! SD card works on GPIO4
📇 Type: SDHC
💾 Size: 15200 MB
```

## ❌ Failure Looks Like:
```
❌ SD card not detected on GPIO4, GPIO5, or GPIO14
```

---

## 🔀 What's Next?

### If SD Works:
1. Note which GPIO worked (4, 5, or 14)
2. Update `complete_ipod_system.cpp`:
   ```cpp
   #define SD_CS 4  // Use the GPIO that worked
   ```
3. Test complete system with music files!

### If SD Doesn't Work:
1. Your display probably doesn't have a built-in SD slot
2. Use `src/ipod_no_sd.cpp` instead (demo mode)
3. Has 9 demo songs, full UI, works perfectly!
4. Plan for PCB design with proper SD support later

---

## 📸 Need Help?
Take a photo of the **back of your display** showing:
- The SD card slot (if present)
- All pin labels
- Any text/model numbers

---

## 🚀 Demo Mode (No SD Needed)
If you want to continue without SD card:
```
Upload: src/ipod_no_sd.cpp
- 9 demo songs built-in
- Full iPod UI
- Touch controls working
- No SD card needed!
```

---

**Ready? Upload `ili9341_builtin_sd_test.cpp` now!** 🎴
