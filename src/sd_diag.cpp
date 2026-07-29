/*
 * SD Card Diagnostic
 *
 * Tries every common SPI pin combo to find your SD module.
 * Once found, mounts the card and lists files.
 *
 * WIRING (typical SD module):
 *   VCC  → 3.3V  (NOT 5V — will damage the card)
 *   GND  → GND
 *   CS   → any free GPIO (try 35)
 *   MOSI → any free GPIO (try 36)
 *   MISO → any free GPIO (try 37)
 *   SCK  → any free GPIO (try 38)
 */

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

// ── Pin combos to try: {CS, MOSI, MISO, SCK} ────────────────────────────────
// Edit the first entry to match what you actually wired, rest are fallbacks.
const int SPI_COMBOS[][4] = {
    {35, 36, 37, 38},   // config.h defaults
    {5,  23, 19, 18},   // Arduino-style VSPI
    {15, 13, 12, 14},   // HSPI
    {10, 11, 13, 12},   // display SPI (shared, last resort)
    {34, 33, 32, 25},
    {4,  23, 19, 18},
    {2,  23, 19, 18},
};
const int NUM_COMBOS = sizeof(SPI_COMBOS) / sizeof(SPI_COMBOS[0]);

int foundCS   = -1;
int foundMOSI = -1;
int foundMISO = -1;
int foundSCK  = -1;

// ── helpers ──────────────────────────────────────────────────────────────────

void listDir(File dir, int depth = 0) {
    while (true) {
        File entry = dir.openNextFile();
        if (!entry) break;

        for (int i = 0; i < depth; i++) Serial.print("  ");
        Serial.print(entry.isDirectory() ? "[DIR] " : "      ");
        Serial.print(entry.name());

        if (!entry.isDirectory()) {
            Serial.printf("  (%lu bytes)", entry.size());
        }
        Serial.println();

        if (entry.isDirectory() && depth < 2) {
            listDir(entry, depth + 1);
        }
        entry.close();
    }
}

// ── setup ────────────────────────────────────────────────────────────────────

void setup() {
    Serial.begin(115200);

    // ESP32-S3 USB CDC: wait up to 5s for Serial Monitor to connect
    // Open your monitor BEFORE the board finishes booting, or press RST after opening
    unsigned long t = millis();
    while (!Serial && (millis() - t) < 5000);
    delay(500);

    Serial.println("\n==============================");
    Serial.println("     SD Card Diagnostic");
    Serial.println("==============================\n");

    Serial.println("Trying SPI pin combinations...\n");

    for (int i = 0; i < NUM_COMBOS; i++) {
        int cs   = SPI_COMBOS[i][0];
        int mosi = SPI_COMBOS[i][1];
        int miso = SPI_COMBOS[i][2];
        int sck  = SPI_COMBOS[i][3];

        Serial.printf("  CS=%-2d  MOSI=%-2d  MISO=%-2d  SCK=%-2d  ... ", cs, mosi, miso, sck);

        // Re-init SPI bus on these pins
        SPI.end();
        SPI.begin(sck, miso, mosi, cs);

        if (SD.begin(cs, SPI, 4000000)) {  // 4 MHz — conservative for breadboard
            Serial.println("MOUNTED! ✓");
            foundCS   = cs;
            foundMOSI = mosi;
            foundMISO = miso;
            foundSCK  = sck;
            break;
        } else {
            SD.end();
            Serial.println("no card");
        }
    }

    if (foundCS == -1) {
        Serial.println("\n❌  SD card not found on any combo.");
        Serial.println("\nCheck:");
        Serial.println("  1. Card is inserted and formatted FAT32");
        Serial.println("  2. VCC → 3.3V  (most SD modules want 3.3V, NOT 5V)");
        Serial.println("  3. GND → GND");
        Serial.println("  4. MOSI/MISO/SCK not swapped");
        Serial.println("  5. CS pulled HIGH when idle (usually handled by SD.begin)");
        Serial.println("  6. Try a different SD card (some cards are picky)");
        Serial.println("\nEdit the SPI_COMBOS table at the top of this file");
        Serial.println("with your actual wiring and re-upload.");
        return;
    }

    // ── found — print what to put in config.h ─────────────────────────────
    Serial.printf("\n✅  SD card mounted on CS=%d  MOSI=%d  MISO=%d  SCK=%d\n\n",
                  foundCS, foundMOSI, foundMISO, foundSCK);

    Serial.println("Add these to your config.h:");
    Serial.printf("  #define SD_CS   %d\n", foundCS);
    Serial.printf("  #define SD_MOSI %d\n", foundMOSI);
    Serial.printf("  #define SD_MISO %d\n", foundMISO);
    Serial.printf("  #define SD_SCLK %d\n\n", foundSCK);

    // ── card info ─────────────────────────────────────────────────────────
    uint8_t cardType = SD.cardType();
    const char* types[] = {"None", "MMC", "SD", "SDHC/SDXC", "Unknown"};
    int ti = (cardType <= 4) ? cardType : 4;
    Serial.printf("Card type  : %s\n", types[ti]);
    Serial.printf("Card size  : %llu MB\n", SD.cardSize() / (1024ULL * 1024));
    Serial.printf("Total space: %llu MB\n", SD.totalBytes() / (1024ULL * 1024));
    Serial.printf("Used space : %llu MB\n\n", SD.usedBytes() / (1024ULL * 1024));

    // ── list files ────────────────────────────────────────────────────────
    Serial.println("Files on card:");
    Serial.println("--------------");
    File root = SD.open("/");
    listDir(root);
    root.close();

    // ── write/read test ───────────────────────────────────────────────────
    Serial.println("\nWrite/read test...");
    File f = SD.open("/diag_test.txt", FILE_WRITE);
    if (f) {
        f.println("SD diag OK");
        f.close();
        File r = SD.open("/diag_test.txt");
        if (r) {
            Serial.print("  Read back: ");
            while (r.available()) Serial.write(r.read());
            r.close();
            SD.remove("/diag_test.txt");
            Serial.println("  ✅ Write/read OK");
        }
    } else {
        Serial.println("  ❌ Could not write test file (card may be read-only)");
    }

    Serial.println("\nSD card is ready to use!");
}

void loop() {
    // nothing — all done in setup
}
