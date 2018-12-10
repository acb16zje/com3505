/////////////////////////////////////////////////////////////////////////////
// MyProjectThing.ino
// COM3505 lab assessment: Robocar
/////////////////////////////////////////////////////////////////////////////

#include "unphone.h"
#include "MyProjectThing.h"
#include "AsyncWebServer.h"
#include "Motor.h"
#include "OTA.h"
#include "UltrasonicSensor.h"

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

  io.connect();
  Serial.println(io.statusText());

  startMotor();
  startUltrasonicSensor();
  startWebServer();
}

// LOOP: task entry point ///////////////////////////////////////////////////
void loop() {
  checkPowerSwitch(); // shutdown if switch off

  // Serial.printf("Front Distance: %d              ", getFrontDistance());
  // Serial.printf("Back Distance: %d\n", getBackDistance());

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

  io.run();
  Serial.print("sending -> ");
  Serial.println(dist);
  distance->save(dist);

  dist++;

  delay(10000);
  if (startOTA) {
    doOTAUpdate();
  }
}
