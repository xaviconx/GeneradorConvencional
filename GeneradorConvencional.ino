//Código desarrollado para Arduino UNO

#include<Wire.h>

//Pines analógicos
#define CargaInicial A0
#define Setpoint A2

//Pines digitales
#define BotonInicio 7

//Variables globales
int PotenciaNominal = 1000;          //Potencia nominal que se genrará
int MaxGanancia = 200;            //Porcentaje máximo que alcanzará el generador
float DeltaTiempo = 2;            //Cantidad de tiempo necesaria para la transición en minutos
int TiempoDeActualizacion = 200;  //Intervalo para actualizar la salida en mili segundos.
int Tolerancia = 20;

unsigned long CurrentMillis = 0,PreviousMillis = 0;

void GeneradorConvencional(int, int, float, float*);
float Ruido(float, float); //Función que devuelve un valor con ruido. Los agrumentos son la referencia y la distancia que puede haber al ruido.
void Plotter();

void setup() {
  //Declaramos las entradas digitales
  pinMode(BotonInicio, INPUT_PULLUP);
  //No es necesario declarar entradas analógicas

  //Inicializamos la comunicación serial
  Serial.begin(9600);

  //Tiempo para que se estabilice el sistema
  delay(500);
  Serial.println("Listo :)");
}

void loop() {
  Serial.println("Listo :)");
  CurrentMillis=millis();
  if (CurrentMillis>PreviousMillis+TiempoDeActualizacion){  //Si ha transcurrido el tiempo de actualización
    PreviousMillis=millis();
  }

}

void GeneradorConvencional(int PotNominal, int TiempoTotal, int TiempoCiclo, float SetPoint, float* Potencia){
  float deltaPotencia = (PotNominal-SetPoint)/TiempoTotal*60000;
  float deltaCiclo = deltaPotencia*(TiempoTotal/TiempoCiclo);
  if ((SetPoint - Potencia)>Tolerancia){
    Potencia=Potencia+deltaCiclo; 
  }
}

float Ruido(float Referencia, float Rango){ 
  return random(Referencia-Rango,Referencia+Rango);
}

void Plotter(){
  Serial.print(0);
  Serial.print(",");
  Serial.print(PotNominal*MaxGanancia/100);
  Serial.print(",");
  Serial.print(Potencia);
}
