// Griffin White
// 2-19-2023
// Spring 2023 Contract Course
// Engine Governor: Interrupt Hall Effect Sensor Test

const int hallSensorPin = 2;                  // Pin for hall effect sensor.

void setup() {
  // Setting the SWITCH pinMode to INPUT_PULLUP.
  pinMode(hallSensorPin,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(hallSensorPin),countPulse,FALLING);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:

}

void countPulse(){
  Serial.println("Pulse!");
  
}
