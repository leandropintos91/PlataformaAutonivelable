#include "botonControl.h"

uint8_t estadoPulsadores(){
	uint8_t pulsador1, pulsador2, pulsador3;

	estado1 = digitalRead(pulsador1);  
    estado2 = digitalRead(pulsador2);  
    estado3 = digitalRead(pulsador3);  

    return estado1 * estado2 * estado3;
}