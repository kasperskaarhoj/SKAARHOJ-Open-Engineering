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


#include "UDPmessenger.h"


/**
 * Constructor (using arguments is deprecated! Use begin() instead)
 */
UDPmessenger::UDPmessenger(){}

/**
 * Setting up LOCAL IP address (used to deduct which 255.255.255.0 subnet all communication is on) and listening/sending port
 */
void UDPmessenger::begin(const IPAddress ip, const uint16_t localPort){

		// Set up Udp communication object:
	#ifdef ESP8266
	WiFiUDP Udp;
	#else
	EthernetUDP Udp;
	#endif
	
	_Udp = Udp;
	
	_localIP = ip;		// This IP
	_localPort = localPort;		// Local port to write/receive on.
	
	_serialOutput = false;

	_maxResponseTime = 10000;	// 1000 us, should be fine for a local network.
	_retryTimeout = 10000;	// 10000 ms between retries.
	
	memset(_slaveLastResponseTime,0,UDPmessenger_SLAVE_POOL+1);
	memset(_slaveIndexToAddress,0,UDPmessenger_SLAVE_POOL+1);
	memset(_slaveLastRetryTime,0,UDPmessenger_SLAVE_POOL+1);
	

	_Udp.begin(_localPort);
	resetTXBuffer();
}

/**
 * Sets up an address which we will track regarding network access. This should be done for all addresses which we will initiate connection to (being a master for), since we don't know if they will respond.
 * All addresses which treats us as a slave can be assumed to exist and we don't have to worry about those.
 * The reason is: If we try to talk to non-present network nodes, we a) waste time on timeouts and b) actually may experience weird behaviour by the Arduino UDP library which may send double-sized packets it seems.
 * Returns index, if it was possible to allocate one.
 */
uint8_t UDPmessenger::trackAddress(uint8_t address)	{
	if (!addressToIndex(address))	{	// If not already registered, search:
		for(uint8_t i=0; i<UDPmessenger_SLAVE_POOL; i++)	{
			if (_slaveIndexToAddress[i+1]==0)	{
				_slaveIndexToAddress[i+1] = address;
				break;
			}
		}
	}
	return addressToIndex(address);
}

/**
 * Searches for an index matching the address, returns it if found, otherwise zero.
 */
uint8_t UDPmessenger::addressToIndex(uint8_t address)	{
	for(uint8_t i=0; i<UDPmessenger_SLAVE_POOL; i++)	{
		if (_slaveIndexToAddress[i+1]==address)	{
			return i+1;
		}
	}
	return 0;	// Returns index zero if not found
}

/**
 * Returns true if missing nodes should be tried again
 */
bool UDPmessenger::retryMissingNodes(uint8_t idx)	{
    if ((unsigned long)(_slaveLastRetryTime[idx] + _retryTimeout) < (unsigned long)millis())  {
	  _slaveLastRetryTime[idx] = millis()+idx*100;
	  Serial << F("Retry ") << idx << F("\n");
      return true; 
    } 
    else {
      return false;
    }
}

/**
 * Returns true if missing nodes should be tried again
 */
uint8_t UDPmessenger::ownAddress()	{
	return _localIP[3];
}

/**
 * Registering call-back function
 */
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
		resetTXBuffer();
