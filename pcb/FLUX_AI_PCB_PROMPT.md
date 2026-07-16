# Flux AI PCB Design Prompt - ESP32-S3 iPod

## Goal
Design and implement a one-sided 60 mm x 100 mm PCB for an ESP32-S3 based iPod-style music player. The board will integrate USB-C for power/programming with low-cap bidirectional TVS protection on D+/D‑, a LiPo charger (TP4056‑class) with a 3.3 V LDO (>=500 mA), battery connector with voltage monitor via ADC, an ILI9341 2.8" SPI display, microSD via SPI (shared bus with display), TTP229 capacitive touch sensor in Serial mode, DRV2605L haptics driver over I2C (with 4.7k pull-ups), PCM5102 I2S audio output to a 3.5 mm jack, optional rotary encoder, reset/boot buttons, LEDs (power, charging, activity), and programming/debug pads.

## Critical Pin Map (Based on Working Configuration)

### ESP32-S3 Pin Assignments

#### Display (ILI9341) - SPI Bus
```
Display Pin → ESP32-S3 Pin → Function
VCC         → 3.3V          → Power
GND         → GND           → Ground
CS          → GPIO10        → Chip Select
RST         → GPIO8         → Reset
DC          → GPIO9         → Data/Command
MOSI        → GPIO11        → SPI Data Out
SCK         → GPIO12        → SPI Clock
MISO        → GPIO13        → SPI Data In
LED         → 3.3V          → Backlight (via 100Ω resistor)
```

#### TTP229 Touch Sensor - Serial Mode (NOT I2C!)
```
TTP229 Pin → ESP32-S3 Pin → Function
VCC        → 3.3V         → Power
GND        → GND          → Ground
SCL        → GPIO2        → Clock (Serial mode)
SDO        → GPIO1        → Data (Serial mode)
```
**CRITICAL:** TTP229 operates in Serial mode, NOT I2C mode. No pull-up resistors needed.

#### SD Card - SPI Mode (Shared with Display)
```
SD Card Pin → ESP32-S3 Pin → Function
VCC         → 3.3V         → Power
GND         → GND          → Ground
CS          → GPIO5        → Chip Select (separate from display)
MOSI        → GPIO11       → SPI Data Out (shared with display)
SCK         → GPIO12       → SPI Clock (shared with display)
MISO        → GPIO13       → SPI Data In (shared with display)
```
**Note:** Display CS=GPIO10, SD CS=GPIO5 for bus sharing.

#### PCM5102 Audio DAC - I2S
```
PCM5102 Pin → ESP32-S3 Pin → Function
VCC         → 3.3V         → Power
GND         → GND          → Ground
BCK         → GPIO4        → Bit Clock
DIN         → GPIO3        → Data In
LCK         → GPIO6        → Left/Right Clock
SCK         → GND          → System Clock (tie to ground)
FMT         → GND          → Format (I2S mode)
XMT         → 3.3V         → Mute control (HIGH = unmuted)
LOUT        → Jack Tip     → Left audio output
ROUT        → Jack Ring    → Right audio output
GND         → Jack Sleeve  → Audio ground
```

#### DRV2605L Haptics - I2C
```
DRV2605L Pin → ESP32-S3 Pin → Function
VCC          → 3.3V         → Power
GND          → GND          → Ground
SDA          → GPIO21       → I2C Data (with 4.7kΩ pull-up)
SCL          → GPIO22       → I2C Clock (with 4.7kΩ pull-up)
```

#### Optional Rotary Encoder
```
Encoder Pin → ESP32-S3 Pin → Function
CLK         → GPIO14       → Clock
DT          → GPIO15       → Data
SW          → GPIO16       → Switch/Button
```

#### Power Management
```
Function         → ESP32-S3 Pin → Notes
Battery Monitor  → GPIO7 (ADC)  → Voltage divider (2:1 ratio)
Charge Status    → GPIO17       → From TP4056 CHRG pin
Power Enable     → GPIO18       → LDO enable control
```

