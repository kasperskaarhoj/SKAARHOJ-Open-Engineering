/*
Copyright 2014 Kasper Skårhøj, LearningLab DTU, kaska@llab.dtu.dk

This file is part of the Kramer VS-*HN Serial Client library for Arduino

The ClientKramerVSHDMIMatrix library is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published by the 
Free Software Foundation, either version 3 of the License, or (at your 
option) any later version.

The ClientKramerVSHDMIMatrix library is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
or FITNESS FOR A PARTICULAR PURPOSE. 
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along 
with the ClientKramerVSHDMIMatrix library. If not, see http://www.gnu.org/licenses/.


IMPORTANT: If you want to use this library in your own projects and/or products,
please play a fair game and heed the license rules! See our web page for a Q&A so
you can keep a clear conscience: http://skaarhoj.com/about/licenses/


*/


#include "Arduino.h"
#include "ClientKramerVSHDMIMatrix.h"


/**
 * Constructor
 */
ClientKramerVSHDMIMatrix::ClientKramerVSHDMIMatrix(HardwareSerial &serial) : SkaarhojSerialClient(serial) {}

/**
 * Overloading method: Setting up baud rate and other settings
 */
void ClientKramerVSHDMIMatrix::begin(){
	
	SkaarhojSerialClient::begin(9600);	// Must call!
	
	_statusRequestInterval = 60000;
}

/**
 * Resets local device state variables. (Overloading from superclass)
 */
void ClientKramerVSHDMIMatrix::_resetDeviceStateVariables()	{
	for(uint8_t outputNum=0; outputNum<ClientKramerVSHDMIMatrix_NUMOUTPUTS; outputNum++)	{
	  	_outputRouting[outputNum] = 255;
	}
	for(uint8_t inputNum=0; inputNum<ClientKramerVSHDMIMatrix_NUMINPUTS; inputNum++)	{
	  	_signalState[inputNum] = false;
	}
}

/**
 * Parses a line from client. (Overloading from superclass)
 */
void ClientKramerVSHDMIMatrix::_parseline()	{
	_bufferReadIndex=0;
	bool value;

	if (isNextPartOfBuffer_P(PSTR("~01@ OK")))	{
		_initialize();
	} else if (isNextPartOfBuffer_P(PSTR("~01@SIGNAL ")))	{
		uint8_t inputIndex = parseInt();
		_bufferReadIndex++;
		isNextPartOfBuffer_P(PSTR(" "));	// This will automatically increment _bufferReadIndex is there is a space - exactly what we need... :-)
		value = parseInt();
		
		if (inputIndex>0 && inputIndex <= ClientKramerVSHDMIMatrix_NUMINPUTS)	{
			_signalState[inputIndex-1] = value;
			if (_serialOutput>1) Serial.print(F("_signalState["));
			if (_serialOutput>1) Serial.print(inputIndex);
			if (_serialOutput>1) Serial.print(F("]="));
			if (_serialOutput>1) Serial.println(_signalState[inputIndex-1]);
		}
	} else if (isNextPartOfBuffer_P(PSTR("~01@VID ")))	{
		_hasInitialized = true;
		
		uint8_t inputIndex = parseInt();
		_bufferReadIndex++;
		uint8_t outputIndex = parseInt();
		
		if (outputIndex>0 && outputIndex <= ClientKramerVSHDMIMatrix_NUMOUTPUTS)	{
			_outputRouting[outputIndex-1] = inputIndex;
			if (_serialOutput>1) Serial.print(F("_outputRouting["));
			if (_serialOutput>1) Serial.print(outputIndex);
			if (_serialOutput>1) Serial.print(F("]="));
			if (_serialOutput>1) Serial.println(_outputRouting[outputIndex-1]);
		}
	}
}

/**
 * Sends status command. (Overloading from superclass)
 */
void ClientKramerVSHDMIMatrix::_sendStatus()	{
	_resetBuffer();
	_addToBuffer_P(PSTR("#VID? 1|VID? 2|VID? 3|VID? 4\r"));
	_sendBuffer();

	_resetBuffer();
	_addToBuffer_P(PSTR("#SIGNAL? 1|SIGNAL? 2|SIGNAL? 3|SIGNAL? 4\r"));
	_sendBuffer();
}

/**
 * Sends ping command. (Overloading from superclass)
 */
void ClientKramerVSHDMIMatrix::_sendPing()	{
	_resetBuffer();
	_addToBuffer_P(PSTR("#\r"));
	_sendBuffer();
}















void ClientKramerVSHDMIMatrix::routeInputToOutput(uint8_t input, uint8_t output)	{
	_resetBuffer();
	_addToBuffer_P(PSTR("#VID "));
	_addToBuffer(String(input));
	_addToBuffer_P(PSTR(">"));
	_addToBuffer(String(output));
	_addToBuffer_P(PSTR("\r"));
	_sendBuffer();
}
uint8_t ClientKramerVSHDMIMatrix::getRoute(uint8_t output)	{
	return _outputRouting[output-1];
}
bool ClientKramerVSHDMIMatrix::getSignal(uint8_t input)	{
	return _signalState[input-1];
}