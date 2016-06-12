/*  SkaarhojDome library for Arduino
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
#ifndef SkaarhojDome_H
#define SkaarhojDome_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "Wire.h"
#include "MCP23017.h"
#include "PCA9685.h"

/**
  Version 1.0.0
	(Semantic Versioning)
**/

class SkaarhojDome
{
  private:
  	uint8_t _boardAddress;
	uint8_t _buttonAddress;
	uint8_t _numberOfButtons;
	
	PCA9685 _buttonLed;
	uint8_t _buttonStatus_debounceCache;
	uint16_t _debounceCounter;
	uint8_t _buttonStatus;
	uint8_t _buttonStatusLastUp;
	uint8_t _buttonStatusLastDown;

	uint8_t _lastButtonStatus;
	uint8_t _lastButtonReleased;
	unsigned long _inputChangeTime;
	bool _inputChangeTimeActive;

	uint8_t _colorBalanceRed[10];	
	uint8_t _colorBalanceGreen[10];
	uint8_t _colorBalanceBlue[10];
	uint8_t _defaultColorNumber;
	uint8_t _buttonColorCache[8];	// 8 buttons
	bool _disableColorCache;

	uint8_t test_color = 0;
	uint8_t test_button = 0;

  public:
	SkaarhojDome();
	bool begin(uint8_t LEDaddress, uint8_t buttonAddress);
	void disableColorCache(bool disable);
	void setColorBalance(uint8_t colorNumber, uint8_t redPart, uint8_t greenPart, uint8_t blueParts);
	void setDefaultColor(uint8_t defaultColorNumber);
	void setButtonColor(uint8_t buttonNumber, uint8_t colorNumber);
	void setButtonColorsToDefault();
	void testSequence();
	uint8_t testSequence(uint16_t delayTime);
	
	bool buttonUp(uint8_t buttonNumber);
	bool buttonDown(uint8_t buttonNumber);
	bool buttonIsPressed(uint8_t buttonNumber);
	bool buttonIsHeldFor(uint8_t buttonNumber, uint16_t timeout);
	bool buttonIsReleasedAgo(uint8_t buttonNumber, uint16_t timeout);
	uint8_t buttonUpAll();
	uint8_t buttonDownAll();
	uint8_t buttonIsPressedAll();
	bool isButtonIn(uint8_t buttonNumber, uint8_t allButtonsState);
	void testProgramme(uint8_t buttonMask);
	void clearButtonColorCache();
	
  private:
	void _writeButtonLed(uint8_t buttonNumber, uint8_t color);
	void _readButtonStatus();
	void _deBounceButtonStatus(uint8_t inputValue);
	bool _validButtonNumber(uint8_t buttonNumber);
	bool _validColorNumber(uint8_t colorNumber);
	bool _validPercentage(uint8_t percentage);
};
#endif 
