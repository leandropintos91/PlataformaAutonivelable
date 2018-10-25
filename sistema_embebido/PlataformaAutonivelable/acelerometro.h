#include "Config.h"
#include "MPU6050.h"
#include "I2Cdev.h"

class Acelerometro{
    
  MPU6050 mpu;
  //mpu.testConnection() ? : exit(2);
  int ax=0, ay=0, az=0;
  //int gx, gy, gz;

  float acx, acy, acz;

  public:
    Acelerometro();
    void getXYZ(double [3]);
    MPU6050 getMpu();
};
