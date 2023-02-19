// Griffin White
// 11-5-2022
// Fall 2022 Contract Course
// Engine Governor: State-Based Design - LCD Output Rewrite + Adaptive Throttle Aggression

const int hallSensorPin = 2;                  // Pin for hall effect sensor.
const int rpmPrecision = 2;                   // Acceptable discrepency between desired and actual rpm.
const int minRpm = 100, maxRpm = 950;         // Maximum and minimum RPM values.
const int numMagnets = 8;                     // Number of magnets on the flywheel.
const int rpmCalcInterval = 20;               // Number of revolutions between each RPM calculation.
const int lcdContrast = 30;                   // Contrast value (0-255) for the LCD display.
const int lcdChar = 16, lcdRow = 2;           // LCD display dimensions.
const int in1 = 3, in2 = 4, in3 = 5, in4 = 6; // Pins for the stepper motor driver.
const int vo = 9, rs = 13, en = 12, d4 = 11, d5 = 10, d6 = 8, d7 = 7; // Pins for the LCD.
const byte keyColPins [4] = {24,28,32,36};    // Pins for the keypad rows.
const byte keyRowPins [4] = {23,27,31,35};    // Pins for the keypad columns.
const String normalHeader = "Set RPM:  RPM: ";
const String inputHeader = "Enter new RPM: ";

#include "Timer.h"          // Timer library.
#include "CheapStepper.h"   // Stepper motor control library.
#include "LiquidCrystal.h"  // LCD display library.
#include "Keypad.h"         // Keypad matrix library.

boolean userInput = false;            // If user has started entering input.
boolean hallActive = false;           // If the hall effect sensor is activated.
boolean throttleRpmUpdated = false;   // If the RPM has just been updated, for throttle control.
boolean displayRpmUpdated = false;    // If the RPM has just been updated, for display output.
boolean normalHeaderPrinted = false;  // If the normal header is fully printed on the LCD,
boolean keyPressed = false;           // If the user is pressing down a key.

int sensorActivations = 0;    // Number of times the hall sensor has been activated.
int stepsRemaining = 0;       // Number of stepper motor steps remaining.
int currentChar = 0;          // Index of the character currently being printed to the display.
double rpm = 0;               // Current RPM.
double desiredRpm = 500;      // Desired RPM.
double rpmDiff = 0;           // Difference between the current RPM and desired RPM.
char keyInput;                // Key currently being pressed by the user.
String inputString = "";      // Raw input String entered by the user.

// 2d array which represents the keypad layout.
char keyMatrix [4][4] = { 
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}};
  
Keypad userKeypad (makeKeymap(keyMatrix), keyRowPins, keyColPins, 4, 4); // Keypad oject.
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);      // LiquidCrystal object.
Timer timeElapsed(MILLIS);                      // Timer object for calculating RPM.
Timer timeCorrecting(MILLIS);                   // Timer object for calculating how long the system has been correcting the RPM.
CheapStepper stepperMotor(in1, in2, in3, in4);  // CheapStepper Object.

void setup() {
  pinMode(hallSensorPin, INPUT_PULLUP); // 
  analogWrite(vo, lcdContrast);         //
  lcd.begin(lcdChar, lcdRow);           //
  Serial.begin(9600);
}

void loop() {  

  // If the hall sensor has just started recieving a signal.
  if (!(hallActive) && (digitalRead(hallSensorPin) == LOW))
    recordPulse(); 

  // If the hall sensor is no longer receiving a signal.
  if (digitalRead(hallSensorPin) == HIGH)
    hallActive = false; // Set currentlyActive to false.
  
  // If the number of sensorActivations is sufficient to calculate the RPM.
  if (sensorActivations == rpmCalcInterval)
    calcRpm();

  // If the RPM is out of range.  
  if (abs(rpmDiff) > rpmPrecision)
    adjustThrottle();
  else{
    stepperMotor.stop();
    if (timeCorrecting.state() == RUNNING){
      timeCorrecting.pause();
      Serial.print("Time required to correct RPM: ");
      Serial.print(timeCorrecting.read());
      Serial.println(" ms");
      timeCorrecting.stop();
    }
  }

  if (displayRpmUpdated){
    updateDisplay();
  }

  keyInput = userKeypad.getKey();
  
  if (keyInput)
    takeInput();
  else
    keyPressed = false;  
}

void recordPulse() {
  if (timeElapsed.state() == STOPPED){ // If the timer isn't running.
      timeElapsed.start();  // Start the timer.
    }       
    sensorActivations++;      // Increment the number of sensorActivations.    
    hallActive = true;   // Flag that the sensor is currentlyActive.
}

void calcRpm(){
    timeElapsed.pause(); // Pause the timer.
    // Calculating RPM: (sensor pulses / (time elapsed * 60 sec * 1000 ms)) / number of magnets on flywheel.
    rpm = (((double)sensorActivations / timeElapsed.read()) * 60000) / numMagnets;
    rpmDiff = desiredRpm - rpm; // Calculate the difference between the current RPM and desired RPM.
    timeElapsed.stop();         // Reset the timer.    
    sensorActivations = 0;      // Reset the number of sensor activations.
    throttleRpmUpdated = true;         // Flag that the RPM was just updated.
    displayRpmUpdated = true;
}

void adjustThrottle(){
  if (timeCorrecting.state() == STOPPED) // If the timer isn't running.
    timeCorrecting.start();
  
  if (throttleRpmUpdated){
    stepsRemaining = (int)abs(rpmDiff*1.1);
    throttleRpmUpdated = false;
  }

  if (stepsRemaining > 0){
    boolean directionFlag = false;
    if (rpmDiff < 0)
      directionFlag = true;

    stepperMotor.step(directionFlag);
    stepsRemaining--;
  }
}

void updateDisplay(){

  if (userInput){
    if (normalHeaderPrinted){
      if (currentChar == 0)
        lcd.clear();
      
      lcd.print(inputHeader.charAt(currentChar));
      currentChar++;

      if (currentChar == normalHeader.length()-1){
        normalHeaderPrinted = false;
        currentChar = 0;
      }       
    }

    else{
      lcd.setCursor(0, 1);
      lcd.print(inputString);
    }
  }

  else {

    if (!(normalHeaderPrinted)){      
      if (currentChar == 0)
        lcd.clear();
      
      if (currentChar == 9){
        lcd.print(desiredRpm);
        lcd.setCursor(0, 1);
      }    
     
      else
        lcd.print(normalHeader.charAt(currentChar));
 
      currentChar++;

      if (currentChar == normalHeader.length()-1){
        normalHeaderPrinted = true;
        currentChar = 0;
      }
    }

    else{
      displayRpmUpdated = false;
      currentChar = 0;
      lcd.setCursor(5, 1);
      lcd.print(rpm);      
    }             
  }
}

void takeInput(){

  userInput = true;
  
  if (!(keyPressed)){
    
    keyPressed = true;

    switch (keyInput) {
      case '#':
        updateDesiredRpm(); 
        break;
      case 'A':
      case 'B':
      case 'C':
      case 'D':
      case '*':
        break;
      default:
        inputString += keyInput;
        break;
    }
    updateDisplay();
  }   
}

void updateDesiredRpm (){
  int newValue = inputString.toInt();
  if (newValue >= minRpm && newValue <= maxRpm){
    desiredRpm = newValue;        
  }

  inputString = "";
  userInput = false;
  updateDisplay();  
}
