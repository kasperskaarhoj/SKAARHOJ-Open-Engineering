/*
Copyright 2012-2014 Kasper Skårhøj, SKAARHOJ, kasper@skaarhoj.com

This file is part of the Blackmagic Design Videohub Serial Client library for Arduino

The ClientBMDVideohubSerial library is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published by the 
Free Software Foundation, either version 3 of the License, or (at your 
option) any later version.

The ClientBMDVideohubSerial library is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
or FITNESS FOR A PARTICULAR PURPOSE. 
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along 
with the ClientBMDVideohubSerial library. If not, see http://www.gnu.org/licenses/.


IMPORTANT: If you want to use this library in your own projects and/or products,
please play a fair game and heed the license rules! See our web page for a Q&A so
you can keep a clear conscience: http://skaarhoj.com/about/licenses/


*/


#include "Arduino.h"
#include "ClientBMDVideohubSerial.h"


/**
 * Constructor
 */
ClientBMDVideohubSerial::ClientBMDVideohubSerial(HardwareSerial &serial) : SkaarhojSerialClient(serial) {}

/**
 * Overloading method: Setting up baud rate and other settings
 */
void ClientBMDVideohubSerial::begin(){
	
	SkaarhojSerialClient::begin(9600);	// Must call!
	
	_EOTChar = '>';
}

/**
 * Resets local device state variables. (Overloading from superclass)
 */
void ClientBMDVideohubSerial::_resetDeviceStateVariables()	{
	_numOutputs = 0;
	
	for(uint8_t outputNum=0; outputNum<ClientBMDVideohubSerial_NUMOUTPUTS; outputNum++)	{
	  	_outputRouting[outputNum] = 255;
	}
}

/**
 * Parses a line from client. (Overloading from superclass)
 */
void ClientBMDVideohubSerial::_parseline()	{
	_bufferReadIndex=0;
		
	if (isNextPartOfBuffer_P(PSTR("@ S?0")))	{
		_hasInitialized = true;
	} else if (isNextPartOfBuffer_P(PSTR("S:")))	{
		uint8_t outputIndex = parseHex();
		_bufferReadIndex++;
		uint8_t inputIndex = parseHex();
		
		if (outputIndex < ClientBMDVideohubSerial_NUMOUTPUTS)	{
			_outputRouting[outputIndex] = inputIndex;
			if (_serialOutput>1) Serial.print(F("_outputRouting["));
			if (_serialOutput>1) Serial.print(outputIndex);
			if (_serialOutput>1) Serial.print(F("]="));
			if (_serialOutput>1) Serial.println(_outputRouting[outputIndex]);
		}
	}
}

/**
 * Sends status command. (Overloading from superclass)
 */
void ClientBMDVideohubSerial::_sendStatus()	{
	_resetBuffer();
	_addToBuffer_P(PSTR("@ S?0\r"));
	_sendBuffer();
}

/**
 * Sends ping command. (Overloading from superclass)
 */
void ClientBMDVideohubSerial::_sendPing()	{
	_resetBuffer();
	_addToBuffer_P(PSTR("\r"));
	_sendBuffer();
}















void ClientBMDVideohubSerial::routeInputToOutput(uint8_t input, uint8_t output)	{
	_resetBuffer();
	_addToBuffer_P(PSTR("@ X:0/"));
	_addToBuffer(String(output-1, HEX));
	_addToBuffer_P(PSTR(","));
	_addToBuffer(String(input-1, HEX));
	_addToBuffer_P(PSTR("\r"));
	_sendBuffer();
}
uint8_t ClientBMDVideohubSerial::getRoute(uint8_t output)	{
	// NOTICE: This value depends on the periodic updates from the video hub because
	// last time I tried to "subscribe" to updates via the command "@ ?", the video hub (Smart Videohub) crashed after routing a few times on the front panel.
	return _outputRouting[output-1]+1;
}