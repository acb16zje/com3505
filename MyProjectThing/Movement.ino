// /////////////////////////////////////////////////////////////////////////////
// Movement.ino
// Motor and ultrasonic sensors control.
// /////////////////////////////////////////////////////////////////////////////

/**
 * Starts the motor and ultrasonic sensors
 */
void startMovementControl() {
  AFMS.begin(); // Initialise motor shield

  // Ultrasonic sensors pin
  pinMode(frontTrigPin, OUTPUT);
  pinMode(frontEchoPin, INPUT);
  pinMode(backTrigPin, OUTPUT);
  pinMode(backEchoPin, INPUT);
}

/**
 * Stops the motor
 */
void stop() {
  L_MOTOR->setSpeed(0);
  R_MOTOR->setSpeed(0);
  L_MOTOR->run(RELEASE);
  R_MOTOR->run(RELEASE);
}

/**
 * Sets the motor to run forward
 */
void forward() {
  L_MOTOR->setSpeed(speed);
  R_MOTOR->setSpeed(speed);
  L_MOTOR->run(FORWARD);
  R_MOTOR->run(FORWARD);
}

/**
 * Sets the motor to run backward
 */
void backward() {
  L_MOTOR->setSpeed(speed);
  R_MOTOR->setSpeed(speed);
  L_MOTOR->run(BACKWARD);
  R_MOTOR->run(BACKWARD);
}

/**
 * Sets the motor to turn left
 */
void left() {
  L_MOTOR->setSpeed(speed);
  R_MOTOR->setSpeed(speed);
  L_MOTOR->run(BACKWARD);
  R_MOTOR->run(FORWARD);
}

/**
 * Sets the motor to turn right
 */
void right() {
  L_MOTOR->setSpeed(speed);
  R_MOTOR->setSpeed(speed);
  L_MOTOR->run(FORWARD);
  R_MOTOR->run(BACKWARD);
}

/**
 * Turns the robocar 90 degrees to the right
 */
void turnRight90() {
  L_MOTOR->setSpeed(speed);
  R_MOTOR->setSpeed(speed);
  L_MOTOR->run(FORWARD);
  R_MOTOR->run(BACKWARD);

  // Time required to turn 90 degrees right
  // (40 speed, 1160 ms), (100 speed, 460 ms), calculate using y = mx +c
  delay(-35 * speed / 3 + 4880 / 3);

  // Change direction after turning right (North to East, East to South etc.)
  currentDirection = Direction((currentDirection + 1) % (West + 1));
}

/**
 * Gets the front distance
 *
 * @return short The front distance returned by front ultrasonic sensor
 */
short getFrontDistance() {
  // Send out the ultrasonic pulse
  digitalWrite(frontTrigPin, LOW);
  delay(100);
  digitalWrite(frontTrigPin, HIGH);
  delay(100);
  digitalWrite(frontTrigPin, LOW);

  // Receive the echo of ultrasonic pulse
  short duration = pulseIn(frontEchoPin, HIGH);
  short distance = (duration / 2) / 29.1;
  df(moveDBG, "Front distance: %d\n", distance);

  return distance;
}

/**
 * Gets the back distance
 *
 * @return short The back distanec returned by back ultrasonic sensor
 */
short getBackDistance() {
  // Send out the ultrasonic pulse
  digitalWrite(backTrigPin, LOW);
  delay(100);
  digitalWrite(backTrigPin, HIGH);
  delay(100);
  digitalWrite(backTrigPin, LOW);

  // Receive the echo of ultrasonic pulse
  short duration = pulseIn(backEchoPin, HIGH);
  short distance = (duration / 2) / 29.1;
  df(moveDBG, "Back distance: %d\n", distance);

  return distance;
}

/**
 * Calculates the total distance moved from the startTime to timeMoved
 *
 * @param  t     The time moved
 * @return float Total distance moved from the startTime to timeMoved
 */
float calculateDistanceMoved(int t) {
  // Formula obtained by experiments, explained in MyProjectThing.mkd
  return (float)((0.28f * speed - 3.5f) * (t / 1000));
}

/**
 * Move the robocar, in auto or manual mode
 */
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
  forward();
  // x > 0: Need to move backward
  // x < 0: Need to move forward
  // if (x > 0) {

  // } else if (x < 0) {

  // }
}
