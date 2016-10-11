/*********************************************************************
This library is inspired by the ... library from Adafruit
written by Limor Fried/Ladyada  for Adafruit Industries. The original 
library is BSD licensed.

Just like SKAARHOJ, Adafruit also invests time and resources providing 
open source code, so SKAARHOJ encourage you to support Adafruit and 
open-source hardware by purchasing products from Adafruit as well!
*********************************************************************/

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <SkaarhojOLED64x256.h>

SkaarhojOLED64x256 display;

#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 
static const unsigned char PROGMEM logo16_glcd_bmp[] =
{ B11111111, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000 };

#include <Streaming.h>


void setup()   {                
  Serial.begin(115200);
  
  Wire.begin();
  
  display.begin(0);

  // init done  
  display.clearDisplay();   // clears the screen and buffer
  display.display(B1111);  // Write to all
  display.setRotation(0);

  // text display tests
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.print("Hello, world!");
  display.print("Hello, world!");
  display.print("Hello, world!");
  display.print("Hello, world!");
  display.print("Hello, world!");
  display.print("Hello, world!");
  display.print("Hello, world!");
  display.print("Hello, world!");
  display.print("Hello, world!");
  display.print("Hello, world!");
  display.print("Hello, world!");
  display.print("Hello, world!");
  display.print("Hello, world!");
  display.print("Hello, world!");
  display.print("Hello, world!");
  display.print("Hello, world!");
  display.print("Hello, world!");
  display.print("Hello, world!");
  display.println("Hello, world!");
  display.setTextColor(BLACK, WHITE); // 'inverted' text
  display.println(3.141592);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.print("0x"); display.println(0xDEADBEEF, HEX);
  display.display(B1111);
  delay(10000);

  display.clearDisplay();   // clears the screen and buffer
  display.display(B1111);  // Write to all

  delay(100);
  
  // text display tests
  for(int i=0; i<4; i++)  {
    display.clearDisplay();   // clears the screen and buffer
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.print("Display ");
    display.print(i);
    display.display(B1 << i);
  }
  delay(100);
  
    // draw a bitmap icon and 'animate' movement

display.clearDisplay();
  display.drawBitmap(0,0, logo16_glcd_bmp, LOGO16_GLCD_WIDTH, LOGO16_GLCD_HEIGHT, WHITE);
    display.display(B1111);

  delay(200);
}

unsigned long lastTime[4];

void loop() {  
  
  for(int k=0; k<16; k++)  {
  for(int j=0; j<16; j++)  {
  for(int i=0; i<1; i++)  {
    display.clearDisplay();   // clears the screen and buffer
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.println(millis());
    display.println(millis()-lastTime[i]);
    display.setTextSize(1);
    display << "j=" << j << "\n";
    display << "k=" << k << "\n";
    lastTime[i] = millis();
    display.display(B1 << i);
  }
  

		// Set Contrast and Current:
    display.sendCommand(0xC1, 1);
    display.sendData(j<<4, 1);
  }
		// Master Contrast Current Control:
    display.sendCommand(0xC7, 1);
    display.sendData(k, 1);  
  }
}

