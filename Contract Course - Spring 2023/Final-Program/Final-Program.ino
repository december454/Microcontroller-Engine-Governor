// Griffin White
// 2-26-2023
// Spring 2023 Contract Course
// Engine Governor: Final Program - Version 1

#include <Wire.h>               // LCD library.
#include <LiquidCrystal_I2C.h>  // LCD library.
#include "Timer.h"              // Timer library.
#include "CheapStepper.h"       // Stepper motor control library.

const double Kp=.1, Ki=0, Kd=0;               // PID gain variables.
const int maxSteps = 1200;                        // Maximum number of steps that the stepper motor can take before reaching end of travel.
const int startupSteps = 300;                     // The number of steps which the stepper motor will open the throttle for startup.
const int minRpm = 300;                           // The minimum RPM value where the controller will try to adjust the throttle. (Prevents the system from going to full throttle during startup.)
const int hallSensorPin = 2;                      // Pin for hall effect sensor.
const int limitSwitch = 12;                       // Pin for the limit switch.
const int desiredRpm = 3600;                      // Desired RPM.
const int rpmPrecision = 2;                       // Acceptable discrepency between desired and actual RPM.
const int rpmPrecisionI = rpmPrecision + 20;      // Descripency where PID integral tuning will come into play.
const int numMagnets = 1;                         // Number of magnets on the flywheel.
const int rpmCalcInterval = 8;                    // Number of revolutions between each RPM calculation.
const int lcdContrast = 30;                       // Contrast value (0-255) for the LCD display.
const int lcdUpdateInterval = 400;                // Update interval (ms) for the LCD display.
const int lcdChar = 16, lcdRow = 2;               // LCD display dimensions.
const int in1 = 38, in2 = 40, in3 = 42, in4 = 44; // Pins for the stepper motor driver.
const int vo = 9, rs = 13, en = 12, d4 = 11, d5 = 10, d6 = 8, d7 = 7; // Pins for the LCD.
const String lcdHeader = " Set  | Curr RPM";  // Text header displayed during normal operation.

double rpm = 0;                     // Current RPM.
double rpmDiff = 0;                 // Difference between the current RPM and desired RPM.
double rpmDiffPrev = 0;             // The rpmDiff value from the prior update. (Used for PID calculations.)
int sensorActivations = 0;          // Number of times the hall sensor has been activated.
int stepsRemaining = 0;             // Number of stepper motor steps remaining.
int pidP = 0, pidI = 0, pidD = 0;   // Output variables for the PID loop.
int stringIndex = 0;                // Current index within a String being printed to the LCD display.
bool throttleRpmUpdated = false;    // If the RPM has just been calculated.
bool directionFlag = true;         // If the stepper motor should rotate clockwise (increase throttle) or counter-clockwise (decrease throttle).
String stringRpm = "0";                // String representation of the current rpm. 

Timer timeElapsed(MILLIS);        // Timer object for RPM calculations.
Timer displayUpdateTimer(MILLIS); // Timer object for updating the LCD.
Timer pidTimeElapsed(MILLIS);     // Timer object for PID change-over-time calculations.

CheapStepper stepperMotor(in1, in2, in3, in4);  // CheapStepper Object.

LiquidCrystal_I2C lcd(0x27,16,2);               // LCD display object.

void setup() {
  pinMode(hallSensorPin,INPUT_PULLUP);  // Setting the hallSensorPin pinMode to INPUT_PULLUP.
  pinMode(limitSwitch,INPUT_PULLUP);    // Setting the limitSwitch pinMode to INPUT_PULLUP.
  attachInterrupt(digitalPinToInterrupt(hallSensorPin),countPulse,FALLING); // Attctching an interrupt to the hallSensorPin.
  Serial.begin(9600);                   // Initializing serial output.
  displayUpdateTimer.start();           // Starting the display update timer.
  pidTimeElapsed.start();               // Starting the PID derivative timer.
  
  initializeLcd();
  initializeStepper();
}

void loop() {
  // If the number of sensorActivations is sufficient to calculate the RPM.
  if (sensorActivations >= rpmCalcInterval)    
    calcRpm();

  // If the current RPM is outside of the acceptable range.
  if (abs(rpmDiff) > rpmPrecision && rpm > minRpm)
    adjustThrottle();
  else
    stepperMotor.stop();

  // If it is time to update the values on the LCD display.
  if (displayUpdateTimer.read() >= lcdUpdateInterval){
    updateDisplay();
  }


  
}

