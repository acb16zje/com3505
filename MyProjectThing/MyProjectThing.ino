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

  startMovementControl();
  startWebServer();

  io.connect();
  dln(setupDBG, io.statusText());

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

  if (isRecall) {
    recall();
  } else {
    moveRoboCar();
  }

  // Send value to AdaFruit in 10 seconds intervals
  currentTime = millis();

  if (currentTime - updatedTime >= period) {
    updatedTime = currentTime;
    io.run();
    df(dataDBG, "Sending -> %f\n", distanceMoved);
    distance->save(distanceMoved);
  }

  // Send IFTTT notification to phone when the robocar is stucked
  if (stuckCounter == MAX_STUCK) {
    sendIFTTT(&http);
  }

  // Initialise OTA Update
  if (startOTA) {
    doOTAUpdate();
  }
}

//
void handleMessage(AdafruitIO_Data *data) {
  df(dataDBG, "Received -> %s\n", data->value());
  if (distanceMoved <= 0) {
    distanceMoved = atof(data->value());
  }
}

// Read battery voltage output
void startBatteryCount() {
  pinMode(batteryPin,INPUT);
  analogReadResolution(12);  // 10 bit is 0-1023, 11 bit is 0-2047, 12 bit is 0-4095
  analogSetPinAttenuation(batteryPin, ADC_6db); // 0db is 0-1V, 2.5db is 0-1.5V, 6db is 0-2.2v, 11db is 0-3.3v
}

void sendIFTTT(HTTPClient *http) {
  String url = "https://maker.ifttt.com/trigger/" + triggerName + "/with/key/" + apiKey;
  http->begin(url);
  http->addHeader("User-Agent", "ESP32");
  http->GET();
}