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

  startMotor();
  startUltrasonicSensor();
  startWebServer();

  io.connect();
  Serial.println(io.statusText());

  while(io.status() < AIO_CONNECTED) {
    delay(500);
  }
  distance->onMessage(handleMessage);
  distance->get();

  // dist = (distance->lastValue()).value();

  updatedTime = millis();
  currentTime = millis();
}

// LOOP: task entry point ///////////////////////////////////////////////////
void loop() {
  checkPowerSwitch(); // shutdown if switch off

  Serial.printf("Front Distance: %d              ", getFrontDistance());
  Serial.printf("Back Distance: %d\n", getBackDistance());

  if (isStop) {
    stop();
    isStop = false;
    timeMoved = millis() - startTime;
    dist += (float)((0.28f*speed - 3.5f)*(timeMoved/10000));
  } else if (isForward) {
    forward();
    startTime = millis();
  } else if (isBackward) {
    backward();
    startTime = millis();
  } else if (isLeft) {
    left();
    startTime = millis();
  } else if (isRight) {
    right();
    startTime = millis();
  }

  io.run();
  Serial.print("sending -> ");
  Serial.println(dist);
  distance->save(dist);

  currentTime = millis();

  if  (currentTime - updatedTime >= period) {
    updatedTime = currentTime;
    io.run();
    Serial.print("sending -> ");
    Serial.println(dist);
    distance->save(dist);
  }

  if (startOTA) {
    doOTAUpdate();
  }
}

void handleMessage(AdafruitIO_Data *data) {

  Serial.print("received <- ");
  Serial.print(data -> value());
  if (dist <= 0) {
    dist = atof(data->value());
  }
}