// /////////////////////////////////////////////////////////////////////////////
// Movement.ino
// Motor and ultrasonic sensors control.
// /////////////////////////////////////////////////////////////////////////////

void startMovementControl() {
  AFMS.begin(); // Initialise motor shield

  // Ultrasonic sensors pin
  pinMode(frontTrigPin, OUTPUT);
  pinMode(frontEchoPin, INPUT);
  pinMode(backTrigPin, OUTPUT);
  pinMode(backEchoPin, INPUT);
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

void turnRight90() {
  L_MOTOR->setSpeed(speed);
  R_MOTOR->setSpeed(speed);
  L_MOTOR->run(FORWARD);
  R_MOTOR->run(BACKWARD);
  delay(1080); // Exact time required to turn 90 degrees at speed 40
}

short getFrontDistance() {
  digitalWrite(frontTrigPin, LOW);
  delay(100);
  digitalWrite(frontTrigPin, HIGH);
  delay(100);
  digitalWrite(frontTrigPin, LOW);
  short duration = pulseIn(frontEchoPin, HIGH);

  return (duration / 2) / 29.1;
}

short getBackDistance() {
  digitalWrite(backTrigPin, LOW);
  delay(100);
  digitalWrite(backTrigPin, HIGH);
  delay(100);
  digitalWrite(backTrigPin, LOW);
  short duration = pulseIn(backEchoPin, HIGH);

  return (duration / 2) / 29.1;
}

void moveRoboCar() {
  // Robot moves randomly in autonomous mode
  // Makes use of ultrasonic sensors to prevent collision
  if (isAuto) {
    short d;
    if (isForward) {
      d = getFrontDistance();
    } else {
      d = getBackDistance();
    }
    df(loopDBG, "Distance: %d\n", d);
    if(d > 4) {
      if (isForward) {
        forward();
      } else {
        backward(); 
      }
    } else {
      turnRight90();
      stop();

      if (getFrontDistance() >= getBackDistance()) {
        isForward = true;
      } else {
        isForward = false;
      }
    }
  } else {
    if (startTime == 0 && (isForward || isBackward)) {
      startTime = millis();
      // Serial.printf("startTime: %d", startTime);
    }
    if (isStop) {
      stop();
      isStop = false;
      if (startTime != 0) {
        timeMoved = millis() - startTime;
        Serial.println(timeMoved);
        dist += (float)((0.28f*speed - 3.5f)*(timeMoved/1000));
        startTime = 0; // setting startTime to 0 determines no movement ongoing
      }
    } else if (isForward) {
      forward();
    } else if (isBackward) {
      backward();
    } else if (isLeft) {
      left();
    } else if (isRight) {
      right();
    }
  }
}