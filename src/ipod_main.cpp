/*
 * Complete ESP32-S3 iPod – Final Robust Version
 * 
 * Hardware:
 * - ILI9341 TFT Display (240x320)
 * - XPT2046 Touch Controller
 * - TTP229 16-key Capacitive Keypad
 * - DRV2605L Haptic Motor Driver
 * - PCM5102A I2S DAC
 * - SD Card Module
 * 
 * Features:
 * - Touch screen UI
 * - 16-key keypad navigation with gestures
 * - Haptic feedback
 * - MP3 playback from SD card
 * - Volume control
 * - Now Playing screen with progress
 * - Music library browser
 */

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <math.h>
#include "AudioFileSourceSD.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2S.h"
#include "AudioLogger.h"

// ═══════════════════════════════════════════════════════════════
// PIN DEFINITIONS
// ═══════════════════════════════════════════════════════════════

// Display (ILI9341)
#define TFT_CS    10
#define TFT_DC    9
#define TFT_RST   8
#define TFT_MOSI  11
#define TFT_SCLK  12
#define TFT_MISO  13

// Touch (XPT2046)
#define TOUCH_CS  6
#define TOUCH_CLK 7
#define TOUCH_DIN 5
#define TOUCH_DO  4
#define TOUCH_IRQ 3

// SD Card – separate SPI bus (FSPI)
#define SD_CS    35
#define SD_MOSI  36
#define SD_MISO  37
#define SD_SCK   38

// PCM5102A DAC (I2S)
#define I2S_BCK  14
#define I2S_LRC  15
#define I2S_DOUT 16

// DRV2605L Haptic (I2C)
#define HAPTIC_SDA 21
#define HAPTIC_SCL 22

// TTP229 Keypad (serial mode)
#define TTP_SDO 1
#define TTP_SCL 2

// ═══════════════════════════════════════════════════════════════
// COLORS
// ═══════════════════════════════════════════════════════════════
#define C_BLACK      0x0000
#define C_WHITE      0xFFFF
#define C_GRAY       0x8410
#define C_DARKGRAY   0x4208
#define C_LIGHTGRAY  0xC618
#define C_BLUE       0x001F
#define C_DARKBLUE   0x000D
#define C_CYAN       0x07FF
#define C_GREEN      0x07E0
#define C_RED        0xF800
#define C_ORANGE     0xFD20
#define C_YELLOW     0xFFE0

#define C_BG         0x1082   // Very dark grey
#define C_HIGHLIGHT  0x3A9F   // iPod blue selection
#define C_TEXT       0xFFFF
#define C_SUBTEXT    0xAD55
#define C_ACCENT     0x5D1C   // Warm silver

// ═══════════════════════════════════════════════════════════════
// DRV2605L REGISTERS
// ═══════════════════════════════════════════════════════════════
#define DRV_ADDR    0x5A
#define DRV_MODE    0x01
#define DRV_LIB     0x03
#define DRV_SEQ1    0x04
#define DRV_GO      0x0C
#define DRV_FEEDBACK 0x1A

// ═══════════════════════════════════════════════════════════════
// GLOBALS
// ═══════════════════════════════════════════════════════════════
TFT_eSPI tft = TFT_eSPI();
SPIClass sdSPI(FSPI);

// State
enum Screen { SCR_MENU, SCR_LIBRARY, SCR_NOWPLAYING, SCR_VOLUME, SCR_SETTINGS };
Screen currentScreen = SCR_MENU;
int selectedItem  = 0;
bool isPlaying    = false;
int  volume       = 75;
int  currentTrack = 0;
bool hapticOK     = false;
bool sdOK         = false;
bool needsRedraw  = true;

// Touch
bool     touchActive    = false;
uint16_t touchX, touchY;
uint32_t lastTouchTime  = 0;

// Keypad gestures
enum KeyGesture {
    GESTURE_NONE, GESTURE_TAP, GESTURE_DOUBLE_TAP, GESTURE_LONG_PRESS,
    GESTURE_SWIPE_UP, GESTURE_SWIPE_DOWN, GESTURE_SWIPE_LEFT,
    GESTURE_SWIPE_RIGHT, GESTURE_CIRCULAR_CW, GESTURE_CIRCULAR_CCW,
    GESTURE_PINCH_IN, GESTURE_PINCH_OUT
};

#define MAX_TOUCH_HISTORY 20
#define TAP_THRESHOLD 200
#define LONG_PRESS_THRESHOLD 500
#define SWIPE_MIN_KEYS 3
#define CIRCULAR_THRESHOLD 5

uint16_t lastKeys = 0;
uint32_t touchStartTime = 0;
uint32_t lastTapTime = 0;
int keyHistory[MAX_TOUCH_HISTORY] = {};
int keyHistoryIndex = 0;
int keyCount = 0;
bool longPressTriggered = false;

// Progress
uint32_t progressStart = 0;
uint32_t trackDuration = 180000;

// Music library
#define MAX_TRACKS 64
#define PATH_LEN  128
char trackList[MAX_TRACKS][PATH_LEN];
int  trackCount = 0;
int  listOffset = 0;

