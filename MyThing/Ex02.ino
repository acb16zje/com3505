// Ex02.ino

// initialisation entry point
void setup02() {
  Serial.begin(115200);

  // set up GPIO pin for an external LED
  pinMode(externalLED, OUTPUT); // set up pin 32 as a digital output

  // set up GPIO pin for a switch
  pinMode(pushButton, INPUT_PULLUP); // pin 14: digital input, built-in pullup resistor
}

// task loop entry point
void loop02() {
  buttonState = digitalRead(pushButton);

  if (buttonState == LOW && (millis() - t) > debounce) {
    ledState == LOW ? buttonState = HIGH : buttonState = LOW;

    t = millis();
    ledState = buttonState;
    digitalWrite(externalLED, buttonState);
  }
}
