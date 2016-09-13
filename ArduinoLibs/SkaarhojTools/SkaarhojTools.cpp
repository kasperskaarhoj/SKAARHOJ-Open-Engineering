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



#include "Arduino.h"
#include "SkaarhojTools.h"
#include "Streaming.h"
#include "MemoryFree.h"


SkaarhojTools::SkaarhojTools(){
	SkaarhojTools(0);
}
SkaarhojTools::SkaarhojTools(uint8_t level){
	_serialOutput = level;
}

/**
 * Setter method: If _serialOutput is set, the library may use Serial.print() to give away information about its operation - mostly for debugging.
 * 0= no output
 * 1= normal output (info)
 * 2= verbose (development)
 * 3= verbose! (development, bugfixing)
 */
void SkaarhojTools::serialOutput(uint8_t level) {
	_serialOutput = level;
}
uint8_t SkaarhojTools::getSerialOutput() {
	return _serialOutput;
}

bool SkaarhojTools::serLevel(uint8_t level) {
	return _serialOutput>=level;
}

/**
 * Timeout check
 */
bool SkaarhojTools::hasTimedOut(unsigned long &time, const unsigned long timeout, bool updateTime)  {
//  if ((unsigned long)(time + timeout) <= (unsigned long)millis()-30000)  {  // This should "wrap around" if time+timout is larger than the size of unsigned-longs, right?
  if (millis()-time < 0x80000000 && millis()-time >= timeout)  {	// The last clause is necessary in case time > millis()
	  if (!(millis()-time >= timeout)) Serial << millis() << F("-") << time << F("=") << timeout << F("*******************\n");
	if (updateTime)	time = millis();
    return true;
  } 
  else {
    return false;
  }
}

/**
 * A way we measure how many times the run-loop has been passed over a second: (debugging)
 * [Called from loop()]
 */
void SkaarhojTools::runLoopCount()  {
  static unsigned long runLoopCountTime = 0;
  static unsigned int runLoopCounter = 0;

	runLoopCounter++; 
	if (hasTimedOut(runLoopCountTime, 1000)) {
	  unsigned long timePassed = millis()-runLoopCountTime;
	  if (timePassed!=0)  {
	    if (serLevel(1))	Serial << F("RunLoopCount: ") << (((float)runLoopCounter)/timePassed*1000) << F(" (") << runLoopCounter << F(" in ") << timePassed << F("ms)") << F(" freeMemory()=") << freeMemory() << "\n";
	    runLoopCountTime = millis();
	    runLoopCounter = 0; 
	  }
	} 
}