#ifndef ASYNC_H
#define ASYNC_H

// ASyncWebServer on port 80
AsyncWebServer aSyncServer(80);

// SSID and Password of your WiFi AP
const char* apSSID = "Gakki-bot22";
const char* apPass = "gakkismile";

void startWebServer();
void routes();
String wifiList(const String&);
void wifiJoin(AsyncWebServerRequest *);
String statusTable(const String&);
String otaPrompt(const String&);
String resetPrompt(const String&);

#endif
