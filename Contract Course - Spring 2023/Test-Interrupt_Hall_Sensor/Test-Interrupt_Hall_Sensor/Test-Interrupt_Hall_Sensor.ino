// Griffin White
// 2-19-2023
// Spring 2023 Contract Course
// Engine Governor: Interrupt Hall Effect Sensor Test

#include "Timer.h"            // Timer library.
Timer timeElapsed(MILLIS);    // Timer object.

const int hallSensorPin = 2;  // Pin for hall effect sensor.
const int numMagnets = 1;     // Number of magnets on the flywheel.

double rpm = 0;               // Current RPM.
boolean displayNeedsUpdate = false;    // If the RPM has just been updated, for display output.

int sensorActivations = 0;    // Number of times the hall sensor has been activated.

void setup() {
  // Setting the SWITCH pinMode to INPUT_PULLUP.
  pinMode(hallSensorPin,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(hallSensorPin),countPulse,FALLING);
  Serial.begin(9600);
}

void loop() {
  if (timeElapsed.read() > 1000){
    rpm = (((double)sensorActivations / timeElapsed.read()) * 60000) / numMagnets;
    sensorActivations = 0;
    timeElapsed.stop();
    timeElapsed.start();
    displayNeedsUpdate = true;
  }

  if(displayNeedsUpdate){
    Serial.print("RPM: ");
    Serial.println(rpm);
    displayNeedsUpdate = false;
  }

  
  
}

void countPulse(){
  if (timeElapsed.state() == STOPPED){ // If the timer isn't running.
      timeElapsed.start();  // Start the timer.
  }
      
  sensorActivations++;      // Increment the number of sensorActivations.    
  
}
