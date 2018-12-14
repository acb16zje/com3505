#ifndef ASYNC_H
#define ASYNC_H

// ASyncWebServer on port 80
AsyncWebServer aSyncServer(80);

// SSID and Password of your WiFi AP
const char* apSSID = "Gakki-bot";
const char* apPass = "gakkismile";

// Login ID and password
const char* loginID = "esp32";
const char* loginPass = "admin";

// Google geolocation
const char* GOOGLE_API_KEY = "AIzaSyDlV2k2civ3htfph1wiB27LS7lj90FR8ks";
WifiLocation location(GOOGLE_API_KEY);

// Methods
void startWebServer();
void routes();
void ajaxCalls();
void wifiJoin(AsyncWebServerRequest *);
void resetBools();
String wifiList(const String&);
String statusTable(const String&);
String otaConfirmation(const String&);
String resetConfirmation(const String&);
String ip2str(IPAddress address);

#endif
