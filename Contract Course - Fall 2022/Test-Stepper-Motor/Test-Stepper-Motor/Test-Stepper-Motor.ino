// Griffin White
// 10-10-2022
// Fall 2022 Contract Course
// Stepper Motor Test

#include <CheapStepper.h>

CheapStepper stepper;

void setup() {
//  Serial.begin(9600);
//     for (int i=0; i<400; i++){
//    stepper.step(true);
//  }
}

void loop() {
   for (int i=0; i<1600; i++){
    stepper.step(false);
  }

  delay(1000);
  
   for (int i=0; i<1600; i++){
    stepper.step(true);
  }

  delay(1000);
  
}
