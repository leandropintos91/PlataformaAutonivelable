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
#include "BTparser.h"
#include <String.h>

#define MARGEN_ERROR_NIVEL_EJES_CON_HISTERISIS 0.6
#define MARGEN_ERROR_NIVEL_EJES_SIN_HISTERISIS 0.3
#define FRECUENCIA_LECTURA_ALTURA 2000
#define HISTERISIS_ALTURA 0.01

uint8_t mpuAddress = 0x68;  //Puede ser 0x68 o 0x69
MPU6050 mpu(mpuAddress);
ultrasonido_hcsr04 sensorDistancia(triggerDistancia, echoDistancia);
float distancia = 0; //distancia medida por el ultrasonido
float distanciaAnt = 0;
float distanciaInicial = 0;
int time = 0;
int timeAnt = 0;
float distBuscada  = 14; //distancia que se busca ajustar desde la app, por defecto 14.
int modo = 1;
int btModo = 0;
int btLed = 1;
int btProx = 1;
float lecturaX, lecturaY, lecturaZ;
int sensity = 1024;
bool subiendoPlataformaAlMaximo = false;
bool bajandoPlataformaAlMinimo = false;
bool motor1Max = false;
bool motor2Max = false;
bool motor3Max = false;
unsigned long currentMillis = 0;
bool debeContraerMotores = true;
bool histerisisY = false;
bool histerisisX = false;
bool debugMediciones = false;
bool guardarTiempoInicioRutina = false;
bool guardarAlturaInicial = false;
long tiempoAnterior = 0;
float alturaAnterior = 0;
int inclinacionASetear = 0;

char btCad[40];
char c;
int indCad = 0;
bool btflag = false;
BTparser btp1;

void setup()
{
  Serial.begin(9600);
  Serial3.begin(9600);
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
  pinMode(pinLedBlanco, OUTPUT);
  pinMode(pinLedVerde, OUTPUT);
  pinMode(pinLedRojo, OUTPUT);
  pararMotores();
}

