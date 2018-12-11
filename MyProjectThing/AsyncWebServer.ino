/////////////////////////////////////////////////////////////////////////////
// AsyncWebServer.ino
// Generate all pages and handle all requests
/////////////////////////////////////////////////////////////////////////////

void startWebServer() {
  if (!SPIFFS.begin()) {
    dln(netDBG, "An Error has occured while mounting SPIFFS");
    return;
  }

  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(apSSID, apPass);
  WiFi.begin(); // for when credentials are already stored by board

  routes();
  aSyncServer.begin();
}

void routes() {
  aSyncServer.serveStatic("/spectre.css", SPIFFS, "/spectre.css");
  aSyncServer.serveStatic("/style.css", SPIFFS, "/style.css");
  aSyncServer.serveStatic("/jquery.js", SPIFFS, "/jquery.js");
  aSyncServer.serveStatic("/script.js", SPIFFS, "/script.js");

  aSyncServer.on("/", [](AsyncWebServerRequest *request) {
    startOTA = false;
    startReset = false;
    request->send(SPIFFS, "/index.html", String(), false, statusTable);
  });

  aSyncServer.on("/control", [](AsyncWebServerRequest *request) {
    startOTA = false;
    startReset = false;
    request->send(SPIFFS, "/control.html", "text/html");
  });

  aSyncServer.on("/wifi", [](AsyncWebServerRequest *request) {
    startOTA = false;
    startReset = false;
    request->send(SPIFFS, "/wifi.html", String(), false, wifiList);
  });

  aSyncServer.on("/wifiJoin", [](AsyncWebServerRequest *request) {
    startOTA = false;
    startReset = false;
    wifiJoin(request);
    while (WiFi.status() != WL_CONNECTED) {
      delay(1);
    }
    request->send(SPIFFS, "/wifi.html", String(), false, wifiList);
  });

  aSyncServer.on("/ota", [](AsyncWebServerRequest *request) {
    startOTA = false;
    startReset = false;
    request->send(SPIFFS, "/ota.html", String(), false, otaPrompt);
  });

  aSyncServer.on("/otaStart", [](AsyncWebServerRequest *request) {
    startOTA = true;
    bool shouldReboot = !Update.hasError();

    AsyncWebServerResponse *response =
      request->beginResponse(200, "text/plain", shouldReboot? "OK" : "FAIL");

    response->addHeader("Connection", "close");
    request->send(response);
  });

  aSyncServer.on("/reset", [](AsyncWebServerRequest *request) {
    startOTA = false;
    startReset = false;
    request->send(SPIFFS, "/ota.html", String(), false, resetPrompt);
  });

  aSyncServer.on("/resetStart", HTTP_GET, [](AsyncWebServerRequest *request) {
    startOTA = true;
    startReset = true;
    bool shouldReboot = !Update.hasError();

    AsyncWebServerResponse *response =
      request->beginResponse(200, "text/plain", shouldReboot? "OK" : "FAIL");

    response->addHeader("Connection", "close");
    request->send(response);
  });

  // Ajax
  aSyncServer.on("/setSpeed", HTTP_GET, [](AsyncWebServerRequest *request) {
    speed = atoi(request->arg("params").c_str());
    request->send(200);
  });

  aSyncServer.on("/stop", HTTP_GET, [](AsyncWebServerRequest *request){
    isStop = true;
    isForward = false;
    isBackward = false;
    isLeft = false;
    isRight = false;
    request->send(200);
  });

  aSyncServer.on("/forward", HTTP_GET, [](AsyncWebServerRequest *request){
    isForward = true;
    isBackward = false;
    isLeft = false;
    isRight = false;
    request->send(200);
  });

  aSyncServer.on("/backward", HTTP_GET, [](AsyncWebServerRequest *request){
    isForward = false;
    isBackward = true;
    isLeft = false;
    isRight = false;
    request->send(200);
  });

  aSyncServer.on("/left", HTTP_GET, [](AsyncWebServerRequest *request){
    isForward = false;
    isBackward = false;
    isLeft = true;
    isRight = false;
    request->send(200);
  });

  aSyncServer.on("/right", HTTP_GET, [](AsyncWebServerRequest *request){
    isForward = false;
    isBackward = false;
    isLeft = false;
    isRight = true;
    request->send(200);
  });
}

