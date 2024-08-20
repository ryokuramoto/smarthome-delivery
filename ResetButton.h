#ifdef BOARD_BUTTON_PIN

volatile bool     g_buttonPressed = false;          // Flag to indicate if the button is pressed
volatile uint32_t g_buttonPressTime = -1;           // Variable to store the time when the button was pressed

// Action to perform when the button is held for a specified duration
void button_action(void)
{
  BlynkState::set(MODE_RESET_CONFIG);              // Set the Blynk state to reset configuration mode
}

// Interrupt service routine to handle button state changes
void button_change(void)
{
#if BOARD_BUTTON_ACTIVE_LOW
  bool buttonState = !digitalRead(BOARD_BUTTON_PIN); // Read button state for active-low configuration
#else
  bool buttonState = digitalRead(BOARD_BUTTON_PIN);  // Read button state for active-high configuration
#endif

  if (buttonState && !g_buttonPressed) {            // Button pressed
    g_buttonPressTime = millis();                   // Record the time when the button was pressed
    g_buttonPressed = true;                         // Set the flag to indicate button is pressed
    DEBUG_PRINT("Hold the button for 10 seconds to reset configuration...");
  } else if (!buttonState && g_buttonPressed) {     // Button released
    g_buttonPressed = false;                        // Clear the flag to indicate button is released
    uint32_t buttonHoldTime = millis() - g_buttonPressTime;  // Calculate how long the button was held
    if (buttonHoldTime >= BUTTON_HOLD_TIME_ACTION) { // If held for long enough to trigger an action
      button_action();                              // Perform the button action
    } else if (buttonHoldTime >= BUTTON_PRESS_TIME_ACTION) {
      // User action for a shorter press can be handled here
    }
    g_buttonPressTime = -1;                         // Reset the press time variable
  }
}

// Initialize the button with appropriate settings
void button_init()
{
#if BOARD_BUTTON_ACTIVE_LOW
  pinMode(BOARD_BUTTON_PIN, INPUT_PULLUP);          // Set button pin mode to input with pull-up resistor for active-low
#else
  pinMode(BOARD_BUTTON_PIN, INPUT_PULLDOWN);        // Set button pin mode to input with pull-down resistor for active-high
#endif
  attachInterrupt(BOARD_BUTTON_PIN, button_change, CHANGE); // Attach interrupt to handle changes in button state
}

#else

#define g_buttonPressed     false                   // Define default values if BOARD_BUTTON_PIN is not defined
#define g_buttonPressTime   0

void button_init() {}                               // Define an empty button_init function if BOARD_BUTTON_PIN is not defined

#endif
