#include "botonControl.h"

uint8_t estadoPulsadores(){
	//uint8_t pulsador1, pulsador2, pulsador3;
  
	uint8_t estado1 = digitalRead(pulsador1);  
  uint8_t estado2 = digitalRead(pulsador2);  
  uint8_t estado3 = digitalRead(pulsador3);  

  return estado1 * estado2 * estado3;
}
