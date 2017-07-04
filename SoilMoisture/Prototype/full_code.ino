
// ------------
// Production Soil Moisture Read Code
// Supports protoype boards with 6 TL-69 sensors
// ------------

/*-------------

This flash is the "production" code for the EZ-Garden System SM Kit

The flow is simple:
0) Setup the code and turn on the "Heartbeat LED"

1) Poll the device and check for active probes
(not-connected probes throw a 0 reading)

2) Check for connectivity to the particle cloud
(code uses particle.publish to upload data so if it's not connected then it will
store the data in an array until the next connectivity cycle)

3) Take a reading from all connected probes

4) Either upload or store the results

5) sleep for some amount of time and then redo 1 through 5!
-------------*/

int aled = D4; // this is the activity LED (aled) - it comes on when a sensor is sampling
int pled = D5; // This LED will come on when the power is on and the code loads

// Since this is my prototypw the pin to sensor mapping kinda sucks because
// I have ever designed a PCB before, but this is what happens when you marry
// hardware and software

// These are the pins for S0
int ps0 = D3;
int as0 = A0;

// These are the pins for S1
int ps1 = D2;
int as1 = A1;

// These are the pins for S2
int ps2 = D1;
int as2 = A2;

// These are the pins for S3
int ps3 = D0;
int as3 = A3;

// These are the pins for S4
int ps4 = D6;
int as4 = A4;

// These are the pins for S5
int ps5 = D7;
int as5 = A5;

void setup() {

  // Set the LED and power pins to output mode
  // It's important you do this here, inside the setup() function rather than
  // outside it or in the loop function.

  pinMode(aled, OUTPUT);
  pinMode(ppin0, OUTPUT);
  pinMode(pled, OUTPUT);
  digitalWrite(pled, HIGH);

  // read in a code snipit note that you DO NOT DO THIS: pinMode(arpin, INPUT);

}

// Next we have the loop function, the other essential part of a microcontroller program.


void loop() {
  // here we will start the process by turning on the indicator LED (aLED)
  digitalWrite(aled, HIGH);


  // Next we turn on the power to the sensor
  digitalWrite(ppin0, HIGH);

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
  digitalWrite(ppin0, LOW);
  digitalWrite(aled, LOW);

  // Wait 10 seconds for the sake of debugging...
  delay(60000);

  // And repeat!
}
