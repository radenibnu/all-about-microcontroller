#include <Servo.h> // Library for controlling the barrier gate servo
// #include <ESP32Servo.h>
#include "esp_camera.h" // Library for using the camera
#include "FS.h" // Library for using SPIFFS file system
#include "SPIFFS.h" // Library for using SPIFFS file system
#include <EEPROM.h>

Servo barrierGate; // Create a servo object

const int buttonPin = 14; //Pin untuk Button
const int ledPin = 2; // Pin untuk LED
const int flashPin = 4; // Pin untuk LED flash

// variables will change:
int buttonState = 0;  // variable for reading the pushbutton status
int pictureNumber = 0; // variable for storing the picture number

// Camera model
#define CAMERA_MODEL_AI_THINKER // Has PSRAM

// Select camera model
#include "camera_pins.h"

void setup() {
  Serial.begin(115200);
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(flashPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  digitalWrite(flashPin, LOW);

  // Attach the servo to the desired pin
  barrierGate.attach(15);
  barrierGate.write(85);

  // Initialize the camera
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
  config.pixel_format = PIXFORMAT_JPEG;
  
  // Init with high specs to pre-allocate larger buffers
  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }
  
  // Camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  // Initialize SPIFFS
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
}

void loop() {
  // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);

  // check if the pushbutton is pressed. If it is, the buttonState is LOW:
  if (buttonState == LOW) {
    // turn LED on:
    digitalWrite(ledPin, HIGH);
    barrierGate.write(180); // Open the gate
    takePhoto(); // Take a photo and save it in SPIFFS
  } else {
    // turn LED off:
    digitalWrite(ledPin, LOW);
    barrierGate.write(85); // Close the gate
  }
}

void takePhoto() {
  // Turn on the flash
  digitalWrite(flashPin, HIGH);

  // Get the picture number from the EEPROM
  pictureNumber = EEPROM.read(0);

  // Increase the picture number for the next photo
  pictureNumber++;

  // Save the picture number to the EEPROM
  EEPROM.write(0, pictureNumber);
  EEPROM.commit();

  // Create a file name with the picture number
  String path = "/picture" + String(pictureNumber) + ".jpg";

  // Take a photo with the camera
  camera_fb_t *fb = NULL;
  fb = esp_camera_fb_get();  

  if(!fb) {
    Serial.println("Camera capture failed");
    digitalWrite(flashPin, LOW);
    return;
  }

  // Create a file in SPIFFS and write the photo data
  File file = SPIFFS.open(path, FILE_WRITE);

  if(!file){
    Serial.println("Failed to open file in writing mode");
  } else {
    file.write(fb->buf, fb->len); // payload (image), payload length
    Serial.printf("Saved file to path: %s\n", path.c_str());
  }

  file.close();

  // Return the frame buffer back to the driver for reuse
  esp_camera_fb_return(fb);

  // Turn off the flash
  digitalWrite(flashPin, LOW);
}