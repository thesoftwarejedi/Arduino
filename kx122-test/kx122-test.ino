#include <Wire.h>

#define KX122_ADDR_1037 0x1E

#define KX122_REGISTER_CNTL1 0x18
#define KS122_REGISTER_XOUTL 0x06

#define KX122_CNTL1_ACTIVE 0x80
#define KX122_CNTL1_HI_RES 0x40
#define KX122_CNTL1_8G 0x10

#define LED 1

int KX122_Accel_X_RawOUT = 0;
int KX122_Accel_Y_RawOUT = 0;
int KX122_Accel_Z_RawOUT = 0;
float KX122_Accel_X_OUT = 0;
float KX122_Accel_Y_OUT = 0;
float KX122_Accel_Z_OUT = 0;

void setup() {
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW); 
  //Serial.begin(115200);
  
  Wire.begin();
  
  //hi res
  Wire.beginTransmission(KX122_ADDR_1037);
  Wire.write(KX122_REGISTER_CNTL1);
  Wire.write(KX122_CNTL1_HI_RES);
  Wire.endTransmission();

  //on and hi res
  Wire.beginTransmission(KX122_ADDR_1037);
  Wire.write(KX122_REGISTER_CNTL1);
  Wire.write(KX122_CNTL1_ACTIVE | KX122_CNTL1_HI_RES);
  Wire.endTransmission();
}

void loop() {
  Wire.beginTransmission(KX122_ADDR_1037);
  Wire.write(KS122_REGISTER_XOUTL);
  Wire.endTransmission();
  Wire.requestFrom(KX122_ADDR_1037, 6);
  /*
  Serial.println(Wire.read());
  Serial.println(Wire.read());
  Serial.println(Wire.read());
  Serial.println(Wire.read());
  Serial.println(Wire.read());
  Serial.println(Wire.read());
  */
  
  KX122_Accel_X_RawOUT = Wire.read();
  KX122_Accel_X_RawOUT |= (Wire.read() << 8);
  
  KX122_Accel_Y_RawOUT = Wire.read();
  KX122_Accel_Y_RawOUT |= (Wire.read() << 8);
  
  KX122_Accel_Z_RawOUT = Wire.read();
  KX122_Accel_Z_RawOUT |= (Wire.read() << 8);

  KX122_Accel_X_OUT = (float)KX122_Accel_X_RawOUT / 16384;
  KX122_Accel_Y_OUT = (float)KX122_Accel_Y_RawOUT / 16384;
  KX122_Accel_Z_OUT = (float)KX122_Accel_Z_RawOUT / 16384;

/*
  Serial.println(KX122_Accel_X_RawOUT);
  Serial.println(KX122_Accel_Y_RawOUT);
  Serial.println(KX122_Accel_Z_RawOUT);
  
  
  Serial.write("KX122 (X) = ");
  Serial.print(KX122_Accel_X_OUT);
  Serial.println(" g");
  Serial.write("KX122 (Y) = ");
  Serial.print(KX122_Accel_Y_OUT);
  Serial.println(" g");
  Serial.write("KX122 (Z) = ");
  Serial.print(KX122_Accel_Z_OUT);
  Serial.println(" g");
*/
  if (KX122_Accel_Z_OUT < .5) {
    digitalWrite(LED, HIGH);
    delay(500);
    digitalWrite(LED, LOW);
  }
  delay(500);
  
}
