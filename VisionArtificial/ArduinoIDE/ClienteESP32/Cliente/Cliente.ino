#include <WiFi.h>

#include <String.h>
#include <WiFiClient.h>
WiFiClient cli;
int flag=0;
char dat;
void name(){
  if (cli.available() ) {      
      String recv=cli.readStringUntil('\n');
      Serial.print("RECV: ");
      Serial.println(recv);
      if(strcmp(recv.c_str(), "<<<NAME>>>") == 0){
        String texto="<<<NAME=CTRL>>>\n";
        cli.write(texto.c_str(),texto.length());
        flag=1;
      }
    }
}

void setup() {
  const char* ssid="MIM-GRUPO-X";
  const char* pwd="1234567890";
  Serial.begin(115200);
  WiFi.mode(WIFI_STA); //Modo Station
  WiFi.begin(ssid, pwd);
  while (WiFi.status() != WL_CONNECTED) { 
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Conectado");
  Serial.print("My IP: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // put your main code here, to run repeatedly:
  if (! cli.connected( )) {
    cli.connect("192.168.23.1",55355);
    flag=0;
  }
  if (cli.connected( ) && flag==0){
    name();
  }
  if(Serial.available()>0){
    dat=Serial.read();
    if(dat=='7'){
      String texto="IMG\n";
      cli.write(texto.c_str(),texto.length());
    }
  }
  

}
