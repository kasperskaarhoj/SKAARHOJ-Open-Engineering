/*  SkaarhojUHB Arduino library for the BI8 board from SKAARHOJ.com
    Copyright (C) 2012 Kasper Skårhøj    <kasperskaarhoj@gmail.com>

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
#include "SkaarhojUHB.h"

SkaarhojUHB::SkaarhojUHB() {}

bool SkaarhojUHB::begin(uint8_t address) {

  _boardAddress = (address & B111); // 0-7

  // Initializing:
  _buttonStatus = 0;
  _lastButtonStatus = 0;
  _buttonStatusLastUp = 0;
  _buttonStatusLastDown = 0;

  clearButtonColorCache();
  _disableColorCache = false;

  _colorBalanceRed[0] = 0;
  _colorBalanceRed[1] = 100;
  _colorBalanceRed[2] = 100;
  _colorBalanceRed[3] = 0;
  _colorBalanceRed[4] = 100;
  _colorBalanceRed[5] = 20;
  _colorBalanceRed[6] = 0;
  _colorBalanceRed[7] = 0;
  _colorBalanceRed[8] = 0;
  _colorBalanceRed[9] = 100;

  _colorBalanceGreen[0] = 0;
  _colorBalanceGreen[1] = 100;
  _colorBalanceGreen[2] = 0;
  _colorBalanceGreen[3] = 100;
  _colorBalanceGreen[4] = 100;
  _colorBalanceGreen[5] = 20;
  _colorBalanceGreen[6] = 0;
  _colorBalanceGreen[7] = 0;
  _colorBalanceGreen[8] = 0;
  _colorBalanceGreen[9] = 100;

  _defaultColorNumber = 5;

  // Create object for reading button presses
  MCP23017 buttonMux;
  _buttonMux = buttonMux;
  _buttonMux.begin(_boardAddress);

  // Create object for writing LED levels:
  PCA9685 buttonLed;
  _buttonLed = buttonLed;
  _buttonLed.begin(B111000 | _boardAddress);

  // Inputs:
  // Set everything as inputs with pull up resistors:
  _buttonMux.inputPolarityMask(65535);
  _buttonMux.internalPullupMask(65535); // All has pull-up
  _buttonMux.inputOutputMask(65535);    // All are inputs.

  _numberOfButtons = 8;

  // Outputs:
  _buttonLed.init(true);
  setButtonColorsToDefault();
}
void SkaarhojUHB::disableColorCache(bool disable) { _disableColorCache = disable; }
void SkaarhojUHB::setColorBalance(uint8_t colorNumber, uint8_t redPart, uint8_t greenPart) {
  if (_validColorNumber(colorNumber) && _validPercentage(redPart) && _validPercentage(greenPart)) {
    _colorBalanceRed[colorNumber] = redPart;
    _colorBalanceGreen[colorNumber] = greenPart;
  }
}
void SkaarhojUHB::setDefaultColor(uint8_t defaultColorNumber) {
  if (_validColorNumber(defaultColorNumber))
    _defaultColorNumber = defaultColorNumber;
}
void SkaarhojUHB::setButtonColor(uint8_t buttonNumber, uint8_t colorNumber) { _writeButtonLed(buttonNumber, colorNumber); }
void SkaarhojUHB::setButtonColorsToDefault() {
  for (uint8_t i = 1; i <= _numberOfButtons; i++) {
    _writeButtonLed(i, _defaultColorNumber);
  }
}
void SkaarhojUHB::testSequence() { testSequence(20); }
uint16_t SkaarhojUHB::testSequence(uint16_t delayTime) {
  uint16_t bDown = 0;
  // Test LEDS:
  for (uint8_t ii = 0; ii <= 9; ii++) {
    for (uint8_t i = 1; i <= _numberOfButtons; i++) {
      _writeButtonLed(i, ii);
      // Test for button press and exit if so:
      bDown = buttonDownAll();
      if (bDown) {
        return bDown;
      }
      delay(delayTime);
    }
    delay(delayTime * 3);
  }
  setButtonColorsToDefault();
  return 0;
}

bool SkaarhojUHB::buttonUp(uint8_t buttonNumber) { // Returns true if a button 1-16 is has just been released
  if (_validButtonNumber(buttonNumber)) {
    _readButtonStatus();

    uint16_t mask = (B1 << (buttonNumber - 1));
    uint16_t buttonChange = (_buttonStatusLastUp ^ _buttonStatus) & mask;
    _buttonStatusLastUp ^= buttonChange;

    return (buttonChange & ~_buttonStatus) ? true : false;
  } else
    return false;
}
bool SkaarhojUHB::buttonDown(uint8_t buttonNumber) { // Returns true if a button 1-16 is has just been pushed down
  if (_validButtonNumber(buttonNumber)) {
    _readButtonStatus();

    uint16_t mask = (B1 << (buttonNumber - 1));
    uint16_t buttonChange = (_buttonStatusLastDown ^ _buttonStatus) & mask;
    _buttonStatusLastDown ^= buttonChange;

    return (buttonChange & _buttonStatus) ? true : false;
  } else
    return false;
}
bool SkaarhojUHB::buttonIsPressed(uint8_t buttonNumber) { // Returns true if a button 1-16 is currently pressed
  if (_validButtonNumber(buttonNumber)) {
    return (buttonIsPressedAll() >> (buttonNumber - 1)) & 1 ? true : false;
  } else
    return false;
}
bool SkaarhojUHB::buttonIsHeldFor(uint8_t buttonNumber, uint16_t timeout) {
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
bool SkaarhojUHB::buttonIsReleasedAgo(uint8_t buttonNumber, uint16_t timeout) {
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

uint16_t SkaarhojUHB::buttonUpAll() { // Returns a word where each bit indicates if a button 1-16 (bits 0-9) has been released since last check
  _readButtonStatus();

  uint16_t buttonChange = _buttonStatusLastUp ^ _buttonStatus;
  _buttonStatusLastUp = _buttonStatus;

  return buttonChange & ~_buttonStatus;
}
uint16_t SkaarhojUHB::buttonDownAll() { // Returns a word where each bit indicates if a button 1-16 (bits 0-9) has been pressed since last check
  _readButtonStatus();

  uint16_t buttonChange = _buttonStatusLastDown ^ _buttonStatus;
  _buttonStatusLastDown = _buttonStatus;

  return buttonChange & _buttonStatus;
}
uint16_t SkaarhojUHB::buttonIsPressedAll() { // Returns a word where each bit indicates if a button 1-16 (bits 0-9) is currently pressed since last check
  _readButtonStatus();

  return _buttonStatus;
}
bool SkaarhojUHB::isButtonIn(uint8_t buttonNumber, uint16_t allButtonsState) {
  if (_validButtonNumber(buttonNumber)) {
    return (allButtonsState & (B1 << (buttonNumber - 1))) ? true : false;
  }
  return false;
}

// Private methods:

void SkaarhojUHB::_writeButtonLed(uint8_t buttonNumber, uint8_t color) {
  if (_validColorNumber(color) && _validButtonNumber(buttonNumber) && ((_buttonColorCache[(buttonNumber - 1)] != color) || _disableColorCache)) {
    _buttonColorCache[(buttonNumber - 1)] = color;

    _buttonLed.setLEDDimmed((buttonNumber - 1) * 2, 100-_colorBalanceRed[color]);
    _buttonLed.setLEDDimmed((buttonNumber - 1) * 2 + 1, 100-_colorBalanceGreen[color]);
  }
}
void SkaarhojUHB::_readButtonStatus() { // Reads button status from MCP23017 chip.
  uint16_t buttonStatus = _buttonMux.digitalWordRead();
  _buttonStatus = buttonStatus >> 8;

  if (_buttonStatus != _lastButtonStatus) {
    _lastButtonReleased = (_lastButtonStatus ^ _buttonStatus) & ~_buttonStatus;
    _lastButtonStatus = _buttonStatus;
    _inputChangeTime = millis();
    _inputChangeTimeActive = true; // Set true if it's ok to act on a trigger
  }
}

bool SkaarhojUHB::_validButtonNumber(uint8_t buttonNumber) { // Checks if a button number is valid (1-16)
  return (buttonNumber >= 1 && buttonNumber <= _numberOfButtons);
}
bool SkaarhojUHB::_validColorNumber(uint8_t colorNumber) { // Checks if a color number is valid (1-10)
  return (colorNumber <= 9);
}
bool SkaarhojUHB::_validPercentage(uint8_t percentage) { // Checks if a value is within 0-100
  return (percentage <= 100);
}

void SkaarhojUHB::testProgramme(uint16_t buttonMask) {

  uint16_t allButtons = buttonDownAll();
  for (uint16_t i = 0; i < 8; i++) {
    if ((allButtons >> i & 1) > 0) {
      Serial.print(F("Button #"));
      Serial.print(i + 1);
      Serial.print(F(" pressed\n"));
    }
  }

  while (true) {
    test_button = (test_button + 1) % 8;
    if (!test_button) {
      test_color = (test_color + 1) % 6;
    }

    if ((buttonMask & ((uint16_t)B1 << test_button)) > 0) {
      _writeButtonLed(test_button + 1, test_color);
      break;
    }
  }
}

void SkaarhojUHB::clearButtonColorCache() {
  for (uint8_t i = 0; i < 8; i++) {
    _buttonColorCache[i] = 255;
  }
}