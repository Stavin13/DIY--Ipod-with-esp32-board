#include "power_manager.h"

PowerManager::PowerManager() :
    currentState(PowerState::ACTIVE),
    chargeState(ChargeState::NOT_CHARGING),
    batteryPercentage(100),
    batteryVoltage(4.2),
    lastActivity(0),
    idleTimeout(30000),  // 30 seconds
    sleepTimeout(300000), // 5 minutes
    usbConnected(false),
    chargingEnabled(true) {
}

void PowerManager::init() {
    // Configure power pins
    pinMode(BATTERY_ADC, INPUT);
    pinMode(CHARGE_STATUS, INPUT_PULLUP);
    pinMode(POWER_ENABLE, OUTPUT);
    
    // Enable main power
    digitalWrite(POWER_ENABLE, HIGH);
    
    // Configure USB detection if available
    pinMode(USB_PD_INT, INPUT_PULLUP);
    
    // Reset activity timer
    resetActivityTimer();
    
    Serial.println("Power manager initialized");
}

void PowerManager::update() {
    // Read battery status
    readBatteryVoltage();
    updateChargeState();
    
    // Check for power state transitions
    unsigned long now = millis();
    unsigned long timeSinceActivity = now - lastActivity;
    
    switch (currentState) {
        case PowerState::ACTIVE:
            if (timeSinceActivity > idleTimeout) {
                setPowerState(PowerState::IDLE);
            }
            break;
            
        case PowerState::IDLE:
            if (timeSinceActivity > sleepTimeout) {
                setPowerState(PowerState::SLEEP);
            }
            break;
            
        case PowerState::SLEEP:
            // Wake up conditions handled by interrupts
            break;
            
        case PowerState::DEEP_SLEEP:
            // Should not reach here in normal operation
            break;
    }
}

void PowerManager::readBatteryVoltage() {
    // Read ADC value
    int adcValue = analogRead(BATTERY_ADC);
    
    // Convert to voltage (assuming voltage divider)
    // Adjust these values based on your voltage divider circuit
    batteryVoltage = (adcValue / 4095.0) * 3.3 * 2.0; // Assuming 2:1 voltage divider
    
    // Convert voltage to percentage (rough approximation for LiPo)
    if (batteryVoltage >= 4.2) {
        batteryPercentage = 100;
    } else if (batteryVoltage >= 4.0) {
        batteryPercentage = 80 + (batteryVoltage - 4.0) * 100;
    } else if (batteryVoltage >= 3.8) {
        batteryPercentage = 60 + (batteryVoltage - 3.8) * 100;
    } else if (batteryVoltage >= 3.6) {
        batteryPercentage = 40 + (batteryVoltage - 3.6) * 100;
    } else if (batteryVoltage >= 3.4) {
        batteryPercentage = 20 + (batteryVoltage - 3.4) * 100;
    } else if (batteryVoltage >= 3.2) {
        batteryPercentage = (batteryVoltage - 3.2) * 100;
    } else {
        batteryPercentage = 0;
    }
}

void PowerManager::updateChargeState() {
    // Check USB connection
    usbConnected = (digitalRead(USB_PD_INT) == LOW);
    
    // Check charging status
    bool chargingPin = (digitalRead(CHARGE_STATUS) == LOW);
    
    if (!usbConnected) {
        chargeState = ChargeState::NOT_CHARGING;
    } else if (chargingPin && chargingEnabled) {
        chargeState = ChargeState::CHARGING;
    } else if (batteryPercentage >= 95) {
        chargeState = ChargeState::CHARGED;
    } else {
        chargeState = ChargeState::NOT_CHARGING;
    }
}

void PowerManager::setPowerState(PowerState state) {
    if (state == currentState) return;
    
    PowerState previousState = currentState;
    currentState = state;
    
    switch (state) {
        case PowerState::ACTIVE:
            if (previousState == PowerState::SLEEP) {
                exitSleep();
            }
            Serial.println("Power state: ACTIVE");
            break;
            
        case PowerState::IDLE:
            Serial.println("Power state: IDLE");
            // Reduce display brightness, lower CPU frequency
            break;
            
        case PowerState::SLEEP:
            Serial.println("Power state: SLEEP");
            enterSleep();
            break;
            
        case PowerState::DEEP_SLEEP:
            Serial.println("Power state: DEEP_SLEEP");
            // Prepare for deep sleep
            esp_deep_sleep_start();
            break;
    }
}

void PowerManager::enterSleep() {
    // Configure wake sources
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_3, 0); // Encoder button
    esp_sleep_enable_ext1_wakeup(0x20, ESP_EXT1_WAKEUP_ANY_HIGH); // Touch interrupt
    
    // Disable unnecessary peripherals
    // Turn off display backlight
    // Reduce power to audio components
    
    // Enter light sleep
    esp_light_sleep_start();
    
    // When we wake up, return to active state
    currentState = PowerState::ACTIVE;
    resetActivityTimer();
}

void PowerManager::exitSleep() {
    // Re-enable peripherals
    // Turn on display backlight
    // Restore audio components
    
    Serial.println("Waking from sleep");
}

void PowerManager::resetActivityTimer() {
    lastActivity = millis();
    
    // If we were sleeping, wake up
    if (currentState == PowerState::SLEEP) {
        setPowerState(PowerState::ACTIVE);
    }
}

void PowerManager::enableCharging(bool enable) {
    chargingEnabled = enable;
    // Control charging circuit if needed
}

void PowerManager::setIdleTimeout(unsigned long ms) {
    idleTimeout = ms;
}

void PowerManager::setSleepTimeout(unsigned long ms) {
    sleepTimeout = ms;
}

void PowerManager::shutdown() {
    Serial.println("Shutting down...");
    
    // Save any important data
    // Turn off all peripherals
    digitalWrite(POWER_ENABLE, LOW);
    
    // Enter deep sleep
    esp_deep_sleep_start();
}

void PowerManager::restart() {
    Serial.println("Restarting...");
    ESP.restart();
}

void PowerManager::enablePeripheral(uint8_t pin, bool enable) {
    digitalWrite(pin, enable ? HIGH : LOW);
}