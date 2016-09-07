/*
Copyright 2014 Kasper Skårhøj, SKAARHOJ, kasper@skaarhoj.com

This file is part of the Skaarhoj Serial Client Superclass library for Arduino

The SkaarhojSerialClient library is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published by the 
Free Software Foundation, either version 3 of the License, or (at your 
option) any later version.

The SkaarhojSerialClient library is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
or FITNESS FOR A PARTICULAR PURPOSE. 
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along 
with the SkaarhojSerialClient library. If not, see http://www.gnu.org/licenses/.


IMPORTANT: If you want to use this library in your own projects and/or products,
please play a fair game and heed the license rules! See our web page for a Q&A so
you can keep a clear conscience: http://skaarhoj.com/about/licenses/

*/



#include "Arduino.h"
#include "SkaarhojSerialClient.h"

/**
 * Constructor
 */
SkaarhojSerialClient::SkaarhojSerialClient(HardwareSerial &serial) : _HardSerial(serial) {
	_EOLChar = 10;	// LF is default
}

/**
 * Setting up Serial Object:
 */
void SkaarhojSerialClient::begin(long baudRate) {
	begin(baudRate, SERIAL_8N1);
}
void SkaarhojSerialClient::begin(long baudRate, uint8_t cfg) {

#ifdef __arm__	/* Arduino DUE */
	_HardSerial.begin(baudRate);	// SERIAL_8N1, default! Couldn't figure how to set right type for Ard. Due... (6/7 2015)
#else
	_HardSerial.begin(baudRate, cfg);	// SERIAL_8N1 is default, see
#endif

	_resetDeviceStateVariables();
}

/**
 * Initiating connection handshake to the server
 */
void SkaarhojSerialClient::connect() {

	// Flush buffer before trying to connect
  _HardSerial.flush();
	
  if (_serialOutput) Serial.println(F("Connecting... (sending ping)"));

  _isConnected = false;
  _hasInitialized = false;
  _pendingAnswer = false;
  _pendingEOT = false;
  _resetDeviceStateVariables();
  
  _sendPing();	// The way to connect. The response of a ping will set _isConnected and trigger asking for a status message. When processing the status message, _hasInitialized should be set in subclass

  _lastReconnectAttempt = millis();
}

/**
 * Reads information from the device as it arrives, dispatches packages to parsing
 */
void SkaarhojSerialClient::runLoop() {
	runLoop(0);
}
void SkaarhojSerialClient::runLoop(uint16_t delayTime) {

	unsigned long enterTime = millis();

	do {
		_runSubLoop();
		
		if (_HardSerial.available())	{
			_readFromClient();
		}

			// Requesting status of the device:
		if (_statusRequestInterval>0 && !_pendingAnswer && hasTimedOut(_lastStatusRequest,_statusRequestInterval))	{
			if (_serialOutput>1)	Serial.println(F("Pulling status."));
			_lastStatusRequest = millis();

			_sendStatus();
		}
		
			// Send a ping / reconnect:
		if (_ackMsgInterval>0 && !_pendingAnswer && hasTimedOut(_lastIncomingMsg,_ackMsgInterval))	{
			if (_serialOutput>1)	Serial.println(F("Sending Ping."));
			_lastReconnectAttempt = millis();
			_pendingAnswer = true;

			_sendPing();
		}
		
    		// Trying to connect, if not:
		if (!isConnected()) {
			if (_reconnectInterval>0 && hasTimedOut(_lastReconnectAttempt,_reconnectInterval))	{
				connect();
			}
		} else {
			if (_ackMsgInterval>0 && hasTimedOut(_lastIncomingMsg,_ackMsgInterval+2000))	{
				if (_serialOutput)	Serial.println(F("Disconnected."));
				_isConnected = false;
			}
		}
	} while (delayTime>0 && !hasTimedOut(enterTime,delayTime));
}
void SkaarhojSerialClient::_runSubLoop() {	// For overloading
}

/**
 * Read from client buffer
 */
void SkaarhojSerialClient::_readFromClient()	{
	while (_HardSerial.available()) {
		char c = _HardSerial.read();

		if (c==_EOLChar)	{	// Line feed, always used
			_parselineDispatch();
			_resetLastIncomingMsg();
			_resetBuffer();
		} else if (c==_EOTChar)	{	// Prompt 
			if (_serialOutput>1)	Serial.println(F("EOT received..."));
			_initialize();
			_resetLastIncomingMsg();
			_resetBuffer();
			_pendingEOT = false;
		} else if (c==13 || c==10)	{	// <CR> and <LF> ignored (they should be captured as _EOLChar if necessary)
			// Ignore.
		} else if (_bufferWriteIndex < _bufferSize-1)	{	// one byte for null termination reserved
			_buffer[_bufferWriteIndex] = c;
			_bufferWriteIndex++;
		} else {
			if (_serialOutput)	Serial.println(F("ERROR: Buffer overflow."));
		}
	}
}

/**
 * Initialize by asking for status
 */
void SkaarhojSerialClient::_initialize()	{
	if (!_isConnected)	{
		_isConnected = true;
		if (_serialOutput) Serial.println(F("Connected"));

		_sendStatus();
	}
}

/**
 * Sends the buffer
 */
void SkaarhojSerialClient::_sendBuffer() {
	_HardSerial.print(_buffer);
	if (_serialOutput>2) {
		Serial.print(F("_sendBuffer: "));
		Serial.println(_buffer);
	}
	_resetBuffer();
	_pendingAnswer = true;
	_pendingEOT = true;
}
