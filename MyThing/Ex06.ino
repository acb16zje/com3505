// Ex06.ino

// initialisation entry point
void setup06() {
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(ssid, password);

  webServer.on("/", handleRoot);
  webServer.onNotFound(handleNotFound);
  webServer.begin();
}

// task loop entry point
void loop06() {
  webServer.handleClient();
}

String getPageTop() {
  return
    "<html><head>\n"
    "<meta charset=\"utf-8\">"
    "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
    "<title>COM3505 IoT</title>"
    "</head><body>\n"
  ;
}

String getPageBody() {
  return "<h1>今日山の日なんだよ、祝日だよ！</h1>\n";
}

String getPageFooter() {
  return "\n<p><a href='/'>Home</a>&nbsp;&nbsp;&nbsp;</p></body></html>\n";
}

void handleNotFound() {
  webServer.send(200, "text/plain", "URI Not Found");
}

// void handleRoot() {
//   String toSend = getPageTop() + getPageBody() + getPageFooter();
//   webServer.send(200, "text/html", toSend);
// }
