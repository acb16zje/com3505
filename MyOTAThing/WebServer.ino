/////////////////////////////////////////////////////////////////////////////
// WebServer.ino
// Generate all pages and handle all requests
/////////////////////////////////////////////////////////////////////////////

void startWebServer() {
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(apSSID.c_str(), apPass.c_str());

  routes();
  WiFi.begin("uos-other", "shefotherkey05"); // Connect to uos-other by default
}

void routes() {
  // register routes to handle different paths
  webServer.onNotFound(hndlNotFound);       // 404s...
  webServer.on("/", hndlRoot);              // root
  webServer.on("/generate_204", hndlRoot);  // Android captive portal support
  webServer.on("/L0", hndlRoot);            // TODO is this...
  webServer.on("/L2", hndlRoot);            // ...iOS captive portal...
  webServer.on("/wifi", hndlWifi);          // page for choosing an AP
  webServer.on("/wifichz", hndlWifichz);    // landing page for AP form submit
  webServer.on("/status", hndlStatus);      // status check, e.g. IP address

  webServer.begin();
  dln(setupDBG, "HTTP server started");
}

// webserver handler callbacks
void hndlNotFound() {
  dbg(netDBG, "URI Not Found: ");
  dln(netDBG, webServer.uri());
  webServer.send(200, "text/plain", "URI Not Found");
}
void hndlRoot() {
  dln(netDBG, "serving page notionally at /");
  replacement_t repls[] = { // the elements to replace in the boilerplate
    {  1, apSSID.c_str() },
    {  3, "<p>Choose a <a href=\"wifi\">wifi access point</a>.</p>" },
    {  4, "<p>Check <a href='/status'>wifi status</a>.</p>" },
  };
  String htmlPage = ""; // a String to hold the resultant page
  GET_HTML(htmlPage, templatePage, repls);
  htmlPage += "<h1>今日山の日なんだよ、祝日だよ！</h1>\n";
  webServer.send(200, "text/html", htmlPage);
}
void hndlWifi() {
  dln(netDBG, "serving page at /wifi");

  String form = ""; // a form for choosing an access point and entering key
  apListForm(form);
  replacement_t repls[] = { // the elements to replace in the boilerplate
    { 1, apSSID.c_str() },
    { 3, "<h2>Network configuration</h2>\n" },
    { 4, form.c_str() },
  };
  String htmlPage = ""; // a String to hold the resultant page
  GET_HTML(htmlPage, templatePage, repls);

  webServer.send(200, "text/html", htmlPage);
}
void hndlWifichz() {
  dln(netDBG, "serving page at /wifichz");

  String title = "<h2>Joining wifi network...</h2>";
  String message = "<p>Check <a href='/status'>wifi status</a>.</p>";

  String ssid = "";
  String key = "";
  for(uint8_t i = 0; i < webServer.args(); i++ ) {
    if(webServer.argName(i) == "ssid")
      ssid = webServer.arg(i);
    else if(webServer.argName(i) == "key")
      key = webServer.arg(i);
  }

  if(ssid == "") {
    message = "<h2>Ooops, no SSID...?</h2>\n<p>Looks like a bug :-(</p>";
  } else {
    char ssidchars[ssid.length()+1];
    char keychars[key.length()+1];
    ssid.toCharArray(ssidchars, ssid.length()+1);
    key.toCharArray(keychars, key.length()+1);
    WiFi.begin(ssidchars, keychars);
  }

  replacement_t repls[] = { // the elements to replace in the template
    { 1, apSSID.c_str() },
    { 3, title.c_str() },
    { 4, message.c_str() },
  };
  String htmlPage = "";     // a String to hold the resultant page
  GET_HTML(htmlPage, templatePage, repls);

  webServer.send(200, "text/html", htmlPage);
}
void hndlStatus() {         // UI for checking connectivity etc.
  dln(netDBG, "serving page at /status");

  String s = "";
  s += "<ul>\n";
  s += "\n<li>SSID: ";
  s += WiFi.SSID();
  s += "</li>";
  s += "\n<li>Status: ";
  switch(WiFi.status()) {
    case WL_IDLE_STATUS:
      s += "WL_IDLE_STATUS</li>"; break;
    case WL_NO_SSID_AVAIL:
      s += "WL_NO_SSID_AVAIL</li>"; break;
    case WL_SCAN_COMPLETED:
      s += "WL_SCAN_COMPLETED</li>"; break;
    case WL_CONNECTED:
      s += "WL_CONNECTED</li>"; break;
    case WL_CONNECT_FAILED:
      s += "WL_CONNECT_FAILED</li>"; break;
    case WL_CONNECTION_LOST:
      s += "WL_CONNECTION_LOST</li>"; break;
    case WL_DISCONNECTED:
      s += "WL_DISCONNECTED</li>"; break;
    default:
      s += "unknown</li>";
  }

  s += "\n<li>Local IP: ";     s += ip2str(WiFi.localIP());
  s += "</li>\n";
  s += "\n<li>Soft AP IP: ";   s += ip2str(WiFi.softAPIP());
  s += "</li>\n";
  s += "\n<li>AP SSID name: "; s += apSSID;
  s += "</li>\n";

  s += "</ul></p>";

  replacement_t repls[] = { // the elements to replace in the boilerplate
    { 1, apSSID.c_str() },
    { 3, "<h2>Status</h2>\n" },
    { 4, s.c_str() },
  };
  String htmlPage = ""; // a String to hold the resultant page
  GET_HTML(htmlPage, templatePage, repls);

  webServer.send(200, "text/html", htmlPage);
}

void apListForm(String& f) { // utility to create a form for choosing AP
  const char *checked = " checked";
  int n = WiFi.scanNetworks();
  dbg(netDBG, "scan done: ");

  if(n == 0) {
    dln(netDBG, "no networks found");
    f += "No wifi access points found :-( ";
    f += "<a href='/'>Back</a><br/><a href='/wifi'>Try again?</a></p>\n";
  } else {
    dbg(netDBG, n); dln(netDBG, " networks found");
    f += "<p>Wifi access points available:</p>\n"
         "<p><form method='POST' action='wifichz'> ";
    for(int i = 0; i < n; ++i) {
      f.concat("<input type='radio' name='ssid' value='");
      f.concat(WiFi.SSID(i));
      f.concat("'");
      f.concat(checked);
      f.concat(">");
      f.concat(WiFi.SSID(i));
      f.concat(" (");
      f.concat(WiFi.RSSI(i));
      f.concat(" dBm)");
      f.concat("<br/>\n");
      checked = "";
    }
    f += "<br/>Pass key: <input type='textarea' name='key'><br/><br/> ";
    f += "<input type='submit' value='Submit'></form></p>";
  }
}
