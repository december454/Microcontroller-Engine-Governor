// Griffin White
// 2-21-2023
// Spring 2023 Contract Course
// Engine Governor: PID Loop Control Test

const int hallSensorPin = 2;                  // Pin for hall effect sensor.
const int numMagnets = 1;                     // Number of magnets on the flywheel.
const int rpmPrecision = 2;                   // Acceptable discrepency between desired and actual rpm.
const int in1 = 3, in2 = 4, in3 = 5, in4 = 6; // Pins for the stepper motor driver.





double desiredRpm = 500;
double Kp=2, Ki=5, Kd=1;        // PID variables.



#include <Wire.h>               // LCD Library.
#include <LiquidCrystal_I2C.h>  // LCD Library.
#include <PID_v1.h>
#include "Timer.h"              // Timer library.
#include "CheapStepper.h"       // Stepper motor control library.

double rpm = 0;               // Current RPM.
int sensorActivations = 0;    // Number of times the hall sensor has been activated.

double Output;

Timer timeElapsed(MILLIS);    // Timer object for RPM calculations.
Timer displayUpdate(MILLIS);  // Timer object for updating the LCD.
PID thorttlePid(&rpm, &Output, &desiredRpm, Kp, Ki, Kd, DIRECT);
CheapStepper stepperMotor(in1, in2, in3, in4);  // CheapStepper Object.
LiquidCrystal_I2C lcd(0x27,20,4);





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
