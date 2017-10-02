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



#include "Arduino.h"
#include "SkaarhojBufferTools.h"


/**
 * Constructor
 */
SkaarhojBufferTools::SkaarhojBufferTools() : _bufferSize( SkaarhojBufferTools_BUFFERSIZE ) {
	_serialOutput = false;
	
	_resetBuffer();

	_EOLChar = 0;
	_EOTChar = 0;
	_handlerIncomingLine = 0;
}

/**
 * Setter method: If _serialOutput is set, the library may use Serial.print() to give away information about its operation - mostly for debugging.
 * 0= no output
 * 1= normal output (info)
 * 2= verbose (development)
 * 3= verbose! (development, bugfixing)
 */
void SkaarhojBufferTools::serialOutput(uint8_t level) {
	_serialOutput = level;
}

/**
 * Set EOL char:
 */
void SkaarhojBufferTools::setEOLChar(char EOLChar) {
	_EOLChar = EOLChar;
}

/**
 * Set EOT char:
 */
void SkaarhojBufferTools::setEOTChar(char EOTChar) {
	_EOTChar = EOTChar;
}

/**
 * Timeout check
 */
bool SkaarhojBufferTools::hasTimedOut(unsigned long time, unsigned long timeout)  {
  if ((unsigned long)(time + timeout) <= (unsigned long)millis())  {  // This should "wrap around" if time+timout is larger than the size of unsigned-longs, right?
    return true; 
  } 
  else {
    return false;
  }
}

/**
 * Setting call-back function:
 */
//void SkaarhojBufferTools::setHandleIncomingLine(void (*fptr)(byte test, byte test2))	{
void SkaarhojBufferTools::setHandleIncomingLine(void (*fptr)(void))	{
	_handlerIncomingLine = fptr;
}

/**
 * Parses the next part of string as an integer. Does this until there are non-numbers present. Advances the _bufferReadIndex.
 */
int SkaarhojBufferTools::parseInt()	{
	int output = 0;
	bool first = true;
	bool neg = false;
	while(_bufferReadIndex < _bufferWriteIndex)	{
		if (first && _buffer[_bufferReadIndex]==45)	{	// Minus sign...
			neg = true;
		} 
		else if (_buffer[_bufferReadIndex]>=48 && _buffer[_bufferReadIndex]<=57)	{
			output = output*10 +_buffer[_bufferReadIndex]-48;
		} else break;

		_bufferReadIndex++;
		first = false;
	}
	return neg ? -output : output;
}

/**
 * Parses the next part of string as an hex integer. Does this until there are non-numbers present. Advances the _bufferReadIndex.
 */
int SkaarhojBufferTools::parseHex()	{
	int output = 0;
	bool first = true;
	bool neg = false;
	while(_bufferReadIndex < _bufferWriteIndex)	{
		if (first && _buffer[_bufferReadIndex]==45)	{	// Minus sign...
			neg = true;
		} 
		else if (_buffer[_bufferReadIndex]>=48 && _buffer[_bufferReadIndex]<=(48+9))	{
			output = (output<<4) + (_buffer[_bufferReadIndex]-48);
		}
		else if (_buffer[_bufferReadIndex]>=65 && _buffer[_bufferReadIndex]<=(65+5))	{
			output = (output<<4) + (_buffer[_bufferReadIndex]-65+10);
		}
		else if (_buffer[_bufferReadIndex]>=97 && _buffer[_bufferReadIndex]<=(97+5))	{
			output = (output<<4) + (_buffer[_bufferReadIndex]-97+10);
		} else break;

		_bufferReadIndex++;
		first = false;
	}
	return neg ? -output : output;
}

/**
 * Checks if next part of buffer equals input string (PROGMEM). Advances the _bufferReadIndex if true
 */
bool SkaarhojBufferTools::isNextPartOfBuffer_P(const char *firstPartStr, bool forceAdvance)	{
	if (strncmp_P(_buffer+_bufferReadIndex, firstPartStr, strlen_P(firstPartStr)))	{
		if (forceAdvance)	_bufferReadIndex+= strlen_P(firstPartStr);
		return false;
	}
	_bufferReadIndex+= strlen_P(firstPartStr);
	return true;
}

/**
 * Checks if remainder of buffer equals input string (PROGMEM). Advances the _bufferReadIndex if true
 */
bool SkaarhojBufferTools::isBufferEqualTo_P(const char *firstPartStr)	{
	if (strcmp_P(_buffer+_bufferReadIndex, firstPartStr))	{return false;}
	_bufferReadIndex+= strlen_P(firstPartStr);
	return true;
}

/**
 * Returns remainder of buffer
 */
char* SkaarhojBufferTools::getRemainingBuffer() {
	return _buffer+_bufferReadIndex;
}

/**
 * Advances the internal pointer
 */
bool SkaarhojBufferTools::advancePointer(uint8_t count) {
	if(_bufferReadIndex + count < _bufferWriteIndex) {
		_bufferReadIndex += count;
		return true;
	}
	return false;
}

/**
 * Parses a line from client
 */
void SkaarhojBufferTools::_parselineDispatch()	{
	
 	if (_serialOutput>2) {
		Serial.print(F("INCOMING LINE: "));
		Serial.println(_buffer);
	}

	_bufferReadIndex = 0;
	
	if (_handlerIncomingLine != 0)	_handlerIncomingLine(); 
	_parseline();
}

/**
 * Overloading this in subclass:
 */
void SkaarhojBufferTools::_parseline()	{}

/**
 * Adds the input String object to buffer (if it has space for it)
 */
void SkaarhojBufferTools::_addToBuffer(const String command) {
	if (_bufferWriteIndex+command.length() < _bufferSize)	{
		command.toCharArray(_buffer+_bufferWriteIndex, command.length()+1);
		_bufferWriteIndex+= command.length();
	} else Serial.println(F("addToBuffer ERROR"));
}

/**
 * Adds the input PROGMEM string reference to buffer (if it has space for it)
 */
void SkaarhojBufferTools::_addToBuffer_P(const char *str) {	// PROGMEM pointer
	if (_bufferWriteIndex+strlen_P(str) < _bufferSize)	{
		strcpy_P(_buffer+_bufferWriteIndex, str);
		_bufferWriteIndex+= strlen_P(str);
	} else Serial.println(F("addToBuffer_P ERROR"));
}

/**
 * Resets the buffer, sets it to zeros all through.
 */
void SkaarhojBufferTools::_resetBuffer()	{
	memset(_buffer, 0, _bufferSize);
	_bufferWriteIndex = 0;
}
