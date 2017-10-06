/*********************************************************************
This library is inspired by the Adafruit_SSD1306 library from Adafruit
written by Limor Fried/Ladyada  for Adafruit Industries. The original
library is BSD licensed.

Just like SKAARHOJ, Adafruit also invests time and resources providing
open source code, so SKAARHOJ encourage you to support Adafruit and
open-source hardware by purchasing products from Adafruit as well!
*********************************************************************/

//  #include "SkaarhojPgmspace.h"  - 23/2 2014
#include "SkaarhojOLED64x256.h"
#include "Streaming.h"

#ifdef __arm__ // Arduino Due:
#define _BV(bit) (1 << (bit))
#endif

// The memory buffer for the LCD
static uint8_t buffer64256[SKAARHOJOLED64x256_LCDHEIGHT * SKAARHOJOLED64x256_LCDWIDTH / 8];

// The most basic function, set a single pixel
void SkaarhojOLED64x256::drawPixel(int16_t x, int16_t y, uint16_t color) {
  if ((bbox_width > 0 && x >= bbox_width) || (bbox_height > 0 && y >= bbox_height))
    return;

  x += bbox_x;
  y += bbox_y;

  if ((x < 0) || (x >= width()) || (y < 0) || (y >= height()))
    return;

  uint8_t ox = x;
  uint8_t oy = y;
  for (uint8_t a = 0; a < (_zoom2Xenabled ? 4 : 1); a++) {
    if (_zoom2Xenabled) {
		x=ox*2+(a&B1);
		y=oy*2+((a>>1)&B1);
    }
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
      buffer64256[(x>>3) + y * (SKAARHOJOLED64x256_LCDWIDTH >> 3)] |= B10000000 >> (x & 7);
    else
      buffer64256[(x>>3) + y * (SKAARHOJOLED64x256_LCDWIDTH >> 3)] &= ~(B10000000 >> (x & 7));
  }
}

// Empty constructor.
SkaarhojOLED64x256::SkaarhojOLED64x256() : Adafruit_GFX(SKAARHOJOLED64x256_LCDWIDTH, SKAARHOJOLED64x256_LCDHEIGHT) {}

void SkaarhojOLED64x256::begin(uint8_t address, uint8_t cs) {
// NOTE: Wire.h should definitely be initialized at this point! (Wire.begin())

#if defined(ARDUINO_SKAARDUINO_V1) || defined(ARDUINO_SKAARDUINO_DUE)
  SPI.begin();
#endif
  _zoom2Xenabled = false;
  _boardAddress = 88 | (address & B111); // 0-7

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
  _vcc = false;
  _rst = true;
  writeControlPins();
  delay(1);

  // bring reset low
  _rst = false;
  writeControlPins();

  // wait 10ms
  delay(10);

  // bring out of reset
  _rst = true;
  writeControlPins();

  // Command Lock:
  sendCommand(0xFD, cs);
  sendData(0x12, cs);

  // Sleep in:
  sendCommand(0xAE, cs);

  // Set Display Clock Divide Ratio / Oscillator Frequency:
  sendCommand(0xB3, cs);
  sendData(0x91, cs);

  // Multiplex Ratio:
  sendCommand(0xCA, cs);
  sendData(0x3F, cs); // 128 MUX

  // Display Offset:
  sendCommand(0xA2, cs);
  sendData(0x00, cs);

  // Display Start Line:
  sendCommand(0xA1, cs);
  sendData(0x00, cs);

  // Set Re-Map & Dual COM Line Mode:
  sendCommand(0xA0, cs);
  sendData(0x10, cs);
  sendData(0x11, cs);

  // Set GPIO
  sendCommand(0xB5, cs);
  sendData(0x00, cs);

  // Function Selection
  sendCommand(0xAB, cs);
  sendData(0x01, cs);

  // Enable External VSL:
  sendCommand(0xB4, cs);
  sendData(0xA0, cs);
  sendData(0xFD, cs);

  // Set Contrast and Current:
  sendCommand(0xC1, cs);
  sendData(0x8F, cs);

  // Master Contrast Current Control:
  sendCommand(0xC7, cs);
  sendData(0x0F, cs);

  // Set Default Linear Gray Scale Table:
  sendCommand(0xB9, cs);

  // Set Phase Length:
  sendCommand(0xB1, cs);
  sendData(0xE2, cs);

  // Enhance Driving Scheme Capability:
  sendCommand(0xD1, cs);
  sendData(0x82, cs);
  sendData(0x20, cs);

  // Set Pre-Charge Voltage:
  sendCommand(0xBB, cs);
  sendData(0x1F, cs);

  // Set Second Pre-Charge Period:
  sendCommand(0xB6, cs);
  sendData(0x08, cs);

  // Set VCOMH Deselect Level:
  sendCommand(0xBE, cs);
  sendData(0x07, cs);

  // Set Display Mode
  sendCommand(0xA6, cs);

  // Turn on VCC:
  setVCC(true);
  delay(100); // Waiting for VCC to be stable. with a 1K load it was after 40 ms.

  // Sleep Out, Display On Command:
  sendCommand(0xAF, cs);
}

