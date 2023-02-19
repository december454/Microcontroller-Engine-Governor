// Griffin White
// 9-25-2022
// Fall 2022 Contract Course
// Hall-Effect Sensor RPM Test

// Including the "Timer" library.
#include "Timer.h"

// Defining a constant for the SENSOR pin on the Arduino.
#define SENSOR 2
#define NUM_MAGNETS 8

// A boolean variable which records if the hall-effect sensor is currently being activated.
boolean currentlyActive = false;
// An int variable which tracks the number of times which the sensor is actiavted (number of revolutions).
int sensorActivations = 0;
// Creating a Timer object to record the time elapsed..
Timer timeElapsed(MILLIS);

// Setup function.
void setup() {
  // Setting the pinMode of the SENSOR pin to INPUT_PULLUP.
  pinMode(SENSOR, INPUT_PULLUP);
  // Starting the serial output, 9600 baud.
  Serial.begin(9600);
}

// Main loop function.
void loop() {  
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
    double rpm = pulsesPerMinute / NUM_MAGNETS;
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
  }  
}
