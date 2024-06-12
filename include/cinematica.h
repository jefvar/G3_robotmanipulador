#ifndef CINEMATICA_H
#define CINEMATICA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <list>
#include <Arduino.h>

//Valor del paso en la ejecución de trayectoria 
#define paso 30
//Longitudes de los eslabones
#define L1 270
#define L2 250
#define L3 100
//Definición de los valores iniciales de los encoders
#define def_theta_0 90
#define def_theta_1 0
#define def_theta_2 0

#define separador '.'
//struct para trabajar con tres coordenadas
struct tres_posiciones {float pos_1; float pos_2; float pos_3;};
//struct 2 strings
struct string_two {String string_1; String string_2;};
//struct 2 int
struct int_two {int int_1; int int_2;};

std::vector<float> conversion_angulos(std::vector<float> vector_angulos_programa);
std::vector<float> f_trayectoria_lineal(float p_ini_x, float p_ini_y, float p_ini_z, float p_fin_x, float p_fin_y, float p_fin_z);
struct tres_posiciones f_cinematica_directa(float theta_0, float theta_1, float theta_2);
struct tres_posiciones f_cinematica_inversa(float x, float y, float z);
struct string_two f_split_pos(String movimiento);
std::vector <float> f_posiciones_2puntos(int inicio, int fin);
std::vector <float> f_posiciones_9puntos(int inicio, int fin);
std::vector<float> conversion_angulos_encoder(float theta0_enc, float theta1_enc, float theta2_enc);

#endif