/*
Name:    ChristmasLED.ino
Created: 12/9/2017 2:31:39 PM
Author:  danah
*/
#include <FastLED.h>

#define PIN 7
//#define LEDS 443
#define LEDS 300
#define BRIGHTNESS 50
#define KINDA_WHITE 25

CRGB leds[LEDS];
CRGB kindaWhite = CRGB(KINDA_WHITE, KINDA_WHITE, KINDA_WHITE);
String out = "";
bool normal = false;

void setup() {
  pinMode(PIN, OUTPUT);
  pinMode(5, OUTPUT);
  FastLED.addLeds<WS2812B, PIN, GRB>(leds, LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  Serial.begin(115200);
}

void loop() {
  if (out.length() > 0) {
    Serial.println(out);
    arrival();
    String ctx = out;
    out = "";
    uint8_t c;
    for (uint8_t i = 0; i < ctx.length(); i++) {
      c = ctx.charAt(i);
      if (c != 'X') {
        Serial.write(c);
        c = c - 'a';
        light(c);
        FastLED.show();
        delay(1000);
        leds[c * 10] = CRGB::Black;
        FastLED.show();
        delay(100);
      }
      else {
        Serial.write('X');
        FastLED.show();
        delay(1000);
      }
    }
    departure();
  }
  normal = false;
  normalColors(true);
  delay(5000);
}

void arrival() {
  normalColors(true);
  for (int j = 40; j >= 0; j--) {
    for (int i = 0; i < 260; i = i + 10) {
      CHSV c = rgb2hsv_approximate(leds[i]);
      c.v = random(j * 6);
      leds[i] = c;
    }
    FastLED.show();
    delay(100);
  }
}

void departure() {
  for (int j = 0; j <= 40; j++) {
    normal = false;
    normalColors(false);
    for (int i = 0; i < 260; i = i + 10) {
      CHSV c = rgb2hsv_approximate(leds[i]);
      c.v = random(j * 6);
      leds[i] = c;
    }
    FastLED.show();
    delay(100);
  }
}

void normalColors(bool showThem) {
  if (!normal) {
    normal = true;
    for (int i = 0; i < 26; i++) {
      light(i);
    }
    if (showThem)
      FastLED.show();
  }
}

void light(uint8_t i) {
  switch (i % 7) {
      case 0:
        leds[i * 10] = CRGB::Red;
        break;
      case 1:
        leds[i * 10] = CRGB::Orange;
        break;
      case 2:
        leds[i * 10] = CRGB::Yellow;
        break;
      case 3:
        leds[i * 10] = CRGB::Green;
        break;
      case 4:
        leds[i * 10] = CRGB::Blue;
        break;
      case 5:
        leds[i * 10] = CRGB::Purple;
        break;
      case 6:
        leds[i * 10] = CRGB::Indigo;
        break;
      }
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




