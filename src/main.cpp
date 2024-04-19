#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "../lib/esp_random.h"
#include <SPI.h>
#include "../lib/ALMar_ESP32_EncoderATM203_Spi2.cpp"
#include "../lib/AlMar_ESP32_Driver_L298n.cpp"

using namespace AlMar;//Si no se pone esto tendria que poner AlMAr::Esp32::.... Cada vez 
using namespace Esp32;


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

EncoderATM203_Spi2* _encoder1;  // Puntero para usar asignación dinámica en la creación
Driver_L298n* _m1; //Creacion del puntero al objeto // Se puede usar un array en su lugar: *_motors[3];  

void setup() {
  Serial.begin(115200);
  int cs_pins[]={PIN_CS_M1,PIN_CS_M2,PIN_CS_M3};
  _encoder1=new EncoderATM203_Spi2(cs_pins,N_MOTORS,PIN_MOSI,PIN_MISO,PIN_SCLK);
  _m1=new Driver_L298n(PIN_M1_EN,PIN_M1_IN1,PIN_M1_IN2,PWM_FREQ_HZ );
  _m1->begin();       // Usar _motors[0]->begin() si se ha declarado como array
}
void loop() {
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
