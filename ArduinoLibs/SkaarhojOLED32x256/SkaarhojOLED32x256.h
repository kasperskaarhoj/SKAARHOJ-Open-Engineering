/*********************************************************************
This library is inspired by the Adafruit_SSD1306 library from Adafruit
written by Limor Fried/Ladyada  for Adafruit Industries. The original
library is BSD licensed.

Just like SKAARHOJ, Adafruit also invests time and resources providing
open source code, so SKAARHOJ encourage you to support Adafruit and
open-source hardware by purchasing products from Adafruit as well!
*********************************************************************/

#ifndef SkaarhojOLED32x256_h
#define SkaarhojOLED32x256_h

#include "Arduino.h"
#include "Wire.h"
#include <Adafruit_GFX.h>

#if defined(ARDUINO_SKAARDUINO_V1) || defined(ARDUINO_SKAARDUINO_DUE)
#include <SPI.h>
#endif

#define BLACK 0
#define WHITE 1

#define SKAARHOJOLED32x256_LCDWIDTH 256
#define SKAARHOJOLED32x256_LCDHEIGHT 32

class SkaarhojOLED32x256 : public Adafruit_GFX {
public:
  void drawPixel(int16_t x, int16_t y, uint16_t color);

  SkaarhojOLED32x256();
  void begin(uint8_t address, uint8_t cs = 0x1F, uint8_t base = 88, bool PCA9672 = false);

  void sendCommand(uint8_t c, uint8_t cs);
  void sendData(uint8_t c, uint8_t cs);

  void clearDisplay(void);
  void invertDisplay(bool i, uint8_t cs);
  void display(uint8_t cs) {
    display(cs, 0, 0, 256, 32);
  }
  void display(uint8_t cs, uint8_t x, uint8_t y, uint16_t w, uint8_t h); // x and w must be divisible by 8 with no remainder
  void zoom2x(bool enable);

private:
  uint8_t _boardAddress, _dataPin, _clockPin, _cs;
  bool _dc, _rst, _vcc, _PCA9672;
  bool _zoom2Xenabled;
  void fastSPIwrite(uint8_t c);

  void chipSelect(uint8_t cs);
  void setDC(bool dc);
  void setVCC(bool dc);
  void writeControlPins();

  volatile uint8_t *mosiport, *clkport;
  uint8_t mosipinmask, clkpinmask;
};

#endif
