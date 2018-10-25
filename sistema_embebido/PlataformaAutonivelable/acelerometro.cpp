#include "acelerometro.h"

#include "Wire.h"

using namespace std; 

Acelerometro::Acelerometro(){
	mpu.initialize();
}

void Acelerometro::getXYZ(double [3]){
	mpu.getAcceleration(&ax, &ay, &az);
	acx = ax / ACCSENSITY;

}
MPU6050 Acelerometro::getMpu(){
return mpu;
}
