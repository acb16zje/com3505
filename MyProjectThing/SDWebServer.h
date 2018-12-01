#ifndef SD_WS_H
#define SD_WS_H

// WiFiServer on port 80
WiFiServer wifiServer(80);

// WebServer on port 80
WebServer webServer(80);

// WiFiClient for REST calls
WiFiClient client;

// SSID and Password of your WiFi AP
String apSSID = "Gakki-bot";
String apPass = "gakkismile";

#endif