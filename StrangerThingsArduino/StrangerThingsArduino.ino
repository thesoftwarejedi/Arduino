/*
Name:    ChristmasLED.ino
Created: 12/9/2017 2:31:39 PM
Author:  danah
*/
#include <FastLED.h>

#define PIN 5
#define LEDS 100
#define BRIGHTNESS 80

CRGB leds[LEDS];
String out = "";
bool normal = false;
bool blank = false;
bool defaultOn = true; 
extern const TProgmemPalette16 christmasLightPalette PROGMEM;

void setup() {
  pinMode(PIN, OUTPUT);
  pinMode(5, OUTPUT);
  FastLED.addLeds<WS2811, PIN, RGB>(leds, LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  Serial.begin(115200);
}

void loop() {
  if (out.length() > 0) {
    String ctx = out;
    out = "";
    if (ctx == "on") {
      defaultOn = true;
      normal = false;
      return;
    } else if (ctx == "off") {
      defaultOn = false;
      blank = false;
      return;
    }
    flicker();
    blank = false;
    allOff();
    FastLED.show();
    uint8_t c;
    for (uint8_t i = 0; i < ctx.length(); i++) {
      c = ctx.charAt(i);
      if (c != 'X') {
        Serial.write(c);
        c = c - 'a';
        light(c * 3+5);
        FastLED.show();
        delay(1000);
        leds[c * 3+5] = CRGB::Black;
        FastLED.show();
        delay(100);
      }
      else {
        Serial.write('X');
        FastLED.show();
        delay(1000);
      }
    }
    flicker();
    normal = false;
    blank = false;
  }
  if (defaultOn) {
    normalColors(true);
  } else {
    allOff();
  }
  delay(5000);
}

void flicker() {
  for (int i = 0; i < 100; i++) {
    if (random(2) == 1) {
      normal = false;
      normalColors(true);
    } else {
      blank = false;
      allOff();
    }
    delay(random(150));
  }
  normal = false;
}

void allOff() {
  if (!blank) {
    for (int i = 0; i < LEDS; i++) {
      leds[i] = CRGB::Black;
    }
    FastLED.show();
    blank = true;
  }
}

void randomFadeOut(uint8_t wait) {
  normalColors(true);
  for (int j = 40; j >= 0; j--) {
    for (int i = 0; i < LEDS; i++) {
      CHSV c = rgb2hsv_approximate(leds[i]);
      c.v = random(j * 6);
      leds[i] = c;
    }
    FastLED.show();
    delay(wait);
  }
}

void randomFadeIn() {
  for (int j = 0; j <= 40; j++) {
    normal = false;
    blank = false;
    normalColors(false);
    for (int i = 0; i < LEDS; i++) {
      CHSV c = rgb2hsv_approximate(leds[i]);
      c.v = random(j * 6);
      leds[i] = c;
    }
    FastLED.show();
    delay(100);
  }
  normal = false;
}

void normalColors(bool showThem) {
  if (!normal) {
    normal = true;
    for (int i = 0; i < LEDS; i++) {
      light(i);
    }
    if (showThem)
      FastLED.show();
  }
}

void light(uint8_t i) {
  //NOBLEND
  //LINEARBLEND
  leds[i] = ColorFromPalette(christmasLightPalette, i * 16, BRIGHTNESS, NOBLEND);
}

void serialEvent() {
  delay(1000);
  do {
    while (Serial.available() > 0) {
      uint8_t in = Serial.read();
      if (in >= 'a' && in <= 'z') {
        out = out + (char)in;
      } else {
        out = out + 'X';
      }
      delay(10);
      Serial.write(0);
    }
    delay(2000);
  } while (Serial.available() > 0);
}

const TProgmemPalette16 christmasLightPalette PROGMEM =
{
    CRGB::Yellow,
    CRGB::Blue,
    CRGB::Red,
    CRGB::Green,
    CRGB::Teal,
    CRGB::Yellow,
    CRGB::Pink,
    CRGB::LightBlue,
    CRGB::Blue,
    CRGB::Red,
    CRGB::Violet,
    CRGB::DarkOrange,
    CRGB::Yellow,
    CRGB::Teal,
    CRGB::Pink,
    CRGB::Green
};




