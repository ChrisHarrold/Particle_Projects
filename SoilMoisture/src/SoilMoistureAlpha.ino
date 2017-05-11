
// ------------
// Initial Soil Moisture Read Code - Single Sensor with LED indicators
// ------------

/*-------------

This simple flash will do two things:

Turn on an LED to indicate that the Device is in the active monitoring state

Take a reading from the YL-69 Soil Moisture probe analog pin

-------------*/

int aled = D0; // this is the activity LED (aled)
int ppin = D1; // This is the pin that will turn on the YL-69 Sensor
int arpin = A0; // This is the pin we will take the analog value reading from

void setup() {

  // Set the LED and power pins to output mode
  // It's important you do this here, inside the setup() function rather than
  // outside it or in the loop function.

  pinMode(aled, OUTPUT);
  pinMode(ppin, OUTPUT);
  // read in a code snipit note that you DO NOT DO THIS: pinMode(arpin, INPUT);

}

// Next we have the loop function, the other essential part of a microcontroller program.

void loop() {
  // here we will start the process by turning on the indicator LED (aLED)
  digitalWrite(aled, HIGH);

  // Next we turn on the power to the sensor
  digitalWrite(ppin, HIGH);

  // attempting to read immediately causes funky voltages so we will wait for 3
  // seconds before we poll the analog pin
  delay(3000);

  // Now we will pull the votage value from the probe
  int SMVolts = analogRead(arpin);

  // and use the particle.publish event trigger to see what the value is via the
  // particle build console - you can also get this from the terminal with a long
  // command that the console shows you which is cool
  Particle.publish("SoilVoltsAlpha", String(SMVolts));

  // Then we'll turn it off...
  digitalWrite(ppin, LOW);
  digitalWrite(aled, LOW);

  // Wait 10 seconds for the sake of debugging...
  delay(60000);

  // And repeat!
}
