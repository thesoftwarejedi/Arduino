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

#include <DMXSerial.h>

//max is 12 strands of 8
#define NUM_EYES 12*8

//there are two leds per eye
#define NUM_LEDS NUM_EYES * 2

//pin 11 for LED data
#define DATA_PIN 10

//pin 12 for a simple status LED
#define ERROR_PIN 12

//we start at channel 1
#define DMXSTART 1

//max channels, just use em all
#define DMXLENGTH 512

//our LEDs
CRGB leds[NUM_LEDS];

void setup () {
  //we want to probe - not be interrupted.  interrupts will kill the LED timings
  DMXSerial.init(DMXProbe);
  DMXSerial.maxChannel(DMXLENGTH);
  
  pinMode(DATA_PIN, OUTPUT);
  pinMode(ERROR_PIN, OUTPUT);

  //add LEDs to FastLED library
  FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, NUM_LEDS);

//blink the LED on startup
  digitalWrite(ERROR_PIN, HIGH);
  delay(200);
  digitalWrite(ERROR_PIN, LOW);
  delay(200);
  digitalWrite(ERROR_PIN, HIGH);
  delay(200);
  digitalWrite(ERROR_PIN, LOW);
  delay(200);
  digitalWrite(ERROR_PIN, HIGH);
  delay(200);
  digitalWrite(ERROR_PIN, LOW);

  //clear lights
  off();
  updateLeds();
}

void loop() {
  if (DMXSerial.receive(100)) {
    digitalWrite(ERROR_PIN, HIGH); //status pin off
    uint8_t *data = DMXSerial.getBuffer();
    uint8_t mod = data[1]; //channel 1 tells us what to modulus by
    if (mod == 0) {
      off();
      updateLeds();
      return;
    }
    for (uint8_t i = 0; i < NUM_EYES; i++) {
      uint8_t channel = (i % mod) * 3 + 2; //at mod 1, everything is chan 2,3,4.  mod 2 is 2,3,4 and 5,6,7.  mod 3 is 2,3,4 and 5,6,7 and 8,9,10
      showEye(i, CRGB(data[channel], data[channel+1], data[channel+2]));
    }
    updateLeds();
  } else {
    off();
    updateLeds();
    digitalWrite(ERROR_PIN, LOW);
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

