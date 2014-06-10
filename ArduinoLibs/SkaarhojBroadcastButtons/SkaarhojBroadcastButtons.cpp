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
#include "SkaarhojBroadcastButtons.h"


SkaarhojBroadcastButtons::SkaarhojBroadcastButtons(){}	// Empty constructor.

void SkaarhojBroadcastButtons::begin() { begin(false); }
void SkaarhojBroadcastButtons::begin(bool fourButtons) {

		// Can be a number from 2,3,4,5 indicating which of the button configurations are used "B2A,B2B", "B3A-D", "B4", "B5A,B5B"
	_fourButtons = fourButtons;	
	
		// Initializing:
	_buttonStatus = 0;
	_buttonStatusLastUp = 0;
	_buttonStatusLastDown = 0;
		
		// Rate from 0-100 for color numbers: Off(0), On(1), Red(2), Green(3), Amber(4), Backlit(5), (off....)
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
	
		// Used to track last used color in order to NOT write colors to buttons if they already have that color (writing same color subsequently will make the LED blink weirdly because each time a new timing scheme is randomly created in the PCA9685)
	_buttonColorCache[0] = 255;
	_buttonColorCache[1] = 255;
	_buttonColorCache[2] = 255;
	_buttonColorCache[3] = 255;
	
	_defaultColorNumber = 5;
	
	pinMode(3, OUTPUT); 
  	pinMode(5, OUTPUT); 
  	pinMode(6, OUTPUT); 
  	pinMode(9, OUTPUT); 
  
		
		// Outputs:
	setButtonColorsToDefault();
}
void SkaarhojBroadcastButtons::setColorBalance(int colorNumber, int redPart, int greenPart) {
	if (_validColorNumber(colorNumber) && _validPercentage(redPart) && _validPercentage(greenPart))	{
		_colorBalanceRed[colorNumber] = redPart;
		_colorBalanceGreen[colorNumber] = greenPart;
	}
}
void SkaarhojBroadcastButtons::setDefaultColor(int defaultColorNumber) {
	if (_validColorNumber(defaultColorNumber))	_defaultColorNumber = defaultColorNumber;
}
void SkaarhojBroadcastButtons::setButtonColor(int buttonNumber, int colorNumber) {
	_writeButtonLed(buttonNumber,colorNumber);
}
void SkaarhojBroadcastButtons::setButtonColorsToDefault() {
	for(int i=1;i<=8;i++)  {
	  _writeButtonLed(i,_defaultColorNumber);
	}
}
void SkaarhojBroadcastButtons::testSequence() { testSequence(20); }
void SkaarhojBroadcastButtons::testSequence(int delayTime) {
	// Test LEDS:
  for(int ii=0;ii<=9;ii++)  {
    for(int i=1;i<=4;i++)  {
      _writeButtonLed(i,ii);
      delay(delayTime);
    }
    delay(delayTime*3);
  }
	setButtonColorsToDefault();
}


bool SkaarhojBroadcastButtons::buttonUp(int buttonNumber) {	// Returns true if a button 1-8 is has just been released
	if (_validButtonNumber(buttonNumber))	{
		_readButtonStatus();

		uint8_t mask = (B1 << (buttonNumber-1));
		uint8_t buttonChange = (_buttonStatusLastUp ^ _buttonStatus) & mask;
		_buttonStatusLastUp ^= buttonChange;

		return (buttonChange & ~_buttonStatus) ? true : false;
	} else return false;
}
bool SkaarhojBroadcastButtons::buttonDown(int buttonNumber) {	// Returns true if a button 1-8 is has just been pushed down
	if (_validButtonNumber(buttonNumber))	{
		_readButtonStatus();

		uint8_t mask = (B1 << (buttonNumber-1));
		uint8_t buttonChange = (_buttonStatusLastDown ^ _buttonStatus) & mask;
		_buttonStatusLastDown ^= buttonChange;

		return (buttonChange & _buttonStatus) ? true : false;
	} else return false;
}
bool SkaarhojBroadcastButtons::buttonIsPressed(int buttonNumber) {	// Returns true if a button 1-8 is currently pressed
	if (_validButtonNumber(buttonNumber))	{
		_readButtonStatus();
		return (_buttonStatus >> (buttonNumber-1)) ? true : false;
	} else return false;
}



// Private methods:

void SkaarhojBroadcastButtons::_writeButtonLed(int buttonNumber, int color)  {
	if (_validColorNumber(color) && _validButtonNumber(buttonNumber) && _buttonColorCache[(buttonNumber-1)] != color)		{
		_buttonColorCache[(buttonNumber-1)] = color;
		
//		float redValue = _colorBalanceRed[color];
//		redValue = redValue/100*255;
		
		uint8_t redValue = 255-(float)_colorBalanceRed[color]/100*255;
		uint8_t greenValue = 255-(float)_colorBalanceGreen[color]/100*255;
		
		if (_fourButtons)	{
			if (buttonNumber==1)	{	// B3A:
				analogWrite(3,redValue);
			}
			if (buttonNumber==2)	{	// B3B:
				analogWrite(5,redValue);
			}
			if (buttonNumber==3)	{	// B3C:
				analogWrite(6,redValue);
			}
			if (buttonNumber==4)	{	// B3D:
				analogWrite(9,redValue);
			}
		} else {
			if (buttonNumber==1)	{	// B(2/4/5)A:
				analogWrite(3,greenValue);
				analogWrite(5,redValue);
			}
			if (buttonNumber==2)	{	// B(2/5)B:
				analogWrite(6,redValue);
				analogWrite(9,greenValue);
			}
		}
	}
}

void SkaarhojBroadcastButtons::_readButtonStatus() {	// Reads button status:

	_buttonStatus = (digitalRead(7) ? 1 : 0) + (digitalRead(8) ? 2 : 0) + (digitalRead(2) ? 8 : 0) + (digitalRead(4) ? 4 : 0);

		// Leveling out rebounce:
	delay(1);
	_buttonStatus = _buttonStatus & ((digitalRead(7) ? 1 : 0) + (digitalRead(8) ? 2 : 0) + (digitalRead(2) ? 8 : 0) + (digitalRead(4) ? 4 : 0));
	delay(1);
	_buttonStatus = _buttonStatus & ((digitalRead(7) ? 1 : 0) + (digitalRead(8) ? 2 : 0) + (digitalRead(2) ? 8 : 0) + (digitalRead(4) ? 4 : 0));
}

bool SkaarhojBroadcastButtons::_validButtonNumber(int buttonNumber)	{	// Checks if a button number is valid (1-4)
	return (buttonNumber>=1 && buttonNumber <= 4);
}
bool SkaarhojBroadcastButtons::_validColorNumber(int colorNumber)	{	// Checks if a color number is valid (1-8)
	return (colorNumber>=0 && colorNumber <= 9);
}
bool SkaarhojBroadcastButtons::_validPercentage(int percentage)	{	// Checks if a value is within 0-100
	return (percentage>=0 && percentage <= 100);
}

