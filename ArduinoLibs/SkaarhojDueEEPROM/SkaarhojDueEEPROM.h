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
#ifndef SkaarhojDueEEPROM_H
#define SkaarhojDueEEPROM_H

<<<<<<< HEAD
#include "Arduino.h"

=======
>>>>>>> origin/master
#include "Wire.h"

class SkaarhojDueEEPROM
{

  private:
	uint8_t _deviceaddress;
		
  public:
	SkaarhojDueEEPROM();
	
	uint8_t read(uint16_t address);
	void write(uint16_t address, uint8_t value);
	void writePage(uint16_t address, uint8_t * valueArray);
	void readPage(uint16_t address, uint8_t * valueArray);

};
#endif 
