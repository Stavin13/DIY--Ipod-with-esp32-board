#include "input_manager.h"

InputManager* InputManager::instance = nullptr;

InputManager::InputManager() : 
    lastEncoderCLK(HIGH),
    lastEncoderDT(HIGH),
    encoderPressed(false),
    encoderPressTime(0),
    longPressTriggered(false),
    touchActive(false),
    queueHead(0),
    queueTail(0) {
    
    instance = this;
}

void InputManager::init() {
    // Initialize encoder pins
    pinMode(ENCODER_CLK, INPUT_PULLUP);
    pinMode(ENCODER_DT, INPUT_PULLUP);
    pinMode(ENCODER_SW, INPUT_PULLUP);
    
    // Read initial encoder state
    lastEncoderCLK = digitalRead(ENCODER_CLK);
    lastEncoderDT = digitalRead(ENCODER_DT);
    
    // Initialize touch (test if available)
    initTouch();
    
    // Setup LVGL input device
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    
    #if TOUCH_ENABLED
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = touchpad_read;
    #else
    indev_drv.type = LV_INDEV_TYPE_ENCODER;
    indev_drv.read_cb = encoderRead;
    #endif
    
    lv_indev_drv_register(&indev_drv);
    
    Serial.println("Input manager initialized");
}

void InputManager::update() {
    updateEncoder();
    
    #if TOUCH_ENABLED
    updateTouch();
    #endif
}

void InputManager::updateEncoder() {
    // Read current encoder state
    int currentCLK = digitalRead(ENCODER_CLK);
    int currentDT = digitalRead(ENCODER_DT);
    
    // Check for rotation
    if (currentCLK != lastEncoderCLK) {
        if (currentCLK == LOW) {
            // Encoder rotated
            if (currentDT == HIGH) {
                addEvent(InputEvent::ENCODER_CW);
            } else {
                addEvent(InputEvent::ENCODER_CCW);
            }
        }
    }
    
    lastEncoderCLK = currentCLK;
    lastEncoderDT = currentDT;
    
    // Check button press
    bool currentPressed = (digitalRead(ENCODER_SW) == LOW);
    
    if (currentPressed && !encoderPressed) {
        // Button just pressed
        encoderPressed = true;
        encoderPressTime = millis();
        longPressTriggered = false;
    } else if (!currentPressed && encoderPressed) {
        // Button just released
        encoderPressed = false;
        
        if (!longPressTriggered) {
            addEvent(InputEvent::ENCODER_CLICK);
        }
    } else if (encoderPressed && !longPressTriggered) {
        // Check for long press
        if (millis() - encoderPressTime > 1000) { // 1 second long press
            longPressTriggered = true;
            addEvent(InputEvent::ENCODER_LONG_PRESS);
        }
    }
}

void InputManager::updateTouch() {
    #if TOUCH_ENABLED
    TouchPoint touch;
    bool touched = readTouch(touch);
    
    if (touched && !touchActive) {
        // Touch started
        touchActive = true;
        touchStartTime = millis();
        lastTouch = touch;
        currentTouch = touch;
        Serial.printf("Touch detected: X=%d, Y=%d\n", touch.x, touch.y);
    } else if (!touched && touchActive) {
        // Touch ended
        touchActive = false;
        
        // Calculate touch duration and movement
        unsigned long touchDuration = millis() - touchStartTime;
        int deltaX = currentTouch.x - lastTouch.x;
        int deltaY = currentTouch.y - lastTouch.y;
        
        if (touchDuration < 500 && abs(deltaX) < 20 && abs(deltaY) < 20) {
            // Short touch with minimal movement = tap
            addEvent(InputEvent::TOUCH_TAP);
        } else if (abs(deltaX) > abs(deltaY)) {
            // Horizontal swipe
            if (deltaX > 30) {
                addEvent(InputEvent::TOUCH_SWIPE_RIGHT);
            } else if (deltaX < -30) {
                addEvent(InputEvent::TOUCH_SWIPE_LEFT);
            }
        } else {
            // Vertical swipe
            if (deltaY > 30) {
                addEvent(InputEvent::TOUCH_SWIPE_DOWN);
            } else if (deltaY < -30) {
                addEvent(InputEvent::TOUCH_SWIPE_UP);
            }
        }
    } else if (touched && touchActive) {
        // Touch continuing
        currentTouch = touch;
    }
    #endif
}

