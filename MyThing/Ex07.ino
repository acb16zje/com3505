// Ex07.ino

// initialisation entry point
void setup07() {
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(ssid, password);

  webServer.on("/", handleRoot);
  webServer.begin();
}

// task loop entry point
void loop07() {
  webServer.handleClient();
}

void handleRoot() {
}
