# Rotary Encoder iPod Interface - Wiring Guide

## 🎮 **Classic iPod-Style Navigation**

Instead of touch, we're using the authentic iPod approach with rotary encoder + buttons for reliable, tactile navigation.

## 🔌 **Required Components**

### **1. Rotary Encoder (KY-040 or similar)**
- **CLK** - Clock signal
- **DT** - Data signal  
- **SW** - Switch (center button)
- **VCC** - 3.3V power
- **GND** - Ground

### **2. Push Buttons (4x)**
- Menu button (back/up navigation)
- Play/Pause button
- Previous track button
- Next track button

## 📋 **Complete Wiring Diagram**

### **ESP32-S3 Connections:**

```
ESP32-S3    →    Component
--------         ---------

Display (ILI9341):
GPIO10      →    CS
GPIO9       →    DC
GPIO8       →    RST
GPIO11      →    SDA/MOSI
GPIO12      →    SCL/SCLK
GPIO13      →    SDO/MISO
3.3V        →    VCC
3.3V        →    LED
GND         →    GND

Rotary Encoder (KY-040):
GPIO0       →    CLK
GPIO1       →    DT
GPIO2       →    SW (center button)
3.3V        →    VCC
GND         →    GND

Navigation Buttons:
GPIO14      →    Menu Button (one side)
GPIO15      →    Play Button (one side)
GPIO16      →    Previous Button (one side)
GPIO17      →    Next Button (one side)
GND         →    All buttons (other side)
```

## 🔧 **Breadboard Layout**

```
ESP32-S3 DevKit Layout:

     [USB-C]
   ┌─────────────┐
   │ 3V3     GND │ ← Power rails
   │ EN      IO0 │ ← Encoder CLK
   │ IO36    IO1 │ ← Encoder DT
   │ IO37    IO2 │ ← Encoder SW
   │ IO38    IO3 │
   │ IO39    IO4 │
   │ IO40    IO5 │
   │ IO41    IO6 │
   │ IO42    IO7 │
   │ RXD0    IO8 │ ← Display RST
   │ TXD0    IO9 │ ← Display DC
   │ IO44   IO10 │ ← Display CS
   │ IO43   IO11 │ ← Display MOSI
   │ IO44   IO12 │ ← Display SCLK
   │ IO45   IO13 │ ← Display MISO
   │ IO46   IO14 │ ← Menu Button
   │ IO47   IO15 │ ← Play Button
   │ IO48   IO16 │ ← Previous Button
   │        IO17 │ ← Next Button
   │        IO18 │
   │        5V   │
   │        GND  │
   └─────────────┘
```

## 🎯 **Component Details**

### **Rotary Encoder (KY-040)**
```
Encoder Pinout:
┌─────────────┐
│  CLK  DT    │
│             │
│  SW   VCC   │
│             │
│      GND    │
└─────────────┘

Connections:
CLK → ESP32-S3 GPIO0
DT  → ESP32-S3 GPIO1  
SW  → ESP32-S3 GPIO2
VCC → ESP32-S3 3.3V
GND → ESP32-S3 GND
```

### **Push Buttons**
```
Button Wiring (x4):
┌─────────────┐
│    Button   │
│  ┌───────┐  │
│  │       │  │
│  └───────┘  │
└──┬─────┬────┘
   │     │
   │     └─── GND
   └───────── GPIO Pin

Menu Button:    GPIO14 → Button → GND
Play Button:    GPIO15 → Button → GND  
Previous:       GPIO16 → Button → GND
Next Button:    GPIO17 → Button → GND
```

## 🎮 **iPod Navigation Controls**

### **Rotary Encoder:**
- **Rotate Clockwise** → Navigate down/right
- **Rotate Counter-clockwise** → Navigate up/left
- **Press Center** → Select/OK

### **Buttons:**
- **Menu** → Go back/up one level
- **Play** → Play/Pause current track
- **Previous** → Previous track (or navigate up)
- **Next** → Next track (or navigate down)

## 🔄 **How It Works**

### **Navigation Flow:**
```
Home Screen
├── Music Library (rotary scroll through songs)
│   └── Select song → Now Playing
├── Now Playing (show current track)
├── Settings (rotary scroll through options)
└── About

Controls:
- Rotary: Scroll through menu items
- Center: Select highlighted item
- Menu: Go back to previous screen
- Play: Toggle play/pause
- Prev/Next: Change tracks or navigate
```

### **Encoder Behavior:**
- **Smooth scrolling** through menu items
- **Wrap-around** navigation (top/bottom)
- **Interrupt-driven** for responsive feel
- **Debounced** to prevent false triggers

## 🛠️ **Assembly Steps**

### **1. Connect Display (same as before)**
Wire the ILI9341 display as previously configured.

### **2. Connect Rotary Encoder**
```
1. Connect encoder VCC to ESP32-S3 3.3V
2. Connect encoder GND to ESP32-S3 GND
3. Connect encoder CLK to GPIO0
4. Connect encoder DT to GPIO1
5. Connect encoder SW to GPIO2
```

### **3. Connect Buttons**
```
1. Connect one side of each button to GND
2. Connect other sides to respective GPIO pins:
   - Menu: GPIO14
   - Play: GPIO15
   - Previous: GPIO16
   - Next: GPIO17
```

### **4. Test Setup**
Upload `src/ipod_rotary_interface.cpp` and test:
- Rotate encoder → Menu items should scroll
- Press center → Should select items
- Press buttons → Should trigger respective actions

## 📊 **Advantages of Rotary Interface**

### **vs Touch Screen:**
- ✅ **More reliable** - No calibration issues
- ✅ **Tactile feedback** - Feel the clicks
- ✅ **Authentic iPod experience** - Classic navigation
- ✅ **Works in all conditions** - No touch sensitivity issues
- ✅ **Lower power** - No touch controller needed
- ✅ **Easier to use** - Physical buttons are intuitive

### **Classic iPod Feel:**
- ✅ **Scroll wheel navigation** - Familiar interface
- ✅ **Physical buttons** - Menu, Play, Prev, Next
- ✅ **Responsive scrolling** - Smooth menu navigation
- ✅ **Visual feedback** - Highlighted selections

## 🎵 **Usage Examples**

### **Playing Music:**
1. **Rotate encoder** → Scroll to "Music Library"
2. **Press center** → Enter library
3. **Rotate encoder** → Scroll through songs
4. **Press center** → Select song (goes to Now Playing)
5. **Press Play button** → Start/pause music

### **Navigation:**
1. **Menu button** → Always goes back/up
2. **Rotary encoder** → Scroll through current menu
3. **Center button** → Select highlighted item
4. **Prev/Next buttons** → Quick track control

This gives you a **fully functional, authentic iPod experience** with reliable physical controls! 🎉

## 🔧 **Troubleshooting**

### **Encoder Not Working:**
- Check CLK/DT connections
- Verify 3.3V power to encoder
- Test with multimeter for continuity

### **Buttons Not Responding:**
- Check GND connections
- Verify GPIO pin assignments
- Test individual buttons with multimeter

### **Erratic Scrolling:**
- Add 0.1µF capacitors across encoder pins
- Check for loose breadboard connections
- Verify interrupt setup in code

The rotary encoder approach is much more reliable and gives you that authentic iPod feel! 🎮