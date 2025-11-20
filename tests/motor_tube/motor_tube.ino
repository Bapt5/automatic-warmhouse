#include <Servo.h>

Servo myservo;           // create servo object to control a servo
int pos = 0;             // variable to store the servo position
int receivedNumber = 0;  // Variable to store the converted integer

const int STOP_ANGLE = 111;  // Angle for stopping water (closed/stop flow)
int currentAngle = STOP_ANGLE;
const int WATERING_ANGLE = 66;  // Angle for watering (open/start flow)
const int MOVE_DELAY = 100;

void setup() {
  myservo.attach(6);   // attaches the servo on pin 5 to the servo object (Note: Changed from 3 to 5 based on your original code)
  Serial.begin(9600);  // Start serial communication at 9600 baud rate
  Serial.println("Servo Control Ready. Enter '1' for 0 degrees, or '0' for 90 degrees.");
  delay(1000);
  myservo.write(STOP_ANGLE);
  stopWatering();
}

void loop() {
  // Check if there is data available to read from the serial port
  if (Serial.available() > 0) {
    // Read the oldest byte in the serial buffer

    receivedNumber = Serial.parseInt();
    Serial.print("You entered: ");
    Serial.println(receivedNumber);  // Print the converted number

    if (receivedNumber == 1)
      myservo.write(WATERING_ANGLE);
    else if (receivedNumber == 0)
      myservo.write(STOP_ANGLE);


    while (Serial.available() > 0) {
      Serial.read();
    }
  }
}
