# TTP229 Gesture Recognition Guide

## 🎯 **Gesture Support with Your TTP229 Module**

Your 16-key TTP229 module can detect **advanced gestures** including swipes, circular scrolling (like iPod wheel), multi-touch, and more!

## 📱 **Supported Gestures**

### **1. Tap (Single Touch)**
```
Action: Quick touch and release on any key
Duration: < 200ms
Use: Select item, confirm action

Example:
[●] → Release
Key 6 tapped → Select current menu item
```

### **2. Double Tap**
```
Action: Two quick taps in succession
Duration: < 300ms between taps
Use: Quick play/pause, zoom

Example:
[●] → Release → [●] → Release (fast)
Double tap anywhere → Play/Pause music
```

### **3. Long Press (Hold)**
```
Action: Touch and hold
Duration: > 500ms
Use: Go back, context menu, delete

Example:
[●●●●●] (hold)
Long press any key → Return to home screen
```

### **4. Swipe Up**
```
Action: Touch sequence moving upward
Pattern: Bottom row → Top row
Use: Navigate up, scroll up

Example:
[13] → [9] → [5] → [1]
Swipe up → Navigate to previous item
```

### **5. Swipe Down**
```
Action: Touch sequence moving downward
Pattern: Top row → Bottom row
Use: Navigate down, scroll down

Example:
[1] → [5] → [9] → [13]
Swipe down → Navigate to next item
```

### **6. Swipe Left**
```
Action: Touch sequence moving left
Pattern: Right column → Left column
Use: Previous track, go back

Example:
[4] → [3] → [2] → [1]
Swipe left → Previous track
```

### **7. Swipe Right**
```
Action: Touch sequence moving right
Pattern: Left column → Right column
Use: Next track, forward

Example:
[1] → [2] → [3] → [4]
Swipe right → Next track
```

### **8. Circular Scroll (Clockwise)**
```
Action: Touch sequence in circular pattern
Pattern: Following outer ring clockwise
Use: Volume up, scroll down list

Example:
[1] → [2] → [3] → [4] → [8] → [12] → [16]
Circular CW → Scroll down / Volume up
```

### **9. Circular Scroll (Counter-Clockwise)**
```
Action: Touch sequence in circular pattern
Pattern: Following outer ring counter-clockwise
Use: Volume down, scroll up list

Example:
[16] → [12] → [8] → [4] → [3] → [2] → [1]
Circular CCW → Scroll up / Volume down
```

### **10. Two-Finger Tap**
```
Action: Touch two keys simultaneously
Pattern: Any two keys at once
Use: Play/pause, special actions

Example:
[6] + [7] (together)
Two-finger tap → Play/Pause
```

### **11. Pinch In (Advanced)**
```
Action: Two touches moving closer
Pattern: Outer keys → Inner keys
Use: Zoom out, minimize

Example:
[1] + [16] → [6] + [11]
Pinch in → Zoom out
```

### **12. Pinch Out (Advanced)**
```
Action: Two touches moving apart
Pattern: Inner keys → Outer keys
Use: Zoom in, maximize

Example:
[6] + [11] → [1] + [16]
Pinch out → Zoom in
```

## 🎮 **iPod-Specific Gesture Mapping**

### **Music Navigation**
```
Gesture              Action
-------              ------
Tap                  Select song/menu
Double Tap           Play/Pause
Long Press           Back to home
Swipe Up             Previous item
Swipe Down           Next item
Swipe Left           Previous track
Swipe Right          Next track
Circular CW          Scroll down list
Circular CCW         Scroll up list
Two-Finger Tap       Play/Pause
```

### **Volume Control**
```
Gesture              Action
-------              ------
Circular CW          Volume up
Circular CCW         Volume down
Swipe Up (edge)      Volume up
Swipe Down (edge)    Volume down
```

### **Advanced Controls**
```
Gesture              Action
-------              ------
Long Press + Swipe   Fast forward/rewind
Two-Finger Swipe     Skip 10 seconds
Pinch In             Minimize player
Pinch Out            Full screen
```

## 📐 **Gesture Detection Algorithm**

### **How It Works**
```
1. Touch Start: Record first touched key and timestamp
2. Touch Move: Track sequence of touched keys
3. Touch End: Analyze pattern and duration
4. Classify: Determine gesture type
5. Execute: Perform corresponding action
```

