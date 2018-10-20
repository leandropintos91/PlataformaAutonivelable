#include "arduino.h"
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"

#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

MPU6050 mpu;
Quaternion q;           // [w, x, y, z]         quaternion container
uint8_t fifoBuffer[64]; // FIFO storage buffer
VectorFloat gravity;    // [x, y, z]            gravity vector
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)

void setup(){
      Serial.begin(115200);
      
  


  
}

void loop(){
 //#ifdef OUTPUT_READABLE_YAWPITCHROLL
      // read a packet from FIFO
      packetSize = mpu.dmpGetFIFOPacketSize();
      mpu.getFIFOBytes(fifoBuffer, packetSize);
      
      // display Euler angles in degrees
      mpu.dmpGetQuaternion(&q, fifoBuffer);
      mpu.dmpGetGravity(&gravity, &q);
      mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
      Serial.print("ypr\t");
      Serial.print(ypr[0] * 180/M_PI);
      Serial.print("\t");
      Serial.print(ypr[1] * 180/M_PI);
      Serial.print("\t");
      Serial.println(ypr[2] * 180/M_PI);
  //#endif
  
}
