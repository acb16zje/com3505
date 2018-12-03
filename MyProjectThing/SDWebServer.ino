/////////////////////////////////////////////////////////////////////////////
// WebServer.ino
// Generate all pages and handle all requests
/////////////////////////////////////////////////////////////////////////////

// #define DBG_OUTPUT_PORT Serial

// static bool hasSD = false;
// File uploadFile;

// bool loadFromSdCard(String path) {
//   IOExpander::digitalWrite(IOExpander::SD_CS, LOW);
//   String dataType = "text/plain";
//   if (path.endsWith("/")) {
//     path += "demo.htm";
//   }

//   if (path.endsWith(".src")) {
//     path = path.substring(0, path.lastIndexOf("."));
//   } else if (path.endsWith(".htm")) {
//     dataType = "text/html";
//   } else if (path.endsWith(".css")) {
//     dataType = "text/css";
//   } else if (path.endsWith(".js")) {
//     dataType = "application/javascript";
//   } else if (path.endsWith(".png")) {
//     dataType = "image/png";
//   } else if (path.endsWith(".gif")) {
//     dataType = "image/gif";
//   } else if (path.endsWith(".jpg")) {
//     dataType = "image/jpeg";
//   } else if (path.endsWith(".ico")) {
//     dataType = "image/x-icon";
//   }


//   File dataFile = SD.open(path.c_str());
//   if (dataFile.isDirectory()) {
//     path += "/index.htm";
//     dataType = "text/html";
//     dataFile = SD.open(path.c_str());
//   }

//   if (!dataFile) {
//     return false;
//   }

//   if (webServer.hasArg("download")) {
//     dataType = "application/octet-stream";
//   }

//   if (webServer.streamFile(dataFile, dataType) != dataFile.size()) {
//     DBG_OUTPUT_PORT.println("Sent less data than expected!");
//   }

//   dataFile.close();
//   IOExpander::digitalWrite(IOExpander::SD_CS, HIGH);
//   return true;
// }

// void printIndex() {
//     IOExpander::digitalWrite(IOExpander::SD_CS, LOW);
//   if (!webServer.hasArg("dir")) {
//     return returnFail("BAD ARGS");
//   }
//   String path = webServer.arg("dir");
// //   Serial.println(path);
// //   if (path != "/" && !SD.exists((char *)path.c_str())) {
// //     return returnFail("BAD PATH");
// //   }
//   File dir = SD.open((char *)path.c_str());
//   path = String();
// //   if (!dir.isDirectory()) {
// //     dir.close();
// //     return returnFail("NOT DIR");
// //   }
//   dir.rewindDirectory();
//   webServer.setContentLength(CONTENT_LENGTH_UNKNOWN);

//   WiFiClient client = webServer.client();

//   String content;

//    while(dir.available()){
//         content += char(dir.read());
//     }

//   webServer.send(200, "text/html", content);
// //   webServer.sendContent("[");
// //   for (int cnt = 0; true; ++cnt) {
// //     File entry = dir.openNextFile();
// //     if (!entry) {
// //       break;
// //     }

// //     String output;
// //     if (cnt > 0) {
// //       output = ',';
// //     }

// //     output += "{\"type\":\"";
// //     output += (entry.isDirectory()) ? "dir" : "file";
// //     output += "\",\"name\":\"";
// //     output += entry.name();
// //     output += "\"";
// //     output += "}";
// //     webServer.sendContent(output);
// //     entry.close();
// //   }
// //   webServer.sendContent("]");
//   dir.close();
//   IOExpander::digitalWrite(IOExpander::SD_CS, HIGH);
// }

// void handleNotFound() {
//   if (hasSD && loadFromSdCard(webServer.uri())) {
//     return;
//   }
//   String message = "SDCARD Not Detected\n\n";
//   message += "URI: ";
//   message += webServer.uri();
//   message += "\nMethod: ";
//   message += (webServer.method() == HTTP_GET) ? "GET" : "POST";
//   message += "\nArguments: ";
//   message += webServer.args();
//   message += "\n";
//   for (uint8_t i = 0; i < webServer.args(); i++) {
//     message += " NAME:" + webServer.argName(i) + "\n VALUE:" + webServer.arg(i) + "\n";
//   }
//   webServer.send(404, "text/plain", message);
//   DBG_OUTPUT_PORT.print(message);
// }

// void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
//     Serial.printf("Listing directory: %s\n", dirname);

//     File root = fs.open(dirname);
//     if(!root){
//         Serial.println("Failed to open directory");
//         return;
//     }
//     if(!root.isDirectory()){
//         Serial.println("Not a directory");
//         return;
//     }

//     File file = root.openNextFile();
//     while(file){
//         if(file.isDirectory()){
//             Serial.print("  DIR : ");
//             Serial.println(file.name());
//             if(levels){
//                 listDir(fs, file.name(), levels -1);
//             }
//         } else {
//             Serial.print("  FILE: ");
//             Serial.print(file.name());
//             Serial.print("  SIZE: ");
//             Serial.println(file.size());
//         }
//         file = root.openNextFile();
//     }
// }

// void readFile(fs::FS &fs, const char * path){
//     Serial.printf("Reading file: %s\n", path);

//     File file = fs.open(path);
//     if(!file){
//         Serial.println("Failed to open file for reading");
//         return;
//     }

//     Serial.print("Read from file: ");
//     while(file.available()){
//         Serial.write(file.read());
//     }
//     file.close();
// }

// void writeFile(fs::FS &fs, const char * path, const char * message){
//     Serial.printf("Writing file: %s\n", path);

//     File file = fs.open(path, FILE_WRITE);
//     if(!file){
//         Serial.println("Failed to open file for writing");
//         return;
//     }
//     if(file.print(message)){
//         Serial.println("File written");
//     } else {
//         Serial.println("Write failed");
//     }
//     file.close();
// }

// void deleteFile(fs::FS &fs, const char * path){
//     Serial.printf("Deleting file: %s\n", path);
//     if(fs.remove(path)){
//         Serial.println("File deleted");
//     } else {
//         Serial.println("Delete failed");
//     }
// }

void startSDWebServer() {
  WiFi.mode(WIFI_AP_STA);

  // apSSID = Gakki-bot
  // apPass = gakkismile
  WiFi.softAP(apSSID.c_str(), apPass.c_str());
  wifiServer.begin();
  Serial.println(WiFi.softAPIP());

  // webServer.on("/", HTTP_GET, printIndex);
  // webServer.onNotFound(handleNotFound);

  // webServer.begin();
  // DBG_OUTPUT_PORT.println("HTTP webServer started");
}