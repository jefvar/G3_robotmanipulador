#include <WiFi.h>
#include <WiFiAP.h>
#include <WiFiClient.h>
#include <WiFiGeneric.h>
#include <WiFiMulti.h>
#include <WiFiSTA.h>
#include <WiFiScan.h>
#include <WiFiServer.h>
#include <WiFiType.h>
#include <WiFiUdp.h>

/*
  ESP32 CAM Camera with MicroSD storage
  esp32cam-microsd.ino
  Take picture when button pressed
  Store image on MicroSD card
 
  DroneBot Workshop 2022
  https://dronebotworkshop.com
*/
// Include Required Libraries
 
// Camera libraries
#include <Arduino.h>
#include "esp_camera.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "driver/rtc_io.h"
 
// MicroSD Libraries
#include "FS.h"
#include "SD_MMC.h"
 
//EEPROM Library
#include "EEPROM.h"
//Wifi Library
#include "wifi.h"
#include "WiFiClient.h"
#include <vector>

#include "MyServer.h"
// Use 1 byte of EEPROM space
#define EEPROM_SIZE 1
 
// Counter for picture number
unsigned int pictureCount = 0;
 
// Pin definitions for CAMERA_MODEL_AI_THINKER
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22
 
void configESPCamera() {
  // Configure Camera parameters
 
  // Object to store the camera configuration parameters
  camera_config_t config;
 
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG; // Choices are YUV422, GRAYSCALE, RGB565, JPEG 
  // Select lower framesize if the camera doesn't support PSRAM
  if (psramFound()) {
    config.frame_size = FRAMESIZE_UXGA; // FRAMESIZE_ + QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA
    config.jpeg_quality = 10; //10-63 lower number means higher quality
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }
 
  // Initialize the Camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
 
  // Camera quality adjustments
  sensor_t * s = esp_camera_sensor_get();
 
  // BRIGHTNESS (-2 to 2) 2
  s->set_brightness(s, 2);
  // CONTRAST (-2 to 2) 2
  s->set_contrast(s, 2);
  // SATURATION (-2 to 2) 1
  s->set_saturation(s, 1); 
  // SPECIAL EFFECTS (0 - No Effect, 1 - Negative, 2 - Grayscale, 3 - Red Tint, 4 - Green Tint, 5 - Blue Tint, 6 - Sepia) 0
  s->set_special_effect(s, 0);
  // WHITE BALANCE (0 = Disable , 1 = Enable) 1
  s->set_whitebal(s, 1);
  // AWB GAIN (0 = Disable , 1 = Enable) 1
  s->set_awb_gain(s, 1);
  // WB MODES (0 - Auto, 1 - Sunny, 2 - Cloudy, 3 - Office, 4 - Home) 1
  s->set_wb_mode(s, 1);
  // EXPOSURE CONTROLS (0 = Disable , 1 = Enable) 0
  s->set_exposure_ctrl(s, 0);
  // AEC2 (0 = Disable , 1 = Enable) -1
  s->set_aec2(s, 1);
  // AE LEVELS (-2 to 2) -1
  s->set_ae_level(s, -1);
  // AEC VALUES (0 to 1200)
  s->set_aec_value(s, 600);
  // GAIN CONTROLS (0 = Disable , 1 = Enable)
  s->set_gain_ctrl(s, 1);
  // AGC GAIN (0 to 30)
  s->set_agc_gain(s, 15);
  // GAIN CEILING (0 to 6)
  s->set_gainceiling(s, (gainceiling_t)5);
  // BPC (0 = Disable , 1 = Enable)
  s->set_bpc(s, 0);
  // WPC (0 = Disable , 1 = Enable)
  s->set_wpc(s, 0);
  // RAW GMA (0 = Disable , 1 = Enable)
  s->set_raw_gma(s, 0);
  // LENC (0 = Disable , 1 = Enable)
  s->set_lenc(s, 0);
  // HORIZ MIRROR (0 = Disable , 1 = Enable)
  s->set_hmirror(s, 0);
  // VERT FLIP (0 = Disable , 1 = Enable)
  s->set_vflip(s, 0);
  // DCW (0 = Disable , 1 = Enable)
  s->set_dcw(s, 0);
  // COLOR BAR PATTERN (0 = Disable , 1 = Enable)
  s->set_colorbar(s, 0);
 
}
 
