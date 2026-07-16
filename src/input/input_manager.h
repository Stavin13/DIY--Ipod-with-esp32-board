#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <Arduino.h>
#include <lvgl.h>
#include <XPT2046_Touchscreen.h>
#include <SPI.h>
#include "../config.h"

enum class InputEvent {
    NONE,
    TOUCH_TAP,
    TOUCH_LONG_PRESS,
    TOUCH_SWIPE_UP,
    TOUCH_SWIPE_DOWN,
    TOUCH_SWIPE_LEFT,
    TOUCH_SWIPE_RIGHT,
    TOUCH_PINCH_IN,
    TOUCH_PINCH_OUT
};

struct TouchPoint {
    int16_t x;
    int16_t y;
    bool pressed;
    unsigned long timestamp;
};

class InputManager {
private:
    // Touch controller
    XPT2046_Touchscreen touch;
    
    // Touch state
    TouchPoint lastTouch;
    TouchPoint currentTouch;
    TouchPoint gestureStart;
    unsigned long touchStartTime;
    bool touchActive;
    bool longPressTriggered;
    
    // Input queue
    static const int INPUT_QUEUE_SIZE = 10;
    InputEvent inputQueue[INPUT_QUEUE_SIZE];
    int queueHead;
    int queueTail;
    
    // Touch processing
    bool readTouch(TouchPoint& point);
    void updateTouch();
    InputEvent detectGesture();
    TouchPoint mapToScreen(TouchPoint raw);
    
    // Queue management
    void addEvent(InputEvent event);
    
    // LVGL touch driver
    static void touchRead(lv_indev_drv_t* indev_driver, lv_indev_data_t* data);
    static InputManager* instance;
    
    // Gesture thresholds
    static const int SWIPE_THRESHOLD = 50;
    static const unsigned long LONG_PRESS_TIME = 800;
    static const unsigned long TAP_MAX_TIME = 300;
    
public:
    InputManager();
    void init();
    void update();
    
    // Event handling
    InputEvent getNextEvent();
    bool hasEvents();
    void clearEvents();
    
    // Direct state queries
    TouchPoint getCurrentTouch();
    bool isTouched();
    
    // Configuration
    void setLongPressTime(unsigned long ms);
    void calibrateTouch();
};

#endif // INPUT_MANAGER_H