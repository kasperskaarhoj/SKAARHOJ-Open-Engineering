/*  SkaarhojBI16 library for Arduino
    Copyright (C) 2013 Kasper Skårhøj    <kasperskaarhoj@gmail.com> 

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
#ifndef SkaarhojBI16_H
#define SkaarhojBI16_H

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

class SkaarhojBI16
{
  private:
	uint8_t _boardAddress;
	MCP23017 _buttonMux;
	PCA9685 _buttonLed;
	uint16_t _buttonStatus;
	uint16_t _buttonStatusLastUp;
	uint16_t _buttonStatusLastDown;

	uint16_t _lastButtonStatus;
	uint16_t _lastButtonReleased;
	unsigned long _inputChangeTime;
	bool _inputChangeTimeActive;

	uint8_t _grayLevel[4];	
	uint8_t _defaultColorNumber;
	uint8_t _buttonColorCache[16];	// 16 buttons
	bool _debugMode;

	uint8_t test_color = 0;
	uint8_t test_button = 0;

  public:
	SkaarhojBI16();
	bool begin(uint8_t address);
	bool isOnline();
	void debugMode();

	void setGrayLevel(uint8_t colorNumber, uint8_t grayLevel);
	void setDefaultColor(uint8_t defaultColorNumber);
	void setButtonColor(uint8_t buttonNumber, uint8_t colorNumber);
	void setButtonColorsToDefault();
	void testSequence();
	uint16_t testSequence(uint16_t delayTime);
	bool buttonUp(uint8_t buttonNumber);
	bool buttonDown(uint8_t buttonNumber);
	bool buttonIsPressed(uint8_t buttonNumber);
	bool buttonIsHeldFor(uint8_t buttonNumber, uint16_t timeout);
	bool buttonIsReleasedAgo(uint8_t buttonNumber, uint16_t timeout);
	uint16_t buttonUpAll();
	uint16_t buttonDownAll();
	uint16_t buttonIsPressedAll();
	bool isButtonIn(uint8_t buttonNumber, uint16_t allButtonsState);
	void testProgramme(uint16_t buttonMask);
		
  private:
	void _writeButtonLed(uint8_t buttonNumber, uint8_t color);
	void _readButtonStatus();
	bool _validButtonNumber(uint8_t buttonNumber);
	bool _validColorNumber(uint8_t colorNumber);
	bool _validPercentage(uint8_t percentage);
};
#endif 
