# ESP32-S3 iPod PCB - Component Footprints and Part Numbers

## 🔧 **Component Specifications for PCB Design**

### **Microcontroller Module**

#### **ESP32-S3-WROOM-1 (U1)**
```
Package: SMD Module with castellated pads
Dimensions: 25.5mm x 18mm x 3.2mm
Footprint: ESP32-S3-WROOM-1 (custom)
Pins: 41 castellated pads
Pitch: 1.27mm

KiCad Footprint: RF_Module:ESP32-S3-WROOM-1
Alternative: Use socket for easy replacement

Supplier: 
- Mouser: 356-ESP32S3WROOM1N16
- DigiKey: 1965-ESP32-S3-WROOM-1-N16R8-ND
- Price: ₹800-1200
```

### **Power Management**

#### **AMS1117-3.3 Voltage Regulator (U2)**
```
Package: SOT-223
Input: 4.5V-7V (5V nominal)
Output: 3.3V, 1A max
Dropout: 1.3V typical

KiCad Footprint: Package_TO_SOT_SMD:SOT-223-3_TabPin2
Part Number: AMS1117-3.3
Supplier: 
- Amazon.in: ₹20-40 each
- Robu.in: ₹25-35 each
```

#### **Power Input Capacitors**
```
C1 (Input): 220μF, 16V Electrolytic
Package: Radial, 6.3mm diameter
KiCad Footprint: Capacitor_THT:CP_Radial_D6.3mm_P2.50mm

C2 (Output): 100μF, 10V Electrolytic  
Package: Radial, 5mm diameter
KiCad Footprint: Capacitor_THT:CP_Radial_D5.0mm_P2.00mm

Supplier: Any electronics store, ₹10-20 each
```

### **Haptic Driver**

#### **DRV2605L Haptic Motor Driver (U3)**
```
Package: TSSOP-20 (or use breakout board)
Dimensions: 6.5mm x 4.4mm
I2C Address: 0x5A

Option 1 - IC Direct:
KiCad Footprint: Package_SO:TSSOP-20_4.4x6.5mm_P0.65mm
Part Number: DRV2605LDGSR
Supplier: DigiKey, Mouser (₹400-600)

Option 2 - Breakout Board (Recommended):
Dimensions: 20mm x 15mm
KiCad Footprint: Custom breakout footprint
Supplier: Amazon.in, ElectronicsComp (₹800-1200)
```

### **Display Interface**

#### **Display Connector (J1)**
```
Type: 2.54mm pitch header, 1x9 pins
Gender: Male (PCB) to Female (display cable)
Package: Through-hole

KiCad Footprint: Connector_PinHeader_2.54mm:PinHeader_1x09_P2.54mm_Vertical
Part Number: Generic 1x9 pin header
Supplier: Robu.in, ₹10-20 each

Alternative: FFC Connector
Type: 0.5mm pitch, 9-pin FFC/FPC
KiCad Footprint: Connector_FFC-FPC:Hirose_FH12-9S-0.5SH_1x09-1MP_P0.50mm_Horizontal
```

### **User Interface**

#### **Tactile Push Buttons (SW1-SW4)**
```
Type: 6x6x5mm tactile switch
Actuation Force: 160gf
Travel: 0.25mm
Life: 1,000,000 cycles

KiCad Footprint: Button_Switch_THT:SW_PUSH_6mm_H5mm
Part Number: Generic 6x6x5mm tactile
Supplier: 
- Robu.in: ₹8-15 each
- Amazon.in: ₹10-20 each
Quantity needed: 4 pieces
```

### **Passive Components**

#### **Resistors (0805 SMD)**
```
Pull-up Resistors (R1-R4): 10kΩ, 0805, 1/8W
I2C Pull-ups (R5-R6): 4.7kΩ, 0805, 1/8W  
LED Current Limiting (R7-R8): 330Ω, 0805, 1/8W

KiCad Footprint: Resistor_SMD:R_0805_2012Metric
Tolerance: 5% (standard)
Supplier: 
- Amazon.in: ₹2-5 each
- ElectronicsComp: ₹1-3 each
- Buy in strips of 10-20 pieces
```

#### **Decoupling Capacitors**
```
C3-C6: 100nF (0.1μF), 0805, 50V Ceramic
Package: 0805 (2012 metric)
Dielectric: X7R or X5R
KiCad Footprint: Capacitor_SMD:C_0805_2012Metric

C7-C8: 10μF, 1206, 25V Tantalum
Package: 1206 (3216 metric)  
KiCad Footprint: Capacitor_SMD:C_1206_3216Metric

Supplier: 
- DigiKey.in: ₹5-15 each
- Mouser.in: ₹3-10 each
- Amazon.in: ₹10-25 each (kits)
```

### **Indicators**

#### **Status LEDs**
```
LED1 (Power): Green, 0805 SMD
LED2 (Status): Blue, 0805 SMD
Forward Voltage: 2.0-3.3V
Current: 20mA max

KiCad Footprint: LED_SMD:LED_0805_2012Metric
Part Numbers:
- Green: Generic 0805 green LED
- Blue: Generic 0805 blue LED
Supplier: 
- Amazon.in: ₹5-10 each
- ElectronicsComp: ₹3-8 each
```

### **Connectors**

