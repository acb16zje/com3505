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

  aSyncServer.on("/wifi", [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/wifi.html", String(), false, wifiList);
  });
  
  aSyncServer.on("/wifichz", [](AsyncWebServerRequest *request) {
    wifiChz(request);
    while (WiFi.status() != WL_CONNECTED) {
      delay(1);
    }
    request->send(SPIFFS, "/wifi.html", String(), false, wifiList);
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
  WiFi.mode(WIFI_AP_STA);

  WiFi.softAP(apSSID, apPass);

  routes();

  WiFi.begin(wifiSSID, wifiPass);

  aSyncServer.begin();

  Serial.println(WiFi.softAPIP());

  Serial.println(WiFi.localIP());

}

String wifiList(const String& var) {

  switch(WiFi.status()) {
    case WL_IDLE_STATUS:
      Serial.println("WL_IDLE_STATUS"); break;
    case WL_NO_SSID_AVAIL:
      Serial.println("WL_NO_SSID_AVAIL"); break;
    case WL_SCAN_COMPLETED:
      Serial.println("WL_SCAN_COMPLETED"); break;
    case WL_CONNECTED:
      Serial.println("WL_CONNECTED"); break;
    case WL_CONNECT_FAILED:
      Serial.println("WL_CONNECT_FAILED"); break;
    case WL_CONNECTION_LOST:
      Serial.println("WL_CONNECTION_LOST"); break;
    case WL_DISCONNECTED:
      Serial.println("WL_DISCONNECTED"); break;
    default:
      Serial.println("unknown");
  }

  String f = "";
  const char *checked = "checked";
  int n = WiFi.scanNetworks();
  dbg(netDBG, "Scan done: ");

  if (WiFi.status() == WL_CONNECTED) {
    f += "<p>Currently Connected to: ";
    f += WiFi.SSID();
    f += "</p></br>";
    f += "<p>IP Address: ";
    f += ip2str(WiFi.localIP());
    f += "</p></br>";
  } else {
    f += "Currently not connected to any networks.";
  }

  if(n == 0) {
    dln(netDBG, "No networks found.");
    f += "No WiFi access points found";
    f += "<a href='/'>Back</a><br/><a href='/wifi'>Try again?</a></p>\n";
  } else {
    dbg(netDBG, n); dln(netDBG, " networks found");
    f += "<p>WiFi access points available:</p>"
         "<form method='POST' action='wifichz'><table id='ap'>";
    for(int i = 0; i < n; ++i) {
      f.concat("<tr><td>");
      f.concat("<input type='radio' name='ssid' value='");
      f.concat(WiFi.SSID(i));
      f.concat("'");
      f.concat(checked);
      f.concat("></td><td>");
      f.concat(WiFi.SSID(i));
      f.concat("</td><td>(");
      f.concat(WiFi.RSSI(i));
      f.concat(" dBm)</td>");
      f.concat("</tr>");
      checked = "";

      // Allow connection to hidden network
      if (i == n - 1) {
        f.concat("<tr><td>");
        f.concat("<input type='radio' name='ssid' value='' id='hidRadio'>");
        f.concat("</td><td>");
        f.concat("<input type='text' name='hidden' id='hidText' disabled>");
        f.concat("(Hidden Network)</td><td></td></tr>");
      }
    }

    f += "</table><br/>Password: <input type='text' name='key'><br/><br/> ";
    f += "<input type='submit' value='Submit'></form>";
  }

  return f;
}

void wifiChz(AsyncWebServerRequest *request) {
  dln(netDBG, "Serving page at /wifichz");

  String title = "<h2>Joining WiFi network...</h2>";
  String message = "<p>Check <a href='/'>WiFi status</a>.</p>";

  String ssid = "";
  String key = "";

  int paramCount = request->params();
  for (uint8_t i = 0; i < paramCount; i++ ) {
    AsyncWebParameter* p = request->getParam(i);
    if (p->name() == "ssid")
      ssid = p->value();
    else if (p->name() == "hidden" && ssid == "")
      // hidden network is always the last option
      ssid = p->value();
    else if (p->name() == "key")
      key = p->value();
  }

  if (ssid == "") {
    message = "<h2>Ooops, no SSID...?</h2>\n<p>Looks like a bug</p>";
  } else {
    WiFi.disconnect();
    char ssidchars[ssid.length()+1];
    char keychars[key.length()+1];
    ssid.toCharArray(ssidchars, ssid.length()+1);
    key.toCharArray(keychars, key.length()+1);
    Serial.println(ssidchars);
    Serial.println(keychars);
    WiFi.begin(ssidchars, keychars);
  }



  Serial.println(message);
}
