/*
Copyright 2014 Kasper Skårhøj, LearningLab DTU, kaska@llab.dtu.dk

This file is part of the dbx ZonePro Client library for Arduino

The ClientDbxZonePro library is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published by the 
Free Software Foundation, either version 3 of the License, or (at your 
option) any later version.

The ClientDbxZonePro library is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
or FITNESS FOR A PARTICULAR PURPOSE. 
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along 
with the ClientDbxZonePro library. If not, see http://www.gnu.org/licenses/.



*/


#include "Arduino.h"
#include "ClientDbxZonePro.h"
#include "SkaarhojTCPClient.h"


/**
 * Constructor (using arguments is deprecated! Use begin() instead)
 */
ClientDbxZonePro::ClientDbxZonePro(){}


/**
 * Overloading method: Setting up IP address for the ZonePro (and local port to open telnet connection to)
 */
void ClientDbxZonePro::begin(IPAddress ip){
	
	SkaarhojTCPClient::begin(ip);
	
	_binBufferSize = ClientDbxZonePro_BUFFERLEN;
		
	_localPort = 3804;	// Set local port
	
	_localNodeId = 0x27AD;	// Just something... ? (Should it be 0x0033 which is default serial client?)
	_SVnumStored = 0;
	
	_statusRequestInterval = 8000;	// Use Status messages sent within 10 seconds ("disco" messages) to keep alive.
	_ackMsgInterval = 0;	// Disable pings - we don't need them.
}

/** 
 * Initiating connection handshake to the server. (Overloading from superclass!)
 */
void ClientDbxZonePro::connect() {
	_pendingAnswer = false;
	_hasInitialized = false;
	_resetDeviceStateVariables();

  if (_serialOutput) Serial.print(F("Connecting to dbx ZonePro... "));

  if (_client.connect(_IP, _localPort)) {
    if (_serialOutput) Serial.println(F("connected"));
	_isConnected = true;
    _lastIncomingMsg = millis();
    _lastStatusRequest = millis();  
	disco();
  } else {
    if (_serialOutput) Serial.println(F("connection failed"));
	_isConnected = false;
  }

  _lastReconnectAttempt = millis();
}

/**
 * Resets local device state variables. (Overloading from superclass)
 */
void ClientDbxZonePro::_resetDeviceStateVariables()	{
	_zoneProNodeId = 0xFFFF;
	_ackMsgInterval = 5000;	// Start ping-ing	
}

/**
 * Read from client buffer. (Overloading from superclass)
 */
void ClientDbxZonePro::_readFromClient()	{
	// When we enter here we expect to read a full package at a time, so we reset buffer:
	_resetBuffer();
	uint16_t len = 0;
	bool overflow = false;
	
	while (_client.available()) {
		if (_bufferWriteIndex<_binBufferSize)	{
			_binarybuffer[_bufferWriteIndex++] = _client.read();
		} else {	// Overflow. Only theoretical because we should at some point reach "21" for the headersize, see below.
			overflow = true;
			_client.read();
			_bufferWriteIndex++; 
		}
		
		if (_bufferWriteIndex==21)	{	// Header has been retrieved completely. Subprocess this:
			if (_binarybuffer[1] || _binarybuffer[2])	{
				// Error, we don't have that large a buffer!
				if (_serialOutput)	Serial.println(F("ERROR: Too large package."));
			}
			len = (_binarybuffer[3]<<8) | _binarybuffer[4];
			
			_processPayload(len-_bufferWriteIndex);

			_resetBuffer();
			len = 0;
			overflow = false;
		}
		
		if (overflow)	{
			Serial.print(F("ERROR: Buffer overflow: Package was "));
			Serial.print(len);
			Serial.println(F(" bytes."));
		}
	}
}

/**
 * Parsing package. (Overloading from superclass)
 */
