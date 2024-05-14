#ifndef CONTROL_H
#define CONTROL_H

#include <Arduino.h>
#include <SPI.h>
#include "../lib/ALMar_ESP32_EncoderATM203_Spi2.cpp"
#include "../lib/AlMar_ESP32_Driver_L298n.cpp"

// Objetos de Clases
using namespace AlMar;//Si no se pone esto tendria que poner AlMAr::Esp32::.... Cada vez 
using namespace Esp32;

EncoderATM203_Spi2* _encoder1;  // Puntero para usar asignación dinámica en la creación
Driver_L298n *_motors[3]; //Creacion del puntero al objeto // Se puede usar un array en su lugar: *_motors[3];  

/*************************** CONSTANTES ***************************/
#define PIN_RGB 38
#define N_MOTORS 3
#define PIN_CS_BASE  10 // (chip select para encoder de motor 1) 10 for SPI3
#define PIN_CS_BRAZO  9  // (chip select para encoder de motor 2) 
#define PIN_CS_ANTEBRAZO  8  // (chip select para encoder de motor 3) 
// Pines comunes a todos los encóders del bus 
#define PIN_SCLK  12 // 36 // 12 for SPI3 //naranja con negro
#define PIN_MISO  13 // 37 // 13 for SPI3 //marron con negro
#define PIN_MOSI  11 // 35 // 11 for SPI3 //rojo con blanco
//Pines para el pwm de los motores 
#define MOTOR_BASE 0
#define MOTOR_BRAZO 1
#define MOTOR_ANTEBRAZO 2
#define KP_M_BASE 0.01
#define KP_M_ANTEBRAZO 0.001
#define Kd 0.001
#define TS 0.05 //tiempo de muestreo
#define ki 0.01

#define PIN_M1_EN 3         //EN_A
#define PIN_M1_IN1 40       //IN1_A
#define PIN_M1_IN2 39       //IN2_A

#define PIN_M2_EN 21         //EN_N primeramente teniamos teniamos 48 y daba pwm de 1,7 por el mismo punto de abajo 1.8v, 21 era el in
#define PIN_M2_IN1 17//48       //IN1_B 17 y 18 corresponden a los pines de tx rx previstos inicialmente
#define PIN_M2_IN2 18//47       //IN2_B 48 y 47 dieron problemas al arrojar 1.8v motor no se mueve

#define PIN_M3_EN 41         //EN_C
#define PIN_M3_IN1 5       //IN1_C el 2 da problemas en un sentido, el pin 5 funciona bien perro era del cs camara
#define PIN_M3_IN2 42       //IN2_C
// Frecuencia de la PWM
#define PWM_FREQ_HZ    40000


void InitTabla(float t[], int n);
void DesplazarTabla(float t[], int n);
float LecturaEncoder(int n_motor);
float integral(float error, float *integral_sum, float K_i, float Ts);
//void ControlPID_POS(float error[2],float uk[2],float kp,float kd,float integral,int n_motor);//opcion separa de la integral
void ControlPID_POS(float error[2],float uk[2],float kp,float kd,float *integral_sum,float Ts,int n_motor);




#endif //