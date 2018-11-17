
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
#include "ultrasonido_hcsr04.h"

uint8_t mpuAddress = 0x68;  //Puede ser 0x68 o 0x69
MPU6050 mpu(mpuAddress);
ultrasonido_hcsr04 sensorDistancia(triggerDistancia, echoDistancia);

int distancia = 0;
 
int ax, ay, az;
int gx, gy, gz;
int MODO = 1; // 1-Libre ; 2-Imitando Celular ; 3-Altura determinada por celular
float lecturaX, lecturaY, lecturaZ;

float acx, acy, acz;
int sensity=16384;
unsigned long previousMillis = 0;        
const long interval = 1000;     

bool nivelando = false;
bool histX = true;
bool histY = true;
bool histZ = true;

short sentido = 1;

bool motor1Max = false;
bool motor2Max = false;
bool motor3Max = false;

bool invertir = false;
bool horario = true;
int turno = 1;

unsigned long currentMillis = 0;
unsigned long ultimoTiempo = 0;

bool debeContraerMotores = true;


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
   Serial.begin(1200);
   Wire.begin();
   mpu.initialize();
   Serial.println(mpu.testConnection() ? F("IMU iniciado correctamente") : F("Error al iniciar IMU"));
   pinMode(pulsador1, INPUT);
   pinMode(pulsador2, INPUT);
   pinMode(pulsador3, INPUT);
   pinMode(motor1Pin1, OUTPUT);
   pinMode(motor1Pin2, OUTPUT);
   pinMode(motor2Pin1, OUTPUT);
   pinMode(motor2Pin2, OUTPUT);
   pinMode(motor3Pin1, OUTPUT);
   pinMode(motor3Pin2, OUTPUT);
   pinMode(finMotor1, INPUT);
   pinMode(finMotor2, INPUT);
   pinMode(finMotor3, INPUT);
   pararMotores();
}
 
