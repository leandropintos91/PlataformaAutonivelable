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
#include <SoftwareSerial.h> 
#include "BTparser.h"

#define MARGEN_ERROR_NIVEL_EJES_CON_HISTERISIS 0.25;
#define MARGEN_ERROR_NIVEL_EJES_SIN_HISTERISIS 0.15;

uint8_t mpuAddress = 0x68;  //Puede ser 0x68 o 0x69
MPU6050 mpu(mpuAddress);
ultrasonido_hcsr04 sensorDistancia(triggerDistancia, echoDistancia);
float distancia = 0; //distancia medida por el ultrasonido
float distanciaAnt = 0;
int time = 0;
int timeAnt = 0;
float distBuscada  = 14; //distancia que se busca ajustar desde la app, por defecto 14.
int modo = 1;
int btModo = 0;
int btLed = 1;
int btProx = 1;
float lecturaX, lecturaY, lecturaZ;
int sensity=1024;
bool subiendoMotores = false;
bool motor1Max = false;
bool motor2Max = false;
bool motor3Max = false;
unsigned long currentMillis = 0;
bool debeContraerMotores = true;
bool histerisisY;
bool histerisisX;

SoftwareSerial BT(10,11); // Definimos los pines RX y TX del Arduino conectados al Bluetooth
char btCad[40];
char c;
int indCad = 0;
bool btflag = false;
BTparser btp1;

void setup()
{
   Serial.begin(2400);
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

     /***PRIMERO REVISA SI HAY COMANDOS ENTRANTES DESDE LA APLICACIÓN BLUETOOTH ***/
     procesarEntrada(); //Lee si hay comandos bluetooth entrantes y los procesa
	
	if(modo == 1)
	{
		realizarMediciones();
		
		if (estadoPulsadores() == HIGH){
		  if(debeContraerMotores)
		  {
			contraerMotores();
		  }
		  else
		  {
			if(!plataformaNivelada())
			  nivelar();
			else
			  pararMotores();    
		  }
		  
		}
		else
		{   
		  pararMotores();
		  debeContraerMotores = true;
		}
	} else {		
		if(subiendoMotores){
			if(time == 0)
			{
				time = millis();
				timeAnt = time;
				distancia = sensorDistancia.getDistancia();
				distanciaAnt = distancia;
			} else {
				time = millis();
				if((time - timeAnt)>= 1500)
				{
					distancia = sensorDistancia.getDistancia();
					if(distancia == distanciaAnt)
					{
						pararMotores();
					} else {
						distanciaAnt = distancia;
						timeAnt = time;
					}
				}
			}
		}
		
	}
}

void nivelar()
{
  if(!ejeYNivelado())
  {
    nivelarEjeY();
  }
  else if(!ejeXNivelado())
  {
    nivelarEjeX();
  }
}

void nivelarEjeY()
{
  if(ejeYPositivo())
    bajarEjeY();
  else
    subirEjeY();
}

void bajarEjeY()
{
  Serial.println("Bajando eje Y");
  if(motor2Max)
  {
    motor3horario();
  }
  else
    motor2antihorario();
}

void subirEjeY()
{
  Serial.println("Subiendo eje Y");
  if(motor3Max)
  {
    motor2horario();
  }
  else
    motor3antihorario();
}

bool ejeYPositivo()
{
  return lecturaY > 0;
}

void nivelarEjeX()
{
  if(ejeXPositivo())
    bajarEjeX();
  else
    subirEjeX();
}

void bajarEjeX()
{
  Serial.println("Bajando eje X");
  if(motor1Max == LOW)
  {
    motor1antihorario();
  }
  else
  {
    motor2y3horario();
  }
}

void subirEjeX()
{
  Serial.println("Subiendo eje X");
  //TODO debemos subir primero la pata 1 pero no tenemos forma de saber cuándo está en su máxima extensión.
  if(!(motor2Max == LOW) && !(motor3Max == LOW))
  {
    Serial.println("Motor 2 y 3 antihorario");
    motor2y3antihorario();
  }
  else
  {
    Serial.println("Motor 1 horario");
    motor1horario();
  }
}

bool ejeXPositivo()
{
  return lecturaX > 0;
}

bool ejeXNivelado()
{
  bool nivelado = false;
  if(histerisisX)
  {
    nivelado = abs(lecturaX) < MARGEN_ERROR_NIVEL_EJES_CON_HISTERISIS;
  }
  else
  {
    nivelado = abs(lecturaX) < MARGEN_ERROR_NIVEL_EJES_SIN_HISTERISIS;
  }

  histerisisX = nivelado;
  return nivelado;
}

