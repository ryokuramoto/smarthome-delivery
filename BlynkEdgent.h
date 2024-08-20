// Ensure the functions declared here are compatible with C linkage
extern "C" {
  void app_loop();
  void restartMCU();  // Restarts the microcontroller unit.
}

#include "Settings.h"  // Stores user settings.
#include <BlynkSimpleEsp32_SSL.h>  // Secure connection to the Blynk cloud.

// Define the file system based on the specified storage method
#if defined(BLYNK_USE_LITTLEFS)
  #include <LittleFS.h>
  #define BLYNK_FS LittleFS
#elif defined(BLYNK_USE_SPIFFS)
  #if defined(ESP32)
    #include <SPIFFS.h>
  #elif defined(ESP8266)
    #include <FS.h>
  #endif
  #define BLYNK_FS SPIFFS
#endif

// Ensure the correct version of the Blynk library is used
#ifndef BLYNK_NEW_LIBRARY
#error "Old version of Blynk library is in use. Please replace it with the new one."
#endif

// Define template name if device name is provided
#if !defined(BLYNK_TEMPLATE_NAME) && defined(BLYNK_DEVICE_NAME)
#define BLYNK_TEMPLATE_NAME BLYNK_DEVICE_NAME
#endif

// Check that the necessary Blynk template IDs are specified
#if !defined(BLYNK_TEMPLATE_ID) || !defined(BLYNK_TEMPLATE_NAME)
#error "Please specify your BLYNK_TEMPLATE_ID and BLYNK_TEMPLATE_NAME"
#endif

// Ensure BLYNK_AUTH_TOKEN is not manually defined when using Blynk.Edgent
#if defined(BLYNK_AUTH_TOKEN)
#error "BLYNK_AUTH_TOKEN is assigned automatically when using Blynk.Edgent, please remove it from the configuration"
#endif

// Initialize a Blynk timer
BlynkTimer edgentTimer;

// Include additional Blynk and device-specific headers
#include "BlynkState.h"  // Manages the different states the device can be in.
#include "ConfigStore.h"  // Stores configuration settings.
#include "ResetButton.h"  // Manages a physical reset button on the device.
#include "ConfigMode.h"  // Handles entering and managing configuration mode.
#include "Indicator.h"  // Manages LED indicators or other UI elements to show status.
#include "OTA.h"  // Manages over-the-air firmware updates.
#include "Console.h"  // Provides logging and console output.

// Set the current state in the Blynk state machine
inline void BlynkState::set(State m) {
  if (state != m && m < MODE_MAX_VALUE) {
    DEBUG_PRINT(String(StateStr[state]) + " => " + StateStr[m]);
    state = m;

    // Custom state handling can be implemented here,
    // such as custom LED indications
  }
}

// Print device information banner for debugging purposes
void printDeviceBanner()
{
#ifdef BLYNK_PRINT
  Blynk.printBanner();
  BLYNK_PRINT.println("----------------------------------------------------");
  BLYNK_PRINT.print(" Device:    "); BLYNK_PRINT.println(getWiFiName());
  BLYNK_PRINT.print(" Firmware:  "); BLYNK_PRINT.println(BLYNK_FIRMWARE_VERSION " (build " __DATE__ " " __TIME__ ")");
  if (configStore.getFlag(CONFIG_FLAG_VALID)) {
    BLYNK_PRINT.print(" Token:     ");
    BLYNK_PRINT.println(String(configStore.cloudToken).substring(0,4) +
                " - •••• - •••• - ••••");
  }
  BLYNK_PRINT.print(" Platform:  "); BLYNK_PRINT.println(String(BLYNK_INFO_DEVICE) + " @ " + ESP.getCpuFreqMHz() + "MHz");
  BLYNK_PRINT.print(" Chip rev:  "); BLYNK_PRINT.println(ESP.getChipRevision());
  BLYNK_PRINT.print(" SDK:       "); BLYNK_PRINT.println(ESP.getSdkVersion());
  BLYNK_PRINT.print(" Flash:     "); BLYNK_PRINT.println(String(ESP.getFlashChipSize() / 1024) + "K");
  BLYNK_PRINT.print(" Free mem:  "); BLYNK_PRINT.println(ESP.getFreeHeap());
  BLYNK_PRINT.println("----------------------------------------------------");
#endif
}

// Main Blynk run function with state checks
void runBlynkWithChecks() {
  Blynk.run();
  if (BlynkState::get() == MODE_RUNNING) {
    if (!Blynk.connected()) {
      if (WiFi.status() == WL_CONNECTED) {
        BlynkState::set(MODE_CONNECTING_CLOUD);
      } else {
        BlynkState::set(MODE_CONNECTING_NET);
      }
    }
  }
}

// Main class for Blynk.Edgent functionality
class Edgent {

public:
  // Initialization function for setting up the device
  void begin()
  {
    WiFi.persistent(false); // Disable persistent storage of WiFi credentials
    WiFi.enableSTA(true);   // Enable Station mode to get MAC address

    // Set minimum WiFi security level (ESP-IDF version >= 4.0.0)
#if (ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0))
    WiFi.setMinSecurity(WIFI_AUTH_WEP);
#endif

#ifdef BLYNK_FS
    BLYNK_FS.begin(true); // Initialize the file system
#endif

    // Initialize various components
    indicator_init();
    button_init();
    config_init();
    printDeviceBanner();
    console_init();

    // Determine the initial state based on stored configuration
    if (configStore.getFlag(CONFIG_FLAG_VALID)) {
      BlynkState::set(MODE_CONNECTING_NET);
    } else if (config_load_blnkopt()) {
      DEBUG_PRINT("Firmware is preprovisioned");
      BlynkState::set(MODE_CONNECTING_NET);
    } else {
      BlynkState::set(MODE_WAIT_CONFIG);
    }

    // Check for valid template ID and name
    if (!String(BLYNK_TEMPLATE_ID).startsWith("TMPL") || !strlen(BLYNK_TEMPLATE_NAME)) {
      DEBUG_PRINT("Invalid configuration of TEMPLATE_ID / TEMPLATE_NAME");
      while (true) { delay(100); }
    }
  }

  // Main run loop to handle different states
  void run() {
    app_loop(); // Run application-specific loop
    switch (BlynkState::get()) {
      case MODE_WAIT_CONFIG:       // Enter configuration mode
      case MODE_CONFIGURING:       enterConfigMode();    break;  // Puts the device into configuration mode for setting up Wi-Fi credentials.
      case MODE_CONNECTING_NET:    enterConnectNet();    break;  // Handles the connection to the Wi-Fi network.
      case MODE_CONNECTING_CLOUD:  enterConnectCloud();  break;  // Connects to the Blynk cloud server.
      case MODE_RUNNING:           runBlynkWithChecks(); break;
      case MODE_OTA_UPGRADE:       enterOTA();           break;  // Manages Over-the-Air updates.
      case MODE_SWITCH_TO_STA:     enterSwitchToSTA();   break;  // Switches the device to Station mode.
      case MODE_RESET_CONFIG:      enterResetConfig();   break;
      default:                     enterError();         break;  // Error handling.
    }
  }

} BlynkEdgent; // Instantiate the Edgent class

// Application-specific loop function
void app_loop() {
    edgentTimer.run();   // Run the Blynk timer
    edgentConsole.run(); // Run the console
}
