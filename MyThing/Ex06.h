#ifndef LAB_06_H
#define LAB_06_H

#include<WebServer.h>
#include<WiFi.h>

// SSID and Password of your WiFi router
const char* ssid = "赤道以北，gakki 最美";
const char* password = "gakkismile";

// Create a WebServer on port 80
WebServer webServer(80);

#endif