void loop()
{
  leerEntradaBluetooth();
  realizarMediciones();

  if (modo == 1)
  {
    if (estadoPulsadores() == HIGH)
    {
      if (debeContraerMotores)
      {
        contraerMotores();
      }
      else
      {
        if (!plataformaNivelada())
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
    if (subiendoPlataformaAlMaximo) {

      if(currentMillis - tiempoAnterior > FRECUENCIA_LECTURA_ALTURA)
      {
        if(modulo(distancia - alturaAnterior) < HISTERISIS_ALTURA)
        {
          subiendoPlataformaAlMaximo = false;
          pararMotores();
        }
        else
        {
          tiempoAnterior = currentMillis;
          alturaAnterior = sensorDistancia.getDistancia();
        }
      }
    }
    else if(bajandoPlataformaAlMinimo)
    {
      if(motor1Max == HIGH && motor2Max == HIGH && motor3Max == HIGH)
        bajandoPlataformaAlMinimo = false;
    }
    if(inclinacionASetear == 1)
    {
      Serial.println(" ---- Estoy subiendo un cm ----");
      Serial.println(distancia);
      moverMotoresHorario();
      if(modulo(distancia - distanciaInicial) >= 1)
      {
        pararMotores();
        inclinacionASetear = 0;
      }
      else
      {
        if(currentMillis - tiempoAnterior > FRECUENCIA_LECTURA_ALTURA)
        {
  
            if(modulo(distancia - alturaAnterior) < HISTERISIS_ALTURA)
            {
              pararMotores();
              inclinacionASetear = 0;
            }
            else
            {
              tiempoAnterior = currentMillis;
              alturaAnterior = distancia;
            }
        }
      }
    }
    if(inclinacionASetear == -1)
    {
      Serial.println(" ---- Estoy bajando un cm ----");
      Serial.println(distancia);
      moverMotoresAntihorario();

      Serial.println("contactos: ");
      Serial.println(motor1Max);
      Serial.println(motor2Max);
      Serial.println(motor3Max);

      if(modulo(distancia - distanciaInicial) >= 1 || (motor1Max == HIGH && motor2Max == HIGH && motor3Max == HIGH))
      {
        Serial.println(" --- dejé de bajar ---");
        pararMotores();
        inclinacionASetear = 0;
      }
    }

  }
}

void nivelar()
{
  if (!ejeYNivelado())
  {
    nivelarEjeY();
  }
  else if (!ejeXNivelado())
  {
    nivelarEjeX();
  }
}

void nivelarEjeY()
{
  if (ejeYPositivo())
    bajarEjeY();
  else
    subirEjeY();
}

void bajarEjeY()
{
  if (debugMediciones)
    Serial.println("Bajando eje Y");
  if (motor2Max)
  {
    motor3horario();
  }
  else
    motor2antihorario();
}

void subirEjeY()
{
  if (debugMediciones)
    Serial.println("Subiendo eje Y");
  if (motor3Max)
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
  if (ejeXPositivo())
    bajarEjeX();
  else
    subirEjeX();
}

void bajarEjeX()
{
  if (debugMediciones)
    Serial.println("Bajando eje X");
  if (motor1Max == LOW)
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
  if (debugMediciones)
    Serial.println("Subiendo eje X");
  if (!(motor2Max == LOW) && !(motor3Max == LOW))
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
  if (histerisisX)
  {
    nivelado = modulo(lecturaX) < MARGEN_ERROR_NIVEL_EJES_CON_HISTERISIS;
  }
  else
  {
    nivelado = modulo(lecturaX) < MARGEN_ERROR_NIVEL_EJES_SIN_HISTERISIS;
  }

  histerisisX = nivelado;
  return nivelado;
}

bool ejeYNivelado()
{
  bool nivelado = false;
  if (histerisisY)
  {
    nivelado = modulo(lecturaY) <= MARGEN_ERROR_NIVEL_EJES_CON_HISTERISIS;
  }
  else
  {
    nivelado = modulo(lecturaY) < MARGEN_ERROR_NIVEL_EJES_SIN_HISTERISIS;
  }

  histerisisY = nivelado;
  return nivelado;
}

bool plataformaNivelada()
{
  bool nivelada = ejeYNivelado() && ejeXNivelado();
  digitalWrite(pinLedVerde, (nivelada ? HIGH : LOW));
  digitalWrite(pinLedRojo, (nivelada ? LOW : HIGH));
  return nivelada;
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
  if (debugMediciones)
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
    printTab();
    Serial.print(modulo(lecturaX));
    printTab();
    Serial.print(modulo(lecturaY));
    printTab();
    Serial.print(modo);
    Serial.println(" ");
  }
}

void calcularValoresInclinacion()
{
  int ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);
  lecturaX = ((float)ax) / sensity;
  lecturaY = ((float)ay) / sensity;
  lecturaZ = ((float)az) / sensity;
}

void leerContactos()
{
  motor1Max = digitalRead(finMotor1);
  motor2Max = digitalRead(finMotor2);
  motor3Max = digitalRead(finMotor3);
}

void imprimirContactos()
{
  Serial.print(motor1Max);
  Serial.print(motor2Max);
  Serial.println(motor3Max);
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
  moverMotoresAntihorario();
  if (motor1Max == HIGH && motor2Max == HIGH && motor3Max == HIGH)
  {
    pararMotores();
    debeContraerMotores = false;
  }

}

void bajarPlataforma()
{
  bajandoPlataformaAlMinimo = true;
  moverMotoresAntihorario();
}

void subirPlataforma()
{
  moverMotoresHorario();
  tiempoAnterior = currentMillis;
  alturaAnterior = distancia;
  subiendoPlataformaAlMaximo = true;
  guardarTiempoInicioRutina = true;
  guardarAlturaInicial = true;
}

