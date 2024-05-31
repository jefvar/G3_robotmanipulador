#ifndef SERVO_H
#define SERVO_H

#include <ESP32Servo.h>

#define PIN_SERVO 14
#define SERVO_FREQ 50
#define CIERRE_SERVO 80
#define APERTURA_SERVO 180

float minUs = 500;          // 0.5 ms
float maxUs = 2500;         // 2.5ms

Servo Pinza;

int posServo = 0;

void config_servo();

#endif