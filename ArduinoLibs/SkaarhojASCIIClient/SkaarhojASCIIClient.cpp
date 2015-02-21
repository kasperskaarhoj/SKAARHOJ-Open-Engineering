/*
Copyright 2014 Kasper Skårhøj, SKAARHOJ, kasper@skaarhoj.com

This file is part of the Skaarhoj ASCII Client Superclass library for Arduino

The SkaarhojASCIIClient library is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published by the 
Free Software Foundation, either version 3 of the License, or (at your 
option) any later version.

The SkaarhojASCIIClient library is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
or FITNESS FOR A PARTICULAR PURPOSE. 
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along 
with the SkaarhojASCIIClient library. If not, see http://www.gnu.org/licenses/.


IMPORTANT: If you want to use this library in your own projects and/or products,
please play a fair game and heed the license rules! See our web page for a Q&A so
you can keep a clear conscience: http://skaarhoj.com/about/licenses/

*/



#include "Arduino.h"
#include "SkaarhojASCIIClient.h"


/**
 * Constructor (using arguments is deprecated! Use begin() instead)
 */
SkaarhojASCIIClient::SkaarhojASCIIClient() {

	_handlerPing = 0;	
	_handlerStatus = 0;
	
	_reconnectInterval = 3000;
	_ackMsgInterval = 5000;
	_statusRequestInterval = 10000;
	
	_lastReconnectAttempt = millis();
	_lastIncomingMsg = millis();
	_lastStatusRequest = millis();
	
	_isConnected = false;
	_hasInitialized = false;
	_pendingAnswer = false;
	_pendingEOT = false;
	
	_section = 0;
}

/**
 * Returns true if the server connection has been initialized; typically sent initial data payload (this depends on implementation in subsclass)
 */
bool SkaarhojASCIIClient::hasInitialized()	{
	return _hasInitialized;
}

/**
 * Returns true if the client (arduino) is connected to the server
 */
bool SkaarhojASCIIClient::isConnected()	{
	return _isConnected;
}

/**
 * Sets the initialized flag true
 */
void SkaarhojASCIIClient::setInitialized()	{
	_hasInitialized = true;
}

/**
 * Set callback function and timeout for sending of pings
 */
void SkaarhojASCIIClient::setHandlePing(void (*fptr)(void), uint16_t pingTimeout)	{
	_handlerPing = fptr;
	_ackMsgInterval = pingTimeout;
}

/**
 * Set callback function and timeout for sending of status messages
 */
void SkaarhojASCIIClient::setHandleStatus(void (*fptr)(void), uint16_t statusTimeout)	{
	_handlerStatus = fptr;
	_statusRequestInterval = statusTimeout;
}

/**
 * Disable pings completely
 */
void SkaarhojASCIIClient::disablePing()	{
	_handlerPing = 0;
	_ackMsgInterval = 0;
}

/**
 * Disable status messages completely
 */
void SkaarhojASCIIClient::disableStatus()	{
	_handlerStatus = 0;
	_statusRequestInterval = 0;
}

/**
 * Resets internal variables. Called after reception of data to confirm, that we are ready to send a new command and there was activity on the connection.
 */
void SkaarhojASCIIClient::_resetLastIncomingMsg()	{
  _lastIncomingMsg = millis();
  _pendingAnswer = false;
}

/**
 * Resets local device state variables. Overloaded in subclass.
 */
void SkaarhojASCIIClient::_resetDeviceStateVariables()	{
}

/**
 * Sends status command. Overloaded in subclass.
 */
void SkaarhojASCIIClient::_sendStatus()	{
	if (_handlerStatus != 0)	_handlerStatus(); 
}

/**
 * Sends ping command. Overloaded in subclass.
 */
void SkaarhojASCIIClient::_sendPing()	{
	if (_handlerPing != 0)	_handlerPing(); 
}








