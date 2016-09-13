/*
Copyright 2014 Kasper Skårhøj, SKAARHOJ, kasper@skaarhoj.com

This file is part of the Skaarhoj Encoders library for Arduino

The SkaarhojEncoders library is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published by the 
Free Software Foundation, either version 3 of the License, or (at your 
option) any later version.

The SkaarhojEncoders library is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
or FITNESS FOR A PARTICULAR PURPOSE. 
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along 
with the SkaarhojEncoders library. If not, see http://www.gnu.org/licenses/.


IMPORTANT: If you want to use this library in your own projects and/or products,
please play a fair game and heed the license rules! See our web page for a Q&A so
you can keep a clear conscience: http://skaarhoj.com/about/licenses/


*/
#ifndef SkaarhojEncoders_H
#define SkaarhojEncoders_H

#include "Arduino.h"
#include "MCP23017.h"
#include <Streaming.h>



class SkaarhojEncoders
{

  private:
	MCP23017 _GPIOchip;
	  
	uint32_t _interruptStateTime[5];
	uint32_t _stateCheckTime[5];
	int16_t _interruptStateNum[5];
	int16_t _interruptStateNumMem[5];
	int16_t _interruptStateLastCount[5];

	uint32_t _pushOnMillis[5];
	bool _countOn[5];
	bool _pushOn[5];
	bool _pushOnTriggerTimeFired[5];
	bool _isPushed[5];
	bool _isReverseMode;
	uint16_t _stateCheckDelay;

	uint8_t _serialOutput;
		
  public:
	SkaarhojEncoders();
	void begin(uint8_t address);
	void runLoop();
	
		// Encoder functions:
	bool reset(uint8_t encNum);
	int16_t state(uint8_t encNum);
	int16_t state(uint8_t encNum, uint32_t buttonPushTriggerDelay);
	int16_t lastCount(uint8_t encNum,uint8_t boost=0);
	int16_t totalCount(uint8_t encNum);
	void setReverseMode(bool mode);
	
	bool hasTimedOut(uint32_t time, uint32_t timeout);
	void setStateCheckDelay(uint16_t delayTime);
	void serialOutput(uint8_t level);
	
	void testProgramme(uint8_t buttonMask);
};
#endif 