void initMicroSDCard() {
  // Start the MicroSD card
 
  Serial.println("Mounting MicroSD Card");
  if (!SD_MMC.begin()) {
    Serial.println("MicroSD Card Mount Failed");
    return;
  }
  uint8_t cardType = SD_MMC.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("No MicroSD Card found");
    return;
  }
 
}
 
void takeNewPhoto(String path) {
  // Take Picture with Camera
 
  // Setup frame buffer
  camera_fb_t  * fb = esp_camera_fb_get();
 
  if (!fb) {
    Serial.println("Camera capture failed");
    return;
  }
 
  // Save picture to microSD card
  fs::FS &fs = SD_MMC;
  File file = fs.open(path.c_str(), FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file in write mode");
  }
  else {
    file.write(fb->buf, fb->len); // payload (image), payload length
    Serial.printf("Saved file to path: %s\n", path.c_str());
  }
  // Close the file
  file.close();
 
  // Return the frame buffer back to the driver for reuse
  esp_camera_fb_return(fb);
}

const camera_fb_t* tomarfoto(){
  Serial.println("Tomando foto...");
  delay(10);
  Serial.println("Imagen capturada");
  static uint8_t data[5]={2,4,6,8,1};
  static camera_fb_t simul;
  simul.width=5;
  simul.height=1;
  simul.buf=data;
  simul.len=5;
  return &simul;
}

int SendImgTo(const char* name, const camera_fb_t* fb){  
  for (int j = 0; j < _conn.size(); j++) {
		if (_conn[j].name == name){ // El nombre concide
			char header[40];
      sprintf(header, "FOTO w=%d h=%d l=%d\n",fb->width,fb->height,fb->len);
      _conn[j].cli.write((uint8_t*) header,strlen(header));
      _conn[j].cli.write(fb->buf,fb->len);
    }
	}
	return -1;
}


void setup() {
  // Disable brownout detector
  //Creacion de SoftAP
  const char* ssid="MIM-GRUPO-X";
  const char* pwd="1234567890";
  IPAddress local_ip(192,168,23,1);
  IPAddress gateway(192,168,23,1);
  IPAddress subnet(255,255,255,0);
  WiFi.softAP(ssid, pwd); //Start Acces point mode
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);
  _server.begin();
  // Start Serial Monitor
  Serial.begin(115200); 
  Serial.print("Started");
  // Initialize the camera
  // Serial.print("Initializing the camera module...");
  // configESPCamera();
  // Serial.println("Camera OK!"); 
  // // Initialize the MicroSD
  // Serial.print("Initializing the MicroSD card module... ");
  // initMicroSDCard(); 
  // // initialize EEPROM with predefined size
  // EEPROM.begin(EEPROM_SIZE); 
  // // Path where new picture will be saved in SD Card
  // String path = "/image.jpg";
  // Serial.printf("Picture file name: %s\n", path.c_str()); 
  // // Take and Save Photo
  // takeNewPhoto(path);   
  delay(10); 
 
}
 
void loop() {
  CheckNewConn();
  int j=CheckReceive(); //lee hasta \n
  if (j>=0) {
    printf("Mensaje recibido de %d (%s)\n",
    j,_conn[j].name.c_str());
    if(strcmp(_conn[j].name.c_str(),"CTRL")==0 && strcmp(_conn[j].recv.c_str(),"IMG")==0){
      const camera_fb_t* foto=tomarfoto();
      SendImgTo("PC", foto);
    }
  }

  // Ejemplo: envía a todos un contador cada 1 segundo
  static uint32_t lastTimeSent=0,counter=0;
  uint32_t t=millis();
  if (t-lastTimeSent> 1000) {
    counter++;
    lastTimeSent=t;
    char msg[32];
    sprintf(msg,"Counter=%ld\n",counter);
    SendToAllStartingWith("",msg);
  }
}