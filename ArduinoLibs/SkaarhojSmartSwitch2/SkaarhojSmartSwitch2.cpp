/*  SkaarhojSmartSwitch2 Arduino library for the BI8 board from SKAARHOJ.com
    Copyright (C) 2012 Kasper Skårhøj    <kasperskaarhoj@gmail.com>
        Copyright (C) 2012 Filip Sandborg-Olsen   <filipsandborg@me.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "SkaarhojSmartSwitch2.h"

#ifdef __arm__ // Arduino Due:
#define _BV(bit) (1 << (bit))
#endif

// The memory buffer for the LCD
static uint8_t bufferSSW[SKAARHOJSMARTSWITCH_LCDWIDTH * SKAARHOJSMARTSWITCH_LCDHEIGHT / 8];

// The most basic function, set a single pixel
void SkaarhojSmartSwitch2::drawPixel(int16_t x, int16_t y, uint16_t color) {
  if ((bbox_width > 0 && x >= bbox_width) || (bbox_height > 0 && y >= bbox_height))
    return;

  x += bbox_x;
  y += bbox_y;

  if ((x < 0) || (x >= width()) || (y < 0) || (y >= height()))
    return;

  if (color == 65535) { // INVERSION MODE
    color = getPixel(x, y) ? 0 : 1;
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
  if ((color > 0)^_invPixels)
    bufferSSW[(7 - x / 8) + (y * SKAARHOJSMARTSWITCH_LCDWIDTH / 8)] |= _BV((x % 8));
  else
    bufferSSW[(7 - x / 8) + (y * SKAARHOJSMARTSWITCH_LCDWIDTH / 8)] &= ~_BV((x % 8));
}

bool SkaarhojSmartSwitch2::getPixel(int16_t x, int16_t y) {
  if ((x < 0) || (x >= width()) || (y < 0) || (y >= height()))
    return false;

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
  return bufferSSW[(7 - x / 8) + (y * SKAARHOJSMARTSWITCH_LCDWIDTH / 8)] & _BV((x % 8));
}

// Empty constructor.
SkaarhojSmartSwitch2::SkaarhojSmartSwitch2() : Adafruit_GFX(SKAARHOJSMARTSWITCH_LCDWIDTH, SKAARHOJSMARTSWITCH_LCDHEIGHT) {}

void SkaarhojSmartSwitch2::begin(uint8_t address) {
  // NOTE: Wire.h should definitely be initialized at this point! (Wire.begin())
  _boardAddress = (address & B111); // 0-3

#if defined(ARDUINO_SKAARDUINO_V1) || defined(ARDUINO_SKAARDUINO_DUE)
  SPI.begin();
#endif

  // Initializing:
  _buttonStatus = 0;
  _lastButtonStatus = 0;
  _buttonStatusLastUp = 0;
  _buttonStatusLastDown = 0;

  _clockPin = 48;
  _dataPin = 49;

  pinMode(_clockPin, OUTPUT);
  pinMode(_dataPin, OUTPUT);

  // Create object for reading button presses
  MCP23017 buttonMux;
  _buttonMux = buttonMux;
  _buttonMux.begin((int)(_boardAddress));

  _buttonMux.internalPullupMask(65535);    // Set pull-ups
  _buttonMux.inputOutputMask(65535 - 255); // Set up input and output pins
  _buttonMux.inputPolarityMask(65535);     // Reverse polarity for inputs.
}

/**
 * red, green, blue: 0-3
 */
void SkaarhojSmartSwitch2::setButtonColor(uint8_t red, uint8_t green, uint8_t blue, uint8_t cs) { _writeCommand(0x40, (red & B11) << 6 | (green & B11) << 4 | (blue & B11) << 2 | B11, cs); }

/**
 * brightness: 0-7
 */
void SkaarhojSmartSwitch2::setButtonBrightness(uint8_t brightness, uint8_t cs) {
  _writeCommand(0x41, (brightness << 5) | B11111, cs); // 3 MSB
}

