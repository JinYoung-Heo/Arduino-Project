/*
 WiFiEsp example: ConnectWPA
 
 This example connects to an encrypted WiFi network using an ESP8266 module.
 Then it prints the  MAC address of the WiFi shield, the IP address obtained
 and other network details.

 For more details see: http://yaab-arduino.blogspot.com/p/wifiesp-example-connect.html
*/

#include "WiFiEsp.h"
#include <BlynkSimpleShieldEsp8266.h>
#include <TinyGPS++.h>
ESP8266 wifi(&Serial1); 

// Emulate Serial1 on pins 6/7 if not present
//#ifndef HAVE_HWSERIAL1
//#include "SoftwareSerial.h"
//SoftwareSerial Serial1(6, 7); // RX, TX
//#endif

char auth[] = "Z39RmpvKIL5SM03V2z9ttZ23OSQ0Ddcd";
TinyGPSPlus gps; // The TinyGPS++ object
float lat;
float lon;

String s;
char ssid[32] = {0};            // your network SSID (name)
char pass[] = "";        // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status

void setup()
{
  // initialize serial for debugging
  Serial.begin(115200);
  // initialize serial for ESP module
  Serial1.begin(115200); // esp-01
  Serial2.begin(9600); // gps
  // initialize ESP module
  WiFi.init(&Serial1);

  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }

  // attempt to connect to WiFi network
  long rssi = WiFi.RSSI();
  Serial.println();
  Serial.println(rssi);
  while (status != WL_CONNECTED) {
      Serial.print("Attempting to connect to WPA SSID: ");
      int numSsid = WiFi.scanNetworks();
      if (numSsid == -1) {
        Serial.println("Couldn't get a wifi connection");
        while (true);
      }
      for (int thisNet = 1; thisNet < 3; thisNet++) {
        Serial.println();
        Serial.println(String(WiFi.SSID())+" trying");
        //Serial.println(String(WiFi.SSID(thisNet))+" trying");
        //status = WiFi.begin(WiFi.SSID(thisNet), NULL);
        status = WiFi.begin(WiFi.SSID(), NULL);
        //delay(10000);
        rssi = WiFi.RSSI();
        if(rssi != 0) break;
        // Connect to WPA/WPA2 network
      }
    }
  
  Serial.println(WiFi.status());
      

  Serial.println("You're connected to the network"+String(WiFi.SSID()));
  printCurrentNet();
  printWifiData();

  Blynk.begin(auth, wifi, WiFi.SSID(), NULL);
  Serial.println("loop start");
}

void loop()
{
  while (Serial2.available() > 0) 
    {
      //Serial.println("gps is available");
      // sketch displays information every time a new sentence is correctly encoded.
      if (gps.encode(Serial2.read()))
        displayInfo();
  }
  Blynk.run(); 
}
void displayInfo() {
  Serial.println("displayInfo!");
   if (gps.location.isValid()) {
       Serial.println("connected");
       lat = gps.location.lat();    //get latitude and longitude
       lon = gps.location.lng();
       Blynk.virtualWrite(V0, 1, lat, lon, "WALLET_LOCATION_WiFi");    //write to Blynk map
   }
}
void printWifiData()
{
  // print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print your MAC address
  byte mac[6];
  WiFi.macAddress(mac);
  char buf[20];
  sprintf(buf, "%02X:%02X:%02X:%02X:%02X:%02X", mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]);
  Serial.print("MAC address: ");
  Serial.println(buf);
}

void printCurrentNet()
{
  // print the SSID of the network you're attached to
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print the MAC address of the router you're attached to
  byte bssid[6];
  WiFi.BSSID(bssid);
  char buf[20];
  sprintf(buf, "%02X:%02X:%02X:%02X:%02X:%02X", bssid[5], bssid[4], bssid[3], bssid[2], bssid[1], bssid[0]);
  Serial.print("BSSID: ");
  Serial.println(buf);

  // print the received signal strength
  long rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI): ");
  Serial.println(rssi);
}
