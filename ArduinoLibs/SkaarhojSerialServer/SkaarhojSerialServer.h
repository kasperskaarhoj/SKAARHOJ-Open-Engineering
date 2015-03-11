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



#ifndef SkaarhojSerialServer_h
#define SkaarhojSerialServer_h

#include "Arduino.h"
#include "SkaarhojBufferTools.h"




class SkaarhojSerialServer : public SkaarhojBufferTools
{
  protected:
	unsigned long _lastCharTime;	// Last time we received a character on the input
	uint16_t _EOLtimeOut;	// Set to a value if you want a message to be recognized based on timeout

  public:
  	HardwareSerial _HardSerial;

	
  public:
    SkaarhojSerialServer(HardwareSerial &serial);
	void begin(long baudRate);
	void begin(long baudRate, uint8_t cfg);

    void runLoop();		//  Keeps connection alive; Looks for incoming traffic and processes that, if any.
    void runLoop(uint16_t delayTime);	// As runLoop but keeps running for delayTime ms before returning (thus implementing a small delay function while still keeping connection alive. )

	void enableEOLTimeout();
	void enableEOLTimeout(uint16_t timeout);
		
  protected:
	virtual void _readFromClient();
	virtual void _sendBuffer();
};

#endif