// Audio error handling
int  lastErrorTrack = -1;
int  retryCount = 0;
#define MAX_RETRIES 2

// Forward declarations
void nextTrack();
void prevTrack();
void togglePlay();
void goBack();
void selectItem();
void handleKeyGesture(KeyGesture gesture);
void updatePlaybackProgress();
void audioStop();
void audioPlay();
void sdReinit();

// ═══════════════════════════════════════════════════════════════
// HAPTIC
// ═══════════════════════════════════════════════════════════════
void hapticWrite(uint8_t reg, uint8_t val) {
    Wire.beginTransmission(DRV_ADDR);
    Wire.write(reg); Wire.write(val);
    Wire.endTransmission();
}

void hapticInit() {
    Wire.begin(HAPTIC_SDA, HAPTIC_SCL);
    Wire.beginTransmission(DRV_ADDR);
    hapticOK = (Wire.endTransmission() == 0);
    if (!hapticOK) { Serial.println("Haptic: not found"); return; }
    hapticWrite(DRV_MODE,     0x00);
    hapticWrite(DRV_FEEDBACK, 0x36);
    hapticWrite(DRV_LIB,      1);
    Serial.println("Haptic: OK");
}

void hapticBuzz(uint8_t effect) {
    if (!hapticOK) return;
    hapticWrite(DRV_SEQ1, effect);
    hapticWrite(0x05, 0);
    hapticWrite(DRV_GO, 0x01);
}

// ═══════════════════════════════════════════════════════════════
// SD CARD – safe init & reinit
// ═══════════════════════════════════════════════════════════════
void sdReinit() {
    sdSPI.end();
    sdSPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
    uint32_t speeds[] = {4000000, 8000000, 10000000};
    for (int i = 0; i < 3; i++) {
        sdOK = SD.begin(SD_CS, sdSPI, speeds[i]);
        if (sdOK) {
            Serial.printf("SD reinit OK at %d Hz\n", speeds[i]);
            return;
        }
        sdSPI.end();
        sdSPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
    }
    Serial.println("SD reinit failed");
}

void sdInit() {
    sdReinit(); // use the same logic
}

bool isMusicFile(const char* name) {
    const char* base = strrchr(name, '/');
    base = base ? base + 1 : name;
    if (base[0] == '.') return false;
    const char* ext = strrchr(name, '.');
    if (!ext) return false;
    return strcasecmp(ext, ".mp3") == 0;
}

void scanMusic(File dir, const String& prefix = "/", int depth = 0) {
    while (trackCount < MAX_TRACKS) {
        File f = dir.openNextFile();
        if (!f) break;
        String name = f.name();
        String fullPath;
        if (name.startsWith("/")) {
            fullPath = name;
        } else {
            fullPath = prefix;
            if (!fullPath.endsWith("/")) fullPath += "/";
            fullPath += name;
        }
        if (f.isDirectory() && depth < 2) {
            scanMusic(f, fullPath, depth + 1);
        } else if (isMusicFile(f.name())) {
            strlcpy(trackList[trackCount], fullPath.c_str(), PATH_LEN);
            trackCount++;
        }
        f.close();
    }
}

void loadMusicLibrary() {
    trackCount = 0;
    if (!sdOK) return;
    File root = SD.open("/");
    scanMusic(root);
    root.close();
    Serial.printf("Music: %d tracks found\n", trackCount);
    for (int i = 0; i < min(5, trackCount); i++) {
        Serial.printf("  [%d] %s\n", i, trackList[i]);
    }
}

// ═══════════════════════════════════════════════════════════════
// I2S / PCM5102A DAC
// ═══════════════════════════════════════════════════════════════
AudioFileSourceSD* audioFile   = nullptr;
AudioGeneratorMP3* mp3         = nullptr;
AudioOutputI2S*    audioOut    = nullptr;

void audioInit() {
    audioLogger = &Serial;
    audioOut = new AudioOutputI2S();
    audioOut->SetPinout(I2S_BCK, I2S_LRC, I2S_DOUT);
    audioOut->SetGain((float)volume / 100.0f);
    Serial.println("I2S DAC: OK");
}

void audioSetVolume() {
    if (audioOut) audioOut->SetGain((float)volume / 100.0f);
}

void audioStop() {
    if (mp3) {
        mp3->stop();
        delete mp3;        mp3 = nullptr;
    }
    if (audioFile) {
        audioFile->close();
        delete audioFile;  audioFile = nullptr;
    }
    isPlaying = false;
    lastErrorTrack = -1;
    retryCount = 0;
}

// Test read a few bytes to verify the file is accessible
bool testFileRead(AudioFileSourceSD* src) {
    uint8_t buf[16];
    int bytes = src->read(buf, 16);
    src->seek(0, SEEK_SET);
    if (bytes != 16) {
        Serial.printf("Read test failed: got %d bytes\n", bytes);
        return false;
    }
    return true;
}

