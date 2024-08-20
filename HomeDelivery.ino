// Ryoh's Configuration: Blynk Template ID, Template Name, and Auth Token
#define BLYNK_TEMPLATE_ID           "XXXXX"
#define BLYNK_TEMPLATE_NAME         "XXXXX"

#include <WiFi.h>
#include "BlynkEdgent.h"  // Blynk and Wi-Fi provisioning library

// Enable debug prints to the Serial Monitor
#define BLYNK_PRINT Serial

#include "CameraWebServer.h"

// Include Electronic Components (sensors, servo motor, ultrasonic sensor, camera)
// Note: This should be declared after other libraries are included.
#include "ElectronicComponents.h" 

#include "Geofence.h"  // Include the Geofence header file

// Timer for Blynk operations
BlynkTimer timer;

Preferences preferences;

bool openState = false;  // Indicates the open state: True when the button is not pressed, False when pressed.
bool lockState = true;   // Indicates the locked state.
bool isV4On = true;      // Indicates whether the system is active.
bool isV0On = true;      // Indicates whether email notifications are active.

// Set to true to reset the stored latitude and longitude in ESP32 memory.
// Use true for testing, but set to false in the final deployment.
bool resetMemory = true;

// ESP32 memory storage keys
const char* LATITUDE_KEY = "latitude_home";
const char* LONGITUDE_KEY = "longitude_home";

// Variables for storing the initialized latitude and longitude values
double latitude_home;
double longitude_home;

// Default values for home latitude and longitude if keys are not found in memory
const double latitude_home_default = 43.657426;
const double longitude_home_default = -79.737513;

// Variables for delivery person's latitude and longitude
double latitude_delivery = 0.0;
double longitude_delivery = 0.0;

bool inGeofence = false; // Indicates whether the delivery person is within the geofence area.

// This function is triggered whenever the state of Virtual Pin V0 changes
// Virtual pin for "Activate System" button
BLYNK_WRITE(V0)
{
  int value = param.asInt(); // Get the incoming value from Virtual Pin V0
  bool requestedV0State = (value == 1); // Determine the desired state based on user interaction

  if (!requestedV0State && lockState && !openState) {
      // If the user requested to turn V0 OFF and the conditions are met
      isV0On = false;
      Serial.println("The system has been disabled.");
  } else {
      // If the conditions are not met, or the user requested to turn V0 ON
      isV0On = true;
      Blynk.virtualWrite(V0, 1); // Force the button back to ON state in the app
      Serial.println("The system has been enabled.");
  }
}

// Virtual pin for "Email Notification" button
BLYNK_WRITE(V4)
{
  int value = param.asInt(); 
  isV4On = (value == 1); // Update the state of V4
}

// Virtual pin to store the delivery person's smartphone GPS latitude
// This pin is not visible in the Blynk app interface
BLYNK_WRITE(V5)
{
  latitude_delivery = param.asDouble(); 
  Serial.print("Updated delivery latitude: ");
  Serial.println(latitude_delivery, 6);  // 6 decimal values
  checkGeofence(); // Check geofence whenever location is updated
}

// Virtual pin to store the delivery person's smartphone GPS longitude
// This pin is not visible in the Blynk app interface
BLYNK_WRITE(V6)
{
  longitude_delivery = param.asDouble(); 
  Serial.print("Updated delivery longitude: ");
  Serial.println(longitude_delivery, 6);
  checkGeofence(); // Check geofence whenever location is updated
}

// Stores the latitude value in preferences
BLYNK_WRITE(V7)
{
  double value = param.asDouble(); 

  Serial.print("Value from V7: ");
  Serial.println(value, 6); 

  // Store the new latitude value in preferences
  preferences.begin("blynk", false);
  preferences.putDouble(LATITUDE_KEY, value);
  preferences.end();

  // Update the home latitude
  latitude_home = value;
  Serial.print("Home latitude updated to: ");
  Serial.println(latitude_home, 6);
  checkGeofence();  // Check geofence with updated value
}

// Stores the longitude value in preferences
BLYNK_WRITE(V8)
{
  double value = param.asDouble();

  Serial.print("Value from V8: ");
  Serial.println(value, 6);

  // Store the new longitude value in preferences
  preferences.begin("blynk", false);
  // This stores the double value with the key "LONGITUDE_KEY" in the preferences.
  preferences.putDouble(LONGITUDE_KEY, value);
  preferences.end();

  // Update the home longitude
  longitude_home = value;
  Serial.print("Home longitude updated to: ");
  Serial.println(longitude_home, 6);
  checkGeofence();  // Check geofence with updated value
}

// This function is called every time the device is connected to Blynk.Cloud
BLYNK_CONNECTED()
{
  // Initialize virtual pin values
  Blynk.virtualWrite(V0, true);
  Blynk.virtualWrite(V1, 0);
  Blynk.virtualWrite(V4, false);
  Blynk.virtualWrite(V5, 0.0);
  Blynk.virtualWrite(V6, 0.0);
  Blynk.virtualWrite(V7, latitude_home);
  Blynk.virtualWrite(V8, longitude_home);

  // Wait for connection to WiFi
  if (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("Wi-Fi not connected.");
  }
  // Print the IP address of the ESP32 to access the camera server
  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");

  // Start the camera web server
  startCameraServer();
}

// This function sends Arduino's uptime every second to Virtual Pin 2.
void myTimerEvent()
{
  // Send the uptime in seconds to Virtual Pin 2
  Blynk.virtualWrite(V2, millis() / 1000);
}

void setup()
{
  Serial.begin(115200); // Initialize serial communication at 115200 baud

  // Initialize preferences with the namespace "blynk" in read-write mode
  preferences.begin("blynk", false);

  // Retrieve the latitude and longitude values from ESP32 memory, or use default values if not found
  latitude_home = preferences.getDouble(LATITUDE_KEY, latitude_home_default);
  longitude_home = preferences.getDouble(LONGITUDE_KEY, longitude_home_default);

  // To manually reset the stored latitude and longitude in ESP32 memory 
  if (resetMemory){
    preferences.putDouble(LATITUDE_KEY, latitude_home_default);
    preferences.putDouble(LONGITUDE_KEY, longitude_home_default);
  }

  // Close the preferences after accessing them
  preferences.end();

  BlynkEdgent.begin();  // Initialize Blynk and Wi-Fi provisioning

  // Perform initial geofence check without triggering an HTTP request.
  checkGeofence();

  // Set a timer to call myTimerEvent every second
  timer.setInterval(1000L, myTimerEvent);

  // Initialize camera setting
  initializeCameraWeb();

  // Initialize electronic components
  initializeElectronicComponents();
}

void loop()
{
  BlynkEdgent.run();  // Handle Blynk and Wi-Fi provisioning
  timer.run(); // Run the timer

  runElectronicComponents();  // Run the electronic components
}
