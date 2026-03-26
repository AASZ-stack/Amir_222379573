//Libraries
#include "DHT.h"
#include <WiFiNINA.h> // for arduino
#include "secrets.h"
#include "ThingSpeak.h"
//thingspeak
char ssid[] = SECRET_SSID;   // your network SSID (name) 
char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;
unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;
// temp sensor
#define DHTPIN 2
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
// soil moisture sensor calibration values
const int dry = 850;
const int wet = 350;
// initializing values
float  dht22Reading = 0;
int soilMoistureReading = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
  }
  ThingSpeak.begin(client);
  dht.begin();
}

void loop() {
  // Connect or reconnect to WiFi
  connectToWifi();
  // read sensor
  dht22Reading = tempSensor();
  soilMoistureReading = soilMoistureSensor();
  // if dht retuns nan
  if(isnan(dht22Reading)) {
    Serial.println("temperature read failed.");
    delay(2000);
    return;
  }
  // upload
  uploadDataToThingSpeak(dht22Reading, soilMoistureReading);
  delay(30000);
}

void uploadDataToThingSpeak(float temp, int moisture) {
  //set fields
  ThingSpeak.setField(1, temp);
  ThingSpeak.setField(2, moisture);
  // write to thingspeak channel
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }

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

int soilMoistureSensor(){
  int rawSensorData = analogRead(A0);
  int moisture = map(rawSensorData, dry, wet, 0, 100);
  moisture = constrain(moisture, 0, 100);
  Serial.print("Raw: ");
  Serial.print(rawSensorData);
  Serial.print(" Moisture: ");
  Serial.print(moisture);
  Serial.println("%");
  return moisture;
}

float tempSensor(){
  //variables for sensor readings
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  //check for failed reads
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return NAN;
  }
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.println(F("°C"));
  return t;
}

