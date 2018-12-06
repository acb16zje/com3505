/////////////////////////////////////////////////////////////////////////////
// aSyncWebServer.ino
// Generate all pages and handle all requests
/////////////////////////////////////////////////////////////////////////////

void startWebServer() {
  if (!SPIFFS.begin()) {
    Serial.println("An Error has occured while mounting SPIFFS");
    return;
  }

  WiFi.mode(WIFI_AP);
  WiFi.softAP(apSSID, apPass);

  routes();
  aSyncServer.begin();

  dln(netDBG, WiFi.softAPIP());
}

void routes() {
  aSyncServer.serveStatic("/spectre.css", SPIFFS, "/spectre.css");
  aSyncServer.serveStatic("/style.css", SPIFFS, "/style.css");
  aSyncServer.serveStatic("/jquery.js", SPIFFS, "/jquery.js");
  aSyncServer.serveStatic("/script.js", SPIFFS, "/script.js");

  aSyncServer.on("/", [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html", "text/html");
  });

  // aSyncServer.on("/setLeftSpeed", HTTP_GET, [](AsyncWebServerRequest *request) {
  //   // setLeftSpeed(request->arg("params"));
  //   request->send(200);
  // });
  // aSyncServer.on("/setRightSpeed", HTTP_GET, [](AsyncWebServerRequest *request) {
  //   // setRightSpeed(request->arg("params"));
  //   request->send(200);
  // });

  aSyncServer.on("/stop", HTTP_GET, [](AsyncWebServerRequest *request){
    isStop = true;
    request->send(200);
  });

  aSyncServer.on("/forward", HTTP_GET, [](AsyncWebServerRequest *request){
    isForward = true;
    request->send(200);
  });

  aSyncServer.on("/backward", HTTP_GET, [](AsyncWebServerRequest *request){
    isBackward = true;
    request->send(200);
  });

  aSyncServer.on("/left", HTTP_GET, [](AsyncWebServerRequest *request){
    isLeft = true;
    request->send(200);
  });

  aSyncServer.on("/right", HTTP_GET, [](AsyncWebServerRequest *request){
    isRight = true;
    request->send(200);
  });
}
