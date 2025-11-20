import processing.serial.*;
import http.requests.*;


Serial serialPort;
String receivedData = "";

float airTemp, airHum, soilSimuHum, waterLevelCm, lightIntensity;
int WATER_MIN_CM = 3;    

PFont displayFont;
int backgroundColor = color(30, 30, 40);

// Low light threshold
final int LIGHT_LOW_THRESHOLD = 700; 
final String FLASK_ENDPOINT = "http://localhost:5000/receive_data"; 
final String RESET_ENDPOINT = "http://localhost:5000/reset_data";

void setup() {
  size(600, 450); 
  displayFont = createFont("Arial", 24, true);
  textFont(displayFont);
  textAlign(LEFT, CENTER);
  
   resetFlaskData();
  
  printArray(Serial.list());
  
  String portName = Serial.list()[4]; 
  serialPort = new Serial(this, portName, 115200);
  
  serialPort.bufferUntil('\n');
}

void draw() {
  background(backgroundColor);
  
  fill(255);
  text("Automatic Greenhouse", 50, 50);
  
  // Affichage des données
  fill(255, 200, 0); 
  text("Temperature (Air): " + airTemp + " °C", 50, 120);

  fill(100, 200, 255); 
  text("Humidity (Air): " + airHum + " %", 50, 180);

  fill(150, 75, 0); 
  text("Humidity (Simu Soil): " + soilSimuHum + " %", 50, 240); 

  fill(0, 255, 150); 
  text("Water level: " + waterLevelCm + " cm", 50, 300);
  

  fill(255, 255, 100); 
  text("Luminosity: " + lightIntensity, 50, 360);
  
  if (waterLevelCm < WATER_MIN_CM) {
    fill(255, 50, 50);
    text("ALERT ! You have to refuel the tank", 300, 50);
  }
}


void serialEvent(Serial p) {
  receivedData = p.readStringUntil('\n');

  if (receivedData != null) {
    receivedData = trim(receivedData);

    if (receivedData.length() > 0) {
      try {
        // Find the position of every tag
        int iTA = receivedData.indexOf("TA");
        int iHA = receivedData.indexOf("HA");
        int iHS = receivedData.indexOf("HS");
        int iN  = receivedData.indexOf("N"); 
        int iL  = receivedData.indexOf("L"); 

        // CHECK: Ensure all tags exist and are in the correct order
        if (iTA != -1 && iHA > iTA && iHS > iHA && iN > iHS && iL > iN) {
          
          // "TA" is 2 chars long, so start at iTA + 2
          String sTA = receivedData.substring(iTA + 2, iHA).trim();
          airTemp = Float.parseFloat(sTA);

          // "HA" is 2 chars long -> +2
          String sHA = receivedData.substring(iHA + 2, iHS).trim();
          airHum = Float.parseFloat(sHA);

          // "HS" is 2 chars long -> +2
          String sHS = receivedData.substring(iHS + 2, iN).trim();
          soilSimuHum = Float.parseFloat(sHS);

          // "N" is ONLY 1 char long -> start at iN + 1
          String sN = receivedData.substring(iN + 1, iL).trim();
          waterLevelCm = Float.parseFloat(sN);

          // "L" is ONLY 1 char long -> start at iL + 1
          String sL = receivedData.substring(iL + 1).trim();
          lightIntensity = Float.parseFloat(sL);
          
          // Use thread() to run the network request asynchronously
          thread("postDataThread");
        }
      } 
      catch (Exception e) {
        // This prints errors without stopping the program
        println("Reading error (ignoring packet): " + e);
      }
    }
  }
}

void postDataThread() {
    JSONObject json = new JSONObject();
    json.setFloat("air_temperature", airTemp);
    json.setFloat("air_humidity", airHum);
    json.setFloat("soil_humidity", soilSimuHum);
    json.setFloat("water_level", waterLevelCm);
    json.setFloat("light_intensity", lightIntensity);

    // Create the PostRequest object from the new library
    PostRequest post = new PostRequest(FLASK_ENDPOINT);
    
    // Set header and body
    // Flask expects Content-Type: application/json
    post.addHeader("Content-Type", "application/json"); 
    post.addData(json.toString());

    // Send the request
    try {
        post.send();
        String response = post.getContent();
        println("Flask Data Post Response: " + response);
    } catch (Exception e) {
        println("Error sending data to Flask: " + e);
    }
}

void resetFlaskData() {
    // We can use a synchronous call in setup() since it doesn't interrupt real-time processes.
    PostRequest resetRequest = new PostRequest(RESET_ENDPOINT);
    
    // We set Content-Type for consistency, even if the body is empty.
    resetRequest.addHeader("Content-Type", "application/json"); 
    
    try {
        resetRequest.send();
        String response = resetRequest.getContent();
        println("Flask Data Post Response: " + response);
    } catch (Exception e) {
        println("Error resetting data on Flask: " + e);
    }
}
