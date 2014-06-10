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
#ifndef SkaarhojGPIO2x8_H
#define SkaarhojGPIO2x8_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "Wire.h"
#include "MCP23017.h"

/**
  Version 1.0.0
	(Semantic Versioning)
**/

class SkaarhojGPIO2x8
{
  private:
	uint8_t _boardAddress;
	MCP23017 _GPIOchip;
	uint8_t _inputStatus;
	uint8_t _inputStatusLastUp;
	uint8_t _inputStatusLastDown;

  public:
	SkaarhojGPIO2x8();
	void begin(uint8_t boardAddress);
	void begin();
	
	bool inputUp(int inputNumber);
	bool inputDown(int inputNumber);
	bool inputIsActive(int inputNumber);
	uint8_t inputUpAll();
	uint8_t inputDownAll();
	uint8_t inputIsActiveAll();
	bool isInputIn(int inputNumber, uint8_t allInputsState);
	
	void setOutput(int outputNumber, bool state);
	void setOutputAll(uint8_t states);
	
  private:
	void _readInputStatus();
	bool _validInputNumber(int inputNumber);
};
#endif 
