#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "../lib/esp_random.h"
#include "Control.cpp"  //encontramos lo relacionado con las funciones para el control y los define
#include "Camara.h"
#include "Pinza.cpp"
#include "Estados.h"


//Variables globales
float _ref_motores[3]={90,240,90};

int contador_consola=0;

hw_timer_t *timer = NULL; //Puntero de variable para configurar timer 
HardwareSerial Serial_hmi(0); // define a Serial for UART1

const int Serial_hmi_RX = 18;
const int Serial_hmi_TX = 17;

//Función de prueba con el Timer 0. ISR de la interrupción
volatile bool has_expired = false;
void IRAM_ATTR timerInterrupcion() {
  has_expired = true;
}


/************************* PROGRAMA PRINCIPAL *************************/
void setup() {
  //Inicializacion de tablas
  InitTabla(_ek_pos_base,2);
  InitTabla(_ek_pos_brazo,2);
  InitTabla(_ek_pos_antebrazo,2);

  //PRUEBA BUZZER
  //pinMode(BUZZER_PIN,OUTPUT);
  //digitalWrite(BUZZER_PIN,HIGH);
  //delay(500);
  //digitalWrite(BUZZER_PIN,LOW);
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
  //_motors[1]=new Driver_L298n(PIN_M2_EN,PIN_M2_IN1,PIN_M2_IN2,PWM_FREQ_HZ );
  _motors[2]=new Driver_L298n(PIN_M3_EN,PIN_M3_IN1,PIN_M3_IN2,PWM_FREQ_HZ );
  //gpio_iomux_out(PIN_M3_EN, PWM0_OUT2B_IDX, 0);
  _motors[0]->begin();       // Usar _motors[0]->begin() si se ha declarado como array
  _motors[1]->begin(); 
  _motors[2]->begin(); 


  /*//Camara
    WiFi.mode(WIFI_MODE_STA);
    WiFi.disconnect();
    ESPNow.init();
    ESPNow.add_peer(receiver_mac);*/

  // Inicialización servomotor
  config_servo();
  Pinza.write(angulo_servo);

  // Comunicación Serial
  // Serial PC -  ESP32
  Serial.begin(115200);
  //gpio_iomux_out(43, U1TXD_OUT_IDX, 0);
  //gpio_iomux_in(44, U1RXD_IN_IDX);
  Serial2.begin(9600);

}

void loop() {
  //LECTURA DE LAS REFERENCIAS DE ANGULO ,POSTERIORMENTE LAS REFERENCIAS VENDRAN DE LA TRAYECTORIA
  // recvWithEndMarker(); // Función para recibir los datos desde el puerto serial
  // if (newData) { // Si se han recibido nuevos datos
  //   parseData(); // Función para convertir los datos a valores flotantes
  //   newData = false; // Reinicia la bandera de nuevos datos recibidos
  // }

  if(Serial2.available()>0) {
    Serial.print(Serial2.read());
  }

  //INTERRUPCION PARA REALIZAR EL CONTROL
  if(has_expired)
   {

      DesplazarTabla(_ek_pos_base,2);
      DesplazarTabla(_ek_pos_brazo,2);
      DesplazarTabla(_ek_pos_antebrazo,2);

      _ek_pos_base[0]=_ref_motores[0]-(LecturaEncoder(MOTOR_BASE)/REDUCCION_BASE);
      _ek_pos_brazo[0]=_ref_motores[1]-LecturaEncoder(MOTOR_BRAZO); 
      _ek_pos_antebrazo[0]=_ref_motores[2]-(LecturaEncoder(MOTOR_ANTEBRAZO)/REDUCCION_BASE);  

      u_integral_antebrazo=integral(_ek_pos_antebrazo[0],&_integral_motores[2],kI_ANTEBRAZO,TS);
      u_integral_base=integral(_ek_pos_base[0],&_integral_motores[0],kI_BASE,TS);

      //ControlPID_POS(_ek_pos_base,_uk_pos_base,KP_M_BASE,KD_M_BASE,u_integral_base,TS,MOTOR_BASE,DUTY_BASE);//Listo queda optimizar
      //ControlPD_POS(_ek_pos_brazo,_uk_pos_brazo,KP_M_BRAZO,KD_M_BRAZO,TS,MOTOR_BRAZO,DUTY_BRAZO);//calibrado
      //ControlPID_POS(_ek_pos_antebrazo,_uk_pos_antebrazo,KP_M_ANTEBRAZO,KD_M_ANTEBRAZO,u_integral_antebrazo,TS,MOTOR_ANTEBRAZO,DUTY_ANTE);//pendiente

      contador_consola++;
      if(contador_consola==25){
              //Serial.printf("_ek_pos_base: %f, _ek_pos_brazo: %f, _ek_pos_ante: %f \n",_ek_pos_base[0],_ek_pos_brazo[0],_ek_pos_antebrazo[0]);
              //printf("Accion integral: %f \n",u_integral_antebrazo);
              Serial.printf("Posicion del motor %d es: %f, u_integral: %f \n",MOTOR_BASE,LecturaEncoder(MOTOR_BASE)/REDUCCION_BASE,u_integral_base);
              Serial.printf("Posicion del motor %d es: %f \n",MOTOR_BRAZO,LecturaEncoder(MOTOR_BRAZO));
              Serial.printf("Posicion del motor %d es: %f , u_integral: %f \n",MOTOR_ANTEBRAZO,LecturaEncoder(MOTOR_ANTEBRAZO)/REDUCCION_BASE,u_integral_antebrazo);
              contador_consola=0; 
      }
      /*contador_cam++;
      if(contador_cam==20){
        static uint8_t a = 254;
        ESPNow.send_message(receiver_mac, &a, 1);
        //Serial.println(a++);
        contador_cam=0;
      }*/
      contador_servo++;
      if(contador_servo==150){
        if (angulo_servo >= CIERRE_SERVO) {
          angulo_servo = APERTURA_SERVO;
        } else {
          angulo_servo = CIERRE_SERVO;
        }
        Pinza.write(angulo_servo);
        //Serial.printf("Angulo del servo: %d\n", angulo_servo);
        contador_servo=0;
      }
      has_expired = false;
   }
   //printf("M1: %f, M2: %f, M3: %f \n",_ref_motores[0],_ref_motores[1],_ref_motores[2]);
   //delay(1000);

}

