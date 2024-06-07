#include "Control.h"

//Variables globales
extern float _ref_motores[3];
char receivedChars[32]; // Buffer para almacenar los caracteres recibidos del puerto serial
boolean newData; // Bandera para indicar que se han recibido nuevos datos
extern float _ref_motores[3];
int numValores=3;

//FUNCIONES PARA TRATAMIENDO DE TABLAS
void InitTabla(float t[], int n)
{
    int i;

    for(i=0;i<n-1;i++)//si la tabla es de 7 hay que hacerlo 6 veces para que el ultimo valor no se vaya a salir
    {
        t[i]=0;
    }

}
void DesplazarTabla(float t[], int n)//no ponemos const porque necesitamos modificar la tabla
{
    int i;

    for(i=n-2;i>=0;i--) //lo hacemos de forma inversa para no tener el problema del orden en la tabla
    {                   //es n-2 para no salirnos del rango ya que n-2=5 +1=6 t[6] es el septimo elemento
        t[i+1]=t[i];
    }

}

void recvWithEndMarker() {
  static byte ndx = 0; // Índice del buffer
  char endMarker = '\n'; // Caracter de fin de mensaje
  char rc; // Variable para almacenar el caracter recibido

  while (Serial.available() > 0 && newData == false) {
    rc = Serial.read(); // Lee el próximo caracter
    if (rc != endMarker) { // Si el caracter no es el fin de mensaje
      receivedChars[ndx] = rc; // Almacena el caracter en el buffer
      ndx++; // Incrementa el índice
      if (ndx >= 32) { // Si se excede el tamaño del buffer
        ndx = 31; // Limita el índice al tamaño máximo del buffer
      }
    } else { // Si se ha recibido el fin de mensaje
      receivedChars[ndx] = '\0'; // Añade el caracter nulo al final del buffer
      ndx = 0; // Reinicia el índice
      newData = true; // Activa la bandera de nuevos datos recibidos
    }
  }
}

void parseData() {
  char * strtokIndx; // Puntero para almacenar el índice del token
  float _ref_motores_local[3];
  strtokIndx = strtok(receivedChars, ","); // Divide la cadena en tokens separados por coma
  for (int i = 0; i < numValores; i++) {
    _ref_motores_local[i] = atof(strtokIndx); // Convierte el token a float y lo guarda en el array
    ; // Convierte el token a float y lo guarda en el array
    strtokIndx = strtok(NULL, ","); // Obtén el siguiente token
  }//LIMITACION A LA ENTRADA DE ANGULOS POR SERIAL 
    //   if(_ref_motores_local[0]>140){
    //   _ref_motores_local[0]=140;}
    // else if(_ref_motores_local[0]<40){
    //   _ref_motores_local[0]=40;}
    //   if(_ref_motores_local[1]>140){
    //   _ref_motores_local[1]=140;}
    // else if(_ref_motores_local[1]<50){
    //   _ref_motores_local[1]=50;}
    //   if(_ref_motores_local[2]>230){
    //   _ref_motores_local[2]=230;}
    // else if(_ref_motores_local[2]<150){
    //   _ref_motores_local[2]=150;}

      for (int i = 0; i < numValores; i++) {
    _ref_motores[i] =_ref_motores_local[i];}

  // Imprimir los valores en el monitor serial
  Serial.print("Valores leídos: ");
  for (int i = 0; i < numValores; i++) {
    Serial.print(_ref_motores[i]);
    Serial.print("\t");
  }
  Serial.println(); // Salto de línea
}

//Funcion que lee encoder
float LecturaEncoder(int n_motor){
  float ang=0;
  int pos_m=_encoder1->Read(n_motor); // lee encoder 0 (M1)
  if (pos_m!=0x80000000) { 
        ang=pos_m*360.0/4096.0;
        //Serial.printf("La posicion del motor %d es: %f \n", n_motor,ang);
  }
  return ang;
}

// Función para la parte integral con anti-wind-up
float integral(float error, float *integral_sum, float K_i, float Ts) {
    if(error<=1 && error >=-1)
    {
      *integral_sum=0;
    }
    else{
      // Integración del error
    *integral_sum += error * Ts; // Ts es el tiempo de muestreo
    }
    // Anti-wind-up: Limitar la parte integral para evitar el "wind-up"
    if (*integral_sum >40) {
        *integral_sum = 40; // Limitar la suma integral superiormente
    } else if (*integral_sum < -40) {
        *integral_sum = -40; // Limitar la suma integral inferiormente
    }

    // Cálculo de la salida de la parte integral
    float output = K_i * (*integral_sum);
    return output;
}
void ControlPID_POS(float error[2],float uk[],float kp,float kd,float u_integral,float Ts,int n_motor,float saturacion){
    float duty=0;
    float u_prop=0,u_der=0;

    u_prop=kp*error[0];
    u_der=kd*(error[0]-error[1])/Ts;

    if(error[0]<=2 && error[0]>=-2){    // Base y antebrazo error en eje conductor es 1º
      u_prop=0;
      u_der=0;
      u_integral=0;
    }    
    /*if (u_prop<=0.55 && u_prop>0)
    {
      u_prop=0.55;
    }
    else if(u_prop>=-0.55 && u_prop<0){
      u_prop=-0.55;
    }*/
    duty=u_prop+u_integral+u_der;
    /*duty=u_prop+u_der;//para la opcion de nacho  
    uk[0]=uk[1]+duty;*/
    if(duty>=saturacion)
    //if(uk[0]>=saturacion)
    {
      duty=saturacion;
      //uk[0]=saturacion;
    }
    else if(duty<=-saturacion)
    //else if(uk[0]<=-saturacion)
    {
      duty=-saturacion;
      //uk[0]=-saturacion;
    }
    //Serial.printf(" Duty motor %d: %f || e[0]=%f,e[1]=%f \n",n_motor,duty,error[0],error[1]);
    //Se aplica el control segun el numero del motor 
    
    _motors[n_motor]->SetDuty(duty);
   // _motors[n_motor]->SetDuty(uk[0]);//con esta opcion se hace como nacho en control de velocidad
}


//Funcion que realiza control
//void ControlPID_POS(float error[2],float uk[2],float kp,float kd,float *integral_sum,float Ts,int n_motor)//descomentar lo de abajo para la parte integral
void ControlPD_POS(float error[2],float uk[2],float kp,float kd,float Ts,int n_motor,float saturacion)
{ 
    float duty=0;
    float u_prop=0,u_der=0;

    u_prop=kp*error[0];
    u_der=kd*(error[0]-error[1])/Ts;

    if(error[0]<=1 && error[0]>=-1){    // Brazo error en eje conductor es 1º
      u_prop=0;
      u_der=0;
    }  

    duty=u_prop+u_der;       
    if(duty>=saturacion)
    {
      duty=saturacion;
    }
    else if(duty<=-saturacion)
    {
      duty=-saturacion;
    }
    //Serial.printf(" Duty motor %d: %f || e[0]=%f,e[1]=%f \n",n_motor,duty,error[0],error[1]);
    //Se aplica el control segun el numero del motor 
    _motors[n_motor]->SetDuty(duty);
}
