#include "motorControl.h"

void motor1horario()
{
	digitalWrite(motor1Pin1, HIGH); //motor1 gira sentido horario
    digitalWrite(motor1Pin2, LOW); //motor1 gira sentido antihorario
    digitalWrite(motor2Pin1, LOW);// motor2 gira sentido horario
    digitalWrite(motor2Pin2, LOW); // motor2 gira sentido antihorario
    digitalWrite(motor3Pin1, LOW);// motor3 gira sentido horario
    digitalWrite(motor3Pin2, LOW); // motor3 gira sentido antihorario
}

void motor1antihorario()
{
	digitalWrite(motor1Pin1, LOW); //motor1 gira sentido horario
    digitalWrite(motor1Pin2, HIGH); //motor1 gira sentido antihorario
    digitalWrite(motor2Pin1, LOW);// motor2 gira sentido horario
    digitalWrite(motor2Pin2, LOW); // motor2 gira sentido antihorario
    digitalWrite(motor3Pin1, LOW);// motor3 gira sentido horario
    digitalWrite(motor3Pin2, LOW); // motor3 gira sentido antihorario
}

void motor2horario()
{
	digitalWrite(motor1Pin1, LOW); //motor1 gira sentido horario
    digitalWrite(motor1Pin2, LOW); //motor1 gira sentido antihorario
    digitalWrite(motor2Pin1, HIGH);// motor2 gira sentido horario
    digitalWrite(motor2Pin2, LOW); // motor2 gira sentido antihorario
    digitalWrite(motor3Pin1, LOW);// motor3 gira sentido horario
    digitalWrite(motor3Pin2, LOW); // motor3 gira sentido antihorario
}

void motor2antihorario()
{
	digitalWrite(motor1Pin1, LOW); //motor1 gira sentido horario
    digitalWrite(motor1Pin2, LOW); //motor1 gira sentido antihorario
    digitalWrite(motor2Pin1, LOW);// motor2 gira sentido horario
    digitalWrite(motor2Pin2, HIGH); // motor2 gira sentido antihorario
    digitalWrite(motor3Pin1, LOW);// motor3 gira sentido horario
    digitalWrite(motor3Pin2, LOW); // motor3 gira sentido antihorario
}

void motor3horario()
{
	digitalWrite(motor1Pin1, LOW); //motor1 gira sentido horario
    digitalWrite(motor1Pin2, LOW); //motor1 gira sentido antihorario
    digitalWrite(motor2Pin1, LOW);// motor2 gira sentido horario
    digitalWrite(motor2Pin2, LOW); // motor2 gira sentido antihorario
    digitalWrite(motor3Pin1, HIGH);// motor3 gira sentido horario
    digitalWrite(motor3Pin2, LOW); // motor3 gira sentido antihorario
}

void motor3antihorario()
{
	digitalWrite(motor1Pin1, LOW); //motor1 gira sentido horario
    digitalWrite(motor1Pin2, LOW); //motor1 gira sentido antihorario
    digitalWrite(motor2Pin1, LOW);// motor2 gira sentido horario
    digitalWrite(motor2Pin2, LOW); // motor2 gira sentido antihorario
    digitalWrite(motor3Pin1, LOW);// motor3 gira sentido horario
    digitalWrite(motor3Pin2, HIGH); // motor3 gira sentido antihorario
}

void pararMotores()
{
	digitalWrite(motor1Pin1, LOW); //motor1 gira sentido horario
    digitalWrite(motor1Pin2, LOW); //motor1 gira sentido antihorario
    digitalWrite(motor2Pin1, LOW);// motor2 gira sentido horario
    digitalWrite(motor2Pin2, LOW); // motor2 gira sentido antihorario
    digitalWrite(motor3Pin1, LOW);// motor3 gira sentido horario
    digitalWrite(motor3Pin2, LOW); // motor3 gira sentido antihorario
}
