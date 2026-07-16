// Touch Test Code - Upload this first to test if touch works

#include <Arduino.h>

// Touch pin definitions
#define TOUCH_CS 6
#define TOUCH_CLK 7
#define TOUCH_DIN 5
#define TOUCH_DO 4
#define TOUCH_IRQ 3

void setup() {
    Serial.begin(115200);
    Serial.println("Touch Test Starting...");
    
    // Initialize touch pins
    pinMode(TOUCH_CS, OUTPUT);
    pinMode(TOUCH_CLK, OUTPUT);
    pinMode(TOUCH_DIN, OUTPUT);
    pinMode(TOUCH_DO, INPUT);
    pinMode(TOUCH_IRQ, INPUT_PULLUP);
    
    // Set initial states
    digitalWrite(TOUCH_CS, HIGH);
    digitalWrite(TOUCH_CLK, LOW);
    digitalWrite(TOUCH_DIN, LOW);
    
    Serial.println("Touch pins initialized");
    Serial.println("Touch the screen to test...");
}

void loop() {
    // Test touch every 100ms
    static unsigned long lastTest = 0;
    if (millis() - lastTest > 100) {
        testTouch();
        lastTest = millis();
    }
}

void testTouch() {
    // Check IRQ pin first (if connected)
    bool irqPressed = (digitalRead(TOUCH_IRQ) == LOW);
    
    // Read X coordinate
    uint16_t x = readTouchAxis(0x90); // X command
    
    // Read Y coordinate  
    uint16_t y = readTouchAxis(0xD0); // Y command
    
    // Print results
    Serial.printf("IRQ: %s, X: %4d, Y: %4d", 
                  irqPressed ? "PRESSED" : "RELEASED", x, y);
    
    // Check if values look reasonable for touch
    if (x > 100 && x < 4000 && y > 100 && y < 4000) {
        Serial.printf(" -> TOUCH DETECTED! Screen coords: X=%d, Y=%d", 
                      map(x, 200, 3800, 0, 240), 
                      map(y, 200, 3800, 0, 320));
    }
    
    Serial.println();
}

uint16_t readTouchAxis(uint8_t command) {
    digitalWrite(TOUCH_CS, LOW);
    delayMicroseconds(1);
    
    // Send command byte
    for (int i = 7; i >= 0; i--) {
        digitalWrite(TOUCH_DIN, (command >> i) & 1);
        digitalWrite(TOUCH_CLK, HIGH);
        delayMicroseconds(1);
        digitalWrite(TOUCH_CLK, LOW);
        delayMicroseconds(1);
    }
    
    // Read 12-bit result
    uint16_t result = 0;
    for (int i = 11; i >= 0; i--) {
        digitalWrite(TOUCH_CLK, HIGH);
        delayMicroseconds(1);
        if (digitalRead(TOUCH_DO)) {
            result |= (1 << i);
        }
        digitalWrite(TOUCH_CLK, LOW);
        delayMicroseconds(1);
    }
    
    digitalWrite(TOUCH_CS, HIGH);
    return result;
}