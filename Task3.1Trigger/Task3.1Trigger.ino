//Libraries
#include <BH1750.h>
#include <Wire.h>
#include <WiFiNINA.h> // for arduino
#include "secrets.h"
//wifi
char ssid[] = SECRET_SSID;   // your network SSID (name) 
char pass[] = SECRET_PASS;   // your network password
WiFiClient client;
//trigger
char   HOST_NAME[] = "maker.ifttt.com";
String IFTTT_KEY = "eeXTQEjF3irbuMXSeE1rVANSNZdys5cOAY1nz9PBYgc"; // for modularity later
//global variables 
float SUNLIGHT = 250;
float NOSUNLIGHT = 20;
bool isThereSunLight = false;

BH1750 lightMeter;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
  }
  Wire.begin();
  lightMeter.begin();
}


void loop() {
  // Connect or reconnect to WiFi
  connectToWifi();
  //read data
  float lux = lightSensor();
  //is sunlight present or no?
  if(!isThereSunLight && lux >= SUNLIGHT){
    Serial.println("Sunlight is present.");
    sendToIFTTT("SunLight");
    isThereSunLight = true;
  }
  if(isThereSunLight && lux <= NOSUNLIGHT){
    Serial.println("Sunlight is no longer present.");
    sendToIFTTT("NoSunLight");
    isThereSunLight = false;
  }
  delay(1000);
}

void sendToIFTTT(String eventName){
  if (client.connect(HOST_NAME, 80)) {
    //if connected
    String path = "/trigger/" + eventName + "/with/key/" + IFTTT_KEY;
    client.println("GET " + path + " HTTP/1.1");
    client.println("Host: " + String(HOST_NAME));
    client.println("Connection: close");
    client.println(); // end HTTP header
    
    Serial.print("Status updated to: ");
    Serial.println(eventName);
  }
  else {
    Serial.println("connection failed");
  }
}

float lightSensor(){
  float lux = lightMeter.readLightLevel();
  Serial.print("Light: ");
  Serial.print(lux);
  Serial.println(" lx");
  return lux;
}

void connectToWifi(){
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected.");
  }
}