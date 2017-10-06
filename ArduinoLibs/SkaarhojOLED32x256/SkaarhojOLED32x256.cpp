/*********************************************************************
This library is inspired by the Adafruit_SSD1306 library from Adafruit
written by Limor Fried/Ladyada  for Adafruit Industries. The original
library is BSD licensed.

Just like SKAARHOJ, Adafruit also invests time and resources providing
open source code, so SKAARHOJ encourage you to support Adafruit and
open-source hardware by purchasing products from Adafruit as well!
*********************************************************************/

//  #include "SkaarhojPgmspace.h"  - 23/2 2014
#include "SkaarhojOLED32x256.h"
#include "Streaming.h"

#ifdef __arm__ // Arduino Due:
#define _BV(bit) (1 << (bit))
#endif

// The memory buffer for the LCD
static uint8_t buffer32256[SKAARHOJOLED32x256_LCDHEIGHT * SKAARHOJOLED32x256_LCDWIDTH / 8];

// The most basic function, set a single pixel
void SkaarhojOLED32x256::drawPixel(int16_t x, int16_t y, uint16_t color) {
  if ((x < 0) || (x >= width()) || (y < 0) || (y >= height()))
    return;

  if ((bbox_width > 0 && x >= bbox_width) || (bbox_height > 0 && y >= bbox_height))
    return;

  x += bbox_x;
  y += bbox_y;

  uint8_t ox = x;
  uint8_t oy = y;
  for (uint8_t a = 0; a < (_zoom2Xenabled ? 4 : 1); a++) {
    if (_zoom2Xenabled) {
      x = ox * 2 + (a & B1);
      y = oy * 2 + ((a >> 1) & B1);
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

    if ((color == WHITE)^_invPixels) {
      buffer32256[y * SKAARHOJOLED32x256_LCDHEIGHT + x / 8] |= 1 << (x & 0x7);
    } else {
      buffer32256[y * SKAARHOJOLED32x256_LCDHEIGHT + x / 8] &= ~(1 << (x & 0x7));
    }
  }
}

// Empty constructor.
SkaarhojOLED32x256::SkaarhojOLED32x256() : Adafruit_GFX(SKAARHOJOLED32x256_LCDWIDTH, SKAARHOJOLED32x256_LCDHEIGHT) {}

void SkaarhojOLED32x256::begin(uint8_t address, uint8_t cs, uint8_t base, bool PCA9672) {
// NOTE: Wire.h should definitely be initialized at this point! (Wire.begin())

#if defined(ARDUINO_SKAARDUINO_V1) || defined(ARDUINO_SKAARDUINO_DUE)
  SPI.begin();
#endif
  _zoom2Xenabled = false;
  _boardAddress = base | (address & B111); // 0-7
  _PCA9672 = PCA9672;

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
  sendCommand(0x12, cs);

  // Display OFF:
  sendCommand(0xAE, cs);

  // Set column address
  sendCommand(0x15, cs);
  sendCommand(0x00, cs);
  sendCommand(0x75, cs);

  // Set row address
  sendCommand(0x75, cs);
  sendCommand(0x00, cs);
  sendCommand(0x1F, cs);

  // Set contrast current
  sendCommand(0x81, cs);
  sendCommand(0x27, cs);

  // Set current range
  sendCommand(0x87, cs);

  // Set re-map and gray scale/mono mode
  sendCommand(0xA0, cs);
  sendCommand(0b10010, cs);

  // Set display start line
  sendCommand(0xA1, cs);
  sendCommand(0x00, cs);

  // Set display offset
  sendCommand(0xA2, cs);
  sendCommand(0x00, cs);

  // Set MUX ratio
  sendCommand(0xA8, cs);
  sendCommand(0x1F, cs);

  // Set phase length
  sendCommand(0xB1, cs);
  sendCommand(0x71, cs);

  // Set front clock divider
  sendCommand(0xB3, cs);
  sendCommand(0xF0, cs);

  // Select default linear gray scale table
  sendCommand(0xB7, cs);

  // Set pre charge
  sendCommand(0xBB, cs);
  sendCommand(0x35, cs);
  sendCommand(0xFF, cs);

  // Set pre-charge voltage
  sendCommand(0xBC, cs);
  sendCommand(0x1F, cs);

  // Set VCOMH
  sendCommand(0xBE, cs);
  sendCommand(0x0F, cs);

  // Display ON
  sendCommand(0xAF, cs);
  
  clearDisplay();
  display(cs);

  // Turn on VCC:
  setVCC(true);
  delay(100); // Waiting for VCC to be stable. with a 1K load it was after 40 ms.

  // Sleep Out, Display On Command:
  sendCommand(0xAF, cs);
}

void SkaarhojOLED32x256::sendCommand(uint8_t c, uint8_t cs) {

  chipSelect(0);
  setDC(false);
  chipSelect(cs);

  fastSPIwrite(c);

  chipSelect(0);
}
void SkaarhojOLED32x256::sendData(uint8_t c, uint8_t cs) {

  chipSelect(0);
  setDC(true);
  chipSelect(cs);

  fastSPIwrite(c);

  chipSelect(0);
}

// clear everything
void SkaarhojOLED32x256::clearDisplay(void) { memset(buffer32256, 0, (SKAARHOJOLED32x256_LCDWIDTH * SKAARHOJOLED32x256_LCDHEIGHT / 8)); }

void SkaarhojOLED32x256::invertDisplay(bool i, uint8_t cs) {
  if (i) {
    sendCommand(0xA7, cs); // Inverted display
  } else {
    sendCommand(0xA4, cs); // Normal display
  }
}

void SkaarhojOLED32x256::display(uint8_t cs, uint8_t x, uint8_t y, uint16_t w, uint8_t h) {
  uint8_t nx = x & 0xF8;
  uint8_t ny = y;

  if (getRotation()==2) { // support rotation to vertical (1+3) also some day...
    nx = SKAARHOJOLED32x256_LCDWIDTH - w - nx;
    ny = SKAARHOJOLED32x256_LCDHEIGHT - h - ny;
  }
  nx = (nx>>3);

  sendCommand(0x15, cs); // Column reset:
  sendCommand(0 + nx, cs);      // 0
  sendCommand(0 + nx+(w>>3)-1, cs); //

  sendCommand(0x75, cs); // Row reset:
  sendCommand(ny, cs);       // 0
  sendCommand(ny+h-1, cs);      // 31

  // SPI
  chipSelect(0);
  setDC(true);
  chipSelect(cs);

  for(uint16_t row=ny; row<ny+h; row++)  {
    for(uint16_t col=nx; col<nx+(w>>3); col++) {
      uint16_t i = (row<<5)+col;
      fastSPIwrite(buffer32256[i]);
    }
  }

  chipSelect(0);
}

inline void SkaarhojOLED32x256::fastSPIwrite(uint8_t d) {

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

void SkaarhojOLED32x256::chipSelect(uint8_t cs) {
  _cs = cs;

  writeControlPins();
}

void SkaarhojOLED32x256::setDC(bool dc) {
  _dc = dc;
  writeControlPins();
}

void SkaarhojOLED32x256::setVCC(bool vcc) {
  _vcc = vcc;
  writeControlPins();
}

void SkaarhojOLED32x256::writeControlPins() {
  Wire.beginTransmission(_boardAddress);
  if (_PCA9672) {
    Wire.write((((uint8_t)~_cs) & 0x1F) | (_rst << 7) | (_dc << 6) | (_vcc << 5));
  } else {
    Wire.write(((uint8_t)~_cs) & 0x1F);
    Wire.write((_rst << 7) | (_dc << 6) | (_vcc << 5));
  }
  Wire.endTransmission();
}
void SkaarhojOLED32x256::zoom2x(bool enable) { _zoom2Xenabled = enable; }