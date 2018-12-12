#ifndef MY_PRJ_H
#define MY_PRJ_H

// Libraries required
#include <WiFi.h>
#include <Adafruit_MotorShield.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <HTTPClient.h>             // ESP32 library for making HTTP requests
#include <Update.h>                 // OTA update library
#include <AdafruitIO_WiFi.h>        // Adafruit IO

#define batteryPin 25               // Pin to read battery voltage
float aread;                        
float avolt;

#define dbg(b, s) if(b) Serial.print(s)
#define dln(b, s) if(b) Serial.println(s)
#define df(b, s, f) if(b) Serial.printf(s, f)

#define setupDBG      true          // For setup phase
#define netDBG        true          // For networking things
#define otaDBG        true          // For debugging in OTA update
#define loopDBG       true          // For debugging in the loop()

const char* IO_USERNAME = "njh97";
const char* IO_KEY      = "ba9f3fd658054b7893333e3a89c2ab42";

const char* WIFI_SSID = "uos-other";
const char* WIFI_PASS = "shefotherkey05";

AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);
AdafruitIO_Feed *distance = io.feed("distance");

float dist = 0;
int period = 10000;

int startTime;
int timeMoved;

int currentTime;
int updatedTime;

// Methods
void setup();
void loop();

#endif
