// Griffin White
// 11-6-2022
// Fall 2022 Contract Course
// Engine Governor: State-Based Design - LCD Output Rewrite

const int hallSensorPin = 2;                    // Pin for hall effect sensor.
const int rpmPrecision = 2;                     // Acceptable discrepency between desired and actual rpm.
const int minRpm = 100, maxRpm = 950;           // Maximum and minimum RPM values.
const int numMagnets = 8;                       // Number of magnets on the flywheel.
const int rpmCalcInterval = 20;                 // Number of revolutions between each RPM calculation.
const int lcdContrast = 30;                     // Contrast value (0-255) for the LCD display.
const int lcdChar = 16, lcdRow = 2;             // LCD display dimensions.
const int in1 = 3, in2 = 4, in3 = 5, in4 = 6;   // Pins for the stepper motor driver.
const int vo = 9, rs = 13, en = 12, d4 = 11, d5 = 10, d6 = 8, d7 = 7; // Pins for the LCD.
const byte keyColPins [4] = {24,28,32,36};      // Pins for the keypad rows.
const byte keyRowPins [4] = {23,27,31,35};      // Pins for the keypad columns.
const String normalHeader = "Set RPM:  RPM: ";  // Text header displayed during normal operation.
const String inputHeader = "Enter new RPM: ";   // Text header displayed while the user is entering input.

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
Timer timeElapsed(MILLIS);                      // Timer onject.
CheapStepper stepperMotor(in1, in2, in3, in4);  // CheapStepper Object.

void setup() {
  pinMode(hallSensorPin, INPUT_PULLUP); // Setting the pin mode for the hall sensor.
  analogWrite(vo, lcdContrast);         // Setting the LCD contrast.
  lcd.begin(lcdChar, lcdRow);           // Initializing the LCD display.
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
  else
    stepperMotor.stop();

  // If the RPM has been updated and needs to be displayed.
  if (displayRpmUpdated){
    updateDisplay();
  }

  // Reading in any user input.
  keyInput = userKeypad.getKey();

  // If a key is currently being pressed.
  if (keyInput)
    takeInput();
  else
    keyPressed = false;  
}

// Method for recording pulses from the hall sensor / trigger wheel.
void recordPulse() {
  if (timeElapsed.state() == STOPPED) // If the timer isn't running.
    timeElapsed.start();  // Start the timer.
    
  sensorActivations++; // Increment the number of sensorActivations.    
  hallActive = true;   // Flag that the sensor is currentlyActive.
}

// Method for calculating the average RPM after a specified number of revolutions.
void calcRpm(){
    timeElapsed.pause(); // Pause the timer.
    // Calculating RPM: (sensor pulses / (time elapsed * 60 sec * 1000 ms)) / number of magnets on flywheel.
    rpm = (((double)sensorActivations / timeElapsed.read()) * 60000) / numMagnets;
    rpmDiff = desiredRpm - rpm; // Calculate the difference between the current RPM and desired RPM.
    timeElapsed.stop();         // Reset the timer.    
    sensorActivations = 0;      // Reset the number of sensor activations.
    throttleRpmUpdated = true;  // Flag that the RPM was just updated.
    displayRpmUpdated = true;   // Flag that the RPM was just updated.
}

// Method for adjusting the throttle.
void adjustThrottle(){
  // If the RPM has been updated since the last method call.
  if (throttleRpmUpdated){
    stepsRemaining = (int)abs(rpmDiff*1.1); // Calculating the number of stepper motor steps remaining. 
    throttleRpmUpdated = false;             // Recording that the updated RPM has been acknowledged.
  }

  // If there are still stepper motor steps remaining.
  if (stepsRemaining > 0){
    boolean directionFlag = false; // Setting the stepper motor direction to counterclockwise (decrease throttle).
    // If the RPM difference is negative (too slow).
    if (rpmDiff < 0)
      directionFlag = true; // Setting the stepper motor direction to clockwise (increase throttle).

    stepperMotor.step(directionFlag); // Stepping in the desired direction.
    stepsRemaining--;                 // Decrementing the remaining steps.
  }
}

// Method for updating the LCD display.
void updateDisplay(){
  // If the user is entering input.
  if (userInput){
    // If the normal header is printed on the display. (Print the user input header.)
    if (normalHeaderPrinted){
      // If no character have been printed for the input header.
      if (currentChar == 0)
        lcd.clear(); // Clear the LCD display.

      lcd.print(inputHeader.charAt(currentChar)); // Print a character.
      currentChar++; // Increment to the next character.

      // If this is the last character.
      if (currentChar == normalHeader.length()-1){
        normalHeaderPrinted = false; // Flag that the normal header is no longer printed.
        currentChar = 0;             // Set the current character to zero.
      }       
    }

    // Else, the user input header is printed.
    else{
      lcd.setCursor(0, 1);    // Set the cusor location.
      lcd.print(inputString); // Print the user's input.
    }
  }

  // Else, there is no user input. (Print the normal RPM display)
  else {
    // If the normal header is not printed on the LCD. (Print the normal header.)
    if (!(normalHeaderPrinted)){
      // If no characters have been printer for the normal header.
      if (currentChar == 0)
        lcd.clear(); // Clear the LCD display.

      // If this is the end of the first line.
      if (currentChar == 9){
        lcd.print(desiredRpm);  // Print the desired RPM.
        lcd.setCursor(0, 1);    // Move the curor to the second line.
      }    

     // Else, this is not the end of the first line.
      else
        lcd.print(normalHeader.charAt(currentChar)); // Print a character.
 
      currentChar++; // Increment to the next character.

      // If this is the last character.
      if (currentChar == normalHeader.length()-1){
        normalHeaderPrinted = true; // Flag that the normal header is now printed.
        currentChar = 0;            // Set the current character to zero.
      }
    }

    // Else, the normal header is printed on the LCD.
    else{
      displayRpmUpdated = false;  // Mark that the updated RPM has been acknowledged.      
      lcd.setCursor(5, 1);        // Set the cursor to the start of the second line.
      lcd.print(rpm);             // Print the current RPM.
    }             
  }
}

// Method for taking user input and changing the desired RPM.
void takeInput(){
  userInput = true; // Flag that the user is entering input.
  // If the user is currently pressing a key.
  if (!(keyPressed)){    
    keyPressed = true; // Mark that a key is currently pressed.
    // Switching on the key pressed by the user.
    switch (keyInput) {
      case '#':
        updateDesiredRpm(); // Read in their input if a '#' is entered.
        break;
      case 'A':
      case 'B':
      case 'C':
      case 'D':
      case '*':
        break;
      default:
        inputString += keyInput; // Record the character they entered.
        break;
    }
    updateDisplay(); // Update the display to print the user's input.
  }   
}

// Method for updating the desired RPM.
void updateDesiredRpm (){
  int newValue = inputString.toInt(); // Converting the user's input to an int.
  // If the user's RPM value is acceptable.
  if (newValue >= minRpm && newValue <= maxRpm){
    desiredRpm = newValue; // Update the desired RPM.
  }
  inputString = "";   // Clear the user's input string.
  userInput = false;  // Flag that the user is no longer entering input.
  updateDisplay();    // Update the display.
}
