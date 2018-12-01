/////////////////////////////////////////////////////////////////////////////
// WebServer.ino
// Generate all pages and handle all requests
/////////////////////////////////////////////////////////////////////////////

#define DBG_OUTPUT_PORT Serial

const char* ssid = "**********";
const char* password = "**********";
const char* host = "esp32sd";

static bool hasSD = false;
File uploadFile;

void returnOK() {
  webServer.send(200, "text/plain", "");
}

void returnFail(String msg) {
  webServer.send(500, "text/plain", msg + "\r\n");
}

bool loadFromSdCard(String path) {
  String dataType = "text/plain";
  if (path.endsWith("/")) {
    path += "index.htm";
  }

  if (path.endsWith(".src")) {
    path = path.substring(0, path.lastIndexOf("."));
  } else if (path.endsWith(".htm")) {
    dataType = "text/html";
  } else if (path.endsWith(".css")) {
    dataType = "text/css";
  } else if (path.endsWith(".js")) {
    dataType = "application/javascript";
  } else if (path.endsWith(".png")) {
    dataType = "image/png";
  } else if (path.endsWith(".gif")) {
    dataType = "image/gif";
  } else if (path.endsWith(".jpg")) {
    dataType = "image/jpeg";
  } else if (path.endsWith(".ico")) {
    dataType = "image/x-icon";
  } else if (path.endsWith(".xml")) {
    dataType = "text/xml";
  } else if (path.endsWith(".pdf")) {
    dataType = "application/pdf";
  } else if (path.endsWith(".zip")) {
    dataType = "application/zip";
  }

  File dataFile = SD.open(path.c_str());
  if (dataFile.isDirectory()) {
    path += "/index.htm";
    dataType = "text/html";
    dataFile = SD.open(path.c_str());
  }

  if (!dataFile) {
    return false;
  }

  if (webServer.hasArg("download")) {
    dataType = "application/octet-stream";
  }

  if (webServer.streamFile(dataFile, dataType) != dataFile.size()) {
    DBG_OUTPUT_PORT.println("Sent less data than expected!");
  }

  dataFile.close();
  return true;
}

void handleFileUpload() {
  if (webServer.uri() != "/edit") {
    return;
  }
  HTTPUpload& upload = webServer.upload();
  if (upload.status == UPLOAD_FILE_START) {
    if (SD.exists((char *)upload.filename.c_str())) {
      SD.remove((char *)upload.filename.c_str());
    }
    uploadFile = SD.open(upload.filename.c_str(), FILE_WRITE);
    DBG_OUTPUT_PORT.print("Upload: START, filename: "); DBG_OUTPUT_PORT.println(upload.filename);
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    if (uploadFile) {
      uploadFile.write(upload.buf, upload.currentSize);
    }
    DBG_OUTPUT_PORT.print("Upload: WRITE, Bytes: "); DBG_OUTPUT_PORT.println(upload.currentSize);
  } else if (upload.status == UPLOAD_FILE_END) {
    if (uploadFile) {
      uploadFile.close();
    }
    DBG_OUTPUT_PORT.print("Upload: END, Size: "); DBG_OUTPUT_PORT.println(upload.totalSize);
  }
}

void printIndex() {
   if (!webServer.hasArg("dir")) {
    return returnFail("BAD ARGS");
  }
  String path = webServer.arg("dir");
  if (path != "/" && !SD.exists((char *)path.c_str())) {
    return returnFail("BAD PATH");
  }
  File dir = SD.open((char *)path.c_str());
  path = String();
  if (!dir.isDirectory()) {
    dir.close();
    return returnFail("NOT DIR");
  }
  dir.rewindDirectory();
  webServer.setContentLength(CONTENT_LENGTH_UNKNOWN);
  webServer.send(200, "text/json", "");
  WiFiClient client = webServer.client();
  webServer.sendContent("[");
  for (int cnt = 0; true; ++cnt) {
    File entry = dir.openNextFile();
    if (!entry) {
      break;
    }

    String output;
    if (cnt > 0) {
      output = ',';
    }

    output += "{\"type\":\"";
    output += (entry.isDirectory()) ? "dir" : "file";
    output += "\",\"name\":\"";
    output += entry.name();
    output += "\"";
    output += "}";
    webServer.sendContent(output);
    entry.close();
  }
  webServer.sendContent("]");
  dir.close();
}

void handleNotFound() {
  if (hasSD && loadFromSdCard(webServer.uri())) {
    return;
  }
  String message = "SDCARD Not Detected\n\n";
  message += "URI: ";
  message += webServer.uri();
  message += "\nMethod: ";
  message += (webServer.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += webServer.args();
  message += "\n";
  for (uint8_t i = 0; i < webServer.args(); i++) {
    message += " NAME:" + webServer.argName(i) + "\n VALUE:" + webServer.arg(i) + "\n";
  }
  webServer.send(404, "text/plain", message);
  DBG_OUTPUT_PORT.print(message);
}

void startSDWebServer() {
  WiFi.mode(WIFI_AP_STA);

  // apSSID = Gakki-bot
  // apPass = gakkismile
  WiFi.softAP(apSSID.c_str(), apPass.c_str());

  webServer.on("/", HTTP_GET, printIndex);
  webServer.onNotFound(handleNotFound);

  webServer.begin();
  Serial.println(WiFi.softAPIP());
  DBG_OUTPUT_PORT.println("HTTP webServer started");

  if (SD.begin(SS)) {
    DBG_OUTPUT_PORT.println("SD Card initialized.");
    hasSD = true;
  }
}