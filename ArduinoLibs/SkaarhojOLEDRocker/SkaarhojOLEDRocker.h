/*********************************************************************
This library is inspired by the Adafruit_SSD1306 library from Adafruit
written by Limor Fried/Ladyada  for Adafruit Industries. The original 
library is BSD licensed.

Just like SKAARHOJ, Adafruit also invests time and resources providing 
open source code, so SKAARHOJ encourage you to support Adafruit and 
open-source hardware by purchasing products from Adafruit as well!
*********************************************************************/


#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include <Adafruit_GFX.h>


#define BLACK 0
#define WHITE 1

#define SKAARHOJOLEDRocker_LCDWIDTH                  96
#define SKAARHOJOLEDRocker_LCDHEIGHT                 64


class SkaarhojOLEDRocker : public Adafruit_GFX {
 public:
  void drawPixel(int16_t x, int16_t y, uint16_t color);

  SkaarhojOLEDRocker();
  void begin(uint8_t address);

  void sendCommand(uint8_t c);
  void sendData(uint8_t c);

  void clearDisplay(void);
  void display(uint8_t cs);


 private:
	uint8_t _boardAddress, _dataPin, _clockPin, _cs;
	bool _dc, _rst, _vcc;
  void fastSPIwrite(uint8_t c);

  	void chipSelect(uint8_t cs);
	void setDC(bool dc);
	void setVCC(bool dc);
	void writeControlPins();

  volatile uint8_t *mosiport, *clkport;
  uint8_t mosipinmask, clkpinmask;
};
