/*
  WiFiMQTTManager.cpp - Library for the ESP8266/ESP32 Arduino platform
  for configuration of WiFi and MQTT credentials using a Captive Portal
  Written by David Reed hashmark47@gmail.com
  GNU license  
*/
#include "EL_WiFiMQTTManager.h"
#include "EL_WiFiManager.h"

WiFiManager *wm;

WiFiMQTTManager::WiFiMQTTManager() {
  wm = new WiFiManager;
  
  lastMsg = 0;
  formatFS = false;
  strcpy(_mqtt_server, "YOURMQTTADDRESS");
  strcpy(_mqtt_port, "1883");
  strcpy(_mqtt_username, "YOURMQTTUSERNAME");
  strcpy(_mqtt_password, "YOURMQTTPASSWORD");
  _LED_BUILTIN = D1;
  _lastMsg = 0;
  _value = 0;
  _shouldSaveConfig = false;

  Serial.begin(115200);
  wm->setDebugOutput(false);
  #ifdef ESP32 
    strcpy(deviceType, "ESP32");
  #elif defined(ESP8266) 
    strcpy(deviceType, "ESP8266");
  #else 
    strcpy(deviceType, "UNKNOWN");
  #endif

}

void WiFiMQTTManager::loop() {
  if (!client->connected()) {
    _reconnect();
  }  
  client->loop();
}

void WiFiMQTTManager::reset() {
    delay(3000);
    wm->resetSettings();
    ESP.restart();
    delay(5000);      
}

void WiFiMQTTManager::setup(String sketchName) {
  _sketchName = sketchName;

  WiFi.begin();

  String _chipId = String(WIFI_getChipId(),HEX);
  String _mac = String(WiFi.macAddress());
  _mac.toLowerCase();
  _mac.replace(":", "");
  _mac.replace("240ac4", "a");            // vendor = Espressif Inc.
  String _clientId = "EnderLab_Candle_" + _mac;

  strcpy(chipId, _chipId.c_str());
  strcpy(clientId, _clientId.c_str());
  strcpy(deviceId, _clientId.c_str());

  Serial.print("WMM: clientId: ");
  Serial.println(clientId);

  _setupSpiffs();
  //set config save notify callback
  wm->setSaveConfigCallback([&]() {
    Serial.println("WMM: should save config...");
    _shouldSaveConfig = true;
  });
  wm->setClass("invert"); // dark theme

  WiFiManagerParameter custom_friendly_name("name", "Friendly Name", _friendly_name, 40);
  WiFiManagerParameter custom_mqtt_server("server", "MQTT Server", _mqtt_server, 40);
  WiFiManagerParameter custom_mqtt_port("port", "MQTT Port", _mqtt_port, 6);
  WiFiManagerParameter custom_mqtt_username("username", "MQTT Username", _mqtt_username, 40);
  WiFiManagerParameter custom_mqtt_password("password", "MQTT Password", _mqtt_password, 40);
  wm->addParameter(&custom_friendly_name);
  wm->addParameter(&custom_mqtt_server);
  wm->addParameter(&custom_mqtt_port);
  wm->addParameter(&custom_mqtt_username);
  wm->addParameter(&custom_mqtt_password); 

  wm->setAPCallback([&](WiFiManager *myWiFiManager) {
    Serial.println("WMM: entering config mode...");
    Serial.print("WMM: ");
    Serial.println(WiFi.softAPIP());
    Serial.print("WMM: connect your device to WiFi SSID ");
    Serial.print(myWiFiManager->getConfigPortalSSID());
    Serial.println(" to configure WiFi and MQTT...");
    digitalWrite(LED_BUILTIN, LOW); 
  });

  Serial.println("WMM: lets try to connect to WiFi...");
  if (!wm->autoConnect()) {
    Serial.println("WMM: failed to connect and hit timeout...");
    delay(3000);
    ESP.restart();
    delay(5000);
  }
  Serial.println("WMM: connected to WiFi!!...");

  //read updated parameters
  strcpy(_friendly_name, custom_friendly_name.getValue());
  strcpy(_mqtt_server, custom_mqtt_server.getValue());
  strcpy(_mqtt_port, custom_mqtt_port.getValue());
  strcpy(_mqtt_username, custom_mqtt_username.getValue());
  strcpy(_mqtt_password, custom_mqtt_password.getValue());
  
  //save the custom parameters to FS
  if (_shouldSaveConfig) {
    Serial.println("WMM: saving config...");
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json["friendly_name"] = _friendly_name;
    json["mqtt_server"] = _mqtt_server;
    json["mqtt_port"]   = _mqtt_port;
    json["mqtt_username"]   = _mqtt_username;
    json["mqtt_password"]   = _mqtt_password;

    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
      Serial.println("WMM: failed to open config file for writing...");
    }

    json.prettyPrintTo(Serial);
    json.printTo(configFile);
    configFile.close();
    _shouldSaveConfig = false;
  }

  Serial.println("WMM: ");
  Serial.print("WMM: local IP:");
  Serial.println(WiFi.localIP());
  
  unsigned short port = (unsigned short) strtoul(_mqtt_port, NULL, 0);

  Serial.print("WMM: _mqtt_server: ");
  Serial.println(_mqtt_server);
  Serial.print("WMM: _mqtt_port: ");
  Serial.println(_mqtt_port);

  Serial.print("WMM: clientId: ");
  Serial.println(clientId);

  client.reset(new PubSubClient(_espClient));
  client->setServer(_mqtt_server, port);

  Serial.print("WMM: attempting MQTT connection...");
  bool connectedMqtt = false;
  if (strlen(_mqtt_username) > 0)
    connectedMqtt = client->connect(clientId, _mqtt_username, _mqtt_password);
  else 
    connectedMqtt = client->connect(clientId);
  if (!connectedMqtt) {
    Serial.println("failed to connect to MQTT...");
    reset();   
  } else {
    Serial.println("mqtt connected...via setup...");
    subscribeTo();
    client->setCallback(subscriptionCallback);
  }
}

