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

#define NUM_LEDS 136
#define DATA_PIN 4
#define BUTTON_PIN 11

CRGB leds[NUM_LEDS];
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 100;    // the debounce time; increase if the output flickers
int ledState = LOW;         // the current state of the output pin
int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin

uint8_t palIndex = 0;
CRGBPalette16 currentPalette;
TBlendType    currentBlending;

uint16_t updatesPerSecond = 1000;

extern const TProgmemPalette16 myPalette1_p PROGMEM;
extern const TProgmemPalette16 myPalette2_p PROGMEM;
extern const TProgmemPalette16 myPalette3_p PROGMEM;
extern const TProgmemPalette16 myPalette4_p PROGMEM;

void setup() {
  //for debugging perhaps
  Serial.begin(115200);

  pinMode(DATA_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);
  
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(128);
  wholeStrip(CRGB::Black);
  FastLED.show();

  currentPalette = myPalette1_p;
  currentBlending = NOBLEND;
}

void loop() {
  static uint8_t startIndex = 0;
  startIndex = startIndex + 1; /* motion speed */
  FillLEDsFromPaletteColors( startIndex);
  FastLED.show();
  FastLED.delay(1000 / updatesPerSecond);
  
  //check button
  int reading = digitalRead(BUTTON_PIN);
  
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }
  
  if ((millis() - lastDebounceTime) > debounceDelay && reading != buttonState) {
    buttonState = reading;    
    if (buttonState == HIGH) {
      palIndex++;
      if (palIndex > 3)
        palIndex = 0;
        
      if (palIndex == 0) {
        currentPalette = myPalette1_p;
        updatesPerSecond = 1000;
        currentBlending = NOBLEND;
      }
      else if (palIndex == 1) {
        currentPalette = myPalette2_p;
        updatesPerSecond = 1000;
        currentBlending = NOBLEND;
      }
      else if (palIndex == 2) {
        currentPalette = myPalette3_p;
        updatesPerSecond = 100;
        currentBlending = LINEARBLEND;
      }
      else if (palIndex == 3) {
        currentPalette = myPalette4_p;
        updatesPerSecond = 1000;
        currentBlending = NOBLEND;
      }
    } 
  }
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
  for (int i = NUM_LEDS / 2; i >= 0; i--) {
    leds[i] = c1;
    FastLED.show();
  }
  for (int i = NUM_LEDS / 2; i < NUM_LEDS; i++) {
    leds[i] = c2;
    FastLED.show();
  }
  delay(1000);
  //switch colors and flash
  for (int j = 0; j < 20; j++) {
    for (int i = 0; i < NUM_LEDS; i++)
      if (leds[i] == (CRGB)c1)
        leds[i] = c2;
      else
        leds[i] = c1;
    FastLED.show();
    delay(speed);
  }
  //set alternating
  for (int i = 0; i < NUM_LEDS; i++) {
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
    for (int i = 0; i < NUM_LEDS; i++)
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
  int j = NUM_LEDS/2;
  int k = 0;
  int l = NUM_LEDS - 1;
  for (int i = NUM_LEDS/2; i > 80; i--) {
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
    for (int i = 0; i < NUM_LEDS; i++)
      if (leds[i] == (CRGB)c1)
        leds[i] = c2;
      else
        leds[i] = c1;
    FastLED.show();
    delay(speed * 2);
  }
  slideUp(5);
}


void FillLEDsFromPaletteColors( uint8_t colorIndex)
{
    uint8_t brightness = 255;
    
    for( int i = 0; i < NUM_LEDS; i++) {
        leds[i] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
        colorIndex += 3;
    }
}

const TProgmemPalette16 myPalette1_p PROGMEM =
{
    CRGB::Blue,
    CRGB::Blue,
    CRGB::Blue,
    CRGB::Blue,
    
    CRGB::Red,
    CRGB::Red,
    CRGB::Red,
    CRGB::Red,
    
    CRGB::Green,
    CRGB::Green,
    CRGB::Green,
    CRGB::Green,
    
    CRGB::Yellow,
    CRGB::Yellow,
    CRGB::Yellow,
    CRGB::Yellow
};

const TProgmemPalette16 myPalette2_p PROGMEM =
{
    CRGB::Blue,
    CRGB::Blue,
    CRGB::Black,
    CRGB::Black,
    
    CRGB::Red,
    CRGB::Red,
    CRGB::Black,
    CRGB::Black,
    
    CRGB::Green,
    CRGB::Green,
    CRGB::Black,
    CRGB::Black,
    
    CRGB::Yellow,
    CRGB::Yellow,
    CRGB::Black,
    CRGB::Black
};

const TProgmemPalette16 myPalette3_p PROGMEM =
{
    CRGB::Blue,
    CRGB::Black,
    CRGB::Black,
    CRGB::Black,
    
    CRGB::Red,
    CRGB::Black,
    CRGB::Black,
    CRGB::Black,
    
    CRGB::Green,
    CRGB::Black,
    CRGB::Black,
    CRGB::Black,
    
    CRGB::Yellow,
    CRGB::Black,
    CRGB::Black,
    CRGB::Black
};

const TProgmemPalette16 myPalette4_p PROGMEM =
{
    CRGB::Blue,
    CRGB::Red,
    CRGB::Green,
    CRGB::Yellow,
    
    CRGB::Blue,
    CRGB::Red,
    CRGB::Green,
    CRGB::Yellow,
    
    CRGB::Blue,
    CRGB::Red,
    CRGB::Green,
    CRGB::Yellow,
    
    CRGB::Blue,
    CRGB::Red,
    CRGB::Green,
    CRGB::Yellow
};
