// Libraries
#include <Wire.h>
#include <VL53L0X.h>
#include <TM1637Display.h>

// Define pins (change pins later idk how the setup looks like)
#define LASER_RX_PIN 2  // Laser receiver


// Create objects for sensors/display
VL53L0X tofSensor;                                // ToF sensor object

// // Create a variable to store user-input minimum height
int minHeight_cm = 0; 
int standHeight = 32;

void setup() {
  Serial.begin(9600);  
  //Serial.println("Hi ");  
  // start laser        
  pinMode(LASER_RX_PIN, OUTPUT);  

  Wire.begin();               // Start I2C for ToF sensor
  tofSensor.init();           // Initialize ToF sensor
  tofSensor.setTimeout(500);  // Set a timeout for ToF readings (after reading takes place)
      

  // Get minimum height from user
  Serial.println("Enter minimum height required (in cm): ");
  while (Serial.available() == 0) {
    // Wait for user input
  }
  minHeight_cm = Serial.parseInt(); // Read the integer from Serial input
  Serial.print("Minimum height set to: ");
  Serial.print(minHeight_cm);
  Serial.println(" cm");

  Serial.println("System Ready!"); 
}

void loop() {
  int laserState = analogRead(A0); // Read laser receiver
  
  if (laserState < 950) {    // Beam broken 
    Serial.println("laser state low");
    int distance = tofSensor.readRangeSingleMillimeters(); // Read ToF in mm
    int height_cm = distance / 10; // Convert to cm
    height_cm = height_cm + standHeight;
    delay(1000);
    
    // Check if height meets minimum requirement
    if (height_cm >= minHeight_cm) {
      
      
      Serial.print("Height: ");
      Serial.print(height_cm);
      Serial.println(" cm - You can ride!");
    } else {
      
      Serial.print("Height: ");
      Serial.print(height_cm);
      Serial.println(" cm - You can't ride!");
    }
    
    delay(1000000000);
    
  } else {
    
    Serial.println("Determining height...");
    delay(100);               // Small delay
  }
}