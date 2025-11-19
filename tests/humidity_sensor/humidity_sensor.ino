#include <DHT.h>

// Define the pin connected to the DATA pin of DHT22
#define DHTPIN 2
#define DHTTYPE DHT22

// Initialize the DHT sensor
DHT dht(DHTPIN, DHTTYPE);

void setup() {
Serial.begin(9600);
dht.begin();
}

void loop() {
delay(2000); // Wait 2 seconds between readings

// Read humidity and temperature
float humidity = dht.readHumidity();
float tempC = dht.readTemperature(); // Celsius
float tempF = dht.readTemperature(true); // Fahrenheit

// Check if readings failed
if (isnan(humidity) || isnan(tempC) || isnan(tempF)) {
Serial.println("Failed to read from DHT22 sensor!");
return;
}

// Print results
Serial.print("Humidity: ");
Serial.print(humidity);
Serial.print("% | Temperature: ");
Serial.print(tempC);
Serial.print("°C ~ ");
Serial.print(tempF);
Serial.println("°F");
}