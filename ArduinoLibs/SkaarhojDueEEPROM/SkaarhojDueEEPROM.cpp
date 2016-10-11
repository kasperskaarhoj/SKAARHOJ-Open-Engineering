/*
Copyright 2014 Kasper Skårhøj, SKAARHOJ, kasper@skaarhoj.com

This file is part of the Skaarhoj EEPROM library for Arduino Due

The SkaarhojDueEEPROM library is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published by the 
Free Software Foundation, either version 3 of the License, or (at your 
option) any later version.

The SkaarhojDueEEPROM library is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
or FITNESS FOR A PARTICULAR PURPOSE. 
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along 
with the SkaarhojDueEEPROM library. If not, see http://www.gnu.org/licenses/.


IMPORTANT: If you want to use this library in your own projects and/or products,
please play a fair game and heed the license rules! See our web page for a Q&A so
you can keep a clear conscience: http://skaarhoj.com/about/licenses/


*/
#include "SkaarhojDueEEPROM.h"


/**
 * Constructor
 */
SkaarhojDueEEPROM::SkaarhojDueEEPROM() {
	_deviceaddress = 87;
  _pageAddress = 0xFFFF;
}

void SkaarhojDueEEPROM::writeBuffered(uint16_t address, uint8_t value) {
  if(_pageAddress != 0xFFFF) {
    if((address & ~31) != _pageAddress) {
      commitPage();
    }
  }

  if(_pageAddress == 0xFFFF) {
    _pageAddress = address & ~31;
    readPage(_pageAddress, _pageBuffer);
  }

  _pageBuffer[address & 31] = value;
}

void SkaarhojDueEEPROM::commitPage() {
  if(_pageAddress != 0xFFFF) {
    writePage(_pageAddress, _pageBuffer);
    _pageAddress = 0xFFFF;
  }
}

void SkaarhojDueEEPROM::write(uint16_t address, uint8_t value) {
  Wire.beginTransmission(_deviceaddress);
  Wire.write(address >> 8); // MSB
  Wire.write(address & 0xFF); // LSB
  Wire.write(value);
  Wire.endTransmission();
  
  uint32_t t = millis();
  while(Wire.endTransmission() != 0);
  Serial.print("EEPROM byte write took: ");
  Serial.print(millis() - t);
  Serial.println(" ms");
}

uint8_t SkaarhojDueEEPROM::read(uint16_t address) {
  Wire.beginTransmission(_deviceaddress);
  Wire.write(address >> 8); // MSB
  Wire.write(address & 0xFF); // LSB
  Wire.endTransmission();
  Wire.requestFrom(_deviceaddress, (uint8_t)1);
  if (Wire.available()) {
    return Wire.read();
  } else return 0xFF;
}

void SkaarhojDueEEPROM::writePage(uint16_t address, uint8_t * valueArray) {	// 30 bytes array (not 32, because last two bytes doesn't get written correctly for some reason - maybe the I2C buffers size on ARduino?)
  Wire.beginTransmission(_deviceaddress);
  Wire.write(address >> 8); // MSB
  Wire.write(address & 0xE0); // LSB

  for(uint8_t i=0; i<I2C_BUFFER; i++)	{
	  Wire.write(valueArray[i]);
  }

  Wire.endTransmission();

  uint32_t t = millis();
  while(Wire.endTransmission() != 0);
  Serial.print("EEPROM page write took: ");
  Serial.print(millis() - t);
  Serial.println(" ms");
}

void SkaarhojDueEEPROM::readPage(uint16_t address, uint8_t * valueArray) {	// 30 bytes array
	//delay(5);  	// Without this delay it has been seen that it a) could stall for up to 2 seconds in reading and b) that the read values are bogus!
  Wire.beginTransmission(_deviceaddress);
  Wire.write(address >> 8); // MSB
  Wire.write(address & 0xE0); // LSB
  Wire.endTransmission();

  Wire.requestFrom(_deviceaddress, (uint8_t)I2C_BUFFER);

  for(uint8_t i=0; i<I2C_BUFFER; i++)	{
	  valueArray[i] = Wire.read();
  }
}