void ClientDbxZonePro::_processPayload(int remainingBytes)	{
	uint16_t readCnt = 0;
		
	if (_serialOutput>2) {
		Serial.print(F("_parseline() incoming package header:\n"));
		for(uint8_t i=0; i<_bufferWriteIndex; i++)	{
			if (_binarybuffer[i]<16)	{
				Serial.print(0);
			}
			Serial.print(_binarybuffer[i],HEX);
			Serial.print(':');
			if (i==20)	{
				Serial.print(' ');
			}
		}
		Serial.print(F(" Remaining bytes="));
		Serial.println(remainingBytes);
	}
	
	uint16_t msgId = (_binarybuffer[17]<<8) | _binarybuffer[18];
	
	switch(msgId)	{
		case 0x0000: 	// Disco message
			if (_zoneProNodeId==0xFFFF)	{
				_zoneProNodeId = (_binarybuffer[5]<<8) | _binarybuffer[6];

				_ackMsgInterval = 0;	// Stop ping-ing
				if (_SVnumStored>0)	{
					_subscribeSVValues();
					_getAllStates();
				}
			}
		break;
		case 0x0100: 	// Set Command
		case 0x0103: 	// Get Command
			_client.read(); readCnt++;
			uint8_t numSVs = _client.read(); readCnt++;
			if (_serialOutput>2)	{
				Serial.print(F("Number of SVs: "));
				Serial.println(numSVs);
			}
			for(uint8_t a=0; a<numSVs; a++)	{
				uint16_t SV_ID = (uint16_t)(_client.read()<<8) | _client.read(); readCnt+=2;
				uint8_t dataType = _client.read(); readCnt++;
				uint16_t dataValue;
				switch(dataType)	{
					case 1:
						dataValue = _client.read(); readCnt++;
					break;
					case 3:
						dataValue = (uint16_t)(_client.read()<<8) | _client.read(); readCnt+=2;
					break;
/*					case 5:	// ULONG
					// Currently, it's a rare occasion and we don't handle it properly. So we only extract the 16 LS bits and ignore the high part.
						_client.read(); _client.read(); readCnt+=2;
						dataValue = (uint16_t)(_client.read()<<8) | _client.read(); readCnt+=2;
					break;
*/					default:
						Serial.print(F("ERROR!! Unknown datatype: "));
						Serial.print(dataType);
						Serial.println(F(". Parsing is broken..."));
						break;
					break;
				}
				_setSVValue(_binarybuffer[10],_binarybuffer[9],_binarybuffer[8],_binarybuffer[7],SV_ID,dataValue);
					
				if (_serialOutput>2)	{
					Serial.print(F(" - SV_ID#"));
					Serial.print(SV_ID);
					Serial.print(F(" = "));
					Serial.println(dataValue);
				}
			}
			if (readCnt!=remainingBytes)	{
				Serial.print(F("ERROR!! read count ("));
				Serial.print(readCnt);
				Serial.print(F(") and remaining bytes ("));
				Serial.print(remainingBytes);
				Serial.println(F(") doesn't add up! Parsing is broken..."));
			}
		break;
	}
	
	while(readCnt < remainingBytes)	{
		_client.read();
		readCnt++;
	}
	
	_resetLastIncomingMsg();
}

/**
 * Resets the byte buffer, sets it to zeros all through. (Overloading from superclass)
 */
void ClientDbxZonePro::_resetBuffer()	{
	memset(_binarybuffer, 0, _binBufferSize);
	_bufferWriteIndex = 0;
}

/**
 * Sends the byte buffer. (Overloading from superclass)
 */
void ClientDbxZonePro::_sendBuffer() {
	if (_bufferWriteIndex)	{
		_binarybuffer[0] = 0x01;
		_binarybuffer[4] = _bufferWriteIndex;
		_binarybuffer[5] = highByte(_localNodeId);
		_binarybuffer[6] = lowByte(_localNodeId);
		_binarybuffer[11] = highByte(_zoneProNodeId);
		_binarybuffer[12] = lowByte(_zoneProNodeId);
		
		_client.write(_binarybuffer, _bufferWriteIndex);
		if (_serialOutput>2) {
			Serial.print(F("_sendBuffer: \n"));
			for(uint8_t i=0; i<_bufferWriteIndex; i++)	{
				if (_binarybuffer[i]<16)	{
					Serial.print(0);
				}
				Serial.print(_binarybuffer[i],HEX);
				Serial.print(':');
				if (i==20)	{
					Serial.print(' ');
				}
			}
			Serial.print(F(" Length="));
			Serial.println(_bufferWriteIndex);
		}
	
		_resetBuffer();
		_pendingAnswer = true;
	}
}

/**
 * Sends ping command. (Overloading from superclass)
 */
void ClientDbxZonePro::_sendPing()	{
	disco();
//	_client.write(0x8C);
}

/**
 * Sends status command. (Overloading from superclass)
 */
void ClientDbxZonePro::_sendStatus()	{
	disco();
}

/**
 * Returns if has initialized - for this particular device we get no information payload back, so we assume this is the case when connected.
 */
