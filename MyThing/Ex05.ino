// Ex05.ino

// initialisation entry point
void setup05() {
  setup03();
}

// task loop entry point
void loop05() {
  buttonState = digitalRead(pushButton);

  if (loopCount % loopIteration == 0) {
    digitalWrite(externalYELLOW, HIGH);
    digitalWrite(externalGREEN, HIGH);
    digitalWrite(externalRED, HIGH);
    delay(1000);
    digitalWrite(externalYELLOW, LOW);
    digitalWrite(externalGREEN, LOW);
    digitalWrite(externalRED, LOW);
  }

  loop03();

  ++loopCount;
}
