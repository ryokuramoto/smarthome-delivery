/*
 * Board configuration (see examples below).
 */

#if defined(USE_WROVER_BOARD)

  #define BOARD_BUTTON_PIN            15       // Pin where user button is attached
  #define BOARD_BUTTON_ACTIVE_LOW     true     // true if button is "active-low"

  #define BOARD_LED_PIN_R             0        // Red LED pin
  #define BOARD_LED_PIN_G             2        // Green LED pin
  #define BOARD_LED_PIN_B             4        // Blue LED pin
  #define BOARD_LED_INVERSE           false    // true if LED is common anode, false if common cathode
  #define BOARD_LED_BRIGHTNESS        128      // 0..255 brightness control

#elif defined(USE_ESP32S3_DEV_MODULE)

  #define BOARD_BUTTON_PIN            0        // Pin where user button is attached
  #define BOARD_BUTTON_ACTIVE_LOW     true     // true if button is "active-low"

  #define BOARD_LED_PIN               13       // Set LED pin - if you have a single-color LED attached
  #define BOARD_LED_INVERSE           false    // true if LED is common anode, false if common cathode
  #define BOARD_LED_BRIGHTNESS        64       // 0..255 brightness control
  Serial.println("test"); 

#elif defined(USE_TTGO_T7)

  #warning "This board does not have a button. Connect a button to gpio0 <> GND"

  #define BOARD_BUTTON_PIN            0        // Pin where user button is attached
  #define BOARD_BUTTON_ACTIVE_LOW     true     // true if button is "active-low"

  #define BOARD_LED_PIN               19       // Set LED pin - if you have a single-color LED attached
  #define BOARD_LED_INVERSE           false    // true if LED is common anode, false if common cathode
  #define BOARD_LED_BRIGHTNESS        64       // 0..255 brightness control

#elif defined(USE_TTGO_T_OI)

  #warning "This board does not have a button. Connect a button to gpio0 <> GND"

  #define BOARD_BUTTON_PIN            0        // Pin where user button is attached
  #define BOARD_BUTTON_ACTIVE_LOW     true     // true if button is "active-low"

  #define BOARD_LED_PIN               3        // Set LED pin - if you have a single-color LED attached
  #define BOARD_LED_INVERSE           false    // true if LED is common anode, false if common cathode
  #define BOARD_LED_BRIGHTNESS        64       // 0..255 brightness control

#elif defined(USE_ESP32_DEV_MODULE)

  #warning "The LED of this board is not configured"

  #define BOARD_BUTTON_PIN            0        // Pin where user button is attached
  #define BOARD_BUTTON_ACTIVE_LOW     true     // true if button is "active-low"

#elif defined(USE_ESP32C3_DEV_MODULE)

  #define BOARD_BUTTON_PIN            9        // Pin where user button is attached
  #define BOARD_BUTTON_ACTIVE_LOW     true     // true if button is "active-low"

  #define BOARD_LED_PIN_WS2812        8        // Pin for WS2812 RGB LED
  #define BOARD_LED_INVERSE           false    // true if LED is common anode, false if common cathode
  #define BOARD_LED_BRIGHTNESS        32       // 0..255 brightness control

#elif defined(USE_ESP32S2_DEV_KIT)

  #define BOARD_BUTTON_PIN            0        // Pin where user button is attached
  #define BOARD_BUTTON_ACTIVE_LOW     true     // true if button is "active-low"

  #define BOARD_LED_PIN               19       // Set LED pin - if you have a single-color LED attached
  #define BOARD_LED_INVERSE           false    // true if LED is common anode, false if common cathode
  #define BOARD_LED_BRIGHTNESS        128      // 0..255 brightness control

#else

  #warning "Custom board configuration is used"

  #define BOARD_BUTTON_PIN            0        // Pin where user button is attached
  #define BOARD_BUTTON_ACTIVE_LOW     true     // true if button is "active-low"

  //#define BOARD_LED_PIN             4        // Set LED pin - if you have a single-color LED attached
  //#define BOARD_LED_PIN_R           15       // Set R,G,B pins - if your LED is PWM RGB
  //#define BOARD_LED_PIN_G           12
  //#define BOARD_LED_PIN_B           13
  //#define BOARD_LED_PIN_WS2812      4        // Set if your LED is WS2812 RGB
  #define BOARD_LED_INVERSE           false    // true if LED is common anode, false if common cathode
  #define BOARD_LED_BRIGHTNESS        64       // 0..255 brightness control

#endif


/*
 * Advanced options
 */

#define BUTTON_HOLD_TIME_INDICATION   3000     // Time in ms for button hold indication
#define BUTTON_HOLD_TIME_ACTION       10000    // Time in ms for button hold action
#define BUTTON_PRESS_TIME_ACTION      50       // Time in ms for button press action

#define BOARD_PWM_MAX                 1023     // Maximum PWM value

#define BOARD_LEDC_CHANNEL_1          1        // LEDC channel 1
#define BOARD_LEDC_CHANNEL_2          2        // LEDC channel 2
#define BOARD_LEDC_CHANNEL_3          3        // LEDC channel 3
#define BOARD_LEDC_TIMER_BITS         10       // Bits for LEDC timer
#define BOARD_LEDC_BASE_FREQ          12000    // Base frequency for LEDC

// Default configurations if not defined
#if !defined(CONFIG_DEVICE_PREFIX)
#define CONFIG_DEVICE_PREFIX          "Blynk"  // Default device prefix
#endif
#if !defined(CONFIG_AP_URL)
#define CONFIG_AP_URL                 "blynk.setup"  // Default AP URL
#endif
#if !defined(CONFIG_DEFAULT_SERVER)
#define CONFIG_DEFAULT_SERVER         "blynk.cloud"  // Default server
#endif
#if !defined(CONFIG_DEFAULT_PORT)
#define CONFIG_DEFAULT_PORT           443      // Default port
#endif

#define WIFI_CLOUD_MAX_RETRIES        500      // Max retries for cloud connection
#define WIFI_NET_CONNECT_TIMEOUT      50000    // Timeout for network connection in ms
#define WIFI_CLOUD_CONNECT_TIMEOUT    50000    // Timeout for cloud connection in ms
#define WIFI_AP_IP                    IPAddress(192, 168, 4, 1)  // Default AP IP
#define WIFI_AP_Subnet                IPAddress(255, 255, 255, 0)  // Default AP Subnet
//#define WIFI_CAPTIVE_PORTAL_ENABLE

//#define USE_TICKER
//#define USE_TIMER_ONE
//#define USE_TIMER_THREE
//#define USE_TIMER_FIVE
#define USE_PTHREAD

#define BLYNK_NO_DEFAULT_BANNER

// Debug print configurations
#if defined(APP_DEBUG)
  #define DEBUG_PRINT(...)  BLYNK_LOG1(__VA_ARGS__)  // Debug print
  #define DEBUG_PRINTF(...) BLYNK_LOG(__VA_ARGS__)   // Debug print formatted
#else
  #define DEBUG_PRINT(...)
  #define DEBUG_PRINTF(...)
#endif
