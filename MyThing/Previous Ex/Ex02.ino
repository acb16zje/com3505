// Ex02.ino
// COM3505 lab exercises sketch template
#include <SPI.h>

int pushButton  = 14
int externalLED = 32;
int state       = HIGH;  // the current state of the output pin
int prev_state  = HIGH;
long t          = 0;     // the last time the output pin was toggled
long debounce   = 500;   // the debounce time, increase if the output flickers

// initialisation entry point
void setup() {
  Serial.begin(115200);
  pinMode(pushButton, INPUT_PULLUP);
  pinMode(externalLED, OUTPUT);
}

// task loop entry point
void loop() {
  state = digitalRead(pushButton);

  if (state == LOW && (millis() - t) > debounce) {
    if (prev_state == LOW) {
      state = HIGH;
    } else {
      state = LOW;
    }

    t = millis();
    prev_state = state;
    Serial.printf("state: %X\n", state);
    Serial.printf("prev_state: %X\n", prev_state);
    digitalWrite(externalLED, state);
  }
}
