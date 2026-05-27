//Libraries
#include <WiFiNINA.h> 
#include "DHT.h"
#include <BH1750.h>
#include <Wire.h>
#include "secrets.h"
#include "ThingSpeak.h"

//Sensors/LED
#define DHTPIN 2 //DHT22
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
#define SOIL_PIN A0 //Soil moisture
const int wet = 350;
const int dry = 850;
BH1750 lightMeter; //BH1750
#define RED_LED_PIN 7 //Red LED

//Wifi
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
WiFiClient client;
//Thingspeak
unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;
//IFTTT
char   HOST_NAME[] = "maker.ifttt.com";
String IFTTT_KEY = SECRET_IFTTT_KEY;
//Initializing values
float temperature = 0;
float humidity = 0;
float lightLux = 0;
int soilMoisture = 0;
//Reading Thresholds
const float TEMP_LOW = 10.0;
const float TEMP_HIGH = 32.0;
const int SOIL_LOW = 20;
const int SOIL_HIGH = 80;
const float NO_LIGHT = 20.0;
const float SUN_LIGHT = 250.0;

//Timers
unsigned long previousSensorRead = 0;
unsigned long previousThingSpeakWrite = 0;
unsigned long previousWifiAttempt = 0;
const unsigned long sensorReadInterval = 5000;
const unsigned long thingSpeakWriteInterval = 20000;
const unsigned long wifiAttemptInterval = 5000;

//IFTTT (alert bool to avoid spam)
bool tempLowAlert = false;
bool tempHighAlert = false;
bool soilLowAlert = false;
bool soilHighAlert = false;
bool lightAlert = false;

//Setup
void setup() {
  Serial.begin(115200);
  while (!Serial) {
  }
  pinMode(RED_LED_PIN, OUTPUT);
  alertLedOff();
  connectToWifi();
  ThingSpeak.begin(client);
  dht.begin();
  Wire.begin();
  lightMeter.begin();
  Serial.println("Smart Plant Monitoring System: ");
}
//Loop
void loop() {
  connectToWifi(); //Connect/reconnect
  unsigned long currentRunTimeMillis = millis();
  if (currentRunTimeMillis - previousSensorRead >= sensorReadInterval) {
    previousSensorRead = currentRunTimeMillis;
    readSensors();
    checkEnvironmentConditions();
  }
  if (currentRunTimeMillis - previousThingSpeakWrite >= thingSpeakWriteInterval) {
    previousThingSpeakWrite = currentRunTimeMillis;
    uploadDataToThingSpeak();
  }
}

//Functions
void alertLedOff() {
  digitalWrite(RED_LED_PIN, LOW);
}
void alertLedOn() {
  digitalWrite(RED_LED_PIN, HIGH);
}
void connectToWifi(){
  if(WiFi.status() != WL_CONNECTED){
    unsigned long currentMillis = millis();
    if (currentMillis - previousWifiAttempt >= wifiAttemptInterval) {
      previousWifiAttempt = currentMillis;
      Serial.println("Attempting WiFi connection...");
      WiFi.begin(ssid, pass);
    } 
  }
}

void readSensors() {
  //DHT22
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor");
    return;
  }
  //Soil moisture
  int rawSensorData = analogRead(SOIL_PIN);
  soilMoisture = map(rawSensorData, dry, wet, 0, 100);
  soilMoisture = constrain(soilMoisture, 0, 100);
  //BH1750
  lightLux = lightMeter.readLightLevel();
  //print 
  Serial.println();
  Serial.println("Sensor Readings");
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");
  Serial.print("Soil moisture: ");
  Serial.print(soilMoisture);
  Serial.println(" %");
  Serial.print("Light: ");
  Serial.print(lightLux);
  Serial.println(" lx");
  Serial.println();
}

void checkEnvironmentConditions() {
  bool redAlert = false; 
  //DHT22
  if (temperature < TEMP_LOW) {
    redAlert = true;
    if (!tempLowAlert) {
      sendToIFTTT("temp_low");
      tempLowAlert = true;
    }
  }
  else {
    tempLowAlert = false;
  }
  if (temperature > TEMP_HIGH) {
    redAlert = true;
    if (!tempHighAlert) {
      sendToIFTTT("temp_high");
      tempHighAlert = true;
    }
  }
  else {
    tempHighAlert = false;
  }
  //Soil moisture
  if (soilMoisture < SOIL_LOW) {
    redAlert = true;
    if (!soilLowAlert) {
      sendToIFTTT("soil_low");
      soilLowAlert = true;
    }
  }
  else if (soilMoisture > 25) {
    soilLowAlert = false;
  }
  if (soilMoisture > SOIL_HIGH) {
    redAlert = true;
    if (!soilHighAlert) {
      sendToIFTTT("soil_high");
      soilHighAlert = true;
    }
  }
  else if (soilMoisture < 75) {
    soilHighAlert = false;
  }
  //BH1750
  if (lightLux < NO_LIGHT && !lightAlert) {
    lightAlert = true;
    sendToIFTTT("no_light");
    Serial.println("Sun light is no longer present.");
  }
  if (lightLux > SUN_LIGHT && lightAlert) {
    lightAlert = false;
    sendToIFTTT("sun_light");
    Serial.println("Sun light is present.");
  }
  //LED alert
  if (redAlert) {
    alertLedOn();
    Serial.println("ALERT: Condition out of range!");
  }
  else {
    alertLedOff();
    Serial.println("Status: All conditions within range.");
  }
}

void sendToIFTTT(String eventName){
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected.");
    return;
  }
  if (client.connect(HOST_NAME, 80)) {
    //if connected
    String path = "/trigger/" + eventName + "/with/key/" + IFTTT_KEY;
    client.println("GET " + path + " HTTP/1.1");
    client.println("Host: " + String(HOST_NAME));
    client.println("Connection: close");
    client.println(); // end HTTP header
    
    Serial.print("Status updated to: ");
    Serial.println(eventName);
    while (client.connected()) {
      if (client.available()) {
        // read an incoming byte from the server
        char c = client.read();
      }
    }
    client.stop();
  }
  else {
    Serial.println("connection failed");
  }
}

void uploadDataToThingSpeak() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected.");
    return;
  }
  //set fields
  ThingSpeak.setField(1, temperature);
  ThingSpeak.setField(2, humidity);
  ThingSpeak.setField(3, soilMoisture);
  ThingSpeak.setField(4, lightLux);
  // write to thingspeak channel
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if (x == 200) {
    Serial.println("ThingSpeak update successful.");
  }
  else {
    Serial.println("Problem updating ThingSpeak. HTTP error code " + String(x));
  }
}