void audioPlay() {
    if (!sdOK || trackCount == 0) return;

    // Prevent infinite loops on a single broken track
    if (lastErrorTrack == currentTrack && retryCount >= MAX_RETRIES) {
        Serial.printf("Skipping permanently broken track %d\n", currentTrack);
        int next = (currentTrack + 1) % trackCount;
        if (next == currentTrack) return;
        currentTrack = next;
        retryCount = 0;
        audioPlay();
        return;
    }

    audioStop();

    // Do NOT use SD.exists() – it may be unreliable after errors.
    // Just try to open and handle failure.
    audioFile = new AudioFileSourceSD(trackList[currentTrack]);
    if (!audioFile->isOpen()) {
        Serial.printf("Failed to open: %s\n", trackList[currentTrack]);
        delete audioFile;
        audioFile = nullptr;
        lastErrorTrack = currentTrack;
        retryCount++;
        int next = (currentTrack + 1) % trackCount;
        if (next != currentTrack) {
            currentTrack = next;
            audioPlay();
        }
        return;
    }

    // Verify the file is readable
    if (!testFileRead(audioFile)) {
        Serial.printf("File not readable: %s\n", trackList[currentTrack]);
        audioFile->close();
        delete audioFile;
        audioFile = nullptr;
        lastErrorTrack = currentTrack;
        retryCount++;
        int next = (currentTrack + 1) % trackCount;
        if (next != currentTrack) {
            currentTrack = next;
            audioPlay();
        } else {
            isPlaying = false;
        }
        return;
    }

    mp3 = new AudioGeneratorMP3();

    Serial.printf("Audio file size: %lu bytes\n", (unsigned long)audioFile->getSize());

    if (mp3->begin(audioFile, audioOut)) {
        isPlaying     = true;
        progressStart = millis();
        trackDuration = 180000;
        lastErrorTrack = -1;
        retryCount = 0;
        Serial.printf("Playing: %s\n", trackList[currentTrack]);
    } else {
        Serial.printf("Failed to start decoder: %s\n", trackList[currentTrack]);
        delete mp3;
        mp3 = nullptr;
        audioFile->close();
        delete audioFile;
        audioFile = nullptr;
        lastErrorTrack = currentTrack;
        retryCount++;
        int next = (currentTrack + 1) % trackCount;
        if (next != currentTrack) {
            currentTrack = next;
            audioPlay();
        } else {
            isPlaying = false;
        }
    }
}

void audioLoop() {
    if (mp3 && mp3->isRunning()) {
        if (!mp3->loop()) {
            uint32_t pos = 0;
            uint32_t size = 0;
            if (audioFile) {
                pos = audioFile->getPos();
                size = audioFile->getSize();
            }
            Serial.printf("Decoder stopped: pos=%lu, size=%lu\n", pos, size);

            // Fatal read error – skip this track and reinit SD
            if (pos == 0xFFFFFFFF) {
                Serial.println("Fatal read error – skipping track and reinitializing SD");
                audioStop();
                lastErrorTrack = currentTrack;
                retryCount = 0;
                sdReinit();                       // Reset SD library
                loadMusicLibrary();                // Reload list (in case paths changed)
                if (trackCount <= 0) {
                    currentTrack = 0;
                    isPlaying = false;
                    if (currentScreen == SCR_NOWPLAYING) drawNowPlaying();
                    return;
                }

                currentTrack %= trackCount;
                nextTrack();
                if (currentScreen == SCR_NOWPLAYING) drawNowPlaying();
                return;
            }

            // Normal end-of-file (within 4 KB of end)
            bool reachedEnd = (size > 0 && pos >= size - min(size, (uint32_t)4096));
            if (reachedEnd) {
                Serial.println("Track ended, advancing");
                nextTrack();
            } else {
                Serial.println("MP3 decoding error – stopping");
                lastErrorTrack = currentTrack;
                retryCount++;
                audioStop();
                if (currentScreen == SCR_NOWPLAYING) {
                    tft.fillRect(0, 44, 240, 220, C_BG);
                    tft.setTextColor(C_RED); tft.setTextSize(2);
                    tft.setCursor(20, 120);
                    tft.print("Playback Error");
                    tft.setTextSize(1);
                    tft.setCursor(20, 150);
                    tft.print("Invalid MP3 file");
                    tft.setCursor(20, 170);
                    tft.print("Skipping...");
                    delay(1500);
                    int next = (currentTrack + 1) % trackCount;
                    if (next != currentTrack) {
                        currentTrack = next;
                        audioPlay();
                    } else {
                        isPlaying = false;
                    }
                    if (currentScreen == SCR_NOWPLAYING) drawNowPlaying();
                }
            }
        }
    }
}

// ═══════════════════════════════════════════════════════════════
// TTP229 KEYPAD (serial mode)
// ═══════════════════════════════════════════════════════════════
void ttpInit() {
    pinMode(TTP_SDO, INPUT);
    pinMode(TTP_SCL, OUTPUT);
    digitalWrite(TTP_SCL, HIGH);
    Serial.println("TTP229: serial mode");
}

