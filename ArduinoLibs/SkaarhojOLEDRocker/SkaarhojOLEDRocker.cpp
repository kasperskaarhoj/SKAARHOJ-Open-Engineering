/*********************************************************************
This library is inspired by the Adafruit_SSD1306 library from Adafruit
written by Limor Fried/Ladyada  for Adafruit Industries. The original
library is BSD licensed.

Just like SKAARHOJ, Adafruit also invests time and resources providing
open source code, so SKAARHOJ encourage you to support Adafruit and
open-source hardware by purchasing products from Adafruit as well!
*********************************************************************/

#include "SkaarhojOLEDRocker.h"

// The memory buffer for the LCD
static uint8_t buffer9664[SKAARHOJOLEDRocker_LCDWIDTH * SKAARHOJOLEDRocker_LCDHEIGHT / 8];

// The most basic function, set a single pixel
void SkaarhojOLEDRocker::drawPixel(int16_t x, int16_t y, uint16_t color) {
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
    buffer9664[x / 8 + y * SKAARHOJOLEDRocker_LCDWIDTH / 8] |= B10000000 >> (x % 8);
  else
    buffer9664[x / 8 + y * SKAARHOJOLEDRocker_LCDWIDTH / 8] &= ~(B10000000 >> (x % 8));
}

// Empty constructor.
SkaarhojOLEDRocker::SkaarhojOLEDRocker() : Adafruit_GFX(SKAARHOJOLEDRocker_LCDWIDTH, SKAARHOJOLEDRocker_LCDHEIGHT) {}

void SkaarhojOLEDRocker::begin(uint8_t address) {
  // NOTE: Wire.h should definitely be initialized at this point! (Wire.begin())

  _boardAddress = 88 | (address & B111); // 0-7
#if defined(ARDUINO_SKAARDUINO_V1) || defined(ARDUINO_SKAARDUINO_DUE)
  SPI.begin();
#endif

  // Set SPI pins up:
  _clockPin = 48;
  _dataPin = 49;

  _readButtonPressNum = 0;

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
  _rst = false;
  writeControlPins();
  delay(1000);

  // bring VAH high:
  _vcc = true;
  writeControlPins();

  // wait 10ms
  delay(2);

  // bring out of reset
  _rst = true;
  writeControlPins();
  delay(2);

  // Init sequence for OLED module
  uint8_t cs = 1;

  // Software Reset:
  chipSelect(cs);
  sendCommand(0x01);
  chipSelect(0);

  // Reserved 1-3:
  chipSelect(cs);
  sendCommand(0x10);
  sendData(0x03);
  chipSelect(0);

  chipSelect(cs);
  sendCommand(0x12);
  sendData(0x63);
  chipSelect(0);

  chipSelect(cs);
  sendCommand(0x13);
  sendData(0x00);
  chipSelect(0);

  // Dot Matrix Display StandBy ON/OFF:
  chipSelect(cs);
  sendCommand(0x14);
  sendData(0x00);
  chipSelect(0);

  // Reserved 4-8
  chipSelect(cs);
  sendCommand(0x16);
  sendData(0x00);
  chipSelect(0);

  chipSelect(cs);
  sendCommand(0x18);
  sendData(0x09);
  chipSelect(0);

  // Reserved 9
  chipSelect(cs);
  sendCommand(0x48);
  sendData(0x03);
  chipSelect(0);

  // System Clock Division Ratio Setting
  chipSelect(cs);
  sendCommand(0xD0);
  sendData(0x80);
  chipSelect(0);

  // Reserved 11
  chipSelect(cs);
  sendCommand(0xDD);
  sendData(0x88);
  chipSelect(0);

  // Dot Matrix Display ON/OFF:
  chipSelect(cs);
  sendCommand(0x02);
  sendData(0x01);
  chipSelect(0);

  delay(10);
}

void SkaarhojOLEDRocker::sendCommand(uint8_t c) {

  setDC(false);
  fastSPIwrite(c);
}
void SkaarhojOLEDRocker::sendData(uint8_t c) {

  setDC(true);
  fastSPIwrite(c);
}

// clear everything
void SkaarhojOLEDRocker::clearDisplay(void) { memset(buffer9664, 0, (SKAARHOJOLEDRocker_LCDWIDTH * SKAARHOJOLEDRocker_LCDHEIGHT / 8)); }

