#include <DMXSerial.h>

const int RedPin =    11; 

void setup () {
  DMXSerial.init(DMXReceiver);
  
  // enable pwm outputs
  pinMode(RedPin,   OUTPUT); // sets the digital pin as output
}


void loop() {
  // Calculate how long no data backet was received
  unsigned long lastPacket = DMXSerial.noDataSince();
  
  if (lastPacket < 5000) {
    if (DMXSerial.read(1) > 127) {
      digitalWrite(RedPin, HIGH);
    } else {
      digitalWrite(RedPin, LOW);
    }

  } else {
    digitalWrite(RedPin,   LOW);
  } 
}

