
//Acelerometro pins
//GND - GND
//VCC - VCC
//SDA - Pin A4
//SCL - Pin A5
 
#include "I2Cdev.h"
#include "MPU6050.h"
#include "Wire.h"
#include "motorControl.h"
#include "botonControl.h"

uint8_t mpuAddress = 0x68;  //Puede ser 0x68 o 0x69
MPU6050 mpu(mpuAddress);
 
int ax, ay, az;
int gx, gy, gz;
int MODO = 1; // 1-Libre ; 2-Imitando Celular ; 3-Altura determinada por celular

float acx, acy, acz;
int sensity=16384;
unsigned long previousMillis = 0;        
const long interval = 1000;     

bool nivelando = false;
bool histX = true;
bool histY = true;
bool histZ = true;

bool motor1Max = false;
bool motor2Max = false;
bool motor3Max = false;


void printTab()
{
   Serial.print(F("\t"));
}
 
void printRAW()
{
   Serial.print(F("a[x y z] g[x y z]:t \t"));
   acx = (float)ax / sensity;
   acy = (float)ay / sensity;
   acz = (float)az / sensity;
   
   Serial.print(acx); printTab();
   Serial.print(acy); printTab();
   Serial.print(acz); printTab();
   Serial.print(gx); printTab();
   Serial.print(gy); printTab();
   Serial.println(gz);
}
 
void setup()
{
   Serial.begin(9600);
   Wire.begin();
   mpu.initialize();
   Serial.println(mpu.testConnection() ? F("IMU iniciado correctamente") : F("Error al iniciar IMU"));
}
 
void loop()
{

    unsigned long currentMillis = millis();
    
    if (estadoPulsadores() == HIGH){
      if (MODO == 1){
        
         mpu.getAcceleration(&ax, &ay, &az);
         
         if(ax > 1) histX = false;
         else
         if(ay > 1) histX = false;
         else
         if(az > 1) histX = false;
         
         if(ax > 0.2 and histX == false)
            if(motor1Max == false) 
                motor1horario();
            else{
                motor2antihorario(); //   no se pueden hacer al mismo tiempo por ahora por lo cual hay que
                motor3antihorario(); //   cambiar acá o poner código para el motor que mueva de a 2 motores.
            }
                
            
         else 
         {
            pararMotores();
            
            histX = true;  
         }
         if(ay > 0.2  and histY == false)
         
           if(motor2Max == false) 
                motor2horario();
            else{
                motor1antihorario(); //   no se pueden hacer al mismo tiempo por ahora por lo cual hay que
                motor3antihorario(); //   cambiar acá o poner código para el motor que mueva de a 2 motores.
            }
            
         else 
         {
            pararMotores();
            
            histY = true;
         }
         if(az > 0.2  and histZ == false)
         
            if(motor3Max == false) 
                motor3horario();
            else{
                motor1antihorario(); //   no se pueden hacer al mismo tiempo por ahora por lo cual hay que
                motor2antihorario(); //   cambiar acá o poner código para el motor que mueva de a 2 motores.
            }
            
         else 
         {
            pararMotores();
            
            histZ = true;
         }                
      }
      if (MODO == 2){
      //recibir valores a los cuales apuntar y mover motores. Sería similar al modo 1 pero cambian los valores de los IF.  
      }
      
      if (MODO == 3){
      //recibir altura y mover todos los motores en igual cantidad. Sería conveniente tenerlo nivelado primero.
      }
      
    }
    else
    { 
       pararMotores();    
    }
    
    /*if (currentMillis - previousMillis >= interval) {
      // save the last time 
      previousMillis = currentMillis;
      // Leer las aceleraciones y velocidades angulares
    }
    */
}
