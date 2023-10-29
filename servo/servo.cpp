// // Control a servo by degrees or millis

// #include "pico/stdlib.h"
// #include <stdio.h>
// #include "hardware/pwm.h"
// #include "hardware/clocks.h"

// float clockDiv = 64;
// float wrap = 39062;
// float frequency = 50;


// // duty cycle einstellen in millisekunden an dem jeweiligen GPIO Pin.
// void setDuty(int servoPin, float millis) {
//     pwm_set_gpio_level(servoPin, (millis/20000.f)*wrap);
// }

// // Servo initialisieren an dem jeweiligen GPIO Pin mit einer start duty cycle in millisekunden.
// void setServo(int servoPin, float startMillis) {
//     gpio_set_function(servoPin, GPIO_FUNC_PWM);
//     uint slice_num = pwm_gpio_to_slice_num(servoPin);

//     pwm_config config = pwm_get_default_config();
    
//     uint64_t clockspeed = clock_get_hz(5);
//     clockDiv = 64;
//     wrap = 39062;

//     while (clockspeed/clockDiv/frequency > 65535 && clockDiv < 256) {
//         clockDiv += 64; 
//     }
//     // Interrupt alle s^-1 = Hz  => wrap also interrupt alle 50 schritte -> 50 Hz (Takt (CPU/clkVT)/Schritte(Hz)   
//     wrap = clockspeed/clockDiv/frequency;
//     // Vorteiler clkVT setzen 
//     pwm_config_set_clkdiv(&config, clockDiv);
//     // Interrupt float wert sezten in ISR Register
//     pwm_config_set_wrap(&config, wrap);
//     // div ist der richtige vorteiler VT
// 	pwm_init(slice_num, &config, true);

// 	setDuty(servoPin, startMillis);
// }

// // Frequenz des PWM-Signals ändern
// void setFrequency(int servoPin, float newfrequency) {
// 	uint slice_num = pwm_gpio_to_slice_num(servoPin);
// 	pwm_config config = pwm_get_default_config();

// 	uint64_t clockspeed = clock_get_hz(5);
// 	clockDiv = 64;
// 	wrap = 39062;

// 	while (clockspeed/clockDiv/newfrequency > 65535 && clockDiv < 256) {
// 		clockDiv += 64;
// 	}

// 	wrap = clockspeed/clockDiv/newfrequency;

// 	pwm_config_set_clkdiv(&config, clockDiv);
// 	pwm_config_set_wrap(&config, wrap);
// 	pwm_init(slice_num, &config, true);

// 	frequency = newfrequency;
// }
#include "servo.h"
#include <cstdio>

Servo::Servo(uint8_t servoPin, float startMillis) : servoPin(servoPin), frequency(50), clockDiv(64), wrap(39062) {
    gpio_set_function(servoPin, GPIO_FUNC_PWM);
    sliceNum = pwm_gpio_to_slice_num(servoPin);

    pwm_config config = pwm_get_default_config();
    
    uint64_t clockspeed = clock_get_hz(clk_sys);
    clockDiv = 64;
    wrap = 39062;

    while (clockspeed/clockDiv/frequency > 65535 && clockDiv < 256) {
        clockDiv += 64; 
    }
    
    wrap = clockspeed/clockDiv/frequency;
    pwm_config_set_clkdiv(&config, clockDiv);
    pwm_config_set_wrap(&config, wrap);
    pwm_init(sliceNum, &config, true);

    setDuty(startMillis);
}

void Servo::setDuty(float millis) {
    pwm_set_gpio_level(servoPin, (millis / 20000.f) * wrap);
}

void Servo::setFrequency(float newFrequency) {
    pwm_config config = pwm_get_default_config();

    uint64_t clockspeed = clock_get_hz(clk_sys);
    clockDiv = 64;
    wrap = 39062;

    while (clockspeed/clockDiv/newFrequency > 65535 && clockDiv < 256) {
        clockDiv += 64;
    }

    wrap = clockspeed/clockDiv/newFrequency;

    pwm_config_set_clkdiv(&config, clockDiv);
    pwm_config_set_wrap(&config, wrap);
    pwm_init(sliceNum, &config, true);

    frequency = newFrequency;
}
