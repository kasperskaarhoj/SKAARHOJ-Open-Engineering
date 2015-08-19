/*  SkaarhojAnalog Arduino library with various utilities for products from SKAARHOJ.com
    Copyright (C) 2015 Kasper Skårhøj    <kasperskaarhoj@gmail.com> 

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
#include "SkaarhojAnalog.h"


SkaarhojAnalog::SkaarhojAnalog(){
	_debugMode = false;
}	// Empty constructor.
void SkaarhojAnalog::debugMode()	{
	_debugMode=true;	
}






/**
 * Joystick
 * i2cAddress: 0-3
 */
void SkaarhojAnalog::joystick_init(int tolerance, uint8_t i2cAddress, uint8_t index)	{

	ADS7828 analogConv; // Address
	_analogConv = analogConv;
	_analogConv.init(i2cAddress);

	_joystick_index = index;
	_joystick_tolerance = tolerance; 
	
		// Internal variables during operation:
	_joystick_previousPosition[0] = -1000;
	_joystick_previousPosition[1] = -1000;
	_joystick_previousPosition[2] = -1000;
	_joystick_previousValue[0] = -1000;
	_joystick_previousValue[1] = -1000;
	_joystick_previousValue[2] = -1000;
	
		// Reset:
	_joystick_centerValue[0] = joystick_AnalogRead(0);
	_joystick_centerValue[1] = joystick_AnalogRead(1);
	_joystick_centerValue[2] = joystick_AnalogRead(2);
	joystick_hasMoved(0);
	joystick_hasMoved(1);
	joystick_hasMoved(2);
	
	Serial.print("Center values: ");
	Serial.print(_joystick_centerValue[0]);
	Serial.print(",");
	Serial.print(_joystick_centerValue[1]);
	Serial.print(",");
	Serial.print(_joystick_centerValue[2]);
	Serial.println();
}

bool SkaarhojAnalog::joystick_hasMoved(uint8_t index)	{	// Index is 0 or 1 or 2
	if (index <=2)	{
		int potValue = joystick_AnalogRead(index)-_joystick_centerValue[index];

		if (potValue >= _joystick_previousValue[index]+_joystick_tolerance || potValue <= _joystick_previousValue[index]-_joystick_tolerance)  {

			_joystick_previousValue[index] = potValue;

			int joystickPosition = potValue;
			if (potValue<0)	{
				joystickPosition = (long)100*(long)joystickPosition/(long)_joystick_centerValue[index];
			} else {
				joystickPosition = (long)100*(long)joystickPosition/(long)(1023-_joystick_centerValue[index]);
			}
			if (joystickPosition>97) joystickPosition=100;
			if (joystickPosition<-97) joystickPosition=-100;
			if (joystickPosition < 3 && joystickPosition > -3)	joystickPosition=0;
		
			if (_joystick_previousPosition[index]!=joystickPosition)  {
				_joystick_previousPosition[index]=-joystickPosition;
				return true;
			}
		}
		return false;
	}
}

int SkaarhojAnalog::joystick_position(uint8_t index)	{
	return _joystick_previousPosition[index];
}
bool SkaarhojAnalog::joystick_buttonUp() {
	bool buttonChange = (_joystick_buttonStatusLastUp ^ joystick_buttonIsPressed());
	_joystick_buttonStatusLastUp ^= buttonChange;

	return (buttonChange & ~_joystick_buttonStatus) ? true : false;
}
bool SkaarhojAnalog::joystick_buttonDown() {
	bool buttonChange = (_joystick_buttonStatusLastDown ^ joystick_buttonIsPressed());
	_joystick_buttonStatusLastDown ^= buttonChange;

	return (buttonChange & _joystick_buttonStatus) ? true : false;
}
bool SkaarhojAnalog::joystick_buttonIsPressed() {
	_joystick_buttonStatus = joystick_AnalogRead(3)<100;
	return _joystick_buttonStatus;
}
int SkaarhojAnalog::joystick_AnalogRead(uint8_t index)	{
	switch(index)	{
		case 0:
			return _analogConv.analogRead(2+(_joystick_index?4:0))>>2;
		break;
		case 1:
			return _analogConv.analogRead(3+(_joystick_index?4:0))>>2;
		break;
		case 2:
			return _analogConv.analogRead(1+(_joystick_index?4:0))>>2;
		break;
		case 3:
			return _analogConv.analogRead(0+(_joystick_index?4:0))>>2;
		break;
		default:
			return -1;
		break;
	}
}
