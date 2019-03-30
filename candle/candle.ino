#ifdef __AVR__
#include <avr/power.h>
#endif

// NeoPatterns will require the Adafruit NeoPixel library.  Be sure to install that.  
#include "NeoPatterns.h"

#define NEO_PIN 5    // First RGBW Strip Pin
#define NEO_COUNT 6  // First RGBW Strip Count
#define MIC_PIN A3   // Microphone Pin
#define FLAME_PIN A1 // IR Sensor Pin
#define SMOKE_PIN 3  // Smoke Pin
#define SMOKE_TIME_MILLIS 2500 // How long should smoke wire be heated?

#define FLAME_DURATION 1200 // millis- duration of flame before lighting candle

void Strip1Complete();

NeoPatterns Strip1(NEO_COUNT, NEO_PIN, NEO_RGB + NEO_KHZ800, &Strip1Complete);

// Vars to keep track of things
bool smoking = false;
bool flaming = false;

bool lit = false;
int flame_val = 0;
int mic_val = 0;
byte rnd = 0;

// Vars for tracking pseudo-async times
unsigned long previousMillis = 0;        // will store last time LED was updated
unsigned long smokeStartedMillis = 0;    // time smoking started
unsigned long flameStartedMillis = 0;        // will store last time LED was updated

void setup(){

    Serial.begin(9600); // uncomment this to figure sensor readings.

    // initialize pins
    pinMode(FLAME_PIN, INPUT);
    pinMode(MIC_PIN, INPUT);
    pinMode(SMOKE_PIN, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT); 

    // Make sure the following pins are OFF
    digitalWrite(SMOKE_PIN, LOW);  
    digitalWrite(LED_BUILTIN, LOW); 


    // Initialize NeoPixel Strip 1
    Strip1.begin();
    Strip1.show();
    Strip1.Color1 = Strip1.Color(62, 255, 4, 100);
    
    blowOutCandle(false);  // Make sure candle is off (but don't smoke)
}

void loop(){
    smokeCheck(); // Check if we should stop smoking.  
    checkSensors(); // Check for fire and strong winds
    Strip1.Update();
}

// Checks for fire and strong winds every 50 milliseconds
void checkSensors(){
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= 50) { // only check every 50ms
    flame_val = digitalRead( FLAME_PIN );
    mic_val = digitalRead( MIC_PIN );
    previousMillis = currentMillis;
    
    //  Uncomment below to check sensor readings.  
        Serial.print(mic_val);
        Serial.print(" : ");
        Serial.println(flame_val);
  }

  if (lit && mic_val == HIGH){ // MIC REGISTERED SOUND.  BLOW OUT.
    blowOutCandle(true); // Turn off light (and start SMOKING!)
    lit = false;
  }

  if (!lit){ // if candle isn't lit
    if (flame_val == LOW)  { // if match flame is detected
        
        // record the starting time (if candle hasn't been lit yet)
        if (!flaming){  
          flaming = true; 
          flameStartedMillis = millis();  // record time that flaming begins
        }
        else if (checkFlame()){ // has match been flaming long enough?          
          lightCandle(); // ok finally light it up. 
          lit = true; 
        }
    }
    else{ // no flame detected
      flaming = false; 
    }
    
  }  
}

// Check if match flame has been detected long enough.
bool checkFlame(){    
  unsigned long currentMillis = millis(); // get current time
  if (currentMillis - flameStartedMillis >= FLAME_DURATION) {
    return true;  
  }
  else
    return false;
}

void Strip1Complete(){
    Strip1.Reverse();

    if (Strip1.Direction == REVERSE){ // go back down
        Strip1.Interval = random(5,22); // choose random speed in range
    }
    else{  // pattern ended.  Stop, then trigger re-flicker
        rnd = random(random(3, 7),random(20, 55)); 

        Strip1.Pixel = random(0,Strip1.numPixels()); // pick a random Pixel
        Strip1.Interval = 1; 
        Strip1.Color2 = Strip1.Color(0, rnd, 0); //GRBW random red and random white-10
    }
}

// (pseudo)ASYNC light the candle by starting the animations
void lightCandle(){
    Strip1.Flicker(Strip1.Color1, Strip1.Color2, 20, 5);
}

void blowOutCandle(bool shouldSmoke){
  if(shouldSmoke)
    smokeStart();
    
  cWipe(0, 10); // synchronously turn the candle off.

  // We don't want no stinking patterns.
  Strip1.ActivePattern = NONE;
}

// SMOKING METHODS
void smokeStart(){
  digitalWrite(SMOKE_PIN, HIGH); // HEAT SMOKE WIRE
  smoking = true;
  smokeStartedMillis = millis(); // record time smoking started
}

// Stop smoking if the time has come.
void smokeCheck(){
  if (!smoking)
    return; // if not smoking, get out of here.
    
  unsigned long currentMillis = millis();
  if (currentMillis - smokeStartedMillis >= SMOKE_TIME_MILLIS) {
    smokeStop();    // smoke break is over
  }
}

// Ok seriously now stop it
void smokeStop(){
  digitalWrite(SMOKE_PIN, LOW); // TURN OFF SMOKE WIRE
  smoking = false;
}

void cWipe(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < Strip1.numPixels(); i++) {
    Strip1.setPixelColor(i, c);
    Strip1.show();
    delay(wait);
  }
}