### **Key Detection Parameters**
```cpp
// Timing thresholds
TAP_THRESHOLD = 200ms        // Max time for tap
LONG_PRESS_THRESHOLD = 500ms // Min time for long press
DOUBLE_TAP_THRESHOLD = 300ms // Max time between taps

// Movement thresholds
SWIPE_MIN_KEYS = 2          // Minimum keys for swipe
SCROLL_THRESHOLD = 3        // Keys for scroll detection

// Touch history
TOUCH_HISTORY_SIZE = 10     // Remember last 10 touches
```

## 🎯 **Gesture Patterns**

### **Keypad Layout Reference**
```
┌────┬────┬────┬────┐
│ 1  │ 2  │ 3  │ 4  │  Row 0
├────┼────┼────┼────┤
│ 5  │ 6  │ 7  │ 8  │  Row 1
├────┼────┼────┼────┤
│ 9  │ 10 │ 11 │ 12 │  Row 2
├────┼────┼────┼────┤
│ 13 │ 14 │ 15 │ 16 │  Row 3
└────┴────┴────┴────┘
 Col0 Col1 Col2 Col3
```

### **Circular Scroll Paths**
```
Outer Ring (Clockwise):
1 → 2 → 3 → 4 → 8 → 12 → 16 → 15 → 14 → 13 → 9 → 5 → 1

Outer Ring (Counter-Clockwise):
1 → 5 → 9 → 13 → 14 → 15 → 16 → 12 → 8 → 4 → 3 → 2 → 1

Middle Ring (Clockwise):
6 → 7 → 11 → 10 → 6

Partial Arcs (for faster detection):
Top Arc: 1 → 2 → 3 → 4
Right Arc: 4 → 8 → 12 → 16
Bottom Arc: 16 → 15 → 14 → 13
Left Arc: 13 → 9 → 5 → 1
```

### **Swipe Patterns**
```
Vertical Swipes:
Up:   13→9→5→1, 14→10→6→2, 15→11→7→3, 16→12→8→4
Down: 1→5→9→13, 2→6→10→14, 3→7→11→15, 4→8→12→16

Horizontal Swipes:
Left:  4→3→2→1, 8→7→6→5, 12→11→10→9, 16→15→14→13
Right: 1→2→3→4, 5→6→7→8, 9→10→11→12, 13→14→15→16

Diagonal Swipes:
↗: 13→10→7→4, 9→6→3
↖: 16→11→6→1, 12→7→2
↘: 1→6→11→16, 2→7→12
↙: 4→7→10→13, 3→6→9
```

## 💻 **Implementation Tips**

### **Improving Gesture Recognition**
```cpp
// 1. Increase touch history size for complex gestures
#define TOUCH_HISTORY_SIZE 15

// 2. Add velocity detection
float calculateSwipeVelocity() {
  // Distance / Time
  return touchDistance / touchDuration;
}

// 3. Add confidence scoring
float gestureConfidence = 0.0;
if (matchesPattern) confidence += 0.3;
if (correctTiming) confidence += 0.3;
if (smoothPath) confidence += 0.4;

// 4. Implement gesture cancellation
if (touchDuration > MAX_GESTURE_TIME) {
  cancelGesture();
}
```

### **Reducing False Positives**
```cpp
// 1. Minimum movement threshold
if (uniqueKeys < 2) {
  // Not a swipe, just a tap
  return GESTURE_TAP;
}

// 2. Direction consistency check
if (hasDirectionChange()) {
  // Erratic movement, ignore
  return GESTURE_NONE;
}

// 3. Timing validation
if (touchDuration < MIN_SWIPE_TIME) {
  // Too fast, probably accidental
  return GESTURE_NONE;
}
```

### **Optimizing Performance**
```cpp
// 1. Early exit for simple gestures
if (duration < TAP_THRESHOLD && touchCount == 1) {
  return GESTURE_TAP; // No need for complex analysis
}

// 2. Cache calculations
static int lastGestureType = GESTURE_NONE;
static unsigned long lastGestureTime = 0;

// 3. Limit history analysis
// Only analyze last N touches, not entire history
int recentTouches = min(touchHistoryIndex, 5);
```

