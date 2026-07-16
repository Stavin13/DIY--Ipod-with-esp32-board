# Breadboard Touch Test Setup

## Components Needed

### **Hardware**
- ESP32-S3 Development Board
- 2.8" ILI9341 TFT Display (your MSP2806 module)
- Large breadboard (830 points)
- Jumper wires (Male-Male, Male-Female)
- USB-C cable for ESP32 power

### **Software**
- PlatformIO or Arduino IDE
- Touch test code (provided)

## Pin Connections

### **Display Connections (SPI)**
```
ESP32-S3 Pin    Display Pin     Wire Color (Suggested)
GPIO10      →   CS              Orange
GPIO9       →   DC              Yellow  
GPIO8       →   RST             White
GPIO11      →   MOSI (SDI)      Blue
GPIO12      →   SCLK (SCK)      Green
GPIO13      →   MISO (SDO)      Purple
3.3V        →   VCC             Red
GND         →   GND             Black
GPIO2       →   LED (optional)  Gray
```

### **Touch Connections (SPI)**
```
ESP32-S3 Pin    Touch Pin       Wire Color (Suggested)
GPIO6       →   T_CS            Brown
GPIO7       →   T_CLK           Pink
GPIO5       →   T_DIN           Cyan
GPIO4       →   T_DO            Magenta
GPIO3       →   T_IRQ           Light Blue
3.3V        →   VCC (shared)    Red
GND         →   GND (shared)    Black
```

## Breadboard Layout Diagram

```
ESP32-S3 Development Board
┌─────────────────────────────────────┐
│ 3V3  EN  36  39  34  35  32  33  25 │
│ GND  23  22  21  19  18  5   17  16 │
│ 15   2   0   4   16  17  5   18  19 │
│ 21   RX  TX  22  23  EN  3V3 GND    │
└─────────────────────────────────────┘
     │   │   │   │   │   │   │   │
     │   │   │   │   │   │   │   └─── GPIO19
     │   │   │   │   │   │   └─────── GPIO18  
     │   │   │   │   │   └─────────── GPIO5
     │   │   │   │   └─────────────── GPIO17
     │   │   │   └─────────────────── GPIO4
     │   │   └─────────────────────── GPIO0
     │   └─────────────────────────── GPIO2
     └─────────────────────────────── GPIO15

Display Module (Back View)
┌─────────────────────────────────────┐
│ VCC GND CS  RST DC  MOSI SCLK MISO │ ← Display pins
│ VCC GND T_CS T_IRQ T_DIN T_DO T_CLK │ ← Touch pins  
└─────────────────────────────────────┘
```

## Step-by-Step Wiring

### **Step 1: Power Connections**
```
1. Connect ESP32 3.3V → Breadboard positive rail (red)
2. Connect ESP32 GND → Breadboard negative rail (black)
3. Connect Display VCC → Breadboard positive rail
4. Connect Display GND → Breadboard negative rail
```

### **Step 2: Display SPI Connections**
```
ESP32-S3        Breadboard      Display
GPIO10     →    Row 10     →    CS
GPIO9      →    Row 9      →    DC
GPIO8      →    Row 8      →    RST
GPIO11     →    Row 11     →    MOSI
GPIO12     →    Row 12     →    SCLK
GPIO13     →    Row 13     →    MISO
```

### **Step 3: Touch SPI Connections**
```
ESP32-S3        Breadboard      Touch
GPIO6      →    Row 6      →    T_CS
GPIO7      →    Row 7      →    T_CLK
GPIO5      →    Row 5      →    T_DIN
GPIO4      →    Row 4      →    T_DO
GPIO3      →    Row 3      →    T_IRQ
```

## Physical Breadboard Layout

### **Breadboard Sections**
```
Power Rails (Top):
[+] ←── 3.3V from ESP32
[-] ←── GND from ESP32

Main Area:
Rows 1-30: ESP32-S3 connections
Rows 31-60: Display module connections  
Rows 61-65: Touch connections

Power Rails (Bottom):
[+] ←── Connected to top positive
[-] ←── Connected to top negative
```

### **Wire Management Tips**
1. **Use different colors** for different signal types:
   - Red: Power (3.3V)
   - Black: Ground
   - Blue/Green: SPI signals
   - Yellow/Orange: Control signals

2. **Keep wires short** to reduce noise
3. **Route power first**, then signals
4. **Double-check connections** before powering on

## Testing Procedure

### **Step 1: Visual Inspection**
```
☐ All power connections secure
☐ No short circuits (red to black)
☐ All signal wires connected
☐ Display module properly seated
☐ ESP32 properly connected
```

### **Step 2: Power Test**
```
1. Connect USB-C to ESP32
2. Check for power LED on ESP32
3. Check for power LED on display (if present)
4. Measure 3.3V on breadboard rails with multimeter
```

### **Step 3: Display Test**
```
1. Upload basic display test code first
2. Should see display initialize
3. Look for colored screen or test pattern
4. If display works, proceed to touch test
```

