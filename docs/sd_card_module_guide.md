# 💾 SD Card Module Integration Guide
## Micro SD Card Reader Module for iPod Project

---

## 📦 **Your SD Card Module Specs**

**Module Name**: Micro SD Card Reader Module (Micro SD Adaptor)

### Features:
- **Dual voltage support**: 3.3V / 5V compatible
- **Working voltage**: VCC 4.5~5.5V
- **Current draw**: 0.2~200mA (idle to active read/write)
- **Interface**: SPI standard (MISO, MOSI, SCK, CS)
- **Onboard 3.3V regulator**: LDO regulator for SD card power
- **Level converter**: Built-in for 3.3V/5V compatibility
- **Card support**: 
  - Micro SD up to 2GB
  - Micro SDHC up to 32GB
- **Mounting**: 4x M2 2.2mm screw holes

### Pinout:
```
1. GND  - Ground
2. VCC  - Power (4.5-5.5V, can use 5V or 3.3V)
3. MISO - Master In Slave Out (SPI data to ESP32)
4. MOSI - Master Out Slave In (SPI data from ESP32)
5. SCK  - Serial Clock (SPI clock)
6. CS   - Chip Select (SPI slave select)
```

---

## 🔌 **Wiring to ESP32-S3**

### Connection Diagram:
```
SD Card Module          ESP32-S3 Pin        Function
──────────────          ────────────        ────────
GND                 →   GND                 Ground
VCC                 →   3.3V                Power (module regulates to 3.3V)
MISO                →   GPIO13              SPI Data In (shared with display)
MOSI                →   GPIO11              SPI Data Out (shared with display)
SCK                 →   GPIO12              SPI Clock (shared with display)
CS                  →   GPIO5               Chip Select (dedicated)
```

### Visual Wiring:
```
                    ┌─────────────────────────────────┐
                    │       ESP32-S3 DevKit           │
                    │                                 │
    ┌───────────────┤ 3.3V                            │
    │       ┌───────┤ GND                             │
    │       │   ┌───┤ GPIO5                           │
    │       │   │┌──┤ GPIO11 (MOSI) ◄─────────────┐  │
    │       │   ││┌─┤ GPIO12 (SCK)  ◄───────────┐ │  │
    │       │   │││┌┤ GPIO13 (MISO) ◄─────────┐ │ │  │
    │       │   ││││└─────────────────────────┼─┼─┼──┘
    │       │   ││││                          │ │ │
    ▼       ▼   ▼▼▼▼                          │ │ │
┌────────────────────────┐                    │ │ │
│  SD Card Module        │                    │ │ │
├────────────────────────┤                    │ │ │
│ VCC  GND  CS  MOSI     │                    │ │ │
│           SCK  MISO    │                    │ │ │
└────────────────────────┘                    │ │ │
                                              │ │ │
                    ┌─────────────────────────┼─┼─┼──┐
                    │   MSP2806 Display       │ │ │  │
                    ├─────────────────────────┼─┼─┼──┤
                    │ MOSI (GPIO11) ◄─────────┘ │ │  │
                    │ SCK  (GPIO12) ◄───────────┘ │  │
                    │ MISO (GPIO13) ◄─────────────┘  │
                    │ CS   (GPIO10)                  │
                    └────────────────────────────────┘
```

---

## ⚡ **Power Considerations**

### Why Connect to 3.3V (Not 5V):
Your module has an onboard 3.3V regulator, but connecting to ESP32's 3.3V is better:

1. **Lower power draw**: No regulator loss
2. **Cleaner signals**: Direct 3.3V logic levels
3. **ESP32 compatibility**: ESP32-S3 is 3.3V native
4. **Shared SPI bus**: Display is also 3.3V

### Current Draw:
```
SD Card Operation       Current Draw
─────────────────       ────────────
Idle (card inserted)    0.2-1mA
Reading data            50-80mA
Writing data            80-200mA
No card inserted        <0.1mA
```

**Important**: Peak current during write operations can reach 200mA. Ensure your power supply can handle total system load:
- ESP32-S3: ~80-240mA
- Display: ~100mA
- SD Card: ~200mA (peak)
- **Total**: ~380-540mA peak

---

## 🔧 **SPI Bus Sharing**

Your SD card shares the SPI bus with the display. This is **completely normal** and works perfectly!

### How SPI Sharing Works:
```
ESP32-S3 SPI Bus:
├── MOSI (GPIO11) ──┬── Display MOSI
│                   └── SD Card MOSI
├── SCK  (GPIO12) ──┬── Display SCK
│                   └── SD Card SCK
├── MISO (GPIO13) ──┬── Display MISO
│                   └── SD Card MISO
├── CS1  (GPIO10) ──── Display CS (active LOW)
└── CS2  (GPIO5)  ──── SD Card CS (active LOW)
```

