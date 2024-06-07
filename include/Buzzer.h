#ifndef CONTROL_H
#define CONTROL_H

#include <Arduino.h>

#define BUZZER_PIN 1
extern bool value_buzz;
extern int frequency_buzz;
extern hw_timer_t *timer4;

void setupTimerBuzz();
void IRAM_ATTR onTimer();
void setFrequency(long frequencyHz);
void tone_buzzne(long frequencyHz, long durationMs);

#endif