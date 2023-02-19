const int lcdContrast = 30;                   // Contrast value (0-255) for the LCD display.
const int lcdChar = 16, lcdRow = 2;           // LCD display dimensions.
const int in1 = 3, in2 = 4, in3 = 5, in4 = 6; // Pins for the stepper motor driver.
const int vo = 9, rs = 13, en = 12, d4 = 11, d5 = 10, d6 = 8, d7 = 7; // Pins for the LCD.

double rpm = 123.45;               // Current RPM.
double desiredRpm = 500;      // Desired RPM.

#include "Timer.h"          // Timer library.
#include "LiquidCrystal.h"  // LCD display library.

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);      // LiquidCrystal object.

Timer millisTimer(MILLIS);
Timer microsTimer(MICROS);


void setup() {
  double test = 0;
  
  analogWrite(vo, lcdContrast);         //
  lcd.begin(lcdChar, lcdRow);           //
  Serial.begin(9600);

//  millisTimer.start();
//  millisTimer.pause();
//  microsTimer.start();
//  microsTimer.pause();
//
//  Serial.println("Timer started, then immediately paused.");
//  Serial.print (" Milliseconds: ");
//  Serial.println (millisTimer.read());
//  Serial.print (" Microseconds: ");
//  Serial.println (microsTimer.read());
//  Serial.println("");
//
//  millisTimer.start();
//  test = 9989998.99 / 123.456;
//  millisTimer.pause();
//  microsTimer.start();
//  test = pow(1.0000091,99999);
//  microsTimer.pause();
//
//  Serial.println("Performing floating-point division: 999999 / 123.456");
//  Serial.print (" Milliseconds: ");
//  Serial.println (millisTimer.read());
//  Serial.print (" Microseconds: ");
//  Serial.println (microsTimer.read());
//  Serial.println("");
//
//  millisTimer.start();
//  lcd.print("A");
//  millisTimer.pause();
//  microsTimer.start();
//  lcd.print("A");
//  microsTimer.pause();
//
//  Serial.println("Writing one character to LCD display.");
//  Serial.print (" Milliseconds: ");
//  Serial.println (millisTimer.read());
//  Serial.print (" Microseconds: ");
//  Serial.println (microsTimer.read());
//  Serial.println("");
//
//  millisTimer.start();
//  lcd.print("AB");
//  millisTimer.pause();
//  microsTimer.start();
//  lcd.print("AB");
//  microsTimer.pause();
//
//  Serial.println("Writing two characters to LCD display.");
//  Serial.print (" Milliseconds: ");
//  Serial.println (millisTimer.read());
//  Serial.print (" Microseconds: ");
//  Serial.println (microsTimer.read());
//  Serial.println("");

  millisTimer.start();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Set RPM: ");
    lcd.print(desiredRpm);
    lcd.setCursor(0, 1);
    lcd.print("RPM: ");
    lcd.print(rpm);   
  millisTimer.pause();
  microsTimer.start();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Set RPM: ");
    lcd.print(desiredRpm);
    lcd.setCursor(0, 1);
    lcd.print("RPM: ");
    lcd.print(rpm);   
  microsTimer.pause();

  Serial.println("Printing Entire RPM output to LCD display.");
  Serial.print (" Milliseconds: ");
  Serial.println (millisTimer.read());
  Serial.print (" Microseconds: ");
  Serial.println (microsTimer.read());
  Serial.println("");

  lcd.clear();
  lcd.print("Set RPM: ");
  lcd.print(desiredRpm);
  lcd.setCursor(0, 1);
  lcd.print("RPM: ");

  millisTimer.start();
    lcd.setCursor(5, 1);
    lcd.print(rpm);   
  millisTimer.pause();
  microsTimer.start();
    lcd.setCursor(5, 1);
    lcd.print(rpm);  
  microsTimer.pause();

  Serial.println("Prinintg RPM value to LCD display, separate header.");
  Serial.print (" Milliseconds: ");
  Serial.println (millisTimer.read());
  Serial.print (" Microseconds: ");
  Serial.println (microsTimer.read());
  Serial.println("");

}

void loop() {
  // put your main code here, to run repeatedly:

}