// Interrupt method for recording distinct signals from the hall effect sensor.
void countPulse(){      
  sensorActivations++;      // Increment the number of sensorActivations.      
}

// Method for calculating the average RPM after a specified number of revolutions.
void calcRpm(){
  // Calculating RPM: (sensor pulses / (time elapsed * 60 sec * 1000 ms)) / number of magnets on flywheel.
  rpm = (((double)sensorActivations / timeElapsed.read()) * 60000) / numMagnets;
  sensorActivations = 0;      // Resetting the number of sensor activations.
  timeElapsed.start();        // Resetting the timer.
  rpmDiff = desiredRpm - rpm; // Calculating the difference between the current RPM and desired RPM.  

  // If the RPM is above the desired value.
  if (rpmDiff < 0)
    directionFlag = true;     // Setting the stepper motor direction to clockwise (decrease throttle).
  
  // Else the RPM is below the desired value.
  else
    directionFlag = false;    // Setting the stepper motor direction to counter-clockwise (increase throttle).
        
  throttleRpmUpdated = true;  // Flagging that the RPM was just updated.
}

// Method for calculating a PID output, based on the discrpency between the current and desired RPM.
int calculatePid(){  
  // Calculating Proportional Value: (P-Gain * RPM Difference)
  pidP = Kp * rpmDiff;
  // Calculating Derivative Value: (D-Gain * (Change in RPM / Time Elapsed))
  pidD = Kd * ((rpmDiff - rpmDiffPrev) / pidTimeElapsed.read());

  // If the RPM is near the target and the integral calculation will have a meaningful effect.
  if (abs(rpmDiff) > rpmPrecisionI)
    // Calculating Integral Value: Current Integral Value + (I-Gain * RPM Difference)
    pidI += pidI + (Ki * rpmDiff);
  // Else, the integral value is 0;
  else
    pidI = 0;

  // Restarting the PID timer.
  pidTimeElapsed.start();

  // Returning the total PID output.
  return abs(pidP + pidI + pidD);    
}

// Method for adjusting the throttle.
void adjustThrottle(){
  // If the RPM has been updated since the last method call, recalculate the number of stepper motor steps.
  if (throttleRpmUpdated){
    stepsRemaining = calculatePid();  // Set stepsRemaining equal to the output of the PID loop.
    throttleRpmUpdated = false;       // Flagging that the new RPM has been acknowldged.
  }

  // If there are stepper motor steps remaining.
  if (stepsRemaining > 0){      
    stepperMotor.step(directionFlag); // Stepping in the desired direction.
    stepsRemaining--;                 // Decrementing the remaining steps.
  }  
}

// Method for updating the LCD display.
void updateDisplay(){
  if (stringIndex == 7){
    stringIndex = 0;
    lcd.setCursor(8,1);

    Serial.print("RPM: ");
    Serial.println(rpm);
    Serial.print("Steps Remaining: ");
    Serial.println(stepsRemaining);
    
    displayUpdateTimer.start();    
  }
  else{
    if (stringIndex == 0){
      stringRpm = String(rpm) + "        ";      
    }
      
    lcd.print(stringRpm.charAt(stringIndex));
    stringIndex++;   
     
  }    
}

void initializeLcd(){
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(lcdHeader);
  lcd.setCursor(1,1);
  lcd.print(desiredRpm);
  lcd.setCursor(6,1);
  lcd.print('|');
  lcd.setCursor(8,1);
}

void initializeStepper(){
  // Loop which will cycle 1150 times. There are ~ 1150 motor steps between fully closed and fully open throttle.
  for (int steps = 0; steps < maxSteps; steps ++){
    // Rotating the motor one step.
    stepperMotor.step(directionFlag); 
    // If the limit switch has been actiavted / throttle is completely closed.
    if (digitalRead(limitSwitch) == LOW){
      // Set the direction flag to false. This will case the stepper to open the throttle.
      directionFlag = false;
      // Reset the number of steps remaining.
      steps = maxSteps - startupSteps;
    }    
  }

  stepperMotor.stop();
}
