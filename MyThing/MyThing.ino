// Ex06.ino
// COM3505 lab exercises sketch template
#include <ESPWebServer.h>
#include <SPI.h>
#include <WiFi.h>

// initialisation entry point
void setup() {
  Serial.begin(115200);

  ESPWebServer webServer(80);
  webServer.on("/", handleRoot);

  String toSend = "Aragaki Yui?";

  webServer.send(200, "text/html", toSend);

  WiFi.mode(WIFI_AP_STA));
  WiFi.softAP("ssid", "password");
}

// task loop entry point
void loop() {

}
