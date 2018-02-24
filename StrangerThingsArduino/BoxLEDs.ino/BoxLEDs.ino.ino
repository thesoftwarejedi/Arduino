#include <FastLED.h>

#define LED_PIN     9
#define NUM_LEDS    14
#define BRIGHTNESS  255
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define UPDATES_PER_SECOND 100

CRGB leds[NUM_LEDS];
CRGBPalette16 currentPalette;
TBlendType    currentBlending;

extern const TProgmemPalette16 stColors_p PROGMEM;

void setup() {
  delay(3000); // power-up safety delay
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);

  currentPalette = stColors_p;
  currentBlending = NOBLEND;
}

void loop() {
  
  
  
  doPalette(10000);
  
  flicker(1000);



  
  wholeStrip(CRGB::Black);
  delay(1000000);
}

void flicker(uint8_t iterations) {
  for (uint8_t i = 0; i < iterations; i++) {
    if (random(2) < 1)
      wholeStrip(CRGB::White);
    else
      wholeStrip(CRGB::Black);
    FastLED.delay(random(100));
  }
}

static uint8_t startIndex = 0;
void doPalette(uint8_t iterations) {
  for (uint8_t i = 0; i < iterations; i++) {
    startIndex++;
    FillLEDsFromPaletteColors(startIndex);
    FastLED.show();
    FastLED.delay(1000 / UPDATES_PER_SECOND);
  }
}

void FillLEDsFromPaletteColors(uint8_t colorIndex) {
  uint8_t brightness = 255;
  for ( int i = 0; i < NUM_LEDS; i++) {
    leds[i] = ColorFromPalette(currentPalette, colorIndex, brightness, currentBlending);
    colorIndex += 3;
  }
}

void wholeStrip(CRGB color) {
  for (uint16_t i = 0; i < NUM_LEDS; i++)
    leds[i] = color;
  FastLED.show();
}

const TProgmemPalette16 stColors_p PROGMEM = {
  CRGB(7, 62, 30),
  CRGB(7, 62, 30),
  CRGB(7, 62, 30),

  CRGB(58, 95, 229),
  CRGB(58, 95, 229),
  CRGB(58, 95, 229),

  CRGB(255, 21, 21),
  CRGB(255, 21, 21),
  CRGB(255, 21, 21),

  CRGB(30, 7, 7),
  CRGB(30, 7, 7),
  CRGB(30, 7, 7),

  CRGB(30, 25, 60),
  CRGB(30, 25, 60),
  CRGB(30, 25, 60),
  CRGB(30, 25, 60)
};
