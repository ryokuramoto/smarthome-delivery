/**********************************************************************
  Filename    : Camera Web Server
  Description : The camera images captured by the ESP32S3 are displayed on the web page.
  Auther      : www.freenove.com
  Modification: 2024/07/01
**********************************************************************/
#include "esp_camera.h"

// ===================
// Select camera model
// ===================
//#define CAMERA_MODEL_WROVER_KIT // Has PSRAM
//#define CAMERA_MODEL_ESP_EYE // Has PSRAM
// #define CAMERA_MODEL_ESP32S3_EYE // Has PSRAM
//#define CAMERA_MODEL_M5STACK_PSRAM // Has PSRAM
//#define CAMERA_MODEL_M5STACK_V2_PSRAM // M5Camera version B Has PSRAM
//#define CAMERA_MODEL_M5STACK_WIDE // Has PSRAM
//#define CAMERA_MODEL_M5STACK_ESP32CAM // No PSRAM
//#define CAMERA_MODEL_M5STACK_UNITCAM // No PSRAM
#define CAMERA_MODEL_AI_THINKER // Has PSRAM
//#define CAMERA_MODEL_TTGO_T_JOURNAL // No PSRAM
// ** Espressif Internal Boards **
//#define CAMERA_MODEL_ESP32_CAM_BOARD
//#define CAMERA_MODEL_ESP32S2_CAM_BOARD
//#define CAMERA_MODEL_ESP32S3_CAM_LCD

#include "camera_pins.h"

// Function to start the camera server
void startCameraServer();

void initializeCameraWeb() {
  // Initialize serial communication at a baud rate of 115200
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  // Camera configuration setup
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000; // Set XCLK frequency to 20MHz
  config.frame_size = FRAMESIZE_UXGA; // Set frame size to UXGA
  config.pixel_format = PIXFORMAT_JPEG; // Set pixel format to JPEG for streaming
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12; // Set JPEG quality to 12 (lower means better quality)
  config.fb_count = 1; // Set the frame buffer count to 1

  // Check if PSRAM is available
  if(psramFound()){
    config.jpeg_quality = 10; // Set better JPEG quality if PSRAM is available
    config.fb_count = 2; // Use 2 frame buffers when PSRAM is available
    config.grab_mode = CAMERA_GRAB_LATEST;
  } else {
    // If PSRAM is not available, reduce the frame size and use DRAM for frame buffer
    config.frame_size = FRAMESIZE_SVGA;
    config.fb_location = CAMERA_FB_IN_DRAM;
  }

  // Initialize the camera with the configured settings
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    // If camera initialization fails, print the error code
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  // Get the camera sensor settings
  sensor_t * s = esp_camera_sensor_get();
  // Adjust sensor settings: flip vertically, increase brightness, and reduce saturation
  s->set_vflip(s, 1); // flip it back
  s->set_brightness(s, 1); // up the brightness just a bit
  s->set_saturation(s, 0); // lower the saturation
}


