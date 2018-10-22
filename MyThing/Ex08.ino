// Ex08.ino

// initialisation entry point
void setup08() {
  setup01();
  startWifi();

  url = "/com3505?email=" + email + "&mac=" + mac;
}

// task loop entry point
void loop08() {
  webServer.handleClient();

  dln(monitorDBG, "Connecting to " + String(host));

  if (!client.connect(host, httpPort)) {
    delay(500);
    dln(monitorDBG, "Connection failed");
  }

  dln(monitorDBG, "Requesting URL: " + url);

  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                "Host: " + host + "\r\n" +
                "Connection: close\r\n\r\n");

  unsigned long timeout = millis();

  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      dln(monitorDBG, "Client Timeout !");
      client.stop();
      return;
    }
  }

  while(client.available()) {
    dln(monitorDBG, client.readStringUntil('\r'));
  }

  dln(monitorDBG, "Closing connection");

  delay(10000);
}

void startWifi() {
  WiFi.begin(clientSSID, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    dln(monitorDBG, "Connecting to WiFi..");
  }

  dln(monitorDBG, "Connected to the WiFi network");
  mac = String(MAC_ADDRESS);
}
