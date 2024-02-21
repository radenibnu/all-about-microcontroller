#include <Servo.h> // Library for controlling the barrier gate servo

const int buttonPin = 10;
const int ledPin = 3;
Servo myservo;  // create servo object to control a servo

void setup() {
  // put your setup code here, to run once:
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
  myservo.write(0);
}

void loop() {
  // put your main code here, to run repeatedly:
  int sensorvalue = digitalRead(buttonPin);

  if (sensorvalue == LOW){
    Serial.print("value button :");
    Serial.println(sensorvalue);
    digitalWrite(ledPin, HIGH);
    myservo.write(80);
  }
  else {
    digitalWrite(ledPin, LOW);
  }
}