/**
 *  Returns true if a button 1-8 is has just been released
 */
bool SkaarhojSmartSwitch2::buttonUp(uint8_t buttonNumber) {
  if (_validButtonNumber(buttonNumber)) {
    _readButtonStatus();

    uint8_t mask = (B1 << (buttonNumber - 1));
    uint8_t buttonChange = (_buttonStatusLastUp ^ _buttonStatus) & mask;
    _buttonStatusLastUp ^= buttonChange;

    return (buttonChange & ~_buttonStatus) ? true : false;
  } else
    return false;
}

/**
 *  Returns true if a button 1-8 is has just been pushed down
 */
bool SkaarhojSmartSwitch2::buttonDown(uint8_t buttonNumber) {
  if (_validButtonNumber(buttonNumber)) {
    _readButtonStatus();

    uint8_t mask = (B1 << (buttonNumber - 1));
    uint8_t buttonChange = (_buttonStatusLastDown ^ _buttonStatus) & mask;
    _buttonStatusLastDown ^= buttonChange;

    return (buttonChange & _buttonStatus) ? true : false;
  } else
    return false;
}

/**
 *  Returns true if a button 1-8 is currently pressed
 */
bool SkaarhojSmartSwitch2::buttonIsPressed(uint8_t buttonNumber) {
  if (_validButtonNumber(buttonNumber)) {
    _readButtonStatus();
    return (SkaarhojSmartSwitch2::buttonIsPressedAll() >> (buttonNumber - 1)) & 1 ? true : false;
  } else
    return false;
}
bool SkaarhojSmartSwitch2::buttonIsHeldFor(uint8_t buttonNumber, uint16_t timeout) {
  if (_validButtonNumber(buttonNumber)) {
    if ((buttonIsPressedAll() >> (buttonNumber - 1)) & 1) {
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
bool SkaarhojSmartSwitch2::buttonIsReleasedAgo(uint8_t buttonNumber, uint16_t timeout) {
  if (_validButtonNumber(buttonNumber)) {
    if (isButtonIn(buttonNumber, _lastButtonReleased)) {
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
uint8_t SkaarhojSmartSwitch2::buttonUpAll() {
  _readButtonStatus();

  uint8_t buttonChange = _buttonStatusLastUp ^ _buttonStatus;
  _buttonStatusLastUp = _buttonStatus;

  return buttonChange & ~_buttonStatus;
}

/**
 *  Returns a byte where each bit indicates if a button 1-8 (bits 0-7) has been pressed since last check
 */
uint8_t SkaarhojSmartSwitch2::buttonDownAll() {
  _readButtonStatus();

  uint8_t buttonChange = _buttonStatusLastDown ^ _buttonStatus;
  _buttonStatusLastDown = _buttonStatus;

  return buttonChange & _buttonStatus;
}

/**
 *  Returns a byte where each bit indicates if a button 1-8 (bits 0-7) is currently pressed
 */
uint8_t SkaarhojSmartSwitch2::buttonIsPressedAll() {
  _readButtonStatus();

  return _buttonStatus;
}

/**
 *
 */
bool SkaarhojSmartSwitch2::isButtonIn(uint8_t buttonNumber, uint8_t allButtonsState) {
  if (_validButtonNumber(buttonNumber)) {
    return (allButtonsState & (B1 << (buttonNumber - 1))) ? true : false;
  }
  return false;
}

// clear everything
void SkaarhojSmartSwitch2::clearDisplay(void) { memset(bufferSSW, 0, (SKAARHOJSMARTSWITCH_LCDWIDTH * SKAARHOJSMARTSWITCH_LCDHEIGHT / 8)); }
void SkaarhojSmartSwitch2::display(uint8_t cs) {

#if defined(ARDUINO_SKAARDUINO_V1) || defined(ARDUINO_SKAARDUINO_DUE)
  SPI.beginTransaction(SPISettings(6000000, MSBFIRST, SPI_MODE1));
  _chipSelect(cs);
  SPI.transfer(0x55);

  for (uint16_t i = 0; i < (SKAARHOJSMARTSWITCH_LCDWIDTH * SKAARHOJSMARTSWITCH_LCDHEIGHT / 8); i++) {
    SPI.transfer(bufferSSW[i]);
  }
  _chipSelect(0);
  SPI.endTransaction();
#else
  _chipSelect(cs);

  shiftOut(_dataPin, _clockPin, MSBFIRST, 0x55);

  for (uint16_t i = 0; i < (SKAARHOJSMARTSWITCH_LCDWIDTH * SKAARHOJSMARTSWITCH_LCDHEIGHT / 8); i++) {
    shiftOut(_dataPin, _clockPin, MSBFIRST, bufferSSW[i]);
  }

  _chipSelect(0);
#endif
}

// Private methods:
void SkaarhojSmartSwitch2::_readButtonStatus() { // Reads button status from MCP23017 chip.
  word buttonStatus = _buttonMux.digitalWordRead();
  _buttonStatus = buttonStatus >> 8 & B11111111;

  if (_buttonStatus != _lastButtonStatus) {
    _lastButtonReleased = (_lastButtonStatus ^ _buttonStatus) & ~_buttonStatus;
    _lastButtonStatus = _buttonStatus;
    _inputChangeTime = millis();
    _inputChangeTimeActive = true; // Set true if it's ok to act on a trigger
  }
}
bool SkaarhojSmartSwitch2::_validButtonNumber(uint8_t buttonNumber) { // Checks if a button number is valid (1-8)
  return (buttonNumber >= 1 && buttonNumber <= 8);
}

/**
 * Writing command to switch:
 */
int SkaarhojSmartSwitch2::_writeCommand(int address, int value, uint8_t cs) {

#if defined(ARDUINO_SKAARDUINO_V1) || defined(ARDUINO_SKAARDUINO_DUE)
  SPI.beginTransaction(SPISettings(6000000, MSBFIRST, SPI_MODE1));
  _chipSelect(cs);

  SPI.transfer(address);
  SPI.transfer(value);

  _chipSelect(0);
  SPI.endTransaction();
#else
  _chipSelect(cs);

  shiftOut(_dataPin, _clockPin, MSBFIRST, address);
  shiftOut(_dataPin, _clockPin, MSBFIRST, value);

  _chipSelect(0);
#endif
}
void SkaarhojSmartSwitch2::_chipSelect(uint8_t cs) { _buttonMux.digitalWordWrite(255 - cs); }

void SkaarhojSmartSwitch2::testProgramme(uint8_t buttonMask) {
  static uint16_t lastTime;
  static uint8_t hasSetBackgroundColor = 0;
  static uint8_t bgColor = 0;
  static unsigned long buttonColorTimer = 0;

  for (int i = 0; i < 8; i++) {
    if (buttonMask & (B1 << i)) {

      if ((millis() & 0x1000) == 0x1000 || (millis() - buttonColorTimer < 5000)) {
        bool newColor = false;
        if (buttonDown(i + 1)) {
          Serial.print(F("Button #"));
          Serial.print(i + 1);
          Serial.print(F(" pressed\n"));
          buttonColorTimer = millis();
          newColor = true;
        }
        if (newColor || !(hasSetBackgroundColor & (B1 << i))) {
          hasSetBackgroundColor |= (B1 << i);
          bgColor = random(0, 63);
          setButtonColor(bgColor >> 4, bgColor >> 2, bgColor, B1 << i);

          clearDisplay(); // clears the screen and buffer
          setTextColor(WHITE);
          setCursor(0, 0);
          setTextSize(1);
          print(F("Display #"));
          println(i + 1);
          print(F("Col="));
          print((bgColor >> 4) & B11);
          print(F(","));
          print((bgColor >> 2) & B11);
          print(F(","));
          print((bgColor)&B11);
          display(B1 << i);
        }
      } else {
        hasSetBackgroundColor = 0;

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
        display(B1 << i);
      }
    }
  }
}