String wifiList(const String& var) {
  String f = "";
  const char *checked = "checked";
  short n = WiFi.scanNetworks();
  dbg(netDBG, "Scan done: ");

  if(n == 0) {
    dln(netDBG, "No networks found.");
    f += "No WiFi access points found";
    f += "<a href='/'>Back</a><br/><a href='/wifi'>Try again?</a></p>\n";
  } else {
    dbg(netDBG, n); dln(netDBG, " networks found");
    f += "<p>WiFi access points available:</p>"
         "<form method='POST' action='wifiJoin'>"
         "<table id='ap' class='table table-striped table-hover'>";
    for(short i = 0; i < n; ++i) {
      f.concat("<tr><th class='text-center'>");
      f.concat("<label class='form-radio'><input type='radio' name='ssid' value='");
      f.concat(WiFi.SSID(i));
      f.concat("'");
      f.concat(checked);
      f.concat("><i class='form-icon'></i></label></th><td>");
      f.concat(WiFi.SSID(i));
      f.concat("</td><td>(");
      f.concat(WiFi.RSSI(i));
      f.concat(" dBm)</td>");
      f.concat("</tr>");
      checked = "";

      // Allow connection to hidden network
      if (i == n - 1) {
        f.concat("<tr><th class='text-center'><label class='form-radio'>");
        f.concat("<input type='radio' name='ssid' value='' id='hidRadio'>");
        f.concat("<i class='form-icon'></i></label></th><td>");
        f.concat("<input type='text' name='hidden' id='hidText' class='form-input' disabled>");
        f.concat("</td><td>Hidden Network</td></tr>");
      }
    }

    f += "</table><br/>"
         "<p class='text-center'>Password: <input type='text' name='key'></p>";
    f += "<input type='submit' value='Submit' class='btn btn-lg p-centered'></form>";
  }

  return f;
}

void wifiJoin(AsyncWebServerRequest *request) {
  dln(netDBG, "Serving page at /wifiJoin");

  String title = "<h2>Joining WiFi network...</h2>";
  String message = "<p>Check <a href='/'>WiFi status</a>.</p>";

  String ssid = "";
  String key = "";

  for (uint8_t i = 0; i < request->params(); i++ ) {
    AsyncWebParameter* p = request->getParam(i);
    if (p->name() == "ssid") {
      ssid = p->value();
    } else if (p->name() == "hidden" && ssid == "") {
      ssid = p->value(); // hidden network is always the last option
    } else if (p->name() == "key") {
      key = p->value();
    }
  }

  if (ssid == "") {
    message = "<h2>Ooops, no SSID...?</h2>\n<p>Looks like a bug</p>";
  } else {
    WiFi.disconnect();
    char ssidchars[ssid.length()+1];
    char keychars[key.length()+1];
    ssid.toCharArray(ssidchars, ssid.length() + 1);
    key.toCharArray(keychars, key.length() + 1);
    WiFi.begin(ssidchars, keychars);
  }
}

String statusTable(const String& var) {
  String s = "<table class='table table-striped table-hover'>";
  s += "<tr><th>SSID</th><td>";
  s += WiFi.SSID() + "</td></tr>";
  s += "<tr><th>Status</th><td>";

  switch(WiFi.status()) {
    case WL_IDLE_STATUS:
      s += "WL_IDLE_STATUS"; break;
    case WL_NO_SSID_AVAIL:
      s += "WL_NO_SSID_AVAIL"; break;
    case WL_SCAN_COMPLETED:
      s += "WL_SCAN_COMPLETED"; break;
    case WL_CONNECTED:
      s += "WL_CONNECTED"; break;
    case WL_CONNECT_FAILED:
      s += "WL_CONNECT_FAILED"; break;
    case WL_CONNECTION_LOST:
      s += "WL_CONNECTION_LOST"; break;
    case WL_DISCONNECTED:
      s += "WL_DISCONNECTED"; break;
    default:
      s += "unknown";
  }
  s += "</td></tr><tr><th>Local IP</th><td>";
  s += ip2str(WiFi.localIP()) + "</td></tr>";
  s += "<tr><th>Soft AP IP</th><td>";
  s += ip2str(WiFi.softAPIP()) + "</td></tr>";
  s += "<tr><th>AP SSID name</th><td>";
  s += String(apSSID) + "</td></tr>";
  s += "<tr><th>Current Version</th><td>";
  s += String(currentVersion);
  s += "<a href='/reset' class='btn btn-error mx-2'>Reset</a></td></tr>";
  s += "</td></tr><tr><th>Latest Version</th><td>";

   // do a GET to read the version file from the cloud
  respCode = doCloudGet(&http, gitID, "version");
  if (respCode == 200) {
    highestAvailableVersion = atoi(http.getString().c_str());
     s += String(highestAvailableVersion);
     if (currentVersion < highestAvailableVersion) {
      s += "<a href='/ota' class='btn btn-success mx-2'>Update</a>";
    }
  } else {
    s += "No connection";
  }
  s += "</td></tr></table>";
  http.end(); // Free resource
  return s;
}

String otaPrompt(const String& var) {
  String message = "";
  respCode = doCloudGet(&http, gitID, "version");
  if (respCode != 200) {
    message = "<p>No internet connection.</p>";
  } else if (currentVersion >= highestAvailableVersion) {
    message = "<p>No updates available.</p>";
  } else {
    message = "<p>Press <a href='/otaStart'>here</a> to start</p>";
    message += "<a href='/status'>here</a> to cancel.</p>";
    message += "<p>The device will restart when the update has completed.</p>";
  }
  http.end();
  return message;
}

String resetPrompt(const String& var) {
  String message = "";
  startReset = true;
  respCode = doCloudGet(&http, gitID, "1.bin");
  if (respCode != 200) {
    message = "<p>No internet connection.</p>";
  } else {
    message = "<p>Press <a href='/resetStart'>here</a> to reset</p>";
    message += "<a href='/status'>here</a> to cancel.</p>";
    message += "<p>The device will restart when the reset has been completed.</p>";
  }
  http.end();
  return message;
}
