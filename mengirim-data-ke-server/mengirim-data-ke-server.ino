#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "Office";
const char* password = "0ff1c3123";
const char* serverUrl = "http://alamat_ip_server_laravel/api/parkir";

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");

    pinMode(D2, INPUT_PULLUP);
}

void loop() {
    int buttonState = digitalRead(D2);
    if (buttonState == LOW) {
        Serial.println("Tombol ditekan");

        // generate barcode
        String barcode = generateBarcode();

        // kirim data ke server Laravel
        HTTPClient http;
        http.begin(serverUrl);
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");
        String postData = "entry_time=" + String(now()) + "&barcode=" + barcode;
        int httpResponseCode = http.POST(postData);
        http.end();

        // print karcis dan barcode
        if (httpResponseCode == 200) {
            Serial.println("Data berhasil disimpan");
            printKarcis(barcode);
        } else {
            Serial.println("Gagal menyimpan data");
        }

        delay(1000);
    }
}

String generateBarcode() {
    // kode untuk generate barcode
}

void printKarcis(String barcode) {
    // kode untuk print karcis dan barcode
}
