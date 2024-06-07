#include <WiFi.h>
#include <ESPNowW.h>

// ESP32S3 MAC Address:  68:B6:B3:3C:E4:78
// ESP32 CAM MAC Address:  94:B5:55:FC:38:1C

uint8_t receiver_mac[] = {0x94 , 0xB5 , 0x55 , 0xFC , 0x38 , 0x1C};
int contador_cam=0;

#define BUZZER_PIN 1