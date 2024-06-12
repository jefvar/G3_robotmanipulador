#include "parabola.h"

int h = h_f-h_0;

//Función Trayectoria Lineal -> Entrada: tres puntos iniciales + tres puntos finales
std::vector<float> f_trayectoria_parabolica(std:: vector <float> vector_2puntos_trayectoria){
    //Vectores puntos objetivos durante la trayectoria
    std::vector<float> x_objetivo(paso + 2*paso_h), y_objetivo(paso + 2*paso_h), z_objetivo(paso + 2*paso_h);
    //Vector angulos calculados durante la trayectoria -> control
    std::vector<float> angulos_trayectoria(3*(paso + 2*paso_h));
    std::vector<float> w(3);
    
    // Plano z, w
    // Vector w
    std::vector<float> w = {vector_2puntos_trayectoria[3] - vector_2puntos_trayectoria[0], vector_2puntos_trayectoria[4] - vector_2puntos_trayectoria[1], vector_2puntos_trayectoria[5] - vector_2puntos_trayectoria[2]};
    // Norma de W
    float normW = sqrt(pow(w[0],2) + pow(w[1],2) + pow(w[2],2));
    float theta = acos((vector_2puntos_trayectoria[3]-vector_2puntos_trayectoria[0])/normW);
    float k = normW/paso;

    for (int j = 0; j < paso + 2*paso_h; j++) {
        if (j<paso_h) {
            x_objetivo[j] = vector_2puntos_trayectoria[0];
            y_objetivo[j] = vector_2puntos_trayectoria[1];
            z_objetivo[j] = vector_2puntos_trayectoria[2] + j*h_0/paso_h;
        } else if (j>=paso_h && j<(paso+paso_h)) {
            float wt = j*k;
            x_objetivo[j] = vector_2puntos_trayectoria[0] + wt*cos(theta);
            y_objetivo[j] = vector_2puntos_trayectoria[1] + wt*sin(theta);
            z_objetivo[j] = vector_2puntos_trayectoria[2] + h_0 - -4*h*pow(wt,2)/pow(normW,2) + 4*h*wt/normW;
        } else {
            x_objetivo[j] = vector_2puntos_trayectoria[3];
            y_objetivo[j] = vector_2puntos_trayectoria[4];
            z_objetivo[j] = vector_2puntos_trayectoria[5] + h_0 - j*h_0/paso_h;
        }
        //Cinemática inversa -> paso a ángulos
        struct tres_posiciones angulo = f_cinematica_inversa(x_objetivo[j], y_objetivo[j], z_objetivo[j]);
        angulos_trayectoria[3*j] = angulo.pos_1;
        angulos_trayectoria[3*j+1] = angulo.pos_2;
        angulos_trayectoria[3*j+2] = angulo.pos_3;
    }

    return angulos_trayectoria;
}
