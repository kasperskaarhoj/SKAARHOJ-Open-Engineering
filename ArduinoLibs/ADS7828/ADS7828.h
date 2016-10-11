/*  ADS7828 library for Arduino
    Copyright (C) 2012 Kasper Skaarhoj <kasperskaarhoj@gmail.com>

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
#ifndef ADS7828_H
#define ADS7828_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif


#include "Wire.h"


#define I2C_BASE_ADDRESS 0x90

class ADS7828
{
  public:
    //NB the i2c address here is the value of the A0, A1 pins ONLY
    //as the class takes care of its internal base address.
    //so i2cAddress should be between 0 and 3
    ADS7828();
    void init(int i2cAddress);

    //These functions provide an 'arduino'-like functionality for accessing
    //pin states:

	word analogRead();
	word analogRead(uint8_t channel, uint8_t oversampling=0);	// Oversampling from 0-4 (4=16 times, no more!!), 16 times = 9 ms, 1 time = 0,5ms
	
	//Our actual i2c address
	byte _i2cAddress;
};
#endif 
