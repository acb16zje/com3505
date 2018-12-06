#ifndef MOTOR_H
#define MOTOR_H

// Motor
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *L_MOTOR = AFMS.getMotor(3);
Adafruit_DCMotor *R_MOTOR = AFMS.getMotor(4);

uint8_t speed = 40; // Default minimum speed
bool isStop = false;
bool isForward = false;
bool isBackward = false;
bool isLeft = false;
bool isRight = false;

// Methods
void startMotor();
void setLeftSpeed(String param);
void setRightSpeed(String param);
void stop();
void forward();
void backward();
void left();
void right();

#endif
