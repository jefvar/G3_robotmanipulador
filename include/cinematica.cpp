#include "cinematica.h"

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
std::vector<float> f_trayectoria_lineal(float p_ini_x, float p_ini_y, float p_ini_z, float p_fin_x, float p_fin_y, float p_fin_z){
    //Vectores puntos objetivos durante la trayectoria
    std::vector<float> x_objetivo(paso), y_objetivo(paso), z_objetivo(paso);
    //Vector angulos calculados durante la trayectoria -> control
    std::vector<float> angulos_trayectoria(3*paso);
    
    for (int j = 0; j < paso; ++j){
        x_objetivo[j] = p_ini_x + (p_fin_x - p_ini_x) * j / (paso - 1);
        y_objetivo[j] = p_ini_y + (p_fin_y - p_ini_y) * j / (paso - 1);
        z_objetivo[j] = p_ini_z + (p_fin_z - p_ini_z) * j / (paso - 1);
        
        //Cinemática inversa -> paso a ángulos
        struct tres_posiciones angulo = f_cinematica_inversa(x_objetivo[j], y_objetivo[j], z_objetivo[j]);
        angulos_trayectoria[3*j] = angulo.pos_1;
        angulos_trayectoria[3*j+1] = angulo.pos_2;
        angulos_trayectoria[3*j+2] = angulo.pos_3;
    }
    return angulos_trayectoria;
}

//Conversión de ángulos
std::vector<float> conversion_angulos(std::vector<float> vector_angulos_programa){
std::vector<float> vector_angulos_robot(3*paso);
    //Valor definido del ángulo de la base
    for (int j = 0; j < paso; ++j){
        vector_angulos_robot[3*j] = def_theta_0 - vector_angulos_programa[3*j];
        vector_angulos_robot[3*j+1] = def_theta_1 - vector_angulos_programa[3*j+1];
        vector_angulos_robot[3*j+2] = def_theta_2 - vector_angulos_programa[3*j+2];
    }
    return vector_angulos_robot;
}

//Conversión de ángulos encoder
std::vector<float> conversion_angulos_encoder(float theta0_enc, float theta1_enc, float theta2_enc){
    std::vector<float> vector_angulos_encoder(3);
    // Referencia punto 0.1
    std::vector<float> referencia_calibracion_pos = f_posiciones_2puntos(0, 1);
    struct tres_posiciones referencia_calibracion_angulo = f_cinematica_inversa(referencia_calibracion_pos[3], referencia_calibracion_pos[4], referencia_calibracion_pos[5]);
    vector_angulos_encoder[0] = theta0_enc + referencia_calibracion_angulo.pos_1;
    vector_angulos_encoder[1] = -theta1_enc + referencia_calibracion_angulo.pos_2;
    vector_angulos_encoder[2] = theta2_enc + referencia_calibracion_angulo.pos_3;
    return vector_angulos_encoder;
}

//Función separación del array inicial en pos_fija_inicial y pos_tablero_final
struct string_two f_split_pos(String movimiento){
    struct string_two digitos;
    int inicio=0, fin;
    fin = movimiento.indexOf(separador,inicio);
    digitos.string_1 = movimiento.substring(inicio,fin);
    digitos.string_2 = movimiento.substring(fin+1); //revisar que el substring no seleccione el punto -> fin+1
    
    return digitos;
}

