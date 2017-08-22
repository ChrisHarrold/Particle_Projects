#include <time.h>
// ------------
// Production Soil Moisture Read Code
// Supports protoype boards with up to 6 TL-69 sensors
// ------------

/*-------------
This flash is the "production" code for the EZ-Garden System SM Kit
-------------*/

STARTUP(System.enableFeature(FEATURE_RETAINED_MEMORY));

// this is the activity LED (aled) - it comes on when a sensor is sampling
int aled = D4;
// This LED will come on when the photon is offline from cloud
int pled = D5;

// character string definitions for storing results
char timestamp[25];
char payload[12];
char onlinevals[192];
// placeholder for the analog probe reading pins and digital power pins
int as[6];
int ps[6];


// the below variable is "retained" meaning that if the photon sleeps or
// looses main power (not backup on the vbat pin) that they are kept in memory
// -----------
// what is this thing called? Set the name in the "setup" section below
retained char dev_name[12];
// Loop counter used if the particle is offline
int loops;
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

  // Change "Tomato_1::" to "whatever::" you want the device to be - this will
  // happen during setup at some point, but it gets manual treatment for now
  // also include the "::" as it will be parsed by the backend code for dsiplay
  strcpy(dev_name, "Tomato_1::");

}

void onlinepoll() {
  // clear out the payload and onlinevals strings since we are live and can
  // upload them real time we don't need to persist
  strcpy(onlinevals, dev_name);
  // Since the photon is online and connected we can live publish values!
  // get the time of the readings to be taken first so the timestamp can
  // be used in later steps and store it in the payload
  time_t time = Time.now();
  strcpy(timestamp, String(Time.timeStr()) + "::");
  strcat(onlinevals, timestamp);

  // Next we turn on the power to the sensor
    for(a = 0; a < 6; a = a + 1 ) {
      digitalWrite(ps[a], HIGH);
    }
  // attempting to read immediately causes funky voltages so we will wait for 2
  // seconds before we poll the analog pin
    delay(2000);


  // For loop takes a reading from each analog pin and creates the output String
  // to send to the particle.publish function - this will eventually change to
  // being sent to the base-station, but for now it's good to see results
    a = 0;
    for(a = 0; a < 6; a = a + 1 ) {
      int SMVolts = analogRead(as[a]);
        if (SMVolts < 4000) {
          snprintf(payload, sizeof(payload), "S%d::%d::", a, SMVolts);
          strcat(onlinevals, payload);
        }
        else {
          snprintf(payload, sizeof(payload), "S%d::DC::", a);
          strcat(onlinevals, payload);
        }
    }
    // Publish the readings to the cloud
    Particle.publish(dev_name, onlinevals, PRIVATE);
    // Then we'll turn off the sensor pack
    for(a = 0; a < 6; a = a + 1 ) {
      digitalWrite(ps[a], LOW);
    }
}

// Next we have the loop function, the main part of the program.
void loop() {

  // here we will start the process by turning on the indicator LED (aLED)
  digitalWrite(aled, HIGH);
  // check to see if the photon is online
  if (Particle.connected()) {
    //If so, upload the data stored in the offlinevals array for the dashboard

    loops = 0;
  }
  // If not - we will loop until it is
  else {
    digitalWrite(pled, HIGH);
    do {
      loops = loops + 1;
      delay(10000);
    } while (!Particle.connected())
    // then we will turn off the alert light, and publish both an "offline" message
    // and run a regular sensor update
    digitalWrite(pled, LOW);
    Particle.publish(dev_name, "Device was offline for " + loops + " loops - check wifi.", PRIVATE);
    onlinepoll();
  }

  // Turn off the activity LED and then sleep for the indicated length of time
  // see the chart below for recommended sleep values
  digitalWrite(aled, LOW);
  // Choose the application below that makes the most sense for you.
  // system.sleep(xxx) will turn off the bulk of the Particle functions (in particular
  // the wifi module) thus saving battery life or power in general. if you are
  // running plugged in, a simple delay(xxx) is enough, but YMMV of course
  // System.sleep(10000);
  delay(10000);
  // set the value in () to the # of seconds between polling intervals
  // easy ref table:
  //
  // 2 hours = 7200 <- wouldn't go below this except for VERY dry climates
  // or super-specific needs for watering intervals
  // 6 hours = 21600
  // 12 hours = 43200
  // 24 = 86400
  //
  // I would recommend values basd on climate - longer for wet and shorter for
  // hot and dry
}
