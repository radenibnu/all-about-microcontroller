#include <Servo.h> // Library for controlling the barrier gate servo
// #include <Wire.h> // Library for I2C communication
// #include <Adafruit_VC0706.h> // Library for the camera module (Adafruit VC0706)

Servo barrierGate; // Create a servo object

// Initialize the camera
// Adafruit_VC0706 cam = Adafruit_VC0706(&Serial1);
const int buttonPin = 10; //Pin untuk Button
const int ledPin = 3; // Pin untuk LED

void setup() {
  Serial.begin(9600);
  // Serial1.begin(38400); // Initialize camera's serial communication

  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);

  // Attach the servo to the desired pin
  barrierGate.attach(9);
  barrierGate.write(85);

  // Initialize the camera
  // if (!cam.begin()) {
  //   Serial.println(F("Failed to find the camera"));
  //   while (1);
  // }

  // Camera setup
  // cam.setImageSize(VC0706_640x480); // Set the image size
  // cam.setCompression(90); // Set image compression (0 to 100)
  // cam.tvOn(); // Turn on the camera's image transmission
}

void loop() {
  // 1. Motor/mobil datang untuk masuk ke parkir, kemudian pengendara menekan tombol untuk mengeluarkan karcis parkir dan camera capture plat motor/mobil
  // if (buttonIsPressed()) {
  //   issueParkingTicket(); // Generate parking ticket
  //   captureVehicleImage(); // Capture vehicle image
  //   delay(1000); // Delay to ensure the gate stays open for a moment
  // }

  buttonIsPressed();

  // barrierGate.write(70);

  // if (buttonIsPressed()){
  //   openBarrierGate();
  // }

  // 3. Setelah itu barrier gate terbuka
}

bool buttonIsPressed() {
  // Implement the logic to check if the button is pressed
  // Return true if the button is pressed, false otherwise
 int sensorvalue = digitalRead(buttonPin);

  if (sensorvalue == LOW){
    Serial.print("value button :");
    Serial.println(sensorvalue);
    digitalWrite(ledPin, HIGH);
    barrierGate.write(0);
  }
  else {
    digitalWrite(ledPin, LOW);
  }
}

// void issueParkingTicket() {
//   // Implement the logic to issue a parking ticket
//   // Generate a parking ticket number
//   int ticketNumber = random(1000, 9999); // Nomor acak sebagai contoh

//   // Print the ticket number to the serial monitor
//   Serial.print("Parking Ticket: #");
//   Serial.println(ticketNumber);
// }

// void captureVehicleImage() {
//   // Implement the logic to capture an image of the vehicle's license plate
//   // Save the image to an SD card or send it to a server
//   // Check if the camera is available
//   if (!cam.takePicture()) {
//     Serial.println(F("Failed to take picture"));
//     return;
//   }

//   Serial.println(F("Taking picture..."));

//   // Create a file to save the image
//   File imgFile = SD.open("vehicle.jpg", FILE_WRITE);
//   if (!imgFile) {
//     Serial.println(F("Failed to create image file"));
//     return;
//   }

//   // Read the image data and save it to the file
//   uint8_t *buffer;
//   size_t jpgSize = cam.readPicture(buffer);
//   if (jpgSize > 0) {
//     imgFile.write(buffer, jpgSize);
//     Serial.println(F("Image saved to SD card as vehicle.jpg"));
//   } else {
//     Serial.println(F("Failed to read image data"));
//   }

//   // Close the image file
//   imgFile.close();

//   // Delete the taken picture from the camera's buffer
//   if (!cam.resumeVideo()) {
//     Serial.println(F("Failed to resume video"));
//   }
// }

// void openBarrierGate() {
//   // Implement the logic to open the barrier gate (e.g., using the servo)
//   barrierGate.write(90); // Open the gate
//   delay(2000); // Delay to keep the gate open for a while
//   barrierGate.write(0); // Close the gate
// }
