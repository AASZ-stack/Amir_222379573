//Libraries
#include <BH1750.h>
#include <Wire.h>
//PIR
#define PIR_PIN 2
//Switch
#define SWITCH_PIN 3
// LED
#define PORCH_LED 4
#define HALLWAY_LED 5
//Lightmeter
BH1750 lightMeter;
//global variables 
const float DARK = 60;
//  volatile required for ISR, so the complier does not optimizes it and can change outside
volatile bool motionTriggered = false;
volatile bool switchTriggered = false;
// timed events
const  unsigned long porchOnDuration = 30000; // 30 sec
const  unsigned long hallwayOnDuration = 60000; // 60sec
unsigned long eventStartTime = 0;
// bool
bool lightOn = false;
bool porchOn = false; // for effiency so the off actions runs once.
bool hallwayOn = false; // for consistency
void setup() {
  Serial.begin(115200);
  while (!Serial) {
  }
  Wire.begin();
  lightMeter.begin();
  pinMode(SWITCH_PIN, INPUT);
  pinMode(PORCH_LED, OUTPUT);
  pinMode(HALLWAY_LED, OUTPUT);
  pinMode(PIR_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(PIR_PIN), isrMotion, RISING); //raising to TRIGGER(the trigger) when the pin goes from low to high
  attachInterrupt(digitalPinToInterrupt(SWITCH_PIN), isrSwitch, CHANGE); //change to trigger when the switch changes state
}

void loop() {
  if(motionTriggered){
    motionTriggered = false;
    bool night = isNight();
    if(!lightOn && night){
      lightEvent();
      Serial.println("Motion was detected at night: lights on");
    }
    else if(!night){
      Serial.println("Motion detected, not at night: Lights off");
    }
  }
  if(switchTriggered){
    switchTriggered = false;
    if(!lightOn && switchStatus()){
      lightEvent();
      Serial.println("Slider switched ON: lights on");
    }
  }
  if(lightOn){
    unsigned long timer = millis() -  eventStartTime;
    if(timer >= porchOnDuration && porchOn) {
      porchLightOff();
      porchOn = false;
      Serial.println("Porch light off");
    }
    if(timer >= hallwayOnDuration && hallwayOn) {
      hallWayLightOff();
      hallwayOn = false;
      lightOn = false;
      Serial.println("Hallway light off");
    }
  }

}
//ISR - no parameters. no return. Runs because of the trrigger in attachInterrupt(). Its purpose is to record the event.
void isrMotion(){
  motionTriggered = true;
}

void isrSwitch(){
  switchTriggered = true;
}
//lights event
void lightEvent() {
  eventStartTime = millis();
  lightOn = true;
  porchLightOn();
  porchOn = true;
  hallWayLightOn();
  hallwayOn = true;
}

float lightSensor(){
  float lux = lightMeter.readLightLevel();
  Serial.print("Light: ");
  Serial.print(lux);
  Serial.println(" lx");
  return lux;
}

bool isNight(){
  float lux = lightSensor();
  if(lux < DARK){
    return true;
  }
  else{
    return false;
  }
}

bool switchStatus() {
  return digitalRead(SWITCH_PIN) == HIGH;
}

void porchLightOn() {
  digitalWrite(PORCH_LED, HIGH);
}

void porchLightOff() {
  digitalWrite(PORCH_LED, LOW);
}

void hallWayLightOn() {
  digitalWrite(HALLWAY_LED, HIGH);
}

void hallWayLightOff() {
  digitalWrite(HALLWAY_LED, LOW);
}