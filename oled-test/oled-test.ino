#include "Adafruit_SSD1306.h"

#include "SPI.h"
#include "Wire.h"
#include "Adafruit_GFX.h"
 
#define OLED_RESET D0  // GPIO0
Adafruit_SSD1306 display(OLED_RESET);
 
void setup()   {
  Serial.begin(115200);
  
  Serial.write(1);
  
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); 
  // init done

  Serial.write(2);
 
  display.display();
  delay(2000);
 
  // Clear the buffer.
  display.clearDisplay();
 
  // text display tests
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,10);
  display.println("Hello");
  display.println("World");
  
  display.display();
  delay(10000);
  display.clearDisplay();
}
 
 
void loop() {
 
}
