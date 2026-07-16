#ifndef POWER_MANAGER_H
#define POWER_MANAGER_H

#include <Arduino.h>
#include "../config.h"

enum class PowerState {
    ACTIVE,
    IDLE,
    SLEEP,
    DEEP_SLEEP
};

enum class ChargeState {
    NOT_CHARGING,
    CHARGING,
    CHARGED,
    ERROR
};

class PowerManager {
private:
    PowerState currentState;
    ChargeState chargeState;
    uint8_t batteryPercentage;
    float batteryVoltage;
    
    unsigned long lastActivity;
    unsigned long idleTimeout;
    unsigned long sleepTimeout;
    
    bool usbConnected;
    bool chargingEnabled;
    
    void readBatteryVoltage();
    void updateChargeState();
    void enterSleep();
    void exitSleep();
    
public:
    PowerManager();
    void init();
    void update();
    
    // Power state management
    PowerState getPowerState() const { return currentState; }
    void setPowerState(PowerState state);
    void resetActivityTimer();
    
    // Battery monitoring
    uint8_t getBatteryPercentage() const { return batteryPercentage; }
    float getBatteryVoltage() const { return batteryVoltage; }
    ChargeState getChargeState() const { return chargeState; }
    
    // USB and charging
    bool isUSBConnected() const { return usbConnected; }
    bool isCharging() const { return chargeState == ChargeState::CHARGING; }
    void enableCharging(bool enable);
    
    // Power saving
    void setIdleTimeout(unsigned long ms);
    void setSleepTimeout(unsigned long ms);
    void enableWakeOnTouch(bool enable);
    void enableWakeOnEncoder(bool enable);
    
    // System control
    void shutdown();
    void restart();
    void enablePeripheral(uint8_t pin, bool enable);
};

#endif // POWER_MANAGER_H