// /////////////////////////////////////////////////////////////////////////////
// // Motor.ino
// // Motor control
// /////////////////////////////////////////////////////////////////////////////

void startMotor() {
  AFMS.begin(); // Initialise motor shield

  // Functions
  rest.function("setLeftSpeed", setLeftSpeed);
  rest.function("setRightSpeed", setRightSpeed);
  rest.function("stop", stop);
  rest.function("forward", forward);
  rest.function("backward", backward);
  rest.function("left", left);
  rest.function("right", right);

  // Give name and ID to device
  rest.set_id("1");
  rest.set_name("Gakki-bot");
}

int setLeftSpeed(String param) {
  L_MOTOR->setSpeed(atoi(param.c_str()));
}

int setRightSpeed(String param) {
  R_MOTOR->setSpeed(atoi(param.c_str()));
}

int stop(String param) {
  L_MOTOR->run(RELEASE);
  R_MOTOR->run(RELEASE);
}

int forward(String param) {
  L_MOTOR->run(FORWARD);
  R_MOTOR->run(FORWARD);
}

int backward(String param) {
  L_MOTOR->run(BACKWARD);
  R_MOTOR->run(BACKWARD);
}

int left(String param) {
  L_MOTOR->run(BACKWARD);
  R_MOTOR->run(FORWARD);
}

int right(String param) {
  L_MOTOR->run(FORWARD);
  R_MOTOR->run(BACKWARD);
}
