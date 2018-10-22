// Ex06.ino

// initialisation entry point
void setup06() {
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(ssid, password);

  webServer.on("/", handleRoot06);
  webServer.begin();
}

// task loop entry point
void loop06() {
  webServer.handleClient();
}

void handleRoot06() {
  String s = MAIN_page;
  webServer.send(200, "text/html", s);
}
