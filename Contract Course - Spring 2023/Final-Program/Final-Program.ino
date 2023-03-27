// Griffin White
// 3-8-2023
// Spring 2023 Contract Course
// Engine Governor: Final Program

const String version = "1.14";
/* Version 1.14 Changes:
 * * * * * * * * * * * *
 * PID Tuning. 
 * Dynamic D Control Removed
 * D RPM Delta Filtered
 * Startup steps.
 */

#include <LiquidCrystal.h>      // LCD library.
#include "Timer.h"              // Timer library.
#include "CheapStepper.h"       // Stepper motor control library.

const double Kp=.01, Ki=0.0, Kd=100;                  // PID gain variables.
//const double Kp=.02, Ki=0, Kd=6;
const int maxSteps = 1200;                        // Maximum number of steps that the stepper motor can take before reaching end of travel.
const int startupSteps = 800;                     // The number of steps from wide-open which the stepper motor will open the throttle for startup.
const int minRpm = 300;                           // The minimum RPM value where the controller will try to adjust the throttle. (Prevents the system from going to full throttle during startup.)
const int stallTimeout = 200;                     // The minimum amount of time (ms) between pulses when the engine is considered stalled.
const int hallSensorPin = 2;                      // Pin for hall effect sensor.
const int stepperSwitchPin = 52;                  // Pin for stepper disable / enable switch.
const int limitSwitch = 12;                       // Pin for the limit switch.
const int desiredRpm = 3600;                      // Desired RPM.
const int rpmPrecision = 2;                       // Acceptable discrepency between desired and actual RPM.
const int rpmPrecisionI = rpmPrecision + 200;      // Descripency where PID integral tuning will come into play.
const int numMagnets = 1;                         // Number of magnets on the flywheel.
const int rpmCalcInterval = 1;                    // Number of revolutions between each RPM calculation.
const int lcdChar = 16, lcdRow = 2;               // LCD display dimensions.
const int lcdContrast = 90;                       // Contrast value (0-255) for the LCD display.
const int lcdUpdateInterval = 400;                // Update interval (ms) for the LCD display.
const int serialUpdateInterval = 50;              // Update interval (ms) for the serial output.  
const int in1 = 38, in2 = 40, in3 = 42, in4 = 44; // Pins for the stepper motor driver.
const int vo = 9, rs = 8, en = 7, d4 = 6, d5 = 5, d6 = 4, d7 = 3; // Pins for the LCD.
const String lcdHeader = " Set  | Curr RPM";  // Text header displayed during normal operation.

double rpm = 0;                     // Current RPM.
double rpmDiff = 0;                 // Difference between the current RPM and desired RPM.
double rpmDiffPrev = 0;             // The rpmDiff value from the prior update. (Used for PID calculations.)
int sensorActivations = 0;          // Number of times the hall sensor has been activated.
int stepsRemaining = 0;             // Number of stepper motor steps remaining.
int pidP = 0, pidI = 0, pidD = 0;   // Output variables for the PID loop.
int stringIndex = 0;                // Current index within a String being printed to the LCD display.
bool throttleRpmUpdated = false;    // If the RPM has just been calculated.
bool directionFlag = true;          // If the stepper motor should rotate clockwise (increase throttle) or counter-clockwise (decrease throttle).
bool engineRunning = false;         // If the engine is running.
bool stepperInitialized = false;    // If the stepper has been initialized.
String stringRpm = "0";             // String representation of the current rpm. 

Timer timeElapsed(MICROS);        // Timer object for RPM calculations.
Timer displayUpdateTimer(MILLIS); // Timer object for updating the LCD.
Timer pidTimeElapsed(MILLIS);     // Timer object for PID change-over-time calculations.
Timer serialOutputTimer(MILLIS);  // Timer object for serial output.

CheapStepper stepperMotor(in1, in2, in3, in4);  // CheapStepper Object.

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);      // LCD display object.

