#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "Office";
const char* password = "0ff1c3123";
const char* endpoint = "http://alamat-endpoint-anda/arduino";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  
  Serial.println("Connected to WiFi");
}

void loop() {
  // Kirim data ke endpoint
  HTTPClient http;
  http.begin(endpoint);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.POST("data=Hello%20from%20Arduino");
  
  int httpResponseCode = http.GET();
  String response = http.getString();
  
  Serial.print("HTTP Response code: ");
  Serial.println(httpResponseCode);
  Serial.print("Response: ");
  Serial.println(response);
  
  http.end();
  
  delay(5000);
}
