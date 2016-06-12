/*  PCA9685 LED library for Arduino
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
#include "PCA9685.h"

PCA9685::PCA9685() {}

void PCA9685::begin(int i2cAddress) {
	_i2cAddress = PCA9685_I2C_BASE_ADDRESS | (i2cAddress & B00111111);
}
bool PCA9685::init(bool reverseDrive) {

	delay(1);
	writeRegister(PCA9685_MODE1, (byte)0x01);	// reset the device

	delay(1);
	bool isOnline;
	if (readRegister(PCA9685_MODE1)==0x01)	{
		isOnline = true;
	} else {
		isOnline = false;
	}
	writeRegister(PCA9685_MODE1, (byte)B10100000);	// set up for auto increment
	writeRegister(PCA9685_MODE2, (byte)0x10 + (reverseDrive?4:0));	// set to output drive type
	
	return isOnline;
}

void PCA9685::setLEDOn(int ledNumber) {
	writeLED(ledNumber, 0x1000, 0);
}

void PCA9685::setLEDOff(int ledNumber) {
	writeLED(ledNumber, 0, 0x1000);
}

void PCA9685::setLEDDimmed(int ledNumber, byte amount) {		// Amount from 0-100 (off-on)
	if (amount==0)	{
		setLEDOff(ledNumber);
	} else if (amount>=100) {
		setLEDOn(ledNumber);
	} else {
		int randNumber = (int)random(4096);	// Randomize the phaseshift to distribute load. Good idea? Hope so.
		writeLED(ledNumber, randNumber, (int)(amount*41+randNumber) & 0xFFF);
	}
}

void PCA9685::writeLED(int ledNumber, word LED_ON, word LED_OFF) {	// LED_ON and LED_OFF are 12bit values (0-4095); ledNumber is 0-15
	if (ledNumber >=0 && ledNumber <= 15)	{
		
		Wire.beginTransmission(_i2cAddress);
		Wire.write(PCA9685_LED0 + 4*ledNumber);

		Wire.write(lowByte(LED_ON));
		Wire.write(highByte(LED_ON));
		Wire.write(lowByte(LED_OFF));
		Wire.write(highByte(LED_OFF));
		
		Wire.endTransmission();
	}
}


//PRIVATE
void PCA9685::writeRegister(int regAddress, byte data) {
	Wire.beginTransmission(_i2cAddress);
	Wire.write(regAddress);
	Wire.write(data);
	Wire.endTransmission();
}

word PCA9685::readRegister(int regAddress) {
	word returnword = 0x00;
	Wire.beginTransmission(_i2cAddress);
	Wire.write(regAddress);
	Wire.endTransmission();
	Wire.requestFrom((int)_i2cAddress, 1);
    
//    int c=0;
	//Wait for our 2 bytes to become available
	while (Wire.available()) {
        //high byte
//        if (c==0)   { returnword = Wire.read() << 8; }
        //low byte
  //      if (c==1)   { 
		returnword |= Wire.read(); 
		//}
        //c++;
    }
    
	return returnword;
}