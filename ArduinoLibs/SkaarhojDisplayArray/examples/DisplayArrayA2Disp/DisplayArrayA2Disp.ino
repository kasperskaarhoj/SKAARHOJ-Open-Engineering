/*********************************************************************
This library is inspired by the Skaarhojdisplay1Array library from Adafruit
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

SkaarhojDisplayArray display1;
SkaarhojDisplayArray display2;

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
  display1.begin(4, 1);
  display2.begin(5, 1);

  // init done
  display1.clearDisplay();   // clears the screen and buffer
  display1.display(B1111);  // Write to all
  display1.setRotation(1);
  display2.clearDisplay();   // clears the screen and buffer
  display2.display(B1111);  // Write to all
  display2.setRotation(1);

  // text display1 tests
  display1.setTextSize(1);
  display1.setTextColor(WHITE);
  display1.setCursor(0, 0);
  display1.println("Hello, world!");
  display1.setTextColor(BLACK, WHITE); // 'inverted' text
  display1.println(3.141592);
  display1.setTextSize(2);
  display1.setTextColor(WHITE);
  display1.print("0x"); display1.println(0xDEADBEEF, HEX);
  display1.display(B1111);
  display2.setTextSize(1);
  display2.setTextColor(WHITE);
  display2.setCursor(0, 0);
  display2.println("Hello, world!");
  display2.setTextColor(BLACK, WHITE); // 'inverted' text
  display2.println(3.141592);
  display2.setTextSize(2);
  display2.setTextColor(WHITE);
  display2.print("0x"); display2.println(0xDEADBEEF, HEX);
  display2.display(B1111);
  delay(2000);

  // text display1 tests
  for (int i = 0; i < 4; i++)  {
    display1.clearDisplay();   // clears the screen and buffer
    display1.setTextSize(2);
    display1.setTextColor(WHITE);
    display1.setCursor(0, 0);
    display1.print("display1 ");
    display1.print(i);
    display1.display(B1 << i);

    display2.clearDisplay();   // clears the screen and buffer
    display2.setTextSize(2);
    display2.setTextColor(WHITE);
    display2.setCursor(0, 0);
    display2.print("display1 ");
    display2.print(i);
    display2.display(B1 << i);
  }
  delay(2000);

  // draw a bitmap icon and 'animate' movement

  display1.clearDisplay();
  display1.drawBitmap(0, 0, logo16_glcd_bmp, LOGO16_GLCD_WIDTH, LOGO16_GLCD_HEIGHT, WHITE);
  display1.display(B1111);
  display2.clearDisplay();
  display2.drawBitmap(0, 0, logo16_glcd_bmp, LOGO16_GLCD_WIDTH, LOGO16_GLCD_HEIGHT, WHITE);
  display2.display(B1111);

  delay(2000);
}

void loop() {
  display1.testProgramme(B1111);
  display2.testProgramme(B1111);
}



