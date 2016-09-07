/*
Copyright 2014 Kasper Skårhøj, SKAARHOJ, kasper@skaarhoj.com

This file is part of the Blackmagic Design SmartView Client library for Arduino

The ClientBMDSmartView library is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published by the 
Free Software Foundation, either version 3 of the License, or (at your 
option) any later version.

The ClientBMDSmartView library is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
or FITNESS FOR A PARTICULAR PURPOSE. 
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along 
with the ClientBMDSmartView library. If not, see http://www.gnu.org/licenses/.


IMPORTANT: If you want to use this library in your own projects and/or products,
please play a fair game and heed the license rules! See our web page for a Q&A so
you can keep a clear conscience: http://skaarhoj.com/about/licenses/


*/

#include <Wire.h>
#include "SkaarhojEADOGMDisplay.h"
#include "Streaming.h"

SkaarhojEADOGMDisplay::SkaarhojEADOGMDisplay() {}		

/**
 * Initialization
 * Address is 0-7 : How address jumpers are set; index is which display on the board (each display has it's own object), starting from 0; boardType depends whether the display is DOGM081 (1), DOGM162 (2) or DOGM163 (3)
 */
void SkaarhojEADOGMDisplay::begin(uint8_t address, uint8_t index, uint8_t boardType) {
	// NOTE: Wire.h should definitely be initialized at this point! (Wire.begin())

#if defined(ARDUINO_SKAARDUINO_V1) || defined(ARDUINO_SKAARDUINO_DUE)  
	SPI.begin();
#endif

	_boardAddress = 88 | (address & B111);	// 0-7
	_boardType = boardType;	// 1=DOGM81, 2=DOGM162, 3=DOGM163
	_index = index;	// which display on the board (0-3)

  	// Set SPI pins up:
	_clockPin = 48;
	_dataPin = 49;

	pinMode(_clockPin, OUTPUT);
	pinMode(_dataPin, OUTPUT);
	digitalWrite(_clockPin, LOW);
	digitalWrite(_dataPin, LOW);
	
	_chipLowerByte = 255;
	_chipUpperByte = 255;	// bit 6 (64) = 0 => red, bit 5 (32) = 0 => blue, bit 4 (16) = 0 => green

    _selectDisplay(_index,false);
    _sendData(false);

    _selectDisplay(_index, true);


#if defined(ARDUINO_SKAARDUINO_V1)  || defined(ARDUINO_SKAARDUINO_DUE) 
	SPISettings settingsA(SkaarhojEADOGMDisplay_SPI_SPEED, MSBFIRST, SPI_MODE0); 
	switch(_boardType)	{
		case 1:
			SPI.beginTransaction(settingsA);
		    SPI.transfer(0x31);
		    SPI.transfer(0x1C);
		    SPI.transfer(0x51);
		    SPI.transfer(0x6A);
		    SPI.transfer(0x74);
		    SPI.transfer(0x30);
		    SPI.transfer(0x0C);
		    SPI.transfer(0x01);
		    SPI.transfer(0x06);
			SPI.endTransaction();
		break;
		case 2:
			SPI.beginTransaction(settingsA);
		    SPI.transfer(0x39);
		    SPI.transfer(0x1C);
		    SPI.transfer(0x52);
		    SPI.transfer(0x69);
		    SPI.transfer(0x74);
		    SPI.transfer(0x38);
		    SPI.transfer(0x0C);
		    SPI.transfer(0x01);
		    SPI.transfer(0x06);
			SPI.endTransaction();
		break;
		case 3:
			SPI.beginTransaction(settingsA);
		    SPI.transfer(0x39);
		    SPI.transfer(0x1D);
		    SPI.transfer(0x50);
		    SPI.transfer(0x6C);
		    SPI.transfer(0x78);
		    SPI.transfer(0x38);
		    SPI.transfer(0x0C);
		    SPI.transfer(0x01);
		    SPI.transfer(0x06);
			SPI.endTransaction();
		break;
	}
#else
	switch(_boardType)	{
		case 1:
		      // Init sequence for a DOG081, 5V:
		    shiftOut(_dataPin, _clockPin, MSBFIRST, 0x31);   
		    shiftOut(_dataPin, _clockPin, MSBFIRST, 0x1C);   
		    shiftOut(_dataPin, _clockPin, MSBFIRST, 0x51);   
		    shiftOut(_dataPin, _clockPin, MSBFIRST, 0x6A);   
		    shiftOut(_dataPin, _clockPin, MSBFIRST, 0x74);   
		    shiftOut(_dataPin, _clockPin, MSBFIRST, 0x30);   
		    shiftOut(_dataPin, _clockPin, MSBFIRST, 0x0C);     // Normally 0x0F, but here disabled cursor.   
		    shiftOut(_dataPin, _clockPin, MSBFIRST, 0x01);   
		    shiftOut(_dataPin, _clockPin, MSBFIRST, 0x06);   
		break;
		case 2:
		      // Init sequence for a DOG162:
		    shiftOut(_dataPin, _clockPin, MSBFIRST, 0x39);   
		    shiftOut(_dataPin, _clockPin, MSBFIRST, 0x1C);   
		    shiftOut(_dataPin, _clockPin, MSBFIRST, 0x52);   
		    shiftOut(_dataPin, _clockPin, MSBFIRST, 0x69);   
		    shiftOut(_dataPin, _clockPin, MSBFIRST, 0x74);   
		    shiftOut(_dataPin, _clockPin, MSBFIRST, 0x38);   
		    shiftOut(_dataPin, _clockPin, MSBFIRST, 0x0C);     // Normally 0x0F, but here disabled cursor.
		    shiftOut(_dataPin, _clockPin, MSBFIRST, 0x01);   
		    shiftOut(_dataPin, _clockPin, MSBFIRST, 0x06);   
		break;
		case 3:
		      // Init sequence for a DOG163:
		    shiftOut(_dataPin, _clockPin, MSBFIRST, 0x39);   
		    shiftOut(_dataPin, _clockPin, MSBFIRST, 0x1D);   
		    shiftOut(_dataPin, _clockPin, MSBFIRST, 0x50);   
		    shiftOut(_dataPin, _clockPin, MSBFIRST, 0x6C);   
		    shiftOut(_dataPin, _clockPin, MSBFIRST, 0x78);   	// 7C
		    shiftOut(_dataPin, _clockPin, MSBFIRST, 0x38);   
		    shiftOut(_dataPin, _clockPin, MSBFIRST, 0x0C);     // Normally 0x0F, but here disabled cursor.
		    shiftOut(_dataPin, _clockPin, MSBFIRST, 0x01);   
		    shiftOut(_dataPin, _clockPin, MSBFIRST, 0x06);   
		break;
	}	
#endif




    _selectDisplay(_index,false);
	
	clearDisplay();
}

