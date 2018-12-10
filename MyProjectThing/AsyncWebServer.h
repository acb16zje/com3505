#ifndef ASYNC_H
#define ASYNC_H

// ASyncWebServer on port 80
AsyncWebServer aSyncServer(80);

// SSID and Password of your WiFi AP
const char* apSSID = "Gakki-bot";
const char* apPass = "gakkismile";

// const char* WIFI_SSID = "uos-other";
// const char* WIFI_PASS = "shefotherkey05";

void startWebServer();
void routes();
String wifiList(const String&);
void wifiJoin(AsyncWebServerRequest *);
String statusTable(const String&);
String otaPrompt(const String&);
String resetPrompt(const String&);

#endif
