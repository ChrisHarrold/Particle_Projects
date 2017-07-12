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


// placeholder for the analog probe reading pins and digital power pins
int as[6];
int ps[6];

// what is this thing called? Set in the "setup" section below
retained char dev_name[12];
// these are the various variables used later if the photon is in offline mode
retained int loops;
retained char offlinevals[1024];
retained int16_t globalindex = 0;
// is the particle online value check
int onlinechk;
// this is a simple int variable for loops and counters
int a = 0;

void setup() {

  // I have 6 analog pins to read for the moisture probes - this tells
  // the program which pins to use and must match the PCB (prototype map)
  as[0] = A1;
  as[1] = A2;
  as[2] = A3;
  as[3] = A4;
  as[4] = A5;
  as[5] = A6;

  // mapping for the digital pins is not quite as clean as the analog's, but
  // once done here I can use the same for loops to process turning them on
  // and off like I do with the analog pins for reading
  ps[0] = D3;
  ps[1] = D2;
  ps[2] = D1;
  ps[3] = D0;
  ps[4] = D6;
  ps[5] = D7;


  // Set the LED and power pins to output mode

  pinMode(pled, OUTPUT);
  pinMode(aled, OUTPUT);
  for(a = 0; a < 6; a = a + 1 ) {
    pinMode(ps[a], OUTPUT);
  }

  // Change "Tomato_1" to whatever you want the device to be - this will happen
  // during setup at some point, but it gets manual treatment for now
  strcpy(dev_name, "Tomato_1");

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
    publishdata = timestamp + "::";

    // For loop takes a reading from each analog pin and creates the output String
    // to send to the particle.publish function - this will eventually change to
    // being sent to the base-station, but for now it's good to see results
    for(a = 0; a < 6; a = a + 1 ) {
      int SMVolts = analogRead(as[a]);
        if (SMVolts < 4000) {
          String voltage = String(SMVolts);
          String publishdata = publishdata + "S" + a + "::" + voltage + "::";

        }
        else {
          String publishdata = publishdata + "S" + a + "::DC::";
        }
    }
    // Publish the readings to the cloud
    Particle.publish(dev_name, publishdata, PRIVATE);
}

void offlinepoll() {

  // Since the photon is offline we have to save the values to upload when
  // we get a connection - same starting cleanup and prep though
  String publishdata = "";
  time_t time = Time.now();
  String timestamp =  String(Time.timeStr());
  publishdata = timestamp + "::";

  // For loop takes a reading from each analog pin and creates the output String
  // to send to the char array for storing until connections resume
  for(a = 0; a < 6; a = a + 1 ) {
    int SMVolts = analogRead(as[a]);
      if (SMVolts < 4000) {
        String voltage = String(SMVolts);
        String publishdata = publishdata + "S" + a + "::" + voltage + "::";
      }
      else {
        String publishdata = publishdata + "S" + a + "::DC::";
      }
  }
  // store the reading in the array
  snprintf(offlinevals, sizeof(offlinevals), "%s", publishdata);
}

void update_now() {

  snprintf(offlinevals, sizeof(offlinevals), "%s", "data to sprintf");
  Particle.publish("SMP_Update", offlinevals, PRIVATE);
  delay(1000);
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
      update_now();
      onlinechk = 1;
      loops = 0;
    }
    else {
      digitalWrite(pled, HIGH);
      onlinechk = 0;
    }
  }

  // Next we turn on the power to the sensor
  for(a = 0; a < 6; a = a + 1 ) {
    digitalWrite(ps[a], HIGH);
  }
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
  for(a = 0; a < 6; a = a + 1 ) {
    digitalWrite(ps[a], LOW);
  }

  // Turn off the activity LED and then
  // put the system to sleep for (poll_delay) seconds
  // defaulted to 10 for the sake of debugging speed...
  loops = loops + 1;
  digitalWrite(aled, LOW);
  // set this value to the # of seconds between polling intervals
  // easy ref table:
  //
  // 2 hours = 7200
  // 6 hours = 21600
  // 12 hours = 43200
  // 24 = 86400
  //
  // I would recommend values basd on climate - longer for wet and shorter for
  // hot and dry - this puts the photon to sleep so it uses less juice
  System.sleep(10);

  // And repeat!
}
