#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "../lib/esp_random.h"

#define PIN_RGB 38

hw_timer_t *timer = NULL;

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(1, PIN_RGB, NEO_GRB + NEO_KHZ800);

volatile bool has_expired = false;
void IRAM_ATTR timerInterrupcion() {
 has_expired = true;
}

uint16_t aleatorio;

void setup() {
  Serial.begin(115200);
  // LED RGB
  pixels.begin();
  pixels.setBrightness(10);
  pixels.show(); // Initialize all pixels to 'off'
  pixels.fill(0x33FFFF);
  pixels.show();
  // CONFIG. HERE
  timer = timerBegin(0, 80, true); // Timer 0, clock divider 80
  timerAttachInterrupt(timer, &timerInterrupcion, true); // Attach the interrupt handling function
  timerAlarmWrite(timer, 1000000, true); // Interrupt every 1 second
  timerAlarmEnable(timer); // Enable the alarm
}

void loop() {
  if(has_expired)
  {
    aleatorio = esp_random();
    Serial.println(aleatorio);
    pixels.fill(aleatorio);
    pixels.show();
    has_expired = false; 
  }
}

