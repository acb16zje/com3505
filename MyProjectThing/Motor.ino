// /////////////////////////////////////////////////////////////////////////////
// // Motor.ino
// // Motor control
// /////////////////////////////////////////////////////////////////////////////

void startMotor() {
  AFMS.begin(); // Initialise motor shield
}

void stop() {
  L_MOTOR->setSpeed(0);
  R_MOTOR->setSpeed(0);
  L_MOTOR->run(RELEASE);
  R_MOTOR->run(RELEASE);
}

void forward() {
  L_MOTOR->setSpeed(speed);
  R_MOTOR->setSpeed(speed);
  L_MOTOR->run(FORWARD);
  R_MOTOR->run(FORWARD);
}

void backward() {
  L_MOTOR->setSpeed(speed);
  R_MOTOR->setSpeed(speed);
  L_MOTOR->run(BACKWARD);
  R_MOTOR->run(BACKWARD);
}

void left() {
    L_MOTOR->setSpeed(speed);
  R_MOTOR->setSpeed(speed);
  L_MOTOR->run(BACKWARD);
  R_MOTOR->run(FORWARD);
}

void right() {
  L_MOTOR->setSpeed(speed);
  R_MOTOR->setSpeed(speed);
  L_MOTOR->run(FORWARD);
  R_MOTOR->run(BACKWARD);
}