/**
 * Clear display
 */
void SkaarhojEADOGMDisplay::clearDisplay(void) {
	_DDRAMaddr = 0;

    _sendData(false);
    _selectDisplay(_index, true);
#if defined(ARDUINO_SKAARDUINO_V1) || defined(ARDUINO_SKAARDUINO_DUE)  
	SPISettings settingsA(SkaarhojEADOGMDisplay_SPI_SPEED, MSBFIRST, SPI_MODE0); 
	SPI.beginTransaction(settingsA);
    SPI.transfer(0x01);
	SPI.endTransaction();
#else
    shiftOut(_dataPin, _clockPin, MSBFIRST, 0x01);
#endif
    _selectDisplay(_index,false);
}

/**
 * Cursor
 */
void SkaarhojEADOGMDisplay::cursor(bool enable) {
    _sendData(false);
    _selectDisplay(_index, true);
#if defined(ARDUINO_SKAARDUINO_V1) || defined(ARDUINO_SKAARDUINO_DUE)  
	SPISettings settingsA(SkaarhojEADOGMDisplay_SPI_SPEED, MSBFIRST, SPI_MODE0); 
	SPI.beginTransaction(settingsA);
    SPI.transfer(enable ? 0x0F : 0x0C);
	SPI.endTransaction();
#else
    shiftOut(_dataPin, _clockPin, MSBFIRST, enable ? 0x0F : 0x0C);
#endif
    _selectDisplay(_index,false);
}

