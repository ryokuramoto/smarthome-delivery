#include <ESP32Servo.h>  // For servo moter
#include <LiquidCrystal.h>  // For LCD1602

// Pin configuration
const int trigPin = 2;  // Ultrasonic trig pin
const int echoPin = 15;  // Ultrasonic echo pin
const int LEDPin = 4;  // LED pin
const int buttonPin = 22;  // Button pin
const int servoPin = 13; // Serve moter pin
const int rs = 33, en = 25, d4 = 26, d5 = 27, d6 = 14, d7 = 12;  // LCD1602A Pins
const int backlightPin = 35;  // Control LCD power ON/OFF

// The variables defined in the .ino file
extern bool openState;  // Indicates the open state: True when the button is not pressed, False when pressed.
extern bool lockState;   // Indicates the locked state.
extern bool inGeofence; // Indicates whether the delivery person is within the geofence area.
extern bool isV4On;      // Indicates whether the system is active.
extern bool isV0On;      // Indicates whether email notifications are active.

// Ultrasonic Sensor Variables
float duration, distance;

// Servo Motor Setup
Servo myservo;

int currentAngle = 180;  // Current angle of the servo
int targetAngle = 180;   // Target angle for the servo
int maxAngle = 180;    // Maximum angle
int minAngle = 0;      // Minimum angle
int angleStep = 1;     // Step size for the servo movement

unsigned long previousMillis = 0;
const long interval = 5; // Adjust this value to control the speed (smaller is faster)
unsigned long buttonPressedTime = 0;
const long lockDelay = 10000; // 10 seconds delay
unsigned long previousSerialMillis = 0;
const long serialInterval = 2000; // 2.0 second interval for serial output

// Initialize the library with the numbers of the interface pins
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void initializeElectronicComponents() {
  // Ultrasonic Sensor
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // LED
  pinMode(LEDPin, OUTPUT);

  // Servo Motor
  myservo.attach(servoPin);

  // Button
  pinMode(buttonPin, INPUT_PULLUP);

  // Initialize Servo Position
  myservo.write(currentAngle);

  // Initialize backlight pin
  pinMode(backlightPin, OUTPUT);
  digitalWrite(backlightPin, LOW);  // Turn off the backlight by default

  // Initialize LCD
  lcd.begin(16, 2); // Set up the LCD's number of columns and rows
  lcd.print("Lock State:"); // Initial message
}

void moveToTargetAngle();

void runElectronicComponents() {
  // If the system is deactivated by the user or if the delivery person is outside the geofence, 
  // do not activate the electronic components.
  if (!isV0On || !inGeofence) {
    // If V0 is OFF, clear the LCD and turn off the backlight
    lcd.clear();
    digitalWrite(backlightPin, LOW);  // Power off LCD
    return;  // Exit the function early
  }

  // Power on LCD if isV0On is true
  digitalWrite(backlightPin, HIGH);  // Turn on the backlight

  // Check button state
  openState = digitalRead(buttonPin) == LOW;  // This is correct code.

  // Ultrasonic Sensor Code
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = (duration * 0.0343) / 2;

  // Logic to change to unlock state. 
  // When unlock, take a camera and send image data to the server
  if (!openState && lockState && distance < 10) {
    lockState = false;
    targetAngle = minAngle;  // Move to 0 degrees when unlocked
    moveToTargetAngle();

    // Send email notification to the user when box has been unlocked.
    if (isV4On) {
      Blynk.logEvent("unlock_state", "The device has unlocked.");
      Serial.println("Notification: The box has been unlocked, and an email has been sent to the user.");
    }
  }

  if (!openState && !lockState) {
    if (millis() - buttonPressedTime >= lockDelay) {
      lockState = true;
      targetAngle = maxAngle;  // Move to 180 degrees when locked
      moveToTargetAngle();
    }
  } else {
    buttonPressedTime = millis();
  }

  // LED is activated when box is unlocked
  digitalWrite(LEDPin, lockState ? LOW : HIGH);  // LED is active when the pin is set to High

  // Display lock state on LCD
  lcd.clear(); // Clear the display before printing a new message
  lcd.setCursor(0, 0); // Move to the beginning of the first line
  lcd.print("Lock State:"); // Display static message on the first line
  lcd.setCursor(0, 1); // Move to the second line
  if (lockState) {
    lcd.print("Locked  ");
  } else {
    lcd.print("Unlocked");
  }

  // Serial output formatted for Serial Plotter
  unsigned long currentMillis = millis();
  if (currentMillis - previousSerialMillis >= serialInterval) {
    previousSerialMillis = currentMillis;
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.print(" cm, Servo Angle: ");
    Serial.print(currentAngle);
    Serial.print(" , Open State: ");
    Serial.print(openState);
    Serial.print(" , Lock State: ");
    Serial.println(lockState);
  }
}

void moveToTargetAngle() {
  while (currentAngle != targetAngle) {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;

      if (currentAngle > targetAngle) {
        currentAngle -= angleStep;
        if (currentAngle < targetAngle) {
          currentAngle = targetAngle;
        }
      } else if (currentAngle < targetAngle) {
        currentAngle += angleStep;
        if (currentAngle > targetAngle) {
          currentAngle = targetAngle;
        }
      }

      myservo.write(currentAngle);
    }
  }
}
