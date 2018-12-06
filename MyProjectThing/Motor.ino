// /////////////////////////////////////////////////////////////////////////////
// // Motor.ino
// // Motor control
// /////////////////////////////////////////////////////////////////////////////

void startMotor() {
  AFMS.begin(); // Initialise motor shield
}

int setLeftSpeed(String param) {
  L_MOTOR->setSpeed(atoi(param.c_str()));
}

int setRightSpeed(String param) {
  R_MOTOR->setSpeed(atoi(param.c_str()));
}

int stop() {
  L_MOTOR->setSpeed(0);
  R_MOTOR->setSpeed(0);
  L_MOTOR->run(RELEASE);
  R_MOTOR->run(RELEASE);
}

int forward() {
  L_MOTOR->setSpeed(40);
  R_MOTOR->setSpeed(40);
  L_MOTOR->run(FORWARD);
  R_MOTOR->run(FORWARD);
}

int backward() {
  L_MOTOR->setSpeed(40);
  R_MOTOR->setSpeed(40);
  L_MOTOR->run(BACKWARD);
  R_MOTOR->run(BACKWARD);
}

int left() {
  L_MOTOR->setSpeed(40);
  R_MOTOR->setSpeed(40);
  L_MOTOR->run(BACKWARD);
  R_MOTOR->run(FORWARD);
}

int right() {
  L_MOTOR->setSpeed(40);
  R_MOTOR->setSpeed(40);
  L_MOTOR->run(FORWARD);
  R_MOTOR->run(BACKWARD);
}
