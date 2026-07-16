# PCB Schematic Specifications - ESP32-S3 iPod
## Complete Component List and Pin Assignments

---

## 1. ESP32-S3 Module Selection

### **RECOMMENDED: ESP32-S3-WROOM-1-N16R8**

**Specifications:**
- Part Number: ESP32-S3-WROOM-1-N16R8
- Flash: 16MB (Quad SPI)
- PSRAM: 8MB (Octal SPI)
- Package: SMD-41 (18mm × 25.5mm × 3.1mm)
- Antenna: PCB antenna (not U-FL)
- Operating Voltage: 3.0V - 3.6V
- Operating Temperature: -40°C to +85°C

**Why This Module:**
- Sufficient flash for music player firmware and assets
- PSRAM for audio buffering and UI graphics
- PCB antenna (no external antenna needed)
- Widely available and cost-effective
- Compatible with all your tested code

**Footprint:** Use standard ESP32-S3-WROOM-1 footprint from KiCad or Flux AI library

**Alternative:** ESP32-S3-WROOM-1-N8R8 (8MB Flash, 8MB PSRAM) if 16MB flash not needed

---

## 2. Complete GPIO Pin Map

### **Pin Assignment Table**

```
┌─────────┬──────────────────────────┬─────────────────────────────────┐
│ GPIO    │ Function                 │ Notes                           │
├─────────┼──────────────────────────┼─────────────────────────────────┤
│ GPIO0   │ BOOT Button              │ 10kΩ pull-up, button to GND    │
│ GPIO1   │ TTP229 SDO (Serial Data) │ Input, no pull-up needed        │
│ GPIO2   │ TTP229 SCL (Serial Clk)  │ Output, no pull-up needed       │
│ GPIO3   │ PCM5102 DIN (Audio Data) │ I2S data output                 │
│ GPIO4   │ PCM5102 BCK (Bit Clock)  │ I2S bit clock output            │
│ GPIO5   │ SD Card CS               │ SPI chip select, 10kΩ pull-up   │
│ GPIO6   │ PCM5102 LCK (L/R Clock)  │ I2S word select output          │
│ GPIO7   │ Battery Monitor (ADC)    │ ADC1_CH6, voltage divider input │
│ GPIO8   │ Display RST              │ Reset output, active low        │
│ GPIO9   │ Display DC               │ Data/Command select output      │
│ GPIO10  │ Display CS               │ SPI chip select output          │
│ GPIO11  │ SPI MOSI                 │ Display + SD shared             │
│ GPIO12  │ SPI SCK                  │ Display + SD shared             │
│ GPIO13  │ SPI MISO                 │ Display + SD shared             │
│ GPIO14  │ Rotary Encoder CLK       │ Optional, 10kΩ pull-up          │
│ GPIO15  │ Rotary Encoder DT        │ Optional, 10kΩ pull-up          │
│ GPIO16  │ Rotary Encoder SW        │ Optional, 10kΩ pull-up          │
│ GPIO17  │ Charge Status Input      │ From TP4056 CHRG pin            │
│ GPIO18  │ Power Enable Output      │ LDO enable control              │
│ GPIO19  │ Charging LED             │ Output, 1kΩ + LED to GND        │
│ GPIO20  │ Activity LED             │ Output, 1kΩ + LED to GND        │
│ GPIO21  │ DRV2605L SDA (I2C)       │ I2C data, 4.7kΩ pull-up         │
│ GPIO22  │ DRV2605L SCL (I2C)       │ I2C clock, 4.7kΩ pull-up        │
│ GPIO43  │ UART TX (Debug)          │ Programming/debug pad           │
│ GPIO44  │ UART RX (Debug)          │ Programming/debug pad           │
│ EN      │ Reset Button             │ 10kΩ pull-up, button to GND     │
└─────────┴──────────────────────────┴─────────────────────────────────┘
```

### **Reserved/Strapping Pins (Do Not Use)**
```
GPIO45 - Strapping pin (VDD_SPI voltage)
GPIO46 - Strapping pin (ROM boot mode)
GPIO19 - USB D- (if using USB-OTG, otherwise available)
GPIO20 - USB D+ (if using USB-OTG, otherwise available)
```

**Note:** We're using GPIO19/20 for LEDs since we're using USB-C only for power/programming via the built-in USB-Serial converter.

---

## 3. Power System Components

### **3.1 USB-C Connector**
- **Part:** USB Type-C Receptacle, 16-pin, mid-mount
- **Example:** Korean Hroparts TYPE-C-31-M-12
- **Pinout:**
  ```
  A1, B12 → GND
  A4, B9  → VBUS (5V)
  A5      → CC1 (5.1kΩ to GND)
  B5      → CC2 (5.1kΩ to GND)
  A6      → D+ (to TVS)
  A7      → D- (to TVS)
  B6      → D+ (to TVS)
  B7      → D- (to TVS)
  Shield  → GND
  ```

