/////////////////////////////////////////////////////////////////////////////
// WebServer.ino
// Generate all pages and handle all requests
/////////////////////////////////////////////////////////////////////////////

void startWebServer() {
  WiFi.mode(WIFI_AP_STA);

  // apSSID = Gakki-bot
  // apPass = gakkismile
  WiFi.softAP(apSSID.c_str(), apPass.c_str());

  server.begin();
  Serial.println(WiFi.softAPIP());
}
