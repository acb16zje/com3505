/////////////////////////////////////////////////////////////////////////////
// MyOTAThing.ino
// COM3505 lab assessment: Over-The-Air update
/////////////////////////////////////////////////////////////////////////////

#include "Misc.h"                   // WiFi, MAC, LED...
#include "WebServer.h"              // WebServer header file
#include "MyOTAThing.h"

// SETUP: initialisation entry point ////////////////////////////////////////
void setup() {
  Serial.begin(115200);                       // initialise the serial line
  pinMode(BUILTIN_LED, OUTPUT);
  pinMode(pushButton, INPUT_PULLUP);
  getMAC(MAC_ADDRESS);                        // store the MAC address
  Serial.printf("\nMyOTAThing setup...\nESP32 MAC = %s\n", MAC_ADDRESS);
  Serial.printf("Firmware is at version %d\n", currentVersion);

  startWiFi();
  // startWebServer();

  doOTAUpdate(); // Check for and perform firmware updates as needed
}

// LOOP: task entry point ///////////////////////////////////////////////////
void loop() {
  // int sliceSize = 500000;
  // loopIteration++;
  // if (loopIteration % sliceSize == 0) // a slice every sliceSize iterations
  //   dln(loopDBG, "OTA loop");

  // webServer.handleClient();

  // Check for firmware updates every 10 seconds
  dln(loopDBG, "\n");
  doOTAUpdate();
  delay(10000);
}

// OTA over-the-air update stuff ///////////////////////////////////////////
void doOTAUpdate() {             // the main OTA logic
  // materials for doing an HTTP GET on github from the BinFiles/ dir
  HTTPClient http; // manage the HTTP request process
  int respCode;    // the response code from the request (e.g. 404, 200, ...)
  int highestAvailableVersion = -1;  // version of latest firmware on server

  // do a GET to read the version file from the cloud
  dln(monitDBG, "Checking for firmware updates...");
  respCode = doCloudGet(&http, gitID, "version");
  if (respCode == 200) // check response code (-ve on failure)
    highestAvailableVersion = atoi(http.getString().c_str());
  else
    Serial.printf("Couldn't get version! rtn code: %d\n", respCode);
  http.end(); // free resources

  // do we know the latest version, and does the firmware need updating?
  if (respCode != 200) {
    dln(monitDBG, "Cannot update\n\n");
    setup();
  } else if (currentVersion >= highestAvailableVersion) {
    dln(monitDBG, "Firmware is up to date\n\n");
    return;
  }

  // Newer firmware available
  Serial.printf("Upgrade firmware from version %d to version %d?\n",
    currentVersion, highestAvailableVersion
  );

  int buttonState = digitalRead(pushButton);

  while (buttonState == HIGH) {
    buttonState = digitalRead(pushButton);
  }

  // Do a GET for the .bin
  dln(monitDBG, "Retrieving update file...");

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
      dln(monitDBG, "Performing OTA... Please do not turn off the ESP32...");

      ledOn(); // Start the LED when it is flashing

      // Start writing to the flash
      size_t written = Update.writeStream(http.getStream());

      ledOff(); // Turn off LED when finished flashing

      // Check if finished writing
      if (written == fileSize) {
        dln(monitDBG, "Written : " + String(written) + " successfully");
      } else {
        dln(monitDBG,
          "Written only : " +
            String(written) + "/" + String(fileSize) + ". Retry?"
        );
      }

      // Final process
      if (Update.end()) {
        dln(monitDBG, "OTA finished.");
        blink();

        if (Update.isFinished()) {
          dln(monitDBG, "Update completed successfully.");

          currentVersion = highestAvailableVersion;

          // ESP.restart(); // Restarting the device will loop the update
        } else {
          dln(monitDBG, "Update failed. Unknown error occurred.");
        }
      } else {
        dln(monitDBG, "Error Occurred. Error #: " + String(Update.getError()));
      }

      http.end(); // Free resource
    } else {
      dln(monitDBG, "Not enough memory space to perform the update.");
    }
  } else {
    dln(monitDBG, "An error occurred when retrieving the bin file.");
    blink();
  }
}

// Helper for downloading from cloud firmware server via HTTP GET
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
