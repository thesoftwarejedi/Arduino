#include "FastLED.h"

#define NUM_EYES 8.
#define NUM_LEDS NUM_EYES * 2
#define DATA_PIN 5

CRGB leds[NUM_LEDS];

void setup() { 
  FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, NUM_LEDS);
  for (uint8_t i = 0; i < NUM_LEDS; i++)
  {
    showLed(i, CRGB::Black);
  }
  FastLED.show();
}

void loop() { 
  for (uint8_t i = 0; i < NUM_EYES; i++)
  {
    showEye(i, CRGB::Red);
    FastLED.show();
    delay(500);
    showEye(i, CRGB::Black);
    FastLED.show();
    delay(500);
  } 
}

void showEye(uint8_t i, CRGB c) {
  i *= 2;
  showLed(i, c);
  showLed(i + 1, c);
}

void showLed(uint8_t i, CRGB c) {
  leds[i] = c;
}