uint16_t ttpRead() {
    uint16_t value = 0;
    digitalWrite(TTP_SCL, HIGH);
    delayMicroseconds(100);
    for (int i = 0; i < 16; i++) {
        digitalWrite(TTP_SCL, LOW);
        delayMicroseconds(5);
        value <<= 1;
        if (digitalRead(TTP_SDO)) value |= 1;
        digitalWrite(TTP_SCL, HIGH);
        delayMicroseconds(5);
    }
    return ~value;
}

// ═══════════════════════════════════════════════════════════════
// TOUCH
// ═══════════════════════════════════════════════════════════════
void readTouch() {
    touchActive = false;
    if (!tft.getTouch(&touchX, &touchY)) return;
    if (millis() - lastTouchTime < 200) return;
    touchActive  = true;
    lastTouchTime = millis();
}

// ═══════════════════════════════════════════════════════════════
// UI HELPER FUNCTIONS
// ═══════════════════════════════════════════════════════════════
void drawStatusBar() {
    tft.fillRect(0, 0, 240, 22, C_DARKGRAY);
    tft.setTextColor(C_WHITE);
    tft.setTextSize(1);
    tft.setCursor(80, 7);
    tft.print("iPod");
    tft.setCursor(4, 7);
    tft.setTextColor(sdOK ? C_GREEN : C_RED);
    tft.print(sdOK ? "SD" : "--");
    tft.setTextColor(C_SUBTEXT);
    tft.setCursor(180, 7);
    tft.printf("vol %d", volume);
}

void drawNavBar(const char* hint1, const char* hint2) {
    tft.fillRect(0, 298, 240, 22, C_DARKGRAY);
    tft.setTextSize(1);
    tft.setTextColor(C_SUBTEXT);
    tft.setCursor(4,   307);
    tft.print(hint1);
    tft.setCursor(160, 307);
    tft.print(hint2);
}

void drawMenuItem(int index, const char* label, bool selected, bool hasArrow = true) {
    int y = 28 + index * 36;
    if (selected) {
        tft.fillRoundRect(4, y, 232, 32, 6, C_HIGHLIGHT);
    } else {
        tft.fillRoundRect(4, y, 232, 32, 6, C_DARKGRAY);
    }
    tft.setTextColor(C_WHITE);
    tft.setTextSize(2);
    tft.setCursor(14, y + 8);
    tft.print(label);
    if (hasArrow && selected) {
        tft.setCursor(208, y + 8);
        tft.print(">");
    }
}

void shortName(const char* src, char* dst, int maxLen) {
    const char* base = strrchr(src, '/');
    base = base ? base + 1 : src;
    strncpy(dst, base, maxLen);
    dst[maxLen] = 0;
    char* dot = strrchr(dst, '.');
    if (dot) *dot = 0;
}

// ═══════════════════════════════════════════════════════════════
// SCREENS
// ═══════════════════════════════════════════════════════════════
void drawMenu() {
    tft.fillScreen(C_BG);
    drawStatusBar();
    const char* items[] = {"Music Library", "Now Playing", "Volume", "Settings"};
    for (int i = 0; i < 4; i++) {
        drawMenuItem(i, items[i], i == selectedItem);
    }
    drawNavBar("UP/DN: navigate", "SELECT: open");
}

void drawLibrary() {
    tft.fillScreen(C_BG);
    drawStatusBar();
    tft.fillRect(0, 22, 240, 20, C_ACCENT);
    tft.setTextColor(C_WHITE); tft.setTextSize(1);
    tft.setCursor(8, 29);
    tft.printf("Music Library  [%d tracks]", trackCount);

    if (trackCount == 0) {
        tft.setTextColor(C_SUBTEXT); tft.setTextSize(1);
        tft.setCursor(20, 140);
        tft.print(sdOK ? "No music files found" : "SD card not found");
        tft.setCursor(20, 158);
        tft.print("Put .mp3 files on SD card");
        drawNavBar("BACK: menu", "");
        return;
    }

    int visibleItems = 7;
    for (int i = 0; i < visibleItems && (i + listOffset) < trackCount; i++) {
        int trackIdx = i + listOffset;
        int y = 44 + i * 36;
        bool sel = (trackIdx == selectedItem);
        bool playing = (trackIdx == currentTrack && isPlaying);

        if (sel)     tft.fillRoundRect(4, y, 232, 32, 5, C_HIGHLIGHT);
        else if (playing) tft.fillRoundRect(4, y, 232, 32, 5, 0x0820);
        else         tft.fillRoundRect(4, y, 232, 32, 5, C_DARKGRAY);

        char name[40];
        shortName(trackList[trackIdx], name, 39);
        tft.setTextColor(C_WHITE); tft.setTextSize(1);
        tft.setCursor(12, y + 6);
        if (strlen(name) > 28) name[28] = 0;
        tft.print(name);

        if (playing) {
            tft.setTextColor(C_GREEN);
            tft.setCursor(210, y + 6);
            tft.print((char)0x10);
        }
    }

    if (trackCount > visibleItems) {
        int sbH = 7 * 36;
        int barH = max(12, sbH * visibleItems / trackCount);
        int barY = 44 + (sbH - barH) * listOffset / (trackCount > visibleItems ? trackCount - visibleItems : 1);
        tft.fillRect(234, 44, 4, sbH, C_DARKGRAY);
        tft.fillRect(234, barY, 4, barH, C_HIGHLIGHT);
    }
    drawNavBar("BACK  UP/DN", "PLAY: select");
}

