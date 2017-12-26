/*
Name:    ChristmasLED.ino
Created: 12/9/2017 2:31:39 PM
Author:  danah
*/
#include <FastLED.h>

#define PIN 3
#define LEDS 100
#define BRIGHTNESS 80


CRGBPalette16 currentPalette;
TBlendType    currentBlending;

extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;


CRGB leds[LEDS];
String out = "";
bool normal = false;
bool blank = false;
bool defaultOn = true; 
bool palette = false;
bool flickerOn = false;
extern const TProgmemPalette16 christmasLightPalette PROGMEM;

void setup() {
  delay( 3000 ); // power-up safety delay
  pinMode(PIN, OUTPUT);
  pinMode(0, INPUT); //serial
  FastLED.addLeds<WS2811, PIN, RGB>(leds, LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  
  currentPalette = RainbowColors_p;
  currentBlending = LINEARBLEND;
    
  Serial.begin(115200);
}

void loop() {
  if (out.length() > 0) {
    String ctx = out;
    out = "";
    Serial.write(ctx.length());
    Serial.write(ctx.charAt(0));
    if (ctx == "on") {
      defaultOn = true;
      normal = false;
      palette = false;
      flickerOn = false;
      return;
    } else if (ctx == "off") {
      defaultOn = false;
      blank = false;
      palette = false;
      flickerOn = false;
      return;
    } else if (ctx == "f") {
      defaultOn = false;
      blank = false;
      palette = false;
      flickerOn = true;
      return;
    } else if (ctx == "0" || ctx == "1" || ctx == "2" || ctx == "3" || ctx == "4" || ctx == "5" || ctx == "6" || ctx == "7" || ctx == "8" || ctx == "9" || ctx == "r") {
      setPalette(ctx[0]);
      palette = true;
      flickerOn = false;
      return;
    }
    flicker();
    blank = false;
    allOff();
    FastLED.show();
    delay(random(2500, 4000));
    uint8_t c;
    for (uint8_t i = 0; i < ctx.length(); i++) {
      c = ctx.charAt(i);
      if (c >= 'a' && c <= 'z') {
        Serial.write(c);
        c = getLightForLetter(c) - 1; //I was one off on the numbering
        light(c, true);
        FastLED.show();
        delay(random(600, 1100));
        leds[c] = CRGB::Black;
        FastLED.show();
        delay(random(200, 500));
      }
      else {
        Serial.write('X');
        FastLED.show();
        delay(random(1000, 2200));
      }
    }
    delay(random(1000, 3000));
    flicker();
    normal = false;
    blank = false;
  }

  //this is what we do if no text
    
  static uint8_t startIndex = 0;
  startIndex = startIndex + 1; /* motion speed */
  
  if (palette) {
    FillLEDsFromPaletteColors( startIndex);
    FastLED.show();
    delay(10);
    return;
  } else if (defaultOn) {
    normalColors(true);
  } else if (flickerOn) {
    flicker();
  } else {
    allOff();
  }
  delay(1000);
}

uint8_t getLightForLetter(uint8_t c) {
  switch(c) {
    case 'a':
      return 24;
      break;
    case 'b':
      return 27;
      break;
    case 'c':
      return 30;
      break;
    case 'd':
      return 34;
      break;
    case 'e':
      return 38;
      break;
    case 'f':
      return 42;
      break;
    case 'g':
      return 45;
      break;
    case 'h':
      return 48;
      break;
    case 'q':
      return 52;
      break;
    case 'p':
      return 55;
      break;
    case 'o':
      return 57;
      break;
    case 'n':
      return 60;
      break;
    case 'm':
      return 62;
      break;
    case 'l':
      return 65;
      break;
    case 'k':
      return 68;
      break;
    case 'j':
      return 71;
      break;
    case 'i':
      return 74;
      break;
    case 'r':
      return 79;
      break;
    case 's':
      return 82;
      break;
    case 't':
      return 85;
      break;
    case 'u':
      return 88;
      break;
    case 'v':
      return 90;
      break;
    case 'w':
      return 92;
      break;
    case 'x':
      return 95;
      break;
    case 'y':
      return 98;
      break;
    case 'z':
      return 100;
      break;   
  }
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
    delay(random(100));
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

void setPalette(char c) {
  if(c == 'r')  { currentPalette = RainbowColors_p;         currentBlending = LINEARBLEND; }
  if(c == '1')  { currentPalette = RainbowStripeColors_p;   currentBlending = NOBLEND;  }
  if(c == '2')  { currentPalette = RainbowStripeColors_p;   currentBlending = LINEARBLEND; }
  if(c == '3')  { SetupPurpleAndGreenPalette();             currentBlending = LINEARBLEND; }
  if(c == '4')  { SetupTotallyRandomPalette();              currentBlending = LINEARBLEND; }
  if(c == '5')  { SetupBlackAndWhiteStripedPalette();       currentBlending = NOBLEND; }
  if(c == '6')  { SetupBlackAndWhiteStripedPalette();       currentBlending = LINEARBLEND; }
  if(c == '7')  { currentPalette = CloudColors_p;           currentBlending = LINEARBLEND; }
  if(c == '8')  { currentPalette = PartyColors_p;           currentBlending = LINEARBLEND; }
  if(c == '9')  { currentPalette = myRedWhiteBluePalette_p; currentBlending = NOBLEND;  }
  if(c == '0')  { currentPalette = myRedWhiteBluePalette_p; currentBlending = LINEARBLEND; }
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
      light(i, false);
    }
    if (showThem)
      FastLED.show();
  }
}

