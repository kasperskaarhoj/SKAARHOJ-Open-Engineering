/*
Copyright 2013 Kasper Skårhøj, SKAARHOJ, kasperskaarhoj@gmail.com

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

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include "UDPmessenger.h"


/**
 * Constructor (using arguments is deprecated! Use begin() instead)
 */
UDPmessenger::UDPmessenger(){}

/**
 * Setting up LOCAL IP address (used to deduct which 255.255.255.0 subnet all communication is on) and listening port
 */
void UDPmessenger::begin(const IPAddress ip, const uint16_t localPort){

		// Set up Udp communication object:
	EthernetUDP Udp;
	_Udp = Udp;
	
	_localIP = ip;
	_localPort = localPort;	
	
	_serialOutput = false;

	_Udp.begin(_localPort);
	clearDataArray();
}

void UDPmessenger::registerReceptionCallbackFunction(void (*fptr)(const uint8_t cmd, const uint8_t from, const uint8_t dataLength, const uint8_t *dataArray))			{
	 _receptionCallBack = fptr; 
}

/**
 * Keeps receiving packets and calls callback function
 */
void UDPmessenger::runLoop() {

	while(true) {	// Iterate until buffer is empty:
  	  uint16_t packetSize = _Udp.parsePacket();
	  if (_Udp.available() && packetSize !=0)   {  
		clearDataArray();
//  		Serial.print(F("Packet size: "));
//	    Serial.println(packetSize, DEC);

	    // Read packet header of 2 bytes (length):
	    _Udp.read(_rxPacketBuffer, UDPmessenger_PGKHDR_SIZE);

	    // Read out packet length (first word):
	    uint16_t packetLength = word(_rxPacketBuffer[0] & B00000111, _rxPacketBuffer[1]);

	    if (packetSize==packetLength) {  // Just to make sure these are equal, they should be!
			if (packetLength > UDPmessenger_PGKHDR_SIZE)	{
				_parsePacket(packetLength);
				send();
			}
	    } else {
			if (_serialOutput) 	{
	      		Serial.print(F("ERROR: Packet size mismatch: "));
			    Serial.print(packetSize, DEC);
			    Serial.print(F(" != "));
			    Serial.println(packetLength, DEC);
			}
			// Flushing the buffer:
			// TODO: Other way? _Udp.flush() ??
	          while(_Udp.available()) {
	              _Udp.read(_rxPacketBuffer, UDPmessenger_BUFFER_SIZE);
	          }
	    }

  		//Serial.println(F("Done with packet"));
	  } else {
		break;	// Exit while(true) loop because there is no more packets in buffer.
	}
  }
}

/**
 * Processes each command in package
 */
void UDPmessenger::_parsePacket(uint16_t packetLength)	{

      uint16_t indexPointer = UDPmessenger_PGKHDR_SIZE;	// header size: 2 bytes has already been read from the whole packet...
      while (indexPointer < packetLength)  {

        // Read the length of segment (first byte):
        _Udp.read(_rxPacketBuffer, 2);
        uint8_t cmdLength = _rxPacketBuffer[0];
    	uint8_t cmd = _rxPacketBuffer[1];
 
       if (cmdLength>=2 && cmdLength-2<=UDPmessenger_BUFFER_SIZE)  {
		  if (cmdLength>=2)	{_Udp.read(_rxPacketBuffer, cmdLength-2);}
		
			if (_receptionCallBack != NULL)	{
			    IPAddress remote = _Udp.remoteIP();
				_receptionCallBack(cmd, remote[3], cmdLength-2, _rxPacketBuffer);
			}
				
          indexPointer+= cmdLength;
        } else { 
			indexPointer = 2000;
			while(_Udp.available()) {
				_Udp.read(_rxPacketBuffer, UDPmessenger_BUFFER_SIZE);
			}
		}
      }
}

void UDPmessenger::clearDataArray()	{
  memset(_txPacketBuffer, 0, UDPmessenger_BUFFER_SIZE);
  _txBufferPointer = UDPmessenger_PGKHDR_SIZE;
}
bool UDPmessenger::addValueToDataBuffer(const uint8_t value, const uint8_t position)	{
	if (_txBufferPointer+2+position < UDPmessenger_BUFFER_SIZE)	{
		_txPacketBuffer[_txBufferPointer+2+position] = value;
		return true;
	} else return false;
}
bool UDPmessenger::addCommand(const uint8_t cmd, const uint8_t dataLength)	{
	if (_txBufferPointer+2+dataLength <= UDPmessenger_BUFFER_SIZE)	{
		_txPacketBuffer[_txBufferPointer] = dataLength+2;
		_txPacketBuffer[_txBufferPointer+1] = cmd;
		_txBufferPointer+= 2+dataLength;
		return true;
	} else return false;
}
void UDPmessenger::send()	{
    IPAddress remote = _Udp.remoteIP();
	send(remote[3], _Udp.remotePort());
}
void UDPmessenger::send(uint8_t address, uint16_t port)	{
  if (_txBufferPointer>2)	{
	  IPAddress remote(_localIP[0], _localIP[1], _localIP[2], address);
	  _Udp.beginPacket(remote,  port);
		_txPacketBuffer[0] = highByte(_txBufferPointer);
		_txPacketBuffer[1] = lowByte(_txBufferPointer);
	  _Udp.write(_txPacketBuffer,_txBufferPointer);
	  _Udp.endPacket();
  }
  clearDataArray();
}

/**
 * Setter method: If _serialOutput is set, the library may use Serial.print() to give away information about its operation - mostly for debugging.
 */
void UDPmessenger::serialOutput(boolean serialOutput) {
	_serialOutput = serialOutput;
}
