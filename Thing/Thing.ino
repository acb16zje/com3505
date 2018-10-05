// Thing.ino
// COM3505 lab exercise runner sketch

/////////////////////////////////////////////////////////////////////////////
// NOTE!!!
// DON'T edit these files, do your coding in MyThing!
/////////////////////////////////////////////////////////////////////////////

#include "Ex01.h"

int LABNUM = 1; // which lab exercise number are we doing?

// initialisation entry point
void setup() {
  switch(LABNUM) {
    case  1: setup01(); break;
    default: Serial.println("oops! invalid lab number");
  }
}

// task loop entry point
void loop() {
  switch(LABNUM) {
    case  1: loop01(); break;
    default: Serial.println("oops! invalid lab number");
  }
}
