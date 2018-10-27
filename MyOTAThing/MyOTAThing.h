// MyOTAThing.h
#ifndef MY_OTA_H
#define MY_OTA_H

// the WiFi and HTTP server libraries ///////////////////////////////////////
#include <HTTPClient.h>             // ESP32 library for making HTTP requests
#include <Update.h>                 // OTA update library

// debugging infrastructure; setting different DBGs true triggers prints ////
#define dbg(b, s) if(b) Serial.print(s)
#define dln(b, s) if(b) Serial.println(s)
#define setupDBG        true
#define netDBG          true
#define monitDBG        true

void setup();
void loop();

// OTA stuff ////////////////////////////////////////////////////////////////
int currentVersion = 1;                       // used to check for updates
const String gitID = "Juneezee";              // team's git ID
const int minSize = 100000;                   // 100k bytes
const int maxSize = 1000000;                  // 1 mega bytes

int doCloudGet(HTTPClient *, String, String); // helper for downloading 'ware
void doOTAUpdate();                           // main OTA logic

int loopIteration = 0;

#endif
