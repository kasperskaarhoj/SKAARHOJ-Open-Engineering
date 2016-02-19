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
	
	char _charBuf[96];
	
  public:

    ClientPanaAWHExTCP();
    ClientPanaAWHExTCP(const IPAddress ip);
    void begin(const IPAddress ip);
	void serialOutput(bool flag);

	void runLoop();
    void connect();
	void changeLastIPBytes(uint8_t lastByte);
	bool isReady();
	
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
	
  private:
	void _sendPtzRequest(const String command);
	void _sendCamRequest(const String command);
	void _sendRequest(const String command, bool camRequest);

};

#endif

