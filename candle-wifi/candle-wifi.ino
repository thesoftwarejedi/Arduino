#include "EL_WiFiManager.h"
#include "EL_WiFiMQTTManager.h"
#include "NeoPatterns.h"
#include <DoubleResetDetector.h>

#define NEO_PIN 5
#define NEO_COUNT 6
void Strip1Complete();
NeoPatterns Strip1(NEO_COUNT, NEO_PIN, NEO_RGB + NEO_KHZ800, &Strip1Complete);

// Number of seconds after reset during which a 
// subseqent reset will be considered a double reset.
#define DRD_TIMEOUT 10
// RTC Memory Address for the DoubleResetDetector to use
#define DRD_ADDRESS 0
DoubleResetDetector drd(DRD_TIMEOUT, DRD_ADDRESS);

byte rnd = 0;
int8_t blowOutAfterMins = 1;
long startMillis;

void configModeCallback (WiFiManager *myWiFiManager) {
  WiFi.softAPIP();
  //LED on
  digitalWrite(LED_BUILTIN, LOW); 
}

void setup(){
  Serial.begin(115200);
  Serial.println("\n Starting");
  
  // initialize pins
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(NEO_PIN, OUTPUT);

  //LED off
  digitalWrite(LED_BUILTIN, HIGH); 

  // Initialize NeoPixel Strip 1
  Strip1.setBrightness(110);
  Strip1.begin();
  Strip1.show();
  
  WiFiManager wifiManager;  
  
  if (drd.detectDoubleReset())
    wifiManager.resetSettings();
  
  WiFiManagerParameter custom_mqtt_server("server", "mqtt server", mqtt_server, 50);
  WiFiManagerParameter custom_mqtt_port("port", "mqtt port", mqtt_port, 6);
  
  wifiManager.setAPCallback(configModeCallback);
  if(!wifiManager.autoConnect()) {
    ESP.reset();
    delay(1000);
  }

  //LED off
  digitalWrite(LED_BUILTIN, HIGH); 
  
  Strip1.Color1 = Strip1.Color(62, 255, 4, 100);
  blowOutCandle();
  lightCandle();
  startMillis = millis();
}

void loop(){
  if ((millis() - startMillis) / 1000 / 60 > blowOutAfterMins)
    blowOutCandle();
  
  Strip1.Update();
  
  drd.loop();
}

void Strip1Complete(){
    Strip1.Reverse();

    if (Strip1.Direction == REVERSE){ 
        Strip1.Interval = random(5,22);
    }
    else{
        rnd = random(random(3, 7),random(20, 55)); 

        Strip1.Pixel = random(0,Strip1.numPixels()); // pick a random Pixel
        Strip1.Interval = 1; 
        Strip1.Color2 = Strip1.Color(0, rnd, 0); 
    }
}

void lightCandle(){
    Strip1.Flicker(Strip1.Color1, Strip1.Color2, 20, 5);
}

void blowOutCandle(){
  cWipe(0, 10);
  Strip1.ActivePattern = NONE;
}

void cWipe(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < Strip1.numPixels(); i++) {
    Strip1.setPixelColor(i, c);
    Strip1.show();
    delay(wait);
  }
}