bool InputManager::readTouch(TouchPoint& point) {
    #if TOUCH_ENABLED
    // Test touch functionality
    
    // Read X coordinate
    digitalWrite(TOUCH_CS, LOW);
    delayMicroseconds(1);
    
    // Send command to read X (0x90)
    uint8_t cmd = 0x90;
    for (int i = 7; i >= 0; i--) {
        digitalWrite(TOUCH_DIN, (cmd >> i) & 1);
        digitalWrite(TOUCH_CLK, HIGH);
        delayMicroseconds(1);
        digitalWrite(TOUCH_CLK, LOW);
        delayMicroseconds(1);
    }
    
    // Read 12-bit result
    uint16_t x = 0;
    for (int i = 11; i >= 0; i--) {
        digitalWrite(TOUCH_CLK, HIGH);
        delayMicroseconds(1);
        x |= (digitalRead(TOUCH_DO) << i);
        digitalWrite(TOUCH_CLK, LOW);
        delayMicroseconds(1);
    }
    
    // Read Y coordinate
    cmd = 0xD0;
    for (int i = 7; i >= 0; i--) {
        digitalWrite(TOUCH_DIN, (cmd >> i) & 1);
        digitalWrite(TOUCH_CLK, HIGH);
        delayMicroseconds(1);
        digitalWrite(TOUCH_CLK, LOW);
        delayMicroseconds(1);
    }
    
    uint16_t y = 0;
    for (int i = 11; i >= 0; i--) {
        digitalWrite(TOUCH_CLK, HIGH);
        delayMicroseconds(1);
        y |= (digitalRead(TOUCH_DO) << i);
        digitalWrite(TOUCH_CLK, LOW);
        delayMicroseconds(1);
    }
    
    digitalWrite(TOUCH_CS, HIGH);
    
    // Debug output
    static unsigned long lastDebug = 0;
    if (millis() - lastDebug > 1000) {
        Serial.printf("Touch raw: X=%d, Y=%d\n", x, y);
        lastDebug = millis();
    }
    
    // Convert raw values to screen coordinates
    // Adjust these calibration values based on testing
    if (x > 200 && x < 3900 && y > 200 && y < 3900) {
        point.x = map(x, 200, 3900, 0, TFT_WIDTH);
        point.y = map(y, 200, 3900, 0, TFT_HEIGHT);
        point.pressed = true;
        return true;
    }
    
    point.pressed = false;
    return false;
    
    #else
    // Touch disabled
    point.x = 0;
    point.y = 0;
    point.pressed = false;
    return false;
    #endif
}

void InputManager::initTouch() {
    #if TOUCH_ENABLED
    // Initialize SPI pins for resistive touch testing
    pinMode(TOUCH_CS, OUTPUT);
    pinMode(TOUCH_CLK, OUTPUT);
    pinMode(TOUCH_DIN, OUTPUT);
    pinMode(TOUCH_DO, INPUT);
    
    // Set initial states
    digitalWrite(TOUCH_CS, HIGH);
    digitalWrite(TOUCH_CLK, LOW);
    digitalWrite(TOUCH_DIN, LOW);
    
    // Configure interrupt pin if used
    if (TOUCH_IRQ >= 0) {
        pinMode(TOUCH_IRQ, INPUT_PULLUP);
    }
    
    Serial.println("Touch controller test mode enabled");
    Serial.println("Touch the screen to test if controller is functional");
    #else
    Serial.println("Touch disabled - rotary encoder navigation only");
    Serial.println("To test touch: set TOUCH_ENABLED to true in config.h");
    #endif
}

void InputManager::addEvent(InputEvent event) {
    int nextTail = (queueTail + 1) % INPUT_QUEUE_SIZE;
    if (nextTail != queueHead) {
        inputQueue[queueTail] = event;
        queueTail = nextTail;
    }
}

InputEvent InputManager::getNextEvent() {
    if (queueHead == queueTail) {
        return InputEvent::NONE;
    }
    
    InputEvent event = inputQueue[queueHead];
    queueHead = (queueHead + 1) % INPUT_QUEUE_SIZE;
    return event;
}

bool InputManager::hasEvents() {
    return queueHead != queueTail;
}

void InputManager::touchpad_read(lv_indev_drv_t* indev_driver, lv_indev_data_t* data) {
    if (instance) {
        TouchPoint touch = instance->getCurrentTouch();
        data->point.x = touch.x;
        data->point.y = touch.y;
        data->state = touch.pressed ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
}

void InputManager::encoderRead(lv_indev_drv_t* indev_driver, lv_indev_data_t* data) {
    // LVGL encoder input handler
    if (instance && instance->hasEvents()) {
        InputEvent event = instance->getNextEvent();
        
        switch (event) {
            case InputEvent::ENCODER_CW:
                data->enc_diff = 1;
                break;
            case InputEvent::ENCODER_CCW:
                data->enc_diff = -1;
                break;
            case InputEvent::ENCODER_CLICK:
                data->state = LV_INDEV_STATE_PR;
                break;
            default:
                data->enc_diff = 0;
                data->state = LV_INDEV_STATE_REL;
                break;
        }
    } else {
        data->enc_diff = 0;
        data->state = LV_INDEV_STATE_REL;
    }
}

TouchPoint InputManager::getCurrentTouch() {
    return currentTouch;
}