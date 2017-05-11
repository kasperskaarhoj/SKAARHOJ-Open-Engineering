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

// Configured modes:
// 0: 0-7 outputs, 8-15 inputs
// 1: 0-15 outputs
// 2: 0-15 inputs
void SkaarhojGPIO2x8::begin(uint8_t boardAddress, uint8_t mode) {
	// NOTE: Wire.h should definitely be initialized at this point! (Wire.begin())
	
	_boardAddress = boardAddress & B111;	// 0-3
	_mode = mode;

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
	
	// Setting up inputs and outputs
	for(uint8_t i=0; i<8; i++) {
		_GPIOchip.pinMode(i,  (_mode==2?INPUT:OUTPUT));
		_GPIOchip.pinMode(i+8,(_mode==1?OUTPUT:INPUT));
	}
}
bool SkaarhojGPIO2x8::inputUp(uint8_t inputNumber) {	// Returns true if a input 1-8 is has just been released
	if (_validInputNumber(inputNumber))	{
		_readInputStatus();

		uint16_t mask = (B1 << (inputNumber-1));
		uint16_t inputChange = (_inputStatusLastUp ^ _inputStatus) & mask;
		_inputStatusLastUp ^= inputChange;

		return (inputChange & ~_inputStatus) ? true : false;
	} else return false;
}
bool SkaarhojGPIO2x8::inputDown(uint8_t inputNumber) {	// Returns true if a input 1-8 is has just been pushed down
	if (_validInputNumber(inputNumber))	{
		_readInputStatus();

		uint16_t mask = (B1 << (inputNumber-1));
		uint16_t inputChange = (_inputStatusLastDown ^ _inputStatus) & mask;
		_inputStatusLastDown ^= inputChange;

		return (inputChange & _inputStatus) ? true : false;
	} else return false;
}
bool SkaarhojGPIO2x8::inputIsActive(uint8_t inputNumber) {	// Returns true if a input 1-8 is currently active
	if (_validInputNumber(inputNumber))	{
		_readInputStatus();
		return (SkaarhojGPIO2x8::inputIsActiveAll() >> (inputNumber-1)) ? true : false;
	} else return false;
}
uint16_t SkaarhojGPIO2x8::inputUpAll() {	// Returns a byte where each bit indicates if a input 1-8 (bits 0-7) has been released since last check
	_readInputStatus();
	
	uint16_t inputChange = _inputStatusLastUp ^ _inputStatus;
	_inputStatusLastUp = _inputStatus;

	return inputChange & ~_inputStatus;
}
uint16_t SkaarhojGPIO2x8::inputDownAll() {	// Returns a byte where each bit indicates if a input 1-8 (bits 0-7) has been made active since last check
	_readInputStatus();
	
	uint16_t inputChange = _inputStatusLastDown ^ _inputStatus;
	_inputStatusLastDown = _inputStatus;
	
	return inputChange & _inputStatus;
}
uint16_t SkaarhojGPIO2x8::inputIsActiveAll() {	// Returns a byte where each bit indicates if a input 1-8 (bits 0-7) is currently active
	_readInputStatus();
	
	return _inputStatus;
}
bool SkaarhojGPIO2x8::isInputIn(uint8_t inputNumber, uint16_t allInputsState)	{
	if (_validInputNumber(inputNumber))	{
		return (allInputsState & (B1 << (inputNumber-1))) ? true : false;
	}
	return false;
}

void SkaarhojGPIO2x8::setOutput(uint8_t outputNumber, bool state) {
	if (_validInputNumber(outputNumber))	{
		_GPIOchip.digitalWrite(outputNumber-1,state);
	}
}
void SkaarhojGPIO2x8::setOutputAll(uint16_t states)	{
	_GPIOchip.digitalWordWrite(states);
}



// Private methods:
void SkaarhojGPIO2x8::_readInputStatus() {	// Reads input status from MCP23017 chip.
	uint16_t inputStatus = _GPIOchip.digitalWordRead();
	switch(_mode) {
		case 0:
			_inputStatus = ~(inputStatus >> 8);
			break;
		case 1:
			_inputStatus = 0;
			break;
		case 2:
			_inputStatus = ~(inputStatus >> 8 | (inputStatus << 8));
			break;
	}
}

bool SkaarhojGPIO2x8::_validInputNumber(uint8_t inputNumber)	{	// Checks if a input number is valid
	return (inputNumber>=1 && inputNumber <= 16);
}