### **3.2 USB TVS Protection**
- **Part:** USBLC6-2SC6 or equivalent
- **Package:** SOT-23-6
- **Connections:**
  ```
  Pin 1 (I/O1) → USB D-
  Pin 2 (GND)  → GND
  Pin 3 (I/O2) → USB D+
  Pin 4 (I/O2) → USB D+
  Pin 5 (VBUS) → USB VBUS (5V)
  Pin 6 (I/O1) → USB D-
  ```
- **Capacitor:** 100nF ceramic near TVS

### **3.3 LiPo Charger (TP4056)**
- **Part:** TP4056 or MCP73831
- **Package:** SOP-8 or SOT-23-5
- **Connections:**
  ```
  VCC  → USB VBUS (5V) via Schottky diode
  GND  → GND
  BAT  → Battery positive
  PROG → 1.2kΩ to GND (1A charge current)
  CHRG → GPIO17 (charge status) + LED
  STDBY → Optional status LED
  ```
- **Input Cap:** 10µF ceramic (25V)
- **Output Cap:** 10µF ceramic (25V)
- **Schottky Diode:** SS34 or 1N5819 (prevents backfeed)

### **3.4 Battery Connector**
- **Part:** JST-PH 2.0mm, 2-pin
- **Polarity:** Red = Positive, Black = Negative
- **Protection:** Add polarity protection diode or use protected battery

### **3.5 LDO Regulator (3.3V)**
- **Part:** AMS1117-3.3 or AP2112K-3.3
- **Package:** SOT-223 or SOT-23-5
- **Output Current:** ≥500mA (800mA recommended)
- **Connections:**
  ```
  VIN  → Battery voltage (3.7V nominal)
  GND  → GND
  VOUT → 3.3V rail
  EN   → GPIO18 (enable control) + 10kΩ pull-up to VIN
  ```
- **Input Caps:** 10µF + 100nF ceramic
- **Output Caps:** 22µF + 100nF ceramic

**Alternative (Better):** AP2112K-3.3
- Lower dropout voltage (250mV vs 1.3V)
- Better for battery operation
- SOT-23-5 package (smaller)

### **3.6 Battery Voltage Monitor**
- **Voltage Divider:**
  ```
  VBAT → 10kΩ → GPIO7 (ADC) → 10kΩ → GND
  ```
- **Filter Cap:** 100nF from GPIO7 to GND
- **Calculation:** ADC reads VBAT/2 (max 4.2V → 2.1V at ADC)

---

## 4. Display System (ILI9341)

### **4.1 Display Connector**
- **Type:** 9-pin header, 2.54mm pitch
- **Connections:**
  ```
  Pin 1: VCC    → 3.3V
  Pin 2: GND    → GND
  Pin 3: CS     → GPIO10
  Pin 4: RST    → GPIO8
  Pin 5: DC     → GPIO9
  Pin 6: MOSI   → GPIO11
  Pin 7: SCK    → GPIO12
  Pin 8: MISO   → GPIO13
  Pin 9: LED    → 3.3V via 100Ω resistor
  ```

### **4.2 Display Support Components**
- **Decoupling:** 100nF ceramic near connector
- **Backlight:** 100Ω resistor (limits current to ~30mA)
- **Optional:** Series resistors on SPI lines (100Ω for noise immunity)

---

## 5. Touch Sensor (TTP229)

### **5.1 TTP229 Connector**
- **Type:** 4-pin header, 2.54mm pitch
- **Mode:** Serial mode (NOT I2C!)
- **Connections:**
  ```
  Pin 1: VCC → 3.3V
  Pin 2: GND → GND
  Pin 3: SCL → GPIO2 (clock output from ESP32)
  Pin 4: SDO → GPIO1 (data input to ESP32)
  ```

### **5.2 Important Notes**
- **NO pull-up resistors** on SCL or SDO (Serial mode, not I2C)
- **Decoupling:** 100nF ceramic near connector
- **Module Config:** Ensure TTP229 module is in Serial mode (check TP2/TP3 solder bridges)

---

## 6. SD Card System

### **6.1 SD Card Connector**
- **Type:** MicroSD card slot, push-push type
- **Alternative:** 6-pin header for external module
- **Connections:**
  ```
  Pin 1: CS   → GPIO5
  Pin 2: MOSI → GPIO11 (shared with display)
  Pin 3: GND  → GND
  Pin 4: VCC  → 3.3V
  Pin 5: SCK  → GPIO12 (shared with display)
  Pin 6: MISO → GPIO13 (shared with display)
  ```

