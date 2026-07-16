# Single USB-C Port Design (Option 2)

## Overview

Single USB-C port that automatically detects and switches between:
- **Charging Mode**: When power adapter is connected
- **Audio Mode**: When DAC dongle or USB-C headphones are connected
- **Data Mode**: When connected to computer (for file transfer)

## Hardware Components Needed

### **USB-C Detection & Switching**
1. **USB-C Connector with CC pins** - ₹150
2. **USB-C PD Controller** (IP2723T or similar) - ₹400
3. **Analog Switch IC** (74HC4053 or similar) - ₹80
4. **Current Sense Resistor** (0.1Ω) - ₹20

### **Power Management**
1. **TP4056 Charging Circuit** - ₹170
2. **Boost Converter** (5V output for USB host) - ₹150
3. **Load Switch** (TPS2115A) - ₹100

### **Total Additional Cost: ₹1,070**

## How It Works

### **Connection Detection**
```
USB-C Device Connected
         ↓
Check CC Pin Voltage
         ↓
┌────────┼────────┐
│        │        │
Power    Audio    Data
Adapter  Device   Cable
│        │        │
5V/9V    5V       5V
High     Low      Medium
Current  Current  Current
```

### **Automatic Switching Logic**
```cpp
void detectUSBDevice() {
    float ccVoltage = readCCPin();
    float busCurrent = readCurrent();
    
    if (ccVoltage > 2.0V && busCurrent > 500mA) {
        // Power adapter detected
        switchToChargingMode();
    } else if (ccVoltage < 1.0V && busCurrent < 100mA) {
        // Audio device detected  
        switchToAudioMode();
    } else {
        // Data cable or unknown device
        switchToDataMode();
    }
}
```

## Circuit Design

### **USB-C Connector Pinout**
```
Pin     Function        ESP32-S3 Connection
A1/B12  GND            GND
A4/B9   VBUS           Power Management Circuit
A5      CC1            GPIO23 (ADC for detection)
A6/B7   D+ (USB2.0)    GPIO18 (via switch)
A7/B6   D- (USB2.0)    GPIO19 (via switch)  
B5      CC2            GPIO24 (ADC for detection)
A1/B12  GND            GND
```

### **Power Path Management**
```
USB-C VBUS → Load Switch → ┌→ TP4056 → Battery
                          └→ Boost → 5V Rail → ESP32
```

### **Data Path Switching**
```
USB-C D+/D- → Analog Switch → ┌→ ESP32 USB Host (Audio Mode)
                              └→ ESP32 USB Device (Data Mode)
```

## Software Implementation

### **Updated Config**
```cpp
// USB-C Detection Pins
#define USB_CC1_ADC 23
#define USB_CC2_ADC 24
#define USB_CURRENT_SENSE 25
#define USB_SWITCH_CTRL 26
#define USB_POWER_ENABLE 27

// USB Data Pins (switched)
#define USB_DP 18
#define USB_DN 19
```

### **USB Manager Class**
```cpp
class USBManager {
public:
    enum USBMode {
        DISCONNECTED,
        CHARGING,
        AUDIO_HOST,
        DATA_DEVICE
    };
    
    void init();
    void update();
    USBMode detectConnectedDevice();
    void switchMode(USBMode mode);
    
private:
    USBMode currentMode;
    unsigned long lastDetection;
};
```

## Updated Shopping List

### **Modified Components for Single USB-C**

#### **USB-C System (₹1,070)**
- **USB-C Connector Breakout** (with CC pins) - ₹150
- **USB-C PD Controller IC** (IP2723T) - ₹400
- **Analog Switch IC** (74HC4053) - ₹80
- **Load Switch IC** (TPS2115A) - ₹100
- **Boost Converter Module** (3.3V→5V) - ₹150
- **Current Sense Resistor** + supporting components - ₹50
- **PCB/Perfboard** for USB-C circuit - ₹140

#### **Other Components (Same as before)**
- **MicroSD Card + Module** - ₹1,300
- **18650 Battery + Holder** - ₹580
- **Breadboard + Wires** - ₹250
- **Case** - ₹200

### **Total Cost: ₹3,400**

## Advantages of Single USB-C

✅ **Sleek Design** - Only one port, very clean
✅ **Automatic Detection** - No manual switching needed
✅ **Professional** - Like modern smartphones/tablets
✅ **Space Efficient** - Smaller case possible
✅ **Future Proof** - USB-C is the standard

## Disadvantages

❌ **Cannot charge while using wired audio**
❌ **More complex circuit** - Higher chance of issues
❌ **Higher cost** - Additional switching components
❌ **Harder to debug** - More complex failure modes

## Implementation Phases

### **Phase 1: Basic USB-C Charging**
- Implement simple USB-C charging first
- Test power management circuit
- Verify battery charging works

### **Phase 2: Add Detection Logic**
- Add CC pin voltage monitoring
- Implement device detection algorithm
- Test with different USB-C devices

### **Phase 3: Add Audio Switching**
- Implement USB host mode switching
- Add audio device enumeration
- Test with USB-C DAC dongles

### **Phase 4: Polish & Optimization**
- Add smooth mode transitions
- Implement proper USB-C PD negotiation
- Add status LEDs and UI feedback

## Alternative: Simplified Single USB-C

If the full switching seems too complex, here's a simpler approach:

### **Manual Mode Selection**
```cpp
// User selects mode via UI
if (userSelectedAudioMode) {
    enableUSBHost();
    disableCharging();
} else {
    enableCharging();  
    disableUSBHost();
}
```

This gives you single USB-C functionality with manual switching through the UI, which is much simpler to implement but still provides the clean single-port design.

Would you like me to start with the simplified version first, or go straight to the full automatic detection system?