void drawNowPlaying() {
    tft.fillScreen(C_BG);
    drawStatusBar();
    tft.fillRect(0, 22, 240, 18, C_ACCENT);
    tft.setTextColor(C_WHITE); tft.setTextSize(1);
    tft.setCursor(80, 27);
    tft.print("Now Playing");

    tft.fillRoundRect(60, 44, 120, 110, 12, C_DARKGRAY);
    tft.drawRoundRect(60, 44, 120, 110, 12, C_GRAY);
    tft.setTextColor(C_GRAY); tft.setTextSize(4);
    tft.setCursor(100, 70);
    tft.print(">>");

    if (isPlaying) {
        uint32_t t = millis();
        for (int b = 0; b < 5; b++) {
            int bh = 8 + (int)(10.0f * abs(sinf((t / 200.0f) + b * 0.7f)));
            tft.fillRect(65 + b * 8, 138 - bh, 5, bh, C_GREEN);
        }
    }

    char name[40] = "No track";
    if (trackCount > 0 && currentTrack < trackCount) {
        shortName(trackList[currentTrack], name, 39);
        if (strlen(name) > 22) name[22] = 0;
    }
    tft.setTextColor(C_WHITE); tft.setTextSize(2);
    int nameX = (240 - (int)(strlen(name) * 12)) / 2;
    tft.setCursor(nameX < 4 ? 4 : nameX, 162);
    tft.print(name);

    tft.setTextColor(C_SUBTEXT); tft.setTextSize(1);
    tft.setCursor(4, 184);
    tft.printf("%d / %d", currentTrack + 1, (trackCount > 0 ? trackCount : 1));

    uint32_t elapsed = isPlaying ? (millis() - progressStart) : 0;
    float prog = min(1.0f, (float)elapsed / (float)(trackDuration > 0 ? trackDuration : 1));
    tft.fillRect(10, 198, 220, 8, C_DARKGRAY);
    tft.fillRect(10, 198, (int)(220 * prog), 8, C_HIGHLIGHT);
    tft.drawRect(10, 198, 220, 8, C_GRAY);

    tft.setTextColor(C_SUBTEXT); tft.setTextSize(1);
    tft.setCursor(10, 210);
    tft.printf("%d:%02d", (int)(elapsed/60000), (int)((elapsed/1000)%60));
    tft.setCursor(190, 210);
    tft.printf("%d:%02d", (int)(trackDuration/60000), (int)((trackDuration/1000)%60));

    int cy = 230;
    tft.fillTriangle(30, cy+15, 50, cy, 50, cy+30, isPlaying ? C_WHITE : C_GRAY);
    tft.fillTriangle(18, cy+15, 30, cy, 30, cy+30, isPlaying ? C_WHITE : C_GRAY);
    if (isPlaying) {
        tft.fillRect(106, cy, 8, 30, C_WHITE);
        tft.fillRect(120, cy, 8, 30, C_WHITE);
    } else {
        tft.fillTriangle(106, cy, 134, cy+15, 106, cy+30, C_GREEN);
    }
    tft.fillTriangle(170, cy, 190, cy+15, 170, cy+30, isPlaying ? C_WHITE : C_GRAY);
    tft.fillTriangle(190, cy, 210, cy+15, 190, cy+30, isPlaying ? C_WHITE : C_GRAY);

    tft.setTextColor(C_SUBTEXT); tft.setTextSize(1);
    tft.setCursor(10, 272);
    tft.print("Vol:");
    tft.fillRect(36, 274, 130, 6, C_DARKGRAY);
    tft.fillRect(36, 274, 130 * volume / 100, 6, C_HIGHLIGHT);

    drawNavBar("BACK", "PREV  PLAY  NEXT");
}

void updatePlaybackProgress() {
    if (currentScreen != SCR_NOWPLAYING || !isPlaying) return;
    uint32_t elapsed = millis() - progressStart;
    float prog = min(1.0f, (float)elapsed / (float)(trackDuration > 0 ? trackDuration : 1));
    tft.fillRect(10, 198, 220, 8, C_DARKGRAY);
    tft.fillRect(10, 198, (int)(220 * prog), 8, C_HIGHLIGHT);
    tft.drawRect(10, 198, 220, 8, C_GRAY);
    tft.fillRect(4, 208, 232, 14, C_BG);
    tft.setTextColor(C_SUBTEXT);
    tft.setTextSize(1);
    tft.setCursor(10, 210);
    tft.printf("%d:%02d", (int)(elapsed / 60000), (int)((elapsed / 1000) % 60));
    tft.setCursor(190, 210);
    tft.printf("%d:%02d", (int)(trackDuration / 60000), (int)((trackDuration / 1000) % 60));
}

