/////////////////////////////////////////////////////////////////////////////
// MyProjectThing.ino
// COM3505 lab assessment: Robocar
/////////////////////////////////////////////////////////////////////////////

#include "unphone.h"
#include "MyProjectThing.h"
#include "AsyncWebServer.h"
#include "Movement.h"
#include "OTA.h"

// SETUP: initialisation entry point
void setup() {
  Wire.setClock(100000); // higher rates trigger an IOExpander bug
  Serial.begin(115200);  // init the serial line

  Wire.begin();          // start I²C
  startBatteryCount();
  IOExpander::begin();   // start unPhone's IOExpander library
  checkPowerSwitch();    // check if power switch is now off & if so shutdown

  // set up the SD card
  // IOExpander::digitalWrite(IOExpander::SD_CS, LOW);
  // if(!SD.begin(-1)) {
  //   D("Card Mount Failed");
  //   delay(3000);
  // }
  // IOExpander::digitalWrite(IOExpander::SD_CS, HIGH);

  startMovementControl();
  startWebServer();

  io.connect();
  Serial.println(io.statusText());

  while(io.status() < AIO_CONNECTED) {
    delay(500);
  }
  distance->onMessage(handleMessage);
  distance->get();

  startTime = 0;
  updatedTime = millis();
  currentTime = millis();
}

// LOOP: task entry point ///////////////////////////////////////////////////
void loop() {
  checkPowerSwitch(); // shutdown if switch off

  moveRoboCar();

  // Sends value to AdaFruit in 10 seconds intervals
  currentTime = millis();

  if (currentTime - updatedTime >= period) {
    updatedTime = currentTime;
    io.run();
    Serial.print("sending -> ");
    Serial.println(dist);
    distance->save(dist);
  }

  //Initialise OTA Update
  if (startOTA) {
    doOTAUpdate();
  }

  if (stuckCounter == 4) {
    sendIFTTT(&http);
  }
}

void handleMessage(AdafruitIO_Data *data) {
  Serial.print("received <- ");
  Serial.print(data -> value());
  if (dist <= 0) {
    dist = atof(data->value());
  }
}

void startBatteryCount() {
  pinMode(batteryPin,INPUT);
  analogReadResolution(12);  // 10 bit is 0-1023, 11 bit is 0-2047, 12 bit is 0-4095
  analogSetPinAttenuation(batteryPin, ADC_6db); // 0db is 0-1V, 2.5db is 0-1.5V, 6db is 0-2.2v, 11db is 0-3.3v
}