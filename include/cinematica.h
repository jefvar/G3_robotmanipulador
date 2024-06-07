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

//struct para trabajar con tres coordenadas
struct tres_posiciones {float pos_1; float pos_2; float pos_3;};
//struct 2 strings
struct string_two {std::string string_1; std::string string_2;};
//struct 2 int
struct int_two {int int_1; int int_2;};

std::vector<float> conversion_angulos(std::vector<float> vector_angulos_programa);
std::vector<float> trayectoria_lineal(float x_inicial, float y_inicial, float z_inicial, float x_final, float y_final, float z_final);
struct tres_posiciones f_cinematica_directa(float theta_0, float theta_1, float theta_2);
struct tres_posiciones f_cinematica_inversa(float x, float y, float z);
struct string_two f_split_pos(std::string movimiento);
std::vector <float> f_posiciones_inicio_fin(int inicio, int fin);

#endif