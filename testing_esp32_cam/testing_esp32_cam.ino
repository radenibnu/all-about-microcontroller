#include "esp_camera.h"
#include <WiFi.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

// Change these settings to match your network
const char *ssid = "harmul";
const char *password = "h4rmul123";

void startCameraServer();

void setup() {
  Serial.begin(115200);

  // Disable brownout problems
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Start the camera server
  startCameraServer();
}

void loop() {
  // Do nothing here
}

void startCameraServer() {
  // Camera configuration
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = 5;
  config.pin_d1 = 18;
  config.pin_d2 = 19;
  config.pin_d3 = 21;
  config.pin_d4 = 36;
  config.pin_d5 = 39;
  config.pin_d6 = 34;
  config.pin_d7 = 35;
  config.pin_xclk = 0;
  config.pin_pclk = 22;
  config.pin_vsync = 25;
  config.pin_href = 23;
  config.pin_sscb_sda = 26;
  config.pin_sscb_scl = 27;
  config.pin_pwdn = 32;
  config.pin_reset = -1;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_SVGA;
  config.jpeg_quality = 12;
  config.fb_count = 2;

  // Initialize the camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  // Start the camera server
  httpd_config_t config_httpd = HTTPD_DEFAULT_CONFIG();
  config_httpd.max_uri_handlers = 8;  // Increase if you have more URI handlers
  httpd_handle_t camera_httpd = NULL;
  if (httpd_start(&camera_httpd, &config_httpd) == ESP_OK) {
    httpd_uri_t uri = {
        .uri       = "/",
        .method    = HTTP_GET,
        .handler   = stream_handler,
        .user_ctx  = NULL};
    httpd_register_uri_handler(camera_httpd, &uri);
    Serial.println("Camera Ready!");
  }
}

esp_err_t stream_handler(httpd_req_t *req) {
  camera_fb_t *fb = NULL;
  esp_err_t res = ESP_OK;

  // Get an image from the camera
  fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }

  // Send the image
  httpd_resp_set_type(req, "image/jpeg");
  httpd_resp_set_hdr(req, "Content-Disposition", "inline; filename=capture.jpg");
  httpd_resp_send(req, (const char *)fb->buf, fb->len);

  // Return the frame buffer to the camera
  esp_camera_fb_return(fb);
  return res;
}
