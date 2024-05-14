#include "Control.h"

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
//Funcion que lee encoder
float LecturaEncoder(int n_motor){
  float ang=0;
  int pos_m=_encoder1->Read(n_motor); // lee encoder 0 (M1)
  if (pos_m!=0x80000000) { 
        ang=pos_m*360.0/4096.0;
        Serial.printf("La posicion del motor %d es: %f \n", n_motor,ang);
  }
  return ang;
}

// Función para la parte integral con anti-wind-up
float integral(float error, float *integral_sum, float K_i, float Ts) {
    // Integración del error
    *integral_sum += error * Ts; // Ts es el tiempo de muestreo

    /*// Anti-wind-up: Limitar la parte integral para evitar el "wind-up"
    if (*integral_sum > limit) {
        *integral_sum = limit; // Limitar la suma integral superiormente
    } else if (*integral_sum < -limit) {
        *integral_sum = -limit; // Limitar la suma integral inferiormente
    }*/

    // Cálculo de la salida de la parte integral
    float output = K_i * (*integral_sum);
    return output;
}

//Funcion que realiza control
void ControlPID_POS(float error[2],float uk[2],float kp,float kd,float *integral_sum,float Ts,int n_motor)
{ 
    float duty=0;
    float u_prop=0,u_der=0;
    float u_integral=0.0;

    u_prop=kp*error[0];
    u_der=kd*(error[0]-error[1])/0.05;
    *integral_sum+=error[0]*Ts; //sumatoria de la parte integral
    u_integral=error[0]+ki*error[0];

    duty=u_prop+u_der+u_integral;
    if(duty>=0.8)
    {
      duty=0.8;
    }
    else if(duty<=-0.8)
    {
      duty=-0.8;
    }
    Serial.printf(" Duty motor %d: %f || e[0]=%f,e[1]=%f \n",n_motor,duty,error[0],error[1]);
    //Se aplica el control segun el numero del motor 
    _motors[n_motor]->SetDuty(duty);
}