bool ClientDbxZonePro::hasInitialized()	{
	return _zoneProNodeId!=0xFFFF;
}





void ClientDbxZonePro::_insertMsgId(uint16_t msgId) {
	_binarybuffer[17] = highByte(msgId);
	_binarybuffer[18] = lowByte(msgId);
}
void ClientDbxZonePro::_insertFlags(uint16_t flags) {
	_binarybuffer[19] = highByte(flags);
	_binarybuffer[20] = lowByte(flags);
}
void ClientDbxZonePro::_insertObjectId(uint8_t b0, uint8_t b1, uint8_t b2,uint8_t b3) {
	_binarybuffer[7] = _binarybuffer[13] = b3;
	_binarybuffer[8] = _binarybuffer[14] = b2;
	_binarybuffer[9] = _binarybuffer[15] = b1;
	_binarybuffer[10] = _binarybuffer[16] = b0;
}

void ClientDbxZonePro::_insertSVID(uint16_t svId) {
	_binarybuffer[22] = 1;	// Payload size = 1
	_binarybuffer[23] = highByte(svId);
	_binarybuffer[24] = lowByte(svId);
}
void ClientDbxZonePro::_insertDataType(uint8_t dataType) {
	_binarybuffer[25] = dataType;
}
void ClientDbxZonePro::_insertData(uint16_t data) {
	switch(_binarybuffer[25])	{
		case 1: // ubyte
			_binarybuffer[26] = data;
			_bufferWriteIndex=27;
		break;
		case 3: // uword
			_binarybuffer[26] = highByte(data);
			_binarybuffer[27] = lowByte(data);
			_bufferWriteIndex=28;
		break;
	}

	// Assuming we are SETTING data:
	_setSVValue(_binarybuffer[10],_binarybuffer[9],_binarybuffer[8],_binarybuffer[7],(uint16_t)(_binarybuffer[23]<<8)|_binarybuffer[24], data);
}









/**
 * gain: 0-221 (0=inf, 221=20db, 201=10db, 181=0db, 161=-10db, 141=-20db, 121=-30db, 101=-40db, 21=-80db)
 */
void ClientDbxZonePro::setInputGain(uint8_t b0, uint8_t b1, uint16_t gain) {
	_resetBuffer();

	_insertMsgId(0x0100);	// Set Command
	_insertObjectId(b0, b1, 0x01, 0x01);

	_insertSVID(0x0000);	
	_insertDataType(3);
	_insertData(gain);

	_sendBuffer();
}

/**
 * Enable/Disable input equalizer
 */
void ClientDbxZonePro::setInputEQEnable(uint8_t b0, uint8_t b1, bool enable) {
	_resetBuffer();

	_insertMsgId(0x0100);	// Set Command
	_insertObjectId(b0, b1, 0x02, 0x01);

	_insertSVID(0x0000);	
	_insertDataType(1);
	_insertData(enable?1:0);

	_sendBuffer();	
}

/**
 * Enable/Disable flat on EQ
 */
void ClientDbxZonePro::setInputEQFlat(uint8_t b0, uint8_t b1, bool flat) {
	_resetBuffer();

	_insertMsgId(0x0100);	// Set Command
	_insertObjectId(b0, b1, 0x02, 0x01);

	_insertSVID(0x0001);	
	_insertDataType(1);
	_insertData(flat?1:0);

	_sendBuffer();	
}

/**
 * Set type of EQ.
 * type = 0-3: 0=Bell Curve, 1=High shelf, 2=Low shelf, 3=High/Low shelf
 */
void ClientDbxZonePro::setInputEQType(uint8_t b0, uint8_t b1, uint8_t type) {
	_resetBuffer();

	_insertMsgId(0x0100);	// Set Command
	_insertObjectId(b0, b1, 0x02, 0x01);

	_insertSVID(0x0002);
	_insertDataType(1);
	_insertData(type);

	_sendBuffer();
}

/**
 * Setting frequency for a given EQ band
 * band = 1-4
 * frq = 0-240 (0=20Hz, 33=50Hz, 57=100Hz, 81=200Hz, 105=400Hz, 129=800Hz, 153=1.6kHz, 177=3.2kHz, 201=6.4kHz, 225=12.8kHz, 240=20.2kHz)
 */
