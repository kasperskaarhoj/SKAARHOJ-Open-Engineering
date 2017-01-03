/*  SkaarhojSIL Arduino library for the SIL board from SKAARHOJ.com
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
#include "SkaarhojSIL.h"


SkaarhojSIL::SkaarhojSIL(){
	_debugMode = false;
}

bool SkaarhojSIL::begin(uint8_t address) {
	// NOTE: Wire.h should definitely be initialized at this point! (Wire.begin())
	
	
	_boardAddress = (address & B111);	// 0-7

		// Initializing:
	_buttonStatus = 0;
	_lastButtonStatus = 0;
	_buttonStatusLastUp = 0;
	_buttonStatusLastDown = 0;
		
	setButtonType(0);	// Assuming NKK buttons as default
	
		// Used to track last used color in order to NOT write colors to buttons if they already have that color (writing same color subsequently will make the LED blink weirdly because each time a new timing scheme is randomly created in the PCA9685)
	clearButtonColorCache();
	
	_defaultColorNumber = 9;
		
		// Create object for reading button presses
	MCP23017 buttonMux;
	_buttonMux = buttonMux;
	_buttonMux.begin(_boardAddress);

		// Create object for writing LED levels:
	PCA9685 buttonLed, buttonLed2;
	_buttonLed = buttonLed;
	_buttonLed2 = buttonLed2;
	_buttonLed.begin(B111000 | _boardAddress);
	_buttonLed2.begin(B100000 | _boardAddress);
	
		// Inputs:
	_buttonMux.inputPolarityMask(65535);
	bool isOnline = _buttonMux.init();

		// Set everything as inputs with pull up resistors:
	_buttonMux.internalPullupMask(65535);	// All has pull-up
	_buttonMux.inputOutputMask(65535);	// All are inputs.

	_numberOfButtons = 16;
		// Outputs:
	_buttonLed.init(true);
	_buttonLed2.init(true);	
	setButtonColorsToDefault();
	
	return isOnline;
}

void SkaarhojSIL::disableColorCache(bool disable)	{
	_disableColorCache = disable;
}
bool SkaarhojSIL::isOnline() {
	return _buttonMux.init();	// It's not necessary to init the board for this - but it doesn't harm and is most easy...
}

void SkaarhojSIL::debugMode()	{
	_debugMode=true;	
}
void SkaarhojSIL::setButtonType(uint8_t type)	{
	// Rate from 0-100 for color numbers: Off(0), On(1), Red(2), Green(3), Amber(4), Backlit(5), Blue 100% (6), ...., White(9)
	switch(type)	{
		case 1: // LP 11 from e-switch
		_colorBalanceRed[0] = 0;
		_colorBalanceRed[1] = 100;
		_colorBalanceRed[2] = 100;
		_colorBalanceRed[3] = 0;
		_colorBalanceRed[4] = 100;
		_colorBalanceRed[5] = 20;
		_colorBalanceRed[6] = 0;
		_colorBalanceRed[7] = 0;
		_colorBalanceRed[8] = 0;
		_colorBalanceRed[9] = 0;

		_colorBalanceGreen[0] = 0;
		_colorBalanceGreen[1] = 100;
		_colorBalanceGreen[2] = 0;
		_colorBalanceGreen[3] = 100;
		_colorBalanceGreen[4] = 50;
		_colorBalanceGreen[5] = 10;
		_colorBalanceGreen[6] = 0;
		_colorBalanceGreen[7] = 0;
		_colorBalanceGreen[8] = 0;
		_colorBalanceGreen[9] = 0;		
		break;
		default: 	// NKK buttons
		_colorBalanceRed[0] = 0;
		_colorBalanceRed[1] = 70;
		_colorBalanceRed[2] = 70;
		_colorBalanceRed[3] = 0;
		_colorBalanceRed[4] = 30;
		_colorBalanceRed[5] = 5;
		_colorBalanceRed[6] = 0;
		_colorBalanceRed[7] = 0;
		_colorBalanceRed[8] = 0;
		_colorBalanceRed[9] = 100*0.5;

		_colorBalanceGreen[0] = 0;
		_colorBalanceGreen[1] = 70*0.8;	// Most recent batch of buttons require this - maybe find another way to create this balance?
		_colorBalanceGreen[2] = 0;
		_colorBalanceGreen[3] = 70;
		_colorBalanceGreen[4] = 30*0.8;	// Most recent batch of buttons require this - maybe find another way to create this balance?
		_colorBalanceGreen[5] = 5*0.8;	// Most recent batch of buttons require this - maybe find another way to create this balance?
		_colorBalanceGreen[6] = 0;
		_colorBalanceGreen[7] = 0;
		_colorBalanceGreen[8] = 0;
		_colorBalanceGreen[9] = 100;		

		_colorBalanceBlue[0] = 0;
		_colorBalanceBlue[1] = 0;
		_colorBalanceBlue[2] = 0;
		_colorBalanceBlue[3] = 0;
		_colorBalanceBlue[4] = 0;
		_colorBalanceBlue[5] = 0;
		_colorBalanceBlue[6] = 100;
		_colorBalanceBlue[7] = 50;
		_colorBalanceBlue[8] = 20;
		_colorBalanceBlue[9] = 100;		
		break;
	}
}
void SkaarhojSIL::setColorBalanceRGB(uint8_t colorNumber, uint8_t redPart, uint8_t greenPart, uint8_t bluePart) {
	if (_validColorNumber(colorNumber) && _validPercentage(redPart) && _validPercentage(greenPart) && _validPercentage(bluePart))	{
		_colorBalanceRed[colorNumber] = redPart;
		_colorBalanceGreen[colorNumber] = greenPart;
		_colorBalanceBlue[colorNumber] = bluePart;
	}
}
uint8_t SkaarhojSIL::getColorRed(uint8_t colorNumber)	{
	return _colorBalanceRed[colorNumber];
}
uint8_t SkaarhojSIL::getColorGreen(uint8_t colorNumber) {
	return _colorBalanceGreen[colorNumber];
}
uint8_t SkaarhojSIL::getColorBlue(uint8_t colorNumber) {
	return _colorBalanceBlue[colorNumber];
}

void SkaarhojSIL::setDefaultColor(uint8_t defaultColorNumber) {
	if (_validColorNumber(defaultColorNumber))	_defaultColorNumber = defaultColorNumber;
}
void SkaarhojSIL::setButtonColor(uint8_t buttonNumber, uint8_t colorNumber) {
	_writeButtonLed(buttonNumber,colorNumber);
}
void SkaarhojSIL::setButtonColorsToDefault() {
	for(uint8_t i=1;i<=_numberOfButtons;i++)  {
	  _writeButtonLed(i,_defaultColorNumber);
	}
}
void SkaarhojSIL::testSequence() { testSequence(20); }
uint16_t SkaarhojSIL::testSequence(uint16_t delayTime) {
  uint16_t bDown = 0;
	// Test LEDS:
  for(uint8_t ii=0;ii<=9;ii++)  {
    for(uint8_t i=1;i<=_numberOfButtons;i++)  {
      _writeButtonLed(i,ii);
			// Test for button press and exit if so:
	  bDown = buttonDownAll();
	  if (bDown)	{
		return bDown;
	  }
      delay(delayTime);
    }
    delay(delayTime*3);
  }
	setButtonColorsToDefault();
	return 0;
}


bool SkaarhojSIL::buttonUp(uint8_t buttonNumber) {	// Returns true if a button 1-16 is has just been released
	if (_validButtonNumber(buttonNumber))	{
		_readButtonStatus();

		uint16_t mask = (B1 << (buttonNumber-1));
		uint16_t buttonChange = (_buttonStatusLastUp ^ _buttonStatus) & mask;
		_buttonStatusLastUp ^= buttonChange;

		return (buttonChange & ~_buttonStatus) ? true : false;
	} else return false;
}
bool SkaarhojSIL::buttonDown(uint8_t buttonNumber) {	// Returns true if a button 1-16 is has just been pushed down
	if (_validButtonNumber(buttonNumber))	{
		_readButtonStatus();

		uint16_t mask = (B1 << (buttonNumber-1));
		uint16_t buttonChange = (_buttonStatusLastDown ^ _buttonStatus) & mask;
		_buttonStatusLastDown ^= buttonChange;

		return (buttonChange & _buttonStatus) ? true : false;
	} else return false;
}
bool SkaarhojSIL::buttonIsPressed(uint8_t buttonNumber) {	// Returns true if a button 1-16 is currently pressed
	if (_validButtonNumber(buttonNumber))	{
		return (buttonIsPressedAll() >> (buttonNumber-1)) & 1 ? true : false;
	} else return false;
}
bool SkaarhojSIL::buttonIsHeldFor(uint8_t buttonNumber, uint16_t timeout) {
	if (_validButtonNumber(buttonNumber))	{
		if ((buttonIsPressedAll() >> (buttonNumber-1)) & 1)	{
			if (_inputChangeTimeActive)	{
				if ((unsigned long)(_inputChangeTime + timeout) < (unsigned long)millis())	{
					_inputChangeTimeActive = false;	// No more triggers until we have a new change
					return true;
				}
			}
		}
	} 
	return false;
}
bool SkaarhojSIL::buttonIsReleasedAgo(uint8_t buttonNumber, uint16_t timeout) {
	if (_validButtonNumber(buttonNumber))	{
		if (isButtonIn(buttonNumber, _lastButtonReleased))	{
			if (_inputChangeTimeActive)	{
				if ((unsigned long)(_inputChangeTime + timeout) < (unsigned long)millis())	{
					_inputChangeTimeActive = false;	// No more triggers until we have a new change
					return true;
				}
			}
		}
	} 
	return false;
}

uint32_t SkaarhojSIL::buttonAll() {
	_readButtonStatus();
	return (((uint32_t)buttonUpAll(false) << 16)| ((uint32_t) buttonDownAll(false) & 0xFFFF));
}

uint16_t SkaarhojSIL::buttonUpAll(bool read) {	// Returns a word where each bit indicates if a button 1-16 (bits 0-9) has been released since last check
	if(read)
		_readButtonStatus();
	
	uint16_t buttonChange = _buttonStatusLastUp ^ _buttonStatus;
	_buttonStatusLastUp = _buttonStatus;

	return buttonChange & ~_buttonStatus;
}
uint16_t SkaarhojSIL::buttonDownAll(bool read) {	// Returns a word where each bit indicates if a button 1-16 (bits 0-9) has been pressed since last check
	if(read)
		_readButtonStatus();
	
	uint16_t buttonChange = _buttonStatusLastDown ^ _buttonStatus;
	_buttonStatusLastDown = _buttonStatus;
	
	return buttonChange & _buttonStatus;
}

uint16_t SkaarhojSIL::buttonIsPressedAll() {	// Returns a word where each bit indicates if a button 1-16 (bits 0-9) is currently pressed since last check
	_readButtonStatus();
	
	return _buttonStatus;
}
bool SkaarhojSIL::isButtonIn(uint8_t buttonNumber, uint16_t allButtonsState)	{
	if (_validButtonNumber(buttonNumber))	{
		return (allButtonsState & (B1 << (buttonNumber-1))) ? true : false;
	}
	return false;
}



// Private methods:
void SkaarhojSIL::_writeButtonLed(uint8_t buttonNumber, uint8_t color)  {
	if (_validColorNumber(color) && _validButtonNumber(buttonNumber) && ((_buttonColorCache[(buttonNumber-1)] != color) || _disableColorCache))		{
		_buttonColorCache[(buttonNumber-1)] = color;
		
		// Set color R,G,B components
		for(uint8_t i=0; i<3; i++) {
			uint8_t c = (i==0?_colorBalanceRed[color]:
						(i==1?_colorBalanceGreen[color]:
							  _colorBalanceBlue[color]));
			if(buttonNumber>5) {
				_buttonLed2.setLEDDimmed((buttonNumber-6)*3+i, 100-c);
			} else {
				_buttonLed.setLEDDimmed((buttonNumber-1)*3+i, 100-c);
			}
		}
	}
}

void SkaarhojSIL::_readButtonStatus() {	// Reads button status from MCP23017 chip.
	uint16_t buttonStatus = _buttonMux.digitalWordRead();
	_buttonStatus = (buttonStatus >> 8) | ((buttonStatus & 0xFF) << 8);
	
	if (_buttonStatus != _lastButtonStatus)	{
		_lastButtonReleased = (_lastButtonStatus ^ _buttonStatus) & ~_buttonStatus;
		_lastButtonStatus = _buttonStatus;
		_inputChangeTime = millis();
		_inputChangeTimeActive = true;	// Set true if it's ok to act on a trigger
	}
}

bool SkaarhojSIL::_validButtonNumber(uint8_t buttonNumber)	{	// Checks if a button number is valid (1-16)
	return (buttonNumber>=1 && buttonNumber <= _numberOfButtons);
}
bool SkaarhojSIL::_validColorNumber(uint8_t colorNumber)	{	// Checks if a color number is valid (1-10)
	return (colorNumber <= 9);
}
bool SkaarhojSIL::_validPercentage(uint8_t percentage)	{	// Checks if a value is within 0-100
	return (percentage <= 100);
}



void SkaarhojSIL::testProgramme(uint16_t buttonMask)	{
	if(buttonMask == 0) return;

	uint16_t allButtons = buttonDownAll();
	for(uint16_t i=0; i<16;i++)	{
		if ((allButtons >> i & 1) > 0)	{
	        Serial.print(F("Button #"));
	        Serial.print(i+1);
	        Serial.print(F(" pressed\n"));
		}
	}

	while(true)	{
		test_button = (test_button+1)%16;
		if (!test_button)	{
			test_color = (test_color+1)%6;
		}

		if ((buttonMask & ((uint16_t)B1 << test_button)) > 0)	{
			_writeButtonLed(test_button+1,test_color);
			break;
		}
	}
}

void SkaarhojSIL::clearButtonColorCache()	{
	for(uint8_t i=0; i<16;i++)	{
		_buttonColorCache[i] = 255;
	}
}	
