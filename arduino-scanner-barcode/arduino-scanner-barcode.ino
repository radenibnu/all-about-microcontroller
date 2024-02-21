#include <Barcode.h>
#include <SoftwareSerial.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>

SoftwareSerial barcode(2, 3);  // RX, TX
Barcode reader(barcode);

char incomingBarcode[15];
char entryTime[20];
char exitTime[20];
int duration;
int cost;

void setup() {
  barcode.begin(9600);
  Serial.begin(9600);
  // Koneksikan ke database
}

void loop() {
  if (reader.read(incomingBarcode)) {
    // Cari barcode dalam database
    if (adaBarcode(incomingBarcode)) {
      // Ambil waktu masuk dan waktu keluar
      // Hitung durasi parkir dan biaya parkir
      // Perbarui status parkir
      // Catat informasi waktu keluar dan biaya parkir ke database
    }
  }
}

bool adaBarcode(char *barcode) {
  // Query database untuk mencari barcode
  // Kembalikan hasil query
}