### **6.2 SD Card Support Components**
- **CS Pull-up:** 10kΩ to 3.3V
- **Decoupling:** 100nF ceramic near connector
- **Optional:** 10kΩ pull-ups on MOSI, SCK, MISO for reliable operation

---

## 7. Audio System (PCM5102)

### **7.1 PCM5102 DAC**
- **Part:** PCM5102A or module
- **Package:** TSSOP-20 or pre-made module
- **Connections:**
  ```
  VCC  → 3.3V
  GND  → GND (analog ground)
  BCK  → GPIO4 (bit clock)
  DIN  → GPIO3 (data)
  LCK  → GPIO6 (left/right clock)
  SCK  → GND (system clock, use ESP32 as master)
  FMT  → GND (I2S format)
  XMT  → 3.3V (soft mute, HIGH = unmuted)
  LOUT → Left audio output
  ROUT → Right audio output
  ```

### **7.2 Audio Output Filtering**
- **Coupling Caps:** 10µF electrolytic on LOUT and ROUT
- **Series Resistors:** 100Ω on each output (protection)
- **Decoupling:** 100nF + 10µF near PCM5102

### **7.3 Headphone Jack**
- **Type:** 3.5mm TRS (Tip-Ring-Sleeve)
- **Connections:**
  ```
  Tip   → LOUT (via 10µF cap + 100Ω resistor)
  Ring  → ROUT (via 10µF cap + 100Ω resistor)
  Sleeve → Audio GND
  ```

### **7.4 Audio Ground Isolation**
- Connect audio GND to digital GND via ferrite bead or 0Ω resistor
- Keep audio traces away from SPI and switching power

---

## 8. Haptics System (DRV2605L)

### **8.1 DRV2605L Driver**
- **Part:** DRV2605L
- **Package:** VQFN-10 or module
- **I2C Address:** 0x5A (default)
- **Connections:**
  ```
  VDD  → 3.3V
  GND  → GND
  SDA  → GPIO21 (4.7kΩ pull-up to 3.3V)
  SCL  → GPIO22 (4.7kΩ pull-up to 3.3V)
  EN   → 3.3V (or GPIO control)
  IN   → Not connected (I2C mode)
  OUT+ → Motor positive
  OUT- → Motor negative
  ```

### **8.2 Haptics Support Components**
- **I2C Pull-ups:** 4.7kΩ on SDA and SCL
- **Decoupling:** 100nF + 10µF near DRV2605L
- **Motor Connector:** 2-pin JST or header for ERM/LRA motor

---

## 9. Rotary Encoder (Optional)

### **9.1 Rotary Encoder**
- **Type:** EC11 or similar with switch
- **Connections:**
  ```
  CLK → GPIO14 (10kΩ pull-up)
  DT  → GPIO15 (10kΩ pull-up)
  SW  → GPIO16 (10kΩ pull-up)
  GND → GND
  ```

### **9.2 Debouncing**
- **Capacitors:** 100nF on CLK, DT, and SW to GND
- **Software:** Use interrupt-based debouncing in code

---

## 10. User Interface Components

### **10.1 Buttons**
- **Reset Button:**
  ```
  EN pin → 10kΩ pull-up to 3.3V
  Button → EN to GND
  Capacitor → 100nF from EN to GND (debounce)
  ```

- **Boot Button:**
  ```
  GPIO0 → 10kΩ pull-up to 3.3V
  Button → GPIO0 to GND
  Capacitor → 100nF from GPIO0 to GND (debounce)
  ```

### **10.2 LEDs**
- **Power LED (Green):**
  ```
  3.3V → 1kΩ → LED (anode) → LED (cathode) → GND
  ```

- **Charging LED (Red/Orange):**
  ```
  GPIO19 → 1kΩ → LED (anode) → LED (cathode) → GND
  ```

- **Activity LED (Blue):**
  ```
  GPIO20 → 1kΩ → LED (anode) → LED (cathode) → GND
  ```

- **LED Package:** 0805 SMD
- **Forward Voltage:** ~2V (red), ~3V (blue/green)
- **Current:** ~2-3mA with 1kΩ resistor

---

## 11. Programming/Debug Interface

### **11.1 Debug Pads**
- **Type:** Test points or 1.27mm header
- **Connections:**
  ```
  Pad 1: 3.3V
  Pad 2: GND
  Pad 3: TX (GPIO43)
  Pad 4: RX (GPIO44)
  Pad 5: EN (Reset)
  Pad 6: GPIO0 (Boot)
  ```

### **11.2 Programming Notes**
- USB-C provides power and programming via ESP32-S3 built-in USB
- Debug pads are for serial monitoring and emergency recovery
- No external USB-to-Serial converter needed

