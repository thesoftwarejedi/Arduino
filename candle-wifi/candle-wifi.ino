#include "EL_WiFiManager.h"
#include "EL_WiFiMQTTManager.h"
#include "NeoPatterns.h"
#include <DoubleResetDetector.h>
#include <ArduinoOTA.h>

#define NEO_PIN 5
#define NEO_COUNT 6
void Strip1Complete();
NeoPatterns Strip1(NEO_COUNT, NEO_PIN, NEO_RGB + NEO_KHZ800, &Strip1Complete);

// Number of seconds after reset during which a 
// subseqent reset will be considered a double reset.
#define DRD_TIMEOUT 2
// RTC Memory Address for the DoubleResetDetector to use
#define DRD_ADDRESS 0
DoubleResetDetector drd(DRD_TIMEOUT, DRD_ADDRESS);

/**************************** FOR OTA **************************************************/
#define OTApassword "15632ftl172" //the password you will need to enter to upload remotely via the ArduinoIDE
int OTAport = 8266;

/****************************************FOR JSON***************************************/
const int BUFFER_SIZE = JSON_OBJECT_SIZE(10);
#define MQTT_MAX_PACKET_SIZE 512
#define MILLION 1000000

WiFiMQTTManager wmm; 

byte rnd = 0;
int8_t blowOutAfterMins = 15;
int brightness = 110;
long startMillis;

void setup(){
  Serial.begin(115200);
  Serial.println("\n Starting");
  
  // initialize pins
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(NEO_PIN, OUTPUT);

  //LED off
  digitalWrite(LED_BUILTIN, HIGH); 

  // Initialize NeoPixel Strip
  Strip1.setBrightness(brightness);
  Strip1.begin();
  Strip1.show();

  if (drd.detectDoubleReset())
    wmm.reset();
  
  wmm.subscribeTo = &subscribeTo;
  wmm.subscriptionCallback = &subscriptionCallback;
  wmm.setup("EnderLab-Candle");
  
  drd.loop();

  //OTA SETUP
  ArduinoOTA.setPort(OTAport);
  ArduinoOTA.setHostname(wmm.clientId);
  ArduinoOTA.setPassword((const char *)OTApassword);
  Serial.print("OTA hostname ");
  Serial.println(wmm.clientId);
  ArduinoOTA.onStart([]() {
    Serial.println("Starting");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();

  //LED off
  digitalWrite(LED_BUILTIN, HIGH); 
  
  Strip1.Color1 = Strip1.Color(62, 255, 4, 100);
  lightCandle();
  sendState();
}

void subscribeTo() {
  Serial.println("subscribing to some topics...");  
  char topic[100];
  snprintf(topic, sizeof(topic), "%s%s%s", "enderlab/candle/", wmm._friendly_name, "/set");
  wmm.client->subscribe(topic);
  Serial.print("subbed ");
  Serial.println(topic);
}

bool lit = true;

void loop(){
  if (lit && ((millis() - startMillis) / 1000 / 60 > blowOutAfterMins)) {
    blowOutCandle();
    sendState();
  }
  
  if (!lit) {
    delay(500);
  } else {
    Strip1.Update();
  }

  wmm.loop();
  
  drd.loop();
  
  ArduinoOTA.handle();
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
  startMillis = millis();
  Strip1.Flicker(Strip1.Color1, Strip1.Color2, 20, 5);
  lit = true;
}

void blowOutCandle(){
  cWipe(0, 10);
  Strip1.ActivePattern = NONE;
  lit = false;
}

void cWipe(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < Strip1.numPixels(); i++) {
    Strip1.setPixelColor(i, c);
    Strip1.show();
    delay(wait);
  }
}

void subscriptionCallback(char* topic, byte* message, unsigned int length) {
  //not needed, just logging
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  //this needed
  StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(message);
  if (!root.success()) {
    Serial.println("parseObject() failed");
    return;
  }

  if (root.containsKey("state")) {
    
    if (strcmp(root["state"], "ON") == 0) {
      lightCandle();
    }
    else if (strcmp(root["state"], "OFF") == 0) {
      blowOutCandle();
    }
    
    if (root.containsKey("brightness")) {
      Serial.print("rcvd brightness ");
      brightness = root["brightness"];
      Serial.println(brightness);
      Strip1.setBrightness(brightness);
    }
  }
  
  sendState();
}

void sendState() {
  Serial.println("Sending state");
  char topic[100];
  snprintf(topic, sizeof(topic), "%s%s", "enderlab/candle/", wmm._friendly_name);
  
  char message[200];
  if (lit) {
    snprintf(message, sizeof(message), "%s%i%s", "{\"state\":\"ON\",\"brightness\":",brightness,"}");
  } else {
    snprintf(message, sizeof(message), "%s%i%s", "{\"state\":\"OFF\",\"brightness\":",brightness,"}");
  }
  Serial.print("State ");
  Serial.println(message);
  wmm.client->publish(topic, message, true);
  Serial.print("Sent state to ");
  Serial.println(topic);
}

char* toChar(String string) {
  char buffer[string.length() + 1];
  string.toCharArray(buffer, string.length() + 1);
  return buffer;
}

