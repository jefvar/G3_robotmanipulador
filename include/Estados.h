#ifndef ESTADOS_H
#define ESTADOS_H

#define PIN_RGB 38

enum estadosControl{ESTADO_INICIAL, CALIBRACION, MANUAL, AUTOMATICO};   // Modos de control del algoritmo de juego
enum estadosControl _estado_control = ESTADO_INICIAL;

extern int numero_turnos;
extern String strLectura;

int contador_consola=0;
int contador_trayectoria = 0;
int contador_pasos = 0;

int contador_buzzer = 0;

int numero_turnos = 0;
float duty_calibracion = 0.8;

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(1, PIN_RGB, NEO_GRB + NEO_KHZ800);

void modo_manual();
void modo_calibracion();
void modo_automatico();
void modo_inicial();
void modos_interrupcion();

#endif