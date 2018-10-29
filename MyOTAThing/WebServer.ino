/////////////////////////////////////////////////////////////////////////////
// WebServer.ino
// Generate all pages and handle all requests
/////////////////////////////////////////////////////////////////////////////

#include "MyOTAThing.h"

void startWebServer() {
  apSSID.concat(MAC_ADDRESS);

  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(apSSID.c_str(), apPass.c_str());

  start_ota = false;
  routes();
  WiFi.begin("uos-other", "shefotherkey05"); // Connect to uos-other by default
}

void routes() {
  // register routes to handle different paths
  webServer.onNotFound(hndlNotFound);       // 404s...
  webServer.on("/", hndlRoot);              // root (status page)
  webServer.on("/generate_204", hndlRoot);  // Android captive portal support
  webServer.on("/L2", hndlRoot);            // ...iOS captive portal...
  webServer.on("/wifi", hndlWifi);          // page for choosing an AP
  webServer.on("/wifichz", hndlWifichz);    // landing page for AP form submit
  webServer.on("/ota", hndlOTA);            // OTA firmware upgrade

  webServer.begin();
  dln(setupDBG, "HTTP server started");
}

// webserver handler callbacks
void hndlNotFound() {
  dbg(netDBG, "URI Not Found: ");
  dln(netDBG, webServer.uri());
  webServer.send(200, "text/plain", "URI Not Found");
}

void hndlRoot() {         // UI for checking connectivity etc.
  dln(netDBG, "serving page at /");

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
  s += "</td></tr><tr><td>Newest Version</td><td>";

  respCode = doCloudGet(&http, gitID, "version");
  if (respCode == 200) {
    highestAvailableVersion = atoi(http.getString().c_str());

    s += String(highestAvailableVersion);

    if (currentVersion < highestAvailableVersion) {
      s += " <a href='/ota' class='update'>Update</a></td></tr>";
    }

    s += "</td></tr></table>";

  } else {
    s += "No connection</td></tr></table>";
  }

  http.end(); // Free resource

  replacement_t repls[] = { // the elements to replace in the boilerplate
    { 1, apSSID.c_str() },
    { 3, "<h2>Status</h2>\n" },
    { 4, s.c_str() },
  };
  String htmlPage = ""; // a String to hold the resultant page
  GET_HTML(htmlPage, templatePage, repls);

  webServer.send(200, "text/html", htmlPage);
  start_ota = false;
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
  start_ota = false;
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

void hndlOTA() {
  dln(netDBG, "serving page at /ota");

  String title = "<h2>OTA Update</h2>";
  String message;
  if ( currentVersion >= highestAvailableVersion ) {
    message = "<p>There are no updates available.</p>";
  } else {
    message = "<p>Please press button to start update or return to cancel.</p>";
    start_ota = true;
  }

  replacement_t repls[] = { // the elements to replace in the template
    { 3, title.c_str() },
    { 4, message.c_str() },
  };

  String htmlPage = "";     // a String to hold the resultant page
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
      f.concat("</tr>\n");
      checked = "";
    }
    f += "</table><br/>Pass key: <input type='textarea' name='key'><br/><br/> ";
    f += "<input type='submit' value='Submit'></form>";
  }
}
