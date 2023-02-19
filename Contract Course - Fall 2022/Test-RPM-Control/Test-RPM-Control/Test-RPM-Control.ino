// Griffin White
// 10-11-2022
// Fall 2022 Contract Course
// RPM Control Test


// Including the "Timer" & "CheapStepper" libraries.
#include "Timer.h"
#include "CheapStepper.h"

// Defining a constant for the SENSOR pin on the Arduino.
#define SENSOR 2
#define NUM_MAGNETS 8
#define MIN_RPM 100
#define MAX_RPM 900

const int in1 = 3, in2 = 4, in3 = 5, in4 = 6; // Pins for the stepper motor driver.

// A boolean variable which records if the hall-effect sensor is currently being activated.
boolean currentlyActive = false;
// An int variable which tracks the number of times which the sensor is actiavted (number of revolutions).
int sensorActivations = 0;
Timer timeElapsed(MILLIS);
CheapStepper stepper(in1, in2, in3, in4);  // CheapStepper Object.

void setup() {
  Serial.begin(9600);
  pinMode(SENSOR, INPUT_PULLUP);
}

void loop() {

  
  double desiredRpm = 900;
  double rpm = 0;
  boolean directionFlag = false;
  
 // If the hall-effect sensor has been activated, and the flag has not yet been marked.
  if ((digitalRead(SENSOR) == LOW) && (currentlyActive == false)){
    // If the timer isn't running.
    if (timeElapsed.state() == STOPPED)
    {
      // Start the timer.
      timeElapsed.start();
    }

    // Increment the number of sensorActivations.
    sensorActivations++;
    // Record that the sensor is currentlyActive.
    currentlyActive = true;  
    // Printing out the current number of sensorActivations.
    // Serial.print ("Sensor Activations: ");
    // Serial.println(sensorActivations);
  }

  // If the sensor is no longer receiving a signal.
  if (digitalRead(SENSOR) == HIGH){
    // Set currentlyActive to false.
    currentlyActive = false;
  }

  // This block of code calculates the RPM every 20 revolutions.
  // If the sensor has been activated 20 times and the timer is running.
  if (sensorActivations == 20 && timeElapsed.state() == RUNNING){
    // Pausing the timer.
    timeElapsed.pause();
    double pulsesPerMinute = ((double)sensorActivations / timeElapsed.read()) * 60000;
    rpm = pulsesPerMinute / NUM_MAGNETS;
    // Printing out the time elapsed and the number of revolutions.
    Serial.print ("Time Elapsed: ");
    Serial.print (timeElapsed.read());
    Serial.print (" ms    Revolutions: ");
    Serial.print (sensorActivations);
    Serial.print ("\n   RPM: ");
    // Converting revolutions/milisecond to revolutions/minute.
    Serial.print (rpm);
    Serial.println("\n");
    
    // Resetting the timer.
    timeElapsed.stop();
    // Resetting the number of sensorActivations.
    sensorActivations = 0;

    double rpmDifference = desiredRpm - rpm;
    if (rpmDifference < 0)
      directionFlag = true;
    else
      directionFlag = false;

    if (abs(rpmDifference) > 2){
      Serial.print ("RPM discrepency: ");
      Serial.print (rpmDifference);
      Serial.println();

      int numSteps = (int)abs(rpmDifference*2.3);
//
//      Serial.print ("numSteps: ");
//      Serial.print (numSteps);
    
      for (int i=0; i<numSteps; i++){
        stepper.step(directionFlag);
      }
      
    }

  }  

  

  
}
