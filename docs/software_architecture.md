# Software Architecture

## Overview

The ESP32-S3 iPod software is built using a modular architecture with separate managers for different subsystems. This design allows for easy testing, maintenance, and feature additions.

## Core Managers

### DisplayManager
- **Purpose**: Handles TFT display and LVGL integration
- **Responsibilities**:
  - TFT initialization and configuration
  - LVGL display driver setup
  - Brightness control
  - Sleep/wake functionality
- **Files**: `src/display/display_manager.h/cpp`

### AudioManager
- **Purpose**: Manages audio playback and routing
- **Responsibilities**:
  - I2S audio output
  - MP3/FLAC/WAV decoding
  - Volume control
  - Playback state management
  - Bluetooth A2DP
  - USB audio
- **Files**: `src/audio/audio_manager.h/cpp`

### SDManager
- **Purpose**: Handles file system and music library
- **Responsibilities**:
  - SD card initialization
  - Music file scanning
  - Library organization (artists/albums)
  - File metadata parsing
  - Search functionality
- **Files**: `src/storage/sd_manager.h/cpp`

### InputManager
- **Purpose**: Processes user input from multiple sources
- **Responsibilities**:
  - Rotary encoder handling
  - Capacitive touch processing
  - Input event queuing
  - LVGL input driver
  - Gesture recognition
- **Files**: `src/input/input_manager.h/cpp`

### UIManager
- **Purpose**: Manages user interface and navigation
- **Responsibilities**:
  - Screen management
  - LVGL UI creation
  - Event handling
  - Status updates
  - Theme management
- **Files**: `src/ui/ui_manager.h/cpp`

### PowerManager
- **Purpose**: Handles power management and battery monitoring
- **Responsibilities**:
  - Battery voltage monitoring
  - Charge state detection
  - Power state transitions
  - Sleep/wake control
  - USB-C PD management
- **Files**: `src/power/power_manager.h/cpp`

## Data Flow

```
Input Events -> InputManager -> UIManager -> Audio/Storage Managers
                                    |
Battery/Power -> PowerManager -> UIManager -> Display Updates
                                    |
Audio Data -> SDManager -> AudioManager -> I2S Output
                              |
UI Updates -> UIManager -> DisplayManager -> TFT Display
```

## Key Design Patterns

### Manager Pattern
Each subsystem is encapsulated in a manager class that:
- Provides a clean interface to other components
- Handles initialization and cleanup
- Manages internal state
- Provides update methods for main loop

### Event-Driven Architecture
- Input events are queued and processed asynchronously
- UI updates are triggered by state changes
- Audio playback events drive UI updates

### State Machines
- Audio playback states (stopped/playing/paused)
- Power states (active/idle/sleep)
- UI navigation states

## Configuration

All hardware-specific settings are centralized in `src/config.h`:
- Pin assignments
- Display dimensions
- Audio settings
- File system paths
- UI colors and fonts

## Libraries Used

- **LVGL**: Graphics library for UI
- **TFT_eSPI**: Display driver
- **ESP32-AudioI2S**: Audio processing
- **Arduino-Audio-Tools**: Audio codecs
- **SD**: File system access
- **Wire**: I2C communication

## Build System

Uses PlatformIO for:
- Dependency management
- Cross-platform builds
- Library version control
- Upload and monitoring

## Future Enhancements

### Phase 2 Features
- Album art display
- Equalizer with presets
- Playlist management
- Shuffle/repeat modes

### Phase 3 Features
- Bluetooth A2DP source
- USB audio device mode
- OTA updates
- Web interface

### Phase 4 Features
- Streaming services (Spotify Connect)
- Voice control
- Advanced DSP effects
- Multi-room audio