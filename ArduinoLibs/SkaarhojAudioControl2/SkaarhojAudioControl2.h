/*  SkaarhojAudioControl2 Arduino library for the AC boards from SKAARHOJ.com
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
#ifndef SkaarhojAudioControl2_H
#define SkaarhojAudioControl2_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "Wire.h"
#include "PCA9685.h"

/**
  Version 1.0.0
	(Semantic Versioning)
**/

class SkaarhojAudioControl2
{
  private:
	  bool _oldI2CAddr;
	uint8_t _boardAddress;
	PCA9685 _VUledDriver;
	uint8_t _buttonStatus;
	uint8_t _buttonStatusLastUp;
	uint8_t _buttonStatusLastDown;
	uint8_t _ledFlags;
	uint8_t _chipAddress;
	bool _isMaster;

  public:
	SkaarhojAudioControl2();
	void begin(int address, int addressVU=-1);
	void setIsMasterBoard();
	void oldI2CAddr(bool oldI2CAddr);
	
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
	void VUmeterDB(int leftValue, int rightValue);
	void VUmeterRaw(uint8_t leftValue, uint8_t rightValue);
	uint16_t PCA9671digitalWordRead();
		
  private:
	void _readButtonStatus();
	bool _validButtonNumber(int buttonNumber);
	void _writeOutputs(uint8_t flags);

};
#endif 
