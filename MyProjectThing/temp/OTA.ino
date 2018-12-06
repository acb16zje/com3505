#import "OTA.h"
#import "Misc.h"

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
  if (!startReset) {
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

  // Do a GET for the .bin
  dln(otaDBG, "Retrieving update file...");

  // Get the response code of the bin file

  if (startReset) {
    respCode = doCloudGet(&http, gitID, "1.bin");
  } else {
    respCode = doCloudGet(&http, gitID, String(highestAvailableVersion) + ".bin");
  }

  // The size of the bin file
  int fileSize = http.getSize();

  // Perform the update
  if (respCode == 200 && fileSize >= minSize && fileSize <= maxSize) {
    df(otaDBG, "Received bin file of size %d bytes.\n", fileSize);

    bool canBegin = Update.begin(fileSize);

    if (canBegin) {
      dln(otaDBG, "Performing OTA... Please do not turn off the ESP32...");

      // Start writing to the flash
      // http.getStream() will not work for HTTPS
      size_t written = Update.writeStream(*http.getStreamPtr());

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
  }

  aSyncServer.onNotFound(onRequest);
  aSyncServer.onFileUpload(onUpload);
  aSyncServer.onRequestBody(onBody);

  aSyncServer.begin();
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