void light(uint8_t i, bool bright) {
  //NOBLEND
  //LINEARBLEND
  leds[i] = ColorFromPalette(christmasLightPalette, i * 16, bright ? 255 : 
  BRIGHTNESS, NOBLEND);
}

void serialEvent() {
  delay(1000);
  do {
    while (Serial.available() > 0) {
      uint8_t in = Serial.read();
      out = out + (char)in;
      delay(10);
      Serial.write(0);
    }
    delay(2000);
  } while (Serial.available() > 0);
}

void FillLEDsFromPaletteColors( uint8_t colorIndex)
{
    uint8_t brightness = 255;
    
    for( int i = 0; i < LEDS; i++) {
        leds[i] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
        colorIndex += 3;
    }
}

// This function fills the palette with totally random colors.
void SetupTotallyRandomPalette()
{
    for( int i = 0; i < 16; i++) {
        currentPalette[i] = CHSV( random8(), 255, random8());
    }
}

// This function sets up a palette of black and white stripes,
// using code.  Since the palette is effectively an array of
// sixteen CRGB colors, the various fill_* functions can be used
// to set them up.
void SetupBlackAndWhiteStripedPalette()
{
    // 'black out' all 16 palette entries...
    fill_solid( currentPalette, 16, CRGB::Black);
    // and set every fourth one to white.
    currentPalette[0] = CRGB::White;
    currentPalette[4] = CRGB::White;
    currentPalette[8] = CRGB::White;
    currentPalette[12] = CRGB::White;
    
}

// This function sets up a palette of purple and green stripes.
void SetupPurpleAndGreenPalette()
{
    CRGB purple = CHSV( HUE_PURPLE, 255, 255);
    CRGB green  = CHSV( HUE_GREEN, 255, 255);
    CRGB black  = CRGB::Black;
    
    currentPalette = CRGBPalette16(
                                   green,  green,  black,  black,
                                   purple, purple, black,  black,
                                   green,  green,  black,  black,
                                   purple, purple, black,  black );
}


// This example shows how to set up a static color palette
// which is stored in PROGMEM (flash), which is almost always more
// plentiful than RAM.  A static PROGMEM palette like this
// takes up 64 bytes of flash.
const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM =
{
    CRGB::Red,
    CRGB::Gray, // 'white' is too bright compared to red and blue
    CRGB::Blue,
    CRGB::Black,
    
    CRGB::Red,
    CRGB::Gray,
    CRGB::Blue,
    CRGB::Black,
    
    CRGB::Red,
    CRGB::Red,
    CRGB::Gray,
    CRGB::Gray,
    CRGB::Blue,
    CRGB::Blue,
    CRGB::Black,
    CRGB::Black
};

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




