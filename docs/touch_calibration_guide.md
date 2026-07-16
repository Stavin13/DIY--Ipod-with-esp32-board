# Touch Calibration Improvement Guide

## Methods to Increase Touch Calibration Accuracy

### 🎯 Method 1: Force Recalibration

**In User_Setup.h, change:**
```cpp
#define REPEAT_CAL true   // Forces recalibration every boot
```

**Or in your code:**
```cpp
// Delete calibration file to force recalibration
SPIFFS.remove("/TouchCalData");
```

### 🎯 Method 2: Use Advanced Calibration Script

Upload `src/advanced_touch_calibration.cpp` which provides:

1. **Standard 5-Point Calibration** - Basic TFT_eSPI calibration
2. **Enhanced 9-Point Calibration** - More calibration points for accuracy
3. **Manual Fine-Tuning** - Adjust calibration parameters manually
4. **Calibration Testing** - Test accuracy with visual feedback

### 🎯 Method 3: Improve Hardware Setup

#### **Better Touch Accuracy:**
- ✅ **Use a stylus** instead of finger for calibration
- ✅ **Stable surface** - don't hold device while calibrating
- ✅ **Good lighting** - see calibration targets clearly
- ✅ **Clean screen** - remove fingerprints and dust
- ✅ **Steady hands** - take time with each calibration point

#### **Wiring Improvements:**
- ✅ **Shorter wires** - reduce electrical noise
- ✅ **Twisted pair wires** - for SPI signals
- ✅ **Good connections** - no loose breadboard connections
- ✅ **Separate power** - dedicated 3.3V for touch controller

### 🎯 Method 4: Software Calibration Tweaks

#### **In User_Setup.h:**
```cpp
// Increase touch precision
#define TOUCH_PRECISION 15    // Higher = more precise (1-20)
#define TOUCH_THRESHOLD 500   // Lower = more sensitive
#define TOUCH_Z_THRESHOLD 300 // Pressure sensitivity
```

#### **In your code:**
```cpp
// Custom calibration values (adjust for your display)
uint16_t calData[5] = {
  300,   // Touch X minimum (adjust if touch offset)
  3700,  // Touch X maximum (adjust if touch offset)  
  300,   // Touch Y minimum (adjust if touch offset)
  3700,  // Touch Y maximum (adjust if touch offset)
  1      // Touch orientation (0-7, try different values)
};

// Apply custom calibration
tft.setTouch(calData);
```

### 🎯 Method 5: Multi-Point Calibration

```cpp
void improvedCalibration() {
  // Do standard calibration first
  tft.calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);
  
  // Then fine-tune with additional points
  calibrateCorners();
  calibrateEdges();
  calibrateCenter();
}

void calibrateCorners() {
  // Test and adjust corner accuracy
  struct Point { int x, y; };
  Point corners[] = {{20,20}, {220,20}, {220,300}, {20,300}};
  
  for (int i = 0; i < 4; i++) {
    // Show target at corner
    drawTarget(corners[i].x, corners[i].y);
    
    // Get touch input
    uint16_t tx, ty;
    while (!tft.getTouch(&tx, &ty)) delay(50);
    
    // Calculate offset and adjust calibration
    adjustCalibration(corners[i].x, corners[i].y, tx, ty);
  }
}
```

### 🎯 Method 6: Calibration Validation

```cpp
void validateCalibration() {
  // Test known points
  struct TestPoint { int x, y; const char* name; };
  TestPoint testPoints[] = {
    {20, 20, "Top-Left"},
    {120, 20, "Top-Center"}, 
    {220, 20, "Top-Right"},
    {120, 160, "Center"},
    {20, 300, "Bottom-Left"},
    {220, 300, "Bottom-Right"}
  };
  
  float totalError = 0;
  int pointCount = sizeof(testPoints) / sizeof(testPoints[0]);
  
  for (int i = 0; i < pointCount; i++) {
    // Show target
    drawTarget(testPoints[i].x, testPoints[i].y);
    
    // Get touch
    uint16_t tx, ty;
    while (!tft.getTouch(&tx, &ty)) delay(50);
    
    // Calculate error
    float error = sqrt(pow(testPoints[i].x - tx, 2) + pow(testPoints[i].y - ty, 2));
    totalError += error;
    
    Serial.print(testPoints[i].name);
    Serial.print(" error: ");
    Serial.print(error);
    Serial.println(" pixels");
  }
  
  float avgError = totalError / pointCount;
  Serial.print("Average error: ");
  Serial.print(avgError);
  Serial.println(" pixels");
  
  if (avgError < 10) {
    Serial.println("Calibration: EXCELLENT");
  } else if (avgError < 20) {
    Serial.println("Calibration: GOOD");
  } else {
    Serial.println("Calibration: NEEDS IMPROVEMENT");
  }
}
```

## 🔧 Quick Calibration Fixes

### **Issue 1: Touch Offset (touches register in wrong location)**
```cpp
// Adjust these values in calData[]
calData[0] += 50;  // Shift touch right
calData[0] -= 50;  // Shift touch left
calData[2] += 50;  // Shift touch down  
calData[2] -= 50;  // Shift touch up
```

### **Issue 2: Touch Scale (touches too sensitive/insensitive)**
```cpp
// Adjust range values
calData[1] = calData[0] + 3400;  // X range (increase for less sensitive)
calData[3] = calData[2] + 3400;  // Y range (increase for less sensitive)
```

### **Issue 3: Inverted Touch (X or Y axis flipped)**
```cpp
// Try different orientation values
calData[4] = 0;  // Normal
calData[4] = 1;  // Rotate 90°
calData[4] = 2;  // Rotate 180°
calData[4] = 3;  // Rotate 270°
calData[4] = 4;  // Mirror X
calData[4] = 5;  // Mirror Y
calData[4] = 6;  // Mirror X + Rotate 90°
calData[4] = 7;  // Mirror Y + Rotate 90°
```

## 📊 Calibration Quality Targets

### **Excellent Calibration:**
- ✅ Average error < 10 pixels
- ✅ Corner accuracy < 5 pixels
- ✅ Center accuracy < 3 pixels
- ✅ Consistent across all areas

### **Good Calibration:**
- ✅ Average error < 20 pixels
- ✅ Corner accuracy < 15 pixels
- ✅ Usable for UI interaction

### **Poor Calibration (needs improvement):**
- ❌ Average error > 20 pixels
- ❌ Inconsistent response
- ❌ Dead zones or overly sensitive areas

## 🎯 Best Practices

1. **Calibrate in final orientation** - same rotation as your app
2. **Use consistent pressure** - same force for all calibration points
3. **Calibrate at operating temperature** - after device has warmed up
4. **Store multiple calibrations** - for different orientations
5. **Validate regularly** - check calibration accuracy periodically
6. **Use stylus for precision** - especially for small targets

The advanced calibration script provides all these methods in an easy-to-use interface!