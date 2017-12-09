#include <FastLED.h>

#define MOTION_PIN 5
#define MOTION_PIN2 6
#define PIN 7
//#define LEDS 443
#define LEDS 300
#define BRIGHTNESS 75
#define KINDA_WHITE 50

CRGB leds[LEDS];
CRGB kindaWhite = CRGB(KINDA_WHITE, KINDA_WHITE, KINDA_WHITE);

void setup() {
  pinMode(MOTION_PIN, INPUT);
  pinMode(MOTION_PIN2, INPUT);
  FastLED.addLeds<WS2812B,PIN,GRB>(leds,LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  wholeStrip(CRGB::Black);
}

bool val = LOW;
bool val2 = LOW;
void loop() {
  val = digitalRead(MOTION_PIN);
  val2 = digitalRead(MOTION_PIN2);

  if (val | val2 == HIGH) {
    wholeStrip(CRGB::Black);
    fireBall(25, 0, val == HIGH);
    rainbowBall(180, 255, 250, val == HIGH);
    while (!mellow(10, val == HIGH)) {
      wholeStrip(kindaWhite);
    }
    wholeStrip(CRGB::Black);
  }
  delay(500);
}

void wholeStrip(CRGB color) {
  for(uint16_t i=0; i<LEDS; i++) {
    setLed(i, color, true);
  }
  FastLED.show();
}

void fireBall(uint8_t sz, uint8_t wait, bool forward) {
  int temp;
  CRGB color;
  for(uint16_t i=0; i<LEDS+sz+1; i+=2) {
    for(uint8_t j=0; j<=sz; j++) {
      if (j > i) break;
      if (j == 0) {
        color = CRGB(255, 255, 230);
      } else {
        color = CRGB(255, (float)j/sz*153, (float)j/sz*18);
      }
      temp = i-j;
      if (temp >= 0 && temp < LEDS)
        setLed(temp, color, forward);
      temp--;
      if (temp >= 0 && temp < LEDS)
        setLed(temp, color, forward);
    }
    
    //tail off
    temp = i-sz-1;
    if (temp >= 0 && temp < LEDS)
      setLed(temp, CRGB::Black, forward);
    temp--;
    if (temp >= 0 && temp < LEDS)
      setLed(temp, CRGB::Black, forward);
    FastLED.show();
    delay(wait);
  }
}

uint16_t sparkleCount = LEDS/15;

bool mellow(uint16_t d, bool forward) {
  for (uint8_t i = kindaWhite.g; i > 0; i--) {
    for (int j = 0; j < d; j++)
      SnowSparkle(sparkleCount, 40, 10, 1000, 255, forward);
    if (digitalRead(MOTION_PIN) == HIGH || digitalRead(MOTION_PIN2) == HIGH)
      return false;
    wholeStrip(CRGB(i, i, i));
  }
  return true;
}

uint8_t backStop = LEDS/4;
void rainbowBall(uint8_t s, uint8_t v, uint8_t fade, bool forward) {
  static uint8_t hue = 0;
  for(int i = 0; i < LEDS + backStop; i++) {
    if (i >= 0 && i < LEDS)
      setLed(LEDS-1-i, CHSV(hue++, s, v), forward);
    fadeall(fade, i - backStop, forward);
    SnowSparkle(sparkleCount, 0, 0, i - backStop, 200, forward);
  }
}

uint8_t fadeLen = LEDS/4;
void fadeall(uint8_t sp, int except, bool forward) {
  int temp;
  CRGB tempColor; 
  for(int i = 0; i < LEDS; i++) {
    if (i >= except)
      nscaleLed(LEDS-1-i, sp, forward); 
    else {
      temp = except-i;
      tempColor = kindaWhite;
      if (temp < fadeLen)
      {
        int f = 255*(fadeLen-temp)/fadeLen;
        tempColor.fadeToBlackBy(f);
      }
      setLed(LEDS-1-i, tempColor, forward); 
    }
  } 
}

void SnowSparkle(uint8_t sparkleCount, uint8_t SparkleDelay, uint8_t SpeedDelay, int limit, uint8_t brightness, bool forward) {
  if (brightness > 255)
    brightness = 255;
  int sparkles[sparkleCount];
  CRGB oldColor[sparkleCount];
  for(int i = 0; i < sparkleCount; i++) {
    sparkles[i] = random(LEDS);
    if (sparkles[i] < limit) {
      oldColor[i] = getLed(sparkles[i], forward);
      setLed(LEDS - sparkles[i], CRGB(CHSV(random(255), 150, brightness)), forward);
    }
  }
  FastLED.show();
  delay(SparkleDelay);
  for(int i = 0; i < sparkleCount; i++) {
    if (sparkles[i] < limit) 
      setLed(LEDS - sparkles[i], oldColor[i], forward);
  }
  FastLED.show();
  delay(SpeedDelay);
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


