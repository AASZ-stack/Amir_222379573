// LED
#define BUTTON_PIN 2
#define PORCH_LED 3
#define HALLWAY_LED 4
//timed events
const  unsigned long porchOnDuration = 30000; // 30 sec
const  unsigned long hallwayOnDuration = 60000; // 60sec
unsigned long eventStartTime = 0;
// bool
bool lightOn = false;
bool porchOn = false; // for effiency so the off actions runs once.
bool hallwayOn = false; // for consistency
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(BUTTON_PIN, INPUT_PULLUP); // button
  pinMode(PORCH_LED, OUTPUT); // (porch) blue led
  pinMode(HALLWAY_LED, OUTPUT); // (hallway) red led
}

// the loop function runs over and over again forever
void loop() {
  if (buttonStatus() && !lightOn) {
    eventStartTime = millis();
    lightOn = true;
    porchLightOn();
    porchOn = true;
    hallWayLightOn();
    hallwayOn = true;
  }
  if(lightOn) {
    unsigned long timer = millis() -  eventStartTime;
    if(timer >= porchOnDuration && porchOn) {
      porchLightOff();
      porchOn = false;
    }
    if(timer >= hallwayOnDuration && hallwayOn) {
      hallWayLightOff();
      hallwayOn = false;
      lightOn = false;
    }

  }
}

bool buttonStatus() {
  return digitalRead(BUTTON_PIN) == LOW;
}

void porchLightOn () {
  digitalWrite(PORCH_LED, HIGH);
}

void porchLightOff () {
  digitalWrite(PORCH_LED, LOW);
}

void hallWayLightOn () {
  digitalWrite(HALLWAY_LED, HIGH);
}

void hallWayLightOff () {
  digitalWrite(HALLWAY_LED, LOW);
}