bool ejeYNivelado()
{
  bool nivelado = false;
  if(histerisisY)
  {
    nivelado = abs(lecturaY) <= MARGEN_ERROR_NIVEL_EJES_CON_HISTERISIS;
  }
  else
  {
    nivelado = abs(lecturaY) < MARGEN_ERROR_NIVEL_EJES_SIN_HISTERISIS;
  }

  histerisisY = nivelado;
  return nivelado;
}

bool plataformaNivelada()
{
  return ejeYNivelado() && ejeXNivelado();
}

void realizarMediciones()
{
  currentMillis = millis();
  distancia = sensorDistancia.getDistancia();
  leerContactos();
  calcularValoresInclinacion();

  imprimirMediciones();
}

void imprimirMediciones()
{
  Serial.print(lecturaX);
  printTab();
  Serial.print(lecturaY);
  printTab();
  Serial.print(distancia);
  printTab();
  Serial.print(ejeXNivelado());
  printTab();
  Serial.print(ejeYNivelado());
  printTab();
  Serial.print(histerisisX);
  printTab();
  Serial.print(histerisisY);
  Serial.println(" ");
}

void calcularValoresInclinacion()
{
  int ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);
  lecturaX = ((float)ax)/sensity;
  lecturaY = ((float)ay)/sensity;
  lecturaZ = ((float)az)/sensity;
}

void leerContactos() 
{
  motor1Max = digitalRead(finMotor1);
  motor2Max = digitalRead(finMotor2);
  motor3Max = digitalRead(finMotor3);
}

void printTab()
{
   Serial.print(F("\t"));
}
 
void printRAW()
{
   Serial.print(F("a[x y z]:t \t"));
   
   Serial.print(lecturaX); printTab();
   Serial.print(lecturaY); printTab();
   Serial.print(lecturaZ); printTab();
}

void contraerMotores() 
{
  leerContactos();
  if(motor1Max == LOW)
    motor1antihorario();
  else
    pararMotor1();
  if(motor2Max == LOW)
    motor2antihorario();
  else
    pararMotor2();
  if(motor3Max == LOW)
    motor3antihorario();
  else
    pararMotor3();
  if(motor1Max == HIGH && motor2Max == HIGH && motor3Max == HIGH)
    debeContraerMotores = false;
    
}

void subirMotores() 
{
    moverMotoresHorario();
	subiendoMotores = true;
}

void procesarEntrada(){
     while(BT.available())    // Si llega un dato por el puerto BT se envía al monitor serial
     {   
        c =  BT.read();
        if(c == ';')
        {
          indCad=0;
          char btCad[40];
          btflag = true;
        }
        else
        {
             btCad[indCad] = c;
             indCad++;
        }
    }
    if (btflag == true)
    {
        btflag = false;
        btp1.parseString(btCad);
    }

    realizarTareas();
}

void realizarTareas()
{
    if(strcmp(btp1.getCode(), "MODE"))
    {
        btModo = btp1.getVal1();
		if(modo == 1)
		{
			modo = btModo;
			BT.write("-RMOD OK;");	
		} else {
			if(btModo == 1)
			{
				modo = btModo;
				BT.write("-RMOD OK;");
			} else {
				BT.write("-RMOD NO;");
			}
		}
    }
    if(strcmp(btp1.getCode(), "STAT"))
    {
        realizarMediciones();
        BT.write("RSTA ");
        BT.write(modo);
        BT.write(" ");
        BT.write("");//aqui va el estado del led
        BT.write(" ");
        BT.write(distancia);//aquí enviar la altura con el ultrasonido
        BT.write(" ");
        BT.write(lecturaX);
        BT.write(" ");
        BT.write(lecturaY);
        BT.write(" ");
        BT.write(lecturaZ);
        BT.write(";");
    }
    if(strcmp(btp1.getCode(), "SETH"))
    {
        distBuscada = btp1.getVal1(); 
    }
	if(strcmp(btp1.getCode(), "SETL"))
    {
		btLed = btp1.getVal1();
		if(btLed == 1)
	    {
		  prenderLed();
	    } else if(btLed == 0) {
		  apagarLed();
	    }
		
    }
	if(strcmp(btp1.getCode(), "SETP"))
    {
       btProx = btp1.getVal1(); 
	   
	   if(btProx == 1)
	   {
		  contraerMotores();
	   } else if(btProx == 2) {
		  subirMotores();
	   }
    }
}
