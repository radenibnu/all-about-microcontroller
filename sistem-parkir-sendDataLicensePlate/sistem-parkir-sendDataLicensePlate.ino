#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <Adafruit_Thermal.h>
#include <Adafruit_GFX.h>

#define SSID "Office"
#define PASS "0ff1c3123"

const int buttonPin = D2;
const int ledPin = D3;
int buttonState = 0;
char server[] = "https://sparkir.rumahsakit-harapanmulia.id/";
int port = 80;

Adafruit_Thermal printer(&Serial, 9); // define printer object

void setup() {
  Serial.begin(115200);
  Serial.println();
  pinMode(buttonPin,INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  connectWiFi();
  printer.begin(); // initialize printer
}

void loop() {
  int buttonState = digitalRead(buttonPin);

  if (buttonState == LOW) {
    digitalWrite(ledPin, HIGH);
    delay(1000);
    String license_plate = "F 1234 G";
    sendToLaravel(license_plate);
  } 
  else {
    digitalWrite(ledPin, LOW);
  }
}

void sendToLaravel(String license_plate) {  
  String data = "&license_plate=" + license_plate;
  WiFiClient client;
  if (client.connect(server, port)) {
    client.println("POST /api/parkir HTTP/1.1");
    client.println("Host: " + String(server));
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.println("Content-Length: " + String(data.length()));
    client.println("");
    client.print(data);
    Serial.println("Data send to laravel :  " + data);
  } else {
    Serial.println("Faile to connect to server ");
  }
  delay(1000);
}

void connectWiFi() {
  Serial.print("Connecting to WiFi...");
  WiFi.begin(SSID, PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}