void setup() {
  pinMode(stepperSwitchPin,INPUT_PULLUP);     // Setting the stepperSwitchPin pinMode to INPUT_PULLUP.
  pinMode(hallSensorPin,INPUT_PULLUP);        // Setting the hallSensorPin pinMode to INPUT_PULLUP.
  pinMode(limitSwitch,INPUT_PULLUP);          // Setting the limitSwitch pinMode to INPUT_PULLUP.
  attachInterrupt(digitalPinToInterrupt(hallSensorPin),countPulse,FALLING); // Attctching an interrupt to the hallSensorPin.
  Serial.begin(115200);                       // Initializing serial output.
  analogWrite(vo, lcdContrast);               // Setting the LCD contrast.
  lcd.begin(lcdChar, lcdRow);                 // Initializing the LCD display.
  displayUpdateTimer.start();                 // Starting the display update timer.
  pidTimeElapsed.start();                     // Starting the PID derivative timer.
  serialOutputTimer.start();                  // Starting the serial output timer.
  
  printDebugInfo();
  initializeLcd();
  initializeStepper();
}

void loop() {
  // If the number of sensorActivations is sufficient to calculate the RPM.
  if (sensorActivations >= rpmCalcInterval)    
    calcRpm();

  // If the current RPM is outside of the acceptable range and above the minimum.
  if (abs(rpmDiff) > rpmPrecision && rpm > minRpm)
    adjustThrottle();
  else
    stepperMotor.stop();

  // If it is time to update the values on the LCD display.
  if (displayUpdateTimer.read() >= lcdUpdateInterval){
    updateDisplay();
  }

  // If it is time to print the serial output.
  if (serialOutputTimer.read() >= serialUpdateInterval){
    serialOutput();
  }

  // If the rpm has risen above the minRpm. (The engine has started.)
  if (rpm > minRpm)
    // Flagging that the engine is running.
    engineRunning = true;

  // If the engine has not been started or it was running and has stopped.
  if ((timeElapsed.read() / 1000 > stallTimeout && engineRunning) || (!stepperInitialized && !engineRunning)){
    // Initializig the stepper motor, preparing for the engine to be restarted.
    initializeStepper();
  }  
}

// Interrupt method for recording distinct signals from the hall effect sensor.
void countPulse(){      
  sensorActivations++;      // Increment the number of sensorActivations.      
}

// Method for calculating the average RPM after a specified number of revolutions.
void calcRpm(){
  // Calculating RPM: (sensor pulses / (time elapsed * 60 sec * 1000000 microseconds)) / number of magnets on flywheel.
  rpm = (((double)sensorActivations / timeElapsed.read()) * 60000000) / numMagnets;
  sensorActivations = 0;      // Resetting the number of sensor activations.
  timeElapsed.start();        // Resetting the timer.
  rpmDiffPrev = rpmDiff;
  rpmDiff = desiredRpm - rpm; // Calculating the difference between the current RPM and desired RPM.  
        
  throttleRpmUpdated = true;  // Flagging that the RPM was just updated.
}

// Method for calculating a PID output, based on the discrpency between the current and desired RPM.
int calculatePid(){  
  // Calculating Proportional Value: (P-Gain * RPM Difference)
  pidP = Kp * rpmDiff;

  int rpmDelta = rpmDiff - rpmDiffPrev;

  if(abs(rpmDelta) < 20){
    pidD = 0;
  }
  
  else{
    if (rpmDelta < 0)
      rpmDelta += 20;
    else
      rpmDelta -= 20;
  
  
    // Calculating Derivative Value: (D-Gain * (Change in RPM / Time Elapsed))
//    if (abs(rpmDiff) <= 200){
//      if (abs(rpmDiff) <= 100)
//        pidD = 0;
//      else
//        pidD = ((Kd * ((rpmDiff - 100) * (rpmDiff - 100))/10000)) * ((rpmDelta) / pidTimeElapsed.read());
//    }
//    else  
      pidD = Kd * ((rpmDelta) / pidTimeElapsed.read());

  }
  if (rpmDiff < 250 && pidD > 0){
    pidD = 0;
  }
    

  // If the RPM is near the target and the integral calculation will have a meaningful effect.
  if (abs(rpmDiff) < rpmPrecisionI)
    // Calculating Integral Value: Current Integral Value + (I-Gain * RPM Difference)
    pidI += pidI + (Ki * rpmDiff);
  // Else, the integral value is 0;
  else
    pidI = 0;

  // Restarting the PID timer.
  pidTimeElapsed.start();

  // Finding the total of the functions.
  int total = pidP + pidI + pidD;
  // int total = pidP;

  // If the total is positive. (The throttle needs to increase.)
  if (total >= 0)
    directionFlag = false;        // Setting the stepper motor direction to clockwise (increase throttle).
  // Else the total is negative. (The throttle needs to decrease.)
  else
    directionFlag = true;         // Setting the stepper motor direction to counter-clockwise (decrease throttle).
  
  return abs(total);  
}

