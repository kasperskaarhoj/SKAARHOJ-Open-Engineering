/*  SkaarhojBI16 Arduino library for the BI8 board from SKAARHOJ.com
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
#include "SkaarhojBI16.h"


SkaarhojBI16::SkaarhojBI16(){
	_debugMode = false;
}	// Empty constructor.

bool SkaarhojBI16::begin(uint8_t address)	{
	// NOTE: Wire.h should definitely be initialized at this point! (Wire.begin())
	_boardAddress = (address & B111);	// 0-7

		// Initializing:
	_buttonStatus = 0;
	_lastButtonStatus = 0;
	_buttonStatusLastUp = 0;
	_buttonStatusLastDown = 0;
			
		// Used to track last used gray level in order to NOT write pwm values to buttons if they already have that gray level (writing same value subsequently will make the LED blink weirdly because each time a new timing scheme is randomly created in the PCA9685)
	for(int i=1;i<=16;i++)  {
		_buttonColorCache[(i-1)] = 255;
	}

	_grayLevel[0] = 0;
	_grayLevel[1] = 100;
	_grayLevel[2] = 20;
	_grayLevel[3] = 5;
	
	_defaultColorNumber = 3;
		
		// Create object for reading button presses
	MCP23017 buttonMux;
	_buttonMux = buttonMux;
	_buttonMux.begin((int)_boardAddress);

		// Create object for writing LED levels:
	PCA9685 buttonLed;
	_buttonLed = buttonLed;
	_buttonLed.begin((int)(B111000 | _boardAddress));
	
		// Inputs:
	bool isOnline = _buttonMux.init();

		// Set everything as inputs with pull up resistors:
	_buttonMux.internalPullupMask(65535);	// All has pull-up
	_buttonMux.inputOutputMask(65535);	// All are inputs.
 	_buttonMux.inputPolarityMask(65535);

		// Outputs:
	_buttonLed.init();	
	setButtonColorsToDefault();
	
	return isOnline;
}
bool SkaarhojBI16::isOnline() {
	return _buttonMux.init();	// It's not necessary to init the board for this - but it doesn't harm and is most easy...
}
void SkaarhojBI16::debugMode()	{
	_debugMode = true;	
}
void SkaarhojBI16::setGrayLevel(uint8_t colorNumber, uint8_t grayLevel) {
	if (_validColorNumber(colorNumber) && _validPercentage(grayLevel))	{
		_grayLevel[colorNumber] = grayLevel;
	}
}
void SkaarhojBI16::setDefaultColor(uint8_t defaultColorNumber) {
	if (_validColorNumber(defaultColorNumber))	_defaultColorNumber = defaultColorNumber;
}
void SkaarhojBI16::setButtonColor(uint8_t buttonNumber, uint8_t colorNumber) {
	_writeButtonLed(buttonNumber,colorNumber);
}
void SkaarhojBI16::setButtonColorsToDefault() {
	for(int i=1;i<=16;i++)  {
	  _writeButtonLed(i,_defaultColorNumber);
	}
}
void SkaarhojBI16::testSequence() { testSequence(20); }
uint16_t SkaarhojBI16::testSequence(uint16_t delayTime) {
  uint16_t bDown = 0;
	// Test LEDS:
  for(uint8_t ii=0;ii<=3;ii++)  {	// Gray levels
    for(uint8_t i=1;i<=16;i++)  {	// Buttons
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


bool SkaarhojBI16::buttonUp(uint8_t buttonNumber) {	// Returns true if a button 1-16 is has just been released
	if (_validButtonNumber(buttonNumber))	{
		_readButtonStatus();

		uint16_t mask = (B1 << (buttonNumber-1));
		uint16_t buttonChange = (_buttonStatusLastUp ^ _buttonStatus) & mask;
		_buttonStatusLastUp ^= buttonChange;

		return (buttonChange & ~_buttonStatus) ? true : false;
	} else return false;
}
bool SkaarhojBI16::buttonDown(uint8_t buttonNumber) {	// Returns true if a button 1-16 is has just been pushed down
	if (_validButtonNumber(buttonNumber))	{
		_readButtonStatus();

		uint16_t mask = (B1 << (buttonNumber-1));
		uint16_t buttonChange = (_buttonStatusLastDown ^ _buttonStatus) & mask;
		_buttonStatusLastDown ^= buttonChange;

		return (buttonChange & _buttonStatus) ? true : false;
	} else return false;
}
bool SkaarhojBI16::buttonIsPressed(uint8_t buttonNumber) {	// Returns true if a button 1-16 is currently pressed
	if (_validButtonNumber(buttonNumber))	{
		_readButtonStatus();
		return (buttonIsPressedAll() >> (buttonNumber-1)) & 1 ? true : false;
	} else return false;
}
bool SkaarhojBI16::buttonIsHeldFor(uint8_t buttonNumber, uint16_t timeout) {
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
bool SkaarhojBI16::buttonIsReleasedAgo(uint8_t buttonNumber, uint16_t timeout) {
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
uint16_t SkaarhojBI16::buttonUpAll() {	// Returns a word where each bit indicates if a button 1-10 (bits 0-9) has been released since last check
	_readButtonStatus();
	
	uint16_t buttonChange = _buttonStatusLastUp ^ _buttonStatus;
	_buttonStatusLastUp = _buttonStatus;

	return buttonChange & ~_buttonStatus;
}
uint16_t SkaarhojBI16::buttonDownAll() {	// Returns a word where each bit indicates if a button 1-10 (bits 0-9) has been pressed since last check
	_readButtonStatus();
	
	uint16_t buttonChange = _buttonStatusLastDown ^ _buttonStatus;
	_buttonStatusLastDown = _buttonStatus;
	
	return buttonChange & _buttonStatus;
}
uint16_t SkaarhojBI16::buttonIsPressedAll() {	// Returns a word where each bit indicates if a button 1-10 (bits 0-9) is currently pressed since last check
	_readButtonStatus();
	
	return _buttonStatus;
}
bool SkaarhojBI16::isButtonIn(uint8_t buttonNumber, uint16_t allButtonsState)	{
	if (_validButtonNumber(buttonNumber))	{
		return (allButtonsState & (B1 << (buttonNumber-1))) ? true : false;
	}
	return false;
}



// Private methods:

void SkaarhojBI16::_writeButtonLed(uint8_t buttonNumber, uint8_t color)  {
	if (_validColorNumber(color) && _validButtonNumber(buttonNumber) && _buttonColorCache[(buttonNumber-1)] != color)		{
		_buttonColorCache[(buttonNumber-1)] = color;
		_buttonLed.setLEDDimmed((buttonNumber-1),  _grayLevel[color]);
	}
}

void SkaarhojBI16::_readButtonStatus() {	// Reads button status from MCP23017 chip.
	uint16_t buttonStatus = _buttonMux.digitalWordRead();
	_buttonStatus = (buttonStatus >> 8) | (buttonStatus << 8); 
	
	if (_buttonStatus != _lastButtonStatus)	{
		_lastButtonReleased = (_lastButtonStatus ^ _buttonStatus) & ~_buttonStatus;
		_lastButtonStatus = _buttonStatus;
		_inputChangeTime = millis();
		_inputChangeTimeActive = true;	// Set true if it's ok to act on a trigger
	}
}

bool SkaarhojBI16::_validButtonNumber(uint8_t buttonNumber)	{	// Checks if a button number is valid (1-16)
	return (buttonNumber>=1 && buttonNumber <= 16);
}
bool SkaarhojBI16::_validColorNumber(uint8_t colorNumber)	{	// Checks if a color number is valid (0-3)
	return (colorNumber <= 3);
}
bool SkaarhojBI16::_validPercentage(uint8_t percentage)	{	// Checks if a value is within 0-100
	return (percentage <= 100);
}



void SkaarhojBI16::testProgramme(uint16_t buttonMask)	{

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
			test_color = (test_color+1)%4;
		}

		if ((buttonMask & ((uint16_t)B1 << test_button)) > 0)	{
			_writeButtonLed(test_button+1,test_color);
			break;
		}
	}
}
