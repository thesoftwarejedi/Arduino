#include <QueueList.h>

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

/*
 * TIMING
 */
//seconds until completely on
#define SECONDS_UNTIL_ON 15
//seconds to remain completely on
#define SECONDS_PAUSE_ON 30
//a percent of randomness to add to the wait between eyes turning on
#define RANDOM_PCT 70
//how often an eye blinks
#define BLINK_EVERY_MILLIS 10000
//how long a blink lasts
#define BLINK_LENGTH 100

/*
 * COLOR MIN AND MAX
 */
#define MAX_RED 25
#define MIN_RED 10
#define MAX_GREEN 0
#define MIN_GREEN 0
#define MAX_BLUE 0
#define MIN_BLUE 0

//the number of eyes in the line
#define NUM_EYES 8*8

/*
 * don't change these
 */

//there are two leds per eye
#define NUM_LEDS NUM_EYES * 2

//pin for LED data
#define DATA_PIN A3
#define TRIGGER_PIN A2
#define TRIGGER_G_PIN A1

//our LEDs
CRGB leds[NUM_LEDS];

uint8_t eyeOrder[NUM_EYES];

struct Blink {
  unsigned long endTime;
  uint8_t eye;
  CRGB color;
};

QueueList<Blink> queue;

//frequency to check for blink (lower is higher resolution)
#define BLINK_REFRESH_RATE 10

//record start time
unsigned long start = millis();

void setup () {  
  //for debugging perhaps
  Serial.begin(115200);
  
  pinMode(DATA_PIN, OUTPUT);
  pinMode(TRIGGER_PIN, INPUT_PULLUP);
  pinMode(TRIGGER_G_PIN, OUTPUT);

  digitalWrite(TRIGGER_G_PIN, LOW);

  //add LEDs to FastLED library
  //strip style
  FastLED.addLeds<WS2811, DATA_PIN, GRB>(leds, NUM_LEDS);
  //cmas style
  //FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, NUM_LEDS);
  
  off();
  updateLeds();
  
  //init to the index
  for (uint8_t i = 0; i < NUM_EYES; i++) {
    eyeOrder[i] = i;
  }

  randomSeed(analogRead(A0));
}

bool blinkEnabled = false;

void loop() {  
  //shuffle
  for (uint8_t i = 0; i < NUM_EYES; i++) {
    int n = random(0, NUM_EYES);
    uint8_t temp = eyeOrder[n];
    eyeOrder[n] = eyeOrder[i];
    eyeOrder[i] = temp;
  }

  //computer the time to wait between lights (randomized later)
  float wait = (float)1 / (float)(NUM_EYES / (float)SECONDS_UNTIL_ON) * (float)1000;
  
  Serial.println(wait);
  
  CRGB color;

  //now go
  blinkEnabled = true;
  for (uint8_t c = 0; c < NUM_EYES; c++) {
    uint8_t c1 = random(MIN_RED, MAX_RED);
    uint8_t c2 = random(MIN_GREEN, MAX_GREEN);
    uint8_t c3 = random(MIN_BLUE, MAX_BLUE);
    color = CRGB(c1, c2, c3);
    showEye(eyeOrder[c], color);
    updateLeds();
    float curWait = random(wait - (wait * (float)((float)RANDOM_PCT / 100)), wait + (wait * (float)((float)RANDOM_PCT / 100)));

    delayWithUpdate(curWait);
  }

  delayWithUpdate(SECONDS_PAUSE_ON * 1000);

  blinkEnabled = false;
  queue = QueueList<Blink>();
  
  off();
  updateLeds();

  while (digitalRead(TRIGGER_PIN) == HIGH) {
    delay(200);
    //make sure it stays off, was possibly unplugged briefly
    updateLeds();
  }
}

void off() {
  for (uint8_t i = 0; i < NUM_EYES; i++) {
    showEye(i, CRGB::Black);
  }
}

unsigned long nextBlink = 0;

void updateLeds() {
  if (blinkEnabled && millis() > nextBlink) {
    
    //check for a light to blink
    for (uint8_t i = 0; i < NUM_EYES; i++) {
      //if the eye is on
      if (leds[i*2].r > 0 || leds[i*2].g > 0 || leds[i*2].b > 0) {
        //if the odds are to blink
        if (random(0, BLINK_EVERY_MILLIS / BLINK_REFRESH_RATE) == 0) {
          //add the eye to our queue to turn back on
          queue.push({millis() + BLINK_LENGTH, i, leds[i*2]});
          //black the eye
          showEye(i, CRGB::Black);
        }
      }
    }

    //check for a light to unblink
    while (!queue.isEmpty() && millis() > queue.peek().endTime) {
      //grab the info and set the eye back to the color it was
      Blink b = queue.pop();
      showEye(b.eye, b.color);
    }
    
    //set our time to check blink next
    nextBlink = millis() + BLINK_REFRESH_RATE;
  }
  FastLED.show();
}

void showEye(uint8_t i, CRGB c) {
  uint16_t li = i * 2;
  showLed(li, c);
  showLed(li + 1, c);
}

void delayWithUpdate(unsigned long ms) {
  
  Serial.println(ms);
  
  unsigned long pauseEnd = millis() + ms;
  while (millis() < pauseEnd) {
    delay(10);
    updateLeds();
  }
}

void showLed(uint16_t i, CRGB c) {
  leds[i] = c;
}

