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
#include "SkaarhojAudioControl2.h"

SkaarhojAudioControl2::SkaarhojAudioControl2(){
	_oldI2CAddr = false;
}	// Empty constructor.

void SkaarhojAudioControl2::begin(int address, int addressVU) {
	// NOTE: Wire.h should definitely be initialized at this point! (Wire.begin())
	
	_boardAddress = (address & B111);	// 0-7

	// Initializing:
	_buttonStatus = 0;
	_buttonStatusLastUp = 0;
	_buttonStatusLastDown = 0;
	
	_ledFlags = 0;
	
	_isMaster = false;

	PCA9685 VUledDriver; 
	_VUledDriver = VUledDriver;
	  // VU meter: 
    _VUledDriver.begin(B110000 | (addressVU==-1 ? _boardAddress : (addressVU & B111)));
    _VUledDriver.init();

	// Create object for reading button presses
	_chipAddress = (_oldI2CAddr ? B100000 : B1011000) | _boardAddress;
	_writeOutputs(_ledFlags);
}

void SkaarhojAudioControl2::oldI2CAddr(bool oldI2CAddr)	{
	_oldI2CAddr = oldI2CAddr;
}

void SkaarhojAudioControl2::setIsMasterBoard()	{
	_isMaster = true;
}

/**
 *  Returns true if a button 1-8 is has just been released
 */
bool SkaarhojAudioControl2::buttonUp(int buttonNumber) {	
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
bool SkaarhojAudioControl2::buttonDown(int buttonNumber) {
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
bool SkaarhojAudioControl2::buttonIsPressed(int buttonNumber) {
	if (_validButtonNumber(buttonNumber))	{
		_readButtonStatus();
		return (SkaarhojAudioControl2::buttonIsPressedAll() >> (buttonNumber-1)) ? true : false;
	} else return false;
}

/**
 *  Returns a byte where each bit indicates if a button 1-8 (bits 0-7) has been released since last check
 */
uint8_t SkaarhojAudioControl2::buttonUpAll() {
	_readButtonStatus();
	
	uint8_t buttonChange = _buttonStatusLastUp ^ _buttonStatus;
	_buttonStatusLastUp = _buttonStatus;

	return buttonChange & ~_buttonStatus;
}

/**
 *  Returns a byte where each bit indicates if a button 1-8 (bits 0-7) has been pressed since last check
 */
uint8_t SkaarhojAudioControl2::buttonDownAll() {
	_readButtonStatus();
	
	uint8_t buttonChange = _buttonStatusLastDown ^ _buttonStatus;
	_buttonStatusLastDown = _buttonStatus;
	
	return buttonChange & _buttonStatus;
}

/**
 *  Returns a byte where each bit indicates if a button 1-8 (bits 0-7) is currently pressed
 */
uint8_t SkaarhojAudioControl2::buttonIsPressedAll() {
	_readButtonStatus();
	
	return _buttonStatus;
}

/**
 *  
 */
bool SkaarhojAudioControl2::isButtonIn(int buttonNumber, uint8_t allButtonsState)	{
	if (_validButtonNumber(buttonNumber))	{
		return (allButtonsState & (B1 << (buttonNumber-1))) ? true : false;
	}
	return false;
}





void SkaarhojAudioControl2::setButtonLight(uint8_t buttonNumber, bool state)	{
	uint8_t tempLedFlags = _ledFlags;

	if (_validButtonNumber(buttonNumber))	{
		uint8_t mask = (B1 << ((buttonNumber-1)<2 ? (buttonNumber-1) : (buttonNumber-1)+2));
		if (_isMaster)	mask = mask << 1;	// Because of mistake on PCB - it wasn't on purpose that the 7 wires were moved one bit on ACM compared to AC2... sigh.
		if (state)	{	// On
			tempLedFlags |= mask;
		} else {
			tempLedFlags &= 255 ^ mask;
		}
	}
	if (tempLedFlags!= _ledFlags)	{
		_ledFlags = tempLedFlags;
		_writeOutputs(tempLedFlags);
	}
}
void SkaarhojAudioControl2::setChannelIndicatorLight(uint8_t ledNumber, uint8_t redGreenBits)	{
	uint8_t tempLedFlags = _ledFlags;

	if (_isMaster)	{
		if (ledNumber==1)	{
			tempLedFlags &= 255 ^ (B11 << (3+4*(ledNumber-1)));
			tempLedFlags |= ((redGreenBits&B11) << (3+4*(ledNumber-1)));
		}
	} else {
		if (ledNumber == 1 || ledNumber == 2)	{
			tempLedFlags &= 255 ^ (B11 << (2+4*((ledNumber)-1)));
			tempLedFlags |= ((redGreenBits&B11) << (2+4*((ledNumber)-1)));
		}
	}
	if (tempLedFlags!= _ledFlags)	{
		_ledFlags = tempLedFlags;
		_writeOutputs(tempLedFlags);
	}
}


void SkaarhojAudioControl2::VUmeter(uint16_t leftValue, uint16_t rightValue)  {
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

void SkaarhojAudioControl2::VUmeterDB(int leftValue, int rightValue)  {
  int meterThreshold[] = {-60, -42, -24, -12, -9, -6, -4, -2};

  for(uint8_t i=0; i<8; i++)  {
    _VUledDriver.setLEDDimmed(i,leftValue>meterThreshold[i]?100:0);
  }  
  for(uint8_t i=8; i<16; i++)  {
    _VUledDriver.setLEDDimmed(i,rightValue>meterThreshold[i-8]?100:0);
  }  
}

void SkaarhojAudioControl2::VUmeterRaw(uint8_t leftValue, uint8_t rightValue)	{
    for(uint8_t i=0; i<8; i++)  {
      _VUledDriver.setLEDDimmed(i,((leftValue>>i)&B1)?100:0);
    }  
    for(uint8_t i=8; i<16; i++)  {
      _VUledDriver.setLEDDimmed(i,((rightValue>>(i-8))&B1)?100:0);
    }  
}

// Private methods:
void SkaarhojAudioControl2::_readButtonStatus() {	// Reads button status from MCP23017 chip.
	uint16_t buttonStatus = PCA9671digitalWordRead();
	_buttonStatus = ~(buttonStatus >> 8 & B11111);
}
bool SkaarhojAudioControl2::_validButtonNumber(int buttonNumber)	{	// Checks if a button number is valid (1-4)
	return (buttonNumber>=1 && buttonNumber <= (_isMaster ? 5 : 4));
}

void SkaarhojAudioControl2::_writeOutputs(uint8_t flags)	{
    Wire.beginTransmission(_chipAddress);
    Wire.write(255);
    Wire.write(255-flags);
    Wire.endTransmission();
}
uint16_t SkaarhojAudioControl2::PCA9671digitalWordRead()	{
	_writeOutputs(_ledFlags);	// For some reason I need to write before I can consistently read the value from the chip...
	
	uint8_t byte1 = 0;
	uint8_t byte2 = 0;
	
	Wire.requestFrom(_chipAddress, (uint8_t)2);  // asking for two bytes - could ask for more, would just get the same data again.
	while(Wire.available())	{
		byte1 = Wire.read();	// LSB
		byte2 = Wire.read();
	}

	return (byte1 << 8) | byte2;
}