#### Status LEDs
```
LED Function → ESP32-S3 Pin → Notes
Power LED    → 3.3V         → Direct connection via 1kΩ resistor
Charging LED → GPIO19       → From TP4056 or GPIO control
Activity LED → GPIO20       → User-controlled status
```

#### Programming/Debug
```
Function → ESP32-S3 Pin → Notes
BOOT     → GPIO0        → Boot mode button (pull to GND)
RESET    → EN           → Reset button
TX       → GPIO43       → UART TX (debug pad)
RX       → GPIO44       → UART RX (debug pad)
```

## GPIO Summary Table
```
GPIO0  - BOOT Button
GPIO1  - TTP229 SDO (Serial Data)
GPIO2  - TTP229 SCL (Serial Clock)
GPIO3  - PCM5102 DIN (Audio Data)
GPIO4  - PCM5102 BCK (Audio Bit Clock)
GPIO5  - SD Card CS
GPIO6  - PCM5102 LCK (Audio L/R Clock)
GPIO7  - Battery Monitor (ADC)
GPIO8  - Display RST
GPIO9  - Display DC
GPIO10 - Display CS
GPIO11 - SPI MOSI (Display + SD shared)
GPIO12 - SPI SCK (Display + SD shared)
GPIO13 - SPI MISO (Display + SD shared)
GPIO14 - Rotary Encoder CLK (optional)
GPIO15 - Rotary Encoder DT (optional)
GPIO16 - Rotary Encoder SW (optional)
GPIO17 - Charge Status Input
GPIO18 - Power Enable Output
GPIO19 - Charging LED
GPIO20 - Activity LED
GPIO21 - DRV2605L SDA (I2C)
GPIO22 - DRV2605L SCL (I2C)
GPIO43 - UART TX (debug)
GPIO44 - UART RX (debug)
EN     - Reset Button
```

## Implementation Plan

### 1. Project Setup
- Board dimensions: 60 mm x 100 mm
- One-sided component placement
- 0805 passive components for hand soldering
- 4× M3 mounting holes in corners (3mm diameter)
- 2-layer PCB (top + bottom ground plane)

### 2. Schematic Design

#### Power System
1. **USB-C Connector**
   - USB Type-C receptacle (16-pin)
   - Low-capacitance bidirectional TVS diodes on D+ and D- (e.g., USBLC6-2SC6)
   - 5.1kΩ pull-down resistors on CC1 and CC2 for 5V negotiation
   - ESD protection on VBUS

2. **LiPo Charging Circuit (TP4056)**
   - TP4056 or equivalent Li-ion charger IC
   - USB VBUS → TP4056 VCC
   - Battery connector (JST-PH 2.0mm, 2-pin)
   - CHRG and STDBY status outputs
   - 1.2kΩ programming resistor for 1A charge current
   - Input capacitor: 10µF ceramic
   - Output capacitor: 10µF ceramic

3. **3.3V LDO Regulator**
   - AMS1117-3.3 or AP2112K-3.3 (>=500mA)
   - Input: Battery voltage (3.7V nominal)
   - Output: 3.3V for all components
   - Input capacitor: 10µF + 100nF ceramic
   - Output capacitor: 22µF + 100nF ceramic
   - Enable pin control via GPIO18

4. **Battery Voltage Monitor**
   - Voltage divider: 10kΩ + 10kΩ (2:1 ratio)
   - Connect to GPIO7 (ADC1_CH6)
   - 100nF filter capacitor to ground

#### Core Components

5. **ESP32-S3-WROOM-1 Module**
   - Place centrally on board
   - 100nF decoupling capacitors on all VCC pins (within 5mm)
   - 10µF bulk capacitor nearby
   - EN pin: 10kΩ pull-up + reset button to GND
   - GPIO0: 10kΩ pull-up + boot button to GND
   - All unused pins: leave floating or add test pads

