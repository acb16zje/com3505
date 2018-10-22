// Ex08.ino

// initialisation entry point
void setup09() {
  Serial.begin(115200);
  setup07();
}

// task loop entry point
void loop09() {
  webServer.handleClient();

  htmlPage = getPageTop() + getPageBody();
  htmlPage += prinScanResult(WiFi.scanNetworks()) + getPageFooter();
}

String prinScanResult(int networksFound) {
  String format = "<h3>Network(s) found: </h3>\n";

  for (int i = 0; i < networksFound; i++) {
    format += "<p>";
    format += String(i + 1) + ": ";
    format += String(WiFi.SSID(i)) + ", Ch: ";
    format += String(WiFi.channel(i)) + " (";
    format += String(WiFi.RSSI(i));
    format += "dBm)</p>\n";
  }

  return format;
}

void handleRoot() {
  webServer.send(200, "text/html", htmlPage.c_str());
}
