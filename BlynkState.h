// Define the different states the device can be in
enum State {
  MODE_WAIT_CONFIG,       // Waiting for configuration
  MODE_CONFIGURING,       // In the process of configuration
  MODE_CONNECTING_NET,    // Attempting to connect to the network
  MODE_CONNECTING_CLOUD,  // Attempting to connect to the Blynk cloud
  MODE_RUNNING,           // Running normally after successful connection
  MODE_OTA_UPGRADE,       // Performing an OTA (Over-The-Air) upgrade
  MODE_SWITCH_TO_STA,     // Switching to Station mode
  MODE_RESET_CONFIG,      // Resetting configuration
  MODE_ERROR,             // In an error state

  MODE_MAX_VALUE          // Maximum value for the state enumeration
};

// Debugging strings for each state
#if defined(APP_DEBUG)
const char* StateStr[MODE_MAX_VALUE+1] = {
  "WAIT_CONFIG",          // Corresponds to MODE_WAIT_CONFIG
  "CONFIGURING",          // Corresponds to MODE_CONFIGURING
  "CONNECTING_NET",       // Corresponds to MODE_CONNECTING_NET
  "CONNECTING_CLOUD",     // Corresponds to MODE_CONNECTING_CLOUD
  "RUNNING",              // Corresponds to MODE_RUNNING
  "OTA_UPGRADE",          // Corresponds to MODE_OTA_UPGRADE
  "SWITCH_TO_STA",        // Corresponds to MODE_SWITCH_TO_STA
  "RESET_CONFIG",         // Corresponds to MODE_RESET_CONFIG
  "ERROR",                // Corresponds to MODE_ERROR

  "INIT"                  // Initial state (not used in the enum)
};
#endif

namespace BlynkState
{
  volatile State state = MODE_MAX_VALUE; // Initialize the state variable to MODE_MAX_VALUE

  // Function to get the current state
  State get()        { return state; }
  
  // Function to check if the current state is equal to the given state
  bool  is (State m) { return (state == m); }
  
  // Function to set the state to the given state
  void  set(State m);
};