### Key Points:
- **Shared signals**: MOSI, SCK, MISO are connected to both devices
- **Separate CS pins**: Each device has its own Chip Select
- **Only one active**: Only the device with CS=LOW responds
- **No conflicts**: When Display CS is LOW, SD card ignores the bus
- **Fast switching**: ESP32 can switch between devices in microseconds

---

## 💻 **Software Configuration**

### Update `src/config.h`:
```cpp
// SD Card pins
#define SD_CS    5      // Chip Select (dedicated)
#define SD_MOSI  11     // Shared with display
#define SD_SCK   12     // Shared with display
#define SD_MISO  13     // Shared with display

// Supported music formats
#define MUSIC_EXTENSIONS ".mp3,.wav,.flac,.aac,.m4a"

// SD card settings
#define SD_MAX_FILES 1000
#define SD_SCAN_DEPTH 5
```

### Initialize SD Card:
```cpp
#include <SD.h>
#include <SPI.h>

void setup() {
  Serial.begin(115200);
  
  // Initialize SPI bus (shared with display)
  SPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
  
  // Initialize SD card
  if (!SD.begin(SD_CS)) {
    Serial.println("❌ SD Card initialization failed!");
    Serial.println("Check:");
    Serial.println("  - Card inserted?");
    Serial.println("  - Wiring correct?");
    Serial.println("  - Card formatted (FAT32)?");
    return;
  }
  
  Serial.println("✅ SD Card initialized!");
  
  // Print card info
  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("❌ No SD card attached");
    return;
  }
  
  Serial.print("📇 Card Type: ");
  if (cardType == CARD_MMC) {
    Serial.println("MMC");
  } else if (cardType == CARD_SD) {
    Serial.println("SDSC");
  } else if (cardType == CARD_SDHC) {
    Serial.println("SDHC");
  }
  
  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("💾 Card Size: %llu MB\n", cardSize);
  
  uint64_t usedBytes = SD.usedBytes() / (1024 * 1024);
  Serial.printf("📊 Used: %llu MB\n", usedBytes);
  
  uint64_t freeBytes = (SD.totalBytes() - SD.usedBytes()) / (1024 * 1024);
  Serial.printf("📂 Free: %llu MB\n", freeBytes);
}
```

---

## 🧪 **Testing Your SD Card Module**

### Test 1: Basic Detection
```cpp
#include <SD.h>
#include <SPI.h>

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("🔍 SD Card Test");
  Serial.println("─────────────────");
  
  if (!SD.begin(5)) {  // GPIO5 = CS
    Serial.println("❌ FAILED");
    return;
  }
  
  Serial.println("✅ SUCCESS");
  Serial.printf("Size: %llu MB\n", SD.cardSize() / 1024 / 1024);
}

void loop() {
  // Nothing
}
```

### Test 2: Read/Write Test
```cpp
void testReadWrite() {
  Serial.println("\n📝 Testing write...");
  
  File file = SD.open("/test.txt", FILE_WRITE);
  if (!file) {
    Serial.println("❌ Failed to open file for writing");
    return;
  }
  
  file.println("Hello from iPod!");
  file.close();
  Serial.println("✅ Write successful");
  
  Serial.println("\n📖 Testing read...");
  file = SD.open("/test.txt");
  if (!file) {
    Serial.println("❌ Failed to open file for reading");
    return;
  }
  
  Serial.print("Content: ");
  while (file.available()) {
    Serial.write(file.read());
  }
  file.close();
  Serial.println("✅ Read successful");
}
```

### Test 3: List Files
```cpp
void listFiles(const char* dirname) {
  Serial.printf("\n📂 Listing: %s\n", dirname);
  
  File root = SD.open(dirname);
  if (!root || !root.isDirectory()) {
    Serial.println("❌ Not a directory");
    return;
  }
  
  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.printf("  📁 %s/\n", file.name());
    } else {
      Serial.printf("  📄 %s (%d bytes)\n", file.name(), file.size());
    }
    file = root.openNextFile();
  }
}
```

---

## 🎵 **Music Library Organization**

### Recommended Folder Structure:
```
SD Card Root:
├── Music/
│   ├── Artist1/
│   │   ├── Album1/
│   │   │   ├── 01 - Song1.mp3
│   │   │   ├── 02 - Song2.mp3
│   │   │   └── cover.jpg
│   │   └── Album2/
│   │       └── ...
│   └── Artist2/
│       └── ...
├── Playlists/
│   ├── favorites.m3u
│   └── workout.m3u
└── Podcasts/
    └── ...
```

