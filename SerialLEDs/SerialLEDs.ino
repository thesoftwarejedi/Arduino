/*
 Name:		ChristmasLED.ino
 Created:	12/9/2017 2:31:39 PM
 Author:	danah
*/
#include <FastLED.h>

#define PIN 7
//#define LEDS 443
#define LEDS 300
#define BRIGHTNESS 50
#define KINDA_WHITE 25

CRGB leds[LEDS];
CRGB kindaWhite = CRGB(KINDA_WHITE, KINDA_WHITE, KINDA_WHITE);
uint16_t x = 0;
uint8_t y = 0;

void setup() {
  pinMode(PIN, OUTPUT);
  pinMode(5, OUTPUT);
	FastLED.addLeds<WS2812B, PIN, GRB>(leds, LEDS).setCorrection(TypicalLEDStrip);
	FastLED.setBrightness(BRIGHTNESS);
  Serial.begin(614400);
}

void loop() {
}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    uint8_t in = (uint8_t)Serial.read();
    // if the incoming character is 255 send to LED strip
    if (in == 255) {
      digitalWrite(5, HIGH);
      delay(100);
      digitalWrite(5, LOW);
      FastLED.show();
      x = 0;
      y = 0;
    } else {
      // add it to the array
      if (y == 0)
        leds[x].r = in;
      else if (y == 1)
        leds[x].g = in;
      else
        leds[x].b = in;
        
      y++;
      if (y == 3) {
        y = 0; 
        x++;
        if (x == LEDS) {
          x = 0;
        }
      }
    }
  }
}

