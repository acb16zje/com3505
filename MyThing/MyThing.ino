// Ex06.ino
// COM3505 lab exercises sketch template
#include <WiFi.h>
#include <WebServer.h>

#include "Ex07_index.h"

//SSID and Password of your WiFi router
const char* ssid = "Aragaki Yui";
const char* password = "ismywife";

WebServer webServer(80);

// initialisation entry point
void setup() {
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(ssid, password);


  webServer.on("/", handleRoot);
  webServer.begin();
}

// task loop entry point
void loop() {
  webServer.handleClient();
}

void handleRoot() {
  String s = MAIN_page;
  webServer.send(200, "text/html", s);
}
