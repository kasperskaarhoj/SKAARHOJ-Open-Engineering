/*  SkaarhojDome Arduino library for the BI8 board from SKAARHOJ.com
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
#include "SkaarhojDome.h"

SkaarhojDome::SkaarhojDome() {}

bool SkaarhojDome::begin(uint8_t LEDaddress, uint8_t buttonAddress) {

  _boardAddress = (LEDaddress & B111); // 0-7
  _buttonAddress = buttonAddress;

  // Initializing:
  _buttonStatus = 0;
  _lastButtonStatus = 0;
  _buttonStatusLastUp = 0;
  _buttonStatusLastDown = 0;
  _debounceCounter = 0;
  _buttonStatus_debounceCache = 0;

  clearButtonColorCache();
  _disableColorCache = false;

  _colorBalanceRed[0] = 0;
  _colorBalanceRed[1] = 70;
  _colorBalanceRed[2] = 70;
  _colorBalanceRed[3] = 0;
  _colorBalanceRed[4] = 20;
  _colorBalanceRed[5] = 2;
  _colorBalanceRed[6] = 0;
  _colorBalanceRed[7] = 0;
  _colorBalanceRed[8] = 0;
  _colorBalanceRed[9] = 100;

  _colorBalanceGreen[0] = 0;
  _colorBalanceGreen[1] = 70;
  _colorBalanceGreen[2] = 0;
  _colorBalanceGreen[3] = 70;
  _colorBalanceGreen[4] = 20;
  _colorBalanceGreen[5] = 2;
  _colorBalanceGreen[6] = 0;
  _colorBalanceGreen[7] = 0;
  _colorBalanceGreen[8] = 0;
  _colorBalanceGreen[9] = 100;

  _colorBalanceBlue[0] = 0;
  _colorBalanceBlue[1] = 70;
  _colorBalanceBlue[2] = 0;
  _colorBalanceBlue[3] = 0;
  _colorBalanceBlue[4] = 20;
  _colorBalanceBlue[5] = 2;
  _colorBalanceBlue[6] = 100;
  _colorBalanceBlue[7] = 50;
  _colorBalanceBlue[8] = 20;
  _colorBalanceBlue[9] = 100;

  _defaultColorNumber = 5;

  // Create object for reading button presses
  Wire.beginTransmission(_buttonAddress);
  Wire.write(255);
  Wire.endTransmission();

  // Create object for writing LED levels:
  PCA9685 buttonLed;
  _buttonLed = buttonLed;
  _buttonLed.begin(B111000 | _boardAddress);

  _numberOfButtons = 5;

  // Outputs:
  _buttonLed.init();
  setButtonColorsToDefault();
}
void SkaarhojDome::disableColorCache(bool disable) { _disableColorCache = disable; }
void SkaarhojDome::setColorBalance(uint8_t colorNumber, uint8_t redPart, uint8_t greenPart, uint8_t bluePart) {
  if (_validColorNumber(colorNumber) && _validPercentage(redPart) && _validPercentage(greenPart) && _validPercentage(bluePart)) {
    _colorBalanceRed[colorNumber] = redPart;
    _colorBalanceGreen[colorNumber] = greenPart;
    _colorBalanceGreen[colorNumber] = bluePart;
  }
}
void SkaarhojDome::setDefaultColor(uint8_t defaultColorNumber) {
  if (_validColorNumber(defaultColorNumber))
    _defaultColorNumber = defaultColorNumber;
}
void SkaarhojDome::setButtonColor(uint8_t buttonNumber, uint8_t colorNumber) { _writeButtonLed(buttonNumber, colorNumber); }
void SkaarhojDome::setButtonColorsToDefault() {
  for (uint8_t i = 1; i <= _numberOfButtons; i++) {
    _writeButtonLed(i, _defaultColorNumber);
  }
}
void SkaarhojDome::testSequence() { testSequence(20); }
uint8_t SkaarhojDome::testSequence(uint16_t delayTime) {
  uint8_t bDown = 0;
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

bool SkaarhojDome::buttonUp(uint8_t buttonNumber) { // Returns true if a button 1-16 is has just been released
  if (_validButtonNumber(buttonNumber)) {
    _readButtonStatus();

    uint8_t mask = (B1 << (buttonNumber - 1));
    uint8_t buttonChange = (_buttonStatusLastUp ^ _buttonStatus) & mask;
    _buttonStatusLastUp ^= buttonChange;

    return (buttonChange & ~_buttonStatus) ? true : false;
  } else
    return false;
}
bool SkaarhojDome::buttonDown(uint8_t buttonNumber) { // Returns true if a button 1-16 is has just been pushed down
  if (_validButtonNumber(buttonNumber)) {
    _readButtonStatus();

    uint8_t mask = (B1 << (buttonNumber - 1));
    uint8_t buttonChange = (_buttonStatusLastDown ^ _buttonStatus) & mask;
    _buttonStatusLastDown ^= buttonChange;

    return (buttonChange & _buttonStatus) ? true : false;
  } else
    return false;
}
bool SkaarhojDome::buttonIsPressed(uint8_t buttonNumber) { // Returns true if a button 1-16 is currently pressed
  if (_validButtonNumber(buttonNumber)) {
    return (buttonIsPressedAll() >> (buttonNumber - 1)) & 1 ? true : false;
  } else
    return false;
}
bool SkaarhojDome::buttonIsHeldFor(uint8_t buttonNumber, uint16_t timeout) {
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
bool SkaarhojDome::buttonIsReleasedAgo(uint8_t buttonNumber, uint16_t timeout) {
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

uint8_t SkaarhojDome::buttonUpAll() { // Returns a word where each bit indicates if a button 1-16 (bits 0-9) has been released since last check
  _readButtonStatus();

  uint8_t buttonChange = _buttonStatusLastUp ^ _buttonStatus;
  _buttonStatusLastUp = _buttonStatus;

  return buttonChange & ~_buttonStatus;
}
uint8_t SkaarhojDome::buttonDownAll() { // Returns a word where each bit indicates if a button 1-16 (bits 0-9) has been pressed since last check
  _readButtonStatus();

  uint8_t buttonChange = _buttonStatusLastDown ^ _buttonStatus;
  _buttonStatusLastDown = _buttonStatus;

  return buttonChange & _buttonStatus;
}
uint8_t SkaarhojDome::buttonIsPressedAll() { // Returns a word where each bit indicates if a button 1-16 (bits 0-9) is currently pressed since last check
  _readButtonStatus();

  return _buttonStatus;
}
bool SkaarhojDome::isButtonIn(uint8_t buttonNumber, uint8_t allButtonsState) {
  if (_validButtonNumber(buttonNumber)) {
    return (allButtonsState & (B1 << (buttonNumber - 1))) ? true : false;
  }
  return false;
}

// Private methods:

void SkaarhojDome::_writeButtonLed(uint8_t buttonNumber, uint8_t color) {
  if (_validColorNumber(color) && _validButtonNumber(buttonNumber) && ((_buttonColorCache[(buttonNumber - 1)] != color) || _disableColorCache)) {
    _buttonColorCache[(buttonNumber - 1)] = color;

    _buttonLed.setLEDDimmed((buttonNumber - 1) * 3 + 1, _colorBalanceGreen[color]);
    _buttonLed.setLEDDimmed((buttonNumber - 1) * 3 + 2, _colorBalanceBlue[color]);
    _buttonLed.setLEDDimmed((buttonNumber - 1) * 3 + 3, _colorBalanceRed[color]);
  }
}
void SkaarhojDome::_readButtonStatus() { // Reads button status from MCP23017 chip.

  Wire.requestFrom(_buttonAddress, (uint8_t)1);
  if (Wire.available()) {
    while (Wire.available()) {
      _deBounceButtonStatus(~Wire.read());
    }
  }

  if (_buttonStatus != _lastButtonStatus) {
    _lastButtonReleased = (_lastButtonStatus ^ _buttonStatus) & ~_buttonStatus;
    _lastButtonStatus = _buttonStatus;
    _inputChangeTime = millis();
    _inputChangeTimeActive = true; // Set true if it's ok to act on a trigger
  }
}
void SkaarhojDome::_deBounceButtonStatus(uint8_t inputValue)	{
	if (inputValue != _buttonStatus)	{
		if (inputValue == _buttonStatus_debounceCache)	{
			if (((uint16_t)millis())-_debounceCounter > 100)	{	
				_buttonStatus =_buttonStatus_debounceCache = inputValue;
			}
		} else {
			_buttonStatus_debounceCache = inputValue;
			_debounceCounter = millis();
		}
	} else _buttonStatus_debounceCache = _buttonStatus;
}
bool SkaarhojDome::_validButtonNumber(uint8_t buttonNumber) { // Checks if a button number is valid (1-16)
  return (buttonNumber >= 1 && buttonNumber <= _numberOfButtons);
}
bool SkaarhojDome::_validColorNumber(uint8_t colorNumber) { // Checks if a color number is valid (1-10)
  return (colorNumber <= 9);
}
bool SkaarhojDome::_validPercentage(uint8_t percentage) { // Checks if a value is within 0-100
  return (percentage <= 100);
}

void SkaarhojDome::testProgramme(uint8_t buttonMask) {

  uint8_t allButtons = buttonDownAll();
  for (uint8_t i = 0; i < 8; i++) {
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

    if ((buttonMask & ((uint8_t)B1 << test_button)) > 0) {
      _writeButtonLed(test_button + 1, test_color);
      break;
    }
  }
}

void SkaarhojDome::clearButtonColorCache() {
  for (uint8_t i = 0; i < 8; i++) {
    _buttonColorCache[i] = 255;
  }
}