### Scanning Music Files:
```cpp
void scanMusicLibrary() {
  Serial.println("🎵 Scanning music library...");
  
  int fileCount = 0;
  scanDirectory("/Music", fileCount);
  
  Serial.printf("✅ Found %d music files\n", fileCount);
}

void scanDirectory(const char* dirname, int& count) {
  File root = SD.open(dirname);
  if (!root || !root.isDirectory()) return;
  
  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      String path = String(dirname) + "/" + file.name();
      scanDirectory(path.c_str(), count);
    } else {
      String filename = file.name();
      if (filename.endsWith(".mp3") || 
          filename.endsWith(".wav") ||
          filename.endsWith(".flac")) {
        Serial.printf("  🎵 %s\n", file.name());
        count++;
      }
    }
    file = root.openNextFile();
  }
}
```

---

## 🐛 **Troubleshooting**

### Problem: "SD Card initialization failed"

**Check 1: Wiring**
```
Verify connections:
✓ VCC → 3.3V (not 5V)
✓ GND → GND
✓ CS  → GPIO5
✓ MOSI → GPIO11
✓ SCK  → GPIO12
✓ MISO → GPIO13
```

**Check 2: Card Inserted**
- Push card in until it clicks
- Card should be flush with module

**Check 3: Card Format**
```
SD card must be formatted as FAT32:
- Windows: Right-click → Format → FAT32
- Mac: Disk Utility → Erase → MS-DOS (FAT)
- Linux: sudo mkfs.vfat -F 32 /dev/sdX1
```

**Check 4: Card Size**
- Use 32GB or smaller
- Larger cards may need exFAT (not supported by default)

**Check 5: SPI Speed**
```cpp
// Try slower SPI speed
SD.begin(SD_CS, SPI, 4000000);  // 4MHz instead of default 25MHz
```

### Problem: "No SD card attached"

**Possible causes:**
1. Card not fully inserted
2. Card contacts dirty (clean with isopropyl alcohol)
3. Card damaged or corrupted
4. Module defective

### Problem: Files not found

**Check:**
1. File names are case-sensitive
2. Use forward slashes: `/Music/song.mp3`
3. No spaces in file names (use underscores)
4. 8.3 format for compatibility: `SONG_001.MP3`

### Problem: Slow read/write

**Solutions:**
1. Use Class 10 or UHS-I cards
2. Increase SPI speed: `SD.begin(SD_CS, SPI, 25000000)`
3. Use larger buffer sizes
4. Avoid frequent small writes

---

## 📊 **Performance Tips**

### Optimize SPI Speed:
```cpp
// Default: 4MHz (safe but slow)
SD.begin(SD_CS);

// Fast: 25MHz (most cards support this)
SD.begin(SD_CS, SPI, 25000000);

// Maximum: 40MHz (test your card)
SD.begin(SD_CS, SPI, 40000000);
```

### Buffer Audio Reads:
```cpp
#define AUDIO_BUFFER_SIZE 4096

uint8_t audioBuffer[AUDIO_BUFFER_SIZE];

void streamAudio(File& audioFile) {
  while (audioFile.available()) {
    size_t bytesRead = audioFile.read(audioBuffer, AUDIO_BUFFER_SIZE);
    // Send to audio decoder
    playAudioBuffer(audioBuffer, bytesRead);
  }
}
```

### Cache File Listings:
```cpp
// Don't scan directory every time
// Cache results in RAM
std::vector<String> musicFiles;

void cacheLibrary() {
  musicFiles.clear();
  scanDirectory("/Music", musicFiles);
}
```

---

## 🛒 **Shopping Info (India)**

```
Component              Price (₹)    Where to Buy
─────────────────────────────────────────────────
SD Card Module         50-100       Robu.in, Amazon, local
Micro SD Card 32GB     300-500      Amazon, Flipkart
Micro SD Card 16GB     200-300      Amazon, Flipkart
Card Reader (USB)      100-200      For formatting/loading
─────────────────────────────────────────────────
Total:                 ~₹650-1100
```

### Recommended SD Cards:
- **SanDisk Ultra 32GB** (₹400-500) - Reliable, fast
- **Samsung EVO Plus 32GB** (₹450-550) - Excellent performance
- **Kingston Canvas Select 32GB** (₹350-450) - Budget option

---

## ✅ **Integration Checklist**

Before powering on:
- [ ] SD card formatted as FAT32
- [ ] VCC connected to 3.3V (not 5V)
- [ ] All 6 pins connected correctly
- [ ] CS on GPIO5 (not shared with display)
- [ ] MOSI/SCK/MISO shared with display
- [ ] Card fully inserted in module
- [ ] Music files copied to card

After power on:
- [ ] Serial monitor shows "SD Card initialized"
- [ ] Card size displayed correctly
- [ ] Can list files
- [ ] Can read test file
- [ ] Music files detected

---

## 🎯 **Next Steps**

1. **Wire the module** following the diagram above
2. **Format SD card** as FAT32
3. **Run basic test** to verify detection
4. **Copy music files** to `/Music` folder
5. **Test file reading** with sample code
6. **Integrate with audio** playback system

Your SD card module is perfect for this project - it has built-in level conversion and voltage regulation, making it super easy to integrate with the ESP32-S3!

