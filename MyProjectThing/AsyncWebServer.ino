/////////////////////////////////////////////////////////////////////////////
// AsyncWebServer.ino
// Handle web requests
/////////////////////////////////////////////////////////////////////////////

/**
 * Starts the WiFI AP and the AsyncWebServer
 */
void startWebServer() {
  if (!SPIFFS.begin()) {
    dln(netDBG, "An Error has occured while mounting SPIFFS");
    return;
  }

  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(apSSID, apPass);
  WiFi.begin(WIFI_SSID, WIFI_PASS); // wireless AP wont show up if this fails

  routes();
  ajaxCalls();
  aSyncServer.begin();
}

/**
 * Handles all general routes (ajax excluded)
 */
void routes() {
  // Assets
  aSyncServer.serveStatic("/spectre.css", SPIFFS, "/spectre.css").setCacheControl("max-age=600");
  aSyncServer.serveStatic("/style.css", SPIFFS, "/style.css").setCacheControl("max-age=600");
  aSyncServer.serveStatic("/jquery.js", SPIFFS, "/jquery.js").setCacheControl("max-age=600");
  aSyncServer.serveStatic("/script.js", SPIFFS, "/script.js").setCacheControl("max-age=600");

  // Basic routes
  aSyncServer.on("/login", [](AsyncWebServerRequest *request) {
    if (request->authenticate(loginID, loginPass)) {
      request->redirect("/");
    } else {
      return request->requestAuthentication();
    }
  });

  aSyncServer.onNotFound([](AsyncWebServerRequest *request) {
    // Static HTML page, not required to flash into SPIFFS
    const char html[] = {
      "<!DOCTYPE html><html>"
      "<head>"
        "<meta charset='utf-8'>"
        "<meta name='viewport' content='width=device-width, initial-scale=1.0, user-scalable=no'>"
        "<title>Gakki-bot</title>"
        "<link rel='stylesheet' href='spectre.css'>"
        "<link rel='stylesheet' href='style.css'>"
      "</head>"
      "<body>"
        "<div class='container grid-sm'>"
          "<div class='columns'>"
            "<div class='column col-7 col-md-8 col-sm-10 is-centered'>"
              "<div class='hero hero-lg'>"
                "<div class='hero-body'>"
                  "<h1 class='text-primary text-center'>404 Not Found</h1>"
                  "<div class='text-center'>"
                    "<a href='/' class='btn btn-lg'>Home</a>"
                  "</div>"
                "</div>"
              "</div>"
            "</div>"
          "</div>"
        "</div>"
      "</body></html>"
    };

    request->send(404, "text/html", String(html));
  });

  aSyncServer.on("/logout", [](AsyncWebServerRequest *request) {
    // Static HTML page, not required to flash into SPIFFS
    const char html[] = {
      "<!DOCTYPE html><html>"
      "<head>"
        "<meta charset='utf-8'>"
        "<meta name='viewport' content='width=device-width, initial-scale=1.0, user-scalable=no'>"
        "<title>Gakki-bot</title>"
        "<link rel='stylesheet' href='spectre.css'>"
        "<link rel='stylesheet' href='style.css'>"
      "</head>"
      "<body>"
        "<div class='container grid-sm'>"
          "<div class='columns'>"
            "<div class='column col-7 col-md-8 col-sm-10 is-centered'>"
              "<div class='hero hero-lg'>"
                "<div class='hero-body'>"
                  "<h1 class='text-primary text-center'>Gakki-bot</h1>"
                  "<div class='text-center'>"
                    "<a href='/login' class='btn btn-success btn-lg'>Sign in</a>"
                  "</div>"
                "</div>"
              "</div>"
            "</div>"
          "</div>"
        "</div>"
      "</body></html>"
    };

    request->send(401, "text/html", String(html));
  });

  aSyncServer.on("/", [](AsyncWebServerRequest *request) {
    if (request->authenticate(loginID, loginPass)) {
      resetBools();
      request->send(SPIFFS, "/index.html", String(), false, statusTable);
    } else {
      request->redirect("/login");
    }
  });

  aSyncServer.on("/control", [](AsyncWebServerRequest *request) {
    if (request->authenticate(loginID, loginPass)) {
      resetBools();
      request->send(SPIFFS, "/control.html", "text/html");
    } else {
      request->redirect("/login");
    }
  });

  aSyncServer.on("/wifi", [](AsyncWebServerRequest *request) {
    if (request->authenticate(loginID, loginPass)) {
      resetBools();
      request->send(SPIFFS, "/wifi.html", String(), false, wifiList);
    } else {
      request->redirect("/login");
    }
  });

  aSyncServer.on("/wifiJoin", [](AsyncWebServerRequest *request) {
    if (request->authenticate(loginID, loginPass)) {
      resetBools();
      wifiJoin(request);
      while (WiFi.status() != WL_CONNECTED) {
        delay(10);
      }
      request->send(200, "text/plain", "GAKKI BLESS");
    } else {
      request->redirect("/login");
    }
  });

  aSyncServer.on("/ota", [](AsyncWebServerRequest *request) {
    if (request->authenticate(loginID, loginPass)) {
      resetBools();
      request->send(SPIFFS, "/ota.html", String(), false, otaConfirmation);
    } else {
      request->redirect("/login");
    }
  });

  aSyncServer.on("/otaStart", [](AsyncWebServerRequest *request) {
    if (request->authenticate(loginID, loginPass)) {
      resetBools();
      startOTA = true;
      request->send(200, "text/plain", Update.hasError()? "OTA FAILED" : "OTA STARTED");
      aSyncServer.reset();
    } else {
      request->redirect("/login");
    }
  });

  aSyncServer.on("/reset", [](AsyncWebServerRequest *request) {
    if (request->authenticate(loginID, loginPass)) {
      resetBools();
      request->send(SPIFFS, "/reset.html", String(), false, resetConfirmation);
    } else {
      request->redirect("/login");
    }
  });

  aSyncServer.on("/resetStart", [](AsyncWebServerRequest *request) {
    if (request->authenticate(loginID, loginPass)) {
      resetBools();
      startOTA = true;
      startReset = true;
      request->send(200, "text/plain", Update.hasError()? "RESET FAILED" : "RESET STARTED");
      aSyncServer.reset();
    } else {
      request->redirect("/login");
    }
  });
}

