# TTP229 Gesture Troubleshooting Guide

## 🔍 **Step-by-Step Debugging**

### **Step 1: Check Basic Touch Detection**

Upload `src/ttp229_simple_test.cpp` and open Serial Monitor (115200 baud).

**Expected Output:**
```
✅ TTP229 found at address 0x57
Touch any key (1-16)...
```

**When you touch a key:**
```
Touch detected! Raw value: 0x0001 (0000000000000001) - Keys: 1
Touch detected! Raw value: 0x0002 (0000000000000010) - Keys: 2
```

---

## 🚨 **Common Issues & Fixes**

### **Issue 1: "TTP229 NOT found!"**

**Symptoms:**
```
❌ TTP229 NOT found! Error: 2
```

**Causes & Solutions:**

#### **A. Wrong I2C Pins**
```
Check your wiring:
TTP229 SCL → ESP32-S3 GPIO2 (not GPIO22!)
TTP229 SDO → ESP32-S3 GPIO1 (not GPIO21!)

ESP32-S3 uses GPIO1/2 for I2C, not 21/22!
```

#### **B. Swapped SDA/SCL**
```
Try swapping:
If SCL is on GPIO1 and SDO on GPIO2, swap them:
TTP229 SCL → GPIO2
TTP229 SDO → GPIO1
```

#### **C. Power Issue**
```
Check:
1. VCC connected to 3.3V (NOT 5V!)
2. GND connected properly
3. Power LED on TTP229 lit (if present)
4. Try different 3.3V pin on ESP32-S3
```

#### **D. Bad Connections**
```
1. Push wires firmly into breadboard
2. Check for loose connections
3. Try different jumper wires
4. Check breadboard power rails
```

---

### **Issue 2: Touch Detected But No Gestures**

**Symptoms:**
```
✅ TTP229 found
✅ Keys detected when touched
❌ Gestures not recognized
```

**Solutions:**

#### **A. Gesture Too Fast**
```
Problem: Moving too quickly
Solution: Slow down your swipe/scroll motion
Try: 1-2 seconds for full swipe
```

#### **B. Gesture Too Slow**
```
Problem: Moving too slowly
Solution: Speed up slightly
Try: Complete gesture in 0.5-1 second
```

#### **C. Not Enough Keys**
```
Problem: Only touching 1-2 keys
Solution: Touch more keys in sequence
For swipe: Touch at least 3 keys
For scroll: Touch at least 4 keys in circular pattern
```

#### **D. Erratic Path**
```
Problem: Touching random keys
Solution: Follow straight or circular path
For swipe up: 13→9→5→1 (straight line)
For scroll CW: 1→2→3→4→8→12→16 (circular)
```

---

### **Issue 3: Inconsistent Detection**

**Symptoms:**
```
Sometimes works, sometimes doesn't
```

**Solutions:**

#### **A. Adjust Timing Thresholds**

In `src/ttp229_gesture_ipod.cpp`, modify:

```cpp
// Make gestures easier to detect
#define TAP_THRESHOLD 300        // Was 200ms
#define LONG_PRESS_THRESHOLD 400 // Was 500ms
#define SWIPE_MIN_KEYS 2         // Was 2 (keep same)
#define SCROLL_THRESHOLD 2       // Was 3 (easier)
```

#### **B. Increase Touch History**

```cpp
// Remember more touches
#define TOUCH_HISTORY_SIZE 15    // Was 10
```

#### **C. Clean Touch Pads**
```
1. Wipe pads with dry cloth
2. Ensure fingers are dry
3. Press firmly but not too hard
4. Touch center of each pad
```

---

### **Issue 4: Only Tap Works, No Swipes**

**Symptoms:**
```
✅ Tap detected
❌ Swipe not detected
❌ Scroll not detected
```

**Debug Steps:**

#### **1. Check Serial Monitor**
```
Look for:
"🎯 Gesture detected: TAP" ← Working
"🎯 Gesture detected: SWIPE UP" ← Not working?
```

#### **2. Print Touch History**

Add this to `handleTouch()` in gesture code:

```cpp
// After touch end, print history
if (touchStatus == 0 && lastTouchStatus != 0) {
  Serial.print("Touch history: ");
  for (int i = 0; i < TOUCH_HISTORY_SIZE; i++) {
    if (touchHistory[i] > 0) {
      Serial.print(touchHistory[i]);
      Serial.print(" ");
    }
  }
  Serial.println();
}
```

**Expected for swipe up:**
```
Touch history: 13 9 5 1
```

**If you see:**
```
Touch history: 13 13 13 13  ← Only one key, not moving
Touch history: 13 7 2 16    ← Random keys, not straight line
```

#### **3. Simplify Gesture Detection**

Try this simpler swipe detection:

```cpp
GestureType detectSwipeDirection() {
  int firstKey = -1, lastKey = -1;
  
  // Get first and last keys
  for (int i = 0; i < TOUCH_HISTORY_SIZE; i++) {
    if (touchHistory[i] > 0) {
      if (firstKey == -1) firstKey = touchHistory[i];
      lastKey = touchHistory[i];
    }
  }
  
  if (firstKey == -1 || lastKey == -1) return GESTURE_NONE;
  
  // Simple direction check
  if (lastKey < firstKey - 4) return GESTURE_SWIPE_UP;
  if (lastKey > firstKey + 4) return GESTURE_SWIPE_DOWN;
  if ((lastKey % 4) > (firstKey % 4)) return GESTURE_SWIPE_RIGHT;
  if ((lastKey % 4) < (firstKey % 4)) return GESTURE_SWIPE_LEFT;
  
  return GESTURE_NONE;
}
```

