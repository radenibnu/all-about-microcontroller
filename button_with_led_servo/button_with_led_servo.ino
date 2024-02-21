#include <Servo.h> // Library for controlling the barrier gate servo

Servo barrierGate; // Create a servo object

const int buttonPin = 14; //Pin untuk Button
const int ledPin = 2; // Pin untuk LED

// variables will change:
int buttonState = 0;  // variable for reading the pushbutton status

void setup() {
  Serial.begin(9600);
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  // Attach the servo to the desired pin
  barrierGate.attach(15);
  barrierGate.write(85);
}

void loop() {
  // buttonIsPressed();
  // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonState == LOW) {
    // turn LED on:
    digitalWrite(ledPin, HIGH);
    barrierGate.write(180); // Open the gate
  } else {
    // turn LED off:
    digitalWrite(ledPin, LOW);
  }
}

