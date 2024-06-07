#include "Buzzer.h"

bool value_buzz = true;
int frequency_buzz = 20; // 20 to 20000
hw_timer_t *timer4 = NULL;

void IRAM_ATTR onTimer() {
  value_buzz = !value_buzz;
  digitalWrite(BUZZER_PIN, value_buzz); 
}

void setupTimerBuzz() {
    // Use 1st timer of 4  - 1 tick take 1/(80MHZ/80) = 1us so we set divider 80 and count up 
    pinMode(BUZZER_PIN,OUTPUT);
    timer4 = timerBegin(2, 80, true);//div 80
    timerAttachInterrupt(timer4, &onTimer, true);
}

void setFrequency(long frequencyHz){
    timerAlarmDisable(timer4);
    timerAlarmWrite(timer4, 1000000l / frequencyHz, true);
    timerAlarmEnable(timer4);
}

void tone_buzz(long frequencyHz, long durationMs){
    setFrequency(frequencyHz);
    delay(durationMs);
}