//Función para obtener las coordenadas xyz de los puntos inicial y final de la trayectoria
std::vector <float> f_posiciones_9puntos(int inicio, int fin){
    struct posiciones_tablero {float coord_x; float coord_y; float coord_z;}pos_0, pos_1, pos_2, pos_3, pos_4, 
    pos_5, pos_6, pos_7, pos_8, pos_9, pos_10, pos_11, pos_12, pos_13, pos_14, pos_15, pos_16, pos_17, pos_18,
    pos_19, pos_20, pos_21, pos_22, pos_23, pos_24, pos_25, pos_26,pos_27, pos_28,
    pos_29, pos_30, pos_31, pos_32, pos_33, pos_34, pos_35, pos_36, pos_37, pos_38, pos_39;
    //Vector donde se almacenan las 6 coordenadas 3punto inicial 3 punto final
    std:: vector<float> vector_9puntos_trayectoria(27);
    //Posiciones en el tablero
    pos_0.coord_x = 350.75;
    pos_0.coord_y = -37;
    pos_0.coord_z = -135;

    pos_1.coord_x = 350.75;
    pos_1.coord_y = 0;
    pos_1.coord_z = -135;

    pos_2.coord_x = 350.75;
    pos_2.coord_y = 37;
    pos_2.coord_z = -135;

    pos_3.coord_x = 390;
    pos_3.coord_y = -37;
    pos_3.coord_z = -135;

    pos_4.coord_x = 390;
    pos_4.coord_y = 0;
    pos_4.coord_z = -135;

    pos_5.coord_x = 390;
    pos_5.coord_y = 37;
    pos_5.coord_z = -135;

    pos_6.coord_x = 429.25;
    pos_6.coord_y = -37;
    pos_6.coord_z = -135;

    pos_7.coord_x = 429.25;
    pos_7.coord_y = 0;
    pos_7.coord_z = -135;

    pos_8.coord_x = 429.25;
    pos_8.coord_y = 37;
    pos_8.coord_z = -135;

    //Posiciones fijas de las fichas
    pos_9.coord_x = 320;
    pos_9.coord_y = -70;
    pos_9.coord_z = -135;

    pos_10.coord_x = 355;
    pos_10.coord_y = 70;
    pos_10.coord_z = -135;

    pos_11.coord_x = 390;
    pos_11.coord_y = -70;
    pos_11.coord_z = -135;

    pos_12.coord_x = 425;
    pos_12.coord_y = -70;
    pos_12.coord_z = -135;

    pos_13.coord_x = 460;
    pos_13.coord_y = -70;
    pos_13.coord_z = -135;

    pos_14.coord_x = 320;
    pos_14.coord_y = 70;
    pos_14.coord_z = -135;

    pos_15.coord_x = 355;
    pos_15.coord_y = 70;
    pos_15.coord_z = -135;

    pos_16.coord_x = 390;
    pos_16.coord_y = 70;
    pos_16.coord_z = -135;

    pos_17.coord_x = 425;
    pos_17.coord_y = 70;
    pos_17.coord_z = -135;

    pos_18.coord_x = 460;
    pos_18.coord_y = 70;
    pos_18.coord_z = -135;

    //Posiciones fijas de las fichas elevadas a z=-95
    pos_19.coord_x = 320;
    pos_19.coord_y = -70;
    pos_19.coord_z = -95;

    pos_20.coord_x = 355;
    pos_20.coord_y = 70;
    pos_20.coord_z = -95;

    pos_21.coord_x = 390;
    pos_21.coord_y = -70;
    pos_21.coord_z = -95;

    pos_22.coord_x = 425;
    pos_22.coord_y = -70;
    pos_22.coord_z = -95;

    pos_23.coord_x = 460;
    pos_23.coord_y = -70;
    pos_23.coord_z = -95;

    pos_24.coord_x = 320;
    pos_24.coord_y = 70;
    pos_24.coord_z = -95;

    pos_25.coord_x = 355;
    pos_25.coord_y = 70;
    pos_25.coord_z = -95;

    pos_26.coord_x = 390;
    pos_26.coord_y = 70;
    pos_26.coord_z = -95;

    pos_27.coord_x = 425;
    pos_27.coord_y = 70;
    pos_27.coord_z = -95;

    pos_28.coord_x = 460;
    pos_28.coord_y = 70;
    pos_28.coord_z = -95;

    //Posiciones elevadas del tablero a z=-95
    pos_29.coord_x = 350.75;
    pos_29.coord_y = -37;
    pos_29.coord_z = -95;

    pos_30.coord_x = 350.75;
    pos_30.coord_y = 0;
    pos_30.coord_z = -95;

    pos_31.coord_x = 350.75;
    pos_31.coord_y = 37;
    pos_31.coord_z = -95;

    pos_32.coord_x = 390;
    pos_32.coord_y = -37;
    pos_32.coord_z = -95;

    pos_33.coord_x = 390;
    pos_33.coord_y = 0;
    pos_33.coord_z = -95;

    pos_34.coord_x = 390;
    pos_34.coord_y = 37;
    pos_34.coord_z = -95;

    pos_35.coord_x = 429.25;
    pos_35.coord_y = -37;
    pos_35.coord_z = -95;

    pos_36.coord_x = 429.25;
    pos_36.coord_y = 0;
    pos_36.coord_z = -95;

    pos_37.coord_x = 429.25;
    pos_37.coord_y = 37;
    pos_37.coord_z = -95;

    //Posición de referencia 
    pos_38.coord_x = 390;
    pos_38.coord_y = 0;
    pos_38.coord_z = -80;

    //Posición HOME
    // pos_39.coord_x = 360;
    // pos_39.coord_y = 0;
    // pos_39.coord_z = -80;
    struct tres_posiciones home_ini = f_cinematica_directa(0, 30, 90);
    pos_39.coord_x = home_ini.pos_1;
    pos_39.coord_y = home_ini.pos_2;
    pos_39.coord_z = home_ini.pos_3;
    
    //vector que almacena las posiciones del tablero
    std::vector<posiciones_tablero> pos_tablero{pos_0, pos_1, pos_2, pos_3, pos_4, pos_5, pos_6, pos_7, pos_8};
    //vector que almacenas las posiciones fijas/de inicio
    std::vector<posiciones_tablero> pos_fijas{pos_9, pos_10, pos_11, pos_12, pos_13, pos_14, pos_15, pos_16, pos_17, pos_18};
    //vector que almacenas las posiciones fijas/ con z elevada + posicion de referencia con z elevada en el tablero
    std::vector<posiciones_tablero> pos_fijas_altura{pos_19, pos_20, pos_21, pos_22, pos_23, pos_24, pos_25, pos_26, pos_27, pos_28};
    //vector que almacenas las posiciones tablero/ con z elevada + posicion de referencia con z elevada en el tablero
    std::vector<posiciones_tablero> pos_tablero_altura{pos_29, pos_30, pos_31, pos_32, pos_33, pos_34, pos_35, pos_36, pos_37};
    //HOME y REFERENCIA
    std::vector<posiciones_tablero> pos_referencia{pos_38, pos_39};
    
    for (int i = 0; i < 1; i++)
    {
        //Primera posición HOME
        vector_9puntos_trayectoria[3*i] = pos_referencia[1].coord_x;
        vector_9puntos_trayectoria[3*i+1] = pos_referencia[1].coord_y;
        vector_9puntos_trayectoria[3*i+2] = pos_referencia[1].coord_z;
    }

    for (int i = 1; i < 2; i++)
    {
        //Posición fija en altura
        vector_9puntos_trayectoria[3*i] = pos_fijas_altura[inicio].coord_x;
        vector_9puntos_trayectoria[3*i+1] = pos_fijas_altura[inicio].coord_y;
        vector_9puntos_trayectoria[3*i+2] = pos_fijas_altura[inicio].coord_z;
    }

    for (int i = 2; i < 3; i++)
    {
        //Posición fija abajo
        vector_9puntos_trayectoria[3*i] = pos_fijas[inicio].coord_x;
        vector_9puntos_trayectoria[3*i+1] = pos_fijas[inicio].coord_y;
        vector_9puntos_trayectoria[3*i+2] = pos_fijas[inicio].coord_z;
    }

    for (int i = 3; i < 4; i++)
    {
        //Posición fija abajo
        vector_9puntos_trayectoria[3*i] = pos_fijas_altura[inicio].coord_x;
        vector_9puntos_trayectoria[3*i+1] = pos_fijas_altura[inicio].coord_y;
        vector_9puntos_trayectoria[3*i+2] = pos_fijas_altura[inicio].coord_z;
    }

    for (int i = 4; i < 5; i++)
    {
        //Posición referencia
        vector_9puntos_trayectoria[3*i] = pos_referencia[2].coord_x;
        vector_9puntos_trayectoria[3*i+1] = pos_referencia[2].coord_x;
        vector_9puntos_trayectoria[3*i+2] = pos_referencia[2].coord_x;
    }

    for (int i = 5; i < 6; i++)
    {
        //Posición tablero altura
        vector_9puntos_trayectoria[3*i] = pos_tablero_altura[fin].coord_x;
        vector_9puntos_trayectoria[3*i+1] = pos_tablero_altura[fin].coord_y;
        vector_9puntos_trayectoria[3*i+2] = pos_tablero_altura[fin].coord_z;
    }

    for (int i = 6; i < 7; i++)
    {
        //Posición tablero abajo
        vector_9puntos_trayectoria[3*i] = pos_tablero[fin].coord_x;
        vector_9puntos_trayectoria[3*i+1] = pos_tablero[fin].coord_y;
        vector_9puntos_trayectoria[3*i+2] = pos_tablero[fin].coord_z;
    }
    
    for (int i = 7; i < 8; i++)
    {
        //Posición tablero altura
        vector_9puntos_trayectoria[3*i] = pos_tablero_altura[fin].coord_x;
        vector_9puntos_trayectoria[3*i+1] = pos_tablero_altura[fin].coord_y;
        vector_9puntos_trayectoria[3*i+2] = pos_tablero_altura[fin].coord_z;
    }

    for (int i = 8; i < 9; i++)
    {
        //Última posición HOME
        vector_9puntos_trayectoria[3*i] = pos_referencia[1].coord_x;
        vector_9puntos_trayectoria[3*i+1] = pos_referencia[1].coord_y;
        vector_9puntos_trayectoria[3*i+2] = pos_referencia[1].coord_z;
    }
    return vector_9puntos_trayectoria;
}

