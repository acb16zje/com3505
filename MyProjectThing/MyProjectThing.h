#ifndef MY_PRJ_H
#define MY_PRJ_H

// Libraries required
#include <WiFi.h>
#include <aREST.h>
#include <Adafruit_MotorShield.h>
#include <WebServer.h>
#include <HTTPClient.h>             // ESP32 library for making HTTP requests
#include <Update.h>                 // OTA update library

#define dbg(b, s) if(b) Serial.print(s)
#define dln(b, s) if(b) Serial.println(s)
#define df(b, s, f) if(b) Serial.printf(s, f)

#define setupDBG      true        // For setup phase
#define netDBG        true        // For networking things
#define otaDBG        true        // For debugging in OTA update
#define loopDBG       true        // For debugging in the loop()
#define isGUI         false       // WebServer UI for the device?

// Methods
void setup();
void loop();

#endif
