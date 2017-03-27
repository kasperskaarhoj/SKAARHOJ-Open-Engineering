/*  SkaarhojBI8 Arduino library for the BI8 board from SKAARHOJ.com
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
#include "SkaarhojBI8.h"


SkaarhojBI8::SkaarhojBI8(){
	_debugMode = false;
	_oldBI8 = false;
}	// Empty constructor.

bool SkaarhojBI8::begin(uint8_t address)	{
	return begin(address, false, false);
}
bool SkaarhojBI8::begin(uint8_t address, bool reverseButtons) {
	return begin(address, reverseButtons, false);
}
bool SkaarhojBI8::begin(uint8_t address, bool reverseButtons, bool extendedBicolor) {
	// NOTE: Wire.h should definitely be initialized at this point! (Wire.begin())
	
	
	_boardAddress = (address & B111);	// 0-7
	_reverseButtons = reverseButtons;	// If set, buttons on the PCB is mounted on the bottom (opposite side of the chips). This affects how the LEDs should be programmed. All button numbers are the same. (OBSOLETE!)	
	_extendedBicolor = extendedBicolor;	// If set, the board is Bicolor and has 16 buttons on the input.
	_B1Alt = false;	
	_RGBbuttons = false;

		// Initializing:
	_buttonStatus = 0;
	_lastButtonStatus = 0;
	_buttonStatusLastUp = 0;
	_buttonStatusLastDown = 0;
		
	setButtonType(0);	// Assuming NKK buttons as default
	
		// Used to track last used color in order to NOT write colors to buttons if they already have that color (writing same color subsequently will make the LED blink weirdly because each time a new timing scheme is randomly created in the PCA9685)
	clearButtonColorCache();
	
	_defaultColorNumber = 5;
		
		// Create object for reading button presses
	MCP23017 buttonMux;
	_buttonMux = buttonMux;
	_buttonMux.begin(_boardAddress);

		// Create object for writing LED levels:
	PCA9685 buttonLed;
	_buttonLed = buttonLed;
	_buttonLed.begin(B111000 | _boardAddress);
	
		// Inputs:
	_buttonMux.inputPolarityMask(0);
	bool isOnline = _buttonMux.init();

		// Set everything as inputs with pull up resistors:
	_buttonMux.internalPullupMask(65535);	// All has pull-up
	_buttonMux.inputOutputMask(65535);	// All are inputs.

	if (!_extendedBicolor)	{
		word buttonStatus = _buttonMux.digitalWordRead();	// Read out.
		if ((buttonStatus & 1) == 0)  {	// Test value of GPB0
			 if (_debugMode && isOnline) Serial.println(F("BI8 >= v24-09-12: Switches pulls to low. Internal pull-ups enabled. BEST."));
			 _buttonMux.inputPolarityMask(65535);
		} else if ((buttonStatus >> 8) < 255) {	// Test if any of GPA0-7 are low (indicating pull-down resistors of 10K - or a button press!! Could be refined to test for more than one press)
			 if (_debugMode && isOnline) Serial.println(F("BI8 < v24-09-12: Switches pulls to high. External pull-ups enabled. WORKS..."));
			_buttonMux.internalPullupMask(0);	// In this case we don't need pull-up resistors...
			setButtonType(1);	// Assuming E-switch buttons for old BI8 boards
			_oldBI8 = true;
		} else {
			 if (_debugMode && isOnline) Serial.println(F("BI8 < v24-09-12: Switches pulls to high. NO pull-ups enabled!! Inputs must be configured as outputs! BAD!!"));	// NEVER three exclamation marks! The AVR crashes with "Bootloader Huh?"... see http://forums.adafruit.com/viewtopic.php?f=25&t=21396
			_buttonMux.inputOutputMask(0);	// We configure everything as outputs...
			setButtonType(1);	// Assuming E-switch buttons for old BI8 boards
			_oldBI8 = true;
		}


		if ((buttonStatus & 2) == 0)  {	// Test value of GPB1 (RGB board)
			 if (_debugMode && isOnline) Serial.println(F("RGB board"));
			 _RGBbuttons = true;

				// Create object for writing LED levels on PCA 2:
			PCA9685 buttonLed2;
			_buttonLed2 = buttonLed2;
			_buttonLed2.begin(B100000 | _boardAddress);
			_buttonLed2.init();

			for(uint8_t i=0; i<16; i++)	{
				_buttonLed2.setLEDDimmed(i, 0);	
			}
		}
	} else {
		 _buttonMux.inputPolarityMask(65535);
		 if (_debugMode && isOnline) Serial.println(F("BI8 w/16 buttons, bi-color"));

			// Create object for writing LED levels on PCA 2:
		PCA9685 buttonLed2;
		_buttonLed2 = buttonLed2;
		_buttonLed2.begin(B100000 | _boardAddress);
		_buttonLed2.init();

		for(uint8_t i=0; i<16; i++)	{
			_buttonLed2.setLEDDimmed(i, 0);	
		}
	}

	_numberOfButtons = _extendedBicolor ? 16 : (_RGBbuttons ? 10 : 8);


		// Outputs:
	_buttonLed.init();	
	setButtonColorsToDefault();
	
	return isOnline;
}
void SkaarhojBI8::usingB1alt()	{
	_B1Alt=true;	
}
void SkaarhojBI8::disableColorCache(bool disable)	{
	_disableColorCache = disable;
}
bool SkaarhojBI8::isOnline() {
	return _buttonMux.init();	// It's not necessary to init the board for this - but it doesn't harm and is most easy...
}
bool SkaarhojBI8::isRGBboard()	{
	return _RGBbuttons;
}
void SkaarhojBI8::debugMode()	{
	_debugMode=true;	
}
void SkaarhojBI8::setButtonType(uint8_t type)	{
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
		_colorBalanceRed[9] = 100;

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
void SkaarhojBI8::setColorBalance(uint8_t colorNumber, uint8_t redPart, uint8_t greenPart) {
	if (_validColorNumber(colorNumber) && _validPercentage(redPart) && _validPercentage(greenPart))	{
		_colorBalanceRed[colorNumber] = redPart;
		_colorBalanceGreen[colorNumber] = greenPart;
	}
}
void SkaarhojBI8::setColorBalanceRGB(uint8_t colorNumber, uint8_t redPart, uint8_t greenPart, uint8_t bluePart) {
	if (_validColorNumber(colorNumber) && _validPercentage(redPart) && _validPercentage(greenPart) && _validPercentage(bluePart))	{
		_colorBalanceRed[colorNumber] = redPart;
		_colorBalanceGreen[colorNumber] = greenPart;
		_colorBalanceBlue[colorNumber] = bluePart;
	}
}

void SkaarhojBI8::setButtonDefaultColor(uint16_t R, uint16_t G, uint16_t B) {
	R = map(R, 0, 4095, 0, 100);
	G = map(G, 0, 4095, 0, 100);
	B = map(B, 0, 4095, 0, 100);

	setColorBalanceRGB(4, R, G, B);
	setColorBalanceRGB(5, R/10, G/10, B/10);
}

uint8_t SkaarhojBI8::getColorRed(uint8_t colorNumber)	{
	return _colorBalanceRed[colorNumber];
}
uint8_t SkaarhojBI8::getColorGreen(uint8_t colorNumber) {
	return _colorBalanceGreen[colorNumber];
}
uint8_t SkaarhojBI8::getColorBlue(uint8_t colorNumber) {
	return _colorBalanceBlue[colorNumber];
}

void SkaarhojBI8::setDefaultColor(uint8_t defaultColorNumber) {
	if (_validColorNumber(defaultColorNumber))	_defaultColorNumber = defaultColorNumber;
}
void SkaarhojBI8::setButtonColor(uint8_t buttonNumber, uint8_t colorNumber) {
	_writeButtonLed(buttonNumber,colorNumber);
}
void SkaarhojBI8::setButtonColorsToDefault() {
	for(uint8_t i=1;i<=_numberOfButtons;i++)  {
	  _writeButtonLed(i,_defaultColorNumber);
	}
}
void SkaarhojBI8::testSequence() { testSequence(20); }
uint16_t SkaarhojBI8::testSequence(uint16_t delayTime) {
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


bool SkaarhojBI8::buttonUp(uint8_t buttonNumber) {	// Returns true if a button 1-16 is has just been released
	if (_validButtonNumber(buttonNumber))	{
		_readButtonStatus();

		uint16_t mask = (B1 << (buttonNumber-1));
		uint16_t buttonChange = (_buttonStatusLastUp ^ _buttonStatus) & mask;
		_buttonStatusLastUp ^= buttonChange;

		return (buttonChange & ~_buttonStatus) ? true : false;
	} else return false;
}
bool SkaarhojBI8::buttonDown(uint8_t buttonNumber) {	// Returns true if a button 1-16 is has just been pushed down
	if (_validButtonNumber(buttonNumber))	{
		_readButtonStatus();

		uint16_t mask = (B1 << (buttonNumber-1));
		uint16_t buttonChange = (_buttonStatusLastDown ^ _buttonStatus) & mask;
		_buttonStatusLastDown ^= buttonChange;

		return (buttonChange & _buttonStatus) ? true : false;
	} else return false;
}
bool SkaarhojBI8::buttonIsPressed(uint8_t buttonNumber) {	// Returns true if a button 1-16 is currently pressed
	if (_validButtonNumber(buttonNumber))	{
		return (buttonIsPressedAll() >> (buttonNumber-1)) & 1 ? true : false;
	} else return false;
}
bool SkaarhojBI8::buttonIsHeldFor(uint8_t buttonNumber, uint16_t timeout) {
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
bool SkaarhojBI8::buttonIsReleasedAgo(uint8_t buttonNumber, uint16_t timeout) {
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

uint32_t SkaarhojBI8::buttonAll() {
	_readButtonStatus();
	return (((uint32_t)buttonUpAll(false) << 16)| ((uint32_t) buttonDownAll(false) & 0xFFFF));
}

uint16_t SkaarhojBI8::buttonUpAll(bool read) {	// Returns a word where each bit indicates if a button 1-16 (bits 0-9) has been released since last check
	if(read)
		_readButtonStatus();
	
	uint16_t buttonChange = _buttonStatusLastUp ^ _buttonStatus;
	_buttonStatusLastUp = _buttonStatus;

	return buttonChange & ~_buttonStatus;
}
uint16_t SkaarhojBI8::buttonDownAll(bool read) {	// Returns a word where each bit indicates if a button 1-16 (bits 0-9) has been pressed since last check
	if(read)
		_readButtonStatus();
	
	uint16_t buttonChange = _buttonStatusLastDown ^ _buttonStatus;
	_buttonStatusLastDown = _buttonStatus;
	
	return buttonChange & _buttonStatus;
}

uint16_t SkaarhojBI8::buttonIsPressedAll() {	// Returns a word where each bit indicates if a button 1-16 (bits 0-9) is currently pressed since last check
	_readButtonStatus();
	
	return _buttonStatus;
}
bool SkaarhojBI8::isButtonIn(uint8_t buttonNumber, uint16_t allButtonsState)	{
	if (_validButtonNumber(buttonNumber))	{
		return (allButtonsState & (B1 << (buttonNumber-1))) ? true : false;
	}
	return false;
}



// Private methods:

void SkaarhojBI8::_writeButtonLed(uint8_t buttonNumber, uint8_t color)  {
	if (_validColorNumber(color) && _validButtonNumber(buttonNumber) && ((_buttonColorCache[(buttonNumber-1)] != color) || _disableColorCache))		{
		_buttonColorCache[(buttonNumber-1)] = color;
		
		uint8_t isNormal = _reverseButtons ? 0 : 1;
		if (!_RGBbuttons)	{
			if (_extendedBicolor && buttonNumber>8)	{
	      		_buttonLed2.setLEDDimmed((buttonNumber-9)*2+1*(1-isNormal),  _colorBalanceRed[color]);
			    _buttonLed2.setLEDDimmed((buttonNumber-9)*2+1*isNormal, _colorBalanceGreen[color]);
			}	else {
			    if((buttonNumber<=4 && !(buttonNumber==1 && _B1Alt)) || !_oldBI8)  {
			      _buttonLed.setLEDDimmed((buttonNumber-1)*2+1*(1-isNormal),  _colorBalanceRed[color]);
			      _buttonLed.setLEDDimmed((buttonNumber-1)*2+1*isNormal, _colorBalanceGreen[color]);
			    } else {
			      _buttonLed.setLEDDimmed((buttonNumber-1)*2+1*(1-isNormal),  _colorBalanceGreen[color]);
			      _buttonLed.setLEDDimmed((buttonNumber-1)*2+1*isNormal, _colorBalanceRed[color]);
			    }
			}
		} else {
				// isNormal is assumed not used for RGB boards:
			if (buttonNumber<9)	{
				_buttonLed.setLEDDimmed((buttonNumber-1)*2,  _colorBalanceGreen[color]);
				_buttonLed.setLEDDimmed((buttonNumber-1)*2+1, _colorBalanceRed[color]);
				_buttonLed2.setLEDDimmed((buttonNumber-1), _colorBalanceBlue[color]);
			} else if(buttonNumber==10)	{
				_buttonLed2.setLEDDimmed((buttonNumber-1)-1,  _colorBalanceGreen[color]);
				_buttonLed2.setLEDDimmed((buttonNumber-1), _colorBalanceRed[color]);
				_buttonLed2.setLEDDimmed((buttonNumber-1)+1, _colorBalanceBlue[color]);
			} else if(buttonNumber==9)	{
				_buttonLed2.setLEDDimmed((buttonNumber-1)+3,  _colorBalanceGreen[color]);
				_buttonLed2.setLEDDimmed((buttonNumber-1)+4, _colorBalanceRed[color]);
				_buttonLed2.setLEDDimmed((buttonNumber-1)+5, _colorBalanceBlue[color]);
			}
		}
	}
}

void SkaarhojBI8::_readButtonStatus() {	// Reads button status from MCP23017 chip.
	uint16_t buttonStatus = _buttonMux.digitalWordRead();
	_buttonStatus = buttonStatus >> 8;
	
	_buttonStatus = 
		((_buttonStatus & B10000) >> 4) | 	// B1
		((_buttonStatus & B100000) >> 4) | 	// B2
		((_buttonStatus & B1000000) >> 4) | 	// B3
		((_buttonStatus & B10000000) >> 4) | 	// B4
		((_buttonStatus & B1000) << 1) | 	// B5
		((_buttonStatus & B100) << 3) | 	// B6
		((_buttonStatus & B10) << 5) | 	// B7
		((_buttonStatus & B1) << 7) |	// B8
		((buttonStatus & B10000000) << 1) |	// B9
		((buttonStatus & B1000000) << 3);	// B10
		
	if (_extendedBicolor)	{
          uint16_t extraButtonStatus =
              ((buttonStatus & B10000) >> 4) |   // B1
              ((buttonStatus & B100000) >> 4) |  // B2
              ((buttonStatus & B1000000) >> 4) |   // B3
              ((buttonStatus & B10000000) >> 4) |  // B4
              ((buttonStatus & B1000) << 1) |      // B5
              ((buttonStatus & B100) << 3) |       // B6
              ((buttonStatus & B10) << 5) |        // B7
              ((buttonStatus & B1) << 7);
                _buttonStatus = (_buttonStatus & 0xFF) | (extraButtonStatus << 8);
	}
	
	if (_buttonStatus != _lastButtonStatus)	{
		_lastButtonReleased = (_lastButtonStatus ^ _buttonStatus) & ~_buttonStatus;
		_lastButtonStatus = _buttonStatus;
		_inputChangeTime = millis();
		_inputChangeTimeActive = true;	// Set true if it's ok to act on a trigger
	//	Serial.print("changed ");
	//	Serial.println(_lastButtonReleased, BIN);
	}
}

bool SkaarhojBI8::_validButtonNumber(uint8_t buttonNumber)	{	// Checks if a button number is valid (1-16)
	return (buttonNumber>=1 && buttonNumber <= _numberOfButtons);
}
bool SkaarhojBI8::_validColorNumber(uint8_t colorNumber)	{	// Checks if a color number is valid (1-10)
	return (colorNumber <= 9);
}
bool SkaarhojBI8::_validPercentage(uint8_t percentage)	{	// Checks if a value is within 0-100
	return (percentage <= 100);
}



void SkaarhojBI8::testProgramme(uint16_t buttonMask)	{
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

void SkaarhojBI8::clearButtonColorCache()	{
	for(uint8_t i=0; i<16;i++)	{
		_buttonColorCache[i] = 255;
	}
}	
