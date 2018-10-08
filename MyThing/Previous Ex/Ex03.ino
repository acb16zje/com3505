// Ex03.ino
// COM3505 lab exercises sketch template
#include <SPI.h>

int pushButton     = 14;
int externalRED    = 32;
int externalYELLOW = 15;
int externalGREEN  = 12;
int state          = 0;     // the current state of the output pin
int led_select     = 0;
long t             = 0;     // the last time the output pin was toggled
long debounce      = 500;   // the debounce time, increase if the output flickers

// initialisation entry point
void setup() {
  Serial.begin(115200);
  pinMode(pushButton, INPUT_PULLUP);
  pinMode(externalRED, OUTPUT);
  pinMode(externalYELLOW, OUTPUT);
  pinMode(externalGREEN, OUTPUT);
}

// task loop entry point
void loop() {
  state = digitalRead(pushButton);

  if (state == LOW && (millis() - t) > debounce) {
      if (led_select == 3) {
        led_select = 0;
      } else {
        led_select += 1;
      }

      t = millis();

      Serial.printf("state: %X\n",state);
      Serial.printf("led_select: %X\n", led_select);
      switch (led_select) {
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
