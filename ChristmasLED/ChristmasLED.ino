/*
 Name:		ChristmasLED.ino
 Created:	12/9/2017 2:31:39 PM
 Author:	danah
*/
#include <FastLED.h>

#define MOTION_PIN 5
#define MOTION_PIN2 6
#define PIN 7
//#define LEDS 443
#define LEDS 300
#define BRIGHTNESS 100
#define KINDA_WHITE 50

CRGB leds[LEDS];
CRGB kindaWhite = CRGB(KINDA_WHITE, KINDA_WHITE, KINDA_WHITE);

void setup() {
	FastLED.addLeds<WS2812B, PIN, GRB>(leds, LEDS).setCorrection(TypicalLEDStrip);
	FastLED.setBrightness(BRIGHTNESS);
	wholeStrip(CRGB::Black);
}

void loop() {
	splitHalves(150);
	delay(5000);
	twinkle(30, 60, 1);
	twinkle(30, 50, 4);
	twinkle(30, 40, 8);
	twinkle(30, 30, 20);
	christmas(20, 25);
	delay(5000);
	twinkle(30, 60, 1);
	twinkle(30, 50, 4);
	twinkle(30, 40, 8);
	twinkle(30, 30, 20);
	candyCane(20, 25);
	delay(5000);
	twinkle(30, 60, 1);
	twinkle(30, 50, 4);
	twinkle(30, 40, 8);
	twinkle(30, 30, 20);
}

void splitHalves(uint8_t speed) {
	//split the halves
	for (int i = LEDS / 2; i >= 0; i--) {
		leds[i] = CRGB::Red;
		FastLED.show();
	}
	for (int i = LEDS / 2; i < LEDS; i++) {
		leds[i] = CRGB::Green;
		FastLED.show();
	}
	delay(1000);
	//switch colors and flash
	for (int j = 0; j < 20; j++) {
		for (int i = 0; i < LEDS; i++)
			if (leds[i] == (CRGB)CRGB::Red)
				leds[i] = CRGB::Green;
			else
				leds[i] = CRGB::Red;
		FastLED.show();
		delay(speed);
	}
	//set alternating
	for (int i = 0; i < LEDS; i++) {
		if (i % 4 == 0)
			leds[i] = CRGB::Green;
		else if (i % 4 == 2)
			leds[i] = CRGB::Red;
		else
			leds[i] = CRGB(0,0,0);
	}
	FastLED.show();
	//switch colors and flash
	for (int j = 0; j < 20; j++) {
		for (int i = 0; i < LEDS; i++)
			if (leds[i] == (CRGB)CRGB::Red)
				leds[i] = CRGB::Green;
			else
				leds[i] = CRGB::Red;
		FastLED.show();
		delay(speed * 2);
	}
	wholeStrip(CRGB::White);
	delay(1000);
	//4 lines over white slow draw
	int j = LEDS/2;
	int k = 0;
	int l = LEDS - 1;
	for (int i = LEDS/2; i > 80; i--) {
		j++;
		k++;
		l--;
		leds[i] = CRGB::Red;
		leds[j] = CRGB::Green;
		leds[k] = CRGB::Green;
		leds[l] = CRGB::Red;
		FastLED.show();
		delay(speed);
	}
	delay(1000);
	//switch colors and flash
	for (int j = 0; j < 20; j++) {
		for (int i = 0; i < LEDS; i++)
			if (leds[i] == (CRGB)CRGB::Red)
				leds[i] = CRGB::Green;
			else
				leds[i] = CRGB::Red;
		FastLED.show();
		delay(speed * 2);
	}
	slideUp(5);
}

void christmas(uint16_t loop, uint8_t speed) {
	for (uint16_t l = 0; l < loop; l++) {
		for (uint8_t m = 0; m < 20; m++) {
			scrollDown();
			if (m < 9)
				leds[0] = CRGB::Red;
			else if (m >= 10 && m < 19)
				leds[0] = CRGB::Green;
			else
				leds[0] = CRGB::Black;
			FastLED.show();
			delay(speed);
		}
	}
	slideDown(speed);
}

void candyCane(uint16_t loop, uint8_t speed) {
	for (uint16_t l = 0; l < loop; l++) {
		for (uint8_t m = 0; m < 20; m++) {
			scrollUp();
			if (m < 9)
				leds[LEDS-1] = CRGB::Red;
			else if (m >= 10 && m < 19)
				leds[LEDS - 1] = CRGB::White;
			else
				leds[LEDS - 1] = CRGB::Black;
			FastLED.show();
			delay(speed);
		}
	}
	slideUp(speed);
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
		leds[LEDS-1] = CRGB::Black;
		scrollUp();
		FastLED.show();
		delay(speed);
	}
}

void wholeStrip(CRGB color) {
	for (uint16_t i = 0; i < LEDS; i++)
		leds[i] = color;
	FastLED.show();
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

uint16_t sparkleCount = LEDS / 15;

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

uint8_t backStop = LEDS / 4;
void rainbowBall(uint8_t s, uint8_t v, uint8_t fade, bool forward) {
	static uint8_t hue = 0;
	for (int i = 0; i < LEDS + backStop; i++) {
		if (i >= 0 && i < LEDS)
			setLed(LEDS - 1 - i, CHSV(hue++, s, v), forward);
		fadeall(fade, i - backStop, forward);
		SnowSparkle(sparkleCount, 0, 0, i - backStop, 200, forward);
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

void SnowSparkle(uint8_t sparkleCount, uint8_t SparkleDelay, uint8_t SpeedDelay, int limit, uint8_t brightness, bool forward) {
	if (brightness > 255)
		brightness = 255;
	int sparkles[sparkleCount];
	CRGB oldColor[sparkleCount];
	for (int i = 0; i < sparkleCount; i++) {
		sparkles[i] = random(LEDS);
		if (sparkles[i] < limit) {
			oldColor[i] = getLed(sparkles[i], forward);
			setLed(LEDS - sparkles[i], CRGB(CHSV(random(255), 150, brightness)), forward);
		}
	}
	FastLED.show();
	delay(SparkleDelay);
	for (int i = 0; i < sparkleCount; i++) {
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