6. **ILI9341 Display Connector**
   - 9-pin header (2.54mm pitch)
   - Series resistors on SPI lines (optional, 100Ω for noise)
   - LED backlight: 100Ω current limiting resistor from 3.3V

7. **TTP229 Touch Sensor Connector**
   - 4-pin header (2.54mm pitch)
   - NO pull-up resistors (Serial mode, not I2C)
   - 100nF decoupling capacitor near connector

8. **SD Card Connector**
   - MicroSD card slot or 6-pin header
   - 10kΩ pull-up on CS line
   - 100nF decoupling capacitor
   - Shared MOSI/SCK/MISO with display

9. **PCM5102 Audio DAC**
   - PCM5102A module or bare IC
   - SCK tied to GND (use ESP32 as master clock)
   - FMT tied to GND (I2S format)
   - XMT tied to 3.3V (unmute)
   - 100nF decoupling on VCC
   - Audio output filtering: 10µF coupling capacitors on LOUT/ROUT

10. **3.5mm Headphone Jack**
    - TRS connector (Tip-Ring-Sleeve)
    - Tip: Left channel (LOUT via 10µF cap)
    - Ring: Right channel (ROUT via 10µF cap)
    - Sleeve: Audio ground
    - Optional: 100Ω series resistors for protection

11. **DRV2605L Haptics Driver**
    - DRV2605L IC or module
    - 4.7kΩ pull-up resistors on SDA and SCL
    - 100nF + 10µF decoupling capacitors
    - Motor output connector (2-pin)
    - EN pin: tie to 3.3V or GPIO control

12. **Rotary Encoder (Optional)**
    - 5-pin encoder with switch
    - 10kΩ pull-ups on CLK, DT, SW
    - 100nF debounce capacitors on all pins

#### Supporting Components

13. **Buttons**
    - Reset button: EN to GND
    - Boot button: GPIO0 to GND
    - Tactile switches (6mm × 6mm)

14. **LEDs**
    - Power LED: 3.3V → 1kΩ → LED → GND (green)
    - Charging LED: GPIO19 → 1kΩ → LED → GND (red/orange)
    - Activity LED: GPIO20 → 1kΩ → LED → GND (blue)
    - All LEDs: 0805 package

15. **Programming/Debug Pads**
    - TX, RX, GND, 3.3V test points
    - 1.27mm pitch header or castellated holes
    - Silkscreen labels

### 3. PCB Layout Constraints

#### Component Placement Priority
1. **ESP32-S3 module** - Center of board
2. **Decoupling capacitors** - Within 5mm of ESP32-S3 VCC pins
3. **USB-C connector** - Top edge, with TVS diodes immediately adjacent
4. **TP4056 charger** - Near USB-C connector
5. **LDO regulator** - Between battery and ESP32-S3
6. **Display connector** - Top or side edge
7. **Audio components** - Bottom area, isolated from digital
8. **SD card slot** - Side edge, near display
9. **TTP229 connector** - Opposite side from display
10. **Buttons and LEDs** - Accessible edges

#### Trace Routing Rules
1. **SPI Bus (Display + SD)**
   - Keep traces short (<50mm)
   - Match lengths within 5mm
   - Route together, maintain spacing
   - Avoid crossing under ESP32-S3 module

2. **I2S Audio**
   - Route away from SPI and power switching
   - Keep BCK, LCK, DIN together
   - Shield with ground traces if possible
   - Minimize trace length

3. **I2C Bus (Haptics)**
   - Keep traces short
   - Route together
   - 4.7kΩ pull-ups near master (ESP32-S3)

4. **Power Distribution**
   - Wide traces for 3.3V and GND (0.5mm minimum)
   - Star ground from LDO output
   - Separate analog and digital grounds, join at one point

5. **USB Data Lines**
   - Differential pair routing for D+ and D-
   - 90Ω differential impedance
   - Keep equal length (within 0.5mm)
   - TVS diodes within 5mm of connector

#### Ground Plane
- Solid ground plane on bottom layer
- Ground stitching vias every 5mm around perimeter
- Thermal reliefs on ground pads for hand soldering
- Separate audio ground region, connect via ferrite bead or 0Ω resistor

