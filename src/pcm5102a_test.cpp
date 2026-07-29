/*
 * PCM5102A DAC Test
 * 
 * Sends a 440Hz sine wave via I2S to verify the PCM5102A is wired correctly.
 * You should hear a continuous tone in your headphones/speakers.
 * 
 * PCM5102A Wiring:
 *   BCK  → GPIO 14
 *   LCK  → GPIO 15
 *   DIN  → GPIO 16
 *   SCK  → GND  (IMPORTANT: enables internal clock)
 *   VIN  → 3.3V
 *   GND  → GND
 *   FMT  → GND
 *   XSMT → 3.3V (un-mute)
 */

#include <Arduino.h>
#include <driver/i2s.h>
#include <math.h>

// I2S pins - change these if you used different GPIOs
#define I2S_BCLK  14   // Bit Clock  → PCM5102A BCK
#define I2S_LRC   15   // Word Select → PCM5102A LCK
#define I2S_DOUT  16   // Data Out   → PCM5102A DIN

// Tone settings
#define SAMPLE_RATE   44100
#define TONE_HZ       440      // Concert A - easy to recognise
#define AMPLITUDE     10000    // 0–32767; lower = quieter, safer first test

// Pre-built sine table (one full cycle, 256 samples)
static int16_t sineTable[256];

void buildSineTable() {
    for (int i = 0; i < 256; i++) {
        sineTable[i] = (int16_t)(AMPLITUDE * sinf(2.0f * M_PI * i / 256.0f));
    }
}

void initI2S() {
    i2s_config_t cfg = {
        .mode                 = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
        .sample_rate          = SAMPLE_RATE,
        .bits_per_sample      = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format       = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags     = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count        = 8,
        .dma_buf_len          = 64,
        .use_apll             = false,
        .tx_desc_auto_clear   = true,
    };

    i2s_pin_config_t pins = {
        .bck_io_num   = I2S_BCLK,
        .ws_io_num    = I2S_LRC,
        .data_out_num = I2S_DOUT,
        .data_in_num  = I2S_PIN_NO_CHANGE,
    };

    i2s_driver_install(I2S_NUM_0, &cfg, 0, NULL);
    i2s_set_pin(I2S_NUM_0, &pins);
    i2s_zero_dma_buffer(I2S_NUM_0);

    Serial.println("I2S initialized");
}

void setup() {
    Serial.begin(115200);
    delay(500);
    Serial.println("\n=== PCM5102A DAC Test ===");
    Serial.println("You should hear a 440Hz tone.");
    Serial.println("If silent, check:");
    Serial.println("  1. SCK pin on PCM5102A tied to GND");
    Serial.println("  2. XSMT pin tied to 3.3V (not GND)");
    Serial.println("  3. BCK/LCK/DIN wiring matches pins 14/15/16");

    buildSineTable();
    initI2S();
}

void loop() {
    // Fill a buffer with one cycle of the sine wave, scaled to tone frequency
    static uint32_t phase = 0;
    // phase increments per sample = 256 * TONE_HZ / SAMPLE_RATE
    const uint32_t phaseInc = (uint32_t)((256.0f * TONE_HZ / SAMPLE_RATE) * 65536);

    // Write 256 stereo samples per call (512 int16 values)
    int16_t buf[512];
    for (int i = 0; i < 256; i++) {
        int16_t sample = sineTable[(phase >> 16) & 0xFF];
        phase += phaseInc;
        buf[i * 2]     = sample; // Left
        buf[i * 2 + 1] = sample; // Right
    }

    size_t written = 0;
    i2s_write(I2S_NUM_0, buf, sizeof(buf), &written, portMAX_DELAY);
}
