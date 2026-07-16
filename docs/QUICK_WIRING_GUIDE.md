# ⚡ QUICK WIRING GUIDE - ESP32-S3 iPod

## 🔌 **Super Simple Connections**

### **You Need:**
- ESP32-S3 DevKit
- TTP229 16-Key Touch Module  
- MSP2806 Display (2.8" ILI9341)
- 13 jumper wires

---

## 📋 **TTP229 Touch Module (4 wires)**

```
TTP229 Pin    →    ESP32-S3 Pin    Wire Color
----------         ------------    ----------
VCC           →    3.3V            Red
GND           →    GND             Black
SCL           →    GPIO2           Yellow
SDO           →    GPIO1           Green
```

**Important:** GPIO1 and GPIO2 are the correct I2C pins for ESP32-S3!

---

## 📋 **MSP2806 Display (9 wires)**

```
Display Pin   →    ESP32-S3 Pin    Wire Color
-----------        ------------    ----------
VCC           →    3.3V            Red
GND           →    GND             Black
CS            →    GPIO10          Orange
RST           →    GPIO8           White
DC            →    GPIO9           Gray
SDA/MOSI      →    GPIO11          Blue
SCL/SCLK      →    GPIO12          Purple
SDO/MISO      →    GPIO13          Brown
LED           →    3.3V            Red
```

---

## 🎨 **Visual Guide**

```
ESP32-S3 DevKit Pinout:

        [USB-C Port]
    ┌──────────────────┐
    │                  │
    │  3.3V ●──────────┼─── TTP229 VCC (Red)
    │                  │    Display VCC (Red)
    │                  │    Display LED (Red)
    │                  │
    │  GND  ●──────────┼─── TTP229 GND (Black)
    │                  │    Display GND (Black)
    │                  │
    │  IO1  ●──────────┼─── TTP229 SDO (Green)
    │  IO2  ●──────────┼─── TTP229 SCL (Yellow)
    │                  │
    │  IO8  ●──────────┼─── Display RST (White)
    │  IO9  ●──────────┼─── Display DC (Gray)
    │  IO10 ●──────────┼─── Display CS (Orange)
    │  IO11 ●──────────┼─── Display SDA (Blue)
    │  IO12 ●──────────┼─── Display SCL (Purple)
    │  IO13 ●──────────┼─── Display SDO (Brown)
    │                  │
    └──────────────────┘
```

---

## ✅ **Quick Checklist**

Before powering on, verify:

**TTP229:**
- [ ] VCC → 3.3V (NOT 5V!)
- [ ] GND → GND
- [ ] SCL → GPIO2
- [ ] SDO → GPIO1

**Display:**
- [ ] VCC → 3.3V
- [ ] GND → GND
- [ ] LED → 3.3V (backlight)
- [ ] CS → GPIO10
- [ ] RST → GPIO8
- [ ] DC → GPIO9
- [ ] SDA → GPIO11
- [ ] SCL → GPIO12
- [ ] SDO → GPIO13

---

## 🧪 **Test It!**

### **Step 1: Upload Test Code**
```
File: src/ttp229_keypad_test.cpp
```

### **Step 2: Open Serial Monitor**
```
Baud Rate: 115200
```

### **Step 3: Check Output**
```
Should see:
✅ TTP229 module found!
✅ I2C SDA: GPIO1
✅ I2C SCL: GPIO2
```

### **Step 4: Touch Keys**
```
Touch any key (1-16) on TTP229
Should see: "👆 Key X pressed"
```

### **Step 5: Upload Full Interface**
```
File: src/ttp229_gesture_ipod.cpp
```

### **Step 6: Try Gestures!**
```
- Tap: Select
- Swipe: Navigate
- Circular: Scroll (iPod wheel!)
- Double tap: Play/pause
```

---

## 🚨 **Common Issues**

### **"TTP229 not found"**
```
Fix:
1. Check GPIO1 and GPIO2 (not 21/22!)
2. Verify 3.3V power
3. Check GND connection
4. Swap SCL and SDO if still not working
```

### **Display blank/white**
```
Fix:
1. Check LED connected to 3.3V
2. Verify all 9 wires connected
3. Check RST pin (GPIO8)
4. Try different USB cable
```

### **Touch not responding**
```
Fix:
1. Clean touch pads
2. Dry fingers
3. Press firmly
4. Check I2C address (0x57)
```

---

## 💡 **Pro Tips**

1. **Use breadboard power rails** for 3.3V and GND
2. **Keep wires short** (< 15cm)
3. **Color code your wires** for easy debugging
4. **Take a photo** of your wiring for reference
5. **Test TTP229 first** before connecting display

---

## 🎯 **Pin Summary**

```
ESP32-S3 Pins Used:
GPIO1  - TTP229 SDA (I2C Data)
GPIO2  - TTP229 SCL (I2C Clock)
GPIO8  - Display Reset
GPIO9  - Display Data/Command
GPIO10 - Display Chip Select
GPIO11 - Display MOSI (SPI Data)
GPIO12 - Display Clock (SPI Clock)
GPIO13 - Display MISO (SPI Data)

Total: 8 GPIO pins + Power/Ground
```

---

## 🚀 **You're Done!**

With just **13 wires**, you have:
- ✅ 16-key capacitive touch keypad
- ✅ 2.8" color display
- ✅ Gesture recognition
- ✅ Full iPod interface

Now enjoy your gesture-controlled iPod! 🎵

---

## 📝 **Notes**

**Why GPIO1 and GPIO2?**
- ESP32-S3 uses GPIO21/22 for USB
- GPIO1/2 are the standard I2C pins for S3
- These work perfectly for TTP229

**Can I use different pins?**
- Yes! Any GPIO can be I2C
- Just update the code:
  ```cpp
  #define I2C_SDA X  // Your pin
  #define I2C_SCL Y  // Your pin
  ```

**Need help?**
- Check Serial Monitor for errors
- Verify connections with multimeter
- Test each component separately
- Read full guide: docs/complete_wiring_ttp229.md