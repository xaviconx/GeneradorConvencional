//To do: Complementar comunicación i2c

#include <Arduino.h>

//Código desarrollado para Arduino UNO

#include<Wire.h>

//Pines analógicos
#define CargaInicial A0
#define Setpoint A2

//Pines digitales
#define BotonInicio 7

//Variables globales
float PotenciaNominal = 1000;       //Potencia nominal que se genrará.
float PotenciaActual = 500;         //Condición inicial y variable que almacena la potencia deseada.
float PotenciaConRuido = PotenciaActual;  //Variable que representa la potencia real.
float PotenciaDeseada=0;            //Variable que almacena el setpoint de la potencia.

float MaxGanancia = 200;            //Porcentaje máximo que alcanzará el generador.
float DeltaTiempo = 0.5;            //Cantidad de tiempo necesaria para la transición en minutos.
int TiempoDeActualizacion = 200;    //Intervalo para actualizar la salida en milisegundos.
float deltaPotencia = 0;            //Variable que almacena el aumento de potencia por ciclo.
float Tolerancia = 20;              //Tolerancia para la potencia deseada.

int Ganancia = 0;                   //Variable que almacena el multiplicador de la potencia.

unsigned long CurrentMillis = 0,PreviousMillis = 0; //Variables para el manejo del tiempo
//i2c
byte Central = 0;                   //Dirección i2c del maestro
byte AddressGenerador = 4;          //Dirección de este dispositivo

void GeneradorConvencional(float *PAct, float Pdes, float tol, float *deltaP);
float Ruido(float, float); //Función que devuelve un valor con ruido. Los agrumentos son la referencia y la distancia que puede haber al ruido.
void Plotter();             //Función que imprime los valores, pensada para usarse con el serial plotter de arduino
void Boton();               //Función para la lectura del botón. Incluye anti rebote.
void i2cRequest();          //Rutina de interrupción i2c

void setup() {
  //Declaramos las entradas digitales
  pinMode(BotonInicio, INPUT_PULLUP);   //El botón de inicio se habilita con resistencia de pullup
  //No es necesario declarar entradas analógicas

  //Inicializamos la comunicación serial
  Serial.begin(9600);
  //Inicializamos la comuniación i2c
  Wire.begin(AddressGenerador);
  Wire.onRequest(i2cRequest);

  //Tiempo para que se estabilice el sistema
  delay(500);
  Serial.println("Listo :)");
}

void loop() {
  if (digitalRead(BotonInicio)==LOW){
    Boton();  
  }
  //Serial.println("Listo :)");
  CurrentMillis=millis();
  if (CurrentMillis>PreviousMillis+TiempoDeActualizacion){  //Si ha transcurrido el tiempo de actualización
    PreviousMillis=millis();
    GeneradorConvencional(&PotenciaActual, PotenciaDeseada, Tolerancia, &deltaPotencia);
  }
  PotenciaConRuido=Ruido(PotenciaActual,10);
  Plotter();
  delay(100);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Boton(){
  while(digitalRead(BotonInicio)==LOW){   //Antirebote
    Ganancia=map(analogRead(Setpoint),0,1023,0,MaxGanancia);
    //Serial.println(Ganancia);
    delay(100);
  }
  float intervalos = (1000/TiempoDeActualizacion)*DeltaTiempo*60; //Se determina la cantidad de intervalos antes de alcanzar la potencia deseada
  PotenciaDeseada=PotenciaNominal*Ganancia/100.;    //Se determina la potencia deseada
  deltaPotencia=(PotenciaDeseada-PotenciaActual);   //Se calcula el incremento por segundo
  deltaPotencia=deltaPotencia/intervalos;           

/*   Serial.print(" , ");
  Serial.print(intervalos);
  Serial.print(" , ");
  Serial.print(deltaPotencia,6); */
}

void GeneradorConvencional(float *PAct, float Pdes, float tol, float *deltaP){
  //Serial.println(deltaCiclo);
  if (abs(Pdes - *PAct) > tol){
    *PAct=*PAct+*deltaP; 
  }else{
    *deltaP=0;
  }
}

float Ruido(float Referencia, float Rango){ 
  return random(Referencia-Rango,Referencia+Rango);
}

void Plotter(){
  Serial.print(0);
  Serial.print(" , ");
  Serial.print(PotenciaNominal*(MaxGanancia/100));
  Serial.print(" , ");
  Serial.print(PotenciaActual);
  Serial.print(" , ");
  Serial.println(PotenciaConRuido);
}

void i2cRequest(){
  Wire.beginTransmission(Central);
  //Wire.write(PotenciaConRuido);
  Wire.endTransmission();
}