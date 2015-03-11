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


/**
  Version 1.0.0
	(Semantic Versioning)
**/


/**
  Connection with DB-9 to SKAARHOJ RS-422 port: Use straight cable
**/


#ifndef ClientBMDVideohubSerial_h
#define ClientBMDVideohubSerial_h

#include "Arduino.h"
#include "SkaarhojSerialClient.h"

//  #include "SkaarhojPgmspace.h"  - 23/2 2014





#define ClientBMDVideohubSerial_NUMOUTPUTS 16

class ClientBMDVideohubSerial : public SkaarhojSerialClient 
{
  private:
  	uint8_t _outputRouting[ClientBMDVideohubSerial_NUMOUTPUTS];
  	uint8_t _numOutputs;
	
  public:
    ClientBMDVideohubSerial(HardwareSerial &serial);
    void begin();

  private:	// Overloading:
    void _resetDeviceStateVariables();
  	void _parseline();
  	void _sendStatus();
  	void _sendPing();

  public:
	  // "input" and "output" are the "natural" numbers starting with "1" (same as label on BNC plugs)
	  // Internally - also in the protocol - these "natural numbers" actually start at 0. So on a 16 input router (1-16), internally the inputs are referred to with 0-15.
	void routeInputToOutput(uint8_t input, uint8_t output);
	uint8_t getRoute(uint8_t output);
};

#endif