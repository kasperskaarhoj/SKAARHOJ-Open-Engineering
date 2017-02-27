/*********************************************************************
This library is inspired by the SkaarhojDisplayArray library from Adafruit
written by Limor Fried/Ladyada  for Adafruit Industries. The original
library is BSD licensed.

Just like SKAARHOJ, Adafruit also invests time and resources providing
open source code, so SKAARHOJ encourage you to support Adafruit and
open-source hardware by purchasing products from Adafruit as well!
*********************************************************************/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <SkaarhojOLED32x64.h>
#include <SkaarhojPgmspace.h>

SkaarhojOLED32x64 display;

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




void setup()   {
  Serial.begin(9600);

  Wire.begin();

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(2);
}

void loop() {
  display.display();
}



