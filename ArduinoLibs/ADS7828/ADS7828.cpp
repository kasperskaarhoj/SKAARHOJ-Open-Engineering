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
#include "ADS7828.h"

ADS7828::ADS7828(int i2cAddress) {
	_i2cAddress = (I2C_BASE_ADDRESS >>1) | (i2cAddress & 0x03);
}

void ADS7828::init() {
	Wire.begin();
}

word ADS7828::analogRead(uint8_t channel) {
	word returnword = 0x00;
	Wire.beginTransmission(_i2cAddress);
	Wire.write(B10000100 | ((channel&7) << 3));
	Wire.endTransmission();
	
	Wire.requestFrom((int)_i2cAddress, 2);

	// Wait for our 2 bytes to become available
	int c=0;
	while(Wire.available())    // slave may send less than requested
	  { 
		if (c==0)	{
			//high byte
			returnword = Wire.read() << 8;
		}
		if (c==1)	{
			//low byte
			returnword |= Wire.read();
		}
		c++;
	  }
	return returnword;
} 
word ADS7828::analogRead() {
	return analogRead(0);
} 

