#include <SPI.h>
#include "../../lib/ALMar_ESP32_EncoderATM203_Spi2.h"
#include "../lib/ALMar_ESP32_EncoderATM203_Spi2.h"

using namespace AlMar;
using namespace Esp32;

#define N_MOTORS 1
#define PIN_CS_M1  10 // (chip select para encoder de motor 1) 10 for SPI3
#define PIN_CS_M2  40  // (chip select para encoder de motor 2) 
#define PIN_CS_M3  41  // (chip select para encoder de motor 3) 
// Pines comunes a todos los encóders:
#define PIN_SCLK  12 // 36 // 12 for SPI3
#define PIN_MISO  13 // 37 // 13 for SPI3
#define PIN_MOSI  11 // 35 // 11 for SPI3


AlMar::Esp32::EncoderATM203_Spi2* _encoder1;  // Puntero para usar asignación dinámica en la creación

void setup() {
  int cs_pins[]={PIN_CS_M1,PIN_CS_M2,PIN_CS_M3};
  _encoder1=new EncoderATM203_Spi2(cs_pins,N_MOTORS,PIN_MOSI,PIN_MISO,PIN_SCLK);

}
void loop() {
  int pos=_encoder1->Read(0); // lee encoder 0 (M1)
  if (pos!=0x80000000) { 
        Serial.println(pos);
  }
}
