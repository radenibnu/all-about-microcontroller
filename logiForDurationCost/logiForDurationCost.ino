unsigned long duration;
unsigned long currentTime;
unsigned long entryTime;
float cost;

void setup() {
  // kode untuk inisialisasi peralatan
  entryTime = millis();
}

void loop() {
  currentTime = millis();
  duration = (currentTime - entryTime) / 1000; // durasi dalam detik

  cost = duration * 0.05; // asumsikan tarif parkir adalah 0,05 per detik

  // kode untuk mencetak barcode, mengirim data ke database, dll.
}