void drawVolumeScreen() {
    tft.fillScreen(C_BG);
    drawStatusBar();
    tft.setTextColor(C_WHITE); tft.setTextSize(3);
    tft.setCursor(40, 80);
    tft.print("Volume");
    tft.setTextSize(6);
    tft.setCursor(volume >= 100 ? 50 : 70, 130);
    tft.printf("%d", volume);
    tft.fillRect(20, 210, 200, 20, C_DARKGRAY);
    tft.fillRect(20, 210, 200 * volume / 100, 20, C_HIGHLIGHT);
    tft.drawRect(20, 210, 200, 20, C_GRAY);
    tft.setTextColor(C_SUBTEXT); tft.setTextSize(1);
    tft.setCursor(20, 240);
    tft.print("0");
    tft.setCursor(200, 240);
    tft.print("100");
    drawNavBar("BACK", "UP/DN: adjust");
}

void drawSettings() {
    tft.fillScreen(C_BG);
    drawStatusBar();
    tft.setTextColor(C_WHITE); tft.setTextSize(2);
    tft.setCursor(60, 40);
    tft.print("Settings");
    tft.setTextSize(1);
    tft.setTextColor(C_SUBTEXT);
    tft.setCursor(14, 90);
    tft.printf("SD Card:   %s", sdOK ? "Connected" : "Not found");
    tft.setCursor(14, 110);
    tft.printf("Haptic:    %s", hapticOK ? "Active" : "Not found");
    tft.setCursor(14, 130);
    tft.printf("Tracks:    %d", trackCount);
    tft.setCursor(14, 150);
    tft.printf("Volume:    %d%%", volume);
    tft.setCursor(14, 170);
    tft.printf("I2S DAC:   PCM5102A");
    tft.setCursor(14, 190);
    tft.printf("Display:   ILI9341 240x320");
    drawNavBar("BACK", "");
}

void drawScreen() {
    switch (currentScreen) {
        case SCR_MENU:       drawMenu();       break;
        case SCR_LIBRARY:    drawLibrary();    break;
        case SCR_NOWPLAYING: drawNowPlaying(); break;
        case SCR_VOLUME:     drawVolumeScreen(); break;
        case SCR_SETTINGS:   drawSettings();   break;
    }
    needsRedraw = false;
}

// ═══════════════════════════════════════════════════════════════
// ACTIONS
// ═══════════════════════════════════════════════════════════════
void goBack() {
    hapticBuzz(10);
    if (currentScreen == SCR_MENU) return;
    currentScreen = SCR_MENU;
    selectedItem  = 0;
    needsRedraw   = true;
}

void navigateUp() {
    hapticBuzz(1);
    if (currentScreen == SCR_LIBRARY) {
        if (selectedItem > 0) {
            selectedItem--;
            if (selectedItem < listOffset) listOffset = selectedItem;
        }
    } else if (currentScreen == SCR_MENU) {
        selectedItem = (selectedItem - 1 + 4) % 4;
    } else if (currentScreen == SCR_VOLUME) {
        volume = min(100, volume + 5);
    }
    needsRedraw = true;
}

void navigateDown() {
    hapticBuzz(1);
    if (currentScreen == SCR_LIBRARY) {
        if (selectedItem < trackCount - 1) {
            selectedItem++;
            if (selectedItem >= listOffset + 7) listOffset = selectedItem - 6;
        }
    } else if (currentScreen == SCR_MENU) {
        selectedItem = (selectedItem + 1) % 4;
    } else if (currentScreen == SCR_VOLUME) {
        volume = max(0, volume - 5);
    }
    needsRedraw = true;
}

void selectItem() {
    hapticBuzz(2);
    if (currentScreen == SCR_MENU) {
        switch (selectedItem) {
            case 0:
                currentScreen = SCR_LIBRARY;
                selectedItem  = currentTrack;
                listOffset    = max(0, currentTrack - 3);
                break;
            case 1: currentScreen = SCR_NOWPLAYING; break;
            case 2: currentScreen = SCR_VOLUME;     break;
            case 3: currentScreen = SCR_SETTINGS;   break;
        }
    } else if (currentScreen == SCR_LIBRARY) {
        if (trackCount > 0) {
            currentTrack   = selectedItem;
            currentScreen  = SCR_NOWPLAYING;
            audioPlay();
        }
    }
    needsRedraw = true;
}

void togglePlay() {
    hapticBuzz(isPlaying ? 1 : 2);
    if (isPlaying) {
        audioStop();
    } else if (trackCount > 0) {
        audioPlay();
    }
    needsRedraw = true;
}

void prevTrack() {
    hapticBuzz(3);
    if (trackCount == 0) return;
    currentTrack  = (currentTrack - 1 + trackCount) % trackCount;
    audioPlay();
    needsRedraw   = true;
}

void nextTrack() {
    hapticBuzz(3);
    if (trackCount == 0) return;
    int next = (currentTrack + 1) % trackCount;
    if (next == currentTrack) {
        audioStop();
        needsRedraw = true;
        return;
    }
    currentTrack = next;
    audioPlay();
    needsRedraw = true;
}

