#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "../lib/esp_random.h"
#include <SPI.h>
#include "../lib/ALMar_ESP32_EncoderATM203_Spi2.cpp"
#include "../lib/AlMar_ESP32_Driver_L298n.cpp"

/*************************** CONSTANTES ***************************/
#define PIN_RGB 38
#define N_MOTORS 1
#define PIN_CS_M1  10 // (chip select para encoder de motor 1) 10 for SPI3
#define PIN_CS_M2  40  // (chip select para encoder de motor 2) 
#define PIN_CS_M3  41  // (chip select para encoder de motor 3) 
// Pines comunes a todos los encóders del bus 
#define PIN_SCLK  12 // 36 // 12 for SPI3
#define PIN_MISO  13 // 37 // 13 for SPI3
#define PIN_MOSI  11 // 35 // 11 for SPI3
//Pines para el pwm de los motores 
#define PIN_M1_EN   5
#define PIN_M1_IN1  6
#define PIN_M1_IN2   7
// Frecuencia de la PWM
#define PWM_FREQ_HZ    200

// Objetos de Clases
using namespace AlMar;//Si no se pone esto tendria que poner AlMAr::Esp32::.... Cada vez 
using namespace Esp32;

hw_timer_t *timer = NULL;
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(1, PIN_RGB, NEO_GRB + NEO_KHZ800);
EncoderATM203_Spi2* _encoder1;  // Puntero para usar asignación dinámica en la creación
Driver_L298n* _m1; //Creacion del puntero al objeto // Se puede usar un array en su lugar: *_motors[3];  


/* Función de prueba con el Timer 0. ISR de la interrupción
volatile bool has_expired = false;
void IRAM_ATTR timerInterrupcion() {
 has_expired = true;
}
*/

// uint16_t aleatorio;      // Se creó esta variable para cambiar el color del RGB de forma aleatoria. No es necesario para el funcionamiento del robot.


/************************* PROGRAMA PRINCIPAL *************************/
void setup() {
  Serial.begin(115200);
  // LED RGB
  pixels.begin();
  pixels.setBrightness(10);
  pixels.show(); // Initialize all pixels to 'off'
  pixels.fill(0x33FFFF);
  pixels.show();
  // CONFIG. HERE
  /* El código a continuación se utilizó para realizar una prueba con el led RGB con un timer (Timer 0). No es necesario para el funcionamiento del robot*/
  //timer = timerBegin(0, 80, true); // Timer 0, clock divider 80
  //timerAttachInterrupt(timer, &timerInterrupcion, true); // Attach the interrupt handling function
  //timerAlarmWrite(timer, 1000000, true); // Interrupt every 1 second
  //timerAlarmEnable(timer); // Enable the alarm

  /* Motor 1 */
  int cs_pins[]={PIN_CS_M1,PIN_CS_M2,PIN_CS_M3};
  _encoder1=new EncoderATM203_Spi2(cs_pins,N_MOTORS,PIN_MOSI,PIN_MISO,PIN_SCLK);
  _m1=new Driver_L298n(PIN_M1_EN,PIN_M1_IN1,PIN_M1_IN2,PWM_FREQ_HZ );
  _m1->begin();       // Usar _motors[0]->begin() si se ha declarado como array
}

void loop() {
  /* Código de Pruebas. No es necesario para el funcionamiento del robot*/
  // if(has_expired)
  // {
  //   aleatorio = esp_random();
  //   Serial.println(aleatorio);
  //   pixels.fill(aleatorio);
  //   pixels.show();
  //   has_expired = false; 
  // }

  int pos=_encoder1->Read(0); // lee encoder 0 (M1)
  if (pos!=0x80000000) { 
        Serial.print((float) (pos*360.0/4096.0));
        Serial.println();
  }

  /*float duty_m1=0.7; // Valor entre -1.0 y 1.0
  _m1->SetDuty(duty_m1);  // Usar _motors[0]->SetDuty(duty_m1); si se ha declarado como array
  Serial.println("Giro izq");
  delay(5000);
  
  _m1->SetDuty(0);
  Serial.println("Giro parado");
  delay(5000);

  _m1->SetDuty(-duty_m1);
  delay(5000);
  Serial.println("Giro der");*/
}
