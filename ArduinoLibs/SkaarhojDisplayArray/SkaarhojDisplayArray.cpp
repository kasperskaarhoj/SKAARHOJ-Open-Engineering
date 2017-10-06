/*********************************************************************
This library is inspired by the Adafruit_SSD1306 library from Adafruit
written by Limor Fried/Ladyada  for Adafruit Industries. The original
library is BSD licensed.

Just like SKAARHOJ, Adafruit also invests time and resources providing
open source code, so SKAARHOJ encourage you to support Adafruit and
open-source hardware by purchasing products from Adafruit as well!
*********************************************************************/

#include "SkaarhojDisplayArray.h"

#ifdef __arm__ // Arduino Due:
#define _BV(bit) (1 << (bit))
#endif

  // The memory buffer for the LCD
  static uint8_t buffer12832[SKAARHOJDISPARRAY_LCDHEIGHT * SKAARHOJDISPARRAY_LCDWIDTH / 8];

// The most basic function, set a single pixel
void SkaarhojDisplayArray::drawPixel(int16_t x, int16_t y, uint16_t color) {
  if ((bbox_width > 0 && x >= bbox_width) || (bbox_height > 0 && y >= bbox_height))
    return;

  x += bbox_x;
  y += bbox_y;

  if ((x < 0) || (x >= width()) || (y < 0) || (y >= height()))
    return;

  // check rotation, move pixel around if necessary
  switch (getRotation()) {
  case 1:
    swap(x, y);
    x = WIDTH - x - 1;
    break;
  case 2:
    x = WIDTH - x - 1;
    y = HEIGHT - y - 1;
    break;
  case 3:
    swap(x, y);
    y = HEIGHT - y - 1;
    break;
  }

  // x is which column
  if ((color == WHITE)^_invPixels)
    buffer12832[x + (y / 8) * SKAARHOJDISPARRAY_LCDWIDTH] |= _BV((y % 8));
  else
    buffer12832[x + (y / 8) * SKAARHOJDISPARRAY_LCDWIDTH] &= ~_BV((y % 8));
}

// Empty constructor.
SkaarhojDisplayArray::SkaarhojDisplayArray() : Adafruit_GFX(SKAARHOJDISPARRAY_LCDWIDTH, SKAARHOJDISPARRAY_LCDHEIGHT) {}

