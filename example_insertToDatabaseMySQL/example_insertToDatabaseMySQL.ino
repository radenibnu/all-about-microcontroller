#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include <WiFi101.h>

char ssid[] = "YourWiFiNetwork";
char pass[] = "YourWiFiPassword";

WiFiClient client;
MySQL_Connection conn((Client *)&client);

char code[20];

void setup() {
  Serial.begin(115200);
  while (!Serial) {}

  // Connect to WiFi
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Connect to database
  conn.connect("YourDatabaseName", "YourDatabaseHost", 3306, "YourUsername", "YourPassword");
  if (conn.connected()) {
    Serial.println("Connected to database");
  } else {
    Serial.println("Failed to connect to database");
  }
}

void loop() {
  sprintf(code, "%lu", (unsigned long)now());

  // Insert code into database
  MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
  char query[100];
  sprintf(query, "INSERT INTO parkir (code) VALUES ('%s')", code);
  cur_mem->execute(query);

  // Wait for button press
  // ...
}