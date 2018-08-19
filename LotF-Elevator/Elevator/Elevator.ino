/*
 Name:		Elevator.ino
 Created:	8/20/2017 2:31:39 PM
 Author:	danah
*/
#include <FastLED.h>

#define PIN1 5
#define LEDS 150
#define BRIGHTNESS 20
#define FADE_LEN 20
#define MAX_SPEED 1
#define MIN_SPEED 90
#define ACCELERATION .7
#define ACCEL_CONSTANT 1
#define NUM_FLOORS 8
#define FLOOR_LEN 75
#define BLINK_ODDS 60
#define MAIN_COLOR CRGB(200, 56, 0)

//#define DEBUG 1

CRGB leds[LEDS];
CRGB black[LEDS];
CRGB temp[LEDS];
CRGB fade[FADE_LEN];
CRGB fade2[FADE_LEN];

void setup() {
  
  #ifdef DEBUG
  Serial.begin(9600);
  #endif 
  
  FastLED.addLeds<WS2812B, PIN1, GRB>(leds, LEDS).setCorrection(TypicalLEDStrip);
	FastLED.setBrightness(BRIGHTNESS);

  setMainFloor(MAIN_COLOR);
  grabFades();
  
  #ifdef DEBUG
  for (uint8_t i = 0; i < FADE_LEN; i++) {
    Serial.println(fade[i].red);
    Serial.println(fade[i].green);
    Serial.println(fade[i].blue);
  }
  for (uint8_t i = 0; i < FADE_LEN; i++) {
    Serial.println(fade2[i].red);
    Serial.println(fade2[i].green);
    Serial.println(fade2[i].blue);
  }
  #endif 
}

void loop() {
  setMainFloor(MAIN_COLOR);
  for (uint8_t i = 0; i < 50; i++)
  {
    draw();
    delay(100);
  }
  slideDown(MIN_SPEED, ACCELERATION * -1);

  CHSV hsv;
  CRGB color;
  for (uint8_t i = 0; i < NUM_FLOORS; i++) {
    delay(500);
    hsv = CHSV(30 - (30 / NUM_FLOORS) * (i + 1), 255, 255); 
    hsv2rgb_rainbow(hsv, color);
    drawFloor(color, FLOOR_LEN, MAX_SPEED, 0);
    slideDown(MAX_SPEED, 0);
  }

  drawFloor(color, LEDS - FADE_LEN * 2, MAX_SPEED, ACCELERATION);
  
  while(true)
  {
    draw();
    delay(100);
  }
}

void setMainFloor(CRGB color) {
  uint8_t fadeAmt;
  wholeStrip(color);
  for (uint8_t i = 0; i < FADE_LEN; i++) {
    fadeAmt = 255 - (uint8_t)((float)i / FADE_LEN * 256);

    #ifdef DEBUG
    Serial.print("led ");
    Serial.print(i);
    Serial.print(" fade ");
    Serial.println(fadeAmt);
    #endif 
    
    leds[i].fadeToBlackBy(fadeAmt);
    leds[LEDS - 1 - i].fadeToBlackBy(fadeAmt);
  } 
}

void grabFades() {
  uint16_t len = FADE_LEN * sizeof(CRGB);  
  memcpy(fade, leds, len);
  for (uint8_t i = 0; i < FADE_LEN; i++) {
    fade2[i] = fade[FADE_LEN - 1 - i];
  }
}