void SkaarhojDisplayArray::begin(uint8_t address, uint8_t boardType, uint8_t cs, bool initReset, bool vccDefault) {
  // NOTE: Wire.h should definitely be initialized at this point! (Wire.begin())

  _boardAddress = 88 | (address & B111); // 0-7
  _boardType = boardType;                // 1=Array A (vertical), 2=Array B (Large horizontal), 3=Array C (Small horizontal)
#if defined(ARDUINO_SKAARDUINO_V1) || defined(ARDUINO_SKAARDUINO_DUE)
  SPI.begin();
#endif

  // Set SPI pins up:
  _clockPin = 48;
  _dataPin = 49;

  pinMode(_clockPin, OUTPUT);
  pinMode(_dataPin, OUTPUT);

#if !defined(ARDUINO_SKAARDUINO_V1) && !defined(ARDUINO_SKAARDUINO_DUE)
  clkport = portOutputRegister(digitalPinToPort(_clockPin));
  clkpinmask = digitalPinToBitMask(_clockPin);
  mosiport = portOutputRegister(digitalPinToPort(_dataPin));
  mosipinmask = digitalPinToBitMask(_dataPin);
#endif

  // Control pins:
  _cs = 0;
  _dc = false;
  _vcc = vccDefault;
  _rst = true;
  writeControlPins();
  delay(1);

  if (initReset) {
    // bring reset low
    _rst = false;
    writeControlPins();
    // wait 10ms
    delay(10);

    // bring out of reset
    _rst = true;
    writeControlPins();
  }

  switch (_boardType) {
  case 1: // SSD1306 driver chip, display array A. Initialized as in OLED manual (ER-OLED0.91-1):

    // Display Off:
    sendCommand(0xAE, cs);

    // Set Display Clock Divide Ratio / Oscillator Frequency:
    sendCommand(0xD5, cs);
    sendCommand(0x80, cs);

    // Set Multiplex Ratio:
    sendCommand(0xA8, cs);
    sendCommand(0x1F, cs);

    // Set Display Offset:
    sendCommand(0xD3, cs);
    sendCommand(0x0, cs);

    // Set Display Start Line:
    sendCommand(0x40, cs);

    // Set Charge Pump:
    sendCommand(0x8D, cs);
    sendCommand(0x14, cs); // Internal VSS

    // Memory Mode (Not part of default application suggestion!)
    sendCommand(0x20, cs);
    sendCommand(0x00, cs); // Horizontal Addressing Mode

    // Set Segment Re-Map:
    sendCommand(0xA1, cs);

    // Set COM Output Scan Direction:
    sendCommand(0xC8, cs);

    // Set COM Pins Hardware Configuration:
    sendCommand(0xDA, cs);
    sendCommand(0x02, cs);

    // Set Contrast Control:
    sendCommand(0x81, cs);
    sendCommand(0x8F, cs);

    // Set Pre-Charge Period:
    sendCommand(0xD9, cs);
    sendCommand(0xF1, cs);

    // Set VCOMH Deselect Level:
    sendCommand(0xDB, cs);
    sendCommand(0x40, cs);

    // Set Entire Display On/Off:
    sendCommand(0xA4, cs);

    // Set Normal/Inverse Display:
    sendCommand(0xA6, cs);

    break;
  case 2: // SSD1305 driver chip, display array A. Initialized as in OLED manual (ER-OLED011-1):
  case 3:

    // Display Off:
    sendCommand(0xAE, cs);

    // Set Display Clock Divide Ratio / Oscillator Frequency:
    sendCommand(0xD5, cs);
    sendCommand(0xA0, cs);

    // Set Multiplex Ratio:
    sendCommand(0xA8, cs);
    sendCommand(0x23, cs);

    // Set Display Offset:
    sendCommand(0xD3, cs);
    sendCommand(0x0, cs);

    // Set Display Start Line:
    sendCommand(0x40, cs);

    // Set Master Configuration:
    sendCommand(0xAD, cs);
    sendCommand(0x8E, cs);

    // Set Master Configuration:
    sendCommand(0xD8, cs);
    sendCommand(0x05, cs);

    // Memory Mode (Not part of default application suggestion!)
    sendCommand(0x20, cs);
    sendCommand(0x00, cs); // Horizontal Addressing Mode

    // Set Segment Re-Map:
    sendCommand(0xA1, cs);

    // Set COM Output Scan Direction:
    sendCommand(0xC8, cs);

    // Set COM Pins Hardware Configuration:
    sendCommand(0xDA, cs);
    sendCommand(0x12, cs);

    // Set Look Up Table:
    sendCommand(0x91, cs);
    sendCommand(0x3F, cs);
    sendCommand(0x3F, cs);
    sendCommand(0x3F, cs);
    sendCommand(0x3F, cs);

    // Set Current (Contrast) Control for Bank 0:
    sendCommand(0x81, cs);
    sendCommand(0x6F, cs);

    // Set Pre-Charge Period:
    sendCommand(0xD9, cs);
    sendCommand(0xD2, cs);

    // Set VCOMH Deselect Level:
    sendCommand(0xDB, cs);
    sendCommand(0x34, cs);

    // Set Entire Display On/Off:
    sendCommand(0xA4, cs);

    // Set Normal/Inverse Display:
    sendCommand(0xA6, cs);

    // Turn on VCC:
    setVCC(true);
    delay(100); // Waiting for VCC to be stable. with a 1K load it was after 40 ms.
    break;
  }

  // Turn Display On:
  sendCommand(0xAF, cs);
}

void SkaarhojDisplayArray::sendCommand(uint8_t c, uint8_t cs) {

  chipSelect(0);
  setDC(false);
  chipSelect(cs);

  fastSPIwrite(c);

  chipSelect(0);
}

// clear everything
void SkaarhojDisplayArray::clearDisplay(void) { memset(buffer12832, 0, (SKAARHOJDISPARRAY_LCDWIDTH * SKAARHOJDISPARRAY_LCDHEIGHT / 8)); }

