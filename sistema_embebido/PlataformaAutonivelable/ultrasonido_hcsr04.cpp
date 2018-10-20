#include "ultrasonido_hcsr04.h"

ultrasonido_hcsr04::ultrasonido_hcsr04() {
    ultimoMicrosegundo = 0;
    distancia = 0;
	pinMode(TRIGGER_PIN, OUTPUT); //pin como salida
	pinMode(ECHO_PIN, INPUT);  //pin como entrada
}

short ultrasonido_hcsr04::getDistancia()
{
  long tiempo;
	digitalWrite(TRIGGER_PIN, HIGH);
	if(micros() - ultimoMicrosegundo > 10)
	{
	  digitalWrite(TRIGGER_PIN, LOW);
	  ultimoMicrosegundo = micros();
	  tiempo = pulseIn(ECHO_PIN, HIGH); //obtenemos el ancho del pulso
	  distancia = tiempo/59;             //escalamos el tiempo a una distancia en cm
	}
	return distancia;
}
