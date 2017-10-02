/*
Copyright 2014 Kasper Skårhøj, SKAARHOJ, kasper@skaarhoj.com

This file is part of the Skaarhoj Buffer Tools Superclass library for Arduino

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


#ifndef SkaarhojBufferTools_h
#define SkaarhojBufferTools_h

#include "Arduino.h"
#include <SkaarhojPgmspace.h>



#define SkaarhojBufferTools_BUFFERSIZE 128

class SkaarhojBufferTools
{
  protected:
	  uint8_t _serialOutput;		// If set, the library will print status/debug information to the Serial object. There are various levels. (0-3)
	
  	char _buffer[SkaarhojBufferTools_BUFFERSIZE];		// Buffer for incoming lines.
	const uint8_t _bufferSize;
	uint16_t _bufferWriteIndex;
	uint8_t _bufferReadIndex;

	char _EOLChar;	// End of Line character
	char _EOTChar;	// End of Transmission character


//	void (*_handlerIncomingLine)(byte test, byte test2);
	void (*_handlerIncomingLine)(void);


  public:
    SkaarhojBufferTools();

	void serialOutput(uint8_t level); // Sets whether informative (debug) serial output is enabled or not. Level=1: Normal, Level=2: Development
	void setEOLChar(char EOLChar);
	void setEOTChar(char EOTChar);
	bool hasTimedOut(unsigned long time, unsigned long timeout);

//	void setHandleIncomingLine(void (*fptr)(byte test, byte test2));
	void setHandleIncomingLine(void (*fptr)(void));

	virtual int parseInt();
	virtual int parseHex();
	virtual bool isNextPartOfBuffer_P(const char *firstPartStr, bool forceAdvance=false);
	virtual bool isBufferEqualTo_P(const char *firstPartStr);
	char* getRemainingBuffer();
	bool advancePointer(uint8_t count);
		
  protected:
	virtual void _parselineDispatch();
	virtual void _parseline();
	virtual void _addToBuffer(const String command);
	virtual void _addToBuffer_P(const char *str);
	virtual void _resetBuffer();
};

#endif

