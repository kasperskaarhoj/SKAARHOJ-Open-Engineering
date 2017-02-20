/*********************************************************************
This library is inspired by the Adafruit_SSD1306 library from Adafruit
written by Limor Fried/Ladyada for Adafruit Industries. The original 
library is BSD licensed.

Just like SKAARHOJ, Adafruit also invests time and resources providing 
open source code, so SKAARHOJ encourage you to support Adafruit and 
open-source hardware by purchasing products from Adafruit as well!
*********************************************************************/


#include "Arduino.h"
//  #include "SkaarhojPgmspace.h"  - 23/2 2014
#include <Adafruit_GFX.h>
#include <Wire.h>


#define BLACK 0
#define WHITE 1

#define SKAARHOJDISPARRAY_LCDWIDTH                  64
#define SKAARHOJDISPARRAY_LCDHEIGHT                 32


class SkaarhojOLED32x64 : public Adafruit_GFX {
 public:
  void drawPixel(int16_t x, int16_t y, uint16_t color);

  SkaarhojOLED32x64();
  void begin(bool initReset=true, bool vccDefault=false);

  void sendCommand(uint8_t c);

  void clearDisplay(void);
  void invertDisplay(bool i);
  void display();
  
  void testProgramme(uint8_t buttonMask);

 private:
	uint8_t _boardAddress; 
};
