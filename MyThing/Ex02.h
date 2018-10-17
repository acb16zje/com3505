#ifndef LAB_02_H
#define LAB_02_H

const int pushButton  = 14;
const int externalLED = 32;
const long debounce   = 500;   // the debounce time, increase if the output flickers

int ledState     = LOW;
int buttonState  = LOW;  // the current state of the output pin
long t           = 0;    // the last time the output pin was toggled

#endif
