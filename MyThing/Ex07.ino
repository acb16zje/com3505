// Ex07.ino

// initialisation entry point
void setup07() {
  setup06();
}

// task loop entry point
void loop07() {
  webServer.handleClient();
}

// void handleRoot() {
//   String toSend = root;
//   webServer.send(200, "text/html", toSend);
// }
