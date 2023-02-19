// Defining constansts for the internal LED pin and the switch pin.
#define LED 13
#define SWITCH 12

void setup() {
  // Setting the LED pinMode to output.
  pinMode(LED, OUTPUT);
  // Setting the SWITCH pinMode to INPUT_PULLUP.
  pinMode(SWITCH,INPUT_PULLUP);
}

void loop() {
  // If the switch is open.
  if (digitalRead(SWITCH) == HIGH)
    // Turn the LED off.
    digitalWrite (LED, LOW);
  // Else, the switch is closed.
  else
    // Turning the LED on.
    digitalWrite (LED, HIGH);
}
