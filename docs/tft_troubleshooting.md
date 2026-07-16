# TFT Display Troubleshooting Guide

## Current Issue: White Screen Only

You're getting a white screen because the backlight is working but the display controller isn't receiving proper signals.

## Step-by-Step Solution

### Step 1: Fix Library Issues

**Problem**: TFT_eSPI has compilation errors with ESP32-S3
**Solution**: Switch to Adafruit libraries (more stable)

1. **Update platformio.ini** (already done):
   ```ini
   lib_deps = 
       adafruit/Adafruit GFX Library@^1.11.9
       adafruit/Adafruit ILI9341@^1.6.0
       adafruit/Adafruit BusIO@^1.14.5
   ```

2. **Use the new test file**: `src/adafruit_tft_test.cpp`

### Step 2: Verify Your Wiring

**Double-check these connections**:
```
ESP32-S3    →    ILI9341 Display
GPIO10      →    CS
GPIO9       →    DC  
GPIO8       →    RST
GPIO11      →    MOSI (SDA)
GPIO12      →    SCLK (SCL)
GPIO13      →    MISO (optional)
3.3V        →    VCC
3.3V        →    LED (backlight)
GND         →    GND
```

### Step 3: Test GPIO Pins First

Before testing display, verify ESP32-S3 pins work:

1. **Upload**: `src/simple_gpio_test.cpp`
2. **Watch Serial Monitor**: Should see pin toggle messages
3. **Optional**: Use multimeter to verify 3.3V on pins

### Step 4: Test Display with Adafruit Library

1. **Upload**: `src/adafruit_tft_test.cpp`
2. **Expected Results**:
   - Serial: "Display initialized!"
   - Screen: Red → Green → Blue → Text and shapes
   - Blinking white dot

### Step 5: If Still White Screen

**Check these common issues**:

1. **Power Supply**:
   - ESP32-S3 needs stable 3.3V
   - Display draws ~50mA
   - USB power should be sufficient

2. **Wiring Issues**:
   - Loose breadboard connections
   - Wrong GPIO pins
   - Crossed wires

3. **Display Issues**:
   - Some ILI9341 displays need different initialization
   - Try different SPI speeds

### Step 6: Alternative Test Code

If Adafruit library doesn't work, try this minimal test:

```cpp
#include <SPI.h>

#define TFT_CS    10
#define TFT_DC    9  
#define TFT_RST   8

void setup() {
  Serial.begin(115200);
  
  // Initialize pins
  pinMode(TFT_CS, OUTPUT);
  pinMode(TFT_DC, OUTPUT);
  pinMode(TFT_RST, OUTPUT);
  
  // Reset display
  digitalWrite(TFT_RST, LOW);
  delay(100);
  digitalWrite(TFT_RST, HIGH);
  delay(100);
  
  // Initialize SPI
  SPI.begin(12, 13, 11, 10);  // SCLK, MISO, MOSI, CS
  SPI.setFrequency(1000000);  // 1MHz (slow and safe)
  
  Serial.println("Basic SPI test - check for any response");
}
```

## Next Steps

1. **Try GPIO test first** - verify ESP32-S3 pins work
2. **Upload Adafruit test** - should show colors
3. **If still white**: Check wiring with multimeter
4. **Report results**: Tell me what you see in Serial Monitor

## Common ESP32-S3 Issues

- **USB Connection**: Use data cable, not charge-only
- **Boot Mode**: Hold BOOT button while uploading if needed
- **Power**: Some displays need external 5V (yours should work on 3.3V)
- **SPI Speed**: Try slower speeds if display glitches

## Success Indicators

✅ **GPIO Test**: Pin toggle messages in Serial Monitor
✅ **Display Test**: Colors, text, and shapes on screen  
✅ **Serial Output**: "Display initialized!" message
✅ **Animation**: Blinking white dot in center

Let me know what happens with each test!