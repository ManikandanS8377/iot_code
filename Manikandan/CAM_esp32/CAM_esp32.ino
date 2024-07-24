#include "esp_camera.h"

// Camera configuration
camera_config_t config = {
    .pin_pwdn = 32,
    .pin_reset = -1,
    .pin_xclk = 0,
    .pin_sscb_sda = 26,
    .pin_sscb_scl = 27,

    .xclk_freq_hz = 20000000,
    .ledc_timer = LEDC_TIMER_0,
    .ledc_channel = LEDC_CHANNEL_0,

    .pixel_format = PIXFORMAT_JPEG,
    .frame_size = FRAMESIZE_SVGA,
    .jpeg_quality = 10,
    .fb_count = 2
};

void setup() {
  // Initialize the camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
}

void loop() {
  // Acquire a frame
  camera_fb_t * fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    return;
  }

  // Process the image
  process_image(fb->width, fb->height, fb->format, fb->buf, fb->len);

  // Return the frame buffer back to the driver for reuse
  esp_camera_fb_return(fb);
}

void process_image(size_t width, size_t height, pixformat_t format, uint8_t * buf, size_t len) {
  // Your image processing code goes here
  // You can save the image to an SD card, send it over WiFi, etc.
}