/**
 * Contrast
 */
void SkaarhojEADOGMDisplay::contrast(uint8_t contrast) {
    _sendData(false);
    _selectDisplay(_index, true);
	
//	Serial.println(_boardType!=1);
#if defined(ARDUINO_SKAARDUINO_V1) || defined(ARDUINO_SKAARDUINO_DUE)  
	SPISettings settingsA(SkaarhojEADOGMDisplay_SPI_SPEED, MSBFIRST, SPI_MODE0); 
	SPI.beginTransaction(settingsA);
    SPI.transfer(_boardType!=1 ? 0x39 : 0x31);
    SPI.transfer(0x70 | (contrast & 0xF));
    SPI.transfer(_boardType!=1 ? 0x38 : 0x30);
	SPI.endTransaction();
#else
    shiftOut(_dataPin, _clockPin, MSBFIRST, _boardType!=1 ? 0x39 : 0x31);   
    shiftOut(_dataPin, _clockPin, MSBFIRST, 0x70 | (contrast & 0xF));
    shiftOut(_dataPin, _clockPin, MSBFIRST, _boardType!=1 ? 0x38 : 0x30);   
#endif	
    _selectDisplay(_index,false);
}

/**
 * Goto (0,0 is first line, first column)
 */
void SkaarhojEADOGMDisplay::gotoRowCol(uint8_t row, uint8_t col) {
	_DDRAMaddr = (row << (_boardType==1?3:4)) + col;

//	Serial << row << " - " << col << " => " << _DDRAMaddr << "\n";

	switch(_boardType)	{
		case 1:
			_DDRAMaddr%=8;
		break;
		case 2:
			_DDRAMaddr%=32;
		break;
		case 3:
			_DDRAMaddr%=48;
		break;
	}
	
//	 Serial << _DDRAMaddr << "\n";
	if (_boardType==2 && _DDRAMaddr >= 16)	{
		_DDRAMaddr+=24;
	}

    _sendData(false);
    _selectDisplay(_index, true);
#if defined(ARDUINO_SKAARDUINO_V1) || defined(ARDUINO_SKAARDUINO_DUE)  
	SPISettings settingsA(SkaarhojEADOGMDisplay_SPI_SPEED, MSBFIRST, SPI_MODE0); 
	SPI.beginTransaction(settingsA);
    SPI.transfer(B10000000 | (_DDRAMaddr & B1111111));
	SPI.endTransaction();
#else
    shiftOut(_dataPin, _clockPin, MSBFIRST, B10000000 | (_DDRAMaddr & B1111111));
#endif
/*	for(uint8_t a=0;a<8;a++)	{
		digitalWrite(_dataPin, (B10000000 | (_DDRAMaddr & B1111111)) & (B1<<(7-a)));
		digitalWrite(_clockPin, HIGH);
//		delayMicroseconds(1500);
		digitalWrite(_clockPin, LOW);
	}
  */  
	_selectDisplay(_index,false);
	delayMicroseconds(30);
}

/**
 * New line
 */