---

### **Issue 5: False Gesture Detection**

**Symptoms:**
```
Gestures detected when not intended
Random swipes/scrolls
```

**Solutions:**

#### **A. Increase Thresholds**
```cpp
#define TAP_THRESHOLD 150        // Stricter (was 200)
#define SWIPE_MIN_KEYS 3         // Need more keys (was 2)
#define SCROLL_THRESHOLD 4       // Need more keys (was 3)
```

#### **B. Add Minimum Distance**
```cpp
// In detectSwipeDirection()
int distance = abs(lastKey - firstKey);
if (distance < 4) return GESTURE_NONE; // Too short
```

#### **C. Debounce Touches**
```cpp
// In handleTouch()
static unsigned long lastGestureTime = 0;
if (millis() - lastGestureTime < 500) return; // Ignore rapid gestures
lastGestureTime = millis();
```

---

## 🧪 **Testing Gestures**

### **Test Each Gesture Individually:**

#### **1. Test Tap**
```
Action: Quick touch key 6 (center)
Expected: "🎯 Gesture detected: TAP"
Duration: < 200ms
```

#### **2. Test Swipe Up**
```
Action: Touch 13 → 9 → 5 → 1 (bottom to top)
Expected: "🎯 Gesture detected: SWIPE UP"
Speed: 1 second for full swipe
```

#### **3. Test Swipe Down**
```
Action: Touch 1 → 5 → 9 → 13 (top to bottom)
Expected: "🎯 Gesture detected: SWIPE DOWN"
```

#### **4. Test Swipe Left**
```
Action: Touch 4 → 3 → 2 → 1 (right to left)
Expected: "🎯 Gesture detected: SWIPE LEFT"
```

#### **5. Test Swipe Right**
```
Action: Touch 1 → 2 → 3 → 4 (left to right)
Expected: "🎯 Gesture detected: SWIPE RIGHT"
```

#### **6. Test Circular Scroll**
```
Action: Touch 1 → 2 → 3 → 4 → 8 → 12 (clockwise arc)
Expected: "🎯 Gesture detected: SCROLL CLOCKWISE"
Speed: 1-2 seconds for arc
```

---

## 💡 **Pro Tips**

### **For Better Gesture Recognition:**

1. **Touch firmly** - Don't just graze the pads
2. **Move smoothly** - Consistent speed
3. **Follow paths** - Straight lines or circular arcs
4. **Practice** - Gestures improve with practice
5. **Check Serial** - Monitor shows what's detected

### **Optimal Gesture Speed:**
```
Tap:         < 200ms
Swipe:       500-1000ms
Scroll:      1000-2000ms
Long Press:  > 500ms
```

### **Optimal Touch Pattern:**
```
Swipe:  Touch 3-4 keys in line
Scroll: Touch 4-6 keys in arc
Tap:    Single key, quick release
```

---

## 🔧 **Quick Fixes**

### **If Nothing Works:**

1. **Upload simple test first:**
   ```
   src/ttp229_simple_test.cpp
   ```

2. **Verify each key works:**
   ```
   Touch keys 1-16 individually
   All should register
   ```

3. **Check wiring again:**
   ```
   VCC → 3.3V
   GND → GND
   SCL → GPIO2
   SDO → GPIO1
   ```

4. **Try different I2C pins:**
   ```cpp
   #define I2C_SDA 3  // Try GPIO3
   #define I2C_SCL 4  // Try GPIO4
   ```

5. **Reduce gesture complexity:**
   ```
   Start with just tap and swipe
   Add scroll later when working
   ```

---

## 📊 **Expected Behavior**

### **Working System:**
```
✅ TTP229 detected on I2C
✅ All 16 keys respond to touch
✅ Tap gesture works reliably
✅ Swipe gestures work (may need practice)
✅ Scroll gestures work (requires smooth motion)
✅ Serial Monitor shows gesture names
✅ Display updates with gestures
```

### **Partially Working:**
```
✅ TTP229 detected
✅ Keys respond
⚠️ Only tap works
❌ Swipes not detected
```
**Solution:** Adjust timing thresholds, slow down gestures

### **Not Working:**
```
❌ TTP229 not detected
```
**Solution:** Check wiring, especially GPIO1/2

---

## 🎯 **Final Checklist**

Before asking for help, verify:

- [ ] TTP229 detected in simple test
- [ ] All 16 keys respond individually
- [ ] Wiring correct (GPIO1/2, not 21/22)
- [ ] Power is 3.3V (not 5V)
- [ ] Serial Monitor shows touch events
- [ ] Tried adjusting timing thresholds
- [ ] Practiced gesture motions
- [ ] Checked for loose connections

If all checked and still not working, provide:
1. Serial Monitor output
2. Photo of wiring
3. Which gestures work/don't work
4. Touch history output

This will help diagnose the specific issue! 🚀