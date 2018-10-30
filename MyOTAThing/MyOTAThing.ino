/////////////////////////////////////////////////////////////////////////////
// MyOTAThing.ino
// COM3505 lab assessment: Over-The-Air update
/////////////////////////////////////////////////////////////////////////////

#include "Misc.h"                    // WiFi, MAC, LED...
#include "WebServer.h"               // WebServer header file
#include "MyOTAThing.h"

// SETUP: initialisation entry point ////////////////////////////////////////
void setup() {
  Serial.begin(115200);                       // initialise the serial line
  pinMode(BUILTIN_LED, OUTPUT);
  pinMode(pushButton, INPUT_PULLUP);
  getMAC(MAC_ADDRESS);                        // store the MAC address
  Serial.printf("\nMyOTAThing setup...\nESP32 MAC = %s\n", MAC_ADDRESS);
  Serial.printf("Firmware is at version %d\n", currentVersion);

  startWebServer();

  // doOTAUpdate(); // Check for and perform firmware updates as needed
}

// LOOP: task entry point ///////////////////////////////////////////////////
void loop() {
  // Check for firmware updates every 10 seconds
  // int sliceSize = 500000;
  // loopIteration++;
  // if (loopIteration % sliceSize == 0) // a slice every sliceSize iterations
  //   dln(loopDBG, "OTA loop");

  // Update the firmware when OTA page is visted
  if (start_ota) {
    int buttonState = digitalRead(pushButton);
    blink(1, 100);

    // User confirmation
    if (buttonState == LOW) {
      dln(loopDBG, "\n");
      doOTAUpdate();
      delay(1000);
    }
  }
  ledOff();
  webServer.handleClient();
}

// OTA over-the-air update stuff ///////////////////////////////////////////
void doOTAUpdate() {             // the main OTA logic
  // materials for doing an HTTP GET on github from the BinFiles/ dir

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
    start_ota = false;
    return;
  }

  // Newer firmware available
  Serial.printf("Upgrade firmware from version %d to version %d\n",
    currentVersion, highestAvailableVersion
  );

  ledOff();
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
      dln(monitDBG, "Performing OTA... Please do not turn off the ESP32...");

      ledOn(); // Start the LED when it is flashing

      // Start writing to the flash
      // http.getStream() will not work for HTTPS
      size_t written = Update.writeStream(*http.getStreamPtr());

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

  start_ota = false; // Allow the user to update again
}

// Helper for downloading from cloud firmware server via HTTP GET
int doCloudGet(HTTPClient *http, String gitID, String fileName) {
  // build up URL from components; for example:
  //http://com3505.gate.ac.uk/repos/com3505-labs-2018-adalovelace/BinFiles/2.bin
  // String baseUrl =
  //   "http://com3505.gate.ac.uk/repos/";
  // String url =
  //   baseUrl + "com3505-labs-2018-" + gitID + "/BinFiles/" + fileName;

  // Fetch from GitHub directly
  String baseUrl = "https://" + token + "@raw.githubusercontent.com/";
  String url =
    baseUrl + "UniSheffieldInternetOfThings/com3505-labs-2018-" +
    gitID + "/master/BinFiles/" + fileName;

  // make GET request and return the response code
  http->begin(url);
  http->addHeader("User-Agent", "ESP32");
  return http->GET();
}
