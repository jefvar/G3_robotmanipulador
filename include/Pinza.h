#ifndef PINZA_H
#define PINZA_H

#include <ESP32Servo.h>

#define PIN_SERVO 14
#define SERVO_FREQ 50
#define CIERRE_SERVO 90
#define APERTURA_SERVO 0

float minUs = 500;          // 1 ms
float maxUs = 2500;         // 2ms

Servo Pinza;

int contador_servo = 0;
int angulo_servo = APERTURA_SERVO;

void config_servo();
void abrir_servo();
void cerrar_servo();


#endif