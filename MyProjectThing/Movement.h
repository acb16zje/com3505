#ifndef MOVEMENT_H
#define MOVEMENT_H

// Motor
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *L_MOTOR = AFMS.getMotor(3);
Adafruit_DCMotor *R_MOTOR = AFMS.getMotor(4);

#define frontTrigPin 26             // pin A0
#define frontEchoPin 25             // pin A1
#define backTrigPin 21              // GPIO 25, the pin below TX
#define backEchoPin 4               // pin A5

uint8_t speed = 40; // Default minimum speed
bool isAuto = false;
bool isStop = false;
bool isForward = false;
bool isBackward = false;
bool isLeft = false;
bool isRight = false;

// Methods
void startMovementControl();
void stop();
void forward();
void backward();
void left();
void right();
short getFrontDistance();
short getBackDistance();

#endif
