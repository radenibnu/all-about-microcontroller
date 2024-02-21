#include <Servo.h>
#include "esp_camera.h" // Library for camera
#include "FS.h"

Servo barrierGate;
const int buttonPin = 14;
const int ledPin = 2;
int buttonState = 0;

void setup() {
  Serial.begin(115200);
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  barrierGate.attach(15);
  barrierGate.write(85);

  // Camera setup
  camera_config_t config;
  esp_camera_init(&config);
}

void loop() {
  buttonState = digitalRead(buttonPin);

  if (buttonState == LOW) {
    digitalWrite(ledPin, HIGH);
    barrierGate.write(180); // Open the gate

    // Capture a photo
    capturePhoto();

    delay(5000); // Add a delay to prevent multiple button presses
  } else {
    digitalWrite(ledPin, LOW);
  }
}

void capturePhoto() {
  camera_fb_t *fb = NULL;
  fb = esp_camera_fb_get();

  if (!fb) {
    Serial.println("Camera capture failed");
    return;
  }

  // Save the photo to SPIFFS
  fs::File file = SPIFFS.open("/photo.jpg", FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    esp_camera_fb_return(fb);
    return;
  }

  // Write the image data to the file
  file.write(fb->buf, fb->len);
  file.close();

  Serial.println("Photo saved to SPIFFS");

  esp_camera_fb_return(fb);
}