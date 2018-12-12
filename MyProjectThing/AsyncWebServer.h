#ifndef ASYNC_H
#define ASYNC_H

// ASyncWebServer on port 80
AsyncWebServer aSyncServer(80);

// SSID and Password of your WiFi AP
const char* apSSID = "Gakki-bot22";
const char* apPass = "gakkismile";

// Login ID and password
const char* loginID = "esp32";
const char* loginPass = "admin";

// Methods
void startWebServer();
void routes();
void wifiJoin(AsyncWebServerRequest *);
void resetBools();
String wifiList(const String&);
String statusTable(const String&);
String otaPrompt(const String&);
String resetPrompt(const String&);
String ip2str(IPAddress address);

#endif
