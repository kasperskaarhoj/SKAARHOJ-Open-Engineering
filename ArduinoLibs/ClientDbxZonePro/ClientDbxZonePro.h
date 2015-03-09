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



#ifndef ClientDbxZonePro_h
#define ClientDbxZonePro_h

#include "Arduino.h"
#include "SkaarhojTCPClient.h"

#include <Ethernet.h>
//  #include "SkaarhojPgmspace.h"  - 23/2 2014



#define ClientDbxZonePro_BUFFERLEN 50
#define ClientDbxZonePro_SVNUM 10

class ClientDbxZonePro : public SkaarhojTCPClient 
{

  private:
	uint16_t _zoneProNodeId;
	uint16_t _localNodeId;
	
	uint8_t _binBufferSize;
	uint8_t _binarybuffer[ClientDbxZonePro_BUFFERLEN];
	uint8_t _SVstore[ClientDbxZonePro_SVNUM*8];
	uint8_t _SVnumStored;
	
  public:
    ClientDbxZonePro();
    void begin(IPAddress ip);
	void connect();
	bool hasInitialized();

  private:
	void _resetDeviceStateVariables();
	void _readFromClient();
	void _resetBuffer();	
	void _processPayload(int remainingBytes);
	void _sendBuffer();
	void _sendStatus();
	void _sendPing();

	void _insertMsgId(uint16_t msgId);
	void _insertFlags(uint16_t flags);
	void _insertObjectId(uint8_t b0, uint8_t b1, uint8_t b2,uint8_t b3);
	void _insertSVID(uint16_t svId);
	void _insertDataType(uint8_t dataType);
	void _insertData(uint16_t data);

	void _subscribe(uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3);
	void _getAllStates();
	void _setSVValue(uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3, uint16_t SV_ID, uint16_t value);
	void _subscribeSVValues();
		 
  public:
	void setInputGain(uint8_t b0, uint8_t b1, uint16_t gain);
	void setInputEQEnable(uint8_t b0, uint8_t b1, bool enable);
	void setInputEQFlat(uint8_t b0, uint8_t b1, bool flat);
	void setInputEQType(uint8_t b0, uint8_t b1, uint8_t type);
	void setInputEQFreq(uint8_t b0, uint8_t b1, uint8_t band, uint8_t frq);
	void setInputEQWidth(uint8_t b0, uint8_t b1, uint8_t band, uint8_t width);
	void setInputEQLevel(uint8_t b0, uint8_t b1, uint8_t band, uint8_t level);

	void setOutputMute(uint8_t b0, uint8_t b1, bool mute);
	void setOutputMix(uint8_t b0, uint8_t b1, uint8_t input, uint16_t level);
	void setOutputMaster(uint8_t b0, uint8_t b1, uint16_t level);
	void registerSV(uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3, uint16_t SV_ID);
	uint16_t getSVValue(uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3, uint16_t SV_ID);
	
	void disco();
	void recallScene(uint8_t scene);

};

#endif