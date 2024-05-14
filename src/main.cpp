#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "../lib/esp_random.h"
#include "Control.cpp"  //encontramos lo relacionado con las funciones para el control y los define

//Variables globales
float _ref_base=100,_ref_ante=190;
String inputString = "";
float _ek_pos_base[2]; //error de posicion estado actual y anterior
float _uk_pos_m1[2]; //accion de control actual y anterior
float _ek_pos_brazo[2]; 
float _uk_pos_m2[2]; 
float _ek_pos_m3[2]; 
float _uk_pos_m3[2];
float _integral_motores[3]={0.0,0.0,0.0};

hw_timer_t *timer = NULL; //Puntero de variable para configurar timer 
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(1, PIN_RGB, NEO_GRB + NEO_KHZ800);

 uint16_t aleatorio;      // Se creó esta variable para cambiar el color del RGB de forma aleatoria. No es necesario para el funcionamiento del robot.

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
  InitTabla(_uk_pos_m1,2);
  InitTabla(_ek_pos_brazo,2);
  InitTabla(_uk_pos_m2,2);
  InitTabla(_ek_pos_m3,2);
  InitTabla(_uk_pos_m3,2);
  // LED RGB
  pixels.begin();
  pixels.setBrightness(10);
  pixels.show(); // Initialize all pixels to 'off'
  pixels.fill(0x33FFFF);
  pixels.show();
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
}

void loop() {


  if(Serial.available())
  {
      char incomingChar = Serial.read();
      // Si el carácter recibido no es un salto de línea
      if (incomingChar != '\n') {
        // Agrega el carácter a la cadena de entrada
        inputString += incomingChar;
      } else {
        // Si se recibe un salto de línea, convierte la cadena en un flotante
        _ref_ante = inputString.toFloat();
        
        // Reinicia la cadena de entrada
        inputString = "";
    }
  }
     if(has_expired)
   {
      float integral_suma=0.0;
      DesplazarTabla(_ek_pos_base,2);
      DesplazarTabla(_uk_pos_m1,2);
      DesplazarTabla(_ek_pos_brazo,2);
      DesplazarTabla(_uk_pos_m2,2);
      DesplazarTabla(_ek_pos_m3,2);
      DesplazarTabla(_uk_pos_m3,2);
      LecturaEncoder(MOTOR_ANTEBRAZO);

      /*_ek_pos_base[0]=_ref_ante-LecturaEncoder(MOTOR_BASE);
      _ek_pos_brazo[0]=_ref_ante-LecturaEncoder(MOTOR_BRAZO);  
      Serial.printf("_ek_pos_base: %f, _ek_pos_m2: %f \n",_ek_pos_base[0],_ek_pos_brazo[0]);

      integral_suma=integral(_ek_pos_base[0],&_integral_motores[0],ki,TS);*/

      aleatorio = esp_random();
      //Serial.println(aleatorio);
      pixels.fill(aleatorio);
      pixels.show();
      //ControlPID_POS(_ek_pos_m1,_uk_pos_m1,KP_M_BASE,Kd,ki,MOTOR_BASE);
      //ControlPID_POS(_ek_pos_m2,_uk_pos_m2,KP_M_BASE,Kd,ki,MOTOR_BRAZO);
      has_expired = false; 
   }

}