void SkaarhojEADOGMDisplay::newline() {
	//Serial << _DDRAMaddr << " / " << (_DDRAMaddr >> (_boardType==1?3:4)) << "\n";
	gotoRowCol((_DDRAMaddr >> (_boardType==1?3:4))+1, 0);
}
void SkaarhojEADOGMDisplay::setBacklight(bool red, bool green, bool blue)	{
	_chipUpperByte = (_chipUpperByte & B10001111) | (red ? 0 : B01000000) | (blue ? 0 : B00100000) | (green ? 0 : B00010000);	// bit 6 (64) = 0 => red, bit 5 (32) = 0 => blue, bit 4 (16) = 0 => green
	_sendData(false);
}

void SkaarhojEADOGMDisplay::_incDDRAMaddr()	{
	_DDRAMaddr++;
	switch(_boardType)	{
		case 1:
			if (_DDRAMaddr>=8)	{
				_DDRAMaddr=0;
			}
		break;
		case 2:
			if (_DDRAMaddr>=32)	{
				_DDRAMaddr=0;
			}
		break;
		case 3:
			if (_DDRAMaddr>=48)	{
				_DDRAMaddr=0;
			}
		break;
	}
}

/**
 * Set C/D line for sending data (true = data) or not (false = command)
 */
void SkaarhojEADOGMDisplay::_sendData(bool enable)  {

  _chipUpperByte = enable ? (_chipUpperByte | B10000000) : (_chipUpperByte & B01111111);

  Wire.beginTransmission(_boardAddress);
  Wire.write(_chipLowerByte);
  Wire.write(_chipUpperByte);
  Wire.endTransmission();
}

/**
 * Select which display number (index) to write to. You can only write to one at a time.
 */
void SkaarhojEADOGMDisplay::_selectDisplay(uint8_t dispNum, bool enable)  {
  if (dispNum<4)  {
    if (enable)  {
      _chipLowerByte = _chipLowerByte & (255-(B1<<dispNum));
    } else {
      _chipLowerByte = _chipLowerByte | (B1<<dispNum);
    }
  
    Wire.beginTransmission(_boardAddress);
    Wire.write(_chipLowerByte);
    Wire.write(_chipUpperByte);
    Wire.endTransmission();
  }
}

/**
 * Write a character (Overloading Print-class)
 */
size_t SkaarhojEADOGMDisplay::write(uint8_t character) {
    _sendData(true);
    _selectDisplay(_index,true);
	if (character==10)	{
		newline();
	} else if (character==13)	{
	} else {
#if defined(ARDUINO_SKAARDUINO_V1) || defined(ARDUINO_SKAARDUINO_DUE)  
		SPISettings settingsA(SkaarhojEADOGMDisplay_SPI_SPEED, MSBFIRST, SPI_MODE0); 
		SPI.beginTransaction(settingsA);
	    SPI.transfer(character);
		SPI.endTransaction();
#else
	    shiftOut(_dataPin, _clockPin, MSBFIRST, character);
#endif
		_incDDRAMaddr();
	}
    _selectDisplay(_index,false);
}

/**
 * Write a whole string (Overloading Print-class)
 */ 
size_t SkaarhojEADOGMDisplay::write(const uint8_t *buffer, size_t size) {
    _sendData(true);
    _selectDisplay(_index,true);
	while(*buffer)	{
		if (buffer[0]==10)	{
			newline();
			_sendData(true);	// Must set "data" again and enable display after sending newline command - otherwise the remaining characters will not get displayed.
			_selectDisplay(_index,true);
		} else if (buffer[0]==13)	{
		} else {
#if defined(ARDUINO_SKAARDUINO_V1) || defined(ARDUINO_SKAARDUINO_DUE)  
			SPISettings settingsA(SkaarhojEADOGMDisplay_SPI_SPEED, MSBFIRST, SPI_MODE0); 
			SPI.beginTransaction(settingsA);
		    SPI.transfer(buffer[0]);
			SPI.endTransaction();
#else
			shiftOut(_dataPin, _clockPin, MSBFIRST, buffer[0]);   
#endif
			_incDDRAMaddr();
		}
		buffer++;
	}
    _selectDisplay(_index,false);
}