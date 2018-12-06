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
void startMotor();
int setLeftSpeed(String param);
int setRightSpeed(String param);
int stop();
int forward();
int backward();
int left();
int right();

#endif