//  		Serial.print(F("Packet size: "));
//	    Serial.println(packetSize, DEC);

	    // Read packet header of 2 bytes (length):
	    _Udp.read(_rxPacketBuffer, UDPmessenger_PGKHDR_SIZE);

	    // Read out packet length (first word):
	    uint16_t packetLength = word(_rxPacketBuffer[0] & B00000111, _rxPacketBuffer[1]);

	    if (packetSize==packetLength) {  // Just to make sure these are equal, they should be!
			if (packetLength > UDPmessenger_PGKHDR_SIZE)	{
				_parsePacket(packetLength);
				send();	// Sends a response if we build one
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
 // Serial << F("Parse-begin: ") << packetLength << F("\n");

      uint16_t indexPointer = UDPmessenger_PGKHDR_SIZE;	// header size: 2 bytes has already been read from the whole packet...
      while (indexPointer < packetLength)  {

        // Read the length of segment (first byte):
        _Udp.read(_rxPacketBuffer, 2);
        uint8_t cmdLength = _rxPacketBuffer[0];
    	uint8_t cmd = _rxPacketBuffer[1];

//	  Serial << cmd << F("-") << cmdLength << F("\n");
 
       if (cmdLength>=2 && cmdLength-2<=UDPmessenger_BUFFER_SIZE)  {
		  if (cmdLength>2)	{_Udp.read(_rxPacketBuffer, cmdLength-2);}
		
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
//	  Serial << F("Parse-end\n");
}

/**
 * Clearing TX buffer and pointer
 */
void UDPmessenger::resetTXBuffer()	{
  memset(_txPacketBuffer, 0, UDPmessenger_BUFFER_SIZE);
  _txBufferPointer = UDPmessenger_PGKHDR_SIZE;
}

/**
 * Returns true if a command of given data length can fit in buffer
 */
bool UDPmessenger::fitCommand(const uint8_t dataLength)	{
	if (_txBufferPointer+2+dataLength <= UDPmessenger_BUFFER_SIZE)	{
		return true;
	} else return false;
}

/**
 * Adds a command to the TX buffer. Returns true if there was space enough for it! 
 * Must call AFTER adding values for the command. Check if there is space for it using fitCommand()
 */
bool UDPmessenger::addCommand(const uint8_t cmd, const uint8_t dataLength)	{
	if (_txBufferPointer+2+dataLength <= UDPmessenger_BUFFER_SIZE)	{
		_txPacketBuffer[_txBufferPointer] = dataLength+2;
		_txPacketBuffer[_txBufferPointer+1] = cmd;
		_txBufferPointer+= 2+dataLength;
		return true;
	} else return false;
}

/**
 * Writes bytes to the current command in the TX buffer. Returns true if within buffer memory area
 */
bool UDPmessenger::addValueToDataBuffer(const uint8_t value, const uint8_t position)	{
	if (_txBufferPointer+2+position < UDPmessenger_BUFFER_SIZE)	{
		_txPacketBuffer[_txBufferPointer+2+position] = value;
		return true;
	} else return false;
}

/**
 * Write TX buffer back remote IP/port
 */
void UDPmessenger::send()	{
    IPAddress remote = _Udp.remoteIP();
	send(remote[3], _Udp.remotePort(), true);
}

/**
 * Write TX buffer to own choice of address and port
 */
void UDPmessenger::send(uint8_t address, uint16_t port, bool bypassResponseTimeCheck)	{
  uint8_t idx = 0;
  if (_txBufferPointer>2)	{
	  if (!bypassResponseTimeCheck)	{
		  idx = addressToIndex(address);
	  }
	  
	  if (bypassResponseTimeCheck || idx==0 || _slaveLastResponseTime[idx]<_maxResponseTime || retryMissingNodes(idx))	{
		  IPAddress remote(_localIP[0], _localIP[1], _localIP[2], address);
		  _Udp.beginPacket(remote,  port);
			_txPacketBuffer[0] = highByte(_txBufferPointer);
			_txPacketBuffer[1] = lowByte(_txBufferPointer);
		  _Udp.write(_txPacketBuffer,_txBufferPointer);
		  
		  /*
			  Serial << F("UDPmessenger: addr:") << address << F(": ");
			  	for(uint8_t i=0; i<_txBufferPointer; i++)	{
			  		Serial << _HEXPADL(_txPacketBuffer[i],2,"0") << F(" ");
			  	}
			  Serial << F("\n");
			*/  

		  if (!bypassResponseTimeCheck && idx)	{	// Update response time:
			  unsigned long test = micros();
			  _Udp.endPacket();
			   unsigned long res = micros()-test;
		   	  _slaveLastResponseTime[idx] = res > 0xFFFF ? 0xFFFF : res;
		/*	  if (_serialOutput) 	{
				  Serial.print(address);
				  Serial.print(F(": "));
				  Serial.println(res);
			  }*/
		  } else {
		  	_Udp.endPacket();
		  }
	  }
  }
  resetTXBuffer();
}

/**
 * Returns true if a slave (registered to be tracked) has a good response time
 */
bool UDPmessenger::isPresent(uint8_t address)	{
	uint8_t idx = addressToIndex(address);
	return (idx && _slaveLastResponseTime[idx]>0) ? _slaveLastResponseTime[idx]<_maxResponseTime : false;
}

/**
 * Setter method: If _serialOutput is set, the library may use Serial.print() to give away information about its operation - mostly for debugging.
 */
void UDPmessenger::serialOutput(boolean serialOutput) {
	_serialOutput = serialOutput;
}