// Method for adjusting the throttle.
void adjustThrottle(){
  // If the stepper switch is turned on.
  if (digitalRead(stepperSwitchPin) == LOW){
    // If the RPM has been updated since the last method call, recalculate the number of stepper motor steps.
    if (throttleRpmUpdated){
      stepsRemaining = calculatePid();  // Set stepsRemaining equal to the output of the PID loop.
      throttleRpmUpdated = false;       // Flagging that the new RPM has been acknowldged.
    }
  
    // If there are stepper motor steps remaining, and it isn't attempting to open beyond wide-open.
    if ((stepsRemaining > 0) && (directionFlag || digitalRead(limitSwitch) == HIGH)){      
      stepperMotor.step(directionFlag); // Stepping in the desired direction.
      stepsRemaining--;                 // Decrementing the remaining steps.
      // Flagging that the stepper motor has been moved from its initial position.
      stepperInitialized = false;
    }
  }
}

// Method for updating the LCD display.
void updateDisplay(){
  if (stringIndex == 7){
    stringIndex = 0;
    lcd.setCursor(8,1);
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
  // If the stepper switch is turned on.
  if (digitalRead(stepperSwitchPin) == LOW){
    // Flagging that the throttle should open.
    directionFlag = false;
    // Loop which will cycle 1200 times. There are ~ 1200 motor steps between fully closed and fully open throttle.
    for (int steps = 0; steps < maxSteps; steps ++){
      // Rotating the motor one step.
      stepperMotor.step(directionFlag); 
      // If the limit switch has been actiavted / throttle is wide open.
      if (digitalRead(limitSwitch) == LOW){
        // Set the direction flag to true. This will cause the stepper to close the throttle.
        directionFlag = true;
        // Reset the number of steps remaining.
        steps = maxSteps - startupSteps;
      }    
    }
    // Flagging that the engine is not running.
    engineRunning = false;
    // Setting the rpm to 0.
    rpm = 0;
    // Flagging that the stepper has been initialized.
    stepperInitialized = true;
  }

  stepperMotor.stop();
}

void serialOutput(){
  serialOutputTimer.start();
  Serial.print(millis());
  Serial.print(',');
  Serial.print(rpm);
  Serial.print(',');
  if (directionFlag)
    Serial.print(stepsRemaining * -1);
  else
    Serial.print(stepsRemaining);
  Serial.print(',');
  Serial.print(pidP);
  Serial.print(',');
  Serial.print(pidI);
  Serial.print(',');
  Serial.print(pidD);
  Serial.print(',');
  Serial.println((rpmDiff - rpmDiffPrev));
}

void printDebugInfo(){
  Serial.println ((String)"------ Microcontroller Engine Governor - Version: " + version + " ------");
  Serial.println ("-------------------------------------------------------------");
  
  Serial.print   ((String)"PID Gains:        Kp: ");
  Serial.print   (Kp, 6);
  Serial.print   ("   Ki: ");
  Serial.print   (Ki, 6);
  Serial.print   ("   Kd: ");
  Serial.print   (Kd, 6);
  
  Serial.println ("\n-------------------------------------------------------------");
  Serial.println ((String)"Set RPM:             " + ((int)desiredRpm)        + "    |  Min RPM:                " + minRpm);
  Serial.println ("                             |");
  Serial.println ((String)"RPM Precision        " + rpmPrecision      + "       |  PID I Precision:         " + rpmPrecisionI);
  Serial.println ("                             |");
  Serial.println ((String)"Start Steps:         " + startupSteps      + "     |  Max Steps:               " + maxSteps);
  Serial.println ("                             |");
  Serial.println ((String)"RPM Calc Interval:   " + rpmCalcInterval    + "       |  Num Magnets:             " + numMagnets);
  Serial.println ("                             |");
  Serial.println ((String)"LCD Update Interval: " + lcdUpdateInterval + "     |  Serial Update Interval:  " + serialUpdateInterval);
  Serial.println ("-------------------------------------------------------------\n");
  Serial.println ((String)"Time,RPM,Steps Remaining, PID p, PID i, PID d, RPM Change");
}
