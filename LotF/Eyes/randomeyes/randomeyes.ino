#include <FastLED.h>
#include <fastled_config.h>
#include <fastled_delay.h>
#include <fastled_progmem.h>
#include <fastpin.h>
#include <fastspi.h>
#include <fastspi_bitbang.h>
#include <fastspi_dma.h>
#include <fastspi_nop.h>
#include <fastspi_ref.h>
#include <fastspi_types.h>
#include <hsv2rgb.h>
#include <led_sysdefs.h>
#include <lib8tion.h>
#include <noise.h>
#include <pixelset.h>
#include <pixeltypes.h>
#include <platforms.h>
#include <power_mgt.h>

/*
 * TIMING
 */
//seconds until completely on
#define SECONDS_UNTIL_ON 15
//seconds to remain completely on
#define SECONDS_PAUSE_ON 3
//a percent of randomness to add to the wait between eyes tunring on
#define RANDOM_PCT 50

/*
 * COLOR MIN AND MAX
 */
#define MAX_RED 35
#define MIN_RED 20
#define MAX_GREEN 0
#define MIN_GREEN 0
#define MAX_BLUE 0
#define MIN_BLUE 0

//the number of eyes in the line
#define NUM_EYES 5*8

/*
 * don't change these
 */

//there are two leds per eye
#define NUM_LEDS NUM_EYES * 2

//pin for LED data
#define DATA_PIN A3
#define TRIGGER_PIN A2
#define TRIGGER_G_PIN A1

//our LEDs
CRGB leds[NUM_LEDS];

uint8_t eyeOrder[NUM_EYES];

void setup () {  
  //enable serial in case we want debug statements
  Serial.begin(115200);
  
  pinMode(DATA_PIN, OUTPUT);
  pinMode(TRIGGER_PIN, INPUT_PULLUP);
  pinMode(TRIGGER_G_PIN, OUTPUT);
  
  digitalWrite(TRIGGER_G_PIN, LOW);

  //add LEDs to FastLED library
  FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, NUM_LEDS);
  
  off();
  updateLeds();
  
  //initialize the order to turn on the eyes to the index; shuffle later
  for (uint8_t i = 0; i < NUM_EYES; i++) {
    eyeOrder[i] = i;
  }

  randomSeed(analogRead(A0));
}

void loop() {  
  
  //shuffle the order to show the eyes
  for (uint8_t i = 0; i < NUM_EYES; i++) {
    int n = random(0, NUM_EYES);
    uint8_t temp = eyeOrder[n];
    eyeOrder[n] = eyeOrder[i];
    eyeOrder[i] = temp;
  }

  //come up with our target per eye wait time
  float wait = (float)1 / (float)(NUM_EYES / (float)SECONDS_UNTIL_ON) * (float)1000;
  
  CRGB color;

  //go
  for (uint8_t c = 0; c < NUM_EYES; c++) {
    uint8_t c1 = random(MIN_RED, MAX_RED);
    uint8_t c2 = random(MIN_GREEN, MAX_GREEN);
    uint8_t c3 = random(MIN_BLUE, MAX_BLUE);
    color = CRGB(c1, c2, c3);
    showEye(eyeOrder[c], color);
    updateLeds();
    
    float curWait = random(wait - (wait * (float)((float)RANDOM_PCT / 100)), wait + (wait * (float)((float)RANDOM_PCT / 100)));
    //todo: make some eye blinking happen here
    delay(curWait);
  }
  
  //todo: make some eye blinking happen here too
  delay(SECONDS_PAUSE_ON * 1000);
  
  off();
  updateLeds();

  while (digitalRead(TRIGGER_PIN) == HIGH) {
    delay(200);
    //make sure it stays off, was possibly unplugged briefly
    updateLeds();
  }
}

void off() {
  for (uint8_t i = 0; i < NUM_EYES; i++) {
    showEye(i, CRGB::Black);
  }
}

void updateLeds() {
  FastLED.show();
}

void showEye(uint8_t i, CRGB c) {
  uint16_t li = i * 2;
  showLed(li, c);
  showLed(li + 1, c);
}

void showLed(uint16_t i, CRGB c) {
  leds[i] = c;
}

