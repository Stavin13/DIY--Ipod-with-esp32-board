# Arduino IDE Setup for ESP32-S3 iPod

## Step 1: Install ESP32 Board Package

### 1.1 Add ESP32 Board Manager URL
1. Open Arduino IDE
2. Go to **File → Preferences**
3. In "Additional Board Manager URLs", add:
   ```
   https://espressif.github.io/arduino-esp32/package_esp32_index.json
   ```
4. Click **OK**

### 1.2 Install ESP32 Boards
1. Go to **Tools → Board → Boards Manager**
2. Search for "ESP32"
3. Install **"esp32" by Espressif Systems** (latest version)
4. Wait for installation to complete

### 1.3 Select Your Board
1. Go to **Tools → Board → ESP32 Arduino**
2. Select **"ESP32S3 Dev Module"**
3. Configure board settings:
   - **CPU Frequency**: 240MHz
   - **Flash Mode**: QIO
   - **Flash Size**: 16MB
   - **Partition Scheme**: Huge APP (3MB No OTA/1MB SPIFFS)
   - **PSRAM**: Enabled

## Step 2: Install Required Libraries

### 2.1 Open Library Manager
1. Go to **Tools → Manage Libraries**
2. Search and install each library below

### 2.2 Install These Libraries (in order):

#### **Library 1: Adafruit BusIO**
- Search: "Adafruit BusIO"
- Install: **"Adafruit BusIO" by Adafruit**
- Version: Latest

#### **Library 2: Adafruit GFX Library**
- Search: "Adafruit GFX"
- Install: **"Adafruit GFX Library" by Adafruit**
- Version: Latest

#### **Library 3: Adafruit ILI9341**
- Search: "Adafruit ILI9341"
- Install: **"Adafruit ILI9341" by Adafruit**
- Version: Latest

#### **Library 4: XPT2046 Touchscreen**
- Search: "XPT2046"
- Install: **"XPT2046_Touchscreen" by Paul Stoffregen**
- Version: Latest

### 2.3 Verify Installation
After installing, you should see these in **Sketch → Include Library**:
- ✅ Adafruit GFX Library
- ✅ Adafruit ILI9341
- ✅ XPT2046_Touchscreen
- ✅ Adafruit BusIO

## Step 3: Upload Test Code

### 3.1 Create New Sketch
1. **File → New** (create new sketch)
2. Copy the contents of `src/arduino_display_test.cpp`
3. Paste into the Arduino IDE
4. **File → Save As** → Name it "ESP32_iPod_Test"

### 3.2 Connect ESP32-S3
1. Connect ESP32-S3 to computer via USB-C cable
2. **Tools → Port** → Select your ESP32 port (usually `/dev/cu.usbmodem...` on Mac)

### 3.3 Upload Code
1. Click **Upload** button (→)
2. If upload fails, hold **BOOT** button on ESP32-S3 while clicking upload
3. Wait for "Done uploading" message

### 3.4 Open Serial Monitor
1. **Tools → Serial Monitor**
2. Set baud rate to **115200**
3. You should see initialization messages

## Step 4: Wiring Your Display

### 4.1 Power Off ESP32-S3
Disconnect USB cable before wiring

### 4.2 Connect Display to ESP32-S3
```
ESP32-S3    →    MSP2806 Display
GPIO10      →    CS
GPIO9       →    DC
GPIO8       →    RST
GPIO11      →    SDA/MOSI
GPIO12      →    SCL/SCLK
GPIO13      →    SDO/MISO (optional)
GPIO6       →    T_CS
GPIO7       →    T_CLK
GPIO5       →    T_DIN
GPIO4       →    T_DO
GPIO3       →    T_IRQ (optional)
3.3V        →    VCC
3.3V        →    LED (backlight)
GND         →    GND
```

### 4.3 Double-Check Connections
- **Power**: 3.3V and GND connected
- **Display SPI**: CS, DC, RST, MOSI, SCLK
- **Touch SPI**: T_CS, T_CLK, T_DIN, T_DO

## Step 5: Test Your Setup

### 5.1 Power On
1. Reconnect USB cable
2. ESP32-S3 should boot
3. Display backlight should turn on

### 5.2 Expected Results
**Serial Monitor should show:**
```
=== ESP32-S3 iPod Display Test ===
MSP2806 - 2.8" ILI9341 + Touch

Wiring Check:
ESP32-S3 → MSP2806
...

Initializing display...
Display initialized!
Initializing touch...
Touch initialized!
Running display tests...
```

**Display should show:**
1. **Color screens**: Black → Red → Green → Blue
2. **Text**: "ESP32-S3 iPod Test" with status messages
3. **Shapes**: Rectangles, circles, lines
4. **Color bars**: 6 colored vertical bars
5. **iPod interface**: Mock music player interface

### 5.3 Touch Test
- Touch anywhere on screen
- Should see red circles where you touch
- Serial Monitor shows touch coordinates

## 🚨 Troubleshooting

### Issue 1: Upload Failed
```
Failed to connect to ESP32-S3
```
**Solutions:**
- Hold **BOOT** button while clicking upload
- Try different USB cable (data cable, not charge-only)
- Check port selection in Tools → Port

### Issue 2: Library Not Found
```
fatal error: Adafruit_ILI9341.h: No such file or directory
```
**Solutions:**
- Install libraries via Library Manager (Step 2)
- Restart Arduino IDE after installing
- Check library installation in Sketch → Include Library

### Issue 3: White Screen Only
```
Display shows backlight but no content
```
**Solutions:**
- Check wiring connections
- Verify 3.3V power supply
- Try different jumper wires
- Check breadboard connections

### Issue 4: Compilation Errors
```
Multiple definition errors
```
**Solutions:**
- Use only one .cpp file at a time
- Don't mix Arduino IDE with PlatformIO files
- Create fresh sketch with only the test code

### Issue 5: Touch Not Working
```
Touch coordinates not showing
```
**Solutions:**
- Check touch wiring (T_CS, T_CLK, T_DIN, T_DO)
- Verify XPT2046_Touchscreen library installed
- Try touching different areas of screen

## 📋 Quick Checklist

Before asking for help, verify:
- ✅ ESP32 board package installed
- ✅ All 4 libraries installed via Library Manager
- ✅ Board set to "ESP32S3 Dev Module"
- ✅ Port selected correctly
- ✅ All wiring connections secure
- ✅ 3.3V power connected to VCC and LED
- ✅ GND connected properly
- ✅ Serial Monitor at 115200 baud

## 🎯 Success Indicators

**You're ready for the next step when:**
- Display shows colors and text clearly
- Touch responds with red circles
- Serial Monitor shows "All tests complete!"
- No compilation or upload errors

Once this basic test works, you can move on to the full iPod software!