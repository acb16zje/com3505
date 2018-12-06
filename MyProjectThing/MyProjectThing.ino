////////////////////////setup/////////////////////////////////////////////////////
// MyProjectThing.ino
// COM3505 lab assessment: Robocar
/////////////////////////////////////////////////////////////////////////////

#include "unphone.h"
#include "MyProjectThing.h"
#include "AsyncWebServer.h"
#include "Motor.h"


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

  startMotor();
  startWebServer();
}

// LOOP: task entry point ///////////////////////////////////////////////////
void loop() {
  checkPowerSwitch(); // shutdown if switch off
  // forward();
  // delay(5000);
  // stop();
  // delay(5000);

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

  // if (startOTA) {
  //   doOTAUpdate();
  // }
}
