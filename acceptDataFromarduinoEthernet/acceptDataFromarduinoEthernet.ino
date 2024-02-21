#include <SPI.h>
#include <Ethernet.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192,168,1,177);
EthernetClient client;

void setup() {
  Ethernet.begin(mac, ip);
  Serial.begin(9600);
  delay(1000);
}

void loop() {
  if (client.connect("192.168.1.100", 80)) {
    String data = "field1=value1&field2=value2";
    client.println("POST /insert.php HTTP/1.1");
    client.println("Host: 192.168.1.100");
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.println("Content-Length: " + String(data.length()));
    client.println();
    client.println(data);
    client.stop();
  }
  delay(1000);
}