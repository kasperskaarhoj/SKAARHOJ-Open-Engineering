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



#ifndef SkaarhojTCPServer_h
#define SkaarhojTCPServer_h

#include "Arduino.h"
#include "SkaarhojBufferTools.h"

#include <Ethernet.h>
//  #include "SkaarhojPgmspace.h"  - 23/2 2014



class SkaarhojTCPServer : public SkaarhojBufferTools
{
  protected:
	uint16_t _localPort; 		// local port on server
	
  public:
  	EthernetServer _server;		// Object for communication, see constructor.
	EthernetClient _currentClient;
	
	
  public:
    SkaarhojTCPServer(uint16_t localPort);
	void begin();	// The de-facto constructor

    void runLoop();		//  Keeps connection alive; Looks for incoming traffic and processes that, if any.
    void runLoop(uint16_t delayTime);	// As runLoop but keeps running for delayTime ms before returning (thus implementing a small delay function while still keeping connection alive. )

  protected:
	virtual void _readFromClient();
	virtual void _sendBuffer();
};

#endif

