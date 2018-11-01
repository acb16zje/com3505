/////////////////////////////////////////////////////////////////////////////
// Auth.ino
// Handling cookie and session for login
/////////////////////////////////////////////////////////////////////////////

// Check if header is present and correct
bool isAuthenticated() {
  dln(setupDBG, "Checking authentication status...");

  if (webServer.hasHeader("Cookie")) {
    String cookie = webServer.header("Cookie");
    dbg(setupDBG, "Found cookie: ");
    dln(setupDBG, cookie);

    if (cookie.indexOf("ESPSESSIONID=1") != -1) {
      dln(setupDBG, "Authentication Successful");
      return true;
    }
  }

  Serial.println("Authentication Failed");
  return false;
}

void setupAuth() {
  // Here the list of headers to be recorded
  const char* headerkeys[] = {"User-Agent", "Cookie"} ;
  headerkeyssize = sizeof(headerkeys) / sizeof(char*);

  // Ask webServer to track these headers
  webServer.collectHeaders(headerkeys, headerkeyssize);
}
