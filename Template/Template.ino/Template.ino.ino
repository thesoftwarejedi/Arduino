#include <bitswap.h>
#include <chipsets.h>
#include <color.h>
#include <colorpalettes.h>
#include <colorutils.h>
#include <controller.h>
#include <cpp_compat.h>
#include <dmx.h>
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

#define NUM_LEDS 60
#define DATA_PIN A3

CRGB leds[NUM_LEDS];

void setup() {
  //for debugging perhaps
  Serial.begin(115200);

  pinMode(DATA_PIN, OUTPUT);
  
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  wholeStrip(CRGB::Black);
}

void loop() {
  // put your main code here, to run repeatedly:

}

void showLed(uint16_t i, CRGB c) {
  leds[i] = c;
}

void scrollDown() {
  for (uint16_t i = NUM_LEDS - 1; i >= 1; i--)
    leds[i] = leds[i - 1];
  leds[0] = CRGB::Black;
}

void scrollUp() {
  for (uint16_t i = 0; i < NUM_LEDS - 1; i++)
    leds[i] = leds[i + 1];
  leds[NUM_LEDS - 1] = CRGB::Black;
}

void slideDown(uint8_t speed) {
  for (uint16_t i = NUM_LEDS - 1; i > 0; i--) {
    leds[0] = CRGB::Black;
    scrollDown();
    FastLED.show();
    delay(speed);
  }
}

void slideUp(uint8_t speed) {
  for (uint16_t i = 0; i < NUM_LEDS - 1; i++) {
    leds[NUM_LEDS-1] = CRGB::Black;
    scrollUp();
    FastLED.show();
    delay(speed);
  }
}

void wholeStrip(CRGB color) {
  for (uint16_t i = 0; i < NUM_LEDS; i++)
    leds[i] = color;
}



void splitHalves(uint8_t speed, CRGB c1, CRGB c2) {
  //split the halves
  for (int i = LEDS / 2; i >= 0; i--) {
    leds[i] = c1;
    FastLED.show();
  }
  for (int i = LEDS / 2; i < LEDS; i++) {
    leds[i] = c2;
    FastLED.show();
  }
  delay(1000);
  //switch colors and flash
  for (int j = 0; j < 20; j++) {
    for (int i = 0; i < LEDS; i++)
      if (leds[i] == (CRGB)c1)
        leds[i] = c2;
      else
        leds[i] = c1;
    FastLED.show();
    delay(speed);
  }
  //set alternating
  for (int i = 0; i < LEDS; i++) {
    if (i % 4 == 0)
      leds[i] = c2;
    else if (i % 4 == 2)
      leds[i] = c1;
    else
      leds[i] = CRGB(0,0,0);
  }
  FastLED.show();
  //switch colors and flash
  for (int j = 0; j < 20; j++) {
    for (int i = 0; i < LEDS; i++)
      if (leds[i] == (CRGB)c1)
        leds[i] = c2;
      else
        leds[i] = c1;
    FastLED.show();
    delay(speed * 2);
  }
  wholeStrip(CRGB::White);
  delay(1000);
  //4 lines over white slow draw
  int j = LEDS/2;
  int k = 0;
  int l = LEDS - 1;
  for (int i = LEDS/2; i > 80; i--) {
    j++;
    k++;
    l--;
    leds[i] = c1;
    leds[j] = c2;
    leds[k] = c2;
    leds[l] = c1;
    FastLED.show();
    delay(speed);
  }
  delay(1000);
  //switch colors and flash
  for (int j = 0; j < 20; j++) {
    for (int i = 0; i < LEDS; i++)
      if (leds[i] == (CRGB)c1)
        leds[i] = c2;
      else
        leds[i] = c1;
    FastLED.show();
    delay(speed * 2);
  }
  slideUp(5);
}
