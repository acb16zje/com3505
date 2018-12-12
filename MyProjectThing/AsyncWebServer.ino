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
  // WiFi.begin(); // for when credentials are already stored by board

  routes();
  aSyncServer.begin();
}

void routes() {
  // Assets
  aSyncServer.serveStatic("/spectre.css", SPIFFS, "/spectre.css").setCacheControl("max-age=600");
  aSyncServer.serveStatic("/style.css", SPIFFS, "/style.css").setCacheControl("max-age=600");
  aSyncServer.serveStatic("/jquery.js", SPIFFS, "/jquery.js").setCacheControl("max-age=600");
  aSyncServer.serveStatic("/script.js", SPIFFS, "/script.js").setCacheControl("max-age=600");

  // Basic routes
  aSyncServer.on("/login", HTTP_ANY, [](AsyncWebServerRequest *request) {
    if (request->authenticate(loginID, loginPass)) {
      request->redirect("/");
    } else {
      return request->requestAuthentication();
    }
  });

  aSyncServer.on("/logout", HTTP_ANY, [](AsyncWebServerRequest *request) {
    request->requestAuthentication();
    request->send(200);
  });

  aSyncServer.on("/", [](AsyncWebServerRequest *request) {
    if (request->authenticate(loginID, loginPass)) {
      startOTA = false;
      startReset = false;
      isAuto = false;
      isStop = true;
      request->send(SPIFFS, "/index.html", String(), false, statusTable);
    } else {
      request->redirect("/login");
    }
  });

  aSyncServer.on("/control", [](AsyncWebServerRequest *request) {
    if (request->authenticate(loginID, loginPass)) {
      startOTA = false;
      startReset = false;
      request->send(SPIFFS, "/control.html", "text/html");
    } else {
      request->redirect("/login");
    }
  });

  aSyncServer.on("/wifi", [](AsyncWebServerRequest *request) {
    if (request->authenticate(loginID, loginPass)) {
      startOTA = false;
      startReset = false;
      isAuto = false;
      isStop = true;
      request->send(SPIFFS, "/wifi.html", String(), false, wifiList);
    } else {
      request->redirect("/login");
    }
  });

  aSyncServer.on("/wifiJoin", [](AsyncWebServerRequest *request) {
    if (request->authenticate(loginID, loginPass)) {
      startOTA = false;
      startReset = false;
      isAuto = false;
      isStop = true;
      wifiJoin(request);
      while (WiFi.status() != WL_CONNECTED) {
        delay(1);
      }
      request->send(SPIFFS, "/wifi.html", String(), false, wifiList);
    } else {
      request->redirect("/login");
    }
  });

  aSyncServer.on("/ota", [](AsyncWebServerRequest *request) {
    if (request->authenticate(loginID, loginPass)) {
      startOTA = false;
      startReset = false;
      isAuto = false;
      isStop = true;
      request->send(SPIFFS, "/ota.html", String(), false, otaPrompt);
    } else {
      request->redirect("/login");
    }
  });

  aSyncServer.on("/otaStart", [](AsyncWebServerRequest *request) {
    if (request->authenticate(loginID, loginPass)) {
      startOTA = true;
      isAuto = false;
      isStop = true;
      request->send(200, "text/plain", !Update.hasError()? "OK" : "FAIL");
      aSyncServer.reset();
    } else {
      request->redirect("/login");
    }
  });

  aSyncServer.on("/reset", [](AsyncWebServerRequest *request) {
    if (request->authenticate(loginID, loginPass)) {
      startOTA = false;
      startReset = false;
      isAuto = false;
      isStop = true;
      request->send(SPIFFS, "/reset.html", String(), false, resetPrompt);
    } else {
      request->redirect("/login");
    }
  });

  aSyncServer.on("/resetStart", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->authenticate(loginID, loginPass)) {
      startOTA = true;
      startReset = true;
      isAuto = false;
      isStop = true;
      request->send(200, "text/plain", !Update.hasError()? "OK" : "FAIL");
      aSyncServer.reset();
    } else {
      request->redirect("/login");
    }
  });

  // jQuery AJAX
  aSyncServer.on("/setSpeed", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->authenticate(loginID, loginPass)) {
      speed = atoi(request->arg("params").c_str());
      request->send(200);
    } else {
      request->redirect("/login");
    }
  });

  aSyncServer.on("/setMode", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->authenticate(loginID, loginPass)) {
      isAuto = String(request->arg("params")) == "auto";
      isStop = !isAuto;
      isForward = isAuto;
      request->send(200);
    } else {
      request->redirect("/login");
    }
  });

  aSyncServer.on("/stop", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->authenticate(loginID, loginPass)) {
      isStop = true;
      isForward = false;
      isBackward = false;
      isLeft = false;
      isRight = false;
      request->send(200);
    } else {
      request->redirect("/login");
    }
  });

  aSyncServer.on("/forward", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->authenticate(loginID, loginPass)) {
      isForward = true;
      isBackward = false;
      isLeft = false;
      isRight = false;
      request->send(200);
    } else {
      request->redirect("/login");
    }
  });

  aSyncServer.on("/backward", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->authenticate(loginID, loginPass)) {
      isForward = false;
      isBackward = true;
      isLeft = false;
      isRight = false;
      request->send(200);
    } else {
      request->redirect("/login");
    }
  });

  aSyncServer.on("/left", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->authenticate(loginID, loginPass)) {
      isForward = false;
      isBackward = false;
      isLeft = true;
      isRight = false;
      request->send(200);
    } else {
      request->redirect("/login");
    }
  });

  aSyncServer.on("/right", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->authenticate(loginID, loginPass)) {
      isForward = false;
      isBackward = false;
      isLeft = false;
      isRight = true;
      request->send(200);
    } else {
      request->redirect("/login");
    }
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
    if (WiFi.SSID() == "")  {
      f += "<p>WiFi access points available:</p>";
    } else {
      f += "<p>Currently connected to <strong>" + WiFi.SSID() + "</strong>. ";
      f += "WiFi access points available:</p>";
    }
    f += "<form method='POST' action='wifiJoin'>"
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
         "<p class='text-center'>Password: <input type='text' name='key'></p>"
         "<input type='submit' value='Submit' class='btn btn-lg p-centered'></form>";
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

  s += "</td></tr><tr><th>Distance Travelled (cm)";
  s += "</th><td>";
  s += String(dist);
  s += "</td></tr></table>";
  http.end(); // Free resource
  return s;
}

