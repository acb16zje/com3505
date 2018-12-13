#ifndef MOVEMENT_H
#define MOVEMENT_H

// Motor
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *L_MOTOR = AFMS.getMotor(3);
Adafruit_DCMotor *R_MOTOR = AFMS.getMotor(4);

// Ultrasonic Sensors
#define frontTrigPin 26            // pin A0
#define frontEchoPin 4             // pin A5
#define backTrigPin 21             // GPIO 25, the pin below TX
#define backEchoPin 14             // pin 5

// Constants
const uint8_t MAX_STUCK = 4;
const short MAX_DISTANCE = 6;

// Variables
uint8_t stuckCounter = 0;
uint8_t speed = 50;                // Default minimum speed
bool isAuto = false;
bool isRecall = false;
bool isStop = false;
bool isForward = false;
bool isBackward = false;
bool isLeft = false;
bool isRight = false;

// For counting distance moved
float distanceMoved = 0;
int startTime = 0;
int timeMoved = 0;

// Virtual direction
enum Direction {North, East, South, West};
Direction currentDirection = North;

// X and Y coordinate
float x = 0;
float y = 0;

// Methods
void startMovementControl();
void stop();
void forward();
void backward();
void left();
void right();
short getFrontDistance();
short getBackDistance();
void moveRoboCar();
void recall();

#endif
