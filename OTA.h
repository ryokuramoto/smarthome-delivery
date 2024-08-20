#include <WiFi.h>
#include <Update.h>
#include <HTTPClient.h>

// URL for Over-The-Air update
String overTheAirURL;

// External timer used for scheduling events
extern BlynkTimer edgentTimer;

// Function to handle OTA update requests
BLYNK_WRITE(InternalPinOTA) {
  // Read the OTA URL from the parameter
  overTheAirURL = param.asString();

  // Set a timeout to start the OTA process after 2 seconds
  edgentTimer.setTimeout(2000L, [](){
    // Log event for OTA start
    Blynk.logEvent("sys_ota", "OTA started");

    // Disconnect from Blynk to avoid interference with the OTA process
    Blynk.disconnect();

    // Set the state to OTA upgrade mode
    BlynkState::set(MODE_OTA_UPGRADE);
  });
}

// Function to handle the OTA process
void enterOTA() {
  // Set the state to OTA upgrade mode
  BlynkState::set(MODE_OTA_UPGRADE);

  // Print the firmware update URL for debugging
  DEBUG_PRINT(String("Firmware update URL: ") + overTheAirURL);

  // Initialize HTTP client
  HTTPClient http;
  http.begin(overTheAirURL);

  // Collect MD5 header for validation
  const char* headerkeys[] = { "x-MD5" };
  http.collectHeaders(headerkeys, sizeof(headerkeys)/sizeof(char*));

  // Send HTTP GET request
  int httpCode = http.GET();
  if (httpCode != HTTP_CODE_OK) {
    // If the response is not 200, set the state to error and return
    DEBUG_PRINT("HTTP response should be 200");
    BlynkState::set(MODE_ERROR);
    return;
  }

  // Get the content length of the firmware
  int contentLength = http.getSize();
  if (contentLength <= 0) {
    // If content length is not defined, set the state to error and return
    DEBUG_PRINT("Content-Length not defined");
    BlynkState::set(MODE_ERROR);
    return;
  }

  // Begin the OTA update
  bool canBegin = Update.begin(contentLength);
  if (!canBegin) {
    // If there's not enough space, set the state to error and return
    DEBUG_PRINT("Not enough space to begin OTA");
    BlynkState::set(MODE_ERROR);
    return;
  }

  // Check for MD5 header and set MD5 for update validation
  if (http.hasHeader("x-MD5")) {
    String md5 = http.header("x-MD5");
    if (md5.length() == 32) {
      md5.toLowerCase();
      DEBUG_PRINT("Expected MD5: " + md5);
      Update.setMD5(md5.c_str());
    }
  }

  // Close file system if defined
#ifdef BLYNK_FS
  BLYNK_FS.end();
#endif

  // Get the stream of the firmware
  Client& client = http.getStream();
  // Write the firmware to flash
  int written = Update.writeStream(client);
  if (written != contentLength) {
    // If the written bytes do not match the content length, set the state to error and return
    DEBUG_PRINT(String("OTA written ") + written + " / " + contentLength + " bytes");
    BlynkState::set(MODE_ERROR);
    return;
  }

  // End the update process
  if (!Update.end()) {
    // If there's an error ending the update, set the state to error and return
    DEBUG_PRINT("Error #" + String(Update.getError()));
    BlynkState::set(MODE_ERROR);
    return;
  }

  // Check if the update is finished
  if (!Update.isFinished()) {
    // If the update is not finished, set the state to error and return
    DEBUG_PRINT("Update failed.");
    BlynkState::set(MODE_ERROR);
    return;
  }

  // Print success message and reboot the device
  DEBUG_PRINT("=== Update successfully completed. Rebooting.");
  restartMCU();
}
