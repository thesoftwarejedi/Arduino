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

//timing
#define SECONDS_UNTIL_ON 15
#define SECONDS_PAUSE_ON 3

//max is 12 strands of 8
#define NUM_EYES 1

//there are two leds per eye
#define NUM_LEDS NUM_EYES * 2

//pin for LED data
#define DATA_PIN A3
#define TRIGGER_PIN A2
#define TRIGGER_G_PIN A1

//random color to assign
#define MAX_RED 35
#define MIN_RED 20
#define MAX_GREEN 0
#define MIN_GREEN 0
#define MAX_BLUE 0
#define MIN_BLUE 0

//randomness
#define RANDOM_PCT 20

//our LEDs
CRGB leds[NUM_LEDS];

uint8_t eyeOrder[NUM_EYES];

void setup () {  
  pinMode(DATA_PIN, OUTPUT);
  pinMode(TRIGGER_PIN, INPUT_PULLUP);
  pinMode(TRIGGER_G_PIN, OUTPUT);
  
  digitalWrite(TRIGGER_G_PIN, LOW);


  //add LEDs to FastLED library
  FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, NUM_LEDS);
  
  off();
  updateLeds();
  
  //init to the index
  for (uint8_t i = 0; i < NUM_EYES; i++) {
    eyeOrder[i] = i;
  }

  randomSeed(analogRead(A0));
}

void loop() {  
  //shuffle
  for (uint8_t i = 0; i < NUM_EYES; i++) {
    int n = random(0, NUM_EYES);
    uint8_t temp = eyeOrder[n];
    eyeOrder[n] = eyeOrder[i];
    eyeOrder[i] = temp;
  }

  //now go
  float wait = 1 / (NUM_EYES / SECONDS_UNTIL_ON) * 1000;
  CRGB color;

  for (uint8_t c = 0; c < NUM_EYES; c++) {
    uint8_t c1 = random(MIN_RED, MAX_RED);
    uint8_t c2 = random(MIN_GREEN, MAX_GREEN);
    uint8_t c3 = random(MIN_BLUE, MAX_BLUE);
    color = CRGB(c1, c2, c3);
    showEye(eyeOrder[c], color);
    updateLeds();
    delay(random(wait - wait * RANDOM_PCT, wait + wait * RANDOM_PCT));
  }
  
  delay(SECONDS_PAUSE_ON * 1000);
  
  off();
  updateLeds();

  while (digitalRead(TRIGGER_PIN) == HIGH) {
    delay(200);
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