#### Silkscreen
- Component designators (R1, C1, U1, etc.)
- Pin 1 indicators on all ICs and connectors
- Polarity marks on LEDs and capacitors
- Voltage labels (3.3V, GND, VBAT)
- Connector pin functions
- "BOOT", "RESET" button labels
- Board name and version number

### 4. Design Rule Checks

#### Electrical Rules (ERC)
- [ ] All power pins connected
- [ ] No floating inputs
- [ ] Pull-ups/pull-downs on critical pins
- [ ] Decoupling capacitors on all ICs
- [ ] TVS protection on external connections
- [ ] Current capacity adequate for all traces

#### Design Rules (DRC)
- [ ] Minimum trace width: 0.2mm (0.5mm for power)
- [ ] Minimum clearance: 0.2mm
- [ ] Via size: 0.6mm drill, 1.0mm pad
- [ ] Mounting holes: 3.2mm drill, 5.0mm keepout
- [ ] Board outline: 60mm × 100mm ±0.1mm
- [ ] All components on top layer only
- [ ] Silkscreen text: minimum 1mm height

### 5. Bill of Materials (BOM) Guidelines

#### Passive Components (0805 package)
- Resistors: 1%, 1/8W
- Capacitors: X7R/X5R ceramic, 25V rating minimum
- LEDs: Standard brightness, 20mA

#### Active Components
- ESP32-S3-WROOM-1-N16R8 (16MB Flash, 8MB PSRAM)
- TP4056 or MCP73831 (Li-ion charger)
- AMS1117-3.3 or AP2112K-3.3 (LDO)
- USBLC6-2SC6 (USB TVS)
- DRV2605L (Haptics driver)
- PCM5102A (Audio DAC)

#### Connectors
- USB Type-C receptacle (16-pin, mid-mount)
- JST-PH 2.0mm battery connector
- 2.54mm pin headers for peripherals
- MicroSD card slot (push-push type)
- 3.5mm TRS audio jack

## Success Criteria

### Schematic Verification
- [ ] All subsystems integrated per pin map
- [ ] No GPIO conflicts
- [ ] Proper decoupling on all ICs
- [ ] ESD/TVS protection on external connections
- [ ] Pull-ups/pull-downs on all critical pins
- [ ] Power sequencing correct

### Layout Verification
- [ ] Board size: 60mm × 100mm
- [ ] One-sided component placement
- [ ] All passives are 0805 package
- [ ] Decoupling caps within 5mm of ESP32-S3
- [ ] SPI traces matched and short
- [ ] Audio isolated from digital noise
- [ ] Solid ground plane with stitching
- [ ] Mounting holes in corners
- [ ] Clear silkscreen labels
- [ ] ERC and DRC pass with zero errors

### Manufacturing Readiness
- [ ] Gerber files generated
- [ ] BOM exported with part numbers
- [ ] Pick-and-place file (if needed)
- [ ] Assembly drawing created
- [ ] Design files backed up

## Notes for Flux AI

1. **TTP229 Serial Mode**: This is NOT I2C! It's a simple serial protocol with clock and data. Do not add I2C pull-ups.

2. **SPI Bus Sharing**: Display and SD card share MOSI/SCK/MISO but have separate CS pins (GPIO10 and GPIO5).

3. **Audio Isolation**: Keep PCM5102 and audio traces away from SPI, I2C, and switching power supplies.

4. **Hand Soldering**: Use 0805 passives, SOIC/TSSOP ICs where possible. Avoid BGA or QFN without exposed pads.

5. **Power Budget**: Total current ~400mA typical, 600mA peak. LDO must handle this.

6. **Battery**: Design for 3.7V Li-ion/LiPo, 1000-3000mAh capacity.

7. **Programming**: USB-C provides power and programming via built-in USB on ESP32-S3.

This prompt is based on a working breadboard prototype with verified pin assignments and tested code.
