/////////////////////////////////////////////////////////////////////////////
// MyOTAThing.ino
// COM3505 lab assessment: Over-The-Air update
/////////////////////////////////////////////////////////////////////////////

#include "Auth.h"                    // Login page handler
#include "Misc.h"                    // WiFi, MAC, LED...
#include "WebServer.h"               // WebServer header file
#include "MyOTAThing.h"

// SETUP: initialisation entry point ////////////////////////////////////////
void setup() {
  Serial.begin(115200);                       // initialise the serial line
  pinMode(BUILTIN_LED, OUTPUT);
  pinMode(pushButton, INPUT_PULLUP);
  getMAC(MAC_ADDRESS);                        // store the MAC address
  df(setupDBG, "\nMyOTAThing setup...\nESP32 MAC = %s\n", MAC_ADDRESS);
  df(setupDBG, "Firmware is at version %d\n", currentVersion);

  // Boot up indication light
  blink(3,500);

  if (isGUI) {
    startWebServer();
  } else {
    startWiFi();
    doOTAUpdate();
  }
}

// LOOP: task entry point ///////////////////////////////////////////////////
void loop() {
  if (isGUI) {
    // Update the firmware when OTA page is visted
    if (startOTA)
      userConfirmation();

    ledOff(); // ensures LED is turned off when user leaves /ota
    webServer.handleClient();
  } else {
    // Check for updates every 15 seconds
    dln(loopDBG, "\n");
    doOTAUpdate();
    delay(15000);
  }
}

// OTA over-the-air update stuff ///////////////////////////////////////////
void doOTAUpdate() {             // the main OTA logic
  // materials for doing an HTTP GET on github from the BinFiles/ dir

  // Do a GET to read the version file from the cloud
  // This step is already performed in WebServer.ino, double check only
  dln(otaDBG, "Checking for firmware updates...");
  respCode = doCloudGet(&http, gitID, "version");
  if (respCode == 200) // check response code (-ve on failure)
    highestAvailableVersion = atoi(http.getString().c_str());
  else {
    df(otaDBG, "Couldn't get version! rtn code: %d\n", respCode);
    dln(otaDBG, "Cannot update\n\n");
    setup();
  }
  http.end(); // free resources

  // do we know the latest version, and does the firmware need updating?
  if (currentVersion >= highestAvailableVersion && !startReset) {
    dln(otaDBG, "Firmware is up to date\n\n");
    startOTA = false;
    return;
  }

  // Newer firmware available
  if (isGUI && !startReset) {
    df(otaDBG, "Upgrading firmware from version %d ", currentVersion);
    df(otaDBG, "to version %d...\n", highestAvailableVersion);
  } else if (startReset) {
    df(otaDBG, "Resetting firmware from version %d ", currentVersion);
    dln(otaDBG, "to version 1...\n");
  } else {
    df(otaDBG, "Upgrade firmware from version %d ", currentVersion);
    df(otaDBG, "to version %d?\n", highestAvailableVersion);
    dln(otaDBG, "Press the pull-up button to confirm...");
  }

  ledOff();

  // User confirmation stage, for Version 1 only
  if (!isGUI) {
    userConfirmation();
  }

  // Do a GET for the .bin
  dln(otaDBG, "Retrieving update file...");

  // Get the response code of the bin file

  if (startReset) {
    respCode = doCloudGet(
    &http, gitID, "1.bin"
  );
  } else {
    respCode = doCloudGet(
    &http, gitID, String(highestAvailableVersion) + ".bin"
  );

  }

  // The size of the bin file
  int fileSize = http.getSize();

  // Perform the update
  if (respCode == 200 && fileSize >= minSize && fileSize <= maxSize) {
    df(otaDBG, "Received bin file of size %d bytes.\n", fileSize);

    bool canBegin = Update.begin(fileSize);

    if (canBegin) {
      dln(otaDBG, "Performing OTA... Please do not turn off the ESP32...");

      ledOn(); // Start the LED when it is flashing

      // Start writing to the flash
      // http.getStream() will not work for HTTPS
      size_t written = Update.writeStream(*http.getStreamPtr());

      ledOff(); // Turn off LED when finished flashing

      // Check if finished writing
      if (written == fileSize) {
        dln(otaDBG, "Written : " + String(written) + " successfully");
      } else {
        dln(otaDBG,
          "Written only : " +
            String(written) + "/" + String(fileSize) + ". Retry?"
        );
      }

      // Final process
      if (Update.end()) {
        dln(otaDBG, "OTA finished.");
        blink();

        if (Update.isFinished()) {
          dln(otaDBG, "Update completed successfully.");

          if (startReset) {
            currentVersion = 1;
          } else {
            currentVersion = highestAvailableVersion;
          }

          ESP.restart(); // Restart the device
        } else {
          dln(otaDBG, "Update failed. Unknown error occurred.");
        }
      } else {
        dln(otaDBG, "Error Occurred. Error #: " + String(Update.getError()));
      }

      http.end(); // Free resource
    } else {
      dln(otaDBG, "Not enough memory space to perform the update.");
    }
  } else {
    dln(otaDBG, "An error occurred when retrieving the bin file.");
    blink();
  }

  startOTA = false; // Allow the user to update again
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

void userConfirmation() {
  int buttonState = digitalRead(pushButton);
  blink(1, 100);

  if (isGUI) {
    // Press pull-up button to start update
    if (buttonState == LOW  && (millis() - t) > debounce) {
      t = millis();
      dln(loopDBG, "\n");
      doOTAUpdate();
      delay(1000);
    }
  } else {
    while (buttonState == HIGH) {
      buttonState = digitalRead(pushButton);
      blink(1, 100);
    }
  }
}
