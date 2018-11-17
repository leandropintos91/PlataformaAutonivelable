#include "Arduino.h"
#include "Config.h"

//Motor A
#define motorPin1 5  // Pin 14 of L293
#define motorPin2 6  // Pin 10 of L293
//Motor B
#define motorPin3 10 // Pin  7 of L293
#define motorPin4 9  // Pin  2 of L293
//Motor C
#define motorPin5 11 // Pin  7 of L293
#define motorPin6 12  // Pin  2 of L293

void motor1horario();
void motor1antihorario();
void motor2horario();
void motor2antihorario();
void motor3horario();
void motor3antihorario();
void motor1y2horario();
void motor1y2antihorario();
void motor1y3horario();
void motor1y3antihorario();
void motor2y3horario();
void motor2y3antihorario();
void moverMotoresHorario();
void moverMotoresAntihorario();
void pararMotores();
void pararMotor1();
void pararMotor2();
void pararMotor3();
