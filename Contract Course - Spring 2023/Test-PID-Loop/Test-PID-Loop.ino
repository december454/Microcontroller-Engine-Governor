// Griffin White
// 2-21-2023
// Spring 2023 Contract Course
// Engine Governor: PID Loop Control Test

#include "Timer.h"            // Timer library.
Timer timeElapsed(MILLIS);    // Timer object.
Timer displayUpdate(MILLIS); 
const int hallSensorPin = 2;  // Pin for hall effect sensor.
const int numMagnets = 1;     // Number of magnets on the flywheel.

double rpm = 0;               // Current RPM.
int sensorActivations = 0;    // Number of times the hall sensor has been activated.

void setup() {
  // Setting the SWITCH pinMode to INPUT_PULLUP.
  pinMode(hallSensorPin,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(hallSensorPin),countPulse,FALLING);
  Serial.begin(9600);
  displayUpdate.start();
}

void loop() {
  
  if (sensorActivations >= 4){
    rpm = (((double)sensorActivations / timeElapsed.read()) * 60000) / numMagnets;
    sensorActivations = 0;
    timeElapsed.start();
  }

  if(displayUpdate.read() >= 1000){
    Serial.print("RPM: ");
    Serial.println(rpm);
    displayUpdate.start();    
  }

  
  
}

void countPulse(){
      
  sensorActivations++;      // Increment the number of sensorActivations.    
  
}
