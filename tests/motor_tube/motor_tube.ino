#include <Servo.h>

Servo myservo;         // create servo object to control a servo
int pos = 0;           // variable to store the servo position
int receivedNumber = 0; // Variable to store the converted integer
void setup() {
  myservo.attach(5);   // attaches the servo on pin 5 to the servo object (Note: Changed from 3 to 5 based on your original code)
  Serial.begin(9600);  // Start serial communication at 9600 baud rate
  Serial.println("Servo Control Ready. Enter '1' for 0 degrees, or '0' for 90 degrees.");
  myservo.write(90);  //init
}

void loop() {
  // Check if there is data available to read from the serial port
  if (Serial.available() > 0) {
    // Read the oldest byte in the serial buffer

    receivedNumber = Serial.parseInt();
    Serial.print("You entered: ");
    Serial.println(receivedNumber);  // Print the converted number

    myservo.write(receivedNumber);

    Serial.print("Set to ");
    Serial.print(receivedNumber);
    Serial.println(" degrees.");//66 start to water and above this, it stops watering
    while (Serial.available() > 0) {
      Serial.read();
    }
  }
  // The servo maintains its last written position until a new command is received.
  // No delay is needed here as the servo will just hold its position.
  
}