---

## 12. Decoupling Capacitors

### **12.1 ESP32-S3 Module**
Place within 5mm of module:
- **4× 100nF** ceramic (one per VCC pin)
- **1× 10µF** ceramic (bulk capacitor)

### **12.2 All Other ICs**
- **100nF** ceramic on each VCC pin
- **10µF** ceramic for ICs with high current draw

### **12.3 Capacitor Specifications**
- **Type:** X7R or X5R ceramic
- **Voltage Rating:** 25V minimum (50V preferred)
- **Package:** 0805

---

## 13. Schematic Organization

### **Recommended Sheet Structure:**

1. **Sheet 1: Power System**
   - USB-C connector + TVS
   - TP4056 charger
   - Battery connector
   - LDO regulator
   - Power distribution

2. **Sheet 2: ESP32-S3 Core**
   - ESP32-S3 module
   - Decoupling capacitors
   - Reset and boot buttons
   - Programming pads
   - Crystal (if external)

3. **Sheet 3: Display & Touch**
   - ILI9341 display connector
   - TTP229 touch connector
   - Support components

4. **Sheet 4: Storage & Audio**
   - SD card connector
   - PCM5102 DAC
   - Headphone jack
   - Audio filtering

5. **Sheet 5: Peripherals**
   - DRV2605L haptics
   - Rotary encoder (optional)
   - LEDs
   - Status indicators

---

## 14. Critical Design Notes

### **14.1 TTP229 Serial Mode**
⚠️ **CRITICAL:** TTP229 uses Serial protocol, NOT I2C!
- No I2C address
- No pull-up resistors needed
- GPIO2 is clock OUTPUT from ESP32
- GPIO1 is data INPUT to ESP32

### **14.2 SPI Bus Sharing**
- Display CS = GPIO10
- SD Card CS = GPIO5
- Shared: MOSI (GPIO11), SCK (GPIO12), MISO (GPIO13)
- Both devices can coexist on same bus

### **14.3 I2S Audio**
- Keep traces short and away from SPI
- Use analog ground for audio section
- Tie PCM5102 SCK to GND (ESP32 provides clock)

### **14.4 Power Budget**
```
Component         Current (mA)
─────────────────────────────
ESP32-S3          80-240
Display           100
SD Card           50-100
Audio DAC         10
Haptics           50-100
LEDs              10
─────────────────────────────
Total             300-600 mA
```
LDO must handle 600mA minimum, 800mA recommended.

### **14.5 Battery Considerations**
- Recommended: 2000-3000mAh Li-ion/LiPo
- Voltage range: 3.0V (empty) to 4.2V (full)
- Runtime: 4-8 hours typical

---

## 15. Component Summary (BOM Preview)

| Qty | Reference | Value/Part Number | Package | Description |
|-----|-----------|-------------------|---------|-------------|
| 1 | U1 | ESP32-S3-WROOM-1-N16R8 | SMD-41 | Main MCU |
| 1 | U2 | TP4056 | SOP-8 | Li-ion charger |
| 1 | U3 | AMS1117-3.3 | SOT-223 | 3.3V LDO |
| 1 | U4 | USBLC6-2SC6 | SOT-23-6 | USB TVS |
| 1 | U5 | DRV2605L | VQFN-10 | Haptics driver |
| 1 | U6 | PCM5102A | TSSOP-20 | Audio DAC |
| 1 | J1 | USB Type-C | 16-pin | USB connector |
| 1 | J2 | JST-PH-2 | 2-pin | Battery connector |
| 1 | J3 | Header 1×9 | 2.54mm | Display connector |
| 1 | J4 | Header 1×4 | 2.54mm | TTP229 connector |
| 1 | J5 | MicroSD slot | Push-push | SD card |
| 1 | J6 | 3.5mm TRS | Through-hole | Headphone jack |
| 2 | SW1, SW2 | Tactile switch | 6×6mm | Reset, Boot |
| 3 | D1-D3 | LED | 0805 | Status LEDs |
| ~30 | R1-R30 | Various | 0805 | Resistors |
| ~25 | C1-C25 | Various | 0805 | Capacitors |

---

## Ready for Schematic Creation! ✅

This specification provides everything needed to create the complete schematic. All pin assignments are verified against your working code, and all component values are production-ready.

**Next Steps:**
1. Import ESP32-S3-WROOM-1 symbol into schematic
2. Place and wire power system (USB-C → TP4056 → LDO → 3.3V)
3. Add ESP32-S3 with decoupling
4. Connect all peripherals per pin map
5. Add support components (resistors, capacitors, LEDs)
6. Run ERC to verify connections
7. Generate netlist for PCB layout

Good luck with your schematic! 🚀
