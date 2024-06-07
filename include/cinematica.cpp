#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
//#include <windows.h>
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
struct string_two {std:: string string_1; std:: string string_2;};
//struct 2 int
struct int_two {int int_1; int int_2;};

//Función Cinemática Directa: Ángulos -> Posición TCP
struct tres_posiciones f_cinematica_directa(float theta_0, float theta_1, float theta_2){
    struct tres_posiciones punto;
    //Puntos de interés del robot 
    float P1[2], P2[2], P3[2];
    float w;
    //Ángulos a radianes
    float theta_0_rad, theta_1_rad, theta_2_rad;
    theta_0_rad = theta_0 * (3.141592/180);
    theta_1_rad = theta_1 * (3.141592/180);    
    theta_2_rad = theta_2 * (3.141592/180);

    //Punto 1 -> hombro
    P1[0] = L1 * sin(theta_1_rad); 
    P1[1] = L1 * cos(theta_1_rad); 
    //Punto 2 -> codo
    P2[0] = P1[0] + L2 * (sin(theta_1_rad + theta_2_rad));
    P2[1] = P1[1] + L2 * (cos(theta_1_rad + theta_2_rad));
    //Punto 3 -> pinza
    P3[0] = P2[0];
    P3[1] = P2[1] - L3;

    w = P3[0];

    punto.pos_1 = w*cos(theta_0_rad);
    punto.pos_2 = w*sin(theta_0_rad);
    punto.pos_3 = P3[1];
    return punto;
}

//Función Cinemática Inversa: Posición TCP -> Ángulos 
struct tres_posiciones f_cinematica_inversa(float x, float y, float z){
    struct tres_posiciones angulo;    
    //Ángulos a calcular en radianes
    float theta_0_rad, theta_1_rad, theta_2_rad;
    //Puntos de cálculo
    float w2, z2, w1, z1;
    //Variables auxiliares
    float w, raiz, k, a, b, c, z_pos, z_neg;

    //Cálculo posicion theta_b -> plano w
    w = sqrt(pow(x,2) + pow(y,2));
    theta_0_rad = atan2 (y, x);

    //Posición de P3[w3,z3] = [w,z]
    //Posición de P2[w2,z2]
    w2 = w;
    z2 = z + L3;

    //Posición de P1[w1,z1]
    k = -pow(L2,2) + pow(L1,2) + pow(w2,2) + pow(z2,2);
    a = 4*pow(w2,2) + 4*pow(z2,2);
    b = -4*z2*k;
    c = -4*pow(w2,2)*pow(L1,2)+pow(k,2);

    if(pow(b,2) < (4*a*c)){
        printf("trayectoria erronea");
        angulo.pos_1 = 0;
        angulo.pos_2 = 0;
        angulo.pos_3 = 0;
        return angulo;
    }
    
    raiz = sqrt(pow(b,2)-4*a*c);
    z_pos = (-b+raiz)/(2*a);
    z_neg = (-b+raiz)/(2*a);

    if (z_pos > z_neg)
    {
        z1 = z_pos;
    }
    else if(z_neg >= z_pos)
    {
        z1 = z_neg;
    }
    w1 = sqrt(pow(L1,2) - pow(z1,2));

    //Ángulos
    theta_1_rad = acos(z1/L1);
    theta_2_rad = acos((z2-z1)/L2) - theta_1_rad;

    angulo.pos_1 = (180/3.141592) * theta_0_rad;
    angulo.pos_2 = (180/3.141592) * theta_1_rad;
    angulo.pos_3 = (180/3.141592) * theta_2_rad;

    return angulo;
}

//Función Trayectoria Lineal -> Entrada: tres puntos iniciales + tres puntos finales
std:: vector<float> trayectoria_lineal(float x_inicial, float y_inicial, float z_inicial, float x_final, float y_final, float z_final){
    //Vectores puntos objetivos durante la trayectoria
    std:: vector<float> x_objetivo(paso), y_objetivo(paso), z_objetivo(paso);
    //Vector angulos calculados durante la trayectoria -> control
    std:: vector<float> angulos_trayectoria(3*paso);
    
    for (int j = 0; j < paso; ++j){
        x_objetivo[j] = x_inicial + (x_final - x_inicial) * j / (paso - 1);
        y_objetivo[j] = y_inicial + (y_final - y_inicial) * j / (paso - 1);
        z_objetivo[j] = z_inicial + (z_final - z_inicial) * j / (paso - 1);

        //Cinemática inversa -> paso a ángulos
        struct tres_posiciones angulo = f_cinematica_inversa(x_objetivo[j], y_objetivo[j], z_objetivo[j]);
        angulos_trayectoria[3*j] = angulo.pos_1;
        angulos_trayectoria[3*j+1] = angulo.pos_2;
        angulos_trayectoria[3*j+2] = angulo.pos_3;
    }
    return angulos_trayectoria;
}

