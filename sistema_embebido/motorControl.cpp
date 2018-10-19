#include "motorControl.h"

void motor1horario()
{
	digitalWrite(motorPin1, HIGH); //motor1 gira sentido horario
    digitalWrite(motorPin2, LOW); //motor1 gira sentido antihorario
    digitalWrite(motorPin4, LOW);// motor2 gira sentido horario
    digitalWrite(motorPin3, LOW); // motor2 gira sentido antihorario
    digitalWrite(motorPin5, LOW);// motor3 gira sentido horario
    digitalWrite(motorPin6, LOW); // motor3 gira sentido antihorario
}

void motor1antihorario()
{
	digitalWrite(motorPin1, LOW); //motor1 gira sentido horario
    digitalWrite(motorPin2, HIGH); //motor1 gira sentido antihorario
    digitalWrite(motorPin4, LOW);// motor2 gira sentido horario
    digitalWrite(motorPin3, LOW); // motor2 gira sentido antihorario
    digitalWrite(motorPin5, LOW);// motor3 gira sentido horario
    digitalWrite(motorPin6, LOW); // motor3 gira sentido antihorario
}

void motor2horario()
{
	digitalWrite(motorPin1, LOW); //motor1 gira sentido horario
    digitalWrite(motorPin2, LOW); //motor1 gira sentido antihorario
    digitalWrite(motorPin4, HIGH);// motor2 gira sentido horario
    digitalWrite(motorPin3, LOW); // motor2 gira sentido antihorario
    digitalWrite(motorPin5, LOW);// motor3 gira sentido horario
    digitalWrite(motorPin6, LOW); // motor3 gira sentido antihorario
}

void motor2antihorario()
{
	digitalWrite(motorPin1, LOW); //motor1 gira sentido horario
    digitalWrite(motorPin2, LOW); //motor1 gira sentido antihorario
    digitalWrite(motorPin4, LOW);// motor2 gira sentido horario
    digitalWrite(motorPin3, HIGH); // motor2 gira sentido antihorario
    digitalWrite(motorPin5, LOW);// motor3 gira sentido horario
    digitalWrite(motorPin6, LOW); // motor3 gira sentido antihorario
}

void motor3horario()
{
	digitalWrite(motorPin1, LOW); //motor1 gira sentido horario
    digitalWrite(motorPin2, LOW); //motor1 gira sentido antihorario
    digitalWrite(motorPin4, LOW);// motor2 gira sentido horario
    digitalWrite(motorPin3, LOW); // motor2 gira sentido antihorario
    digitalWrite(motorPin5, HIGH);// motor3 gira sentido horario
    digitalWrite(motorPin6, LOW); // motor3 gira sentido antihorario
}

void motor3antihorario()
{
	digitalWrite(motorPin1, LOW); //motor1 gira sentido horario
    digitalWrite(motorPin2, LOW); //motor1 gira sentido antihorario
    digitalWrite(motorPin4, LOW);// motor2 gira sentido horario
    digitalWrite(motorPin3, LOW); // motor2 gira sentido antihorario
    digitalWrite(motorPin5, LOW);// motor3 gira sentido horario
    digitalWrite(motorPin6, HIGH); // motor3 gira sentido antihorario
}

void pararMotores()
{
	digitalWrite(motorPin1, LOW); //motor1 gira sentido horario
    digitalWrite(motorPin2, LOW); //motor1 gira sentido antihorario
    digitalWrite(motorPin4, LOW);// motor2 gira sentido horario
    digitalWrite(motorPin3, LOW); // motor2 gira sentido antihorario
    digitalWrite(motorPin5, LOW);// motor3 gira sentido horario
    digitalWrite(motorPin6, LOW); // motor3 gira sentido antihorario
}
