const int sensorPin = A15;

void setup() {
  Serial.begin(9600);

}

void loop() {
  int ammeterVal;
  ammeterVal = (analogRead(sensorPin) + analogRead(sensorPin) + analogRead(sensorPin)) / 3;
  Serial.print("Ammeter: ");
  Serial.println (ammeterVal);
  delay(300);
}
