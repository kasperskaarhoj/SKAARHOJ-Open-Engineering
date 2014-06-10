/*  SkaarhojGPIO2x8 Arduino library for the GPIO2x8 board from SKAARHOJ.com
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
#include "SkaarhojGPIO2x8.h"


SkaarhojGPIO2x8::SkaarhojGPIO2x8(){}	// Empty constructor.
void SkaarhojGPIO2x8::begin() {
	return SkaarhojGPIO2x8::begin(0);
}
void SkaarhojGPIO2x8::begin(uint8_t boardAddress) {
	// NOTE: Wire.h should definitely be initialized at this point! (Wire.begin())
	
	_boardAddress = boardAddress & B111;	// 0-3

		// Initializing:
	_inputStatus = 0;
	_inputStatusLastUp = 0;
	_inputStatusLastDown = 0;
		
		// Create object for reading inputs and outputs from MCP23017
	MCP23017 GPIOchip;
	_GPIOchip = GPIOchip;
	_GPIOchip.begin((int)(B00 | _boardAddress));
	
	_GPIOchip.internalPullupMask(65535);	// All has pull-up
	_GPIOchip.inputPolarityMask(0);
	
	// Setting up the outputs on the board:
	_GPIOchip.pinMode(0,  OUTPUT);
	_GPIOchip.pinMode(1,  OUTPUT);
	_GPIOchip.pinMode(2,  OUTPUT);
	_GPIOchip.pinMode(3,  OUTPUT);
	_GPIOchip.pinMode(4,  OUTPUT);
	_GPIOchip.pinMode(5,  OUTPUT);
	_GPIOchip.pinMode(6,  OUTPUT);
	_GPIOchip.pinMode(7,  OUTPUT);

	// Setting up the inputs on the board:
	_GPIOchip.pinMode(8,  INPUT);
	_GPIOchip.pinMode(9,  INPUT);
	_GPIOchip.pinMode(10,  INPUT);
	_GPIOchip.pinMode(11,  INPUT);
	_GPIOchip.pinMode(12,  INPUT);
	_GPIOchip.pinMode(13,  INPUT);
	_GPIOchip.pinMode(14,  INPUT);
	_GPIOchip.pinMode(15,  INPUT);	
}
bool SkaarhojGPIO2x8::inputUp(int inputNumber) {	// Returns true if a input 1-8 is has just been released
	if (_validInputNumber(inputNumber))	{
		_readInputStatus();

		uint8_t mask = (B1 << (inputNumber-1));
		uint8_t inputChange = (_inputStatusLastUp ^ _inputStatus) & mask;
		_inputStatusLastUp ^= inputChange;

		return (inputChange & ~_inputStatus) ? true : false;
	} else return false;
}
bool SkaarhojGPIO2x8::inputDown(int inputNumber) {	// Returns true if a input 1-8 is has just been pushed down
	if (_validInputNumber(inputNumber))	{
		_readInputStatus();

		uint8_t mask = (B1 << (inputNumber-1));
		uint8_t inputChange = (_inputStatusLastDown ^ _inputStatus) & mask;
		_inputStatusLastDown ^= inputChange;

		return (inputChange & _inputStatus) ? true : false;
	} else return false;
}
bool SkaarhojGPIO2x8::inputIsActive(int inputNumber) {	// Returns true if a input 1-8 is currently active
	if (_validInputNumber(inputNumber))	{
		_readInputStatus();
		return (SkaarhojGPIO2x8::inputIsActiveAll() >> (inputNumber-1)) ? true : false;
	} else return false;
}
uint8_t SkaarhojGPIO2x8::inputUpAll() {	// Returns a byte where each bit indicates if a input 1-8 (bits 0-7) has been released since last check
	_readInputStatus();
	
	uint8_t inputChange = _inputStatusLastUp ^ _inputStatus;
	_inputStatusLastUp = _inputStatus;

	return inputChange & ~_inputStatus;
}
uint8_t SkaarhojGPIO2x8::inputDownAll() {	// Returns a byte where each bit indicates if a input 1-8 (bits 0-7) has been made active since last check
	_readInputStatus();
	
	uint8_t inputChange = _inputStatusLastDown ^ _inputStatus;
	_inputStatusLastDown = _inputStatus;
	
	return inputChange & _inputStatus;
}
uint8_t SkaarhojGPIO2x8::inputIsActiveAll() {	// Returns a byte where each bit indicates if a input 1-8 (bits 0-7) is currently active
	_readInputStatus();
	
	return _inputStatus;
}
bool SkaarhojGPIO2x8::isInputIn(int inputNumber, uint8_t allInputsState)	{
	if (_validInputNumber(inputNumber))	{
		return (allInputsState & (B1 << (inputNumber-1))) ? true : false;
	}
	return false;
}

void SkaarhojGPIO2x8::setOutput(int outputNumber, bool state) {
	if (_validInputNumber(outputNumber))	{
		_GPIOchip.digitalWrite(outputNumber-1,state);
	}
}
void SkaarhojGPIO2x8::setOutputAll(uint8_t states)	{
	_GPIOchip.digitalWordWrite(states);
}



// Private methods:
void SkaarhojGPIO2x8::_readInputStatus() {	// Reads input status from MCP23017 chip.
	word inputStatus = _GPIOchip.digitalWordRead();
	_inputStatus = ~(inputStatus >> 8);
}

bool SkaarhojGPIO2x8::_validInputNumber(int inputNumber)	{	// Checks if a input number is valid (1-8)
	return (inputNumber>=1 && inputNumber <= 8);
}