/**
 * Handles all ajax calls
 */
void ajaxCalls() {
  aSyncServer.on("/setSpeed", [](AsyncWebServerRequest *request) {
    if (request->authenticate(loginID, loginPass)) {
      speed = atoi(request->arg("params").c_str());
      request->send(200);
    } else {
      request->redirect("/login");
    }
  });

  aSyncServer.on("/setMode", [](AsyncWebServerRequest *request) {
    if (request->authenticate(loginID, loginPass)) {
      resetBools();
      isAuto = String(request->arg("params")) == "auto";
      isStop = !isAuto;
      isForward = isAuto;
      request->send(200);
    } else {
      request->redirect("/login");
    }
  });

  aSyncServer.on("/stop", [](AsyncWebServerRequest *request) {
    if (request->authenticate(loginID, loginPass)) {
      resetBools();
      request->send(200);
    } else {
      request->redirect("/login");
    }
  });

  aSyncServer.on("/forward", [](AsyncWebServerRequest *request) {
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

  aSyncServer.on("/backward", [](AsyncWebServerRequest *request) {
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

  aSyncServer.on("/left", [](AsyncWebServerRequest *request) {
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

  aSyncServer.on("/right", [](AsyncWebServerRequest *request) {
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

  aSyncServer.on("/recall", [](AsyncWebServerRequest *request) {
    if (request->authenticate(loginID, loginPass)) {
      isRecall = isAuto; // Recall is only available in auto mode

      // if (isRecall) {
      //   // Stop the auto movement
      //   isForward = false;
      //   isBackward = false;
      // }

      request->send(200);
    } else {
      request->redirect("/login");
    }
  });
}

/**
 * Resets the booleans and stops all motor when the user navigate to another page
 */
void resetBools() {
  startOTA = false;
  startReset = false;
  isAuto = false;
  isRecall = false;
  isStop = true;
  isForward = false;
  isBackward = false;
  isLeft = false;
  isRight = false;
  x = 0;
  y = 0;
  currentDirection = North;
}

/**
 * Template processor function for /wifi
 *
 * @param  var    The variable in wifi.html
 * @return String The HTML code for a list of WiFI access points
 */
String wifiList(const String& var) {
  String f = "";
  const char *checked = "checked";
  short n = WiFi.scanNetworks();
  Serial.println(WiFi.status() == WL_CONNECTED);
  dbg(netDBG, "Scan done: ");

  if (n == 0) {
    dln(netDBG, "No networks found.");
    f += "No WiFi access points found";
         "<a href='/'>Back</a><br/><a href='/wifi'>Try again?</a></p>\n";
  } else {
    dbg(netDBG, n); dln(netDBG, " networks found");
    if (WiFi.SSID() == "")  {
      f += "<p>WiFi access points available:</p>";
    } else {
      f += "<p>Currently connected to <strong>" + WiFi.SSID() + "</strong>. "
           "WiFi access points available:</p>";
    }
    f += "<form method='POST' action='wifiJoin'>"
         "<table id='ap' class='table table-striped table-hover'>";

    for(short i = 0; i < n; ++i) {
      f.concat("<tr><td class='text-center'>");
      f.concat("<label class='form-radio'><input type='radio' name='ssid' value='");
      f.concat(WiFi.SSID(i));
      f.concat("'");
      f.concat(checked);
      f.concat("><i class='form-icon'></i></label></td><td>");
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

/**
 * Joins the WiFi network selected
 *
 * @param request AsyncWebServerRequest pointer
 */
void wifiJoin(AsyncWebServerRequest *request) {
  String ssid = "", key = "";
  
  if (request->hasArg("ssid")) {
    ssid = request->arg("ssid");
  } else if (request->hasArg("hidden")) {
    ssid = request->arg("hidden");
  }

  key = request->arg("key");

  // for (uint8_t i = 0; i < request->params(); i++) {
  //   AsyncWebParameter* p = request->getParam(i);
  //   if (p->name() == "ssid") {
  //     ssid = p->value();
  //   } else if (p->name() == "hidden" && ssid == "") {
  //     ssid = p->value(); // hidden network is always the last option
  //   } else if (p->name() == "key") {
  //     key = p->value();
  //   }
  // }

  // WiFi.disconnect();
  const char* c = ssid.c_str();
  const char* d = key.c_str();
  WiFi.begin(c, d);
}

/**
 * Template processor function for /
 *
 * @param  var    The variable in index.html
 * @return String The HTML code for status table
 */
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
      s += "WL_UNKNOWN";
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
    s += "No Connection";
  }

  s += "</td></tr><tr><th>Total Distance Moved (cm)</th><td>";
  s += String(distanceMoved);

  aread = analogRead(batteryPin);    // get a reading from the adc pin connected to battery
  avolt = (aread/4095)*2.2*2;

  s += "</td></tr><tr><th>Battery Voltage (V)</th><td>";
  s += String(avolt) + "</td></tr></table>";

  http.end(); // Free resource
  return s;
}

/**
 * Template processor function for /ota
 *
 * @param  var    The variable in ota.html
 * @return String The HTML code for OTA confirmation
 */
String otaConfirmation(const String& var) {
  String message;
  respCode = doCloudGet(&http, gitID, "version");
  if (respCode != 200) {
    message = "<p class='text-center'>No internet connection.</p>";
  } else if (currentVersion >= highestAvailableVersion) {
    message = "<p class='text-center'>No updates available.</p>";
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

/**
 * Template processor function for /reset
 *
 * @param  var    The variable in reset.html
 * @return String The HTML codew for reset confirmation
 */
String resetConfirmation(const String& var) {
  String message;
  startReset = true;
  respCode = doCloudGet(&http, gitID, "1.bin");
  if (respCode != 200) {
    message = "<p class='text-center'>No internet connection.</p>";
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

/**
 * Utility for printing IP addresses
 *
 * @param  address The IPAddress instance
 * @return String  The IP address in String form
 */
String ip2str(IPAddress address) {
  return String(address[0]) + "." + String(address[1]) + "." +
         String(address[2]) + "." + String(address[3]);
}