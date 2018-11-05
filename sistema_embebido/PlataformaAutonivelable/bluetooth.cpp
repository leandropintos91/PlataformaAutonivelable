#include "bluetooth.h"

bool mensajeAcelerometro = false;
bool mensajeLuz = false;
bool mensajeProximidad = false;
String cadena;
char c;


void leerMensaje(){
	 if (Serial.available())
    { 
		delay(20);
        c = Serial.read();
		procesarMensaje(c);
    }
}


void procesarMensaje(char c)
{
	
	switch(c) {
			case '#' : 	if(!mensajeAcelerometro){
							cadena = "";
							mensajeAcelerometro = true;
						} else {
							if(cadena=="ZZ")
							{
								//Serial.write(obtenerAltura());
								Serial.write("_50_\r\n");
							} else {
								//setearAltura(cadena);
							}
							mensajeAcelerometro = false;
							cadena = "";
						}
						
						break;      
			case '&' : 	if(!mensajeProximidad){
							cadena = "";
							mensajeProximidad = true;
						} else {
							if(cadena=="ZZ")
							{
								//Serial.write(obtenerEstadoPlataforma());
								
								Serial.write("/Altura maxima alcanzada/\r\n");
							} else if(cadena="1") {
								//bajarPlataforma();
							} else {
								//subirPlataforma();
							}
							mensajeProximidad = false;
							cadena = "";
						}
						break;
			case '@': 	if(!mensajeLuz){
							cadena = "";
							mensajeLuz = true;
						} else {
							if(cadena=="ZZ")
							{
								//Serial.write(obtenerEstadoLed());
								Serial.write("-Encendido-\r\n");
							} else if(cadena="1") {
								//encenderLed();
							} else {
								//apagarLed();
							}
							mensajeLuz = false;
							cadena = "";
						}
						break;
			default:  	cadena+= c;
						break;
		}
}