#### **USB-C Connector (J2)**
```
Type: USB-C receptacle, SMD
Pins: USB 2.0 (4-pin data + power)
Package: SMD with through-hole mounting

KiCad Footprint: Connector_USB:USB_C_Receptacle_HRO_TYPE-C-31-M-12
Part Number: HRO TYPE-C-31-M-12 (or similar)
Supplier: 
- Amazon.in: ₹50-100 each
- AliExpress: ₹20-50 each
```

#### **LRA Connector (J3)**
```
Type: JST PH 2-pin connector
Pitch: 2.0mm
Package: Through-hole, right-angle

KiCad Footprint: Connector_JST:JST_PH_B2B-PH-K_1x02_P2.00mm_Vertical
Part Number: JST B2B-PH-K-S
Mating Connector: JST PHR-2 with wires
Supplier: 
- DigiKey.in: ₹15-25 each
- Amazon.in: ₹20-40 each
```

#### **Battery Connector (J4, Optional)**
```
Type: JST PH 2-pin connector  
Pitch: 2.0mm
Package: Through-hole, right-angle

KiCad Footprint: Connector_JST:JST_PH_B2B-PH-K_1x02_P2.00mm_Vertical
Same as LRA connector
Use for 3.7V LiPo battery connection
```

### **Mechanical Components**

#### **Mounting Holes**
```
Size: M2.5 (2.5mm diameter)
Hole diameter: 2.7mm (clearance)
Pad diameter: 4.5mm
Plated: Yes (grounded)

KiCad Footprint: MountingHole:MountingHole_2.7mm_M2.5_Pad_Via
Quantity: 4 corners
Spacing: 94mm x 54mm (to fit in case)
```

#### **Test Points**
```
Type: SMD test point pads
Diameter: 1.5mm
Package: Circular pad

KiCad Footprint: TestPoint:TestPoint_Pad_D1.5mm
Nets: +3V3, +5V, GND, I2C_SDA, I2C_SCL
Quantity: 5-7 test points
```

## 📐 **PCB Layout Specifications**

### **Board Dimensions**
```
Size: 100mm x 60mm
Thickness: 1.6mm (standard)
Layers: 2 (Top + Bottom)
Material: FR4
Copper weight: 1oz (35μm)
```

### **Design Rules**
```
Minimum trace width: 0.15mm (6mil)
Minimum via size: 0.2mm (8mil) 
Minimum drill: 0.1mm (4mil)
Minimum clearance: 0.15mm (6mil)
Annular ring: 0.05mm (2mil)
```

### **Recommended Values**
```
Signal traces: 0.2mm (8mil)
Power traces: 0.5mm (20mil)
Via size: 0.3mm (12mil)
Via drill: 0.15mm (6mil)
Clearance: 0.2mm (8mil)
```

## 🛒 **Complete BOM (Bill of Materials)**

### **Active Components**
```
U1: ESP32-S3-WROOM-1          1x    ₹800-1200
U2: AMS1117-3.3               1x    ₹25-40
U3: DRV2605L (breakout)       1x    ₹800-1200
```

### **Passive Components**
```
R1-R4: 10kΩ, 0805            4x    ₹2-5 each
R5-R6: 4.7kΩ, 0805           2x    ₹2-5 each  
R7-R8: 330Ω, 0805            2x    ₹2-5 each
C1: 220μF electrolytic        1x    ₹15-25
C2: 100μF electrolytic        1x    ₹10-20
C3-C6: 100nF, 0805           4x    ₹3-8 each
C7-C8: 10μF, 1206            2x    ₹8-15 each
```

### **Mechanical Components**
```
SW1-SW4: 6x6x5mm tactile     4x    ₹10-20 each
LED1: Green LED, 0805         1x    ₹5-10
LED2: Blue LED, 0805          1x    ₹5-10
J1: 1x9 pin header           1x    ₹10-20
J2: USB-C connector          1x    ₹50-100
J3: JST PH 2-pin             1x    ₹20-40
```

### **Total Component Cost**
```
Active components:     ₹1,625-2,440
Passive components:    ₹150-300
Mechanical components: ₹200-400
PCB fabrication:       ₹500-800 (5 pcs)

Total per board:       ₹500-800 (components only)
Total project cost:    ₹2,475-3,940 (5 complete PCBs)
```

## 🔧 **Assembly Notes**

### **SMD Component Placement**
```
Order of assembly:
1. Smallest components first (0805 resistors/caps)
2. Larger SMD components (regulators, LEDs)
3. Through-hole components (buttons, connectors)
4. Final inspection and testing
```

### **Soldering Tips**
```
SMD Components:
- Use flux for better wetting
- Temperature: 300-350°C
- Tip: Fine chisel or conical
- Solder: 0.6mm rosin core

Through-hole:
- Temperature: 350-400°C  
- Pre-tin pads and component leads
- Insert component, flip board, solder
```

### **Testing Procedure**
```
1. Visual inspection (shorts, cold joints)
2. Power-on test (check 3.3V rail)
3. Programming test (USB-C connection)
4. I2C communication (DRV2605L)
5. Button functionality
6. Display interface
7. Complete system test
```

This comprehensive component list gives you everything needed to create a professional PCB for your haptic iPod project! 🎯

The total cost per board is very reasonable (₹500-800 for components), and you'll get a much more reliable and professional result compared to breadboard prototypes.

Would you like me to create the actual KiCad project files with these components and footprints?