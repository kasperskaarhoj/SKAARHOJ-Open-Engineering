/*  SkaarhojAudioControl Arduino library for the AC boards from SKAARHOJ.com
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
#ifndef SkaarhojAudioControl_H
#define SkaarhojAudioControl_H

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

class SkaarhojAudioControl
{
  private:
	uint8_t _boardAddress;
	MCP23017 _buttonMux;
	PCA9685 _VUledDriver;
	uint8_t _buttonStatus;
	uint8_t _buttonStatusLastUp;
	uint8_t _buttonStatusLastDown;
	uint8_t _ledFlags;
	bool _isMaster;

  public:
	SkaarhojAudioControl();
	void begin(int address);
	void setIsMasterBoard();
	
	bool buttonUp(int buttonNumber);
	bool buttonDown(int buttonNumber);
	bool buttonIsPressed(int buttonNumber);
	uint8_t buttonUpAll();
	uint8_t buttonDownAll();
	uint8_t buttonIsPressedAll();
	bool isButtonIn(int buttonNumber, uint8_t allButtonsState);

	void setButtonLight(uint8_t buttonNumber, bool state);
	void setChannelIndicatorLight(uint8_t ledNumber, uint8_t redGreenBits);
	
	void VUmeter(uint16_t leftValue, uint16_t rightValue);
	
  private:
	void _readButtonStatus();
	bool _validButtonNumber(int buttonNumber);
	void _writeOutputs(uint8_t flags);

};
#endif 
