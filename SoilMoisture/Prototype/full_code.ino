#include <time.h>
// ------------
// Production Soil Moisture Read Code
// Supports protoype boards with 6 TL-69 sensors
// ------------

/*-------------
This flash is the "production" code for the EZ-Garden System SM Kit
-------------*/

STARTUP(System.enableFeature(FEATURE_RETAINED_MEMORY));

int aled = D4; // this is the activity LED (aled) - it comes on when a sensor is sampling
int pled = D5; // This LED will come on when the photon is offline from cloud

// Since this is my prototype the pin to sensor mapping kinda sucks because
// I have never designed a PCB before, but this is what happens when you marry
// hardware and software

// power for S0
int ps0 = D3;

// power for S1
int ps1 = D2;

// power S2
int ps2 = D1;

// power S3
int ps3 = D0;

// power S4
int ps4 = D6;

// power S5
int ps5 = D7;

// placeholder for the analog probe reading pins
int as[6];

// what is this thing called? Set in the "setup" section below
retained char dev_name[12];
// these are the various variables used later if the photon is in offline mode
retained int loops;
retained char offlinevals[1024];
retained int16_t globalindex = 0;
// is the particle online value check
int onlinechk;
// set this value to the # of miliseconds between polling intervals
// easy ref table:
//
// 2 hours = 7200000
// 6 hours = 21600000
// 12 hours = 43200000
// 24 = 86400000
// I would recommend values basd on climate - longer for wet and shorter for
// hot and dry
int poll_delay = 10000;
// this is a simple int variable for loops and counters
int a = 0;

void setup() {

  // I have 6 analog pins to read for the moisture probes - this tells
  // the program which pins to use and must match the PCB (this is the map for
  // the prototype boards)
  as[0] = A1;
  as[1] = A2;
  as[2] = A3;
  as[3] = A4;
  as[4] = A5;
  as[5] = A6;


  // Set the LED and power pins to output mode
  // It's important you do this here, inside the setup() function rather than
  // outside it or in the loop function.

  pinMode(pled, OUTPUT);
  pinMode(aled, OUTPUT);
  pinMode(ps0, OUTPUT);
  pinMode(ps1, OUTPUT);
  pinMode(ps2, OUTPUT);
  pinMode(ps3, OUTPUT);
  pinMode(ps4, OUTPUT);
  pinMode(ps5, OUTPUT);

  snprintf(dev_name, sizeof(dev_name), "%s", "Tomato_1");

}

void onlinepoll() {
    // Since the photon is online and connected we can live publish values!
    // let's blank everything out quick like and cleanup first and then we
    // can process the polling requests
    String publishdata = "";
    //get the time of the readings to be taken first so the timestamp can
    //be used in later steps
    time_t time = Time.now();
    String timestamp =  String(Time.timeStr());


    for(a = 0; a < 6; a = a + 1 ) {
      int SMVolts = analogRead(as[a]);
        if (SMVolts < 4000) {
          String voltage = String(SMVolts);
          String publishdata = publishdata + "S" + a + "::" + timestamp + "::" + voltage + "::";

        }
        else {
          String publishdata = publishdata + "S" + a + "::No Sensor discovered!::";
        }
    }

    Particle.publish(dev_name, publishdata, PRIVATE);
}

void offlinepoll() {


}

// Next we have the loop function, the main part of the program.
void loop() {

  // here we will start the process by turning on the indicator LED (aLED)
  digitalWrite(aled, HIGH);

  // check and see if we have polling data stored
  if (loops > 0); {
    // if so, then check to see if the photon is online
    if (Particle.connected()) {
      //If so, upload the data stored in the offlinevals array for the dashboard
      snprintf(offlinevals, sizeof(offlinevals), "%s", "data to sprintf");
      Particle.publish("SMP_Update", offlinevals, PRIVATE);
      delay(1000);
      onlinechk = 1;
      loops = 0;
    }
    else {
      digitalWrite(pled, HIGH);
      onlinechk = 0;
    }
  }

  // Next we turn on the power to the sensor
  digitalWrite(ps0, HIGH);
  digitalWrite(ps1, HIGH);
  digitalWrite(ps2, HIGH);
  digitalWrite(ps3, HIGH);
  digitalWrite(ps4, HIGH);
  digitalWrite(ps5, HIGH);
  // attempting to read immediately causes funky voltages so we will wait for 2
  // seconds before we poll the analog pin
  delay(2000);

//Based on the online check we now poll in either online of offline mode
  if (onlinechk = 1) {
    onlinepoll();
  }
  else {
    offlinepoll();
  }

  // Then we'll turn off the sensor pack
  digitalWrite(ps0, LOW);
  digitalWrite(ps1, LOW);
  digitalWrite(ps2, LOW);
  digitalWrite(ps3, LOW);
  digitalWrite(ps4, LOW);
  digitalWrite(ps5, LOW);

  // Turn off the activity LED and then
  // wait delay seconds for the sake of debugging...
  loops = loops + 1;
  digitalWrite(aled, LOW);
  delay(poll_delay);

  // And repeat!
}
