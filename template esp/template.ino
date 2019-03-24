#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>

void setup() {
  Serial.begin(115200);
  setupWifi("GoodLife", "Exodus20:15");
  setupOta("tractor-comm-1", "123", 8266);
}

void loop() {
  // put your main code here, to run repeatedly:

}

void setupWifi(char* ssid, char* pw) {
  delay(10);

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pw);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setupOta(char* hostname, char* otaPw, int otaPort) {  
  ArduinoOTA.setPort(otaPort);
  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname(hostname);
  ArduinoOTA.setPassword(otaPw);
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
}
