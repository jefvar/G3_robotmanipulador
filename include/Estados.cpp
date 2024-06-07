#include "Estados.h"
#include "Control.h"
#include "Pinza.h"

int numero_turnos = 0;
float duty_calibracion = 0.8;

void modo_manual()  {
    while(strLectura != "end") {

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
        _motors[MOTOR_ANTEBRAZO]->SetDuty(0.52);
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
        digitalWrite(BUZZER_PIN,HIGH);
    } else if (strLectura == "PINZA_OFF") {
        cerrar_servo();
        digitalWrite(BUZZER_PIN,LOW);
    }
    else {
        _motors[MOTOR_BASE]->SetDuty(0);
        _motors[MOTOR_BRAZO]->SetDuty(0);
        _motors[MOTOR_ANTEBRAZO]->SetDuty(0);
        digitalWrite(BUZZER_PIN,LOW);
    }
}