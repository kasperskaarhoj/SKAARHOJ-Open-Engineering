/*********************************************************************
This library is inspired by the SkaarhojDisplayArray library from Adafruit
written by Limor Fried/Ladyada  for Adafruit Industries. The original
library is BSD licensed.

Just like SKAARHOJ, Adafruit also invests time and resources providing
open source code, so SKAARHOJ encourage you to support Adafruit and
open-source hardware by purchasing products from Adafruit as well!
*********************************************************************/

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <SkaarhojDisplayArray.h>
#include <SkaarhojPgmspace.h>

SkaarhojDisplayArray display;

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
  Serial.begin(115200);

  Wire.begin();

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(0, 1);

  // init done
  display.clearDisplay();   // clears the screen and buffer
  display.display(B1111);  // Write to all
  display.setRotation(1);

  // text display tests
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Hello, world!");
  display.setTextColor(BLACK, WHITE); // 'inverted' text
  display.println(3.141592);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.print("0x"); display.println(0xDEADBEEF, HEX);
  display.display(B1111);
  delay(2000);

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
  delay(2000);

  // draw a bitmap icon and 'animate' movement

  display.clearDisplay();
  display.drawBitmap(0, 0, logo16_glcd_bmp, LOGO16_GLCD_WIDTH, LOGO16_GLCD_HEIGHT, WHITE);
  display.display(B1111);

  delay(2000);
}

void loop() {
  display.testProgramme(B1111);
}



