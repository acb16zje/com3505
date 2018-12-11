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

  startMotor();
  startWebServer();
  IOExpander::pinMode(trigPin, OUTPUT);
  IOExpander::pinMode(echoPin, INPUT);
}

// LOOP: task entry point ///////////////////////////////////////////////////
void loop() {
  checkPowerSwitch(); // shutdown if switch off

  uint8_t duration, distance;

  // IOExpander::digitalWrite(trigPin, HIGH); // We send a 10us pulse
  // delayMicroseconds(10);
  // IOExpander::digitalWrite(trigPin, LOW);

  // duration = pulseIn(echoPin, HIGH, 20000); // We wait for the echo to come back, with a timeout of 20ms, which corresponds approximately to 3m

  // // pulseIn will only return 0 if it timed out. (or if echoPin was already to 1, but it should not happen)
  // if(duration == 0) // If we timed out
  // {
  //   IOExpander::pinMode(echoPin, OUTPUT); // Then we set echo pin to output mode
  //   IOExpander::digitalWrite(echoPin, LOW); // We send a LOW pulse to the echo pin
  //   delayMicroseconds(200);
  //   IOExpander::pinMode(echoPin, INPUT); // And finaly we come back to input mode
  // }
  // distance = (duration/2) / 29.1; // We calculate the distance (sound speed in air is aprox. 291m/s), /2 because of the pulse going and coming

  // IOExpander::digitalWrite(trigPin, LOW);
  // delay(100);
  // IOExpander::digitalWrite(trigPin, HIGH);
  // delay(100);
  // IOExpander::digitalWrite(trigPin, LOW);
  // uint8_t duration = pulseIn(echoPin, HIGH);
  // uint8_t distance = (duration / 2) / 29.1;
  // dln(loopDBG, "Duration: %d        Distance: %d\n", duration, distance);

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
