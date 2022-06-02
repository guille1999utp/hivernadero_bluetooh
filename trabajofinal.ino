#include <SoftwareSerial.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
Adafruit_MPU6050 mpu; //llama el objeto la libreria

//la libreria wire.h tiene los pines asignados para el arduno uno
int PWMPIN = 3; int sync = 8;int Pi = 0;
int Fs=10; int corte=0;
//Variables
float referencia = 35; float ek = 0;
float ek_anterior = 0; float valor;
//Pi constantes
float kp = 30;   float ki = 0.01;   int kd = 0.0;
int Gp = 0;    int Gi = 0;    int Gd = 0;
String frase;

//Variables secundarias

SoftwareSerial BT(10,11);

void setup() 
{
  Serial.begin(9600);
  BT.begin(38400);
  if (!mpu.begin())
  {
    while (1) 
    {
      delay(10);
    }
  }
pinMode(PWMPIN,OUTPUT);
pinMode (sync, INPUT); 

  
// Serial.println("temp,temp1"); //lectura del serial ploter para graficar
mpu.setFilterBandwidth(MPU6050_BAND_21_HZ); //inicializacion del mpu 
} 
  
void loop() 
{
 sensors_event_t a, g, temp;
 mpu.getEvent(&a, &g, &temp); //funcion de libreria del mpu pa definir valiavble q se van a usar para escribir la temperaatura
 valor = temp.temperature;
 
//bluetooh

while(BT.available())    // Si llega un dato por el puerto BT se envía al monitor serial
  {
    if(BT.available()>0){
      char letra = BT.read(); 
      Serial.write(letra);
      if((String) letra != "R" && (String)letra != "K" && (String)letra != "C"){
      frase += letra;
      }
      if ((String) letra == "R"){
      String gp = frase;
      kp=gp.toFloat();
      frase = "";
      }
      else if ((String)letra == "K"){
      String gi = frase;
      ki=gi.toFloat();
      frase = "";
      }
      else if((String)letra == "C"){
      String sp = frase;
      referencia=sp.toFloat();
      frase="";
      }

        Serial.print(" kp: ");Serial.print (kp); Serial.print(" ki: ");Serial.print (ki);Serial.print(" sp1: "); Serial.print (referencia); 
      }
   }

//Funciones del sistema

  ek = referencia - valor; //SET POINT MENOS EL VALOR DEL PROCESO
  Gp = kp * ek; //gp significa salida debido a la acción proporcional.
  Gi = (ki/2*Fs)*(ek+ek_anterior); 
  Pi = Gp + Gi + Gd;
  if(Pi < 0){Pi = 20;}
  if(Pi > 255){Pi = 255;} //la salida del pwm de 0 a 255 de 8 bits
  
corte=digitalRead(sync); //Corte almacena la señal de entrada cuando esta en 1 esta en un punto de partida de la onda 
if(corte==HIGH)
{
analogWrite(PWMPIN,Pi);
}
ek_anterior = ek; 

//impresion de variables
Serial.print("TEMP:");
Serial.println(valor);
Serial.print(",");
Serial.print("REF");
Serial.println(referencia);
Serial.print(",");
Serial.print("pwm");
Serial.println(Pi);
delay(60); 


}
