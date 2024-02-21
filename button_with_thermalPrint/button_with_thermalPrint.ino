#include <Barcode.h>
#include <SoftwareSerial.h>
#include <Time.h>

#define TX_PIN 10
#define RX_PIN 11
#define BAUD_RATE 9600
#define buttonPin 2

SoftwareSerial thermalPrinter(TX_PIN, RX_PIN);
Barcode barcode;

void setup() {
  thermalPrinter.begin(BAUD_RATE);
  barcode.begin(thermalPrinter);
  setTime(0);
  pinMode(buttonPin, INPUT);
}

void loop() {
  int buttonState = digitalRead(buttonPin);
  if (buttonState == HIGH) {
    char code[16];
    sprintf(code, "%lu", (unsigned long)now());
    //sprintf untuk memformat waktu saat ini menjadi string dan menyimpannya ke dalam array karakter code
    barcode.code128(code, 30);
    thermalPrinter.println();
    delay(1000);
  }
  delay(10);
}