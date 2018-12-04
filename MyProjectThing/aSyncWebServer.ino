/////////////////////////////////////////////////////////////////////////////
// aSyncWebServer.ino
// Generate all pages and handle all requests
/////////////////////////////////////////////////////////////////////////////


void startWebServer() {
    if (!SPIFFS.begin()) {
        Serial.println("An Error has occured while mounting SPIFFS");
        return;
    }
    WiFi.mode(WIFI_AP_STA);

    WiFi.softAP(apSSID, apPass);

    routes();

    aSyncServer.begin();

    Serial.println(WiFi.softAPIP());
}

// String header() {
//     File file = SPIFFS.open("/head.html");
//     String content = "";

//     while(file.available()) {
//         content += char(file.read());
//     }

//     file.close();
//     return content;
// }
void routes() {
    aSyncServer.on("/", [](AsyncWebServerRequest *request) {
        request->send(SPIFFS,"/home.html","text/html");
    });
    aSyncServer.on("/style.css", [](AsyncWebServerRequest *request) {
        request->send(SPIFFS,"/style.css","text/css");
    });
    aSyncServer.on("/spectre.css", [](AsyncWebServerRequest *request) {
        request->send(SPIFFS,"/spectre.css","text/css");
    });
    aSyncServer.on("/spectre-icons.css", [](AsyncWebServerRequest *request) {
        request->send(SPIFFS,"/spectre-icons.css","text/css");
    });
    aSyncServer.on("/jquery.js", [](AsyncWebServerRequest *request) {
        request->send(SPIFFS,"/jquery.js","text/javascript");
    });
    aSyncServer.on("/script.js", [](AsyncWebServerRequest *request) {
        request->send(SPIFFS,"/script.js","text/javascript");
    });
    aSyncServer.on("/aREST.js", [](AsyncWebServerRequest *request) {
        request->send(SPIFFS,"/aREST.js","text/javascript");
    });    
}
