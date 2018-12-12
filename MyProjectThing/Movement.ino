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
  short distance = (duration / 2) / 29.1;
  df(moveDBG, "Front distance: %d\n", distance);

  return distance;
}

short getBackDistance() {
  digitalWrite(backTrigPin, LOW);
  delay(100);
  digitalWrite(backTrigPin, HIGH);
  delay(100);
  digitalWrite(backTrigPin, LOW);
  short duration = pulseIn(backEchoPin, HIGH);
  short distance = (duration / 2) / 29.1;
  df(moveDBG, "Back distance: %d\n", distance);

  return distance;
}

void moveRoboCar() {
  if (isAuto) {
    // Auto: Use ultrasonic sensors to prevent collision, has stuck counter
    short d = isForward ? getFrontDistance() : getBackDistance();

    if(d > MAX_DISTANCE) {
      stuckCounter = 0;
      if (isForward) {
        forward();
      } else {
        backward();
      }
    } else if (stuckCounter < MAX_STUCK) {
      turnRight90();
      stop();
      stuckCounter++;
      isForward = getFrontDistance() >= getBackDistance();
    }
  } else {
    // Manual: User-controlled, record distance travelled
    if (startTime == 0 && (isForward || isBackward)) {
      startTime = millis();
      df(moveDBG, "startTime: %d", startTime);
    }

    if (isStop) {
      stop();
      isStop = false;
      if (startTime != 0) {
        timeMoved = millis() - startTime;
        dln(moveDBG, timeMoved);
        dist += (float)((0.28f * speed - 3.5f) * (timeMoved / 1000)); // explained in documentation
        startTime = 0;                                                // Reset startTime since robot is stopped
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

void recall() {
  // Calculate the distance between current position and original position
}
