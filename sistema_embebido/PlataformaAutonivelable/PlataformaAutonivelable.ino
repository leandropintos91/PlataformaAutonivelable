#include "acelerometro.h"

Acelerometro a1;
double acc[3];
 
void setup(){
  Serial.begin(9600);
  Serial.println("ok");
   Wire.begin();
  //a1 = new Acelerometro();

  Serial.println(a1.getMpu().testConnection() ? F("IMU iniciado correctamente") : F("Error al iniciar IMU"));

}

void loop(){
a1.getXYZ(acc);
Serial.print(acc[0]); Serial.print("1\t"); 
Serial.print(acc[1]); Serial.print("2\t");
Serial.print(acc[2]); Serial.print("3\t");

delay(500);
}
