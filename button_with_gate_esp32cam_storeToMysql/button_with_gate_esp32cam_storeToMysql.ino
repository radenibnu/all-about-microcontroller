#include <Arduino.h>
#include <WiFi.h>
#include "esp_camera.h"
#include "base64.h"
#include <Servo.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include <ESPAsyncWebSrv.h>

// Set your WiFi credentials
const char* ssid = "harmul";
const char* password = "h4rmul123";

// Set your MySQL database credentials
const char* host = "localhost";
const char* user = "root";
const char* passwordDB = "";
const char* database = "parkir-coba";

const int buttonPin = 14; // Pin untuk Button
const int ledPin = 2;    // Pin untuk LED
const int servoPin = 15; // Pin untuk Servo

int buttonState = 0;  // variable for reading the pushbutton status

// Camera model
#define CAMERA_MODEL_AI_THINKER // Has PSRAM

// Select camera model
#include "camera_pins.h"

void startCameraServer();

WiFiClient client;
AsyncWebServer server(80);
Servo gateServo;

void setup() {
    Serial.begin(115200);

    // Connect to WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");

    // Initialize button, LED, and servo
    pinMode(buttonPin, INPUT);
    pinMode(ledPin, OUTPUT);
    gateServo.attach(servoPin);

    // Initialize camera
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
    config.pin_sscb_sda = SIOD_GPIO_NUM;
    config.pin_sscb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.frame_size = FRAMESIZE_UXGA;
    config.pixel_format = PIXFORMAT_JPEG; // for streaming
    //config.pixel_format = PIXFORMAT_RGB565; // for face detection/recognition
    config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
    config.fb_location = CAMERA_FB_IN_PSRAM;
    config.jpeg_quality = 12;
    config.fb_count = 1;

     // if PSRAM IC present, init with UXGA resolution and higher JPEG quality
  //                      for larger pre-allocated frame buffer.
  if(config.pixel_format == PIXFORMAT_JPEG){
    if(psramFound()){
      config.jpeg_quality = 10;
      config.fb_count = 2;
      config.grab_mode = CAMERA_GRAB_LATEST;
    } else {
      // Limit the frame size when PSRAM is not available
      config.frame_size = FRAMESIZE_SVGA;
      config.fb_location = CAMERA_FB_IN_DRAM;
    }
  } else {
    // Best option for face detection/recognition
    config.frame_size = FRAMESIZE_240X240;
    #if CONFIG_IDF_TARGET_ESP32S3
    config.fb_count = 2;
    #endif
  }

  #if defined(CAMERA_MODEL_ESP_EYE)
  pinMode(13, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
#endif

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t * s = esp_camera_sensor_get();
  // initial sensors are flipped vertically and colors are a bit saturated
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1); // flip it back
    s->set_brightness(s, 1); // up the brightness just a bit
    s->set_saturation(s, -2); // lower the saturation
  }
  // drop down frame size for higher initial frame rate
  if(config.pixel_format == PIXFORMAT_JPEG){
    s->set_framesize(s, FRAMESIZE_QVGA);
  }

  #if defined(CAMERA_MODEL_M5STACK_WIDE) || defined(CAMERA_MODEL_M5STACK_ESP32CAM)
    s->set_vflip(s, 1);
    s->set_hmirror(s, 1);
  #endif

  #if defined(CAMERA_MODEL_ESP32S3_EYE)
    s->set_vflip(s, 1);
  #endif

    // Set up routes for the web server
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    // Your code for handling the root request goes here
    request->send(200, "text/plain", "Hello, world!");
    });

    server.begin();
}

void loop() {
    // read the state of the pushbutton value:
    buttonState = digitalRead(buttonPin);

    // check if the pushbutton is pressed. If it is, the buttonState is LOW:
    if (buttonState == LOW) {
        digitalWrite(ledPin, HIGH);

        openGate();

        String base64Image = captureAndEncodeImage();

        insertImageToDatabase(base64Image);

        digitalWrite(ledPin, LOW);

        delay(5000);
    }
}

String captureAndEncodeImage() {
    camera_fb_t* fb = esp_camera_fb_get();
    if (!fb) {
        Serial.println("Failed to capture image");
        return "";
    }

    String base64Image = base64::encode(fb->buf, fb->len);
    esp_camera_fb_return(fb);

    return base64Image;
}

void openGate() {
    // Adjust the servo angle to open the gate
    gateServo.write(180);
    delay(1000); // Adjust the delay based on your servo and gate mechanism
}

void insertImageToDatabase(String base64Image) {
    // Convert the MySQL server host string to IPAddress
    // IPAddress serverIP;
    // if (!WiFi.hostByName(host, serverIP)) {
    //     Serial.println("Failed to get server IP address");
    //     return;
    // }

    // // Create a MySQL connection
    // MySQL_Connection conn((Client *)&client);

    // // Connect to the database
    // if (conn.connect(serverIP, 3306, user, passwordDB, database)) {
    //     // Base64 decode the image data
    //     base64_decode((unsigned char *)base64Image.c_str(), base64Image.length(), (unsigned char *)base64Image.c_str());

    //     // Create an INSERT query
    //     String query = "INSERT INTO images (image_data) VALUES ('" + base64Image + "');";

    //     MySQL_Cursor *cur = new MySQL_Cursor(&conn);
    //     cur->execute(query);

    //     // Clean up
    //     delete cur;

    //     // Disconnect from the database
    //     conn.close();
    // } else {
    //     Serial.println("Failed to connect to the database");
    // }
}
