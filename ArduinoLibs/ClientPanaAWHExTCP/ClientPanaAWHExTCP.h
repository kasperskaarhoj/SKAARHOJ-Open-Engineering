/*
Copyright 2013 Kasper Skårhøj, SKAARHOJ, kasperskaarhoj@gmail.com

This file is part of the Panasonic AW-HE series Robotic Camera Client library for Arduino

The Panasonic AW-HE library is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published by the 
Free Software Foundation, either version 3 of the License, or (at your 
option) any later version.

The Panasonic AW-HE library is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
or FITNESS FOR A PARTICULAR PURPOSE. 
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along 
with the Panasonic AW-HE library. If not, see http://www.gnu.org/licenses/.

*/


/**
  Version 1.0.2
**/


#ifndef ClientPanaAWHExTCP_h
#define ClientPanaAWHExTCP_h

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include <Streaming.h>

#define PanaAWHE_BUFSIZE 30
#define PanaAWHE_QUEUESIZE 100
#define PanaAWHE_NUMCAMS 8

//  #include "SkaarhojPgmspace.h"  - 23/2 2014
#include <Ethernet.h>
#include <utility/w5100.h>

class ClientPanaAWHExTCP
{
  private:
	IPAddress _cameraIP;		// IP address of the camera
	uint8_t _baseAddr;
	uint8_t _serialOutput;
	EthernetClient _client;
	bool _activeHTTPRequest;
	uint32_t _activeHTTPRequestTime;
	uint32_t _lastSeen[PanaAWHE_NUMCAMS];
	
	char _charBuf[96];
	char _cmdBuf[PanaAWHE_BUFSIZE];

	bool _isOnline[PanaAWHE_NUMCAMS];
	
  public:

    ClientPanaAWHExTCP();
    ClientPanaAWHExTCP(const IPAddress ip);
    void begin(const IPAddress ip);
	void serialOutput(bool flag);

	void runLoop();
    void connect();
	void changeLastIPBytes(uint8_t lastByte);
	bool isReady();
	bool isConnected(uint8_t cam);
	
    bool doPan(uint8_t cam, uint8_t panSpeed);
    bool doTilt(uint8_t cam, uint8_t tiltSpeed);
    bool doZoom(uint8_t cam, uint8_t zoomSpeed);
	bool doPanTilt(uint8_t cam, uint8_t panSpeed,uint8_t tiltSpeed);
	bool setAutoFocus(uint8_t cam, bool enable);
	bool doFocus(uint8_t cam, uint8_t focusPos);
	bool onTouchAutofocus(uint8_t cam);
	bool setAutoIris(uint8_t cam, bool enable);
	bool deletePreset(uint8_t cam, uint8_t presetNum);
	bool storePreset(uint8_t cam, uint8_t presetNum);
	bool recallPreset(uint8_t cam, uint8_t presetNum);
	bool power(uint8_t cam, bool enable);
	
	bool setContrast(uint8_t cam, uint8_t contrast);
	bool setColorBars(uint8_t cam, bool state);
	bool setShutter(uint8_t cam, uint8_t shutter);
	bool setSensorGain(uint8_t cam, uint8_t gain);
	bool setGainR(uint8_t cam, int8_t gain);
	bool setGainB(uint8_t cam, int8_t gain);
	bool setPedestalR(uint8_t cam, int8_t pedestal);
	bool setPedestalB(uint8_t cam, int8_t pedestal);
	bool setIris(uint8_t cam, uint16_t iris);
	bool setPresetSpeed(uint8_t cam, uint8_t speed);

	int8_t getGainR(uint8_t cam);
	int8_t getGainB(uint8_t cam);
	int8_t getPedestalR(uint8_t cam);
	int8_t getPedestalB(uint8_t cam);
	uint16_t getIris(uint8_t cam);
	uint8_t getPresetSpeed(uint8_t cam);
	bool presetExists(uint8_t cam, uint8_t preset);
	bool getAutoIris(uint8_t cam);
	uint8_t getShutter(uint8_t cam);
	uint8_t getSensorGain(uint8_t cam);


  private:
	void _sendPtzRequest(uint8_t cam, uint8_t cmdlen, const char* format, ...);
	void _sendCamRequest(uint8_t cam, uint8_t cmdlen, const char* format, ...);
	void _sendRequest(uint8_t cam, const char* command, uint8_t len, bool camRequest);
	void _addToQueue(uint8_t cam, uint8_t cmdlen, const char* command, bool camRequest);
	uint8_t _getNextQueue(uint8_t start = 0);
	void _popQueue(uint8_t pos = 0);
	void _parseIncoming(char* buffer); 
	bool _sendPing();
	bool _requestState(uint8_t cam);
	uint32_t _lastPingAttempt;

	char _queue[PanaAWHE_QUEUESIZE];
	uint8_t _queuePtr;

	int8_t _gainR[PanaAWHE_NUMCAMS];
	int8_t _gainB[PanaAWHE_NUMCAMS];
	int8_t _pedestalR[PanaAWHE_NUMCAMS];
	int8_t _pedestalB[PanaAWHE_NUMCAMS];
	uint16_t _iris[PanaAWHE_NUMCAMS];
	uint8_t _sensorGain[PanaAWHE_NUMCAMS];
	bool _colorBars[PanaAWHE_NUMCAMS];
	bool _autoIris[PanaAWHE_NUMCAMS];
	uint32_t _presetState[PanaAWHE_NUMCAMS];
	uint8_t _presetSpeed[PanaAWHE_NUMCAMS];
	uint8_t _shutter[PanaAWHE_NUMCAMS];

	uint8_t _stateRequestPointer[PanaAWHE_NUMCAMS];
	uint32_t _lastStateRequest;
	uint32_t _lastSentCommand[PanaAWHE_NUMCAMS];
	uint8_t _lastStateCam;
};

#endif

