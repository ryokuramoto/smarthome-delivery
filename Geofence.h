#include <cmath> // Required for distance calculation

// Geofence parameters
const double geofence_radius_m = 20.0; // Geofence radius in meters.

// Variables for storing the initialized latitude and longitude values
extern double latitude_home;
extern double longitude_home;

// Variables for delivery person's latitude and longitude
extern double latitude_delivery;
extern double longitude_delivery;

// Geofence state variable
extern bool inGeofence;

// Function to calculate the distance between two coordinates in meters
double calculateDistance(double lat1, double lon1, double lat2, double lon2) {
    const double R = 6371000; // Earth's radius in meters
    double dLat = (lat2 - lat1) * M_PI / 180.0;
    double dLon = (lon2 - lon1) * M_PI / 180.0;
    double a = sin(dLat / 2) * sin(dLat / 2) +
               cos(lat1 * M_PI / 180.0) * cos(lat2 * M_PI / 180.0) *
               sin(dLon / 2) * sin(dLon / 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    return R * c;
}

// Function to check if the delivery person is within the geofence
void checkGeofence() {
    double distance = calculateDistance(latitude_home, longitude_home, latitude_delivery, longitude_delivery);

    // Print home and delivery locations on one line
    Serial.print("Home Location: (");
    Serial.print(latitude_home, 6);
    Serial.print(", ");
    Serial.print(longitude_home, 6);
    Serial.print("),  ");

    Serial.print("Delivery Location: (");
    Serial.print(latitude_delivery, 6);
    Serial.print(", ");
    Serial.print(longitude_delivery, 6);
    Serial.print("),  ");

    // Print the distance calculated
    Serial.print("Distance: ");
    Serial.println(distance, 6);

    if (distance <= geofence_radius_m) {
        inGeofence = true;
        Serial.println("Delivery person is within the geofence.");
    } else {
        inGeofence = false;
        Serial.println("Delivery person is outside the geofence.");
    }
    Serial.println("");
}

