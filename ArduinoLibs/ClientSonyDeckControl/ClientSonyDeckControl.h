/*
Copyright 2012-2014 Kasper Skårhøj, SKAARHOJ, kasper@skaarhoj.com

This file is part of the Sony Deck Control RS-422 Client library for Arduino

The ClientSonyDeckControl library is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published by the 
Free Software Foundation, either version 3 of the License, or (at your 
option) any later version.

The ClientSonyDeckControl library is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
or FITNESS FOR A PARTICULAR PURPOSE. 
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along 
with the ClientSonyDeckControl library. If not, see http://www.gnu.org/licenses/.


IMPORTANT: If you want to use this library in your own projects and/or products,
please play a fair game and heed the license rules! See our web page for a Q&A so
you can keep a clear conscience: http://skaarhoj.com/about/licenses/


*/



#ifndef ClientSonyDeckControl_h
#define ClientSonyDeckControl_h

#include "Arduino.h"
#include "SkaarhojSerialClient.h"

//  #include "SkaarhojPgmspace.h"  - 23/2 2014


#define ClientSonyDeckControl_BUFFERLEN 15

class ClientSonyDeckControl : public SkaarhojSerialClient 
{
  private:
  	uint8_t _binarybuffer[ClientSonyDeckControl_BUFFERLEN];

	uint8_t _binBufferSize;
	uint8_t _binarybufferCheckSum;
	uint8_t _binarybufferExpectedLength;
	uint8_t _lastStatusCheckSum;



		// State variables:
	bool _isPlaying;
	bool _isRecording;
	bool _isForwarding;
	bool _isRewinding;
	bool _isStopped;

	bool _isCassetteOut;
	bool _isInLocalModeOnly;
	bool _isStandby;
	bool _isInJogMode;
	bool _isDirectionBackwards;
	bool _isStill;
	bool _isNearEOT;
	bool _isEOT;		

  public:
    ClientSonyDeckControl(HardwareSerial &serial);
    void begin();
	void pingTimeout(uint16_t timeout);
	
  private:
  	void _resetDeviceStateVariables();
	void _sendStatus();
	void _sendPing();
	void _resetBuffer();
	void _sendBuffer();
	void _readFromClient();
	void _parselineDispatch();

  public:
	bool isOnline();
	bool TXready();


/********************************
* ClientSonyDeckControl State methods
* Returns the most recent information we've 
* got about the device state
 ********************************/

		// Deck status:
	bool isPlaying();		// If playing
	bool isRecording();		// If recording
	bool isForwarding();		// If fast forwarding x2 or more
	bool isRewinding();		// If rewinding x1 or more
	bool isStopped();		// If stopped

		// Other status registers:
	bool isCassetteOut();	
	bool isInLocalModeOnly();
	bool isStandby();		// If 
	bool isInJogMode();
	bool isDirectionBackwards();
	bool isStill();
	bool isNearEOT();	// 3 minutes before
	bool isEOT();	// 30 seconds before


/********************************
 * ClientSonyDeckControl Command methods
 * Asks the deck to do something
 ********************************/
	void doPlay();
	void doRecord();
	void doFastForward();
	void doRewind();
	void doStop();
};

#endif

