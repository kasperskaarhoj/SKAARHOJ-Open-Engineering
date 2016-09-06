/*
Copyright 2012 Kasper Skårhøj, SKAARHOJ, kasperskaarhoj@gmail.com

This file is part of the UDPmessenger library for Arduino

The UDPmessenger library is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published by the 
Free Software Foundation, either version 3 of the License, or (at your 
option) any later version.

The UDPmessenger library is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
or FITNESS FOR A PARTICULAR PURPOSE. 
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along 
with the UDPmessenger library. If not, see http://www.gnu.org/licenses/.

*/


/**
  Version 1.0.0
**/


#ifndef UDPmessenger_h
#define UDPmessenger_h

#include "Arduino.h"
#ifdef ESP8266
#include <WifiUDP.h>
#else
#include <EthernetUdp.h>
#endif
#include <SkaarhojPgmspace.h>
#include <Streaming.h>


#define UDPmessenger_BUFFER_SIZE 64
#define UDPmessenger_PGKHDR_SIZE 2
#define UDPmessenger_SLAVE_POOL 10

class UDPmessenger
{
  private:
  	#ifdef ESP8266
	WiFiUDP _Udp;			// Udp Object for communication, see constructor.
	#else
	EthernetUDP _Udp;
	#endif
	uint16_t _localPort; 		// local port to send/receive from
	IPAddress _localIP;		// IP address of the local arduino
	boolean _serialOutput;		// If set, the library will print status/debug information to the Serial object
	uint8_t _slaveIndexToAddress[UDPmessenger_SLAVE_POOL+1];   			// Mapping between index and addresses. The +1 address is to unknown addresses.
	uint16_t _slaveLastResponseTime[UDPmessenger_SLAVE_POOL+1];   			// Responsetimes in us last time we tried to talk to it
	unsigned long _slaveLastRetryTime[UDPmessenger_SLAVE_POOL+1];	// Last retry times...
	
	uint8_t _rxPacketBuffer[UDPmessenger_BUFFER_SIZE];   			// Buffer for storing segments of the packets from UDPmessenger
	uint8_t _txPacketBuffer[UDPmessenger_BUFFER_SIZE];   			// Buffer for building outgoing packets (done simultaneously with receiving, so therefore needs to exist separately)
	uint16_t _txBufferPointer;	// Next write location in _txPacketBuffer for a command.
	uint16_t _maxResponseTime;
	uint16_t _retryTimeout;
	
public:
	
    UDPmessenger();
	void begin(const IPAddress ip, const uint16_t localPort);
	uint8_t trackAddress(uint8_t address);
	uint8_t addressToIndex(uint8_t address);
	bool retryMissingNodes(uint8_t idx);
	uint8_t ownAddress();
	void registerReceptionCallbackFunction(void (*fptr)(const uint8_t cmd, const uint8_t from, const uint8_t dataLength, const uint8_t *dataArray));
    void runLoop();
	void serialOutput(boolean serialOutput);
	void resetTXBuffer();
	bool addValueToDataBuffer(const uint8_t value, const uint8_t position);
	bool fitCommand(const uint8_t dataLength);
	bool addCommand(const uint8_t cmd, const uint8_t dataLength);
	void send();
	void send(uint8_t address, uint16_t port, bool bypassResponseTimeCheck=false);
	bool isPresent(uint8_t address);
	
  private:
	void (*_receptionCallBack)(const uint8_t cmd, const uint8_t from, const uint8_t dataLength, const uint8_t *dataArray);
	void _parsePacket(uint16_t packetLength);
};

#endif

