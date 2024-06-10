#ifndef ESTADOS_H
#define ESTADOS_H

enum estadosControl{ESTADO_INICIAL, CALIBRACION, MANUAL, AUTOMATICO};   // Modos de control del algoritmo de juego

extern int numero_turnos;
extern String strLectura;
bool flag_manual = false;

int numero_turnos = 0;
float duty_calibracion = 0.8;

void modo_manual();
void modo_calibracion();
void modo_automatico();

#endif