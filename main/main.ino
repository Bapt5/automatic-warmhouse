#include <DHT.h>
#include <Servo.h>

#define DHT_AIR_PIN 2
#define DHT_SOIL_PIN 3
#define DHTTYPE DHT11
#define TRIG_PIN 12 
#define ECHO_PIN 11     
#define LIGHT_PIN 4
#define WATERRING_LED_PIN 5
const int SERVO_PIN = 6;

Servo wateringServo;

const int LIGHT_SENSOR_PIN = A1;

DHT dhtAir(DHT_AIR_PIN, DHTTYPE);
DHT dhtSoil(DHT_SOIL_PIN, DHTTYPE);

float airTemp = 0;
float airHum = 0;
float soilSimuHum = 0;
float waterLevelCm = 0;
int lightIntensity = 0;

long lastReadTime = 0;
const long readingInterval = 500; 

long endOfWatering = 0;

const int SOIL_LOW_THRESHOLD_SIMU = 16; 
const int LIGHT_LOW_THRESHOLD = 150; 
const int WATER_MIN_CM = 3;
const int WATERING_DURATION = 5000;
const int BOTTLE_HEIGHT_CM = 11x;

// variable for watering control
const int WATERING_ANGLE = 66; // Angle for watering (open/start flow)
const int STOP_ANGLE = 111;     // Angle for stopping water (closed/stop flow)

void setup() {
  Serial.begin(115200); 

  dhtAir.begin();
  dhtSoil.begin();

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LIGHT_PIN, OUTPUT);
  pinMode(WATERRING_LED_PIN, OUTPUT);

  digitalWrite(LIGHT_PIN, LOW);
  digitalWrite(WATERRING_LED_PIN, LOW);

  wateringServo.attach(SERVO_PIN);
  wateringServo.write(STOP_ANGLE);
}


void loop() {
  if (millis() - lastReadTime >= readingInterval) {
    lastReadTime = millis();
    readSensors();
    controlLogic();
    sendSerialData();
  }

  if (millis() > endOfWatering) {
    wateringOff();
  }
}


void readSensors() {
  airHum = dhtAir.readHumidity();
  airTemp = dhtAir.readTemperature();

  soilSimuHum = dhtSoil.readHumidity();

  if (isnan(airHum) || isnan(airTemp) || isnan(soilSimuHum)) {
    return;
  }

  lightIntensity = pow(10, analogRead(LIGHT_SENSOR_PIN) * 5.0 / 1024.0);
  waterLevelCm = readUltrasonicSensor();
}


float readUltrasonicSensor() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);
  
  float distance = duration * 0.034 / 2;

  return max(0.0, BOTTLE_HEIGHT_CM - distance);
}


void controlLogic() {
  if (lightIntensity < LIGHT_LOW_THRESHOLD) {
    digitalWrite(LIGHT_PIN, HIGH); 
  } else {
    digitalWrite(LIGHT_PIN, LOW);
  }

  if (soilSimuHum < SOIL_LOW_THRESHOLD_SIMU && waterLevelCm > WATER_MIN_CM && millis() > endOfWatering) {
    watering();
  }
}


void watering() {
  // Serial.println("--- SIMULATION ARROSAGE EN COURS ---");
  wateringOn();
  endOfWatering = millis() + WATERING_DURATION;
}

void wateringOn() {
  digitalWrite(WATERRING_LED_PIN, HIGH);
  wateringServo.write(WATERING_ANGLE);
}

void wateringOff() {
  digitalWrite(WATERRING_LED_PIN, LOW);
  wateringServo.write(STOP_ANGLE);
}

void sendSerialData() {
  char tempStr[6], humAirStr[6], humSolStr[6], waterLevelStr[6], lightStr[6];
  dtostrf(airTemp, 4, 1, tempStr);
  dtostrf(airHum, 4, 1, humAirStr);
  dtostrf(soilSimuHum, 4, 1, humSolStr);
  dtostrf(waterLevelCm, 4, 1, waterLevelStr);
  dtostrf(lightIntensity, 4, 1, lightStr);

  Serial.print("TA");
  Serial.print(tempStr);
  Serial.print("HA");
  Serial.print(humAirStr);
  Serial.print("HS");
  Serial.print(humSolStr);
  Serial.print("N");
  Serial.print(waterLevelStr);
  Serial.print("L");
  Serial.print(lightStr);
  Serial.println();
}