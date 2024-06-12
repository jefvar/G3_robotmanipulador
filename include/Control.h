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
#define KP_M_BASE 0.07 //calibrado con 0.07
#define KP_M_BRAZO 0.035
#define KP_M_ANTEBRAZO_SUBIDA 0.3
#define KP_M_ANTEBRAZO_BAJADA 0.3
#define KD_M_BASE 0.005 //calibrado con 0.005
#define KD_M_BRAZO 0.008
#define KD_M_ANTEBRAZO_SUBIDA 0.001 //0.001
#define KD_M_ANTEBRAZO_BAJADA 0.001 //0.001
#define TS 0.02 //tiempo de muestreo
#define kI_ANTEBRAZO_SUBIDA 0.02 //0.02//0.01
#define kI_ANTEBRAZO_BAJADA 0.02 //0.02//0.01
#define kI_BASE 0.03 //calibrado con 0.03
#define kI_BRAZO 0.02 

#define DUTY_BASE 1.0
#define DUTY_BRAZO 1.0
#define DUTY_ANTE 0.8


#define PIN_M1_EN 3         //EN_A
#define PIN_M1_IN1 40       //IN1_A
#define PIN_M1_IN2 39       //IN2_A
#define REDUCCION_BASE 2.0

#define PIN_M2_EN 21         //EN_N primeramente teniamos teniamos 48 y daba pwm de 1,7 por el mismo punto de abajo 1.8v, 21 era el in
#define PIN_M2_IN1 6//43//17//48       //IN1_B 17 y 18 corresponden a los pines de tx rx previstos inicialmente, 43 y 44 si funcionan pero se interrumpen con el la impresion del serial uart0
#define PIN_M2_IN2 7//44//18//47       //IN2_B 48 y 47 dieron problemas al arrojar 1.8v motor no se mueve

#define PIN_M3_EN 41         //EN_C
#define PIN_M3_IN1 5       //IN1_C el 2 da problemas en un sentido, el pin 5 funciona bien perro era del cs camara
#define PIN_M3_IN2 42       //IN2_C
// Frecuencia de la PWM
#define PWM_FREQ_HZ    40000

float lectura_encoder_base;
float lectura_encoder_brazo;
float lectura_encoder_antebrazo;

float _ek_pos_base[2]; //error de posicion estado actual y anterior
float _uk_pos_base[2]; //accion de control actual y anterior
float _ek_pos_brazo[2]; 
float _uk_pos_brazo[2]; 
float _ek_pos_antebrazo[2];                 
float _uk_pos_antebrazo[2];
float _integral_motores[3]={0.0,0.0,0.0};
float u_integral_antebrazo=0;
float u_integral_base=0;
float u_integral_brazo=0;

void InitTabla(float t[], int n);
void DesplazarTabla(float t[], int n);
float RelLineal(float x,float x1,float x2,float y1,float y2);
void recvWithEndMarker();
void parseData();


float LecturaEncoder(int n_motor);
float integral(float error, float *integral_sum, float K_i, float Ts);
//void ControlPID_POS(float error[2],float uk[2],float kp,float kd,float integral,int n_motor);//opcion separa de la integral
void ControlPID_POS(float error[2],float kp,float kd,float u_integral,float Ts,int n_motor,float saturacion);
void ControlPD_POS(float error[2],float kp,float kd, float u_integral, float Ts,int n_motor,float saturacion);
void ControlPID_POS_ANTEBRAZO(float error[2],float kp_up,float kp_down,float kd_up,float kd_down,float u_integral,float Ts,float saturacion);

void ControlPID_POS_HOME(float errorbase[2],float errorbrazo[2], float errorante[2],float u_integral_base, float u_integral_brazo,float u_integral_ante);

#endif //