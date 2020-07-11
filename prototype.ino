#include <TinyGPS++.h>    //GPS library
#include <BlynkSimpleSerialBLE.h>        //Access to Blynk app
#include <Wire.h>
#include <ADXL345.h>

ADXL345 adxl;
double xyz[3];
double pax = 0, pay = 0, paz = 0;
double cax = 0, cay = 0, caz = 0;
int speakerpin = 57;
int ledpin = 5;

char auth[] = "HkzSj35MUt6E6yXz9LSGPc7I5f4HANrR";
TinyGPSPlus gps; // The TinyGPS++ object
float lat;
float lon;
unsigned long time_previous, time_current;
unsigned long time_previous2, time_current2;

void setup() {
  Serial.begin(9600);
  Serial2.begin(9600);
  Serial3.begin(9600); // hc-06

  pinMode(ledpin, OUTPUT);
  digitalWrite(ledpin, LOW);

  adxl.powerOn();

  //set activity/ inactivity thresholds (0-255)
  adxl.setActivityThreshold(75); //62.5mg per increment
  adxl.setInactivityThreshold(75); //62.5mg per increment
  adxl.setTimeInactivity(10); // how many seconds of no activity is inactive?

  //look of activity movement on this axes - 1 == on; 0 == off
  adxl.setActivityX(1);
  adxl.setActivityY(1);
  adxl.setActivityZ(1);

  //look of inactivity movement on this axes - 1 == on; 0 == off
  adxl.setInactivityX(1);
  adxl.setInactivityY(1);
  adxl.setInactivityZ(1);

  //look of tap movement on this axes - 1 == on; 0 == off
  adxl.setTapDetectionOnX(0);
  adxl.setTapDetectionOnY(0);
  adxl.setTapDetectionOnZ(1);

  //set values for what is a tap, and what is a double tap (0-255)
  adxl.setTapThreshold(50); //62.5mg per increment
  adxl.setTapDuration(15); //625us per increment
  adxl.setDoubleTapLatency(80); //1.25ms per increment
  adxl.setDoubleTapWindow(200); //1.25ms per increment

  //set values for what is considered freefall (0-255)
  adxl.setFreeFallThreshold(7); //(5 - 9) recommended - 62.5mg per increment
  adxl.setFreeFallDuration(45); //(20 - 70) recommended - 5ms per increment

  //setting all interrupts to take place on int pin 1
  //I had issues with int pin 2, was unable to reset it
  adxl.setInterruptMapping( ADXL345_INT_SINGLE_TAP_BIT,   ADXL345_INT1_PIN );
  adxl.setInterruptMapping( ADXL345_INT_DOUBLE_TAP_BIT,   ADXL345_INT1_PIN );
  adxl.setInterruptMapping( ADXL345_INT_FREE_FALL_BIT,    ADXL345_INT1_PIN );
  adxl.setInterruptMapping( ADXL345_INT_ACTIVITY_BIT,     ADXL345_INT1_PIN );
  adxl.setInterruptMapping( ADXL345_INT_INACTIVITY_BIT,   ADXL345_INT1_PIN );

  //register interrupt actions - 1 == on; 0 == off
  adxl.setInterrupt( ADXL345_INT_SINGLE_TAP_BIT, 1);
  adxl.setInterrupt( ADXL345_INT_DOUBLE_TAP_BIT, 1);
  adxl.setInterrupt( ADXL345_INT_FREE_FALL_BIT,  1);
  adxl.setInterrupt( ADXL345_INT_ACTIVITY_BIT,   1);
  adxl.setInterrupt( ADXL345_INT_INACTIVITY_BIT, 1);

  adxl.getAcceleration(xyz);
  pax = xyz[0];
  pay = xyz[1];
  paz = xyz[2];


  time_previous = millis();
  while (Serial3.available() == 0) {
    time_current = millis();
    if (time_current - time_previous >= 1000) {
      Serial.println("Not yet!");
      time_previous = time_current;
    }
  }
  Blynk.begin(Serial3, auth);

}

void loop() {
  Blynk.run();

  if (Blynk.connected()) {
    if (Serial2.available() > 0) { // gps
      if (gps.encode(Serial2.read())) { // gps
        displayInfo();
        Serial.println("gps available");
        delay(2000);
      }
    }
  }

  time_current = millis();
  if (time_current - time_previous >= 200   ) {
    time_previous = time_current;
    if (Blynk.connected() == 0) {

      speaker();

      int illuminance = readIlluminance();

      if (illuminance <= 4) {
        digitalWrite(ledpin, HIGH);
      }

      if (illuminance > 4) {
        digitalWrite(ledpin, LOW);
      }


      Serial.println("Disconnected");
    }
  }
}

int readIlluminance() {
  int reading = analogRead(56);

  int illuminance = map(reading, 0, 1023, 10, 0);
  return illuminance;

}

void speaker() {
  int x, y, z;
  adxl.readXYZ(&x, &y, &z); //read the accelerometer values and store them in variables  x,y,z
  // Output x,y,z values

  adxl.getAcceleration(xyz);
  cax = xyz[0];
  cay = xyz[1];
  caz = xyz[2];
  Serial.print("x : " + String(cax));
  Serial.print("y : " + String(cay));
  Serial.print("z : " + String(caz));

  if (abs(cax - pax) >= 0.1 or (abs(cay - pay) >= 0.1) or (abs(caz - paz) >= 0.1)) {
    tone(speakerpin, 1000);
  }
  if ((abs(cax - pax) < 0.1) and (abs(cay - pay) < 0.1) and (abs(caz - paz) < 0.1)) {
    noTone(speakerpin);
  }

  pax = cax;
  pay = cay;
  paz = caz;
}

void displayInfo() {
  if (gps.location.isValid()) {
    //Serial.println("connected");
    lat = gps.location.lat();    //get latitude and longitude
    lon = gps.location.lng();
    Blynk.virtualWrite(V0, 1, lat, lon, "WALLET_LOCATION_Bluetooth");    //write to Blynk map
    /*
      //create location string which can be used directly in Google maps
      slocation = String(lat) + "," + String(lon);
      spd = gps.speed.kmph();    //get speed
      Blynk.virtualWrite(V1, spd);
      bearing = TinyGPSPlus::cardinal(gps.course.value()); //direction
      Blynk.virtualWrite(V2, bearing);
      sats = gps.satellites.value();    //get number of satellites
      Blynk.virtualWrite(V3, sats);
      hdop = gps.hdop.value() / 10;    //get accuracy of location
      Blynk.virtualWrite(V4, hdop);
      msg5 = "GPS Good";
      Blynk.virtualWrite(V5, msg5);
    */
  }
  /*else
    {
      msg5 = "Invalid Data";
      Blynk.virtualWrite(V5, msg5);
    }*/
}
