/*********************************************************************
This library is inspired by the ... library from Adafruit
written by Limor Fried/Ladyada  for Adafruit Industries. The original
library is BSD licensed.

Just like SKAARHOJ, Adafruit also invests time and resources providing
open source code, so SKAARHOJ encourage you to support Adafruit and
open-source hardware by purchasing products from Adafruit as well!
*********************************************************************/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <SkaarhojOLEDRocker.h>
#include <SkaarhojPgmspace.h>
#include <SPI.h>

SkaarhojOLEDRocker display;

#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16
static const unsigned char PROGMEM logo16_glcd_bmp[] =
{ B11111111, B11111111,
  B00000000, B00000000,
  B00111111, B11111110,
  B01111111, B11111110,
  B11111000, B00000000,
  B11111000, B00000000,
  B11111000, B00000000,
  B01111111, B11111100,
  B00111111, B11111110,
  B00000000, B00011111,
  B00000000, B00001111,
  B00000000, B00011111,
  B01111111, B11111110,
  B01111111, B11111100,
  B00000000, B00000000,
  B11111111, B11111111
};

#include <Streaming.h>

void setup()   {
  Serial.begin(115200);

  Wire.begin();

  display.begin(6);

  // init done
  display.clearDisplay();   // clears the screen and buffer
  display.display(B1111);  // Write to all

  // text display tests
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print("Hello, world!");
  display.display(B1111);
  delay(5000);
  // while(true);;


  display.clearDisplay();   // clears the screen and buffer
  display.display(B1111);  // Write to all

  delay(100);

  // text display tests
  for (int i = 0; i < 4; i++)  {
    display.clearDisplay();   // clears the screen and buffer
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.print("Display ");
    display.print(i);
    display.display(B1 << i);
  }
  delay(1000);

  // draw a bitmap icon and 'animate' movement

  display.clearDisplay();
  display.drawBitmap(0, 0, logo16_glcd_bmp, LOGO16_GLCD_WIDTH, LOGO16_GLCD_HEIGHT, WHITE);
  display.display(B1111);

  delay(2000);
}

void loop() {
  display.testProgramme(B1111);
}