String otaPrompt(const String& var) {
  String message;
  respCode = doCloudGet(&http, gitID, "version");
  if (respCode != 200) {
    message = "<p>No internet connection.</p>";
  } else if (currentVersion >= highestAvailableVersion) {
    message = "<p>No updates available.</p>";
  } else {
    message = "<h4 class='text-center'>Please confirm the update process</h4><br>"
              "<div class='columns'><div class='column col-12 text-center'>"
              "<a class='btn btn-lg' href='/'>Back</a>\n"
              "<a class='btn btn-success btn-lg' href='/otaStart'>Update</a></div></div><br>"
              "<p class='text-center'>The device will restart when the update is completed.</p>";
  }
  http.end();
  return message;
}

String resetPrompt(const String& var) {
  String message;
  startReset = true;
  respCode = doCloudGet(&http, gitID, "1.bin");
  if (respCode != 200) {
    message = "<p>No internet connection.</p>";
  } else {
    message = "<h4 class='text-center'>Please confirm the reset process</h4><br>"
              "<div class='columns'><div class='column col-12 text-center'>"
              "<a class='btn btn-lg' href='/'>Back</a>\n"
              "<a class='btn btn-error btn-lg' href='/resetStart'>Reset</a></div></div><br>"
              "<p class='text-center'>The device will restart when the reset is completed.</p>";
  }
  http.end();
  return message;
}

// Utility for printing IP addresses
String ip2str(IPAddress address) {
  return
    String(address[0]) + "." + String(address[1]) + "." +
    String(address[2]) + "." + String(address[3]);
}
