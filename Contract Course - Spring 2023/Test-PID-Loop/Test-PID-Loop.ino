// Griffin White
// 2-21-2023
// Spring 2023 Contract Course
// Engine Governor: PID Loop Control Test

const int hallSensorPin = 2;                  // Pin for hall effect sensor.
const int numMagnets = 8;                     // Number of magnets on the flywheel.
const int rpmPrecision = 2;                   // Acceptable discrepency between desired and actual RPM.
const int in1 = 3, in2 = 4, in3 = 5, in4 = 6; // Pins for the stepper motor driver.
const int updateInterval = 8;                 // Number of pulses between each calculation of the RPM.

double desiredRpm = 500;      // Desired RPM.
double rpm = 0;               // Current RPM.
double rpmDiff = 0;           // Difference between the current RPM and desired RPM.
int sensorActivations = 0;    // Number of times the hall sensor has been activated.
int stepsRemaining = 0;       // Number of stepper motor steps remaining.
bool throttleRpmUpdated = false;
boolean directionFlag = false; 

// double Kp=2, Ki=5, Kd=1;        // PID variables.
// double Kp=2, Ki=20, Kd=10;        // PID variables.
// double Kp=15, Ki=1, Kd=1; 
// double Kp=.0, Ki=0, Kd=2; 
double Kp=1, Ki=.00001, Kd=.000001; 
double pidOutput = 0;       

#include <Wire.h>               // LCD library.
#include <LiquidCrystal_I2C.h>  // LCD library.
#include <PID_v1.h>             // PID library.
#include "Timer.h"              // Timer library.
#include "CheapStepper.h"       // Stepper motor control library.

Timer timeElapsed(MILLIS);      // Timer object for RPM calculations.
Timer displayUpdate(MILLIS);    // Timer object for updating the LCD.

PID throttlePid(&rpm, &pidOutput, &desiredRpm, Kp, Ki, Kd, DIRECT);
CheapStepper stepperMotor(in1, in2, in3, in4);  // CheapStepper Object.
LiquidCrystal_I2C lcd(0x27,20,4);               // LCD Object.

void setup() {
  pinMode(hallSensorPin,INPUT_PULLUP);  // Setting the hallSensorPin pinMode to INPUT_PULLUP.
  attachInterrupt(digitalPinToInterrupt(hallSensorPin),countPulse,FALLING); // Attctching an interrupt to the hallSensorPin.
  Serial.begin(9600);                   // Initializing serial output.
  displayUpdate.start();                // Starting the display update timer.
  throttlePid.SetMode(AUTOMATIC);       // Starting the throttlePid control.
  throttlePid.SetOutputLimits(0,200);   // Settingthe output limits for the throttlePid.
}

void loop() {
  // If the number of sensorActivations is sufficient to calculate the RPM.
  if (sensorActivations >= updateInterval)    
    calcRpm();

  // If the current RPM is outside of the acceptable range.
  if (abs(rpmDiff) > rpmPrecision)
    adjustThrottle();
  else
    stepperMotor.stop();
  

  if(displayUpdate.read() >= 1000){
    Serial.print("RPM: ");
    Serial.println(rpm);
    Serial.print("pidOutput: ");
    Serial.println(pidOutput);
    displayUpdate.start();    
  }   
}

// Method for calculating the average RPM after a specified number of revolutions.
void calcRpm(){
  // Calculating RPM: (sensor pulses / (time elapsed * 60 sec * 1000 ms)) / number of magnets on flywheel.
  rpm = (((double)sensorActivations / timeElapsed.read()) * 60000) / numMagnets;
  sensorActivations = 0;      // Resetting the number of sensor activations.
  timeElapsed.start();        // Resetting the timer.
  rpmDiff = desiredRpm - rpm; // Calculating the difference between the current RPM and desired RPM.  

  // If the RPM is above the desired value.
  if (rpmDiff < 0){
    directionFlag = true;                         // Setting the stepper motor direction to clockwise (increase throttle).
    throttlePid.SetControllerDirection(REVERSE);  // Setting the throttle PID loop for reverse (above desired speed) operation.
  }
  // Else the RPM is below the desired value.
  else{
    directionFlag = false;                        // Setting the stepper motor direction to counter-clockwise (decrease throttle).
    throttlePid.SetControllerDirection(DIRECT);   // Setting the throttle PID loop for direct (above desired speed) operation.   
  }      
  throttleRpmUpdated = true;  // Flagging that the RPM was just updated.
}

// Method for adjusting the throttle.
void adjustThrottle(){
  // If the RPM has been updated since the last method call, recalculate the number of stepper motor steps.
  if (throttleRpmUpdated){
    throttlePid.Compute();            // Compute the output of the throttle PID loop (the corrections to be made).
    stepsRemaining = abs(pidOutput);  // Set stepsRemaining equal to the output of the PID loop.
    throttleRpmUpdated = false;       // Flagging that the new RPM has been acknowldged.
  }

  if (stepsRemaining > 0){      

    stepperMotor.step(directionFlag);
    stepsRemaining--;
  }  
}

void countPulse(){
      
  sensorActivations++;      // Increment the number of sensorActivations.    
  
}
