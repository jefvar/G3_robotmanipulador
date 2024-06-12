#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "../lib/esp_random.h"
#include "Control.cpp"  //encontramos lo relacionado con las funciones para el control y los define
#include "Camara.h"
#include "Pinza.cpp"
#include "Estados.cpp"
#include "Buzzer.cpp"
#include "cinematica.cpp"

//Variables globales
// float _ref_motores[3]={90,100,90};
float _ref_motores[3]={0,30,90};

hw_timer_t *timer = NULL; //Puntero de variable para configurar timer 
HardwareSerial Serial_hmi(1);

const int Serial_hmi_RX = 18;
const int Serial_hmi_TX = 17;

// VARIABLES GLOBALES

String strLectura;

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
  InitTabla(_uk_pos_brazo,2);

  //PRUEBA BUZZER
  pinMode(BUZZER_PIN,OUTPUT);

  // LED RGB
  pixels.begin();
  pixels.setBrightness(10);
  pixels.show(); // Initialize all pixels to 'off'


  // CONFIGURAR AQUI LA INTERRUPCION
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
  Serial_hmi.begin(115200, SERIAL_8N1, Serial_hmi_RX, Serial_hmi_TX);
  changePage(0);
}

void loop() {
  //LECTURA DE LAS REFERENCIAS DE ANGULO ,POSTERIORMENTE LAS REFERENCIAS VENDRAN DE LA TRAYECTORIA
  recvWithEndMarker(); // Función para recibir los datos desde el puerto serial
  if (newData) { // Si se han recibido nuevos datos
    parseData(); // Función para convertir los datos a valores flotantes
    newData = false; // Reinicia la bandera de nuevos datos recibidos
  }

  if (Serial_hmi.available() > 0) {
    strLectura = Serial_hmi.readStringUntil('\n');
    strLectura.trim();
    Serial.print(strLectura);

    if (strLectura == "INICIAL") {
      _estado_control = ESTADO_INICIAL;
    } else if (strLectura == "CALIBRACION") {
      _estado_control = CALIBRACION;
    } else if (strLectura == "AUTOMATICO") {
      _estado_control = AUTOMATICO;
    } else if (strLectura == "MANUAL") {
      _estado_control = MANUAL;
    }

  }

  //INTERRUPCION PARA REALIZAR EL CONTROL

  if(has_expired)
   {
      
      modos_interrupcion();

      has_expired = false;
   }

}

