/*
  WiFiMQTTManager.h - Library for the ESP8266/ESP32 Arduino platform
  for configuration of WiFi and MQTT credentials using a AP mode and Captive Portal
  Written by David Reed hashmark47@gmail.com
  GNU license  
*/
#ifndef WiFiMQTTManager_h
#define WiFiMQTTManager_h

//#define MQTT_MAX_PACKET_SIZE 512

#include <ArduinoJson.h> // https://github.com/bblanchon/ArduinoJson
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "Arduino.h"
#include "FS.h"
#include "EL_utility.h"

#ifdef ESP32
  #include <SPIFFS.h>
#endif

void _settingsAP();

class WiFiMQTTManager {
  public:
    WiFiMQTTManager();
    //WiFiManager wm;
    void setup(String sketchName);
    void loop();
    void setDebugOutput(bool b);
    std::shared_ptr<PubSubClient> client;
    char clientId[40];
    char deviceId[40];
    char chipId[40];
    void (*subscribeTo)();
    void (*subscriptionCallback)(char* topicIn, byte* message, unsigned int length);
    bool formatFS;
    long lastMsg;
    char deviceType[40];
    char _friendly_name[40];
    void reset();
  private:
    void _setupSpiffs();
    void _reconnect();
    char _mqtt_server[40];
    char _mqtt_port[6];
    char _mqtt_username[40];
    char _mqtt_password[40];
    String _sketchName;
    int _LED_BUILTIN;
    long _lastMsg;
    char _msg[50];
    int _value;
    bool _shouldSaveConfig;  
    WiFiClient _espClient;

};

#endif

