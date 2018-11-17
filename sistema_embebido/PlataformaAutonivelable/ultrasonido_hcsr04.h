#include "Arduino.h"

#define TRIGGER_PIN 2  // Pin 14 of L293
#define ECHO_PIN 3  // Pin 10 of L293

class ultrasonido_hcsr04 {
    public:
      ultrasonido_hcsr04(int,int);
    	void trigger();
    	short getDistancia();

    private:
    	unsigned long ultimoMicrosegundo;
    	short distancia;
      short triggerPin;
      short echoPin;
};