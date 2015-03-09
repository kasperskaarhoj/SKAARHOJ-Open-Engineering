/*
Copyright 2012 Kasper Skårhøj, SKAARHOJ, kasperskaarhoj@gmail.com

This file is part of the ClientGrassValleyKayak library for Arduino

The ClientGrassValleyKayak library is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published by the 
Free Software Foundation, either version 3 of the License, or (at your 
option) any later version.

The ClientGrassValleyKayak library is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
or FITNESS FOR A PARTICULAR PURPOSE. 
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along 
with the ClientGrassValleyKayak library. If not, see http://www.gnu.org/licenses/.

*/


/**
  Version 1.0.2
**/


#ifndef ClientGrassValleyKayak_h
#define ClientGrassValleyKayak_h

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif


#include <EthernetUdp.h>
//  #include "SkaarhojPgmspace.h"  - 23/2 2014

class ClientGrassValleyKayak
{
  private:
	EthernetUDP _Udp;			// Udp Object for communication, see constructor.
	uint16_t _commandPort; 		// local port to send from
	IPAddress _switcherIP;		// IP address of the switcher
	boolean _serialOutput;		// If set, the library will print status/debug information to the Serial object	
	
	uint8_t _packetBuffer[96];
	
  public:
    ClientGrassValleyKayak();
    void begin(const IPAddress ip);
    void connect();
    void runLoop();
	bool isConnectionTimedOut();

  private:
	void _parsePacket(uint16_t packetLength);
	void _sendAnswerPacket(uint16_t remotePacketID);
	void _sendCommand(uint8_t len);
	
  public:

/********************************
 * General Getter/Setter methods
 ********************************/
  	void serialOutput(boolean serialOutput);

/********************************
* ClientGrassValleyKayak Switcher state methods
* Returns the most recent information we've 
* got about the switchers state
 ********************************/
	uint8_t getProgramInput();
	uint8_t getPreviewInput();
	boolean getProgramTally(uint8_t inputNumber);
	boolean getPreviewTally(uint8_t inputNumber);
	uint16_t getTransitionPosition();

/********************************
 * ClientGrassValleyKayak Switcher Change methods
 * Asks the switcher to changes something
 ********************************/
	void changeProgramInput(uint8_t inputNumber);
	void changePresetInput(uint8_t inputNumber);
	void doCut();
	void doAuto();
	void changeTransitionPosition(unsigned int value);
};

#endif

