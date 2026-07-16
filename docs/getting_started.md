# Getting Started

## Prerequisites

### Software Requirements
- [PlatformIO IDE](https://platformio.org/platformio-ide) or PlatformIO Core
- [Git](https://git-scm.com/) for version control
- [Python 3.7+](https://python.org/) (for PlatformIO)

### Hardware Requirements
- ESP32-S3 development board
- USB-C cable for programming
- Breadboard and jumper wires for prototyping
- Components listed in [hardware_setup.md](hardware_setup.md)

## Quick Start

### 1. Clone the Repository
```bash
git clone <repository-url>
cd esp32-ipod-player
```

### 2. Install PlatformIO
If using VS Code:
```bash
# Install PlatformIO extension from VS Code marketplace
```

If using command line:
```bash
pip install platformio
```

### 3. Build the Project
```bash
# Navigate to project directory
cd esp32-ipod-player

# Build the project
pio run

# Upload to ESP32-S3
pio run --target upload

# Monitor serial output
pio device monitor
```

### 4. Basic Hardware Setup
For initial testing, you only need:
1. ESP32-S3 board
2. USB-C cable
3. Optional: LED on GPIO2 for status indication

## Development Phases

### Phase 1: Core Hardware (Weeks 1-3)
**Goal**: Get basic hardware working

**Tasks**:
- [ ] Order all components
- [ ] Design PCB or breadboard layout
- [ ] 3D print or design case
- [ ] Assemble core components
- [ ] Test power supply
- [ ] Verify pin connections

**Deliverables**:
- Working hardware prototype
- Power management functional
- Basic connectivity verified

### Phase 2: Basic Software (Weeks 4-6)
**Goal**: Implement core functionality

**Tasks**:
- [ ] Display initialization and LVGL setup
- [ ] SD card reading and file system
- [ ] Basic audio playback (MP3)
- [ ] Rotary encoder input
- [ ] Simple UI navigation

**Deliverables**:
- Basic music player functionality
- File browsing and selection
- Play/pause/stop controls
- Volume adjustment

### Phase 3: Advanced Features (Weeks 7-9)
**Goal**: Add advanced audio and connectivity

**Tasks**:
- [ ] USB-C charging and audio
- [ ] Bluetooth A2DP implementation
- [ ] Capacitive touch input
- [ ] Music library scanning and organization
- [ ] Album art display

**Deliverables**:
- Multi-source audio (SD, Bluetooth, USB)
- Touch navigation
- Organized music library
- Visual enhancements

### Phase 4: Polish and Testing (Weeks 10-12)
**Goal**: Finalize and optimize

**Tasks**:
- [ ] UI animations and transitions
- [ ] Equalizer implementation
- [ ] Power optimization
- [ ] Final case assembly
- [ ] Comprehensive testing
- [ ] Documentation completion

**Deliverables**:
- Production-ready device
- Complete documentation
- User manual
- Source code cleanup

## Testing Strategy

### Unit Testing
Each manager class should be testable independently:
```cpp
// Example test structure
void test_audio_manager() {
    AudioManager audio;
    assert(audio.init() == true);
    assert(audio.getVolume() == 50); // Default volume
    // Add more tests...
}
```

### Integration Testing
Test manager interactions:
- UI -> Audio control
- Input -> UI navigation
- Power -> System sleep/wake

### Hardware Testing
- Pin connectivity verification
- Power consumption measurement
- Audio quality testing
- Battery life testing

## Troubleshooting

### Common Issues

**Display not working**:
- Check SPI connections
- Verify power supply (3.3V)
- Check TFT_eSPI configuration

**SD card not detected**:
- Verify SPI connections
- Check SD card format (FAT32)
- Ensure proper power supply

**Audio not playing**:
- Check I2S connections
- Verify audio file format
- Test with simple WAV file first

**Touch not responding**:
- Check I2C connections
- Verify touch controller power
- Test I2C communication

### Debug Tools
- Serial monitor for logging
- Logic analyzer for signal verification
- Multimeter for power measurements
- Oscilloscope for audio signals

## Next Steps

1. **Start with Phase 1**: Order components and plan hardware
2. **Set up development environment**: Install PlatformIO and test basic build
3. **Begin prototyping**: Start with display and basic UI
4. **Iterate quickly**: Test each component as you add it
5. **Document progress**: Keep notes on what works and what doesn't

## Resources

- [ESP32-S3 Datasheet](https://www.espressif.com/sites/default/files/documentation/esp32-s3_datasheet_en.pdf)
- [LVGL Documentation](https://docs.lvgl.io/)
- [PlatformIO Documentation](https://docs.platformio.org/)
- [TFT_eSPI Library](https://github.com/Bodmer/TFT_eSPI)
- [ESP32-AudioI2S Library](https://github.com/pschatzmann/ESP32-A2DP)