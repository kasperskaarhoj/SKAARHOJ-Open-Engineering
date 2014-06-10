/*
Copyright 2014 Kasper Skårhøj, SKAARHOJ, kasper@skaarhoj.com

This file is part of the Skaarhoj Serial Server Superclass library for Arduino

The SkaarhojSerialServer library is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published by the 
Free Software Foundation, either version 3 of the License, or (at your 
option) any later version.

The SkaarhojSerialServer library is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
or FITNESS FOR A PARTICULAR PURPOSE. 
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along 
with the SkaarhojSerialServer library. If not, see http://www.gnu.org/licenses/.


IMPORTANT: If you want to use this library in your own projects and/or products,
please play a fair game and heed the license rules! See our web page for a Q&A so
you can keep a clear conscience: http://skaarhoj.com/about/licenses/

*/



#include "Arduino.h"
#include "SkaarhojSerialServer.h"
#include "SkaarhojBufferTools.h"


/**
 * Constructor
 */
SkaarhojSerialServer::SkaarhojSerialServer(HardwareSerial &serial) : _HardSerial(serial) {}


/**
 * Setting up Serial Object:
 */
void SkaarhojSerialServer::begin(long baudRate) {
	begin(baudRate, SERIAL_8N1);
}
void SkaarhojSerialServer::begin(long baudRate, uint8_t cfg) {

	_HardSerial.begin(baudRate, cfg);	// SERIAL_8N1 is default, see

	_EOLChar = 13;	// CR is default

	_lastCharTime = 0;
	_EOLtimeOut = 0;	// Set to a value if you want a message to be recognized based on a timeout in ms instead (useful for arduino serial monitor)
}

/**
 * Reads information from the server as it arrives, dispatches lines to parsing
 */
void SkaarhojSerialServer::runLoop() {
	runLoop(0);
}
void SkaarhojSerialServer::runLoop(uint16_t delayTime) {

	unsigned long enterTime = millis();

	do {
		if (_HardSerial.available())	{
			_readFromClient();
		}
		
			// Parse line based on timeout:
		if (_bufferWriteIndex > 0 && _EOLtimeOut>0 && hasTimedOut(_lastCharTime,_EOLtimeOut))	{
			_parselineDispatch();
			_resetBuffer();
		}
		
		// This is where you might install a periodic status message, but at this point I don't know why since there seems to be no way to know if we have a client connected other than if it asks us for stuff.
	} while (delayTime>0 && !hasTimedOut(enterTime,delayTime));
}

/**
 * Enable EOL timeout:
 */
void SkaarhojSerialServer::enableEOLTimeout()	{
	enableEOLTimeout(2);	// 2 is a value that should work with 9600 baud and up
}
void SkaarhojSerialServer::enableEOLTimeout(uint16_t timeout) {
	_EOLtimeOut = timeout;
}


/**
 * Read from client buffer
 */
void SkaarhojSerialServer::_readFromClient()	{
	while (_HardSerial.available()) {
		char c = _HardSerial.read();
		_lastCharTime = millis();
		
		if (c==_EOLChar)	{
			_parselineDispatch();
			_resetBuffer();
		} else if (c==13 || c==10)	{	// <CR> and <LF> ignored (they should be captured as _EOLChar if necessary)
			// Ignore.
		} else if (_bufferWriteIndex < _bufferSize-1)	{	// one byte for null termination reserved
			_buffer[_bufferWriteIndex] = c;
			_bufferWriteIndex++;
		} else {
			if (_serialOutput)	Serial.println(F("ERROR: Buffer overflow."));
			_HardSerial.flush();
			_resetBuffer();
		}
	}
}

/**
 * Sends the buffer to all connected clients
 */
void SkaarhojSerialServer::_sendBuffer() {
	_HardSerial.print(_buffer);
	if (_serialOutput>2) {
		Serial.print(F("sendBuffer: "));
		Serial.print(_buffer);
	}
	_resetBuffer();
}