/*
 Name:		ChristmasLED.ino
 Created:	12/9/2017 2:31:39 PM
 Author:	danah
*/
#include <FastLED.h>

#define PIN 7
#define LEDS 142

//todo switches make pgm and adj selectable
bool adj = false;
bool pgm = false;

CRGB leds[LEDS];

uint8_t bottomS = 0;
uint8_t bottomE = 17;
uint8_t rightS = 18;
uint8_t rightE = 35;
uint8_t topS = 36;
uint8_t topE = 52;
uint8_t leftS = 53;
uint8_t leftE = 71;

void setup() {
	FastLED.addLeds<WS2812B, PIN, GRB>(leds, LEDS).setCorrection(TypicalLEDStrip);	
	FastLED.setBrightness(0);
	wholeStrip(CRGB::Black);

  Serial.begin(9600);
}

void loop() {
  if (pgm) {
    twinkle(100, 50, 5);
  
    CRGB color = randomColor();
    CRGB color2 = randomColor();  //change setled to allow selecting which strip or both, then do multicolor scans
  
    scan(bottomS, bottomE, color, 3, 10);
    scan(rightS, rightE, color, 3, 10);
    scan(topS, topE, color, 3, 10);
    scan(leftS, leftE, color, 3, 10);
    scan(bottomS, leftE, color, 1, 5);
    
    flicker(CRGB::White, 6, 100);
    flicker(color, 50, 100);
    flicker(CRGB::White, 6, 66);
    flicker(color, 67, 66);
    flicker(CRGB::White, 6, 33);
    flicker(color, 100, 33);
    flicker(CRGB::White, 6, 10);
    flicker(color, 200, 10);
  
    wholeStrip(CRGB::Black);
    delay(1000);
  
    wholeStrip(CRGB::White);
    delay(500);
  
    wholeStrip(CRGB::Black);
    delay(200);
  
    wholeStrip(CRGB::White);
    delay(200);
  
    wholeStrip(CRGB::Black);
    delay(500);
  }
  rainbowTravel(200, 255, 1);

  if (pgm) {
    wholeStrip(CRGB::Black);
    delay(5000);
  }
}

CRGB randomColor() {
  switch(random(8)) {
    case 0:
      return CRGB::White;
    case 1:
      return CRGB::Red;
    case 2:
      return CRGB::Orange;
    case 3:
      return CRGB::Yellow;
    case 4:
      return CRGB::Green;
    case 5:
      return CRGB::Blue;
    case 6:
      return CRGB::Purple;
     case 7:
      return CRGB::Pink;
  }
}

void flicker(CRGB color, int c, int d) {
  for (uint8_t i = 0; i < c; i++) {
    if (random(3) == 1)
      wholeStrip(color);
    else
      wholeStrip(CRGB::Black);
    delay(random(d));
  }
}

void scan(uint8_t s, uint8_t e, CRGB color, uint8_t c, uint8_t d) {
  bool up = true;
  while (c-- > 0) {
    if (up)
      for (uint8_t i = s; i <= e; i++) {
        wholeStrip(CRGB::Black);
        setLed(i, color);
        setDistanceBrightness();
        FastLED.show();
        delay(d);
      }
    else
      for (uint8_t i = e; i > s; i--) {
        wholeStrip(CRGB::Black);
        setLed(i, color);
        setDistanceBrightness();
        FastLED.show();
        delay(d);
      }
    up = !up;
  }
}

void rainbowTravel(int d, int c, int s) {
  Serial.println(d);
  Serial.println(c);
  Serial.println(s);
  
  for (int i = 0; i < c; i++) {
    Serial.println(i);
    travel(CRGB::Black, ColorFromPalette(RainbowColors_p, i+=s, 100, LINEARBLEND), d, 1);
  }
}

void travel(CRGB c1, CRGB c2, int d, int c) {
  for (;c > 0; c--) {
    for (int i = 0; i < 71; i++)
      leds[i] = c1;
    for (int i = 71; i < 143; i++)
      leds[i] = c2;
    setDistanceBrightness();
    FastLED.show();
    delay(d);
    for (int i = 0; i < 71; i++)
      leds[i] = c2;
    for (int i = 71; i < 143; i++)
      leds[i] = c1;
    setDistanceBrightness();
    FastLED.show();
    delay(d);
  }
}

void twinkle(uint16_t loop, uint8_t d, uint8_t count) {
  Serial.println("twinkle");
  
	for (uint16_t l = 0; l < loop; l++) {
		for (uint8_t c = 0; c < count; c++) {
			leds[random(LEDS)] = CRGB::White;
		}
    setDistanceBrightness();
		FastLED.show();
		delay(d);
		wholeStrip(CRGB::Black);
	}
}

void scrollDown() {
	for (uint16_t i = LEDS/2 - 1; i > 0; i--)
		setLed(i, leds[i - 1]);
	setLed(0, CRGB::Black);
}

void scrollUp() {
	for (uint16_t i = 0; i < LEDS/2 - 1; i++)
    setLed(i, leds[i + 1]);
	setLed(LEDS/2 - 1, CRGB::Black);
}

void slideDown(uint8_t speed) {
	for (uint16_t i = LEDS/2 - 1; i > 0; i--) {
		scrollDown();
    setLed(0, CRGB::Black);
    setDistanceBrightness();
		FastLED.show();
		delay(speed);
	}
}

void slideUp(uint8_t speed) {
	for (uint16_t i = 0; i < LEDS/2 - 1; i++) {
		scrollUp();
    setLed(LEDS/2 - 1, CRGB::Black);
    setDistanceBrightness();
		FastLED.show();
		delay(speed);
	}
}

void wholeStrip(CRGB color) {
	for (uint16_t i = 0; i < LEDS/2; i++)
		setLed(i, color);
  setDistanceBrightness();
	FastLED.show();
}

void setLed(int i, CRGB c) {
  leds[i] = c;
  leds[LEDS - i - 1] = c; 
}

void setDistanceBrightness() {
  FastLED.setBrightness(readDistance());
}

//returns a value of closeness - 0 for far, 100 for near and linear spread otherwise
int readDistance() {
  if (adj) {
    uint16_t sensorValue = analogRead(A0);
    for (byte i = 0; i < 3; i++) {
      delay(3);
      uint16_t sensorValue2 = analogRead(A0);
      sensorValue = sensorValue2 < sensorValue ? sensorValue2 : sensorValue;
    }
    
    if (sensorValue < 100)
      return 0;
    if (sensorValue > 600)
      return 100;
      
    sensorValue = sensorValue - 100;
    sensorValue = sensorValue / 5;
    //Serial.println(sensorValue);
    return sensorValue;
  } else {
    return 100;
  }
}

