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
#include "SkaarhojAudioControl.h"

SkaarhojAudioControl::SkaarhojAudioControl(){}	// Empty constructor.

void SkaarhojAudioControl::begin(int address) {
	// NOTE: Wire.h should definitely be initialized at this point! (Wire.begin())
	
	_boardAddress = (address & B111);	// 0-7

	// Initializing:
	_buttonStatus = 0;
	_buttonStatusLastUp = 0;
	_buttonStatusLastDown = 0;
	
	_ledFlags = 0;
	
	_isMaster = false;

	// Create object for reading button presses
	MCP23017 buttonMux;
	_buttonMux = buttonMux;
	_buttonMux.begin((int)(_boardAddress));

	_buttonMux.internalPullupMask(65535);	// Set pull-ups
	_buttonMux.inputOutputMask(65535-255);	// Set up input and output pins
    _buttonMux.inputPolarityMask(65535);	// Reverse polarity for inputs.

	PCA9685 VUledDriver; 
	_VUledDriver = VUledDriver;
	  // VU meter: 
    _VUledDriver.begin(B111000 | _boardAddress);  // Address 32
    _VUledDriver.init();

	_writeOutputs(_ledFlags);
}

void SkaarhojAudioControl::setIsMasterBoard()	{
	_isMaster = true;
}

/**
 *  Returns true if a button 1-8 is has just been released
 */
bool SkaarhojAudioControl::buttonUp(int buttonNumber) {	
	if (_validButtonNumber(buttonNumber))	{
		_readButtonStatus();

		uint8_t mask = (B1 << (buttonNumber-1));
		uint8_t buttonChange = (_buttonStatusLastUp ^ _buttonStatus) & mask;
		_buttonStatusLastUp ^= buttonChange;

		return (buttonChange & ~_buttonStatus) ? true : false;
	} else return false;
}

/**
 *  Returns true if a button 1-8 is has just been pushed down
 */
bool SkaarhojAudioControl::buttonDown(int buttonNumber) {
	if (_validButtonNumber(buttonNumber))	{
		_readButtonStatus();

		uint8_t mask = (B1 << (buttonNumber-1));
		uint8_t buttonChange = (_buttonStatusLastDown ^ _buttonStatus) & mask;
		_buttonStatusLastDown ^= buttonChange;

		return (buttonChange & _buttonStatus) ? true : false;
	} else return false;
}

/**
 *  Returns true if a button 1-8 is currently pressed
 */
bool SkaarhojAudioControl::buttonIsPressed(int buttonNumber) {
	if (_validButtonNumber(buttonNumber))	{
		_readButtonStatus();
		return (SkaarhojAudioControl::buttonIsPressedAll() >> (buttonNumber-1)) ? true : false;
	} else return false;
}

/**
 *  Returns a byte where each bit indicates if a button 1-8 (bits 0-7) has been released since last check
 */
uint8_t SkaarhojAudioControl::buttonUpAll() {
	_readButtonStatus();
	
	uint8_t buttonChange = _buttonStatusLastUp ^ _buttonStatus;
	_buttonStatusLastUp = _buttonStatus;

	return buttonChange & ~_buttonStatus;
}

/**
 *  Returns a byte where each bit indicates if a button 1-8 (bits 0-7) has been pressed since last check
 */
uint8_t SkaarhojAudioControl::buttonDownAll() {
	_readButtonStatus();
	
	uint8_t buttonChange = _buttonStatusLastDown ^ _buttonStatus;
	_buttonStatusLastDown = _buttonStatus;
	
	return buttonChange & _buttonStatus;
}

/**
 *  Returns a byte where each bit indicates if a button 1-8 (bits 0-7) is currently pressed
 */
uint8_t SkaarhojAudioControl::buttonIsPressedAll() {
	_readButtonStatus();
	
	return _buttonStatus;
}

/**
 *  
 */
bool SkaarhojAudioControl::isButtonIn(int buttonNumber, uint8_t allButtonsState)	{
	if (_validButtonNumber(buttonNumber))	{
		return (allButtonsState & (B1 << (buttonNumber-1))) ? true : false;
	}
	return false;
}





void SkaarhojAudioControl::setButtonLight(uint8_t buttonNumber, bool state)	{
	uint8_t tempLedFlags = _ledFlags;

	if (_validButtonNumber(buttonNumber))	{
		if (state)	{	// On
			tempLedFlags |= (B1 << ((buttonNumber-1)<2 ? (buttonNumber-1) : (buttonNumber-1)+2));
		} else {
			tempLedFlags &= 255 ^ (B1 << ((buttonNumber-1)<2 ? (buttonNumber-1) : (buttonNumber-1)+2));
		}
	}
	if (tempLedFlags!= _ledFlags)	{
		_ledFlags = tempLedFlags;
		_writeOutputs(tempLedFlags);
	}
}
void SkaarhojAudioControl::setChannelIndicatorLight(uint8_t ledNumber, uint8_t redGreenBits)	{
	uint8_t tempLedFlags = _ledFlags;

	if (_isMaster)	{
		if (ledNumber==1)	{
			tempLedFlags &= 255 ^ (B11 << (2+4*(ledNumber-1)));
			tempLedFlags |= ((redGreenBits&B11) << (2+4*(ledNumber-1)));
		}
	} else {
		if (ledNumber>=1 && ledNumber <= 2)	{
			tempLedFlags &= 255 ^ (B11 << (2+4*((3-ledNumber)-1)));
			tempLedFlags |= ((redGreenBits&B11) << (2+4*((3-ledNumber)-1)));
		}
	}
	if (tempLedFlags!= _ledFlags)	{
		_ledFlags = tempLedFlags;
		_writeOutputs(tempLedFlags);
	}
}


void SkaarhojAudioControl::VUmeter(uint16_t leftValue, uint16_t rightValue)  {
	// db: 5xgreen, 2xyellow, 1xred: -48, -32, -24, -18, -15, -12, -9 , -6
	// meterThreshold = 32809,85 * 1,12^dB
  uint16_t meterThreshold[] = {142, 873, 2161, 4266, 6000, 8240, 11640, 16444};

  for(uint8_t i=0; i<8; i++)  {
    _VUledDriver.setLEDDimmed(i,leftValue>meterThreshold[i]?100:0);
  }  
  for(uint8_t i=8; i<16; i++)  {
    _VUledDriver.setLEDDimmed(i,rightValue>meterThreshold[i-8]?100:0);
  }  
}

// Private methods:
void SkaarhojAudioControl::_readButtonStatus() {	// Reads button status from MCP23017 chip.
	word buttonStatus = _buttonMux.digitalWordRead();
	_buttonStatus = buttonStatus >> 8 & B11111;
}
bool SkaarhojAudioControl::_validButtonNumber(int buttonNumber)	{	// Checks if a button number is valid (1-4)
	return (buttonNumber>=1 && buttonNumber <= (_isMaster ? 5 : 4));
}

void SkaarhojAudioControl::_writeOutputs(uint8_t flags)	{
	_buttonMux.digitalWordWrite(255-flags);
}

