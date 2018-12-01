#ifndef MOTOR_H
#define MOTOR_H

#include <aREST.h>
#include <Adafruit_MotorShield.h>

// Motor
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *L_MOTOR = AFMS.getMotor(3);
Adafruit_DCMotor *R_MOTOR = AFMS.getMotor(4);

// aRest
aREST rest = aREST();

// Methods
void setupMotor();
int stop(String message);
int forward(String message);
int backward(String message);
int left(String message);
int right(String message);

#endif
