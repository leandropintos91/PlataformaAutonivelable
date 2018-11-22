#include "ultrasonido_hcsr04.h"

ultrasonido_hcsr04::ultrasonido_hcsr04(int triggerConfiguracion, int echoConfiguracion) {
    ultimoMicrosegundo = 0;
    distancia = 0;
    triggerPin = triggerConfiguracion;
    echoPin = echoConfiguracion;
	pinMode(triggerConfiguracion, OUTPUT); //pin como salida
	pinMode(echoConfiguracion, INPUT);  //pin como entrada
}

float ultrasonido_hcsr04::getDistancia()
{
  long tiempo;
	digitalWrite(triggerPin, HIGH);
	if(micros() - ultimoMicrosegundo > 10)
	{
	  digitalWrite(triggerPin, LOW);
	  ultimoMicrosegundo = micros();
	  tiempo = pulseIn(echoPin, HIGH); //obtenemos el ancho del pulso
	  distancia = ((float)tiempo)/59;             //escalamos el tiempo a una distancia en cm
	}
	return distancia;
}