void loop()
{

    realizarMediciones();
    
    if (estadoPulsadores() == HIGH){
      if (MODO == 1){
        if(debeContraerMotores)
        {
          contraerMotores();
        }
        else
        {
           Serial.print(lecturaX);
           printTab();
           Serial.print(histX);
           printTab();
           Serial.print(motor1Max);
           Serial.print("|");
           Serial.print(lecturaY);
           printTab();
           Serial.print(histY);
           printTab();
           Serial.print(motor2Max);
           printTab();
           Serial.println(motor3Max);
           
           if(abs(lecturaY) > 0.2 and histY == false)
           {   Serial.print("Y NO OK");
              if(lecturaY < 0)
              {
                if(motor3Max == LOW)
                {
                  Serial.println("bajando motor 3");
                  motor3antihorario();
                }
                else
                {
                  Serial.println("subiendo motor 2");
                  motor2horario();
                }
              }
              if(lecturaY > 0)
              {
                  if(motor2Max == LOW)
                  {
                    Serial.println("bajando motor 2");
                    motor2antihorario();
                  }
                  else
                  {
                    Serial.println("subiendo motor 3");
                    motor3horario();
                  }
               }
               /* else
                {
                  Serial.println("subiendo motor 1");
                  motor1horario();
                }*/
             }
              else 
              { 
                 Serial.print("Y OK");
                 pararMotores();
                  
                 histY = true;  
              
                 if(abs(lecturaX) > 0.2 and histX == false)
                 {  Serial.print("1");
                    if(lecturaX > 0)
                    {Serial.print("2");
                      if(motor1Max == LOW)
                      {
                        Serial.println("bajando motor 1");
                        motor1antihorario();
                      }
                      else
                      {
                        Serial.print("No se puede nivelar X");
                        //Serial.println("subiendo 2 y 3");
                        //motor2y3horario();
                      }
                    }
                    if(lecturaX < 0)
                    {Serial.print("3");
                      motor1horario();
                    }
                    else
                    {
                      Serial.print("No se puede nivelar X");
                     }  
                      /*if(motor2Max == LOW || motor3Max == LOW)
                      {
                        Serial.println("bajando motor 2 y 3");
                        if(motor2Max == LOW && motor3Max == LOW)
                          motor2y3antihorario();
                        else
                        {
                          if(motor2Max == LOW)
                            motor2antihorario();
                          else
                            motor3antihorario();
                        }
                      }
                      else
                      {
                        Serial.println("subiendo motor 1");
                        motor1horario();
                      }*/
                    }
                 
                 else 
                 {
                    pararMotores();
                    Serial.print("4");
                    histX = true;  
                 }
            
           }
        
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
    if(MODO != 1)
      debeContraerMotores = true;
    
    /*if (currentMillis - previousMillis >= interval) {
      // save the last time 
      previousMillis = currentMillis;
      // Leer las aceleraciones y velocidades angulares
    }
    */
}

void realizarMediciones()
{
  currentMillis = millis();
  distancia = sensorDistancia.getDistancia();
  leerContactos();
  calcularValoresInclinacion();
}

void calcularValoresInclinacion()
{
  mpu.getAcceleration(&ax, &ay, &az);
  lecturaX = ((float)ax)/1024;
  lecturaY = ((float)ay)/1024;
  lecturaZ = ((float)az)/1024;
  
  if(abs(lecturaY) > 0.5)
    histY = false;
  else
    if(abs(lecturaX) > 0.5)
      histX = false;
    else
      if(abs(lecturaZ) > 0.5)
        histZ = false;
}

void leerContactos() 
{
  motor1Max = digitalRead(finMotor1);
  motor2Max = digitalRead(finMotor2);
  motor3Max = digitalRead(finMotor3);
}

void subir()
{
  switch(turno)
  {
    case 1: motor1horario();
    break;

    case 2: motor2horario();
    break;

    case 3: motor3horario();
    break;
  }
}

void bajar()
{
  switch(turno)
  {
    case 1: motor1antihorario();
    break;

    case 2: motor2antihorario();
    break;

    case 3: motor3antihorario();
    break;
  }
}

void cambioDeTurno()
{
  if((currentMillis - ultimoTiempo) > 2500)
  {
    Serial.println("cambio de turno");
    ultimoTiempo = currentMillis;
    turno++;
    turno = turno > 3 ? 1 : turno;
    pararMotores();
    Serial.println(turno);
  }
}

void cambioSentido()
{
  if((currentMillis - ultimoTiempo) > 2500)
  {
        Serial.println("cambio de sentido");
    ultimoTiempo = currentMillis;
    sentido *= -1;
  }
}

void loopPruebaSubeYBaja()
{

  leerContactos();
 if(true)
 {
         distancia = sensorDistancia.getDistancia();
         if(distancia > 13)
         {
          sentido = 0;
         }
         if(motor1Max && motor2Max && motor3Max)
         {
          sentido = 1;
         }

         Serial.print(motor1Max);
         Serial.print(motor2Max);
         Serial.print(motor3Max);
         printTab();
         Serial.print(distancia);
                  printTab();
         Serial.println(sentido);

         if(sentido == 1){
          moverMotoresHorario();
         }else {
          moverMotoresAntihorario();
         }
         cambioDeTurno();
 }
}

void loop_contraer()
{
  contraerMotores();
}

void contraerMotores() 
{
  leerContactos();
  if(motor1Max == LOW)
  {
    Serial.println("contrayendo 1");
    motor1antihorario();
  }
  else
  {
    pararMotor1();
    if(motor2Max == LOW)
    {
      Serial.println("contrayendo 2");
      motor2antihorario();
    }
     else
     {
        pararMotor2();
        if(motor3Max == LOW)
        {
          Serial.println("contrayendo 3");
          motor3antihorario();
        }
        else
        {
          Serial.println("fin contracción");
          debeContraerMotores = false;
        }
     }
  }
    
}

void extenderMotores() 
{
  turno = (millis() % 3000);
  if(turno <= 999)
    motor1horario();
  if(turno >= 1000 && turno < 1999)
    motor2horario();
  if(turno >= 2000)
    motor3horario();
}
