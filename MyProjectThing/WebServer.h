#ifndef WEB_H
#define WEB_H

#include <WiFi.h>

// WiFiServer on port 80
WiFiServer server(80);

// WiFiClient for REST calls
WiFiClient client;

// SSID and Password of your WiFi AP
String apSSID = "Gakki-bot";
String apPass = "gakkismile";

#endif