// ═══════════════════════════════════════════════════════════════
// TOUCH HANDLING
// ═══════════════════════════════════════════════════════════════
void handleTouch() {
    if (!touchActive) return;
    int tx = touchX, ty = touchY;
    if (ty < 22) { goBack(); return; }
    if (currentScreen == SCR_NOWPLAYING) {
        if (ty >= 228 && ty <= 262) {
            if      (tx < 80)  prevTrack();
            else if (tx < 160) togglePlay();
            else               nextTrack();
            return;
        }
    }
    if (currentScreen == SCR_MENU) {
        int row = (ty - 28) / 36;
        if (row >= 0 && row < 4) {
            selectedItem = row;
            selectItem();
        }
    } else if (currentScreen == SCR_LIBRARY) {
        int row = (ty - 44) / 36;
        if (row >= 0 && row < 7) {
            int track = row + listOffset;
            if (track < trackCount) {
                selectedItem = track;
                selectItem();
            }
        }
    } else if (currentScreen == SCR_VOLUME) {
        if (ty >= 208 && ty <= 232) {
            volume = constrain((tx - 20) * 100 / 200, 0, 100);
            audioSetVolume();
            needsRedraw = true;
        }
    }
}

// ═══════════════════════════════════════════════════════════════
// KEYPAD HANDLING (gesture recognition – unchanged)
// ═══════════════════════════════════════════════════════════════
void addKeyHistory(uint16_t keys) {
    for (int i = 0; i < 16; i++) {
        if ((keys >> i) & 1) {
            keyHistory[keyHistoryIndex] = i + 1;
            keyHistoryIndex = (keyHistoryIndex + 1) % MAX_TOUCH_HISTORY;
            break;
        }
    }
}

void clearKeyHistory() {
    memset(keyHistory, 0, sizeof(keyHistory));
    keyHistoryIndex = 0;
    keyCount = 0;
}

KeyGesture detectCircularGesture(const int* keys, int count) {
    float turn = 0.0f;
    for (int i = 1; i < count; i++) {
        int a = keys[i - 1] - 1;
        int b = keys[i] - 1;
        int r1 = a / 4, c1 = a % 4;
        int r2 = b / 4, c2 = b % 4;
        float a1 = atan2f((float)r1 - 1.5f, (float)c1 - 1.5f);
        float a2 = atan2f((float)r2 - 1.5f, (float)c2 - 1.5f);
        float delta = a2 - a1;
        if (delta > 3.14159265f) delta -= 6.28318530f;
        if (delta < -3.14159265f) delta += 6.28318530f;
        turn += delta;
    }
    if (turn > 2.0f) return GESTURE_CIRCULAR_CW;
    if (turn < -2.0f) return GESTURE_CIRCULAR_CCW;
    return GESTURE_NONE;
}

KeyGesture detectPinchGesture(const int* keys, int count) {
    if (count < 3) return GESTURE_NONE;
    int first = keys[0], last = keys[count - 1];
    bool firstCorner = first == 1 || first == 4 || first == 13 || first == 16;
    bool lastCorner = last == 1 || last == 4 || last == 13 || last == 16;
    bool firstCenter = first == 6 || first == 7 || first == 10 || first == 11;
    bool lastCenter = last == 6 || last == 7 || last == 10 || last == 11;
    if (firstCorner && lastCenter) return GESTURE_PINCH_IN;
    if (firstCenter && lastCorner) return GESTURE_PINCH_OUT;
    return GESTURE_NONE;
}

KeyGesture detectKeyGesture(uint32_t duration) {
    int keys[MAX_TOUCH_HISTORY];
    int count = 0;
    for (int i = 0; i < MAX_TOUCH_HISTORY; i++) {
        if (keyHistory[i]) keys[count++] = keyHistory[i];
    }
    if (count == 0) return GESTURE_NONE;
    if (count == 1) {
        if (duration < TAP_THRESHOLD) {
            if (millis() - lastTapTime < 500) {
                lastTapTime = 0;
                return GESTURE_DOUBLE_TAP;
            }
            lastTapTime = millis();
        }
        return GESTURE_TAP;
    }
    if (count >= SWIPE_MIN_KEYS) {
        if (count >= CIRCULAR_THRESHOLD) {
            KeyGesture circular = detectCircularGesture(keys, count);
            if (circular != GESTURE_NONE) return circular;
        }
        KeyGesture pinch = detectPinchGesture(keys, count);
        if (pinch != GESTURE_NONE) return pinch;

        int first = keys[0], last = keys[count - 1];
        int r1 = (first - 1) / 4, c1 = (first - 1) % 4;
        int r2 = (last - 1) / 4, c2 = (last - 1) % 4;
        if (abs(r2 - r1) > abs(c2 - c1))
            return r2 > r1 ? GESTURE_SWIPE_DOWN : GESTURE_SWIPE_UP;
        if (abs(c2 - c1) > abs(r2 - r1))
            return c2 > c1 ? GESTURE_SWIPE_RIGHT : GESTURE_SWIPE_LEFT;
    }
    return GESTURE_NONE;
}