//Conversión de ángulos
std:: vector<float> conversión_angulos(std:: vector<float> vector_angulos_programa){
std:: vector<float> vector_angulos_robot(3*paso);
    //Valor definido del ángulo de la base
    for (int j = 0; j < paso; ++j){
        vector_angulos_robot[3*j] = def_theta_0 - vector_angulos_programa[3*j];
        vector_angulos_robot[3*j+1] = def_theta_1 - vector_angulos_programa[3*j+1];
        vector_angulos_robot[3*j+2] = def_theta_2 - vector_angulos_programa[3*j+2];
    }
    return vector_angulos_robot;
}

//Función separación del array inicial en pos_fija_inicial y pos_tablero_final
struct string_two f_split_pos(std:: string movimiento){
    struct string_two digitos;
    std::stringstream input_stringstream (movimiento);
    //Extraer
    getline(input_stringstream, digitos.string_1,'.');
    getline(input_stringstream, digitos.string_2,'.');
    
    return digitos;
}

//Función Posiciones 
std:: vector <float> f_posiciones_inicio_fin(int inicio, int fin){
    struct posiciones_tablero {float coord_x; float coord_y; float coord_z;}pos_0, pos_1, pos_2, pos_3, pos_4, 
    pos_5, pos_6, pos_7, pos_8, pos_9, pos_10, pos_11, pos_12, pos_13, pos_14, pos_15, pos_16, pos_17, pos_18;
    //Vector donde se almacenan las 6 coordenadas 3punto inicial 3 punto final
    std:: vector<float> vector_2puntos_trayectoria(6);

    pos_0.coord_x = 350.75;
    pos_0.coord_y = 37;
    pos_0.coord_z = -145;

    pos_1.coord_x = 350.75;
    pos_1.coord_y = 0;
    pos_1.coord_z = -145;

    pos_2.coord_x = 350.75;
    pos_2.coord_y = -37;
    pos_2.coord_z = -145;

    pos_3.coord_x = 390;
    pos_3.coord_y = 37;
    pos_3.coord_z = -145;

    pos_4.coord_x = 390;
    pos_4.coord_y = 0;
    pos_4.coord_z = -145;

    pos_5.coord_x = 390;
    pos_5.coord_y = -37;
    pos_5.coord_z = -145;

    pos_6.coord_x = 429.25;
    pos_6.coord_y = 37;
    pos_6.coord_z = -145;

    pos_7.coord_x = 429.25;
    pos_7.coord_y = 0;
    pos_7.coord_z = -145;

    pos_8.coord_x = 429.25;
    pos_8.coord_y = -37;
    pos_8.coord_z = -145;

    pos_9.coord_x = 320;
    pos_9.coord_y = 70;
    pos_9.coord_z = -145;

    pos_10.coord_x = 355;
    pos_10.coord_y = 70;
    pos_10.coord_z = -145;

    pos_11.coord_x = 390;
    pos_11.coord_y = 70;
    pos_11.coord_z = -145;

    pos_12.coord_x = 425;
    pos_12.coord_y = 70;
    pos_12.coord_z = -145;

    pos_13.coord_x = 460;
    pos_13.coord_y = 70;
    pos_13.coord_z = -145;

    pos_14.coord_x = 320;
    pos_14.coord_y = -70;
    pos_14.coord_z = -145;

    pos_15.coord_x = 355;
    pos_15.coord_y = -70;
    pos_15.coord_z = -145;

    pos_16.coord_x = 390;
    pos_16.coord_y = -70;
    pos_16.coord_z = -145;

    pos_17.coord_x = 425;
    pos_17.coord_y = -70;
    pos_17.coord_z = -145;

    pos_18.coord_x = 460;
    pos_18.coord_y = -70;
    pos_18.coord_z = -145;

    std::vector<posiciones_tablero> pos_tablero{pos_0, pos_1, pos_2, pos_3, pos_4, pos_5, pos_6, pos_7, pos_8};
    std::vector<posiciones_tablero> pos_fijas{pos_9, pos_10, pos_11, pos_12, pos_13, pos_14, pos_15, pos_16, pos_17, pos_18};
    
    for (int i = 0; i < 1; i++)
    {
        vector_2puntos_trayectoria[3*i] = pos_fijas[inicio].coord_x;
        vector_2puntos_trayectoria[3*i+1] = pos_fijas[inicio].coord_y;
        vector_2puntos_trayectoria[3*i+2] = pos_fijas[inicio].coord_z;
    }

    for (int i = 1; i < 2; i++)
    {
        vector_2puntos_trayectoria[3*i] = pos_tablero[fin].coord_x;
        vector_2puntos_trayectoria[3*i+1] = pos_tablero[fin].coord_y;
        vector_2puntos_trayectoria[3*i+2] = pos_tablero[fin].coord_z;
    }
    
    return vector_2puntos_trayectoria;
}
