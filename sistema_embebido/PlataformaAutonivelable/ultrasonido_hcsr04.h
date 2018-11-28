#include "Arduino.h"

class ultrasonido_hcsr04 {
    public:
      ultrasonido_hcsr04(int,int);
    	void trigger();
    	float getDistancia();

    private:
    	unsigned long ultimoMicrosegundo;
    	float distancia;
      short triggerPin;
      short echoPin;
};
