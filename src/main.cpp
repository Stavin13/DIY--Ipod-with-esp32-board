#include <Arduino.h>
#include "config.h"
#include "display/display_manager.h"
#include "audio/audio_manager.h"
#include "storage/sd_manager.h"
#include "input/input_manager.h"
#include "ui/ui_manager.h"
#include "power/power_manager.h"

// Global managers
DisplayManager display;
AudioManager audio;
SDManager storage;
InputManager input;
UIManager ui;
PowerManager power;

void setup() {
    Serial.begin(115200);
    Serial.println("ESP32-S3 iPod Music Player Starting...");
    
    // Initialize power management first
    power.init();
    
    // Initialize display
    if (!display.init()) {
        Serial.println("Display initialization failed!");
        return;
    }
    
    // Initialize SD card
    if (!storage.init()) {
        Serial.println("SD card initialization failed!");
        // Continue without SD for now
    }
    
    // Initialize audio system
    if (!audio.init()) {
        Serial.println("Audio initialization failed!");
        // Continue without audio for now
    }
    
    // Initialize input systems
    input.init();
    
    // Initialize UI
    ui.init(&display, &audio, &storage, &input);
    
    Serial.println("System initialized successfully!");
}

void loop() {
    // Update all managers
    ui.update();
    input.update();
    audio.update();
    power.update();
    
    // Small delay to prevent watchdog issues
    delay(5);
}