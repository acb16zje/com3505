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

  // Change direction after turning right
  switch (currentDirection) {
    case North:
      currentDirection = East;
      break;
    case East:
      currentDirection = South;
      break;
    case South:
      currentDirection = West;
      break;
    case West:
      currentDirection = North;
      break;
  }
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

// Explained in the documentation
float calculateDistanceMoved(int t) {
  return (float)((0.28f * speed - 3.5f) * (t / 1000));
}

void moveRoboCar() {
  if (startTime == 0 && (isForward || isBackward)) {
    startTime = millis();
    df(moveDBG, "startTime: %d", startTime);
  }

  if (isAuto) {
    // Auto: Use ultrasonic sensors to prevent collision, has stuck counter
    short distance = isForward ? getFrontDistance() : getBackDistance();

    if (distance > MAX_DISTANCE) {
      stuckCounter = 0;
      if (isForward) {
        forward();
      } else {
        backward();
      }
    } else if (stuckCounter < MAX_STUCK) {
      turnRight90();
      stop();

      timeMoved = millis() - startTime;
      startTime = 0; // Reset startTime since robot is stopped

      // Calculate the distance moved before it met a wall
      switch (currentDirection) {
        case North:
          y += calculateDistanceMoved(timeMoved);
          break;
        case East:
          x += calculateDistanceMoved(timeMoved);
          break;
        case South:
          y -= calculateDistanceMoved(timeMoved);
          break;
        case West:
          x -= calculateDistanceMoved(timeMoved);
          break;
      }

      isForward = getFrontDistance() >= getBackDistance();
      stuckCounter++; // There is a wall in front of the robot
    }
  } else {
    // Manual: User-controlled, record distance travelled
    if (isStop) {
      stop();
      isStop = false;
      if (startTime != 0) {
        timeMoved = millis() - startTime;
        startTime = 0; // Reset startTime since robot is stopped
        distanceMoved += calculateDistanceMoved(timeMoved);
        dln(moveDBG, timeMoved);
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
  // x > 0: Need to move backward
  // x < 0: Need to move forward
  if (x > 0) {
    
  } else if (x < 0) {

  }
}
