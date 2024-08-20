// Configuration flags for validation and static IP configuration
#define CONFIG_FLAG_VALID       0x01
#define CONFIG_FLAG_STATIC_IP   0x02

// Provisioning error codes
#define BLYNK_PROV_ERR_NONE     0      // No error
#define BLYNK_PROV_ERR_CONFIG   700    // Invalid configuration from app (malformed token, etc.)
#define BLYNK_PROV_ERR_NETWORK  701    // Could not connect to the router
#define BLYNK_PROV_ERR_CLOUD    702    // Could not connect to the cloud
#define BLYNK_PROV_ERR_TOKEN    703    // Invalid token error (after connection)
#define BLYNK_PROV_ERR_INTERNAL 704    // Other issues (e.g., hardware failure)

// Structure to store configuration settings
struct ConfigStore {
  uint32_t  magic;          // Magic number to verify the validity of the stored config
  char      version[15];    // Firmware version
  uint8_t   flags;          // Configuration flags

  char      wifiSSID[34];   // Wi-Fi SSID
  char      wifiPass[64];   // Wi-Fi password

  char      cloudToken[34]; // Blynk cloud token
  char      cloudHost[34];  // Blynk cloud host
  uint16_t  cloudPort;      // Blynk cloud port

  uint32_t  staticIP;       // Static IP address
  uint32_t  staticMask;     // Static subnet mask
  uint32_t  staticGW;       // Static gateway
  uint32_t  staticDNS;      // Static DNS
  uint32_t  staticDNS2;     // Secondary static DNS

  int       last_error;     // Last error code

  // Method to set a configuration flag
  void setFlag(uint8_t mask, bool value) {
    if (value) {
      flags |= mask;
    } else {
      flags &= ~mask;
    }
  }

  // Method to get the status of a configuration flag
  bool getFlag(uint8_t mask) {
    return (flags & mask) == mask;
  }
} __attribute__((packed)); // Ensures the structure is packed

// Global instance of ConfigStore
ConfigStore configStore;

// Default configuration settings
const ConfigStore configDefault = {
  0x626C6E6B,                  // Magic number
  BLYNK_FIRMWARE_VERSION,      // Firmware version
  0x00,                        // Flags initialized to 0
  
  "",                          // Default Wi-Fi SSID
  "",                          // Default Wi-Fi password
  
  "invalid token",             // Default cloud token
  CONFIG_DEFAULT_SERVER,       // Default cloud host
  CONFIG_DEFAULT_PORT,         // Default cloud port
  0,                           // Default static IP
  BLYNK_PROV_ERR_NONE          // No error
};

// Template function to copy a string into a fixed-size array
template<typename T, int size>
void CopyString(const String& s, T(&arr)[size]) {
  s.toCharArray(arr, size);
}

// Function to load configuration from Blynk options
static bool config_load_blnkopt() {
  static const char blnkopt[] = "blnkopt\0"
    BLYNK_PARAM_KV("ssid" , BLYNK_PARAM_PLACEHOLDER_64
                            BLYNK_PARAM_PLACEHOLDER_64
                            BLYNK_PARAM_PLACEHOLDER_64
                            BLYNK_PARAM_PLACEHOLDER_64)
    BLYNK_PARAM_KV("host" , CONFIG_DEFAULT_SERVER)
    BLYNK_PARAM_KV("port" , BLYNK_TOSTRING(CONFIG_DEFAULT_PORT))
    "\0";

  BlynkParam prov(blnkopt+8, sizeof(blnkopt)-8-2);
  BlynkParam::iterator ssid = prov["ssid"];
  BlynkParam::iterator pass = prov["pass"];
  BlynkParam::iterator auth = prov["auth"];
  BlynkParam::iterator host = prov["host"];
  BlynkParam::iterator port = prov["port"];

  // Validate if the necessary parameters are present
  if (!(ssid.isValid() && auth.isValid())) {
    return false;
  }

  // Reset to default before loading values from blnkopt
  configStore = configDefault;

  // Copy values from blnkopt to configStore
  if (ssid.isValid()) { CopyString(ssid.asStr(), configStore.wifiSSID); }
  if (pass.isValid()) { CopyString(pass.asStr(), configStore.wifiPass); }
  if (auth.isValid()) { CopyString(auth.asStr(), configStore.cloudToken); }
  if (host.isValid()) { CopyString(host.asStr(), configStore.cloudHost); }
  if (port.isValid()) { configStore.cloudPort = port.asInt(); }

  return true;
}

#include <Preferences.h>

// Function to load configuration from flash memory
void config_load() {
  Preferences prefs;
  if (prefs.begin("blynk", true)) { // Open preferences in read-only mode
    memset(&configStore, 0, sizeof(configStore)); // Clear configStore
    prefs.getBytes("config", &configStore, sizeof(configStore)); // Load stored config
    if (configStore.magic != configDefault.magic) { // Check if the magic number is valid
      DEBUG_PRINT("Using default config.");
      configStore = configDefault; // Use default config if the magic number is invalid
    }
  } else {
    DEBUG_PRINT("Config read failed");
  }
}

// Function to save configuration to flash memory
bool config_save() {
  Preferences prefs;
  if (prefs.begin("blynk", false)) { // Open preferences in write mode
    prefs.putBytes("config", &configStore, sizeof(configStore)); // Save configStore to flash
    DEBUG_PRINT("Configuration stored to flash");
    return true;
  } else {
    DEBUG_PRINT("Config write failed");
    return false;
  }
}

// Function to initialize configuration
bool config_init() {
  config_load(); // Load the configuration from flash
  return true;
}

// Function to reset configuration to default
void enterResetConfig() {
  DEBUG_PRINT("Resetting configuration!");
  configStore = configDefault; // Reset configStore to default
  config_save(); // Save the default configuration to flash
  BlynkState::set(MODE_WAIT_CONFIG); // Set state to wait for configuration
}

// Function to set the last error code
void config_set_last_error(int error) {
  // Only set error if not provisioned
  if (!configStore.getFlag(CONFIG_FLAG_VALID)) {
    configStore = configDefault; // Reset to default configuration
    configStore.last_error = error; // Set the last error code
    BLYNK_LOG2("Last error code: ", error);
    config_save(); // Save the configuration to flash
  }
}
