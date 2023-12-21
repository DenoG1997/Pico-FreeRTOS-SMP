// #ifndef servo_h
// #define servo_h

// // Control a servo by degrees or millis
// void setDuty(int servoPin, float millis);
// // Servo initialisieren an dem jeweiligen GPIO Pin mit einer start duty cycle in millisekunden.
// void setServo(int servoPin, float startMillis);
// // Setzt die Frequenz des PWM Signals an dem jeweiligen GPIO Pin.
// void setFrequency(int servoPin, float newfrequency);

// #endif

#ifndef SERVO_H
#define SERVO_H

#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include <cstdint>
#include "stdio.h"

class Servo {
public:
    Servo(int servoPin, float startMillis, float frequency);
    void setDuty(float millis);
    void setFrequency(float newFrequency);

private:
    uint8_t mservoPin;
    float mclockDiv;
    float mwrap;
    float mfrequency;
    uint msliceNum;
};

#endif //SERVO_H
