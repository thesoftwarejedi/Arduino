#include <DMXSerial.h>

/*
 Name:		Elevator.ino
 Created:	8/20/2017 2:31:39 PM
 Author:	danah
*/
#include <FastLED.h>

//led pin
#define PIN1 2

//pin for a simple status LED
#define ERROR_PIN 3

#define LEDS 119

//we start at channel 1
#define DMXSTART 1

//max channels, just use em all
#define DMXLENGTH 512

#define BRIGHTNESS 100

CRGB leds[LEDS];
CRGB black[LEDS];

uint8_t _blinkRate = 0;
uint8_t _speed = 127;
CRGB _color = CRGB::Black;

void setup() {
  //we want to probe - not be interrupted.  interrupts will kill the LED timings
  DMXSerial.init(DMXProbe);
  DMXSerial.maxChannel(DMXLENGTH);
  
  pinMode(PIN1, OUTPUT);
  pinMode(ERROR_PIN, OUTPUT);
  
  FastLED.addLeds<WS2812B, PIN1, GRB>(leds, LEDS).setCorrection(TypicalLEDStrip);
	FastLED.setBrightness(BRIGHTNESS);

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

  wholeStrip(CRGB::Black, true);
  updateLeds();
}

void loop() {

  //1 - on/off
  //2,3,4 - color
  //5 speed 127 = stationary
  //6 mode = stopped on floor, starting, moving, stopping
  //7 flickering rate % 
  //8 floor height
  //9 distance between floors
  
  if (DMXSerial.receive(100)) {
    //status pin off
    digitalWrite(ERROR_PIN, HIGH); 
    
    //get data
    uint8_t *data = DMXSerial.getBuffer();
    
    //if set to off
    if (data[DMXSTART] < 128) {
      wholeStrip(CRGB::Black, true);
      updateLeds();
      delay(250);
      return;
    }

    //we are on
    
    //grab color
    _color = CRGB(data[DMXSTART+1], data[DMXSTART+2], data[DMXSTART+3]);

    //grab blink %
    _blinkRate = data[7];

    //get speed
    _speed = 255 - data[5];
    
    if (data[6] < 64 ) { //set to stable
      wholeStrip(_color, false);
      draw();
      delay(_speed); //
      return;
    } 

    //while doing these, we do not re-read from DMX
    //moving does one lap, then re-reads
    if (data[6] < 128) { //starting
      
    } else if (data[6] < 192) { //moving
      wholeStrip(CRGB::Black, true);
      //for (uint8_t = 
    } else { //stopping
      
    }
  
  } else {
    wholeStrip(CRGB::Black, true);
    updateLeds();
    digitalWrite(ERROR_PIN, LOW);
  } 
}

void scrollDown() {
	for (uint8_t i = LEDS - 1; i >= 1; i--)
		setLed(i, getLed(i - 1));
	setLed(0, CRGB::Black);
}

void scrollUp() {
	for (uint16_t i = 0; i < LEDS - 1; i++)
		setLed(i, getLed(i + 1));
	setLed(LEDS - 1, CRGB::Black);
}

void wholeStrip(CRGB color, bool overrideBlink) {
	for (uint8_t i = 0; i < LEDS; i++) {
      setLed(i, color);
	}
}

bool _isBlack = false;

void setLed(uint8_t led, CRGB color) {
    leds[led] = color;
}

CRGB getLed(uint8_t led) {
    return leds[led];
}

void draw() {
  if (random(0, 255) < _blinkRate) {
    FastLED.addLeds<WS2812B, PIN1, GRB>(black, LEDS).setCorrection(TypicalLEDStrip);
  } else {
    FastLED.addLeds<WS2812B, PIN1, GRB>(leds, LEDS).setCorrection(TypicalLEDStrip);
  }
  updateLeds();
}

void updateLeds() {
  FastLED.show();
}