void drawFloor(CRGB color, uint16_t len, float curSpeed, float delta) {
  //kinda lame, but draw the whole thing and grabe the fades, then clear
  setMainFloor(color);
  grabFades();
  wholeStrip(CRGB(0,0,0));

  int16_t whenToStartDelta = LEDS - abs((MIN_SPEED - MAX_SPEED) / delta);

  #ifdef DEBUG
  Serial.println(whenToStartDelta);
  #endif

  float newSpeed;
  for (uint8_t i = 0; i < FADE_LEN; i++) {
    delay(curSpeed * ACCEL_CONSTANT - ACCEL_CONSTANT + 1);    

    #ifdef DEBUG
    Serial.println(curSpeed);
    #endif
    
    newSpeed = curSpeed + delta;
    if (newSpeed > MIN_SPEED)
      curSpeed = MIN_SPEED;
    else if (newSpeed < MAX_SPEED)
      curSpeed = MAX_SPEED;
    else if (whenToStartDelta-- < 0)
      curSpeed = newSpeed;

    #ifdef DEBUG
    Serial.println(whenToStartDelta);
    #endif
    
    scrollDown();
    setLed(0, fade2[FADE_LEN - i - 1]);
    draw();
  }
  for (uint8_t i = 0; i < len; i++) {
    delay(curSpeed * ACCEL_CONSTANT - ACCEL_CONSTANT + 1); 

    #ifdef DEBUG
    Serial.println(curSpeed);
    #endif
    
    newSpeed = curSpeed + delta;
    if (newSpeed > MIN_SPEED)
      curSpeed = MIN_SPEED;
    else if (newSpeed < MAX_SPEED)
      curSpeed = MAX_SPEED;
    else if (whenToStartDelta-- < 0)
      curSpeed = newSpeed;

    #ifdef DEBUG
    Serial.println(whenToStartDelta);
    #endif
      
    scrollDown();
    setLed(0, color);
    draw(); 
  }
  
  for (uint8_t i = 0; i < FADE_LEN; i++) {
    delay(curSpeed * ACCEL_CONSTANT - ACCEL_CONSTANT + 1); 

    #ifdef DEBUG
    Serial.println(curSpeed);
    #endif
    
    newSpeed = curSpeed + delta;
    if (newSpeed > MIN_SPEED)
      curSpeed = MIN_SPEED;
    else if (newSpeed < MAX_SPEED)
      curSpeed = MAX_SPEED;
    else if (whenToStartDelta-- < 0)
      curSpeed = newSpeed;

    #ifdef DEBUG
    Serial.println(whenToStartDelta);
    #endif
      
    scrollDown();
    setLed(0, fade[FADE_LEN - i - 1]);
    draw();
  }
}

void scrollDown() {
	for (uint16_t i = LEDS - 1; i >= 1; i--)
		setLed(i, getLed(i - 1));
	setLed(0, CRGB::Black);
}

/*
void scrollUp() {
	for (uint16_t i = 0; i < LEDS - 1; i++)
		setLed(i, getLed(i + 1));
	setLed(LEDS - 1, CRGB::Black);
}
*/

void slideDown(float curSpeed, float delta) {
  int16_t whenToStartDelta = LEDS - 50 - abs((MIN_SPEED - MAX_SPEED) / delta);
  
  float newSpeed;
	for (uint16_t i = LEDS - 1; i > 0; i--) {
		setLed(0, CRGB::Black);
		scrollDown();
		draw();
		delay(curSpeed * ACCEL_CONSTANT - ACCEL_CONSTANT + 1); 

    #ifdef DEBUG
    Serial.println(curSpeed);
    #endif
    
    newSpeed = curSpeed + delta;

    #ifdef DEBUG
    Serial.println(newSpeed);
    #endif
    
    if (newSpeed > MIN_SPEED)
      curSpeed = MIN_SPEED;
    else if (newSpeed < MAX_SPEED)
      curSpeed = MAX_SPEED;
    else if (whenToStartDelta-- < 0)
      curSpeed = newSpeed;

    #ifdef DEBUG
    Serial.println(whenToStartDelta);
    #endif
	}
}

/*
void slideUp(float curSpeed) {
	for (uint16_t i = 0; i < LEDS - 1; i++) {
		setLed(LEDS - 1, CRGB::Black);
		scrollUp();
		draw();
		delay(curSpeed * ACCEL_CONSTANT - ACCEL_CONSTANT + 1);
	}
}
*/

void wholeStrip(CRGB color) {
	for (uint16_t i = 0; i < LEDS; i++) {
   leds[i] = color;
   temp[i] = color;
	}
}

bool _isBlack = false;

void setLed(uint16_t led, CRGB color) {
  led = LEDS - 1 - led;
  if (_isBlack) {
    temp[led] = color;
  } else {
    leds[led] = color;
  }
}

CRGB getLed(uint16_t led) {
  led = LEDS - 1 - led;
  if (_isBlack) {
    return temp[led];
  } else {
    return leds[led];
  }
}

void draw() {
  bool wasBlack = _isBlack;
  if (random(0, 100) < BLINK_ODDS) {
    _isBlack = true;
    if (!wasBlack) {
      memcpy(temp, leds, LEDS * sizeof(CRGB));
      memcpy(leds, black, LEDS * sizeof(CRGB));
      FastLED.show();
    }
  } else  {
    if (wasBlack) {
      memcpy(leds, temp, LEDS * sizeof(CRGB));
    }
     _isBlack = false;
    FastLED.show();
  }
}