std::vector <float> f_posiciones_2puntos(int inicio, int fin){
    struct posiciones_tablero {float coord_x; float coord_y; float coord_z;}pos_0, pos_1, pos_2, pos_3, pos_4, 
    pos_5, pos_6, pos_7, pos_8, pos_9, pos_10, pos_11, pos_12, pos_13, pos_14, pos_15, pos_16, pos_17, pos_18;
    std:: vector<float> vector_2puntos_trayectoria(6);
    //Posiciones en el tablero
    pos_0.coord_x = 350.75;
    pos_0.coord_y = -37;
    pos_0.coord_z = -135;

    pos_1.coord_x = 350.75;
    pos_1.coord_y = 0;
    pos_1.coord_z = -135;

    pos_2.coord_x = 350.75;
    pos_2.coord_y = 37;
    pos_2.coord_z = -135;

    pos_3.coord_x = 390;
    pos_3.coord_y = -37;
    pos_3.coord_z = -135;

    pos_4.coord_x = 390;
    pos_4.coord_y = 0;
    pos_4.coord_z = -135;

    pos_5.coord_x = 390;
    pos_5.coord_y = 37;
    pos_5.coord_z = -135;

    pos_6.coord_x = 429.25;
    pos_6.coord_y = -37;
    pos_6.coord_z = -135;

    pos_7.coord_x = 429.25;
    pos_7.coord_y = 0;
    pos_7.coord_z = -135;

    pos_8.coord_x = 429.25;
    pos_8.coord_y = 37;
    pos_8.coord_z = -135;

    //Posiciones fijas de las fichas
    pos_9.coord_x = 320;
    pos_9.coord_y = -70;
    pos_9.coord_z = -135;

    pos_10.coord_x = 355;
    pos_10.coord_y = 70;
    pos_10.coord_z = -135;

    pos_11.coord_x = 390;
    pos_11.coord_y = -70;
    pos_11.coord_z = -135;

    pos_12.coord_x = 425;
    pos_12.coord_y = -70;
    pos_12.coord_z = -135;

    pos_13.coord_x = 460;
    pos_13.coord_y = -70;
    pos_13.coord_z = -135;

    pos_14.coord_x = 320;
    pos_14.coord_y = 70;
    pos_14.coord_z = -135;

    pos_15.coord_x = 355;
    pos_15.coord_y = 70;
    pos_15.coord_z = -135;

    pos_16.coord_x = 390;
    pos_16.coord_y = 70;
    pos_16.coord_z = -135;

    pos_17.coord_x = 425;
    pos_17.coord_y = 70;
    pos_17.coord_z = -135;

    pos_18.coord_x = 460;
    pos_18.coord_y = 70;
    pos_18.coord_z = -135;

    //vector que almacena las posiciones del tablero
    std::vector<posiciones_tablero> pos_tablero{pos_0, pos_1, pos_2, pos_3, pos_4, pos_5, pos_6, pos_7, pos_8};
    //vector que almacenas las posiciones fijas/de inicio
    std::vector<posiciones_tablero> pos_fijas{pos_9, pos_10, pos_11, pos_12, pos_13, pos_14, pos_15, pos_16, pos_17, pos_18};

    for (int i = 0; i < 1; i++)
    {
        //Posición tablero altura
        vector_2puntos_trayectoria[3*i] = pos_fijas[inicio].coord_x;
        vector_2puntos_trayectoria[3*i+1] = pos_fijas[inicio].coord_y;
        vector_2puntos_trayectoria[3*i+2] = pos_fijas[inicio].coord_z;
    }

    for (int i = 1; i < 2; i++)
    {
        //Última posición HOME
        vector_2puntos_trayectoria[3*i] = pos_tablero[fin].coord_x;
        vector_2puntos_trayectoria[3*i+1] = pos_tablero[fin].coord_y;
        vector_2puntos_trayectoria[3*i+2] = pos_tablero[fin].coord_z;
    }
    return vector_2puntos_trayectoria;
}