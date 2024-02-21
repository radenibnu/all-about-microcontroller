#include <WiFi.h>
#include <Wire.h>
#include <ArduCAM.h>
#include <SPI.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>

const char* ssid = "harmul";
const char* password = "h4rmul123";

const char* user = "root";
const char* password_db = "";
const char* database = "parkir_coba";

IPAddress ip;
const int buttonPin = 2; // Ganti dengan pin sesuai koneksi tombol
const int ledPin = 13;    // Ganti dengan pin sesuai koneksi LED indikator

boolean buttonState = LOW;
boolean lastButtonState = LOW;
long lastDebounceTime = 0;
long debounceDelay = 50;

WiFiClient client; // Deklarasi objek WiFiClient
MySQL_Connection conn((Client *)&client);

ArduCAM arducam = ArduCAM(OV2640, 5); // Menggunakan pin 5 sebagai Chip Select (CS)
int photoNumber = 0;

void setup() {
  Serial.begin(115200);
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  const char* ipString = "192.168.1.2";
  ip.fromString(ipString);

  // if (conn.connect(ip, 3306, user, password_db)) {
  //   Serial.println("Connected to database");
  // } else {
  //   Serial.println("Connection to database failed");
  // }
}

void loop() {
  int reading = digitalRead(buttonPin);
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == HIGH) {
        digitalWrite(ledPin, HIGH);
        captureAndSaveToDatabase();
      } else {
        digitalWrite(ledPin, LOW);
      }
    }
  }

  lastButtonState = reading;
}

void captureAndSaveToDatabase() {
  arducam.flush_fifo();
  arducam.clear_fifo_flag();

  // Mulai pengambilan gambar
  arducam.start_capture();

  // Tunggu sampai gambar selesai diambil
  while (!arducam.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK));

  // Hitung ukuran gambar
  int totalBytes = arducam.read_fifo_length();
  Serial.printf("Image size: %d\n", totalBytes);

  // Baca gambar dari FIFO dan simpan ke buffer
  uint8_t* imageBuffer = (uint8_t*)malloc(totalBytes);
  arducam.CS_LOW();
  arducam.set_fifo_burst();
  SPI.transferBytes(NULL, imageBuffer, totalBytes);
  arducam.CS_HIGH();

  // Simpan gambar ke database
  saveImageToDatabase(imageBuffer, totalBytes);

  // Bersihkan buffer
  free(imageBuffer);
}

void saveImageToDatabase(uint8_t* imageData, int imageSize) {
  char query[128];
  sprintf(query, "INSERT INTO data_parkir (image_data) VALUES ('%s')", escapeString(imageData, imageSize));
  
  MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
  cur_mem->execute(query);
  delete cur_mem;
}

char* escapeString(uint8_t* input, int length) {
  char* output = (char*)malloc(length * 2 + 1);
  int j = 0;

  for (int i = 0; i < length; i++) {
    if (input[i] == '\'') {
      output[j++] = '\\';
      output[j++] = '\'';
    } else {
      output[j++] = input[i];
    }
  }

  output[j] = '\0';
  return output;
}