### **Step 4: Touch Test**
```
1. Upload touch test code (touch_test.cpp)
2. Open Serial Monitor (115200 baud)
3. Touch different areas of screen
4. Watch for coordinate changes in serial output
```

## Expected Serial Output

### **Touch Working (Success)**
```
Touch Test Starting...
Touch pins initialized
Touch the screen to test...
IRQ: RELEASED, X:  200, Y:  300
IRQ: PRESSED, X: 2150, Y: 1800 -> TOUCH DETECTED! Screen coords: X=120, Y=160
IRQ: PRESSED, X: 3200, Y: 2900 -> TOUCH DETECTED! Screen coords: X=200, Y=280
IRQ: RELEASED, X:  180, Y:  250
```

### **Touch Not Working (Failure)**
```
Touch Test Starting...
Touch pins initialized
Touch the screen to test...
IRQ: RELEASED, X:    0, Y:    0
IRQ: RELEASED, X:    0, Y:    0
IRQ: RELEASED, X: 4095, Y: 4095
IRQ: RELEASED, X: 4095, Y: 4095
```

## Troubleshooting

### **Display Not Working**
```
Problem: Black screen
Solutions:
☐ Check power connections (3.3V, GND)
☐ Verify SPI pin connections
☐ Check RST pin connection
☐ Try different jumper wires
☐ Measure voltage at display VCC pin
```

### **Touch Not Responding**
```
Problem: No coordinate changes
Solutions:
☐ Verify touch pin connections (T_CS, T_CLK, T_DIN, T_DO)
☐ Check if T_IRQ pin changes when touching
☐ Try different pressure when touching
☐ Verify 3.3V power to touch controller
☐ Check for loose connections
```

### **Erratic Touch Readings**
```
Problem: Random coordinates
Solutions:
☐ Add 100nF capacitor between 3.3V and GND near display
☐ Keep wires short and away from power cables
☐ Check for loose breadboard connections
☐ Try different touch pins if available
```

## Connection Verification Checklist

### **Before Powering On**
```
☐ ESP32-S3 3.3V → Breadboard positive rail
☐ ESP32-S3 GND → Breadboard negative rail
☐ Display VCC → Breadboard positive rail
☐ Display GND → Breadboard negative rail
☐ All 6 display SPI pins connected correctly
☐ All 5 touch SPI pins connected correctly
☐ No short circuits between power rails
☐ All connections tight and secure
```

### **After Powering On**
```
☐ ESP32 power LED on
☐ Display shows some activity (backlight, pixels)
☐ Serial monitor shows "Touch Test Starting..."
☐ No smoke or burning smell
☐ 3.3V measured on breadboard rails
```

## Quick Test Code

### **Minimal Touch Test (Copy to Arduino IDE)**
```cpp
// Quick touch test - paste into Arduino IDE
#define TOUCH_CS 6
#define TOUCH_CLK 7
#define TOUCH_DIN 5
#define TOUCH_DO 4

void setup() {
  Serial.begin(115200);
  pinMode(TOUCH_CS, OUTPUT);
  pinMode(TOUCH_CLK, OUTPUT);
  pinMode(TOUCH_DIN, OUTPUT);
  pinMode(TOUCH_DO, INPUT);
  digitalWrite(TOUCH_CS, HIGH);
  Serial.println("Touch test ready - touch screen now");
}

void loop() {
  uint16_t x = readTouch(0x90);
  uint16_t y = readTouch(0xD0);
  Serial.printf("X: %4d, Y: %4d\n", x, y);
  delay(100);
}

uint16_t readTouch(uint8_t cmd) {
  digitalWrite(TOUCH_CS, LOW);
  // Send command
  for(int i=7; i>=0; i--) {
    digitalWrite(TOUCH_DIN, (cmd>>i)&1);
    digitalWrite(TOUCH_CLK, HIGH);
    digitalWrite(TOUCH_CLK, LOW);
  }
  // Read result
  uint16_t result = 0;
  for(int i=11; i>=0; i--) {
    digitalWrite(TOUCH_CLK, HIGH);
    if(digitalRead(TOUCH_DO)) result |= (1<<i);
    digitalWrite(TOUCH_CLK, LOW);
  }
  digitalWrite(TOUCH_CS, HIGH);
  return result;
}
```

## Success Criteria

### **Touch is Working If:**
✅ X values change from ~200 to ~3800 when touching left to right
✅ Y values change from ~200 to ~3800 when touching top to bottom  
✅ Values are stable when not touching
✅ IRQ pin goes LOW when screen is pressed (if connected)

### **Touch is NOT Working If:**
❌ X and Y always read 0 or 4095
❌ Values don't change when touching different areas
❌ Random noise values that change constantly
❌ No response to any touch input

Once you confirm touch is working, you can enable it in the main iPod code by setting `TOUCH_ENABLED true` in config.h!

This breadboard setup will definitively tell you if your display's touch functionality is working or not.