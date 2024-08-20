#include <Blynk/BlynkConsole.h>

// Include external C libraries for ESP32 partition and OTA operations
extern "C" {
  #include "esp_partition.h"
  #include "esp_ota_ops.h"
}

// Initialize the Blynk console for debugging and command input
BlynkConsole edgentConsole;

// Function to initialize the console and set up commands
void console_init()
{
#ifdef BLYNK_PRINT
  // Begin the console with BLYNK_PRINT as the output stream
  edgentConsole.begin(BLYNK_PRINT);
#endif

  // Print a prompt to the console
  edgentConsole.print("\n>");

  // Add a command to reboot the device
  edgentConsole.addCommand("reboot", []() {
    edgentConsole.print(R"json({"status":"OK","msg":"rebooting wifi module"})json" "\n");
    delay(100);
    restartMCU(); // Function to restart the microcontroller unit
  });

  // Add a command to handle configuration
  edgentConsole.addCommand("config", [](int argc, const char** argv) {
    if (argc < 1 || 0 == strcmp(argv[0], "start")) {
      BlynkState::set(MODE_WAIT_CONFIG); // Set the state to wait for configuration
    } else if (0 == strcmp(argv[0], "erase")) {
      BlynkState::set(MODE_RESET_CONFIG); // Set the state to reset configuration
    }
  });

  // Add a command to display device information
  edgentConsole.addCommand("devinfo", []() {
    edgentConsole.printf(
        R"json({"name":"%s","board":"%s","tmpl_id":"%s","fw_type":"%s","fw_ver":"%s"})json" "\n",
        getWiFiName().c_str(),  // Get Wi-Fi name
        BLYNK_TEMPLATE_NAME,    // Template name
        BLYNK_TEMPLATE_ID,      // Template ID
        BLYNK_FIRMWARE_TYPE,    // Firmware type
        BLYNK_FIRMWARE_VERSION  // Firmware version
    );
  });

  // Add a command to connect to Wi-Fi using provided credentials
  edgentConsole.addCommand("connect", [](int argc, const char** argv) {
    if (argc < 2) {
      edgentConsole.print(R"json({"status":"error","msg":"invalid arguments. expected: <auth> <ssid> <pass>"})json" "\n");
      return;
    }
    String auth = argv[0];
    String ssid = argv[1];
    String pass = (argc >= 3) ? argv[2] : "";

    if (auth.length() != 32) {
      edgentConsole.print(R"json({"status":"error","msg":"invalid token size"})json" "\n");
      return;
    }

    edgentConsole.print(R"json({"status":"OK","msg":"trying to connect..."})json" "\n");

    configStore = configDefault; // Load default configuration
    CopyString(ssid, configStore.wifiSSID);  // Copy SSID to config
    CopyString(pass, configStore.wifiPass);  // Copy password to config
    CopyString(auth, configStore.cloudToken); // Copy authentication token to config

    BlynkState::set(MODE_SWITCH_TO_STA); // Set state to switch to Station mode
  });

  // Add a command to display Wi-Fi information or scan for networks
  edgentConsole.addCommand("wifi", [](int argc, const char* argv[]) {
    if (argc < 1 || 0 == strcmp(argv[0], "show")) {
      edgentConsole.printf(
          "mac:%s ip:%s (%s [%s] %ddBm)\n",
          getWiFiMacAddress().c_str(), // Get MAC address
          WiFi.localIP().toString().c_str(), // Get local IP address
          getWiFiNetworkSSID().c_str(), // Get SSID of the network
          getWiFiNetworkBSSID().c_str(), // Get BSSID of the network
          WiFi.RSSI() // Get Wi-Fi signal strength
      );
    } else if (0 == strcmp(argv[0], "scan")) {
      int found = WiFi.scanNetworks(); // Scan for available networks
      for (int i = 0; i < found; i++) {
        bool current = (WiFi.SSID(i) == WiFi.SSID());
        edgentConsole.printf(
            "%s %s [%s] %s ch:%d rssi:%d\n",
            (current ? "*" : " "), WiFi.SSID(i).c_str(), // SSID
            macToString(WiFi.BSSID(i)).c_str(), // BSSID
            wifiSecToStr(WiFi.encryptionType(i)), // Encryption type
            WiFi.channel(i), // Channel
            WiFi.RSSI(i) // Signal strength
        );
      }
      WiFi.scanDelete(); // Clear the scan results
    }
  });

  // Add a command to display firmware information or perform a rollback
  edgentConsole.addCommand("firmware", [](int argc, const char** argv) {
    if (argc < 1 || 0 == strcmp(argv[0], "info")) {
      unsigned sketchSize = ESP.getSketchSize();

      edgentConsole.printf(" Version:   %s (build %s)\n", BLYNK_FIRMWARE_VERSION, __DATE__ " " __TIME__);
      edgentConsole.printf(" Type:      %s\n", BLYNK_FIRMWARE_TYPE);
      edgentConsole.printf(" Platform:  %s\n", BLYNK_INFO_DEVICE);
      edgentConsole.printf(" SDK:       %s\n", ESP.getSdkVersion());

      if (const esp_partition_t* running = esp_ota_get_running_partition()) {
        edgentConsole.printf(" Partition: %s (%dK)\n", running->label, running->size / 1024);
        edgentConsole.printf(" App size:  %dK (%d%%)\n", sketchSize/1024, (sketchSize*100)/(running->size));
        edgentConsole.printf(" App MD5:   %s\n", ESP.getSketchMD5().c_str());
      }

    } else if (0 == strcmp(argv[0], "rollback")) {
      if (Update.rollBack()) {
        edgentConsole.print(R"json({"status":"ok"})json" "\n");
        edgentTimer.setTimeout(50, restartMCU); // Restart MCU after a short delay
      } else {
        edgentConsole.print(R"json({"status":"error"})json" "\n");
      }
    }
  });

  // Add a command to display system status
  edgentConsole.addCommand("status", [](int argc, const char** argv) {
    const int64_t t = esp_timer_get_time() / 1000000;
    unsigned secs = t % BLYNK_SECS_PER_MIN;
    unsigned mins = (t / BLYNK_SECS_PER_MIN) % BLYNK_SECS_PER_MIN;
    unsigned hrs  = (t % BLYNK_SECS_PER_DAY) / BLYNK_SECS_PER_HOUR;
    unsigned days = t / BLYNK_SECS_PER_DAY;

    edgentConsole.printf(" Uptime:          %dd %dh %dm %ds\n", days, hrs, mins, secs);
    edgentConsole.printf(" Chip:            %s rev %d\n", ESP.getChipModel(), ESP.getChipRevision());
    edgentConsole.printf(" Flash:           %dK\n",       ESP.getFlashChipSize() / 1024);
    edgentConsole.printf(" Stack unused:    %d\n",        uxTaskGetStackHighWaterMark(NULL));
    edgentConsole.printf(" Heap free:       %d / %d\n",   ESP.getFreeHeap(), ESP.getHeapSize());
    edgentConsole.printf("      max alloc:  %d\n",        ESP.getMaxAllocHeap());
    edgentConsole.printf("      min free:   %d\n",        ESP.getMinFreeHeap());
    if (ESP.getPsramSize()) {
      edgentConsole.printf(" PSRAM free:      %d / %d\n", ESP.getFreePsram(), ESP.getPsramSize());
    }
#ifdef BLYNK_FS
    uint32_t fs_total = BLYNK_FS.totalBytes();
    edgentConsole.printf(" FS free:         %d / %d\n",   (fs_total-BLYNK_FS.usedBytes()), fs_total);
#endif
  });

#ifdef BLYNK_FS

  // Add a command to list files in the file system
  edgentConsole.addCommand("ls", [](int argc, const char** argv) {
    const char* path = (argc < 1) ? "/" : argv[0];
    File rootDir = BLYNK_FS.open(path);
    while (File f = rootDir.openNextFile()) {
#if defined(BLYNK_USE_SPIFFS) && (ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(4, 0, 0))
      String fn = f.name();
#else
      String fn = f.path();
#endif

      MD5Builder md5;
      md5.begin();
      md5.addStream(f, f.size());
      md5.calculate();
      String md5str = md5.toString();

      edgentConsole.printf("%8d %-24s %s\n",
                            f.size(), fn.c_str(),
                            md5str.substring(0,8).c_str());
    }
  });

  // Add a command to remove files from the file system
  edgentConsole.addCommand("rm", [](int argc, const char** argv) {
    if (argc < 1) return;

    for (int i=0; i<argc; i++) {
      const char* fn = argv[i];
      if (BLYNK_FS.remove(fn)) {
        edgentConsole.printf("Removed %s\n", fn);
      } else {
        edgentConsole.printf("Removing %s failed\n", fn);
      }
    }
  });

  // Add a command to rename files in the file system
  edgentConsole.addCommand("mv", [](int argc, const char** argv) {
    if (argc != 2) return;

    if (!BLYNK_FS.rename(argv[0], argv[1])) {
      edgentConsole.print("Rename failed\n");
    }
  });

  // Add a command to display the contents of a file
  edgentConsole.addCommand("cat", [](int argc, const char** argv) {
    if (argc != 1) return;

    if (!BLYNK_FS.exists(argv[0])) {
      edgentConsole.print("File not found\n");
      return;
    }

    if (File f = BLYNK_FS.open(argv[0], FILE_READ)) {
      while (f.available()) {
        edgentConsole.print((char)f.read());
      }
      edgentConsole.print("\n");
    } else {
      edgentConsole.print("Cannot open file\n");
    }
  });

  // Add a command to write data to a file
  edgentConsole.addCommand("echo", [](int argc, const char** argv) {
    if (argc != 2) return;

    if (File f = BLYNK_FS.open(argv[1], FILE_WRITE)) {
      if (!f.print(argv[0])) {
        edgentConsole.print("Cannot write file\n");
      }
    } else {
      edgentConsole.print("Cannot open file\n");
    }
  });

#endif

}

// Handle commands sent to the internal debug pin
BLYNK_WRITE(InternalPinDBG) {
  String cmd = String(param.asStr()) + "\n";
  edgentConsole.runCommand((char*)cmd.c_str());
}
