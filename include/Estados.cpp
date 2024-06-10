#include "Estados.h"
#include "Control.h"
#include "Pinza.h"
#include "cinematica.h"
#include "Buzzer.h"

int_two pos_trayectoria;
std::vector<float> trayectoria_ini_end;
std::vector<float> trayectoria_total;
// MODO MANUAL
string_two pos_ini_end;

void modo_manual()  {
    if(strLectura != "end" && strLectura != "MANUAL" && strLectura != "") {
        pos_ini_end = f_split_pos(strLectura);
        strLectura = "";
        pos_trayectoria.int_1 = pos_ini_end.string_1.toInt();
        pos_trayectoria.int_2 = pos_ini_end.string_2.toInt();
        trayectoria_ini_end = f_posiciones_inicio_fin(pos_trayectoria.int_1, pos_trayectoria.int_2);
        trayectoria_total = f_trayectoria_lineal(trayectoria_ini_end);
        for (int i = 0; i < 3*paso; i++)
        {
            Serial.printf("vector total de la trayectoria %i: %f \n",i, trayectoria_total[i]);
        }
        flag_manual = true;
    }
}

void modo_calibracion() {
    if(strLectura == "BASE_ANTI1") {
        _motors[MOTOR_BASE]->SetDuty(0.64);
        digitalWrite(BUZZER_PIN,HIGH);
    } else if (strLectura == "BASE_ANTI0") {
        _motors[MOTOR_BASE]->SetDuty(0);
        digitalWrite(BUZZER_PIN,LOW);
    } else if (strLectura == "BASE_HORA1") {
        _motors[MOTOR_BASE]->SetDuty(-0.64);
        digitalWrite(BUZZER_PIN,HIGH);
    } else if (strLectura == "BASE_HORA0") {
        _motors[MOTOR_BASE]->SetDuty(0);
        digitalWrite(BUZZER_PIN,LOW);
    } else if (strLectura == "BRAZO_ANTI1") {
        _motors[MOTOR_BRAZO]->SetDuty(0.54);
        digitalWrite(BUZZER_PIN,HIGH);
    } else if (strLectura == "BRAZO_ANTI0") {
        _motors[MOTOR_BRAZO]->SetDuty(0);
        digitalWrite(BUZZER_PIN,LOW);
    } else if (strLectura == "BRAZO_HORA1") {
        _motors[MOTOR_BRAZO]->SetDuty(-0.51);
        digitalWrite(BUZZER_PIN,HIGH);
    } else if (strLectura == "BRAZO_HORA0") {
        _motors[MOTOR_BRAZO]->SetDuty(0);
        digitalWrite(BUZZER_PIN,LOW);
    } else if (strLectura == "ANTE_ANTI1") {
        _motors[MOTOR_ANTEBRAZO]->SetDuty(0.56);
        digitalWrite(BUZZER_PIN,HIGH);
    } else if (strLectura == "ANTE_ANTI0") {
        _motors[MOTOR_ANTEBRAZO]->SetDuty(0);
        digitalWrite(BUZZER_PIN,LOW);
    } else if (strLectura == "ANTE_HORA1") {
        _motors[MOTOR_ANTEBRAZO]->SetDuty(-0.64);
        digitalWrite(BUZZER_PIN,HIGH);
    } else if (strLectura == "ANTE_HORA0") {
        _motors[MOTOR_ANTEBRAZO]->SetDuty(0);
        digitalWrite(BUZZER_PIN,LOW);
    } else if (strLectura == "PINZA_ON") {
        abrir_servo();
        //digitalWrite(BUZZER_PIN,HIGH);
    } else if (strLectura == "PINZA_OFF") {
        cerrar_servo();
        //digitalWrite(BUZZER_PIN,LOW);
    }
    else {
        _motors[MOTOR_BASE]->SetDuty(0);
        _motors[MOTOR_BRAZO]->SetDuty(0);
        _motors[MOTOR_ANTEBRAZO]->SetDuty(0);
        digitalWrite(BUZZER_PIN,LOW);
    }

    if (strLectura == "ENC_ZERO") {
        _encoder1->SetZero(MOTOR_BASE);
        _encoder1->SetZero(MOTOR_BRAZO);
        _encoder1->SetZero(MOTOR_ANTEBRAZO);
    }
    
}