// Ex03.ino

// initialisation entry point
void setup03() {
  setup02();

  pinMode(externalRED, OUTPUT);
  pinMode(externalYELLOW, OUTPUT);
  pinMode(externalGREEN, OUTPUT);
}

// task loop entry point
void loop03() {
  buttonState = digitalRead(pushButton);

  if (buttonState == LOW && (millis() - t) > debounce) {
    ledSelect == 3 ? ledSelect = 0 : ledSelect += 1;

    t = millis();

    switch (ledSelect) {
      case 0:
        digitalWrite(externalGREEN, LOW);
        break;
      case 1:
        digitalWrite(externalRED, HIGH);
        break;
      case 2:
        digitalWrite(externalRED, LOW);
        digitalWrite(externalYELLOW, HIGH);
        break;
      case 3:
        digitalWrite(externalYELLOW, LOW);
        digitalWrite(externalGREEN, HIGH);
        break;
    }
  }
}
