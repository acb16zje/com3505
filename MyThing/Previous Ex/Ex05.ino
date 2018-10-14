// Ex05.ino
// COM3505 lab exercises sketch template
#include <SPI.h>

#define pushButton      14
#define externalRED     32
#define externalYELLOW  15
#define externalGREEN   12
#define debounce        500   // the debounce time, increase if the output flickers
int state          = 0;     // the current state of the output pin
int led_select     = 0;
long t             = 0;     // the last time the output pin was toggled
int loop_count     = 0;

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
  loop_count++;
  if (loop_count % 10000000 == 0) {
    digitalWrite(externalYELLOW, HIGH);
    digitalWrite(externalGREEN, HIGH);
    digitalWrite(externalRED, HIGH);
    delay(1000);
    digitalWrite(externalYELLOW, LOW);
    digitalWrite(externalGREEN, LOW);
    digitalWrite(externalRED, LOW);
  }
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
