/*
  Name:    ChristmasLED.ino
  Created: 12/9/2017 2:31:39 PM
  Author:  danah
*/
#include <FastLED.h>

#define MOTION_PIN 6

#define PIN 7
//#define LEDS 443
#define LEDS 90
#define BRIGHTNESS 100
#define KINDA_WHITE 50

CRGB leds[LEDS];
CRGB kindaWhite = CRGB(KINDA_WHITE, KINDA_WHITE, KINDA_WHITE);

void setup() {
  pinMode(MOTION_PIN, INPUT);
  
  FastLED.addLeds<WS2812B, PIN, GRB>(leds, LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  wholeStrip(CRGB::Black);
  FastLED.show();

  delay(2000);
}

bool val = LOW;

void loop() {
  val = digitalRead(MOTION_PIN);

  if (val == LOW) {
    delay(500);
    return;
  }
  
  fireBall(15, 60, false);
  fireBall(18, 50, true);
  fireBall(21, 40, false);
  fireBall(24, 30, true);
  fireBall(27, 20, false);
  fireBall(30, 10, true);
  fireBall(33, 0, false);
  fireBall(35, 0, true);
  fireBall(40, 0, false);
  fireBall(45, 0, true);
  fireBall(50, 0, false);

  twinkle(20, 100, 5);
  twinkle(25, 80, 10);
  twinkle(30, 60, 15);
  twinkle(35, 40, 20);
  twinkle(40, 20, 25);

  wholeStrip(CRGB::Black);
  FastLED.show();
  delay(1000);

  olympicShow(1000, true);
  
  for (uint8_t i = 8; i > 0 ; i--) {
    olympicRemove(75, true);
    olympicShow(75, true);
    olympicRemove(75, false);
    olympicShow(75, false);
  }
  delay(60000);
  
  olympicRemove(1000, false);
  wholeStrip(CRGB::Black);
  FastLED.show();

  delay(1000);

  for (uint8_t i = 0; i < 100; i++) {
    wholeStrip(CRGB::Black);
    olympicShow(random(5)+1);
    FastLED.show();
    delay(100);
  }

  olympicRemove(1000, false);
  wholeStrip(CRGB::Black);
  FastLED.show();

  delay(1000);
}

void olympicShow(int wait, bool forward) {
  wholeStrip(CRGB::Black);
  FastLED.show();
  if (forward) {
    for (uint8_t i = 1; i < 6; i++) {
      olympicShow(i);
      FastLED.show();
      delay(wait);
    }
  } else {
    for (uint8_t i = 5; i > 0; i--) {
      olympicShow(i);
      FastLED.show();
      delay(wait);
    }
  }
}

void olympicRemove(int wait, bool forward) {
  if (forward) {
    for (uint8_t i = 1; i < 6; i++) {
      wholeStrip(CRGB::Black);
      for (uint8_t j = i; j < 6; j++)
        olympicShow(j);
      FastLED.show();
      delay(wait);
    }
  } else {
    for (uint8_t i = 5; i > 0; i--) {
      wholeStrip(CRGB::Black);
      for (uint8_t j = i; j > 0; j--)
        olympicShow(j);
      FastLED.show();
      delay(wait);
    }
  }
  wholeStrip(CRGB::Black);
  FastLED.show();
  delay(wait);
}

void olympicShow(uint8_t l) {
  uint8_t n = 0;
  for (uint8_t m = (l - 1) * LEDS / 5; n < 18; n++) {
    olympicLight(m++);
  }
}

void olympicLight(uint8_t i) {
  switch ((i + 1) / (LEDS / 5)) {
    case 0:
      leds[i] = CRGB::Blue;
      break;
    case 1:
      leds[i] = CRGB::Yellow;
      break;
    case 2:
      leds[i] = CRGB::White;
      break;
    case 3:
      leds[i] = CRGB::Green;
      break;
    case 4:
      leds[i] = CRGB::Red;
      break;
  }
}

void twinkle(uint16_t loop, uint8_t speed, uint8_t count) {
  for (uint16_t l = 0; l < loop; l++) {
    for (uint8_t c = 0; c < count; c++) {
      leds[random(LEDS)] = CRGB::White;
    }
    FastLED.show();
    delay(speed);
    wholeStrip(CRGB::Black);
  }
}

void scrollDown() {
  for (uint16_t i = LEDS - 1; i >= 1; i--)
    leds[i] = leds[i - 1];
  leds[0] = CRGB::Black;
}

void scrollUp() {
  for (uint16_t i = 0; i < LEDS - 1; i++)
    leds[i] = leds[i + 1];
  leds[LEDS - 1] = CRGB::Black;
}

void slideDown(uint8_t speed) {
  for (uint16_t i = LEDS - 1; i > 0; i--) {
    leds[0] = CRGB::Black;
    scrollDown();
    FastLED.show();
    delay(speed);
  }
}

void slideUp(uint8_t speed) {
  for (uint16_t i = 0; i < LEDS - 1; i++) {
    leds[LEDS - 1] = CRGB::Black;
    scrollUp();
    FastLED.show();
    delay(speed);
  }
}

void wholeStrip(CRGB color) {
  for (uint16_t i = 0; i < LEDS; i++)
    leds[i] = color;
}

void fireBall(uint8_t sz, uint8_t wait, bool forward) {
  int temp;
  CRGB color;
  for (uint16_t i = 0; i < LEDS + sz + 1; i += 2) {
    for (uint8_t j = 0; j <= sz; j++) {
      if (j > i) break;
      if (j == 0) {
        color = CRGB(255, 255, 230);
      }
      else {
        color = CRGB(255, (float)j / sz * 153, (float)j / sz * 18);
      }
      temp = i - j;
      if (temp >= 0 && temp < LEDS)
        setLed(temp, color, forward);
      temp--;
      if (temp >= 0 && temp < LEDS)
        setLed(temp, color, forward);
    }

    //tail off
    temp = i - sz - 1;
    if (temp >= 0 && temp < LEDS)
      setLed(temp, CRGB::Black, forward);
    temp--;
    if (temp >= 0 && temp < LEDS)
      setLed(temp, CRGB::Black, forward);
    FastLED.show();
    delay(wait);
  }
}

uint8_t fadeLen = LEDS / 4;
void fadeall(uint8_t sp, int except, bool forward) {
  int temp;
  CRGB tempColor;
  for (int i = 0; i < LEDS; i++) {
    if (i >= except)
      nscaleLed(LEDS - 1 - i, sp, forward);
    else {
      temp = except - i;
      tempColor = kindaWhite;
      if (temp < fadeLen)
      {
        int f = 255 * (fadeLen - temp) / fadeLen;
        tempColor.fadeToBlackBy(f);
      }
      setLed(LEDS - 1 - i, tempColor, forward);
    }
  }
}

void setLed(int i, CRGB color, bool forward) {
  i = abs(forward * LEDS - i);
  leds[i] = color;
}

CRGB getLed(int i, bool forward) {
  i = abs(forward * LEDS - i);
  return leds[i];
}

void nscaleLed(int i, uint8_t sp, bool forward) {
  i = abs(forward * LEDS - i);
  leds[i].nscale8(sp);
}


