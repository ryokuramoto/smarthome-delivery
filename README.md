# Smart Home Delivery System

This project aims to develop a cost-effective and efficient home delivery system using basic components, including the ESP32 microcontroller, ultrasonic sensor, and servo motor. The system integrates key technologies such as Blynk for mobile app interaction, real-time notifications, and GPS location tracking.

## Components

- ESP32 module with camera
- Ultrasonic sensor (HC-SR04)
- SG90 9g micro servo motor
- LCD1602 module
- LED
- Potentiometer
- Button

## Installation

### 1. Clone the Repository
```sh
git clone https://github.com/ryokuramoto/SmartHomeDelivery.git
cd SmartHomeDelivery
```

### 2. Install Required Libraries
Ensure you have the following libraries installed in your Arduino IDE:

- **Blynk**: Version 1.3.2 by Volodymyr Shymanskyy
- **Wi-Fi Manager**: Version 2.0.17 by tzapu
- **ESP32Servo**: Version 3.0.5 by Kevin Harrington, John K. Bennett
- **LiquidCrystal**: Version 1.0.7 by Arduino, Adafruit

### 3. Set Up the Blynk App
In the Blynk app, create a new project and configure the following virtual pins:

- **Uptime Label (V2)**: Displays the system's total uptime.
- **Activate System Switch (V0)**: Toggles the system on or off.
- **Email Notification Switch (V4)**: Toggles email notifications on or off.
- **Delivery Person GPS (V5, V6)**: Stores the delivery person's GPS coordinates.
- **Home GPS (V7, V8)**: Stores the home location's GPS coordinates.

### 4. Upload the Code
1. Open `HomeDelivery.ino` in the Arduino IDE.
2. Select the appropriate board and port for your ESP32.
3. Upload the code to your ESP32.

## Usage

### Code Overview

- **Technical Report**: Detailed documentation is available in `Smart Home Delivery_Technical Report.pdf`.
- **HomeDelivery.ino**: Main code file. Configure your Blynk Template ID, Name, and default home GPS location (`latitude_home_default`, `longitude_home_default`).
- **ElectronicComponents.h**: Manages electronic components. Configure ESP32 pins based on your hardware setup.
- **Geofence.h**: Handles geofencing calculations. Modify the geofence radius (`geofence_radius_m`) as required.
- **Wi-Fi Provisioning Files**: These files (`BlynkEdgent.h`, `BlynkState.h`, etc.) manage Wi-Fi provisioning. They are downloaded from the Blynk website and typically require no modification.
- **Camera Files**: Configuration files for the camera (`CameraWebServer.h`, `camera_index.h`, `camera_pins.h`) are based on Freenove documentation. Choose your camera model in `CameraWebServer.h` for automatic pin configuration.

### Hardware Setup

1. **Connect the Hardware**
   - Follow the pin configuration defined in the code to connect your components to the ESP32.

2. **Power the ESP32**
   - Power your ESP32 via USB or another suitable power source.

3. **Run the System**
   - Perform Wi-Fi provisioning by entering Wi-Fi credentials during the first run.

### Operation

1. **Start the System**: Power the ESP32. The system automatically initializes and runs the code.
2. **Wi-Fi Connection**: Ensure the ESP32 is connected to your Wi-Fi network.
3. **Initialize Homeowner App Settings**: On first use, all buttons in the homeowner app are set to ON by default. Adjust the button states as needed.
4. **Delivery Person Detection**: The system activates when the courier enters the geofenced area. Ensure the homeowner's location is pre-configured in the Arduino code. The camera connects to the server, enabling live streaming to the user.
5. **Unlocking the Delivery Box**: The box unlocks when the ultrasonic sensor detects the courier. The servo motor rotates 180 degrees to disengage the lock, allowing the box to open. The status ("Lock" and "Open") is displayed on the LCD module.
6. **User Notification**: The homeowner receives an email notification when the delivery box is unlocked, provided the notification feature in the user app is enabled.
7. **Locking the Delivery Box**: The box automatically locks 10 seconds after closing. The servo motor rotates back 180 degrees to re-engage the lock, securing the box.
8. **System Continuation**: The system remains operational until the ESP32 is powered off.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Contact

For inquiries or support, please contact:

Ryo Kuramoto  
[ryoh.kuramoto@rknewtech.com](mailto:ryoh.kuramoto@rknewtech.com)
