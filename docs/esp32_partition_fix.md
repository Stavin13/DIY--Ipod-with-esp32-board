# ESP32-S3 Partition Error Fix

## Error Explanation
```
cp: /Users/stavinfernandes/Library/Arduino15/packages/esp32/hardware/esp32/3.3.5/tools/partitions/{build.partitions}.csv: No such file or directory
```

This error occurs when the ESP32-S3 board configuration references a partition scheme that doesn't exist in your ESP32 board package.

## Quick Fix - Change Board Settings

### Step 1: Correct Board Configuration
In Arduino IDE, go to **Tools** menu and set:

1. **Board**: "ESP32S3 Dev Module" (not other variants)
2. **Partition Scheme**: "Default 4MB with spiffs (1.2MB APP/1.5MB SPIFFS)"
3. **Flash Size**: "4MB" (not 16MB for now)
4. **PSRAM**: "Disabled" (for initial testing)

### Step 2: Alternative Board Settings (if above doesn't work)
Try these settings:
1. **Board**: "ESP32S3 Dev Module"
2. **Partition Scheme**: "No OTA (2MB APP/2MB SPIFFS)"
3. **Flash Size**: "4MB"
4. **PSRAM**: "Disabled"

### Step 3: If Still Failing, Use Minimal Settings
1. **Board**: "ESP32S3 Dev Module"
2. **Partition Scheme**: "Minimal SPIFFS (1.9MB APP with OTA/190KB SPIFFS)"
3. **Flash Size**: "4MB"
4. **PSRAM**: "Disabled"

## Complete Arduino IDE Settings for ESP32-S3

```
Tools Menu Settings:
├── Board: "ESP32S3 Dev Module"
├── Upload Speed: "921600"
├── CPU Frequency: "240MHz (WiFi/BT)"
├── Flash Mode: "QIO"
├── Flash Frequency: "80MHz"
├── Flash Size: "4MB"
├── Partition Scheme: "Default 4MB with spiffs"
├── Core Debug Level: "None"
├── PSRAM: "Disabled" (enable later after basic test works)
├── Arduino Runs On: "Core 1"
├── Events Run On: "Core 1"
└── Port: [Your ESP32-S3 port]
```

## Alternative: Update ESP32 Board Package

### Method 1: Update to Latest Version
1. **Tools → Board → Boards Manager**
2. Search "ESP32"
3. Update "esp32" by Espressif Systems to latest version
4. Restart Arduino IDE

### Method 2: Use Specific Version
If latest doesn't work, try version 2.0.11:
1. In Boards Manager, click dropdown next to ESP32
2. Select version "2.0.11"
3. Install that specific version

## Test with Simple Code First

Before using the display code, test with this minimal sketch:

```cpp
void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("ESP32-S3 Basic Test");
  Serial.println("If you see this, board settings are correct!");
  
  // Test built-in LED (if available)
  pinMode(2, OUTPUT);
}

void loop() {
  digitalWrite(2, HIGH);
  Serial.println("LED ON");
  delay(1000);
  
  digitalWrite(2, LOW);
  Serial.println("LED OFF");
  delay(1000);
}
```

## If Problem Persists

### Check ESP32 Board Package Installation
1. Go to: `/Users/stavinfernandes/Library/Arduino15/packages/esp32/hardware/esp32/3.3.5/tools/partitions/`
2. List files: should see `.csv` files like:
   - `default.csv`
   - `minimal.csv`
   - `no_ota.csv`
   - `huge_app.csv`

### Reinstall ESP32 Board Package
1. **Tools → Board → Boards Manager**
2. Find "esp32" by Espressif Systems
3. Click "Remove"
4. Restart Arduino IDE
5. Reinstall "esp32" package
6. Restart Arduino IDE again

### Manual Fix (Advanced)
If partition files are missing:
1. Download ESP32 Arduino Core from GitHub
2. Copy partition files to: `/Users/stavinfernandes/Library/Arduino15/packages/esp32/hardware/esp32/[version]/tools/partitions/`

## Recommended Workflow

### Step 1: Fix Board Settings
Use the minimal settings above

### Step 2: Test Basic Sketch
Upload the simple LED blink code

### Step 3: Test GPIO
Upload `src/simple_gpio_test.cpp`

### Step 4: Test Display
Once basic functionality works, try display code

## Common Board Setting Issues

### Issue 1: Wrong Board Selected
- ❌ "ESP32 Dev Module" (wrong for S3)
- ❌ "ESP32-S3-DevKitC-1" (might not exist)
- ✅ "ESP32S3 Dev Module" (correct)

### Issue 2: Incompatible Partition Scheme
- ❌ Custom partition schemes
- ❌ 16MB partitions on 4MB flash
- ✅ Default 4MB schemes

### Issue 3: PSRAM Issues
- Start with PSRAM disabled
- Enable only after basic functionality works

## Success Indicators

You'll know it's working when:
- ✅ Code compiles without partition errors
- ✅ Upload completes successfully
- ✅ Serial Monitor shows output at 115200 baud
- ✅ No "partition not found" errors

Once basic upload works, you can gradually enable more features like PSRAM and try the display code.