void handleKeyGesture(KeyGesture gesture) {
    switch (gesture) {
        case GESTURE_TAP:          Serial.println("Gesture: TAP"); selectItem(); break;
        case GESTURE_DOUBLE_TAP:   Serial.println("Gesture: DOUBLE TAP"); togglePlay(); break;
        case GESTURE_LONG_PRESS:   Serial.println("Gesture: LONG PRESS"); goBack(); break;
        case GESTURE_SWIPE_UP:     Serial.println("Gesture: SWIPE UP"); navigateUp(); break;
        case GESTURE_SWIPE_DOWN:   Serial.println("Gesture: SWIPE DOWN"); navigateDown(); break;
        case GESTURE_SWIPE_LEFT:   Serial.println("Gesture: SWIPE LEFT"); prevTrack(); break;
        case GESTURE_SWIPE_RIGHT:  Serial.println("Gesture: SWIPE RIGHT"); nextTrack(); break;
        case GESTURE_CIRCULAR_CW:  volume = min(100, volume + 5); audioSetVolume(); needsRedraw = true; break;
        case GESTURE_CIRCULAR_CCW: volume = max(0, volume - 5); audioSetVolume(); needsRedraw = true; break;
        case GESTURE_PINCH_IN:
        case GESTURE_PINCH_OUT:    Serial.println("Gesture: PINCH (no action)"); break;
        default: break;
    }
}

void handleKeypad() {
    uint16_t keys = ttpRead();
    if (keys != lastKeys) {
        if (keys != 0) {
            if (lastKeys == 0) {
                touchStartTime = millis();
                longPressTriggered = false;
            }
            addKeyHistory(keys);
            keyCount++;
        } else if (!longPressTriggered) {
            handleKeyGesture(detectKeyGesture(millis() - touchStartTime));
            clearKeyHistory();
        }
        lastKeys = keys;
    }

    if (keys != 0 && !longPressTriggered && keyCount == 1 &&
        millis() - touchStartTime > LONG_PRESS_THRESHOLD) {
        handleKeyGesture(GESTURE_LONG_PRESS);
        longPressTriggered = true;
    }
}

void printControls() {
    Serial.println("🎮 Gesture Controls:");
    Serial.println("   Tap: Select");
    Serial.println("   Double Tap: Play/Pause");
    Serial.println("   Long Press: Back/Menu");
    Serial.println("   Swipe Up/Down: Navigate");
    Serial.println("   Swipe Left/Right: Previous/Next Track");
    Serial.println("   Circular CW/CCW: Volume Up/Down");
    Serial.println();
    Serial.println("📱 16-Key Layout:");
    Serial.println("   ┌────┬────┬────┬────┐");
    Serial.println("   │ 1  │ 2  │ 3  │ 4  │");
    Serial.println("   ├────┼────┼────┼────┤");
    Serial.println("   │ 5  │ 6  │ 7  │ 8  │");
    Serial.println("   ├────┼────┼────┼────┤");
    Serial.println("   │ 9  │ 10 │ 11 │ 12 │");
    Serial.println("   ├────┼────┼────┼────┤");
    Serial.println("   │ 13 │ 14 │ 15 │ 16 │");
    Serial.println("   └────┴────┴────┴────┘");
    Serial.println();
}

// ═══════════════════════════════════════════════════════════════
// SPLASH
// ═══════════════════════════════════════════════════════════════
void showSplash() {
    tft.fillScreen(C_BLACK);
    tft.fillCircle(120, 110, 50, C_WHITE);
    tft.fillCircle(120, 110, 44, C_BG);
    tft.setTextColor(C_WHITE); tft.setTextSize(4);
    tft.setCursor(62, 94);
    tft.print("iPod");
    tft.setTextColor(C_SUBTEXT); tft.setTextSize(1);
    tft.setCursor(60, 172);
    tft.print("ESP32-S3 Edition");
    hapticBuzz(47);
    delay(1800);
    tft.fillScreen(C_BG);
}

// ═══════════════════════════════════════════════════════════════
// SETUP & LOOP
// ═══════════════════════════════════════════════════════════════
void setup() {
    Serial.begin(115200);
    unsigned long t0 = millis();
    while (!Serial && millis() - t0 < 3000);

    Serial.println("\n=== ESP32-S3 iPod ===");

    tft.init();
    tft.setRotation(0);
    tft.fillScreen(C_BG);
    tft.setTextWrap(false);
    Serial.println("Display: OK");

    hapticInit();
    sdInit();
    loadMusicLibrary();
    audioInit();
    ttpInit();

    showSplash();
    drawScreen();
    Serial.println("Ready.");
    printControls();
}

void loop() {
    audioLoop();

    readTouch();
    handleTouch();

    static uint32_t lastKpoll = 0;
    if (millis() - lastKpoll > 20) {
        handleKeypad();
        lastKpoll = millis();
    }

    if (needsRedraw) {
        drawScreen();
    }

    if (currentScreen == SCR_NOWPLAYING && isPlaying) {
        static uint32_t lastProgressUpdate = 0;
        if (millis() - lastProgressUpdate > 1000) {
            lastProgressUpdate = millis();
            updatePlaybackProgress();
        }
    }

    delay(10);
}