void SkaarhojDisplayArray::invertDisplay(bool i, uint8_t cs) {
  if (i) {
    sendCommand(0xA7, cs); // Inverted
  } else {
    sendCommand(0xA6, cs); // Normal,
  }
}

void SkaarhojDisplayArray::display(uint8_t cs) {

  switch (_boardType) {
  case 1:
    sendCommand(0x21, cs);
    sendCommand(0x0, cs);
    sendCommand(127, cs);

    sendCommand(0x22, cs);
    sendCommand(0x0, cs);
    sendCommand(3, cs);
    break;
  case 2:
    sendCommand(0x21, cs);
    sendCommand(0x0 + 4, cs);
    sendCommand(127 + 4, cs);

    sendCommand(0x22, cs);
    sendCommand(0x0, cs);
    sendCommand(4, cs); // Using 4 instead of 3 because we want to clear the last 4 lines of pixels on the 1.1 inch display (so far not supported, see below as well)
  case 3:
    sendCommand(0x21, cs);
    sendCommand(0x0 + 2, cs);
    sendCommand(127 + 2, cs);

    sendCommand(0x22, cs);
    sendCommand(0x0, cs);
    sendCommand(4, cs); // Using 4 instead of 3 because we want to clear the last 4 lines of pixels on the 1.1 inch display (so far not supported, see below as well)
    break;
  }

  // SPI
  chipSelect(0);
  setDC(true);
  chipSelect(cs);

  for (uint16_t i = 0; i < (SKAARHOJDISPARRAY_LCDWIDTH * SKAARHOJDISPARRAY_LCDHEIGHT / 8); i++) {
    fastSPIwrite(buffer12832[i]);
  }

  if (_boardType == 2) { // This is in order to wash away the last 4 lines of pixels which we don't support so far:
    for (uint16_t i = 0; i < (SKAARHOJDISPARRAY_LCDWIDTH * 8 / 8); i++) {
      fastSPIwrite(0);
    }
  }

  chipSelect(0);
}

inline void SkaarhojDisplayArray::fastSPIwrite(uint8_t d) {
#if defined(ARDUINO_SKAARDUINO_V1) || defined(ARDUINO_SKAARDUINO_DUE)
  SPISettings settingsA(10000000, MSBFIRST, SPI_MODE0);
  SPI.beginTransaction(settingsA);
  SPI.transfer(d);
  SPI.endTransaction();
#else
  for (uint8_t bit = 0x80; bit; bit >>= 1) {
    *clkport &= ~clkpinmask;
    if (d & bit)
      *mosiport |= mosipinmask;
    else
      *mosiport &= ~mosipinmask;
    *clkport |= clkpinmask;
  }
#endif
}

void SkaarhojDisplayArray::chipSelect(uint8_t cs) {
  _cs = cs;
  writeControlPins();
}

void SkaarhojDisplayArray::setDC(bool dc) {
  _dc = dc;
  writeControlPins();
}

void SkaarhojDisplayArray::setVCC(bool vcc) {
  _vcc = vcc;
  writeControlPins();
}

void SkaarhojDisplayArray::writeControlPins() {
  Wire.beginTransmission(_boardAddress);
  Wire.write(((uint8_t)~_cs));
  Wire.write((_rst << 7) | (_dc << 6) | (_vcc << 5));
  Wire.endTransmission();
}

void SkaarhojDisplayArray::testProgramme(uint8_t buttonMask) {
  static uint16_t lastTime;

  for (int i = 0; i < 8; i++) {
    if (buttonMask & (B1 << i)) {

      clearDisplay(); // clears the screen and buffer
      setTextColor(WHITE);
      setCursor(0, 0);
      setTextSize(1);
      print(millis(), HEX);
      if (i == 0) {
        print(F(" [dt="));
        print((uint16_t)millis() - lastTime);
        lastTime = millis();
        print(F("]"));
      }
      print(F(" "));
      setTextSize(2);
      print((uint8_t)millis(), BIN);
      setTextSize(1);
      print(millis(), DEC);
      print(F(" - "));
      print(millis(), BIN);
      display(B1 << i);
    }
  }
}
