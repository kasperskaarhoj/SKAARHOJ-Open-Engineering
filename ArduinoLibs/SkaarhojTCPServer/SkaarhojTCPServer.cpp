/*
Copyright 2014 Kasper Skårhøj, SKAARHOJ, kasper@skaarhoj.com

This file is part of the Skaarhoj TCP Server Superclass library for Arduino

The SkaarhojTCPServer library is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published by the 
Free Software Foundation, either version 3 of the License, or (at your 
option) any later version.

The SkaarhojTCPServer library is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
or FITNESS FOR A PARTICULAR PURPOSE. 
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along 
with the SkaarhojTCPServer library. If not, see http://www.gnu.org/licenses/.


IMPORTANT: If you want to use this library in your own projects and/or products,
please play a fair game and heed the license rules! See our web page for a Q&A so
you can keep a clear conscience: http://skaarhoj.com/about/licenses/

*/



#include "Arduino.h"
#include "SkaarhojTCPServer.h"
#include "SkaarhojBufferTools.h"


/**
 * Constructor
 */
SkaarhojTCPServer::SkaarhojTCPServer(uint16_t localPort) : _localPort(localPort), _server(localPort) {}


/**
 * Setting up IP address
 */
void SkaarhojTCPServer::begin() {

	_server.begin();

	_EOLChar = 10;	// LF is default
}

/**
 * Reads information from the server as it arrives, dispatches lines to parsing
 */
void SkaarhojTCPServer::runLoop() {
	runLoop(0);
}
void SkaarhojTCPServer::runLoop(uint16_t delayTime) {

	unsigned long enterTime = millis();

	do {
		_readFromClient();
		
		// This is where you might install a periodic status message, but at this point I don't know why since there seems to be no way to know if we have a client connected other than if it asks us for stuff.
	} while (delayTime>0 && !hasTimedOut(enterTime,delayTime));
}

/**
 * Read from client buffer
 */
void SkaarhojTCPServer::_readFromClient()	{
	_currentClient = _server.available();
	
	if (_currentClient)	{	// This only returns true when a client is SENDING something to us.
		while (_currentClient.available()) {
			char c = _currentClient.read();

			if (c==_EOLChar)	{	// Line feed, always used
				_parselineDispatch();
				_resetBuffer();
			} else if (c==13 || c==10)	{	// Carriage return, ignored
				// Ignore.
			} else if (_bufferWriteIndex < _bufferSize-1)	{	// one byte for null termination reserved
				_buffer[_bufferWriteIndex] = c;
				_bufferWriteIndex++;
			} else {
				if (_serialOutput)	Serial.println(F("ERROR: Buffer overflow."));
				_currentClient.flush();
				_resetBuffer();
			}
		}
	}
}

/**
 * Sends the buffer to all connected clients
 */
void SkaarhojTCPServer::_sendBuffer() {
	_server.print(_buffer);
	if (_serialOutput>2) {
		Serial.print(F("sendBuffer: "));
		Serial.print(_buffer);
	}
	_resetBuffer();
}