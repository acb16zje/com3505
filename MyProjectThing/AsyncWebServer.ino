/////////////////////////////////////////////////////////////////////////////
// aSyncWebServer.ino
// Generate all pages and handle all requests
/////////////////////////////////////////////////////////////////////////////
#import "OTA.h"

void startWebServer() {
  if (!SPIFFS.begin()) {
    Serial.println("An Error has occured while mounting SPIFFS");
    return;
  }

  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(apSSID, apPass);

  WiFi.begin(wifiSSID,wifiPass);

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
    request->send(SPIFFS, "/index.html", String(), false, statusTable);
  });

  aSyncServer.on("/control", [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/control.html", "text/html");
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

  aSyncServer.on("/ota", [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/ota.html", String(), false, otaPrompt);
  });

  aSyncServer.on("/otaStart", [](AsyncWebServerRequest *request) {
    bool shouldReboot = !Update.hasError();
    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", shouldReboot?"OK":"FAIL");
    response->addHeader("Connection", "close");
    request->send(response);
    doOTAUpdate();
  });

  aSyncServer.on("/reset", [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/ota.html", String(), false, resetPrompt);
  });

  aSyncServer.on("/resetStart", [](AsyncWebServerRequest *request) {
    bool shouldReboot = !Update.hasError();
    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", shouldReboot?"OK":"FAIL");
    response->addHeader("Connection", "close");
    request->send(response);
  }, [] (AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool f) {
    doOTAUpdate();
  });

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
      Serial.println("UNKNOWN");
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

String statusTable(const String& var) {
  String s = "<table>";
  s += "<tr><td>SSID</td><td>";
  s += WiFi.SSID();
  s += "</td></tr>";
   s += "<tr><td>Status</td><td>";
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
   s += "</td></tr><tr><td>Local IP</td><td>";
  s += ip2str(WiFi.localIP());
  s += "</td></tr>";
   s += "<tr><td>Soft AP IP</td><td>";
  s += ip2str(WiFi.softAPIP());
  s += "</td></tr>";
   s += "<tr><td>AP SSID name</td><td>";
  s += apSSID;
  s += "</td></tr>";
   s += "<tr><td>Current Version</td><td>";
  s += String(currentVersion);
  s += " <a href='/reset' class='reset'>Reset</a></td></tr>";
  s += "</td></tr><tr><td>Latest Version</td><td>";
   // do a GET to read the version file from the cloud
  respCode = doCloudGet(&http, gitID, "version");
  if (respCode == 200) {
    highestAvailableVersion = atoi(http.getString().c_str());
     s += String(highestAvailableVersion);
     if (currentVersion < highestAvailableVersion) {
      s += " <a href='/ota' class='update'>Update</a>";
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