void reset() {
    delay(3000);
    wm->resetSettings();
    ESP.restart();
    delay(5000);      
}

void WiFiMQTTManager::_setupSpiffs(){
  if (formatFS == true) {
    Serial.print("WMM: formatting FS...please wait..... ");
    //clean FS, for testing
    SPIFFS.format();
  }

  //read configuration from FS json
  Serial.println("WMM: mounting FS...");

  if (SPIFFS.begin()) {
    Serial.println("WMM: mounted file system...");
    if (SPIFFS.exists("/config.json")) {
      //file exists, reading and loading
      Serial.println("WMM: reading config file...");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        Serial.println("WMM: opened config file...");
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        json.printTo(Serial);
        if (json.success()) {
          Serial.println("\nWMM: parsed json...");
          strcpy(_friendly_name, json["friendly_name"]);
          strcpy(_mqtt_server, json["mqtt_server"]);
          strcpy(_mqtt_port, json["mqtt_port"]);
          strcpy(_mqtt_username, json["mqtt_username"]);
          strcpy(_mqtt_password, json["mqtt_password"]);
        } else {
          Serial.println("WMM: failed to load json config...");
        }
      }
    } else { Serial.println("WMM: could not find config file..."); }
  } else {
    Serial.println("WMM: failed to mount FS");
    Serial.println("WMM: formating FS...re-upload to try again...");
    SPIFFS.format();
  }
}

void WiFiMQTTManager::_reconnect() {
  // Loop until we're reconnected
  while (!client->connected()) {
    Serial.print("WMM: attempting MQTT connection...");
    // Attempt to connect

    if (client->connect(clientId) == true) {
      Serial.println("mqtt connected...via reconnect loop...");
      // Subscribe
      subscribeTo();
      client->setCallback(subscriptionCallback);      
    } else {
      Serial.print("mqtt connect failed, rc=");
      Serial.print(client->state());
      Serial.println(" try again in 5 seconds...");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void WiFiMQTTManager::setDebugOutput(bool b) {
  wm->setDebugOutput(b);
}

void _placeholderSubscibeTo() {
  Serial.println("WMM: ....placeholderSubscibeTo called...");
}

void _settingsAP() {
  // start portal w delay
  Serial.println("WMM: starting config portal...");
  
  wm->resetSettings();
  ESP.restart();

}


