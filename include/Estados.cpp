#include "Estados.h"
#include "Control.h"
#include "Pinza.h"
#include "cinematica.h"
#include "Buzzer.h"

int_two pos_trayectoria;
std::vector<float> trayectoria_ini_end;
std::vector<float> trayectoria_total;
std::vector<float> lectura_enc_cin(3);
// MODO MANUAL
string_two pos_ini_end;

bool flag_manual=false;

void modo_manual()  {
    
    if(strLectura != "end" && strLectura != "MANUAL" && strLectura != "" && flag_manual==false) {
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
    pixels.fill(0x024A86);
    pixels.show();

    // MODO MANUAL
    if (flag_manual)
    {
        _ek_pos_base[0]=_ref_motores[0]-lectura_enc_cin[0];
        _ek_pos_brazo[0]=_ref_motores[1]-lectura_enc_cin[1]; 
        _ek_pos_antebrazo[0]=_ref_motores[2]-lectura_enc_cin[2]; 
        
        u_integral_antebrazo=integral(_ek_pos_antebrazo[0],&_integral_motores[2],kI_ANTEBRAZO,TS);
        u_integral_base=integral(_ek_pos_base[0],&_integral_motores[0],kI_BASE,TS);
        u_integral_brazo=integral(_ek_pos_brazo[0],&_integral_motores[1],kI_BRAZO,TS);

        ControlPID_POS(_ek_pos_base,_uk_pos_base,KP_M_BASE,KD_M_BASE,u_integral_base,TS,MOTOR_BASE,DUTY_BASE);//Listo queda optimizar
        ControlPD_POS(_ek_pos_brazo,_uk_pos_brazo,KP_M_BRAZO,KD_M_BRAZO,u_integral_brazo,TS,MOTOR_BRAZO,DUTY_BRAZO);//calibrado
        ControlPID_POS(_ek_pos_antebrazo,_uk_pos_antebrazo,KP_M_ANTEBRAZO,KD_M_ANTEBRAZO,u_integral_antebrazo,TS,MOTOR_ANTEBRAZO,DUTY_ANTE);//pendiente
        
        contador_trayectoria++;
        if(contador_trayectoria==15){
            if(contador_pasos<30){
            if(contador_pasos==0) {
                cerrar_servo();
            }
            _ref_motores[MOTOR_BASE]=trayectoria_total[3*contador_pasos];
            _ref_motores[MOTOR_BRAZO]=trayectoria_total[1+3*contador_pasos];
            _ref_motores[MOTOR_ANTEBRAZO]=trayectoria_total[2+3*contador_pasos];
            Serial.printf("Referencia del motor %d es: %f:\n",MOTOR_BASE, _ref_motores[MOTOR_BASE]);
            Serial.printf("Referencia del motor %d es: %f \n",MOTOR_BRAZO,_ref_motores[MOTOR_BRAZO]);
            Serial.printf("Referencia del motor %d es: %f \n",MOTOR_ANTEBRAZO, _ref_motores[MOTOR_ANTEBRAZO]);
            Serial.printf("Posicion del motor %d es: %f:\n",MOTOR_BASE, lectura_enc_cin[0]);
            Serial.printf("Posicion del motor %d es: %f \n",MOTOR_BRAZO,lectura_enc_cin[1]);
            Serial.printf("Posicion del motor %d es: %f \n",MOTOR_ANTEBRAZO, lectura_enc_cin[2]);
            contador_pasos++;
            
            } 
            else {
                contador_pasos=0;
                abrir_servo();
                flag_manual = false;
            }
            contador_trayectoria=0;
        }
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
        // digitalWrite(BUZZER_PIN,LOW);
    }

    if (strLectura == "ENC_ZERO") {
        _encoder1->SetZero(MOTOR_BASE);
        _encoder1->SetZero(MOTOR_BRAZO);
        _encoder1->SetZero(MOTOR_ANTEBRAZO);
    }
    
    contador_consola++;
    if(contador_consola==50){
        Serial.printf("Posicion del motor %d es: %f:\n",MOTOR_BASE, lectura_enc_cin[0]);
        Serial.printf("Posicion del motor %d es: %f \n",MOTOR_BRAZO,lectura_enc_cin[1]);
        Serial.printf("Posicion del motor %d es: %f \n",MOTOR_ANTEBRAZO, lectura_enc_cin[2]);
        contador_consola=0; 
    }

    pixels.fill(0x02AC66);
    pixels.show();

}

void modo_automatico() {
    pixels.fill(0xC82A54);
    pixels.show();
}

void modo_inicial(){
    pixels.fill(0xE36B2C);
    pixels.show();
}

void modos_interrupcion() {
    
    DesplazarTabla(_ek_pos_base,2);
    DesplazarTabla(_ek_pos_brazo,2);
    DesplazarTabla(_ek_pos_antebrazo,2);

    lectura_encoder_base = LecturaEncoder(MOTOR_BASE)/REDUCCION_BASE;
    lectura_encoder_brazo = LecturaEncoder(MOTOR_BRAZO);
    lectura_encoder_antebrazo = LecturaEncoder(MOTOR_ANTEBRAZO)/REDUCCION_BASE;

    lectura_enc_cin = conversion_angulos_encoder(lectura_encoder_base, lectura_encoder_brazo, lectura_encoder_antebrazo);

    // if ((lectura_enc_cin[0]>=60 || lectura_enc_cin[0]<=-60) || (lectura_enc_cin[1]>=70 || lectura_enc_cin[1]<=10) || (lectura_enc_cin[2]>=135 || lectura_enc_cin[2]<=45)) {
    if ((lectura_enc_cin[0]>=10 || lectura_enc_cin[0]<=-10) ) {
        _motors[MOTOR_BASE]->SetDuty(0);
        _motors[MOTOR_BRAZO]->SetDuty(0);
        _motors[MOTOR_ANTEBRAZO]->SetDuty(0);
        flag_manual = false;
        contador_buzzer++;
        if(contador_buzzer>=20) {
            digitalWrite(BUZZER_PIN,LOW);
            contador_buzzer=20;
        } else {
            digitalWrite(BUZZER_PIN,HIGH);
        }
        
    } else {
        contador_buzzer=0;
    }
    
    /*************************** SWITCH CASE MODOS ***************************/
    switch (_estado_control)
    {
    case ESTADO_INICIAL:
        modo_inicial();
        break;

    case CALIBRACION:
        modo_calibracion();
        break;

    case AUTOMATICO:
        modo_automatico();
        break;

    case MANUAL:
        modo_manual();
        break;
    
    default:
        modo_inicial();     // Por defecto el robot empieza en modo calibración al energizarse.
        break;
    }
}