## 🎨 **Visual Feedback**

### **Gesture Indicators**
```cpp
// Show gesture trail
void drawGestureTrail() {
  for (int i = 0; i < touchHistoryIndex - 1; i++) {
    int key1 = touchHistory[i];
    int key2 = touchHistory[i + 1];
    
    // Draw line between keys
    drawLineBetweenKeys(key1, key2, YELLOW);
  }
}

// Show gesture name
void showGestureName(GestureType gesture) {
  const char* names[] = {
    "Tap", "Double Tap", "Hold",
    "↑", "↓", "←", "→",
    "⟳", "⟲", "✌"
  };
  
  displayText(names[gesture], CENTER, YELLOW);
}
```

### **Touch Visualization**
```cpp
// Highlight touched keys
void highlightTouchedKeys(uint16_t status) {
  for (int i = 0; i < 16; i++) {
    if ((status >> i) & 0x01) {
      // Draw highlight on display
      int x = (i % 4) * 60;
      int y = (i / 4) * 60;
      tft.fillCircle(x + 30, y + 30, 10, YELLOW);
    }
  }
}
```

## 🚀 **Advanced Features**

### **Custom Gestures**
```cpp
// Define your own gesture patterns
struct CustomGesture {
  int pattern[10];      // Key sequence
  int patternLength;    // Number of keys
  int maxDuration;      // Max time (ms)
  void (*action)();     // Function to call
};

// Example: "Z" pattern
CustomGesture zPattern = {
  {1, 2, 3, 4, 7, 10, 13, 14, 15, 16},
  10,
  2000,
  &handleZGesture
};
```

### **Gesture Macros**
```cpp
// Combine gestures for complex actions
void handleGestureMacro() {
  if (lastGesture == GESTURE_SWIPE_UP &&
      currentGesture == GESTURE_SWIPE_DOWN) {
    // Up-Down combo: Refresh
    refreshScreen();
  }
}
```

### **Adaptive Sensitivity**
```cpp
// Adjust thresholds based on user behavior
void adaptSensitivity() {
  if (falsePositiveRate > 0.3) {
    TAP_THRESHOLD -= 10;  // Make stricter
  }
  if (missedGestureRate > 0.3) {
    TAP_THRESHOLD += 10;  // Make more lenient
  }
}
```

## 📊 **Gesture Statistics**

### **Tracking Usage**
```cpp
struct GestureStats {
  int tapCount;
  int swipeCount;
  int scrollCount;
  float avgGestureDuration;
  int falsePositives;
};

void updateStats(GestureType gesture, float duration) {
  stats.totalGestures++;
  stats.avgGestureDuration = 
    (stats.avgGestureDuration * (stats.totalGestures - 1) + duration) 
    / stats.totalGestures;
}
```

## 🎯 **Best Practices**

### **For Users**
```
✅ Use smooth, deliberate motions
✅ Complete gestures fully
✅ Wait for feedback before next gesture
✅ Keep fingers dry for best sensitivity
✅ Practice circular scrolling for iPod feel
```

### **For Developers**
```
✅ Provide clear visual feedback
✅ Add haptic feedback if available
✅ Implement gesture cancellation
✅ Test with different hand sizes
✅ Allow gesture customization
✅ Log gesture data for improvement
```

## 🔧 **Troubleshooting**

### **Gestures Not Detected**
```
Problem: Swipes not recognized
Solutions:
1. Increase TOUCH_HISTORY_SIZE
2. Reduce SWIPE_MIN_KEYS threshold
3. Slow down gesture speed
4. Check touch pad sensitivity
```

### **False Gesture Detection**
```
Problem: Accidental gestures triggered
Solutions:
1. Increase timing thresholds
2. Add minimum movement distance
3. Implement gesture confirmation
4. Add "dead zones" between keys
```

### **Inconsistent Recognition**
```
Problem: Same gesture gives different results
Solutions:
1. Normalize touch patterns
2. Add fuzzy matching
3. Increase confidence threshold
4. Smooth touch history data
```

This gesture system transforms your TTP229 module into a **powerful touch interface** that rivals commercial products! The circular scroll gesture especially gives you that authentic iPod wheel feel. 🎵

Upload `src/ttp229_gesture_ipod.cpp` to try it out!