void subirHasta()
{
  moverMotoresHorario();
  time = 0;
  distanciaAnt = 0;
  while (distancia < distBuscada && (distancia != distanciaAnt))
  {
    if (time == 0)
    {
      time = millis();
      timeAnt = time;
      distancia = sensorDistancia.getDistancia();
      distanciaAnt = distancia;
    } else {
      time = millis();
      if ((time - timeAnt) >= 1500)
      {
        distancia = sensorDistancia.getDistancia();
        if (distancia == distanciaAnt)
        {
          pararMotores();
        } else {
          distanciaAnt = distancia;
          timeAnt = time;
        }
      }
    }
    distancia = sensorDistancia.getDistancia();
  }
  pararMotores();
}

void bajarHasta()
{
  while (distancia > distBuscada)
  {
    leerContactos();
    if (motor1Max == LOW)
      motor1antihorario();
    else
      pararMotor1();
    if (motor2Max == LOW)
      motor2antihorario();
    else
      pararMotor2();
    if (motor3Max == LOW)
      motor3antihorario();
    else
      pararMotor3();
    distancia = sensorDistancia.getDistancia();

  }
  pararMotores();
}
void moverHasta()
{
  distancia = sensorDistancia.getDistancia();
  if (distBuscada < distancia)
  {
    bajarHasta();
  } else if (distancia > distBuscada)
  {
    subirHasta();
  }
}

void leerEntradaBluetooth() {
  if (Serial3.available())   // Si llega un dato por el puerto BT se envía al monitor serial
  {
    c =  Serial3.read();
    if (c == ';')
    {
      indCad = 0;
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
    Serial.println(btCad);
    btp1.parseString(btCad);
    procesarCodigoRecibido();

  }

}

void procesarCodigoRecibido()
{
  Serial.println(btp1.getVal1());
  if (strcmp(btp1.getCode(), "MODE") == 0)
  {
    cambiarModo();
  }
  if (strcmp(btp1.getCode(), "STAT")  == 0)
  {
    enviarEstado();
  }
  if(strcmp(btp1.getCode(), "SETP")  == 0)
  {
    setearAlturaMinimaOMaxima();
  }
  if(strcmp(btp1.getCode(), "SETH")  == 0)
  {
    setearAltura();
  }
  if(strcmp(btp1.getCode(), "SETL")  == 0)
  {
    setearLampara();
  }
}

void setearAlturaMinimaOMaxima()
{
  pararMotores();
  btProx = btp1.getVal1();

  if (btProx == 1)
  {
    bajarPlataforma();
  } else if (btProx == 2)
  {
    subirPlataforma();
  }
}

void setearLampara()
{
  btLed = btp1.getVal1();
  if (btLed == 1)
  {
    digitalWrite(pinLedBlanco, HIGH);
  }
  else if (btLed == 0)
  {
    digitalWrite(pinLedBlanco, LOW);
  }
}

void setearAltura()
{
  pararMotores();
  distanciaInicial = distancia;
  inclinacionASetear = btp1.getVal1();
  Serial.print(" ------ SETIÉ UNA INCLINACIÓN ------   ");
  Serial.println(distanciaInicial);
}

void cambiarModo()
{
  btModo = btp1.getVal1();
  modo = btModo;
  Serial.print("-----cambio a modo ");
  Serial.print(btModo);
  Serial.print(" ");
  Serial.print(modo);
  Serial.print("------");
  Serial3.print("-RMOD OK;");
}

void enviarEstado()
{
  Serial.println(" ---- LLEGO STAT -----");
  realizarMediciones();
  Serial3.print("RSTA ");
  Serial3.print(modo);
  Serial3.print(" ");
  Serial3.print("1");//aqui va el estado del led ---- TODO cambiar esto
  Serial3.print(" ");
  Serial3.print(distancia);
  Serial3.print(" ");
  Serial3.print(lecturaX);
  Serial3.print(" ");
  Serial3.print(lecturaY);
  Serial3.print(" ");
  Serial3.print(lecturaZ);
  Serial3.print(";");
}

float modulo(float numero)
{
  if (numero >= 0L)
  {
    return numero;
  }
  else
  {
    return numero * (-1L);
  }
}