void SkaarhojOLEDRocker::display(uint8_t cs) {

  chipSelect(cs);
  sendCommand(0x34);
  sendData(0x00);
  chipSelect(0);

  chipSelect(cs);
  sendCommand(0x35);
  sendData(0x0B);
  chipSelect(0);

  chipSelect(cs);
  sendCommand(0x36);
  sendData(0x00);
  chipSelect(0);

  chipSelect(cs);
  sendCommand(0x37);
  sendData(0x3F);
  chipSelect(0);

  chipSelect(cs);
  sendCommand(0x08); // Enable write to data ram

  setDC(true);

  for (uint16_t i = 0; i < 768; i++) {
    // Writing 8 pixels in one go:
    fastSPIwrite(buffer9664[i]);
  }

  chipSelect(0);
}

inline void SkaarhojOLEDRocker::fastSPIwrite(uint8_t d) {
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

void SkaarhojOLEDRocker::chipSelect(uint8_t cs) {
  _cs = cs;
  writeControlPins();
}

void SkaarhojOLEDRocker::setDC(bool dc) {
  _dc = dc;
  writeControlPins();
}

void SkaarhojOLEDRocker::setVCC(bool vcc) {
  _vcc = vcc;
  writeControlPins();
}

void SkaarhojOLEDRocker::writeControlPins() {
  Wire.beginTransmission(_boardAddress);
  Wire.write((((uint8_t)~_cs) & B1111) | ((~(B1 << _readButtonPressNum)) << 4));
  Wire.write((_rst << 7) | (_dc << 6) | (_vcc << 5) | B11111);
  Wire.endTransmission();
}

/**
 *  Returns true if a button 1-4, index 0-2 is has just been released
 */
bool SkaarhojOLEDRocker::buttonUp(uint8_t buttonNumber, uint8_t buttonIndex) {
  if (_validButtonNumber(buttonNumber) && buttonIndex < 3) {
    _readButtonStatus(buttonNumber - 1);

    uint8_t mask = (B1 << buttonIndex);
    uint8_t buttonChange = (_buttonStatusLastUp[buttonNumber - 1] ^ _buttonStatus[buttonNumber - 1]) & mask;
    _buttonStatusLastUp[buttonNumber - 1] ^= buttonChange;

    return (buttonChange & ~_buttonStatus[buttonNumber - 1]) ? true : false;
  } else
    return false;
}

/**
 *  Returns true if a button 1-4, index 0-2 is has just been pushed down
 */
bool SkaarhojOLEDRocker::buttonDown(uint8_t buttonNumber, uint8_t buttonIndex) {
  if (_validButtonNumber(buttonNumber) && buttonIndex < 3) {
    _readButtonStatus(buttonNumber - 1);

    uint8_t mask = (B1 << buttonIndex);
    uint8_t buttonChange = (_buttonStatusLastDown[buttonNumber - 1] ^ _buttonStatus[buttonNumber - 1]) & mask;
    _buttonStatusLastDown[buttonNumber - 1] ^= buttonChange;

    return (buttonChange & _buttonStatus[buttonNumber - 1]) ? true : false;
  } else
    return false;
}

/**
 *  Returns true if a button 1-4, index 0-2 is currently pressed
 */
bool SkaarhojOLEDRocker::buttonIsPressed(uint8_t buttonNumber, uint8_t buttonIndex) {
  if (_validButtonNumber(buttonNumber) && buttonIndex < 3) {
    return isButtonIndexIn(buttonIndex, buttonIsPressedAll(buttonNumber));
  } else
    return false;
}
bool SkaarhojOLEDRocker::buttonIsHeldFor(uint8_t buttonNumber, uint8_t buttonIndex, uint16_t timeout) {
  if (_validButtonNumber(buttonNumber) && buttonIndex < 3) {
    if (isButtonIndexIn(buttonIndex, buttonIsPressedAll(buttonNumber))) {
      if (_inputChangeTimeActive) {
        if ((unsigned long)(_inputChangeTime + timeout) < (unsigned long)millis()) {
          _inputChangeTimeActive = false; // No more triggers until we have a new change
          return true;
        }
      }
    }
  }
  return false;
}
bool SkaarhojOLEDRocker::buttonIsReleasedAgo(uint8_t buttonNumber, uint8_t buttonIndex, uint16_t timeout) {
  if (_validButtonNumber(buttonNumber) && buttonIndex < 3) {
    if (isButtonIndexIn(buttonIndex, _lastButtonReleased[buttonNumber - 1])) {
      if (_inputChangeTimeActive) {
        if ((unsigned long)(_inputChangeTime + timeout) < (unsigned long)millis()) {
          _inputChangeTimeActive = false; // No more triggers until we have a new change
          return true;
        }
      }
    }
  }
  return false;
}

/**
 *  Returns a byte where each bit indicates if a button 1-8 (bits 0-7) has been released since last check
 */
uint8_t SkaarhojOLEDRocker::buttonUpAll(uint8_t buttonNumber) {
  if (_validButtonNumber(buttonNumber)) {
    _readButtonStatus(buttonNumber - 1);

    uint8_t buttonChange = _buttonStatusLastUp[buttonNumber - 1] ^ _buttonStatus[buttonNumber - 1];
    _buttonStatusLastUp[buttonNumber - 1] = _buttonStatus[buttonNumber - 1];

    return buttonChange & ~_buttonStatus[buttonNumber - 1];
  }
}

/**
 *  Returns a byte where each bit indicates if a button 1-8 (bits 0-7) has been pressed since last check
 */
uint8_t SkaarhojOLEDRocker::buttonDownAll(uint8_t buttonNumber) {
  if (_validButtonNumber(buttonNumber)) {
    _readButtonStatus(buttonNumber - 1);

    uint8_t buttonChange = _buttonStatusLastDown[buttonNumber - 1] ^ _buttonStatus[buttonNumber - 1];
    _buttonStatusLastDown[buttonNumber - 1] = _buttonStatus[buttonNumber - 1];

    return buttonChange & _buttonStatus[buttonNumber - 1];
  }
}

/**
 *  Returns a byte where each bit indicates if a button 1-8 (bits 0-7) is currently pressed
 */
uint8_t SkaarhojOLEDRocker::buttonIsPressedAll(uint8_t buttonNumber) {
  if (_validButtonNumber(buttonNumber)) {
    _readButtonStatus(buttonNumber - 1);
  }

  return _buttonStatus[buttonNumber - 1];
}

/**
 *
 */
bool SkaarhojOLEDRocker::isButtonIndexIn(uint8_t buttonIndex, uint8_t allButtonsState) {
  if (buttonIndex < 3) {
    return (allButtonsState & (B1 << buttonIndex)) ? true : false;
  }
  return false;
}

// Private methods:
void SkaarhojOLEDRocker::_readButtonStatus(uint8_t buttonNum) {
  if (buttonNum < 4) {
    _buttonStatus[buttonNum] = readButton(buttonNum);

    if (_buttonStatus[buttonNum] != _lastButtonStatus[buttonNum]) {
      _lastButtonReleased[buttonNum] = (_lastButtonStatus[buttonNum] ^ _buttonStatus[buttonNum]) & ~_buttonStatus[buttonNum];
      _lastButtonStatus[buttonNum] = _buttonStatus[buttonNum];
      _inputChangeTime = millis();
      _inputChangeTimeActive = true; // Set true if it's ok to act on a trigger
    }
  }
}
uint8_t SkaarhojOLEDRocker::readButton(uint8_t buttonNum) {
  _readButtonPressNum = buttonNum & B11;
  writeControlPins();

  // Read control pins:
  Wire.requestFrom(_boardAddress, (uint8_t)2); // asking for two bytes
  Wire.read();
  return ~(Wire.read() & B111);
}

bool SkaarhojOLEDRocker::_validButtonNumber(uint8_t buttonNumber) { // Checks if a button number is valid (1-4)
  return (buttonNumber > 0) && (buttonNumber <= 4);
}

void SkaarhojOLEDRocker::testProgramme(uint8_t buttonMask) {
  static uint16_t lastTime;

  for (int i = 0; i < 4; i++) {
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
      println((uint8_t)millis(), BIN);
      setTextSize(1);
      print(millis(), DEC);
      print(F(" - "));
      print(millis(), BIN);
      display(B1 << i);

      for (uint8_t b = 0; b < 3; b++) {
        if (buttonDown(i + 1, b)) {
          Serial.print(F("Button #"));
          Serial.print(i + 1);
          Serial.print(F(", index "));
          Serial.print(b);
          Serial.println(F(" was pressed down"));
        }
        if (buttonUp(i + 1, b)) {
          Serial.print(F("Button #"));
          Serial.print(i + 1);
          Serial.print(F(", index "));
          Serial.print(b);
          Serial.println(F(" was released"));
        }

        if (buttonIsHeldFor(i + 1, b, 1000)) {
          Serial.print(F("Button #"));
          Serial.print(i + 1);
          Serial.print(F(", index "));
          Serial.print(b);
          Serial.println(F(" was held for 1000 ms"));
        }
        if (buttonIsReleasedAgo(i + 1, b, 1000)) {
          Serial.print(F("Button #"));
          Serial.print(i + 1);
          Serial.print(F(", index "));
          Serial.print(b);
          Serial.println(F(" was released 1000 ms ago"));
        }
      }
    }
  }
}
