#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h> // token untuk dapet callback dari friebasenya
#include <time.h>
#include <DHT.h>

const int DHTPIN = 4;
const int DHTTYPE = DHT11;
DHT dht(DHTPIN, DHTTYPE);

int timestamp;
const char* ntpServer = "pool.ntp.org"; //standar yang dipakai untuk mendapatkan waktu di internet

const String USER_EMAIL = "radenibnuqc@gmail.com";
const String USER_PASSWORD = "password123";
const String API_KEY = "AIzaSyCLQ3N8n2IxyBT72OyUNT2TCzr5pIsi5PQ";
const String DATABASE_URL = "https://hackany-iot-default-rtdb.asia-southeast1.firebasedatabase.app/";

const String WIFI_SSID = "wegodev";
const String WIFI_PASSWORD = "23242";

String uid; //user id 

FirebaseData fbdo;
Firebase auth;
FirebaseConfig config;
FirebaseJson json;

void initWifi() {
  //konek wifi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to WiFI...");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println(WiFi.localIP());
}

unsigned long getTime(){
  time_t now;

  Serial.print("Getting time");
  while (now < (24 * 3600)) {
    Serial.print(".");

    now = time(nullptr);
    
    delay(1000);
  }
  Serial.print("");

  return now;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  initWifi();

  dht.begin();

  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(humidity) || isnan(temperature)){
    Serial.println("Failed to read from DHT Sensor !");
    return;
  }

  configTime(0, 0, ntpServer); //settingan untuk mengatur waktu sesuai negara 

  //setup firebase & login
  config.database_url = DATABASE_URL;
  config.api_key =  API_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  Firebase.reconnectWiFi(true);
  fbdo.setResponseSize(4096); // data objek untuk si firebasenya

  config.token_status_callback = tokenStatusCallback;
  config.max_token_generation_retry = 5;

  Firebase.begin(config, auth);

  Serial.println("Getting User Id");
  while (auth.token.uid == ""){
    Serial.print(".");
  } 

  uid = auth.token.uid.c_str();
  Serial.print("User Id : ");
  Serial.println(uid);

  timestamp = getTime();

  Serial.print("Temperature : ");
  Serial.print(temperature);
  Serial.print("Humidity : ");
  Serial.print(humidity);

  // store data
  String path = "/environment/" + String(now);
  json.set("temperature", temperature);
  json.set("humidity", humidity);
  json.set("createdAt", timestamp);
  Serial.print("Set json...");
  Serial.println(Firebase.RTDB.setJSON(&fbdo, path.c_str(), $json) ? "Success" : fbdo.errorReason().c_str());

  Serial.println(timestamp);
}

void loop() {
  // put your main code here, to run repeatedly:
   timestamp = getTime();

  Serial.print("Temperature : ");
  Serial.print(temperature);
  Serial.print("Humidity : ");
  Serial.print(humidity);

  // store data
  String path = "/environment/" + String(now);
  json.set("temperature", temperature);
  json.set("humidity", humidity);
  json.set("createdAt", timestamp);
  Serial.print("Set json...");
  Serial.println(Firebase.RTDB.setJSON(&fbdo, path.c_str(), $json) ? "Success" : fbdo.errorReason().c_str());

  Serial.println(timestamp);

  delay(1000);

}