void ClientDbxZonePro::setInputEQFreq(uint8_t b0, uint8_t b1, uint8_t band, uint8_t frq) {
	_resetBuffer();

	_insertMsgId(0x0100);	// Set Command
	_insertObjectId(b0, b1, 0x02, 0x01);

	_insertSVID(0x0003+3*(band-1));
	_insertDataType(1);
	_insertData(frq);

	_sendBuffer();
}

/**
 * Setting width for a given EQ band
 * band = 1-4
 * width = 0-39 (0=0.1, 19=1.5, 39=16.0)
 */
void ClientDbxZonePro::setInputEQWidth(uint8_t b0, uint8_t b1, uint8_t band, uint8_t width) {
	_resetBuffer();

	_insertMsgId(0x0100);	// Set Command
	_insertObjectId(b0, b1, 0x02, 0x01);

	_insertSVID(0x0005+3*(band-1));
	_insertDataType(1);
	_insertData(width);

	_sendBuffer();
}

/**
 * Setting width for a given EQ band
 * band = 1-4
 * level = 0-48 (0=-12dB, 12=-6dB, 24=0dB, 36=6dB, 48=12dB)
 */
void ClientDbxZonePro::setInputEQLevel(uint8_t b0, uint8_t b1, uint8_t band, uint8_t level) {
	_resetBuffer();

	_insertMsgId(0x0100);	// Set Command
	_insertObjectId(b0, b1, 0x02, 0x01);

	_insertSVID(0x0004+3*(band-1));
	_insertDataType(1);
	_insertData(level);

	_sendBuffer();
}

/**
 * Setting output master mute
 */
void ClientDbxZonePro::setOutputMute(uint8_t b0, uint8_t b1, bool mute) {
	_resetBuffer();

	_insertMsgId(0x0100);	// Set Command
	//_insertFlags(0x0400);	
	_insertObjectId(b0, b1, 0x05, 0x01);

	_insertSVID(0x000d);	
	_insertDataType(1);
	_insertData(mute?1:0);

	_sendBuffer();
}

/**
 * Setting volume on a given input for the output
 * input = 1-12
 * level: 0-415 (0=inf, 415=20db, 315=10dB, 215=0db, 115=-10db)
 */
void ClientDbxZonePro::setOutputMix(uint8_t b0, uint8_t b1, uint8_t input, uint16_t level) {
	_resetBuffer();

	_insertMsgId(0x0100);	// Set Command
	//_insertFlags(0x0400);	
	_insertObjectId(b0, b1, 0x05, 0x01);

	_insertSVID(0x0000+(input-1));	
	_insertDataType(3);
	_insertData(level);

	_sendBuffer();
}

/**
 * Setting output master volume
 * level: 0-221 (0=inf, 221=20db, 201=10db, 181=0db, 161=-10db, 141=-20db, 121=-30db, 101=-40db, 21=-80db)
 */
void ClientDbxZonePro::setOutputMaster(uint8_t b0, uint8_t b1, uint16_t level) {
	_resetBuffer();

	_insertMsgId(0x0100);	// Set Command
	//_insertFlags(0x0400);	
	_insertObjectId(b0, b1, 0x05, 0x01);

	_insertSVID(0x000c);	// Master is SV_ID 12 on a ZonePro 1260m
	_insertDataType(3);
	_insertData(level);

	_sendBuffer();
}


/**
 * Recall scene number
 */
void ClientDbxZonePro::recallScene(uint8_t scene) {
	_resetBuffer();

	_insertMsgId(0x9001);	// Recall Scene
	_insertFlags(0x0000);	

	_binarybuffer[21] = 0x00;
	_binarybuffer[22] = scene;

	_bufferWriteIndex=23;
	_sendBuffer();
}

/**
 * Sends disco message (heartbeat)
 */
void ClientDbxZonePro::disco() {
	_resetBuffer();

	_insertFlags(0x0004);
		// Payload:
	_binarybuffer[21] = highByte(_localNodeId);
	_binarybuffer[22] = lowByte(_localNodeId);
	_bufferWriteIndex=22+1;

	_sendBuffer();
}







/**
 * Register a State Variable
 * If done, then this state variable will be subscribed to and if it's value is on the inbound (or outbound) it will be set in memory.
 */
