/*
Copyright 2016 Filip Sandborg-Olsen, SKAARHOJ, filip@skaarhoj.com

This file is part of the ClientAJAKumoTCP library for Arduino

The ClientAJAKumoTCP library is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published by the 
Free Software Foundation, either version 3 of the License, or (at your 
option) any later version.

The ClientAJAKumoTCP library is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
or FITNESS FOR A PARTICULAR PURPOSE. 
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along 
with the ClientAJAKumoTCP library. If not, see http://www.gnu.org/licenses/.

*/

#ifndef ClientAJAKumoTCP_h
#define ClientAJAKumoTCP_h

#include "Arduino.h"
#include <Ethernet.h>
#include <Streaming.h>

#define ClientAJAKumoTCP_NUMINPUTS 16
#define ClientAJAKumoTCP_NUMOUTPUTS 16

#define ClientAJAKumoTCP_INPUTBUFFER_SIZE 100

class ClientAJAKumoTCP {
private:
	uint16_t calculateChecksum(uint8_t *packet, uint8_t len);
	char toASCII(uint8_t c);
	uint8_t fromASCII(char c);
	bool receiveData(); 
	void transmitPacket(char *buffer);
	void parseIncoming(uint8_t *buffer, uint8_t len);
	void updateRouting(uint8_t num);
	void handleCmd(char* cmd, char* parameter, char* data);
	uint16_t hexToDec(char* str, uint8_t len);

	uint8_t _sessionID;

	IPAddress _IP;

	EthernetClient _client;

	uint8_t _inputBuffer[ClientAJAKumoTCP_INPUTBUFFER_SIZE];
	uint8_t _inputPos;
	bool _isConnected;
	uint32_t _lastConnectionAttempt;

	uint8_t routingState[ClientAJAKumoTCP_NUMOUTPUTS];

	uint8_t _serialOutput;

	uint32_t _lastRoutingUpdate;
	uint32_t _lastSingleRouteUpdate;
	uint8_t _updatePointer;

	uint8_t _receivedRouting;
public:
	ClientAJAKumoTCP();
	void begin(IPAddress ip);

	void routeInputToOutput(uint8_t input, uint8_t output);
	uint8_t getRoute(uint8_t output);
	void runLoop();
	void serialOutput(uint8_t level);

};

#endif