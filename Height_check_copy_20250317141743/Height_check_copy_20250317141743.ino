// Libraries
#include <Wire.h>
#include <VL53L0X.h>
#include <TM1637Display.h>

// Define pins (change pins later idk how the setup looks like)
#define LASER_RX_PIN 2  // Laser receiver
#define DISPLAY_CLK 3   // Clock pin 
#define DISPLAY_DIO 4   // Data pin f

// Create objects for sensors/display
VL53L0X tofSensor;                                // ToF sensor object
TM1637Display display(DISPLAY_CLK, DISPLAY_DIO);  // Display object


// // Create a variable to store user-input minimum height
int minHeight_cm = 0; 

void setup() {
  Serial.begin(9600);    
  // Set laser receiver pin as input        
  pinMode(LASER_RX_PIN, INPUT);  

  Wire.begin();               // Start I2C for ToF sensor
  tofSensor.init();           // Initialize ToF sensor
  tofSensor.setTimeout(500);  // Set a timeout for ToF readings (after reading takes place)

  display.setBrightness(7);  
  display.clear();          

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
  int laserState = digitalRead(LASER_RX_PIN); // Read laser receiver
  
  if (laserState == LOW) {    // Beam broken 
    int distance = tofSensor.readRangeSingleMillimeters(); // Read ToF in mm
    int height_cm = distance / 10; // Convert to cm
    
    // Check if height meets minimum requirement
    if (height_cm >= minHeight_cm) {
      display.showNumberDec(height_cm); // Show height on display
      
      Serial.print("Height: ");
      Serial.print(height_cm);
      Serial.println(" cm - You can ride!");
    } else {
      display.showNumberDec(9999); // Too short!
      Serial.print("Height: ");
      Serial.print(height_cm);
      Serial.println(" cm - You can't ride!");
    }
    
    while (true) {
      // Infinite loop until the system is rebooted? might change let's see
      // delay(10000);  
    }
  } else {
    display.clear();          // Clear display when beam is not broken
    Serial.println("Determining height...");
    delay(100);               // Small delay
  }
}