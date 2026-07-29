/*
 * DRV2605L Haptic Diagnostic
 * 
 * Step 1: Scans common I2C pin pairs to FIND the DRV2605L
 * Step 2: Once found, fires haptic effects so you can feel it
 * 
 * WIRING (DRV2605L module):
 *   VCC  → 3.3V
 *   GND  → GND
 *   SDA  → try GPIO 21 first (or any free GPIO)
 *   SCL  → try GPIO 22 first (or any free GPIO)
 *   (no other pins needed on the module)
 */

#include <Arduino.h>
#include <Wire.h>

#define DRV2605L_ADDR   0x5A
#define REG_MODE        0x01
#define REG_LIBRARY     0x03
#define REG_WAVESEQ1    0x04
#define REG_GO          0x0C
#define REG_FEEDBACK    0x1A
#define REG_STATUS      0x00

// Pairs to try: {SDA, SCL}
// Covers the most common free pins on ESP32-S3
const int I2C_PAIRS[][2] = {
    {21, 22},
    {41, 42},
    {8,  9},
    {1,  2},
    {3,  4},
    {5,  6},
    {39, 40},
    {35, 36},
    {47, 48},
};
const int NUM_PAIRS = sizeof(I2C_PAIRS) / sizeof(I2C_PAIRS[0]);

int foundSDA = -1;
int foundSCL = -1;

// ── helpers ──────────────────────────────────────────────────────────────────

void writeReg(uint8_t reg, uint8_t val) {
    Wire.beginTransmission(DRV2605L_ADDR);
    Wire.write(reg);
    Wire.write(val);
    Wire.endTransmission();
}

uint8_t readReg(uint8_t reg) {
    Wire.beginTransmission(DRV2605L_ADDR);
    Wire.write(reg);
    Wire.endTransmission();
    Wire.requestFrom((uint8_t)DRV2605L_ADDR, (uint8_t)1);
    return Wire.available() ? Wire.read() : 0xFF;
}

bool probeAddress() {
    Wire.beginTransmission(DRV2605L_ADDR);
    return Wire.endTransmission() == 0;
}

// ── DRV2605L init (works for both ERM coin motors AND LRA) ───────────────────

void initDRV2605L() {
    writeReg(REG_MODE, 0x00);       // out of standby
    delay(10);
    writeReg(REG_MODE, 0x00);       // internal trigger
    writeReg(0x02, 0x00);           // no real-time playback

    uint8_t status = readReg(REG_STATUS);
    Serial.printf("   Status reg: 0x%02X\n", status);

    // Auto-detect motor type from status
    bool isLRA = (status & 0x08) != 0;  // bit 3 = LRA detect (rough check)
    Serial.printf("   Motor type guess: %s\n", isLRA ? "LRA" : "ERM (coin motor)");

    if (isLRA) {
        writeReg(REG_FEEDBACK, 0xB6); // LRA mode, closed-loop
        writeReg(REG_LIBRARY, 6);     // LRA library
        writeReg(0x1B, 0x93);         // drive time
        writeReg(0x1C, 0xF5);         // blanking time
        writeReg(0x16, 0x3E);         // rated voltage ~1.2V
        writeReg(0x17, 0x8C);         // clamp voltage ~3V
    } else {
        writeReg(REG_FEEDBACK, 0x36); // ERM mode
        writeReg(REG_LIBRARY, 1);     // ERM library
    }
}

void buzz(uint8_t effect) {
    writeReg(REG_WAVESEQ1, effect);
    writeReg(0x05, 0);              // terminate sequence
    writeReg(REG_GO, 0x01);         // fire
}

// ── setup ────────────────────────────────────────────────────────────────────

void setup() {
    Serial.begin(115200);
    delay(1500);

    Serial.println("\n==============================");
    Serial.println("  DRV2605L Haptic Diagnostic");
    Serial.println("==============================\n");

    // ── Step 1: find the DRV2605L ──────────────────────────────────────────
    Serial.println("Scanning I2C pin pairs for DRV2605L (0x5A)...\n");

    for (int i = 0; i < NUM_PAIRS; i++) {
        int sda = I2C_PAIRS[i][0];
        int scl = I2C_PAIRS[i][1];

        Wire.end();
        Wire.begin(sda, scl);
        Wire.setClock(100000);
        delay(10);

        Serial.printf("  SDA=GPIO%-2d  SCL=GPIO%-2d  ... ", sda, scl);

        if (probeAddress()) {
            Serial.println("FOUND! ✓");
            foundSDA = sda;
            foundSCL = scl;
            break;
        } else {
            Serial.println("not found");
        }
    }

    if (foundSDA == -1) {
        Serial.println("\n❌  DRV2605L not found on any pin pair.");
        Serial.println("Check:");
        Serial.println("  1. VCC → 3.3V  (not 5V directly)");
        Serial.println("  2. GND → GND");
        Serial.println("  3. SDA and SCL not swapped");
        Serial.println("  4. Pull-up resistors (4.7kΩ SDA→3.3V, SCL→3.3V)");
        Serial.println("     (some breakout boards have them built-in)");
        Serial.println("  5. Module not damaged");
        return;
    }

    // ── Step 2: init and test ──────────────────────────────────────────────
    Serial.printf("\n✅  Found on SDA=GPIO%d  SCL=GPIO%d\n\n", foundSDA, foundSCL);
    Serial.println("Add these defines to your config.h:");
    Serial.printf("  #define HAPTIC_SDA %d\n", foundSDA);
    Serial.printf("  #define HAPTIC_SCL %d\n\n", foundSCL);

    Serial.println("Initialising DRV2605L...");
    initDRV2605L();
    Serial.println("Done.\n");

    // ── Step 3: fire effects ───────────────────────────────────────────────
    Serial.println("Firing effects — you should feel vibration:\n");

    uint8_t effects[] = {1, 2, 10, 16, 47};
    const char* names[] = {
        "Sharp Click",
        "Double Click",
        "Soft Bump",
        "Strong Buzz",
        "Buzz 100%"
    };

    for (int i = 0; i < 5; i++) {
        Serial.printf("  Effect #%-3d (%s)... ", effects[i], names[i]);
        buzz(effects[i]);
        delay(800);
        Serial.println("done");
    }

    Serial.println("\n--- Continuous test loop ---");
    Serial.println("Send '1'-'5' in Serial Monitor to trigger effects.");
    Serial.println("Effect #1=click, #10=bump, #16=strong, #47=buzz\n");
}

void loop() {
    if (!Serial.available()) return;
    char c = Serial.read();

    if (foundSDA == -1) return; // no device

    switch (c) {
        case '1': Serial.println("Sharp Click");  buzz(1);  break;
        case '2': Serial.println("Double Click"); buzz(2);  break;
        case '3': Serial.println("Soft Bump");    buzz(10); break;
        case '4': Serial.println("Strong Buzz");  buzz(16); break;
        case '5': Serial.println("Buzz 100%");    buzz(47); break;
        default: break;
    }
}
