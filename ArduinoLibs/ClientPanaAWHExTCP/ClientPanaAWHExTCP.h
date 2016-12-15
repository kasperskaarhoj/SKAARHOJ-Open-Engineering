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

//  #include "SkaarhojPgmspace.h"  - 23/2 2014
#include <Ethernet.h>

class ClientPanaAWHExTCP
{
  private:
	IPAddress _cameraIP;		// IP address of the camera
	bool _serialOutput;
	EthernetClient _client;
	bool _activeHTTPRequest;
	unsigned long _activeHTTPRequestTime;
	uint32_t _lastSeen;
	
	char _charBuf[96];
	char _cmdBuf[PanaAWHE_BUFSIZE];
	
  public:

    ClientPanaAWHExTCP();
    ClientPanaAWHExTCP(const IPAddress ip);
    void begin(const IPAddress ip);
	void serialOutput(bool flag);

	void runLoop();
    void connect();
	void changeLastIPBytes(uint8_t lastByte);
	bool isReady();
	bool isConnected();
	
    bool doPan(uint8_t panSpeed);
    bool doTilt(uint8_t tiltSpeed);
    bool doZoom(uint8_t zoomSpeed);
	bool doPanTilt(uint8_t panSpeed,uint8_t tiltSpeed);
	bool setAutoFocus(bool enable);
	bool doFocus(uint8_t focusPos);
	bool onTouchAutofocus();
	bool setAutoIris(bool enable);
	bool deletePreset(uint8_t presetNum);
	bool storePreset(uint8_t presetNum);
	bool recallPreset(uint8_t presetNum);
	bool power(bool enable);
	
	bool setContrast(uint8_t contrast);
	bool setColorBars(bool state);
	bool setShutter(uint8_t shutter);
	bool setSensorGain(uint8_t gain);
	bool setGainR(int8_t gain);
	bool setGainB(int8_t gain);
	bool setPedestalR(int8_t pedestal);
	bool setPedestalB(int8_t pedestal);
	bool setIris(uint16_t iris);

	int8_t getGainR();
	int8_t getGainB();
	int8_t getPedestalR();
	int8_t getPedestalB();
	uint16_t getIris();

  private:
	void _sendPtzRequest(const char* format, ...);
	void _sendCamRequest(const char* format, ...);
	void _sendRequest(const char* command, bool camRequest);
	void _parseIncoming(char* buffer); 
	bool _sendPing();
	void _requestState();
	uint32_t _lastPingAttempt;

	int8_t _gainR;
	int8_t _gainB;
	int8_t _pedestalR;
	int8_t _pedestalB;
	uint16_t _iris;

	uint8_t _sensorGain;
	bool _colorBars;

	uint8_t _stateRequestPointer;
	uint32_t _lastStateRequest;

};

#endif

