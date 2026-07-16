# Complete Hardware Shopping List

## Core Components

### 1. **ESP32-S3 Development Board**
- **Part**: ESP32-S3-DevKitC-1 (16MB Flash, 8MB PSRAM)
- **Price**: ₹800-1200
- **Where**: Amazon, Robu.in, ElectronicsComp
- **Notes**: Make sure it has PSRAM for smooth UI

### 2. **Display Module** ✅ (You have this)
- **Part**: 2.8" ILI9341 TFT Display (240x320)
- **Price**: ₹600-900 (already purchased)
- **Notes**: Your MSP2806 module

### 3. **MicroSD Card Module**
- **Part**: MicroSD Card Reader Module (SPI)
- **Price**: ₹80-150
- **Where**: Amazon, local electronics
- **Notes**: Get one with 3.3V logic level

### 4. **MicroSD Card**
- **Part**: 256GB MicroSD Card (Class 10, U3)
- **Price**: ₹1200-1800
- **Where**: Amazon, Flipkart
- **Brands**: SanDisk, Samsung, Kingston

### 5. **Power & Audio Only**
- **Note**: No additional input components needed!
- **Your display**: Already has built-in touch controller
- **Interface**: 100% touchscreen - no buttons, no encoder, no extra modules

## Power System

### 6. **18650 Battery**
- **Part**: 18650 Li-ion Battery (3500mAh, Protected)
- **Price**: ₹400-600
- **Where**: Amazon, battery stores
- **Brands**: Panasonic, Samsung, LG

### 7. **18650 Battery Holder**
- **Part**: 18650 Single Cell Holder with Wires
- **Price**: ₹50-100
- **Where**: Amazon, local electronics

### 8. **USB-C Charging Module**
- **Part**: TP4056 USB-C Charging Board with Protection
- **Price**: ₹150-250
- **Where**: Amazon, AliExpress
- **Notes**: Must have over-discharge protection

## USB-C Audio System (Replaces 3.5mm jack)

### 9. **USB-C Connector Breakout**
- **Part**: USB-C Female Breakout Board
- **Price**: ₹100-200
- **Where**: Amazon, AliExpress
- **Notes**: Need access to D+, D-, VBUS, GND, CC pins

### 10. **USB Audio DAC IC**
- **Part**: PCM2912A USB Audio Codec IC
- **Price**: ₹300-500
- **Where**: Mouser, DigiKey, AliExpress
- **Alternative**: Use USB-C to 3.5mm DAC dongle (₹200-400)

### 11. **Audio Amplifier** (Optional - for built-in speaker)
- **Part**: MAX98357A I2S Audio Amplifier
- **Price**: ₹200-300
- **Where**: Amazon, AliExpress
- **Notes**: Only if you want internal speaker

## Additional Components

### 12. **Voltage Regulators**
- **Part**: AMS1117-3.3V Linear Regulator
- **Price**: ₹30-50 each
- **Quantity**: 2 pieces
- **Where**: Local electronics, Amazon

### 13. **Capacitors**
- **Parts**: 
  - 100nF Ceramic (10 pieces) - ₹20
  - 10µF Electrolytic (5 pieces) - ₹30
  - 22µF Electrolytic (2 pieces) - ₹20
- **Total**: ₹70
- **Where**: Local electronics

### 14. **Resistors**
- **Parts**: 
  - 10kΩ (5 pieces) - Pull-ups
  - 4.7kΩ (2 pieces) - I2C pull-ups
  - 1kΩ (3 pieces) - LEDs
- **Price**: ₹30-50
- **Where**: Local electronics

### 15. **Breadboard/PCB**
- **Option A**: Large Breadboard (₹200-300)
- **Option B**: Perfboard/Veroboard (₹100-150)
- **Option C**: Custom PCB (₹500-1000)

### 16. **Jumper Wires**
- **Part**: Male-Male, Male-Female Jumper Wires
- **Price**: ₹100-200
- **Where**: Amazon, local electronics

### 17. **Case/Enclosure**
- **Option A**: 3D Print Custom Case (₹200-400)
- **Option B**: Plastic Project Box (₹150-300)
- **Where**: Local electronics, 3D printing service

## USB-C Audio Options

### **Option 1: Simple USB-C DAC Dongle (Recommended)**
- **Part**: USB-C to 3.5mm DAC Adapter
- **Price**: ₹200-400
- **Pros**: Plug and play, high quality, compact
- **Cons**: External dongle required

### **Option 2: Integrated USB-C Audio**
- **Parts**: PCM2912A + USB-C connector + supporting components
- **Price**: ₹600-1000
- **Pros**: Built-in, professional
- **Cons**: Complex circuit, harder to implement

## Total Cost Breakdown

### **Minimalist Version (₹2,600-3,300)**
```
ESP32-S3 DevKit:        ₹800
Display (owned):        ₹0    ← Your MSP2806 with built-in touch
SD Module:              ₹100
256GB SD Card:          ₹1200
18650 Battery:          ₹500
Battery Holder:         ₹80
USB-C Charging:         ₹200
USB-C DAC Dongle:       ₹300
Components:             ₹200
Breadboard:             ₹200
Case:                   ₹200
------------------------
Total:                  ₹3,580
```

### **Premium Version (₹4,000-5,000)**
```
All above components plus:
Custom PCB:             ₹800
Premium case:           ₹400
Internal speaker:       ₹300
Premium battery:        ₹200
Wireless charging:      ₹400
------------------------
Total:                  ₹5,680
```

## Shopping Strategy

### **Phase 1: Core Functionality (₹1,300)**
1. ESP32-S3 DevKit
2. SD Card module + card
3. Basic components
4. Your display already has touch!

### **Phase 2: Power System (₹800)**
1. 18650 battery + holder
2. USB-C charging module
3. Voltage regulators

### **Phase 3: Audio & Case (₹600)**
1. USB-C DAC solution
2. Case/enclosure
3. Final assembly

## Where to Buy (India)

### **Online Stores**
- **Amazon India**: ESP32, SD cards, batteries, cases
- **Robu.in**: ESP32 modules, sensors, development boards
- **ElectronicsComp**: Electronic components, ICs
- **Flipkart**: SD cards, accessories
- **AliExpress**: Specialized modules (longer shipping)

### **Local Stores**
- **SP Road, Bangalore**: Electronics components
- **Lamington Road, Mumbai**: Electronics market
- **Nehru Place, Delhi**: Electronics hub
- **Ritchie Street, Chennai**: Electronics components

### **Specialized Suppliers**
- **Mouser/DigiKey**: High-quality ICs (for PCM2912A)
- **Element14**: Professional components
- **Digi-Key India**: Electronic components

## USB-C Audio Implementation

### **Simple Approach (Recommended)**
```
ESP32-S3 → USB-C Port (Data) → USB-C DAC Dongle → Headphones
         ↓
    USB-C Port (Power) → Charging Circuit → Battery
```

### **Advanced Approach**
```
ESP32-S3 → USB-C Controller → Audio DAC → Headphones
         ↓                 ↓
    Power Management ← USB-C PD Controller
```

The simple approach using a USB-C DAC dongle is much easier to implement and gives you professional audio quality immediately!