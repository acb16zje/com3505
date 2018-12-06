#ifndef MOTOR_H
#define MOTOR_H

// Motor
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *L_MOTOR = AFMS.getMotor(3);
Adafruit_DCMotor *R_MOTOR = AFMS.getMotor(4);

bool isStop = false;
bool isForward = false;
bool isBackward = false;
bool isLeft = false;
bool isRight = false;

// Methods
void setupMotor();
// int setLeftSpeed(String param);
// int setRightSpeed(String param);
// int stop(String param);
// int forward(String param);
// int backward(String param);
// int left(String param);
// int right(String param);

#endif
