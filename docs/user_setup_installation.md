# User_Setup.h Installation Guide

## For TFT_eSPI Library Configuration

### 📍 Where to Place User_Setup.h

The `User_Setup.h` file needs to be placed in the TFT_eSPI library folder, not in your project folder.

### 🔍 Finding the Library Location

#### **Method 1: Arduino IDE**
1. Open Arduino IDE
2. Go to **File → Preferences**
3. Note the "Sketchbook location" path
4. Navigate to: `[Sketchbook]/libraries/TFT_eSPI/`
5. Replace the existing `User_Setup.h` with our version

#### **Method 2: PlatformIO**
1. Look in your project's `.pio/libdeps/[board_name]/TFT_eSPI/`
2. Replace `User_Setup.h` with our version
3. Or add to `platformio.ini`:
   ```ini
   build_flags = 
       -DUSER_SETUP_LOADED=1
       -DILI9341_DRIVER=1
       -DTFT_CS=10
       -DTFT_DC=9
       -DTFT_RST=8
       -DTFT_MOSI=11
       -DTFT_SCLK=12
       -DTFT_MISO=13
   ```

#### **Method 3: System Libraries**
**Windows:**
```
C:\Users\[Username]\Documents\Arduino\libraries\TFT_eSPI\User_Setup.h
```

**macOS:**
```
/Users/[Username]/Documents/Arduino/libraries/TFT_eSPI/User_Setup.h
```

**Linux:**
```
/home/[Username]/Arduino/libraries/TFT_eSPI/User_Setup.h
```

### 📋 Installation Steps

1. **Backup Original File**
   ```bash
   cp User_Setup.h User_Setup.h.backup
   ```

2. **Copy Our Configuration**
   - Copy the contents of `src/User_Setup.h` from this project
   - Paste into the TFT_eSPI library's `User_Setup.h`

3. **Verify Configuration**
   - Check that pin numbers match your wiring
   - Ensure `ILI9341_DRIVER` is defined
   - Confirm SPI frequencies are conservative (20MHz)

### 🔧 Alternative: Use Adafruit Libraries (Recommended)

Instead of TFT_eSPI, use Adafruit libraries which are more reliable:

#### **PlatformIO Configuration**
```ini
lib_deps = 
    adafruit/Adafruit GFX Library@^1.11.9
    adafruit/Adafruit ILI9341@^1.6.0
    adafruit/Adafruit BusIO@^1.14.5
    paulstoffregen/XPT2046_Touchscreen@^1.4
```

#### **Test Code**
Use `src/display_touch_test.cpp` which supports both libraries:
- Uncomment `#define USE_ADAFRUIT_LIBRARY` for Adafruit
- Uncomment `#define USE_TFT_ESPI_LIBRARY` for TFT_eSPI

### 🚨 Common Issues & Solutions

#### **Issue 1: Compilation Errors**
```
error: 'TFT_CS' was not declared
```
**Solution:** User_Setup.h not found or pins not defined

#### **Issue 2: White Screen**
```
Display shows backlight but no content
```
**Solutions:**
- Check wiring connections
- Try different SPI frequency (lower = more stable)
- Verify power supply (3.3V, sufficient current)

#### **Issue 3: Wrong Colors**
```
Colors appear incorrect or inverted
```
**Solutions:**
- Change `TFT_RGB_ORDER` to `TFT_RGB` or `TFT_BGR`
- Try different color definitions

#### **Issue 4: Touch Not Working**
```
Touch controller not detected
```
**Solutions:**
- Check touch wiring (separate SPI pins)
- Verify XPT2046_Touchscreen library is installed
- Test touch pins with multimeter

### 📝 Pin Configuration Summary

```cpp
// Display (ILI9341)
#define TFT_CS    10    // Chip Select
#define TFT_DC    9     // Data/Command
#define TFT_RST   8     // Reset
#define TFT_MOSI  11    // Data Out
#define TFT_SCLK  12    // Clock
#define TFT_MISO  13    // Data In (optional)

// Touch (XPT2046)
#define TOUCH_CS  6     // Touch Chip Select
#define TOUCH_IRQ 3     // Touch Interrupt (optional)
```

### 🧪 Testing Your Configuration

1. **Upload GPIO Test First**
   ```cpp
   // Test file: src/simple_gpio_test.cpp
   ```

2. **Test Display Only**
   ```cpp
   // Test file: src/adafruit_tft_test.cpp
   ```

3. **Test Display + Touch**
   ```cpp
   // Test file: src/display_touch_test.cpp
   ```

### 💡 Pro Tips

1. **Start with Adafruit libraries** - more stable and easier to configure
2. **Use conservative SPI speeds** - 20MHz or lower for breadboard
3. **Test components separately** - display first, then touch
4. **Check power supply** - ensure 3.3V and sufficient current
5. **Use short wires** - minimize interference on breadboard

### 🔄 Switching Between Libraries

If TFT_eSPI doesn't work, switch to Adafruit:

1. **Update platformio.ini**
2. **Use src/adafruit_config.h** instead of User_Setup.h
3. **Upload src/display_touch_test.cpp** with Adafruit enabled

The Adafruit approach is generally more reliable for ESP32-S3!