void SkaarhojOLED64x256::sendCommand(uint8_t c, uint8_t cs) {

  chipSelect(0);
  setDC(false);
  chipSelect(cs);

  fastSPIwrite(c);

  chipSelect(0);
}
void SkaarhojOLED64x256::sendData(uint8_t c, uint8_t cs) {

  chipSelect(0);
  setDC(true);
  chipSelect(cs);

  fastSPIwrite(c);

  chipSelect(0);
}

// clear everything
void SkaarhojOLED64x256::clearDisplay(void) { memset(buffer64256, 0, (SKAARHOJOLED64x256_LCDWIDTH * SKAARHOJOLED64x256_LCDHEIGHT / 8)); }

void SkaarhojOLED64x256::invertDisplay(bool i, uint8_t cs) {
  if (i) {
    sendCommand(0xA7, cs); // Inverted display
  } else {
    sendCommand(0xA6, cs); // Normal display
  }
}

void SkaarhojOLED64x256::display(uint8_t cs, uint8_t x, uint8_t y, uint16_t w, uint8_t h) {
  uint8_t nx = x;
  uint8_t ny = y;

  if (getRotation()==2) { // support rotation to vertical (1+3) also some day...
    nx = SKAARHOJOLED64x256_LCDWIDTH - w - nx;
    ny = SKAARHOJOLED64x256_LCDHEIGHT - h - ny;
  }
  nx = (nx>>2);

  sendCommand(0x15, cs); // Column reset:
  sendData(28 + nx, cs);      // 0
  sendData(28 + nx+(w>>2)-1, cs); // 	(For each "column" here there are always 4 pixels and therefore two bytes to define it which is why we add 63 and not 255)

  sendCommand(0x75, cs); // Row reset:
  sendData(ny, cs);       // 0
  sendData(ny+h-1, cs);      // 63

  sendCommand(0x5C, cs); // Enable write to data ram

  // SPI
  chipSelect(0);
  setDC(true);
  chipSelect(cs);

  for(uint16_t row=ny; row<ny+h; row++)  {
    for(uint16_t col=nx; col<nx+(w>>2); col++) {
      uint16_t i = (row<<5)+(col>>1);
      if (col & B1)  {
        fastSPIwrite((buffer64256[i] & B0010 ? 0xF : 0x0) | (buffer64256[i] & B0001 ? 0xF : 0x0) << 4); // Third, Forth
        fastSPIwrite((buffer64256[i] & B1000 ? 0xF : 0x0) | (buffer64256[i] & B0100 ? 0xF : 0x0) << 4); // First, Second
      } else {
        // Writing 8 pixels in one go:
        fastSPIwrite((buffer64256[i] & B00100000 ? 0xF : 0x0) | (buffer64256[i] & B00010000 ? 0xF : 0x0) << 4); // Third, Forth
        fastSPIwrite((buffer64256[i] & B10000000 ? 0xF : 0x0) | (buffer64256[i] & B01000000 ? 0xF : 0x0) << 4); // First, Second        
      }
    }
  }

  chipSelect(0);
}

inline void SkaarhojOLED64x256::fastSPIwrite(uint8_t d) {

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

void SkaarhojOLED64x256::chipSelect(uint8_t cs) {
  _cs = cs;

  writeControlPins();
}

void SkaarhojOLED64x256::setDC(bool dc) {
  _dc = dc;
  writeControlPins();
}

void SkaarhojOLED64x256::setVCC(bool vcc) {
  _vcc = vcc;
  writeControlPins();
}

void SkaarhojOLED64x256::writeControlPins() {
  Wire.beginTransmission(_boardAddress);
  Wire.write(((uint8_t)~_cs));
  Wire.write((_rst << 7) | (_dc << 6) | (_vcc << 5));
  Wire.endTransmission();
}
void SkaarhojOLED64x256::zoom2x(bool enable) { _zoom2Xenabled = enable; }