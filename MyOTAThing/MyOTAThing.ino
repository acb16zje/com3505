/////////////////////////////////////////////////////////////////////////////
// MyOTAThing.ino
// COM3505 lab assessment: Over-The-Air update template; ADD YOUR CODE HERE!
/////////////////////////////////////////////////////////////////////////////

// the WiFi and HTTP server libraries ///////////////////////////////////////
#include <WiFi.h>         // WiFi
#include <ESPWebServer.h> // simple webserver
#include <HTTPClient.h>   // ESP32 library for making HTTP requests
#include <Update.h>       // OTA update library

// OTA stuff ////////////////////////////////////////////////////////////////
int doCloudGet(HTTPClient *, String, String); // helper for downloading 'ware
void doOTAUpdate();                           // main OTA logic
int currentVersion = 1;                 // used to check for updates
const String gitID = "Juneezee";              // team's git ID
const int minSize = 100000;                   // 100k bytes
const int maxSize = 1000000;                  // 1 mega bytes

// MAC and IP helpers ///////////////////////////////////////////////////////
char MAC_ADDRESS[13]; // MAC addresses are 12 chars, plus the NULL terminator
void getMAC(char *);
String ip2str(IPAddress);                 // helper for printing IP addresses

// LED utilities, loop slicing //////////////////////////////////////////////
void ledOn();
void ledOff();
void blink(int = 1, int = 300);
int loopIteration = 0;

// SETUP: initialisation entry point ////////////////////////////////////////
void setup() {
  Serial.begin(115200);         // initialise the serial line
  getMAC(MAC_ADDRESS);          // store the MAC address
  Serial.printf("\nMyOTAThing setup...\nESP32 MAC = %s\n", MAC_ADDRESS);
  Serial.printf("Firmware is at version %d\n", currentVersion);

  // get on the network
  WiFi.begin("uos-other", "shefotherkey05");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi.");

  // check for and perform firmware updates as needed
  doOTAUpdate();
}

// LOOP: task entry point ///////////////////////////////////////////////////
void loop() {
  // int sliceSize = 500000;
  // loopIteration++;
  // if (loopIteration % sliceSize == 0) // a slice every sliceSize iterations
  //   Serial.println("OTA loop");

  // do other useful stuff here...?
  Serial.println("\n\n");
  doOTAUpdate();
  delay(5000);
}

// OTA over-the-air update stuff ///////////////////////////////////////////
void doOTAUpdate() {             // the main OTA logic
  // materials for doing an HTTP GET on github from the BinFiles/ dir
  HTTPClient http; // manage the HTTP request process
  int respCode;    // the response code from the request (e.g. 404, 200, ...)
  int highestAvailableVersion = -1;  // version of latest firmware on server

  // do a GET to read the version file from the cloud
  Serial.println("Checking for firmware updates...");
  respCode = doCloudGet(&http, gitID, "version");
  if (respCode == 200) // check response code (-ve on failure)
    highestAvailableVersion = atoi(http.getString().c_str());
  else
    Serial.printf("Couldn't get version! rtn code: %d\n", respCode);
  http.end(); // free resources

  // do we know the latest version, and does the firmware need updating?
  if (respCode != 200) {
    Serial.printf("Cannot update\n\n");
    return;
  } else if (currentVersion >= highestAvailableVersion) {
    Serial.printf("Firmware is up to date\n\n");
    return;
  }

  // ok, we need to do a firmware update...
  Serial.printf(
    "Upgrading firmware from version %d to version %d\n",
    currentVersion, highestAvailableVersion
  );

  // do a GET for the .bin
  /* TODO
  Your main OTA update logic goes here. Use doCloudGet to do an HTTP GET for
  the .bin file, and check that the length of the update is appropriate (via
  HTTPClient's getSize() method. (You might note that a minimal sketch takes
  more than 100k bytes, so perhaps that is a good length to check for. Check
  the response code, and if all is well call Update.begin (which takes the
  size of the update as parameter) and Update.writeStream (which takes a
  Stream that you can get from HTTPClient's getStream() method). Other methods
  needed are Update.end, Update.isFinished and Update.getError. When an update
  has been performed correctly, you can restart the device via ESP.restart().
  */

  Serial.println("Retrieving update file...");

  // Get the response code of the HTTP
  int binRespCode = doCloudGet(
    &http, gitID, String(highestAvailableVersion) + ".bin"
  );

  // The size of the bin file
  int fileSize = http.getSize();

  // Perform the update
  if (binRespCode == 200 && fileSize >= minSize && fileSize <= maxSize) {
    Serial.printf("Received bin file of size %d bytes.\n", fileSize);

    bool canBegin = Update.begin(fileSize);

    if (canBegin) {
      Serial.println("Performing OTA... Please do not turn off the ESP32...");

      // Start writing to the flash
      size_t written = Update.writeStream(http.getStream());

      // Check if finished writing
      if (written == fileSize) {
        Serial.println("Written : " + String(written) + " successfully");
      } else {
        Serial.println(
          "Written only : " + String(written) + "/" + String(fileSize) + ". Retry?"
        );
      }

      // Final process
      if (Update.end()) {
        Serial.println("OTA finished.");

        if (Update.isFinished()) {
          Serial.println("Update successfully completed. Rebooting.");

          currentVersion = highestAvailableVersion;
          // Restarting the device will loop the update
          // ESP.restart();
        } else {
          Serial.println("Update unsuccessful. Unknown error occurred.");
        }
      } else {
        Serial.println("Error Occurred. Error #: " + String(Update.getError()));
      }

      // Free resource
      http.end();
    } else {
      Serial.println("Not enough memory space to perform the update.");
    }
  } else {
    Serial.println("An error occurred when retrieving the bin file.");
  }
}

// helper for downloading from cloud firmware server via HTTP GET
int doCloudGet(HTTPClient *http, String gitID, String fileName) {
  // build up URL from components; for example:
  // http://com3505.gate.ac.uk/repos/com3505-labs-2018-adalovelace/BinFiles/2.bin
  String baseUrl =
    "http://com3505.gate.ac.uk/repos/";
  String url =
    baseUrl + "com3505-labs-2018-" + gitID + "/BinFiles/" + fileName;

  // make GET request and return the response code
  http->begin(url);
  http->addHeader("User-Agent", "ESP32");
  return http->GET();
}

// Misc utilities ///////////////////////////////////////////////////////////
// get the ESP's MAC address
void getMAC(char *buf) { // the MAC is 6 bytes, so needs careful conversion...
  uint64_t mac = ESP.getEfuseMac(); // ...to string (high 2, low 4):
  char rev[13];
  sprintf(rev, "%04X%08X", (uint16_t) (mac >> 32), (uint32_t) mac);

  // the byte order in the ESP has to be reversed relative to normal Arduino
  for(int i=0, j=11; i<=10; i+=2, j-=2) {
    buf[i] = rev[j - 1];
    buf[i + 1] = rev[j];
  }
  buf[12] = '\0';
}

// LED blinkers
void ledOn()  { digitalWrite(BUILTIN_LED, HIGH); }
void ledOff() { digitalWrite(BUILTIN_LED, LOW); }
void blink(int times, int pause) {
  ledOff();
  for(int i=0; i<times; i++) {
    ledOn(); delay(pause); ledOff(); delay(pause);
  }
}

// utility for printing IP addresses
String ip2str(IPAddress address) {
  return
    String(address[0]) + "." + String(address[1]) + "." +
    String(address[2]) + "." + String(address[3]);
}
