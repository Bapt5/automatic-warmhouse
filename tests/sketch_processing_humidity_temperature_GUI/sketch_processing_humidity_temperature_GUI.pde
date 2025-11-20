import processing.serial.*;

// Global Variables
Serial myPort;      // The serial port object
float humidity = 0.0;
float temperature = 0.0;
float h = 0.0;
float t = 0.0;

void setup() {
  size(600, 400); // Set the size of the visualization window

  // 1. Check for available serial ports
  println(Serial.list());
  String portName = Serial.list()[2]; // Tries to use the first port in the list

  // 2. Initialize the serial port (must match the baud rate in Arduino)
  myPort = new Serial(this, portName, 9600);

  // Read one line at a time
  myPort.bufferUntil('\n');
}

void draw() {
  background(255); // Dark background

  // Display the data
  fill(0); // White text
  textSize(32);

  // Humidity
  text("Humidity: " + nf(humidity, 0, 2) + "%", 50, height/2 - 30);

  // Temperature
  text("Temperature: " + nf(temperature, 0, 2) + "°C", 50, height/2 + 30);

  // Add a simple visual representation
  noStroke();
  fill(0, 150, 255, 150); // Blue for humidity bar. The last one is transparency
  h = map(humidity, 0, 100, 0, 300); //humiditi is RH so relative humidity to the current air at current temperature
  rect(width - 250, 350 - h, 50, h);

  fill(255, 0, 0, 150); // Red for temperature
  t = map(temperature, 0, 50, 0, 300); //humiditi is RH so relative humidity to the current air at current temperature

  rect(width - 150, 350 - t, 50, t);
}

// 3. Serial Event Handler
// This function is called every time a newline character ('\n') is received.
void serialEvent(Serial myPort) {
  // Read the serial string until the newline
  String inString = myPort.readStringUntil('\n');

  if (inString != null) {
    // Trim leading/trailing whitespace (important!)
    inString = trim(inString);

    // Split the string based on the comma delimiter
    String[] pieces = split(inString, ',');

    // Check if we have two pieces (Humidity, Temperature)
    if (pieces.length >= 2) {
      try {
        // Convert the string pieces to floating-point numbers
        humidity = float(pieces[0]);
        temperature = float(pieces[1]);

        // Clamp values to sane ranges to prevent visual glitches
        humidity = constrain(humidity, 0, 100);
        temperature = constrain(temperature, -50, 150);
      }
      catch (NumberFormatException e) {
        // Handle cases where the data might be corrupted or non-numeric
        println("Error parsing data: " + inString);
      }
    }
  }
}
