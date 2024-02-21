#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <SoftwareSerial.h>

#define SSID "Office"
#define PASS "0ff1c3123"

const int buttonPin = D2;
const int ledPin = D3;

char server[] = "alamat_ip_server_laravel";
int port = 80;

SoftwareSerial thermalSerial(D5, D6); // RX, TX pin untuk thermal printer

void setup() {
  Serial.begin(115200);
  Serial.println();
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  connectWiFi();
  thermalSerial.begin(19200); // baud rate untuk thermal printer
}

void loop() {
  int buttonState = digitalRead(buttonPin);

  if (buttonState == LOW) {
    digitalWrite(ledPin, HIGH);
    printTicket(); // print karcis
    sendToLaravel(); // kirim data ke server Laravel
  } else {
    digitalWrite(ledPin, LOW);
  }
}

void printTicket() {
  thermalSerial.write(27); // inisialisasi thermal printer
  thermalSerial.write(64);

  thermalSerial.write("Parkir Masuk\n\n"); // tulisan header karcis
  thermalSerial.write("Nomor Karcis: 001\n");
  thermalSerial.write("Waktu Masuk: ");
  thermalSerial.print(getTime()); // menampilkan waktu masuk pada karcis
  thermalSerial.write("\n");

  thermalSerial.write(10); // line feed
  thermalSerial.write(10);
  thermalSerial.write(10);

  thermalSerial.write(29); // set tebal font pada footer karcis
  thermalSerial.write(69);
  thermalSerial.write(1);
  thermalSerial.write("TERIMA KASIH\n"); // tulisan footer karcis
  thermalSerial.write(10);
  thermalSerial.write(10);
  thermalSerial.write(10);
  thermalSerial.write(27); // cut kertas
  thermalSerial.write(109);
  thermalSerial.write(0);
}

void sendToLaravel() {
  String entryTime = getTime();
  String data = "&entry_time=" + entryTime;
  WiFiClient client;
  if (client.connect(server, port)) {
    client.println("POST /api/parkir HTTP/1.1");
    client.println("Host: " + String(server));
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.println("Content-Length: " + String(data.length()));
    client.println("");
    client.print(data);
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

String getTime() {
  time_t now = time(nullptr);
  return String(now);
}
