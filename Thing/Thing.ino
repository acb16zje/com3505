// Thing.ino
// COM3505 lab exercise runner sketch

/////////////////////////////////////////////////////////////////////////////
// NOTE!!!
// DON'T edit these files, do your coding in MyThing!
/////////////////////////////////////////////////////////////////////////////

#include "Ex01.h"
#include "Ex02.h"
#include "Ex03.h"
#include "Ex04.h"
#include "Ex05.h"
#include "Ex06.h"
#include "Ex07.h"
#include "Ex08.h"
#include "Ex09.h"

int LABNUM = 1; // which lab exercise number are we doing?

// initialisation entry point
void setup() {
  switch(LABNUM) {
    case  1: setup01(); break;
    case  2: setup02(); break;
    case  3: setup03(); break;
    case  4: setup04(); break;
    case  5: setup05(); break;
    case  6: setup06(); break;
    case  7: setup07(); break;
    case  8: setup08(); break;
    case  9: setup09(); break;
    default: Serial.println("oops! invalid lab number");
  }
}

// task loop entry point
void loop() {
  switch(LABNUM) {
    case  1: loop01(); break;
    case  2: loop02(); break;
    case  3: loop03(); break;
    case  4: loop04(); break;
    case  5: loop05(); break;
    case  6: loop06(); break;
    case  7: loop07(); break;
    case  8: loop08(); break;
    case  9: loop09(); break;
    default: Serial.println("oops! invalid lab number");
  }
}
