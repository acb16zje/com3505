/////////////////////////////////////////////////////////////////////////////
// MyProjectThing.ino
// COM3505 lab assessment: Robocar
/////////////////////////////////////////////////////////////////////////////

#include "unphone.h"
#include "MyProjectThing.h"
#include "OTA.h"
#include "Motor.h"
#include "SDWebServer.h"

void recoverI2C() {   // try to recover I2C bus in case it's locked up...
  pinMode(SCL, OUTPUT);
  pinMode(SDA, OUTPUT);
  digitalWrite(SDA, HIGH);

  for(int i = 0; i < 10; i++) { // 9th cycle acts as NACK
    digitalWrite(SCL, HIGH);
    delayMicroseconds(5);
    digitalWrite(SCL, LOW);
    delayMicroseconds(5);
  }

  // a STOP signal (SDA from low to high while SCL is high)
  digitalWrite(SDA, LOW);
  delayMicroseconds(5);
  digitalWrite(SCL, HIGH);
  delayMicroseconds(2);
  digitalWrite(SDA, HIGH);
  delayMicroseconds(2);

  // I2C bus should be free now... a short delay to help things settle
  delay(200);
}

// SETUP: initialisation entry point
void setup() {
  Wire.setClock(100000); // higher rates trigger an IOExpander bug
  Serial.begin(115200);  // init the serial line

  // fire up I²C, and the unPhone's IOExpander library
  Wire.begin();
  IOExpander::begin();

  checkPowerSwitch(); // check if power switch is now off & if so shutdown

  // set up the SD card
  IOExpander::digitalWrite(IOExpander::SD_CS, LOW);
  if(!SD.begin(-1)) {
    D("Card Mount Failed");
    delay(3000);
  }
  IOExpander::digitalWrite(IOExpander::SD_CS, HIGH);
  startMotor();
  startSDWebServer();
}

// LOOP: task entry point ///////////////////////////////////////////////////
void loop() {
  checkPowerSwitch(); // shutdown if switch off

  // Handle REST calls
  client = wifiServer.available();
  if (!client) {
    return;
  }
  while(!client.available()){
    delay(1);
  }
  rest.handle(client);
}
