int sensorPin = A1; // select the input pin for the potentiometer

void setup()
{
  Serial.begin(9600);
  Serial.println("Adafruit Analog Light Sensor Test");
}
void loop()
{
  // read the raw value from the sensor:
  int rawValue = analogRead(sensorPin);
  Serial.print("Raw = ");
  Serial.print(rawValue);
  float LuxValue = pow(10, rawValue * 5.0 / 1024.0);
  Serial.print(" - Lux = ");
  Serial.println(LuxValue);
  delay(1000);
}