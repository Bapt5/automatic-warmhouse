#include <Servo.h>

Servo myservo;         // create servo object to control a servo
int pos = 0;           // variable to store the servo position
int receivedNumber = 0; // Variable to store the converted integer

int currentAngle = 90;
const int WATERING_ANGLE = 66; // Angle for watering (open/start flow)
const int STOP_ANGLE = 88;     // Angle for stopping water (closed/stop flow)
const int MOVE_DELAY = 150;

void setup() {
  myservo.attach(6);   // attaches the servo on pin 5 to the servo object (Note: Changed from 3 to 5 based on your original code)
  Serial.begin(9600);  // Start serial communication at 9600 baud rate
  Serial.println("Servo Control Ready. Enter '1' for 0 degrees, or '0' for 90 degrees.");
  myservo.write(currentAngle);  //init
}

void loop() {
  // Check if there is data available to read from the serial port
  if (Serial.available() > 0) {
    // Read the oldest byte in the serial buffer

    receivedNumber = Serial.parseInt();
    Serial.print("You entered: ");
    Serial.println(receivedNumber);  // Print the converted number

    if(receivedNumber == 1)
      startWatering();
    else if (receivedNumber == 0)
      stopWatering();

    // Serial.print("Set to ");
    // Serial.print(receivedNumber);
    // Serial.println(" degrees.");//66 start to water and above this, it stops watering
    while (Serial.available() > 0) {
      Serial.read();
    }
  }
  // The servo maintains its last written position until a new command is received.
  // No delay is needed here as the servo will just hold its position.
  
}

void startWatering() {
  if (currentAngle > WATERING_ANGLE) {
    Serial.println("Starting water flow (Moving to 66 degrees)...");
    
    // Loop decrements the angle slowly
    for (int pos = currentAngle; pos >= WATERING_ANGLE; pos--) { 
      myservo.write(pos);             // tell servo to go to position in variable 'pos'
      delay(MOVE_DELAY);              // waits for MOVE_DELAY milliseconds for the servo to reach position
    }
    currentAngle = WATERING_ANGLE;
    Serial.println("Watering started (Angle: 66).");
  } else if (currentAngle == WATERING_ANGLE) {
    Serial.println("Already watering.");
  } else {
    Serial.println("Servo is already below 66, check current angle.");
  }
}

/**
 * Moves the servo slowly from its current angle to the stopped angle (88 degrees).
 */
void stopWatering() {
  if (currentAngle < STOP_ANGLE) {
    Serial.println("Stopping water flow (Moving to 88 degrees)...");
    
    // Loop increments the angle slowly
    for (int pos = currentAngle; pos <= STOP_ANGLE; pos++) { 
      myservo.write(pos);             // tell servo to go to position in variable 'pos'
      delay(MOVE_DELAY);              // waits for MOVE_DELAY milliseconds for the servo to reach position
    }
    currentAngle = STOP_ANGLE;
    Serial.println("Watering stopped (Angle: 88).");
  } else if (currentAngle == STOP_ANGLE) {
    Serial.println("Already stopped.");
  } else {
    Serial.println("Servo is already above 88, check current angle.");
  }
}