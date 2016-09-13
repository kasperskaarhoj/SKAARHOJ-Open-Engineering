/*
Copyright 2014 Kasper Skårhøj, SKAARHOJ, kasper@skaarhoj.com

This file is part of the Skaarhoj Tools library for Arduino

The SkaarhojBufferTools library is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published by the 
Free Software Foundation, either version 3 of the License, or (at your 
option) any later version.

The SkaarhojBufferTools library is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
or FITNESS FOR A PARTICULAR PURPOSE. 
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along 
with the SkaarhojBufferTools library. If not, see http://www.gnu.org/licenses/.


IMPORTANT: If you want to use this library in your own projects and/or products,
please play a fair game and heed the license rules! See our web page for a Q&A so
you can keep a clear conscience: http://skaarhoj.com/about/licenses/

*/


#ifndef SkaarhojTools_h
#define SkaarhojTools_h

#include "Arduino.h"

//  #include "SkaarhojPgmspace.h"  - 23/2 2014


class SkaarhojTools
{
private:
	uint8_t _serialOutput;		// If set, the library will print status/debug information to the Serial object. There are various levels. (0-3)

  public:
  	SkaarhojTools();
	SkaarhojTools(uint8_t level);
	void serialOutput(uint8_t level); // Sets whether informative (debug) serial output is enabled or not. Level=1: Normal, Level=2: Development
	uint8_t getSerialOutput();
	bool serLevel(uint8_t level);
	bool hasTimedOut(unsigned long &time, const unsigned long timeout, bool updateTime=false);
	void runLoopCount();
	template<typename T> T shapeInt(T inputInteger, T low, T high);
};

/**
 * Integer-in-range function
 */
template<typename T> T SkaarhojTools::shapeInt(T inputInteger, T low, T high)  {
  if (inputInteger<low)  return low;
  if (inputInteger>high)  return high;
  return inputInteger;
}

#endif