void ClientDbxZonePro::registerSV(uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3, uint16_t SV_ID)	{
	if (_SVnumStored<ClientDbxZonePro_SVNUM)	{
		_SVstore[_SVnumStored*8+0] = b0;
		_SVstore[_SVnumStored*8+1] = b1;
		_SVstore[_SVnumStored*8+2] = b2;
		_SVstore[_SVnumStored*8+3] = b3;
		_SVstore[_SVnumStored*8+4] = highByte(SV_ID);
		_SVstore[_SVnumStored*8+5] = lowByte(SV_ID);
		_SVstore[_SVnumStored*8+6] = 0;
		_SVstore[_SVnumStored*8+7] = 0;
		_SVnumStored++;

		if (_serialOutput>1)	{
			Serial.print(F("registerSV("));
			Serial.print(b0);
			Serial.print(F(","));
			Serial.print(b1);
			Serial.print(F(","));
			Serial.print(b2);
			Serial.print(F(","));
			Serial.print(b3);
			Serial.print(F(","));
			Serial.print(SV_ID);
			Serial.print(F(")\n"));
		}
	}
}

/**
 * Get a registered State Variable's value.
 */
uint16_t ClientDbxZonePro::getSVValue(uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3, uint16_t SV_ID)	{
	for(uint8_t a=0; a<_SVnumStored; a++)	{
		if (	_SVstore[a*8+0]==b0 && 
				_SVstore[a*8+1]==b1 && 
				_SVstore[a*8+2]==b2 && 
				_SVstore[a*8+3]==b3 && 
				_SVstore[a*8+4]==highByte(SV_ID) && 
				_SVstore[a*8+5]==lowByte(SV_ID)
			)	{
				return (uint16_t)(_SVstore[a*8+6]<<8) | _SVstore[a*8+7];
			}
	}
	return 0xFFFF;
}

/**
 * Internal function used to set the value of a registered state variable (both used for incoming data as well as outgoing data.)
 */
void ClientDbxZonePro::_setSVValue(uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3, uint16_t SV_ID, uint16_t value) {
	for(uint8_t a=0; a<_SVnumStored; a++)	{
		if (	_SVstore[a*8+0]==b0 && 
				_SVstore[a*8+1]==b1 && 
				_SVstore[a*8+2]==b2 && 
				_SVstore[a*8+3]==b3 && 
				_SVstore[a*8+4]==highByte(SV_ID) && 
				_SVstore[a*8+5]==lowByte(SV_ID)
			)	{
				_SVstore[a*8+6] = highByte(value);
				_SVstore[a*8+7] = lowByte(value);

				if (_serialOutput>1)	{
					Serial.print(F("_setSVValue("));
					Serial.print(b0);
					Serial.print(F(","));
					Serial.print(b1);
					Serial.print(F(","));
					Serial.print(b2);
					Serial.print(F(","));
					Serial.print(b3);
					Serial.print(F(","));
					Serial.print(SV_ID);
					Serial.print(F("="));
					Serial.print(value);
					Serial.print(F(")\n"));
				}
				return;
			}
	}
}

/**
 * Subscribe to all registered state variables.
 */
void ClientDbxZonePro::_subscribeSVValues() {
	for(uint8_t a=0; a<_SVnumStored; a++)	{
		_subscribe(_SVstore[a*8+0],_SVstore[a*8+1],_SVstore[a*8+2],_SVstore[a*8+3]);
	}
}

/**
 * Subscribes to a given object (shift+ctrl+o when you have selected an object in the ZonePro Program Screen - there you will see values for b0-b3)
 * Subscription means that the ZonePro will send you updates if the value is changed on the device.
 * Subscriptions DO NOT invoke a value message if you set the value yourself.
 */
void ClientDbxZonePro::_subscribe(uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3) {
	_resetBuffer();

	_insertMsgId(0x0113);	// SubscribeAll
	_insertFlags(0x0400);	
	_insertObjectId(b0, b1, b2, b3);

	_binarybuffer[21] = highByte(_localNodeId);
	_binarybuffer[22] = lowByte(_localNodeId);
	_binarybuffer[23] = b3;
	_binarybuffer[24] = b2;
	_binarybuffer[25] = b1;
	_binarybuffer[26] = b0;
	_binarybuffer[27] = 0x01;
		
	_bufferWriteIndex=30;
	_sendBuffer();
}

/**
 * Gets value from a given object (shift+ctrl+o when you have selected an object in the ZonePro Program Screen - there you will see values for b0-b3)
 */
void ClientDbxZonePro::_getAllStates() {
	_resetBuffer();

	_insertMsgId(0x0119);	// Get Command	- or all stuff?
	_insertFlags(0x0001);	
	_insertObjectId(0,0,0,1);

	_bufferWriteIndex=21;
	_sendBuffer();
}