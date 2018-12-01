/////////////////////////////////////////////////////////////////////////////
// Motor.ino
// Motor control
/////////////////////////////////////////////////////////////////////////////

void setupMotor() {
  AFMS.begin(); // Initialise motor shield

  // Functions
  rest.function("stop", stop);
  rest.function("forward", forward);
  rest.function("backward", backward);
  rest.function("left", left);
  rest.function("right", right);

  // Give name and ID to device
  rest.set_id("1");
  rest.set_name("Gakki-bot");
}

int stop(String command) {
  L_MOTOR->setSpeed(0);
  L_MOTOR->run( RELEASE );

  R_MOTOR->setSpeed(0);
  R_MOTOR->run( RELEASE );
}

int forward(String command) {
  L_MOTOR->setSpeed(100);
  L_MOTOR->run( FORWARD );

  R_MOTOR->setSpeed(100);
  R_MOTOR->run( FORWARD );
  Serial.println("forward");
}

int backward(String command) {
  L_MOTOR->setSpeed(50);
  L_MOTOR->run( BACKWARD );

  R_MOTOR->setSpeed(50);
  R_MOTOR->run( BACKWARD );
  Serial.println("backward");
}

int left(String command) {
  L_MOTOR->setSpeed(50);
  L_MOTOR->run( BACKWARD );

  R_MOTOR->setSpeed(50);
  R_MOTOR->run( FORWARD );
  Serial.println("left");
}

int right(String command) {
  L_MOTOR->setSpeed(50);
  L_MOTOR->run( FORWARD );

  R_MOTOR->setSpeed(50);
  R_MOTOR->run( BACKWARD );
  Serial.println("right");
}
