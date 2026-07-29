/*
 * Main application entry point.
 * 
 * NOTE: Currently excluded from build via platformio.ini src_filter
 * while PCM5102A DAC is being tested (pcm5102a_test.cpp is active).
 * 
 * To restore full build:
 *   1. Remove the src_filter line from platformio.ini
 *   2. This file will be compiled again automatically
 */

#include <Arduino.h>
#include "config.h"
#include "audio/audio_manager.h"
#include "storage/sd_manager.h"
#include "input/input_manager.h"
#include "ui/ui_manager.h"
#include "power/power_manager.h"

AudioManager audio;
SDManager storage;
InputManager input;
UIManager ui;
PowerManager power;

void setup() {
    Serial.begin(115200);
    Serial.println("ESP32-S3 iPod Music Player Starting...");

    power.init();
    storage.init();
    audio.init();
    input.init();
    // ui.init(&audio, &storage, &input);

    Serial.println("System initialized!");
}

void loop() {
    ui.update();
    input.update();
    audio.update();
    power.update();
    delay(5);
}
