#include "NeoPatterns.h"

#define LED_ON_PIN 3
#define NEO_PIN 4
#define NEO_COUNT 6
#define BRIGHTNESS 120
void Strip1Complete();
NeoPatterns Strip1(NEO_COUNT, NEO_PIN, NEO_RGB + NEO_KHZ800, &Strip1Complete);

byte rnd = 0;
//int8_t blowOutAfterMins = 15;
//long startMillis;

void setup() {
	// initialize pins
	pinMode(NEO_PIN, OUTPUT);
	pinMode(LED_ON_PIN, OUTPUT);

	// Initialize NeoPixel Strip
	Strip1.setBrightness(BRIGHTNESS);
	Strip1.begin();
	Strip1.show();

	Strip1.Color1 = Strip1.Color(62, 255, 4, 100);
	lightCandle();
}

bool lit = true;

void loop() {
	/*
	if (lit && ((millis() - startMillis) / 1000 / 60 > blowOutAfterMins)) {
		blowOutCandle();
	}
	*/

	if (!lit) {
		delay(500);
	}
	else {
		Strip1.Update();
		//delay(10);
	}
}

void Strip1Complete() {
	Strip1.Reverse();

	if (Strip1.Direction == REVERSE) {
		Strip1.Interval = random(5, 22);
	}
	else {
		rnd = random(random(3, 7), random(20, 55));

		Strip1.Pixel = random(0, Strip1.numPixels()); // pick a random Pixel
		Strip1.Interval = 1;
		Strip1.Color2 = Strip1.Color(0, rnd, 0);
	}
}

void lightCandle() {
	digitalWrite(LED_ON_PIN, HIGH);
	//startMillis = millis();
	Strip1.Flicker(Strip1.Color1, Strip1.Color2, 20, 5);
	lit = true;
}

void blowOutCandle() {
	cWipe(0, 10);
	Strip1.ActivePattern = NONE;
	digitalWrite(LED_ON_PIN, LOW);
	lit = false;
}

void cWipe(uint32_t c, uint8_t wait) {
	for (uint16_t i = 0; i < Strip1.numPixels(); i++) {
		Strip1.setPixelColor(i, c);
		Strip1.show();
		delay(wait);
	}
}