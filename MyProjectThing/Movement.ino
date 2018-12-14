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
  L_MOTOR->setSpeed(speed+5);
  R_MOTOR->setSpeed(speed);
  L_MOTOR->run(FORWARD);
  R_MOTOR->run(FORWARD);
}

/**
 * Sets the motor to run backward
 */
void backward() {
  L_MOTOR->setSpeed(speed+5);
  R_MOTOR->setSpeed(speed);
  L_MOTOR->run(BACKWARD);
  R_MOTOR->run(BACKWARD);
}

/**
 * Sets the motor to turn left
 */
void left() {
  L_MOTOR->setSpeed(speed+5);
  R_MOTOR->setSpeed(speed);
  L_MOTOR->run(BACKWARD);
  R_MOTOR->run(FORWARD);
}

/**
 * Sets the motor to turn right
 */
void right() {
  L_MOTOR->setSpeed(speed+5);
  R_MOTOR->setSpeed(speed);
  L_MOTOR->run(FORWARD);
  R_MOTOR->run(BACKWARD);
}

/**
 * Turns the robocar 90 degrees to the right
 */
void turnRight90() {
  L_MOTOR->setSpeed(speed+5);
  R_MOTOR->setSpeed(speed);
  L_MOTOR->run(FORWARD);
  R_MOTOR->run(BACKWARD);

  // Time required to turn 90 degrees right
  // (40 speed, 1160 ms), (100 speed, 460 ms), calculate using y = mx +c
  // delay(-35 * speed / 3 + 4880 / 3);
  delay((int)(0.128*speed*speed - 28.8*speed + 2020));

  // Change direction after turning right (North to East, East to South etc.)
  currentDirection = Direction((currentDirection + 1) % (West + 1));
}

void turnLeft90() {
  L_MOTOR->setSpeed(speed+5);
  R_MOTOR->setSpeed(speed);
  L_MOTOR->run(BACKWARD);
  R_MOTOR->run(FORWARD);
  delay((int)(0.128*speed*speed - 28.8*speed + 2020));

  // Change direction after turning left
  switch (currentDirection) {
    case North:
      currentDirection = West;
      break;
    case East:
      currentDirection = North;
      break;
    case South:
      currentDirection = East;
      break;
    case West:
      currentDirection = South;
      break;
  }
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
  
  if (isAuto) {
    if (startTime == 0) {
      startTime = millis();
      df(moveDBG, "startTime: %d", startTime);
    }
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
      coordinate();

      turnRight90();
      stop();

      isForward = getFrontDistance() >= getBackDistance();
      stuckCounter++; // There is a wall in front of the robot
    }
  } else {
    if (startTime == 0 && (isForward || isBackward)) {
      startTime = millis();
      df(moveDBG, "startTime: %d", startTime);
    }
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
  coordinate();

  // ,forward();
  // x > 0: Need to move backward
  // x < 0: Need to move forward
  Serial.print("x: ");
  Serial.println(x);
  Serial.print("y: ");
  Serial.println(y);
  if (x > 0) {
    switch (currentDirection) {
      case North:
        turnLeft90();
        stop();
        break;
      case East:
        turnRight90();
        turnRight90();
        stop();
        break;
      case South:
        turnRight90();
        stop();
        break;
    }
    // int xTime = (int)(x*1000/(0.28f * speed - 3.5f)); 
    forward();
    Serial.print("GOSTAN ");
    Serial.println(x);
    delay(abs(x));
    stop();
  } else if (x < 0) {
    switch (currentDirection) {
      case North:
        turnRight90();
        stop();
        break;
      case West:
        turnRight90();
        turnRight90();
        stop();
        break;
      case South:
        turnLeft90();
        stop();
        break;
    }
    // int xTime = (int)(x*1000/(0.28f * speed - 3.5f)); 
    forward();
    Serial.print("PERGI DEPAN ");
    Serial.println(x);
    delay(abs(x));
    stop();
  } 
  if (y > 0) {
    switch (currentDirection) {
      case North:
        turnRight90();
        turnRight90();
        stop();
        break;
      case East:
        turnRight90();
        stop();
        break;
      case West:
        turnLeft90();
        stop();
        break;
    }
    // int yTime = (int)(y*1000/(0.28f * speed - 3.5f)); 
    forward();
    Serial.print("SOUTH ");
    Serial.println(y);
    delay(abs(y));
    stop();
  } else if (y < 0) {
    switch (currentDirection) {
      case South:
        turnRight90();
        turnRight90();
        stop();
        break;
      case West:
        turnRight90();
        stop();
        break;
      case East:
        turnLeft90();
        stop();
        break;
    }
    // int yTime = (int)(y*1000/(0.28f * speed - 3.5f)); 
    forward();
    Serial.print("NORTH ");
    Serial.println(y);
    delay(abs(y));
    stop();
  } 
  isRecall = false;
  isAuto = false;
  isForward = false;
  isBackward = false;
  currentDirection = North;
}

void coordinate() {
  timeMoved = millis() - startTime;
  startTime = 0; // Reset startTime since robot is stopped
  Serial.print("timeMoved: ");
  Serial.println(timeMoved);
  Serial.print("isForward: ");
  Serial.println(isForward);
  // float tempDist = isForward ? calculateDistanceMoved(timeMoved) : -calculateDistanceMoved(timeMoved);
  float tempDist = isForward ? timeMoved : -timeMoved;
  Serial.print("tempDist: ");
  Serial.println(tempDist);
  Serial.print("direction: ");
  Serial.println(currentDirection);
  // Calculate the distance moved before it met a wall
  switch (currentDirection) {
    case North:
      y += tempDist;
      break;
    case East:
      x += tempDist;
      break;
    case South:
      y -= tempDist;
      break;
    case West:
      x -= tempDist;
      break;
  }
}