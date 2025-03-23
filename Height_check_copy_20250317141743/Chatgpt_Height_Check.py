#include <Wire.h>
#include <VL53L0X.h>
#include <TM1637Display.h>

// Define pins
#define LASER_RX_PIN 2   // Laser receiver pin
#define DISPLAY_CLK 3    // Clock pin for display
#define DISPLAY_DIO 4    // Data pin for display

// Create sensor and display objects
VL53L0X tofSensor;                                // ToF sensor object
TM1637Display display(DISPLAY_CLK, DISPLAY_DIO);  // Display object

// Variable for user-input minimum height (in cm)
int minHeight_cm = 0;

// Debounce settings for laser input
unsigned long debounceDelay = 50; // 50ms debounce
int lastLaserState = HIGH;
unsigned long lastDebounceTime = 0;

// Variables to manage measurement timing
bool measurementInProgress = false;
unsigned long measurementTime = 0;
const unsigned long measurementInterval = 30000; // 30 seconds delay after a measurement

// Variables for Serial status message
unsigned long lastStatusTime = 0;

void setup() {
  Serial.begin(9600);
  
  pinMode(LASER_RX_PIN, INPUT);

  Wire.begin();               // Start I2C for ToF sensor
  tofSensor.init();           // Initialize ToF sensor
  tofSensor.setTimeout(500);  // Set timeout for ToF readings

  display.setBrightness(7);
  display.clear();

  // Get minimum height from user with a timeout
  Serial.println("Enter minimum height required (in cm) within 10 seconds, or default (100 cm) will be used:");
  unsigned long startTime = millis();
  while (Serial.available() == 0 && (millis() - startTime) < 10000) {
    // Wait for user input for up to 10 seconds
  }
  if (Serial.available() > 0) {
    minHeight_cm = Serial.parseInt();
  } else {
    minHeight_cm = 100; // default value if no input is received
    Serial.println("No input received. Default minimum height set to 100 cm.");
  }
  Serial.print("Minimum height set to: ");
  Serial.print(minHeight_cm);
  Serial.println(" cm");
  Serial.println("System Ready!");
}

void loop() {
  int currentLaserState = digitalRead(LASER_RX_PIN);
  
  // Debounce the laser input
  if (currentLaserState != lastLaserState) {
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // If the beam is broken and no measurement is in progress, take a measurement
    if (currentLaserState == LOW && !measurementInProgress) {
      measurementInProgress = true;  // Start measurement process
      
      // Read distance from ToF sensor (in millimeters)
      int distance = tofSensor.readRangeSingleMillimeters();
      if (tofSensor.timeoutOccurred()) {
        Serial.println("Sensor timeout! Please try again.");
        display.showNumberDec(8888);  // Error code displayed
      } else {
        int height_cm = distance / 10;  // Convert mm to cm
        
        // Check if height meets the minimum requirement
        if (height_cm >= minHeight_cm) {
          display.showNumberDec(height_cm);
          Serial.print("Height: ");
          Serial.print(height_cm);
          Serial.println(" cm - You can ride!");
        } else {
          display.showNumberDec(9999); // Indicate too short
          Serial.print("Height: ");
          Serial.print(height_cm);
          Serial.println(" cm - You can't ride!");
        }
      }
      
      measurementTime = millis(); // Record the time of this measurement
    }
  }
  
  // Manage the measurement delay (non-blocking)
  if (measurementInProgress && (millis() - measurementTime >= measurementInterval)) {
    measurementInProgress = false; // Ready for next measurement
    display.clear();
  }
  
  // Print a status message when no measurement is in progress
  if (currentLaserState == HIGH && !measurementInProgress) {
    if (millis() - lastStatusTime >= 1000) { // every 1 second
      Serial.println("Determining height...");
      lastStatusTime = millis();
    }
  }
  
  lastLaserState = currentLaserState;
}
