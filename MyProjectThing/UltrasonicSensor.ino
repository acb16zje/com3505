/////////////////////////////////////////////////////////////////////////////
// UltrasonicSensor.ino
// Detect front and back distance using Ultrasonic Sensors
/////////////////////////////////////////////////////////////////////////////

// SETUP: initialisation entry point
void startUltrasonicSensor() {
  pinMode(frontTrigPin, OUTPUT);
  pinMode(frontEchoPin, INPUT);
  pinMode(backTrigPin, OUTPUT);
  pinMode(backEchoPin, INPUT);
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
