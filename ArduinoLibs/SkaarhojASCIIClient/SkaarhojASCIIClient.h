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

/**
  Version 1.0.0
	(Semantic Versioning)
**/


#ifndef SkaarhojASCIIClient_h
#define SkaarhojASCIIClient_h

#include "Arduino.h"
#include "SkaarhojBufferTools.h"

//  #include "SkaarhojPgmspace.h"  - 23/2 2014



class SkaarhojASCIIClient : public SkaarhojBufferTools
{
  protected:
	unsigned long _lastReconnectAttempt;	// Time in ms when we last tried to connect. (used when unconnected and trying periodically to reconnect)
	uint16_t _reconnectInterval;	// Time in ms until another connection attempt is made in the runloop.

	unsigned long _lastIncomingMsg;	// Time in ms when we last received anything from server
	uint16_t _ackMsgInterval;	// Time in ms between acknowledge request

	unsigned long _lastStatusRequest;	// Time in ms when we last asked for status info from hyperdeck
	uint16_t _statusRequestInterval;	// Time in ms between status requests


	bool _isConnected;		// Set, when the ethernet client seems to have successfully connected.
	bool _hasInitialized; 	// After a connection, typically a payload of information is received and usually this is considered part of "initialization". So after this initial data transfer, most times the class is ready to communication and that is when this flag should be set. This is on the shoulder of implementation to set!
	bool _pendingAnswer;			// If set, a command has been sent (assumed to provoke a response). Reset again by the _parselineDispatch() function (that is; when we get any response in return which confirms basic connection.)
	bool _pendingEOT;	// Set false when we encounter end-of-transmission char
	
	uint16_t _section;
	
	void (*_handlerPing)(void);
	void (*_handlerStatus)(void);
	
	
  public:
    SkaarhojASCIIClient();

	virtual bool hasInitialized();	// Returns true if device has been initialized.
	virtual bool isConnected();		// Returns true if connected to device
	void setInitialized();

	// Call back function setup:
	void setHandlePing(void (*fptr)(void), uint16_t pingTimeout);
	void setHandleStatus(void (*fptr)(void), uint16_t statusTimeout);
	void disablePing();
	void disableStatus();

  protected:
	virtual void _resetLastIncomingMsg();
	virtual void _resetDeviceStateVariables();
	virtual void _sendStatus();
	virtual void _sendPing();
};

#endif

