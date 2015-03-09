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


/**
  Version 1.0.0
	(Semantic Versioning)
**/


/**
  Connection with DB-9 to SKAARHOJ RS-232 port: Use straight cable
**/


#ifndef ClientKramerVSHDMIMatrix_h
#define ClientKramerVSHDMIMatrix_h

#include "Arduino.h"
#include "SkaarhojSerialClient.h"

//  #include "SkaarhojPgmspace.h"  - 23/2 2014




#define ClientKramerVSHDMIMatrix_NUMOUTPUTS 4
#define ClientKramerVSHDMIMatrix_NUMINPUTS 4

class ClientKramerVSHDMIMatrix : public SkaarhojSerialClient 
{
  private:
    uint8_t _outputRouting[ClientKramerVSHDMIMatrix_NUMOUTPUTS];
 	bool _signalState[ClientKramerVSHDMIMatrix_NUMINPUTS];
	
  public:
    ClientKramerVSHDMIMatrix(HardwareSerial &serial);
    void begin();

  private:	// Overloading:
    void _resetDeviceStateVariables();
  	void _parseline();
  	void _sendStatus();
  	void _sendPing();

  public:
	void routeInputToOutput(uint8_t input, uint8_t output);
	uint8_t getRoute(uint8_t output);
	bool getSignal(uint8_t input);
};

#endif