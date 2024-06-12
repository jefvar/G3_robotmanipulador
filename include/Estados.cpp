#include "Estados.h"
#include "Control.h"
#include "Pinza.h"
#include "cinematica.h"
#include "Buzzer.h"

int_two pos_trayectoria;
std::vector<float> trayectoria_ini_end;
std::vector<float> trayectoria_total;
std::vector<float> trayectoria_total_total;
std::vector<float> lectura_enc_cin(3);
// MODO MANUAL
string_two pos_ini_end;

bool flag_manual=false;

void changePage(int page_id) {
  Serial_hmi.print("page ");
  Serial_hmi.print(page_id);
  Serial_hmi.write(0xFF); // Enviar terminador
  Serial_hmi.write(0xFF); // Enviar terminador
  Serial_hmi.write(0xFF); // Enviar terminador
}

void modo_manual()  {
    
    if(strLectura != "end" && strLectura != "MANUAL" && strLectura != "" && flag_manual==false) {
        pos_ini_end = f_split_pos(strLectura);
        strLectura = "";
        pos_trayectoria.int_1 = pos_ini_end.string_1.toInt();
        pos_trayectoria.int_2 = pos_ini_end.string_2.toInt();
        trayectoria_ini_end = f_posiciones_9puntos(pos_trayectoria.int_1, pos_trayectoria.int_2);
        for(int i=0; i<8; i++) {
            trayectoria_total = f_trayectoria_lineal(trayectoria_ini_end[3*i],trayectoria_ini_end[3*i+1], trayectoria_ini_end[3*i+2], trayectoria_ini_end[3*i+3], trayectoria_ini_end[3*i+4], trayectoria_ini_end[3*i+5]);
            trayectoria_total_total.insert(trayectoria_total_total.end(), trayectoria_total.begin(), trayectoria_total.end());
        }
        
        for (int i = 0; i < 720; i++)
        {
            Serial.printf("vector total de la trayectoria %i: %f \n",i, trayectoria_total_total[i]);
        }
        flag_manual = true;
    }
    pixels.fill(0x024A86);
    pixels.show();

    // MODO MANUAL
    if (flag_manual)
    // if (false)
    {
        _ek_pos_base[0]=_ref_motores[0]-lectura_enc_cin[0];
        _ek_pos_brazo[0]=_ref_motores[1]-lectura_enc_cin[1]; 
        _ek_pos_antebrazo[0]=_ref_motores[2]-lectura_enc_cin[2]; 
        
        u_integral_antebrazo=integral(_ek_pos_antebrazo[0],&_integral_motores[2],kI_ANTEBRAZO_BAJADA,TS);
        u_integral_base=integral(_ek_pos_base[0],&_integral_motores[0],kI_BASE,TS);
        u_integral_brazo=integral(_ek_pos_brazo[0],&_integral_motores[1],kI_BRAZO,TS);

        // ControlPID_POS(_ek_pos_base,KP_M_BASE,KD_M_BASE,u_integral_base,TS,MOTOR_BASE,DUTY_BASE);//Listo queda optimizar
        // ControlPD_POS(_ek_pos_brazo,KP_M_BRAZO,KD_M_BRAZO,u_integral_brazo,TS,MOTOR_BRAZO,DUTY_BRAZO);//calibrado
        // ControlPID_POS_ANTEBRAZO(_ek_pos_antebrazo,KP_M_ANTEBRAZO_SUBIDA,KP_M_ANTEBRAZO_BAJADA,KD_M_ANTEBRAZO_SUBIDA,KD_M_ANTEBRAZO_BAJADA,u_integral_antebrazo,TS,DUTY_ANTE);
        
        ControlPID_POS_HOME(_ek_pos_base,_ek_pos_brazo, _ek_pos_antebrazo,u_integral_base, u_integral_brazo, u_integral_antebrazo);

        contador_trayectoria++;
        if(contador_trayectoria==15){
            if(contador_pasos<240){
            if(contador_pasos==0) {
                cerrar_servo();
            }
            _ref_motores[MOTOR_BASE]=trayectoria_total_total[3*contador_pasos];
            _ref_motores[MOTOR_BRAZO]=trayectoria_total_total[1+3*contador_pasos];
            _ref_motores[MOTOR_ANTEBRAZO]=trayectoria_total_total[2+3*contador_pasos];
            Serial.printf("Referencia es: %f, \t %f, \t %f\n", _ref_motores[MOTOR_BASE], _ref_motores[MOTOR_BRAZO], _ref_motores[MOTOR_ANTEBRAZO]);
            // Serial.printf("Posicion del motor %d es: %f:\n",MOTOR_BASE, lectura_enc_cin[0]);
            // Serial.printf("Posicion del motor %d es: %f \n",MOTOR_BRAZO,lectura_enc_cin[1]);
            // Serial.printf("Posicion del motor %d es: %f \n",MOTOR_ANTEBRAZO, lectura_enc_cin[2]);
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
        _motors[MOTOR_ANTEBRAZO]->SetDuty(0.60);        // Bajada
        digitalWrite(BUZZER_PIN,HIGH);
    } else if (strLectura == "ANTE_ANTI0") {
        _motors[MOTOR_ANTEBRAZO]->SetDuty(0);
        digitalWrite(BUZZER_PIN,LOW);
    } else if (strLectura == "ANTE_HORA1") {
        _motors[MOTOR_ANTEBRAZO]->SetDuty(-0.69);        // Subida
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
        strLectura="";
    }
    
    if(strLectura == "ENC_BASE") {
    
        dtostrf(lectura_enc_cin[0],6,3,buffer);
        Serial_hmi.print("t0.txt=\"");
        Serial_hmi.print(buffer);
        // Serial_hmi.print("°");
        Serial_hmi.print("\"");
        Serial_hmi.write(0xff);
        Serial_hmi.write(0xff);
        Serial_hmi.write(0xff);
        memset(buffer, 0, sizeof(buffer));
        strLectura="";

    } else if (strLectura == "ENC_BRAZO") {
        //Con esta opcion funciona en caja de texto para la variable, habra que ver si funciona para el encoder
        //value=value+1;
        dtostrf(lectura_enc_cin[1],7,3,buffer);
        Serial_hmi.print("t0.txt=\"");
        Serial_hmi.print(buffer);
        Serial_hmi.print("\"");
        Serial_hmi.write(0xff);
        Serial_hmi.write(0xff);
        Serial_hmi.write(0xff);
        memset(buffer, 0, sizeof(buffer));
        strLectura="";

    } else if (strLectura == "ENC_ANTE") {

        dtostrf(lectura_enc_cin[2],7,3,buffer);
        Serial_hmi.print("t0.txt=\"");
        Serial_hmi.print(buffer);
        Serial_hmi.print("\"");
        Serial_hmi.write(0xff);
        Serial_hmi.write(0xff);
        Serial_hmi.write(0xff);
        memset(buffer, 0, sizeof(buffer));
        strLectura="";

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
    _ek_pos_antebrazo[0]=_ref_motores[2]-lectura_enc_cin[2]; 
    u_integral_antebrazo=integral(_ek_pos_antebrazo[0],&_integral_motores[2],kI_ANTEBRAZO_BAJADA,TS);
    ControlPID_POS_ANTEBRAZO(_ek_pos_antebrazo,KP_M_ANTEBRAZO_SUBIDA,KP_M_ANTEBRAZO_BAJADA,KD_M_ANTEBRAZO_SUBIDA,KD_M_ANTEBRAZO_BAJADA,u_integral_antebrazo,TS,DUTY_ANTE);
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

    if ((lectura_enc_cin[0]>=45 || lectura_enc_cin[0]<=-45) || (lectura_enc_cin[1]>=70 || lectura_enc_cin[1]<=20) || (lectura_enc_cin[2]>=135 || lectura_enc_cin[2]<=45)) {
        _motors[MOTOR_BASE]->SetDuty(0);
        _motors[MOTOR_BRAZO]->SetDuty(0);
        _motors[MOTOR_ANTEBRAZO]->SetDuty(0);
        _estado_control = ESTADO_INICIAL;
        flag_manual = false;
        contador_buzzer++;
        if(contador_buzzer>=25) {
            digitalWrite(BUZZER_PIN,LOW);
            contador_buzzer=25;
        } else {
            digitalWrite(BUZZER_PIN,HIGH);
        }
        
    } else {
        contador_buzzer=0;
    }

    // _ek_pos_base[0]=_ref_motores[0]-lectura_enc_cin[0];
    // _ek_pos_brazo[0]=_ref_motores[1]-lectura_enc_cin[1]; 
    // _ek_pos_antebrazo[0]=_ref_motores[2]-lectura_enc_cin[2]; 
    
    // u_integral_base=integral(_ek_pos_base[0],&_integral_motores[0],kI_BASE,TS);
    // u_integral_brazo=integral(_ek_pos_brazo[0],&_integral_motores[1],kI_BRAZO,TS);
    // u_integral_antebrazo=integral(_ek_pos_antebrazo[0],&_integral_motores[2],kI_ANTEBRAZO_BAJADA,TS);

    // ControlPID_POS_HOME(_ek_pos_base,_ek_pos_brazo, _ek_pos_antebrazo,u_integral_base, u_integral_brazo, u_integral_antebrazo);
    
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