// Griffin White
// 2-26-2023
// Spring 2023 Contract Course
// Engine Governor: LCD Write Time Testing

#include "Wire.h"
#include "LiquidCrystal_I2C.h"
#include "LiquidCrystal.h"  // LCD display library.
#include "Timer.h"

Timer serialTimer(MILLIS); 
Timer basicLcdTimer(MILLIS);
Timer i2cLcdTimer(MILLIS);

LiquidCrystal_I2C i2cLcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal basicLcd(4, 5, 6, 7, 8, 9);   


void setup() {
  Wire.setClock(800000);
  
  i2cLcd.init();                      // initialize the lcd 
  i2cLcd.backlight();
  
  Serial.begin(9600);

  basicLcd.begin(16, 2); 

  String testString = "abcdedfghijk";

  serialTimer.start();
  Serial.print(testString);
  serialTimer.pause();

  
  basicLcdTimer.start();
  basicLcd.print(testString);
  basicLcdTimer.pause();

  i2cLcdTimer.start();
  i2cLcd.print(testString);
  i2cLcdTimer.pause();

  Serial.print ("\n\nSerial: ");
  Serial.print (serialTimer.read(), 5);
  Serial.println (" ms");

  Serial.print ("Basic LCD: ");
  Serial.print (basicLcdTimer.read(), 5);
  Serial.println (" ms");

  Serial.print ("I2C LCD: ");
  Serial.print (i2cLcdTimer.read(), 5);
  Serial.println (" ms");

  i2cLcd.clear();
  i2cLcd.setCursor(0,0);
  i2cLcd.print(" Set  | Curr RPM");
  i2cLcd.setCursor(0,1);
  i2cLcd.print(" 1800 |  1801.4");

}

void loop() {
//  lcd.clear();
//  lcd.print ("Test");
//  delay(500);
//  lcd.clear();
//  delay(500);
//  lcd.setCursor(0,1);
//  lcd.print("Hello?");
//  delay(1000);




}
