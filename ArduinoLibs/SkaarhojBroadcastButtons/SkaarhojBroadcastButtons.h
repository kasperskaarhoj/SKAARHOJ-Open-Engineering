/*  SkaarhojBroadcastButtons Arduino library for the Broadcast Buttons Shield from SKAARHOJ.com
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
#ifndef SkaarhojBroadcastButtons_H
#define SkaarhojBroadcastButtons_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif


/**
  Version 1.0.0
	(Semantic Versioning)
**/


class SkaarhojBroadcastButtons
{
  private:
	bool _fourButtons;	// If true, the four-button configuration "B3A-D" is used. Otherwise (default) it is buttons "B2A,B2B", "B4", "B5A,B5B"

	uint8_t _buttonStatus;
	uint8_t _buttonStatusLastUp;
	uint8_t _buttonStatusLastDown;

	uint8_t _colorBalanceRed[10];	
	uint8_t _colorBalanceGreen[10];
	uint8_t _defaultColorNumber;
	uint8_t _buttonColorCache[4];

  public:
	SkaarhojBroadcastButtons();
	void begin();
	void begin(bool fourButtons);
	void setColorBalance(int colorNumber, int redPart, int greenPart);
	void setDefaultColor(int defaultColorNumber);
	void setButtonColor(int buttonNumber, int colorNumber);
	void setButtonColorsToDefault();
	void testSequence();
	void testSequence(int delayTime);
	
	bool buttonUp(int buttonNumber);
	bool buttonDown(int buttonNumber);
	bool buttonIsPressed(int buttonNumber);
	
  private:
	void _writeButtonLed(int buttonNumber, int color);
	void _readButtonStatus();
	bool _validButtonNumber(int buttonNumber);
	bool _validColorNumber(int colorNumber);
	bool _validPercentage(int percentage);
};
#endif 
