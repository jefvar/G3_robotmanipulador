#include "Pinza.h"

//Servo Pinza; 

void config_servo() {
    ESP32PWM::allocateTimer(1);
    Pinza.setPeriodHertz(SERVO_FREQ);
    Pinza.attach(PIN_SERVO, minUs, maxUs);
}   