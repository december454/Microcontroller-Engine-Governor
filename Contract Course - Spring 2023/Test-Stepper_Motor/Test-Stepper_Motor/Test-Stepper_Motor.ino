// Griffin White
// 2-13-2022
// Spring 2022 Contract Course
// Test Stepper Motor 

/*  This program will test the operaation of the carburetor stepper motor. 
 *  It will completely close the throttle, completely open the throttle, pause, and then repeat the cycle. */

#include "CheapStepper.h"   // Stepper motor control library.

// Pins for each input of the stepper motor driver.
const int in1 = 38, in2 = 40, in3 = 42, in4 = 44; 
// Pin for the limit switch.
const int limitSwitch = 12;

// Flag for which direction the stepper motor will rotate.
boolean directionFlag = true;

// CheapSteppr object, initialized with the appropriate pin numbers.
CheapStepper stepperMotor(in1, in2, in3, in4);

void setup() {
  // Setting the limitSwitch pinMode to INPUT_PULLUP.
  pinMode(limitSwitch,INPUT_PULLUP);

}

void loop() {
  // Loop which will cycle 1150 times. There are ~ 1150 motor steps between fully closed and fully open throttle.
  for (int steps = 0; steps < 1150; steps ++){
    // Rotating the motor one step.
    stepperMotor.step(directionFlag); 
    // If the limit switch has been actiavted / throttle is completely closed.
    if (digitalRead(limitSwitch) == LOW){
      // Set the direction flag to false. This will case the stepper to open the throttle.
      directionFlag = false;
      // Reset the number of steps remaining.
      steps = 0;
    }    
    //delay(5);  
  }

  // Brief pause.
  delay(200);
  // Set the direction flag to true. This will cause the stepper to close the throttle.
  directionFlag = true;  
}
