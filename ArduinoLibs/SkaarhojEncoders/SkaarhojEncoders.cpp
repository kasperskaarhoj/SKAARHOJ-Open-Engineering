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
#include "SkaarhojEncoders.h"


/**
 * Constructor
 */
SkaarhojEncoders::SkaarhojEncoders() {
	for (uint8_t i=0; i<5; i++)	{
		_countOn[i] = false;
		_pushOn[i] = false;
		_pushOnTriggerTimeFired[i] = false;
		_interruptStateNum[i] = 0;
		_interruptStateNumMem[i] = 0;
		_stateCheckTime[i] = 0;
		_interruptStateTime[i] = 0;
	}

	_serialOutput = 0;
}

/**
 * Initialization
 */
void SkaarhojEncoders::begin(uint8_t address) {
	MCP23017 GPIOchip;
	_GPIOchip = GPIOchip;

	_GPIOchip.begin(address & B111);
	
    _GPIOchip.init();
    _GPIOchip.internalPullupMask(0xFFFF);  // Pull-ups on all pins
    _GPIOchip.inputOutputMask(0xFFFF);  // All Inputs.

    _GPIOchip.setGPInterruptTriggerType(0);		// (INTCON) Trigger interrupts on change from previous on all.
    _GPIOchip.setGPInterruptEnable((B01010101 <<8) | B0011111);          // Set up which pins triggers interrupt (GPINTEN)
}

/**
 * Checking if an interrupt has occured:
 */
void SkaarhojEncoders::runLoop()	{
    bool directionUp;

    word buttonStatus = _GPIOchip.getGPInterruptTriggerPin();
	if (buttonStatus)	{	// Some interrupt was triggered at all...	
        word capture = _GPIOchip.getGPInterruptCapture();	// Capture states of GPI at the time of interrupt
 /*  	    Serial.println(buttonStatus | 0x8000,BIN);
   	    Serial.println(capture | 0x8000,BIN);
		Serial.println("---");
*/
			// First 4 encoders (GPA0-7), turning:
	    for(uint8_t b=0; b<4; b++)  {
	      if ((buttonStatus >> 8) & (B1<<(b<<1)))  {	// Was this pin causing the interrupt?
	        if ((capture >> 8) & (B1<<(b<<1)))  {	// Check pin A polarity
	          if ((capture >> 8) & (B10<<(b<<1)))  {	// Check pin B (direction)
	            _interruptStateNum[b]++;
				directionUp = true;
	          } 
	          else {
	            _interruptStateNum[b]--;
				directionUp = false;
	          }

			  if (_serialOutput)	{
				  Serial.print(F("Encoder "));
				  Serial.print(b);
				  Serial.print(F(": "));
				  Serial.print(directionUp ? F("UP,   ") : F("DOWN, "));
				  Serial.print(_interruptStateNum[b]);
				  Serial.println();
			  }
			  _interruptStateTime[b] = millis();
	        }
	      }
	    }
		/*
		// Fifth encoder:
      if (buttonStatus & (B100000))  {	// Was this pin causing the interrupt?
        if (capture & (B100000))  {	// Check pin A polarity
          if (capture & (B1000000))  {	// Check pin B (direction)
            _interruptStateNum[4]++;
			directionUp = true;
          } 
          else {
            _interruptStateNum[4]--;
			directionUp = false;
          }

		  if (_serialOutput)	{
			  Serial.print(F("Encoder "));
			  Serial.print(4);
			  Serial.print(F(": "));
			  Serial.print(directionUp ? F("UP,   ") : F("DOWN, "));
			  Serial.print(_interruptStateNum[4]);
			  Serial.println();
		  }

		  _interruptStateTime[4] = millis();
        }
      }
	  */
			// 5 buttons:
	    for(uint8_t b=0; b<5; b++)  {
	      if (buttonStatus & (B1<<b))  {	// Was this pin causing the interrupt?
			if (!(capture & (B1<<b)))  {	// Check polarity
			  _pushOnMillis[b] = millis();

			  if (_serialOutput)	{
				  Serial.print(F("Button "));
				  Serial.print(b);
				  Serial.print(F(" pushed"));
				  Serial.println();
			  }
		    }
	      }
	    }
	}
}

/**
 *
 */
int SkaarhojEncoders::state(uint8_t encNum) {
	return state(encNum, 0);
}

/**
 *
 */
int SkaarhojEncoders::state(uint8_t encNum, unsigned int buttonPushTriggerDelay) {
		// Check:
	if (encNum <5 && hasTimedOut(_stateCheckTime[encNum],100))	{
		_stateCheckTime[encNum] = millis();
		
			// If rotations has been detected in either direction:
		if (_interruptStateNumMem[encNum] != _interruptStateNum[encNum])	{
			_interruptStateNumMem[encNum] = _interruptStateNum[encNum];
			
				// Check if the last interrupt generated signal is younger than 1000 ms, only then will we accept it, otherwise we ignore.
			if (!hasTimedOut(_interruptStateTime[encNum],1000))	{	
				_interruptStateLastCount[encNum] = _interruptStateNum[encNum]-_interruptStateNumMem[encNum];
			} else {
				_interruptStateLastCount[encNum] = 0;
			}
			
			return _interruptStateLastCount[encNum]!=0 ? (_interruptStateLastCount[encNum] < 0 ? -1 : 1) : 0;
		}
	
		bool isPushed = _GPIOchip.digitalRead(3+encNum);
		if (isPushed)  {
			if (!_pushOn[encNum]) {
				_pushOn[encNum] = true;
				_pushOnTriggerTimeFired[encNum] = false;
				return 2;
			}
			if (!_pushOnTriggerTimeFired[encNum] && buttonPushTriggerDelay>0 && hasTimedOut(_pushOnMillis[encNum],buttonPushTriggerDelay))	{
				_pushOnTriggerTimeFired[encNum] = true;
				return (unsigned long) millis()-_pushOnMillis[encNum];
			}
		} else {
			if (_pushOn[encNum])	{
				_pushOn[encNum] = false;
				if (!_pushOnTriggerTimeFired[encNum])	{
					return (unsigned long) millis()-_pushOnMillis[encNum];
				}
			}
		}
	}
		// Default return
	return 0;
}

/**
 *
 */
int SkaarhojEncoders::lastCount(uint8_t encNum) {
	return _interruptStateLastCount[encNum];
}

/**
 * Timeout check
 */
bool SkaarhojEncoders::hasTimedOut(unsigned long time, unsigned long timeout)  {
  if ((unsigned long)(time + timeout) <= (unsigned long)millis())  {  // This should "wrap around" if time+timout is larger than the size of unsigned-longs, right?
    return true; 
  } 
  else {
    return false;
  }
}

/**
 * Setter method: If _serialOutput is set, the library may use Serial.print() to give away information about its operation - mostly for debugging.
 * 0= no output
 * 1= normal output (info)
 * 2= verbose (development)
 * 3= verbose! (development, bugfixing)
 */
void SkaarhojEncoders::serialOutput(uint8_t level) {
	_serialOutput = level;
}