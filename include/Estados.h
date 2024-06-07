#ifndef ESTADOS_H
#define ESTADOS_H

enum estadosControl{ESTADO_INICIAL, CALIBRACION, MANUAL, AUTOMATICO};   // Modos de control del algoritmo de juego

extern int numero_turnos;
extern String strLectura;

void modo_manual();
void modo_calibracion();

#endif