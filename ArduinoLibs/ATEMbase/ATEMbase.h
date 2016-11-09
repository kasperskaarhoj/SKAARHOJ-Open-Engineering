/*
Copyright 2012-2014 Kasper Skårhøj, SKAARHOJ K/S, kasper@skaarhoj.com

This file is part of the Blackmagic Design ATEM Client library for Arduino

The ATEM library is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published by the 
Free Software Foundation, either version 3 of the License, or (at your 
option) any later version.

The ATEM library is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
or FITNESS FOR A PARTICULAR PURPOSE. 
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along 
with the ATEM library. If not, see http://www.gnu.org/licenses/.


IMPORTANT: If you want to use this library in your own projects and/or products,
please play a fair game and heed the license rules! See our web page for a Q&A so
you can keep a clear conscience: http://skaarhoj.com/about/licenses/


*/




#ifndef ATEMbase_h
#define ATEMbase_h

#include "Arduino.h"

#ifdef ESP8266
#include <WifiUDP.h>
#else
#include <EthernetUdp.h>
#endif

#include <SkaarhojPgmspace.h>

#define ATEM_headerCmd_AckRequest 0x1	// Please acknowledge reception of this package...
#define ATEM_headerCmd_HelloPacket 0x2	
#define ATEM_headerCmd_Resend 0x4			// This is a resent information
#define ATEM_headerCmd_RequestNextAfter 0x8	// I'm requesting you to resend something to me.
#define ATEM_headerCmd_Ack 0x10		// This package is an acknowledge to package id (byte 4-5) ATEM_headerCmd_AckRequest

#define ATEM_maxInitPackageCount 40		// The maximum number of initialization packages. By observation on a 2M/E 4K can be up to (not fixed!) 32. We allocate a f more then...
#define ATEM_packetBufferLength 96		// Size of packet buffer

#define ATEM_debug 0				// If "1" (true), more debugging information may hit the serial monitor, in particular when _serialDebug = 0x80. Setting this to "0" is recommended for production environments since it saves on flash memory.

class ATEMbase
{
  protected:
  	#ifdef ESP8266
  	WiFiUDP _Udp;
  	#else
	EthernetUDP _Udp;					// UDP object for communication, see constructor.
	#endif
	uint16_t _localPort; 				// Default local port to send from. Preferably it's chosen randomly inside the class.
	IPAddress _switcherIP;				// IP address of the switcher
	uint8_t _serialOutput;				// If set, the library will print status/debug information to the Serial object

	// ATEM Connection Basics
	uint16_t _localPacketIdCounter;  	// This is our counter for the command packages we might like to send to ATEM
	boolean _initPayloadSent;  			// If true, the initial reception of the ATEM memory has passed and we can begin to respond during the runLoop()
	uint8_t _initPayloadSentAtPacketId;	// The Remote Package ID at which point the initialization payload was completed.
	boolean _hasInitialized;  			// If true, all initial payload packets has been received during requests for resent - and we are completely ready to rock!
	boolean _isConnected;				// Set true if we have received a hello package from the switcher.
	uint16_t _sessionID;				// Session id of session, given by ATEM switcher
	unsigned long _lastContact;			// Last time (millis) the switcher sent a packet to us.
	uint16_t _lastRemotePacketID;		// The most recent Remote Packet Id from switcher
	uint8_t _missedInitializationPackages[(ATEM_maxInitPackageCount+7)/8];	// Used to track which initialization packages have been missed
	uint8_t _returnPacketLength;	
	
	// ATEM Buffer:
	uint8_t _packetBuffer[ATEM_packetBufferLength];   		// Buffer for storing segments of the packets from ATEM and creating answer packets.

	uint16_t _cmdLength;				// Used when parsing packets
	uint16_t _cmdPointer;				// Used when parsing packets

	bool _cBundle;				// If set, we are building a set-command bundle.
	uint8_t _cBBO;		// Bundle Buffer Offset; This is an offset if you want to add more commands.

	uint8_t _ATEMmodel;

	bool neverConnected;
	bool waitingForIncoming;
	
  public:
    ATEMbase();
	void begin(const IPAddress ip);
	void begin(const IPAddress ip, const uint16_t localPort);
    void connect();
    void connect(const boolean useFixedPortNumber);
    void runLoop();
	void runLoop(uint16_t delayTime);
		
	uint16_t getATEM_lastRemotePacketId();
	uint16_t getSessionID();
	
	bool isConnected();
	bool hasInitialized();

  	void serialOutput(uint8_t level);
	bool hasTimedOut(unsigned long time, unsigned long timeout);

	float audioWord2Db(uint16_t input);
	uint16_t audioDb2Word(float input);

  	uint8_t getVideoSrcIndex(uint16_t videoSrc);
  	uint8_t getAudioSrcIndex(uint16_t audioSrc);
	
	uint16_t getVideoIndexSrc(uint8_t index);
	uint16_t getAudioIndexSrc(uint8_t index);
	
	uint8_t maxAtemSeriesVideoInputs();
		
	void commandBundleStart();
	void commandBundleEnd();
	void resetCommandBundle();
	
	uint8_t getATEMmodel();

  protected:
  	void _createCommandHeader(const uint8_t headerCmd, const uint16_t lengthOfData);
  	void _createCommandHeader(const uint8_t headerCmd, const uint16_t lengthOfData, const uint16_t remotePacketID);
  	void _sendPacketBuffer(uint8_t length);
	void _wipeCleanPacketBuffer();

	void _parsePacket(uint16_t packetLength);
	virtual void _parseGetCommands(const char *cmdString);
	bool _readToPacketBuffer();
	bool _readToPacketBuffer(uint8_t maxBytes);
	void _prepareCommandPacket(const char *cmdString, uint8_t cmdBytes, bool indexMatch=true);
	void _finishCommandPacket();
};

#endif

