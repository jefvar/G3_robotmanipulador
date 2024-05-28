#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "../lib/esp_random.h"
#include "Control.cpp"  //encontramos lo relacionado con las funciones para el control y los define
#include "Camara.h"

//Variables globales
float _ref_motores[3]={180,100,180};
char receivedChars[32]; // Buffer para almacenar los caracteres recibidos del puerto serial
boolean newData = false; // Bandera para indicar que se han recibido nuevos datos
//camara
uint8_t receiver_mac[] = {0x94 , 0xB5 , 0x55 , 0xFC , 0x38 , 0x1C};
int contador_cam=0;

int contador=0;

String inputString = "";
float _ek_pos_base[2]; //error de posicion estado actual y anterior
float _uk_pos_base[2]; //accion de control actual y anterior
float _ek_pos_brazo[2]; 
float _uk_pos_brazo[2]; 
float _ek_pos_antebrazo[2]; 
float _uk_pos_antebrazo[2];
float _integral_motores[3]={0.0,0.0,0.0};
float u_integral_antebrazo=0;

hw_timer_t *timer = NULL; //Puntero de variable para configurar timer 

//Función de prueba con el Timer 0. ISR de la interrupción
volatile bool has_expired = false;
void IRAM_ATTR timerInterrupcion() {
  has_expired = true;
  //printf("Hola \n");
}

/************************* PROGRAMA PRINCIPAL *************************/
void setup() {
  Serial.begin(115200);
  //Inicializacion de tablas
  InitTabla(_ek_pos_base,2);
  InitTabla(_uk_pos_base,2);
  InitTabla(_ek_pos_brazo,2);
  InitTabla(_uk_pos_brazo,2);
  InitTabla(_ek_pos_antebrazo,2);
  InitTabla(_uk_pos_antebrazo,2);

  // CONFIGURAR AQUI LA INTERRUPCION
  // El código a continuación se utilizó para realizar una prueba con el led RGB con un timer (Timer 0). No es necesario para el funcionamiento del robot*/
  timer = timerBegin(0, 80, true); // Timer 0, clock divider 80
  timerAttachInterrupt(timer, &timerInterrupcion, true); // Attach the interrupt handling function
  timerAlarmWrite(timer, TS*1000000, true); // Interrupt every 1 second
  timerAlarmEnable(timer); // Enable the alarm

  /* Motor 1 */
  int cs_pins[]={PIN_CS_BASE,PIN_CS_BRAZO,PIN_CS_ANTEBRAZO};
  _encoder1=new EncoderATM203_Spi2(cs_pins,N_MOTORS,PIN_MOSI,PIN_MISO,PIN_SCLK);
  _motors[0]=new Driver_L298n(PIN_M1_EN,PIN_M1_IN1,PIN_M1_IN2,PWM_FREQ_HZ );
  _motors[1]=new Driver_L298n(PIN_M2_EN,PIN_M2_IN1,PIN_M2_IN2,PWM_FREQ_HZ );
  _motors[2]=new Driver_L298n(PIN_M3_EN,PIN_M3_IN1,PIN_M3_IN2,PWM_FREQ_HZ );
  //gpio_iomux_out(PIN_M3_EN, PWM0_OUT2B_IDX, 0);
  _motors[0]->begin();       // Usar _motors[0]->begin() si se ha declarado como array
  _motors[1]->begin(); 
  _motors[2]->begin(); 


  //Camara
    WiFi.mode(WIFI_MODE_STA);
    WiFi.disconnect();
    ESPNow.init();
    ESPNow.add_peer(receiver_mac);

}

void loop() {
  //LECTURA DE LAS REFERENCIAS DE ANGULO ,POSTERIORMENTE LAS REFERENCIAS VENDRAN DE LA TRAYECTORIA
  recvWithEndMarker(); // Función para recibir los datos desde el puerto serial
  if (newData) { // Si se han recibido nuevos datos
    parseData(); // Función para convertir los datos a valores flotantes
    newData = false; // Reinicia la bandera de nuevos datos recibidos
  }

  //INTERRUPCION PARA REALIZAR EL CONTROL
     if(has_expired)
   {

      DesplazarTabla(_ek_pos_base,2);
      //DesplazarTabla(_uk_pos_base,2);
      DesplazarTabla(_ek_pos_brazo,2);
      //DesplazarTabla(_uk_pos_brazo,2); //en brazo no se utilza la integral, funciona bien sin ella
      DesplazarTabla(_ek_pos_antebrazo,2);
      DesplazarTabla(_uk_pos_antebrazo,2);

      /*_ek_pos_base[0]=_ref_motores[0]-LecturaEncoder(MOTOR_BASE);
      //Serial.printf("_ek_pos_base: %f \n",_ek_pos_base[0]);
      _ek_pos_brazo[0]=_ref_motores[1]-LecturaEncoder(MOTOR_BRAZO); */
      _ek_pos_antebrazo[0]=_ref_motores[2]-LecturaEncoder(MOTOR_ANTEBRAZO);  

      u_integral_antebrazo=integral(_ek_pos_antebrazo[0],&_integral_motores[3],ki,TS);

      //ControlPID_POS(_ek_pos_base,_uk_pos_base,KP_M_BASE,KD_M_BASE,MOTOR_BASE,DUTY_BASE);//problema en la base
      //ControlPD_POS(_ek_pos_brazo,_uk_pos_brazo,KP_M_BRAZO,KD_M_BRAZO,MOTOR_BRAZO,DUTY_BRAZO);//calibrado
      ControlPID_POS(_ek_pos_antebrazo,_uk_pos_antebrazo,KP_M_ANTEBRAZO,KD_M_ANTEBRAZO,u_integral_antebrazo,TS,MOTOR_ANTEBRAZO,DUTY_ANTE);//pendiente

      contador++;
      if(contador==14){
              Serial.printf("_ek_pos_base: %f, _ek_pos_brazo: %f, _ek_pos_ante: %f \n",_ek_pos_base[0],_ek_pos_brazo[0],_ek_pos_antebrazo[0]);
              //printf("Accion integral: %f \n",u_integral_antebrazo);
              contador=0; 
      }
      contador_cam++;
      if(contador_cam==20){
        static uint8_t a = 254;
        ESPNow.send_message(receiver_mac, &a, 1);
        //Serial.println(a++);
        contador_cam=0;
      }
      has_expired = false; 
   }
   //printf("M1: %f, M2: %f, M3: %f \n",_ref_motores[0],_ref_motores[1],_ref_motores[2]);
   //delay(1000);

}

