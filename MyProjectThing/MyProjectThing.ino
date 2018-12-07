/////////////////////////////////////////////////////////////////////////////
// MyProjectThing.ino
// COM3505 lab assessment: Robocar
/////////////////////////////////////////////////////////////////////////////

#include "unphone.h"
#include "MyProjectThing.h"
#include "AsyncWebServer.h"
#include "Motor.h"
#include "OTA.h"

// SETUP: initialisation entry point
void setup() {
  Wire.setClock(100000); // higher rates trigger an IOExpander bug
  Serial.begin(115200);  // init the serial line

  Wire.begin();          // start I²C
  IOExpander::begin();   // start unPhone's IOExpander library
  checkPowerSwitch();    // check if power switch is now off & if so shutdown

  // set up the SD card
  // IOExpander::digitalWrite(IOExpander::SD_CS, LOW);
  // if(!SD.begin(-1)) {
  //   D("Card Mount Failed");
  //   delay(3000);
  // }
  // IOExpander::digitalWrite(IOExpander::SD_CS, HIGH);

  // startMotor();
  // startWebServer();
  IOExpander::pinMode(trigPin, OUTPUT);
  IOExpander::pinMode(echoPin, INPUT);
}

// LOOP: task entry point ///////////////////////////////////////////////////
void loop() {
  checkPowerSwitch(); // shutdown if switch off

  IOExpander::digitalWrite(trigPin, LOW);
  delay(100);
  IOExpander::digitalWrite(trigPin, HIGH);
  delay(100);
  IOExpander::digitalWrite(trigPin, LOW);
  int duration = pulseIn(echoPin, HIGH);
  int distance = (duration / 2) / 29.1;
  Serial.printf("Duration: %d        Distance: %d\n", duration, distance);

  if (isStop) {
    stop();
    isStop = false;
  } else if (isForward) {
    forward();
  } else if (isBackward) {
    backward();
  } else if (isLeft) {
    left();
  } else if (isRight) {
    right();
  }

  if (startOTA) {
    doOTAUpdate();
  }
}
