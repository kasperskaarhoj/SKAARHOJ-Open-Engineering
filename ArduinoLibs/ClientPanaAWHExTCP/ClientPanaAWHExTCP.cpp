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

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif



#include "ClientPanaAWHExTCP.h"





/**
 * Constructor (using arguments is deprecated! Use begin() instead)
 */
ClientPanaAWHExTCP::ClientPanaAWHExTCP(){}
ClientPanaAWHExTCP::ClientPanaAWHExTCP(const IPAddress ip){
	begin(ip);
}

/**
 * Setting up IP address for the camera
 */
void ClientPanaAWHExTCP::begin(const IPAddress ip){
	_cameraIP = ip;		// Set camera IP address
	
	EthernetClient client;
	_client = client;	
	_lastSeen = 0;
	_lastPingAttempt = 0;
	
	_serialOutput = false;
	_activeHTTPRequest = false;
	_stateRequestPointer = 0;
	_lastStateRequest = 0;
	_queuePtr = 0;
	_lastSentCommand = 0;
}

/**
 * Enable/Disable debug output to Serial:
 */ 
void ClientPanaAWHExTCP::serialOutput(bool flag){
	_serialOutput = flag;
}

void ClientPanaAWHExTCP::_parseIncoming(char* buffer) {
	if(!strncmp(buffer, "ORI:", 4)) {
		uint16_t val = strtol(buffer+4, NULL, 16);
		_gainR = map(val, 0, 300, -100, 100);
	} else
	if(!strncmp(buffer, "OBI:", 4)) {
		uint16_t val = strtol(buffer+4, NULL, 16);
		_gainB = map(val, 0, 300, -100, 100);
	} else
	if(!strncmp(buffer, "ORP:", 4)) {
		uint16_t val = strtol(buffer+4, NULL, 16);
		_pedestalR = map(val, 0, 300, -100, 100);
	} else
	if(!strncmp(buffer, "OBP:", 4)) {
		uint16_t val = strtol(buffer+4, NULL, 16);
		_pedestalB = map(val, 0, 300, -100, 100);
	} else
	if(!strncmp(buffer, "OGU:", 4)) {
		uint16_t val = strtol(buffer+4, NULL, 16);
		if(val == 0x80) {
			_sensorGain = 0xFF;
		} else {
			_sensorGain = val - 0x08;
		}
	} else 
	if(!strncmp(buffer, "OBR:", 4) || !strncmp(buffer, "DCB:", 4)) {
		_colorBars = buffer[4] == '1';
	} else
	if(!strncmp(buffer, "ORV:", 4)) {
		_iris = strtol(buffer+4, NULL, 16);
	} else {
		if(_serialOutput > 1) {
			Serial << "Unhandled response: " << buffer << "\n";
		}
	}
}

void ClientPanaAWHExTCP::_requestState() {
	char* stateGetters[] = {
		"QRI", // Gain R
		"QBI", // Gain B
		"QRP", // Pedestal R
		"QBP", // Pedestal B
		"QGU", // Sensor Gain
		"QBR", // Color bars
		"QRV",  // Iris
	};

	if(millis() - _lastStateRequest > 500 && isReady()) {
		_stateRequestPointer++;
		_stateRequestPointer %= sizeof(stateGetters)/sizeof(char*);
		if(stateGetters[_stateRequestPointer][0] == '#') {
			_sendPtzRequest(stateGetters[_stateRequestPointer]);
		} else {
			_sendCamRequest(stateGetters[_stateRequestPointer]);
		}

		_lastStateRequest = millis();
	}
}

void ClientPanaAWHExTCP::runLoop() {
  char buffer[21];
  uint8_t pos = 0;
  memset(buffer, 0, 20);
  uint8_t NLc = 0;
  while(_client.available()) {
    char c = _client.read();
    if(c == '\n') { // Jump over the first 5 lines in the HTTP response
    	if(++NLc <= 5) continue;
    }

    if(pos < 20 && NLc >= 5) {
    	buffer[pos++] = c;
    }
  }	


  if(pos > 0) {
  	_lastSeen = millis();
  	_parseIncoming(buffer);
  }

  if(millis() - _lastSentCommand > 130 && _queuePtr > 0 && isReady()) {
  	_sendRequest(_queue, _getNextQueue(), true);
  	_popQueue();
  	_lastSentCommand = millis();
  }
  
  // if(millis() - _lastSeen > 500 && millis() - _lastPingAttempt > 500) {
  // 	if(_sendPing()) {
  // 		_lastSeen = millis();
  // 	}

  // 	_lastPingAttempt = millis();
  // }

	if(_queuePtr == 0) {
		// Request state from camera
		_requestState();
	}

  // If the server's disconnected, stop the client:
  if (!_client.connected() && _activeHTTPRequest) {
	_activeHTTPRequest = false;
	if (_serialOutput > 1) {
		Serial.println(millis()-_activeHTTPRequestTime);
	}
    _client.stop();
  }
}


/**
 * 
 */
void ClientPanaAWHExTCP::connect() {
}

void ClientPanaAWHExTCP::changeLastIPBytes(uint8_t lastByte) {
	_cameraIP[3] = lastByte;
}


bool ClientPanaAWHExTCP::doPan(uint8_t panSpeed) {	// 01-99, 50 is neutral
	if (isReady())	{
		_sendPtzRequest("P%02d", constrain(panSpeed,1,99));
		return true;
	}
	return false;
}
bool ClientPanaAWHExTCP::doTilt(uint8_t tiltSpeed) {	// 01-99, 50 is neutral
	if (isReady())	{
		_sendPtzRequest("T%02d", constrain(tiltSpeed, 1, 99));
		return true;
	}
	return false;
}
bool ClientPanaAWHExTCP::doZoom(uint8_t zoomSpeed) {	// 01-99, 50 is neutral
	if (isReady())	{
		_sendPtzRequest("Z%02d", constrain(zoomSpeed, 1, 99));
		return true;
	}
	return false;
}
bool ClientPanaAWHExTCP::doPanTilt(uint8_t panSpeed,uint8_t tiltSpeed) {	// 01-99, 50 is neutral
	if (isReady())	{
		_sendPtzRequest("PTS%02d%02d", constrain(panSpeed, 1, 99), constrain(tiltSpeed, 1, 99));
		return true;
	}
	return false;
}
bool ClientPanaAWHExTCP::setAutoFocus(bool enable) {
	if (isReady())	{
		_sendPtzRequest("D1%d", enable);
		return true;
	}
	return false;
}
bool ClientPanaAWHExTCP::doFocus(uint8_t focusPos) {	// 01-99, >50 = Far
	if (isReady())	{
		_sendPtzRequest("F%02d", constrain(focusPos, 1, 99));
		return true;
	}
	return false;
}
bool ClientPanaAWHExTCP::onTouchAutofocus() {
	if (isReady())	{
		_sendCamRequest("OSE:69:1");
		return true;
	}
	return false;
}
bool ClientPanaAWHExTCP::setAutoIris(bool enable) {
	if (isReady())	{
		_sendPtzRequest("D3%d", enable);
		return true;
	}
	return false;
}
bool ClientPanaAWHExTCP::deletePreset(uint8_t presetNum) {	// 00-99
	if (isReady())	{
		_sendPtzRequest("C%02d", constrain(presetNum, 0, 99));
		return true;
	}
	return false;
}
bool ClientPanaAWHExTCP::storePreset(uint8_t presetNum) {	// 00-99
	if (isReady())	{
		_sendPtzRequest("M%02d", constrain(presetNum, 0, 99));
		return true;
	}
	return false;
}
bool ClientPanaAWHExTCP::recallPreset(uint8_t presetNum) {	// 00-99
	if (isReady())	{
		_sendPtzRequest("R%02d", constrain(presetNum, 0, 99));
		return true;
	}
	return false;
}
bool ClientPanaAWHExTCP::power(bool enable) {
	if (isReady())	{
		_sendPtzRequest("O%d", enable);
		return true;
	}
	return false;
}

bool ClientPanaAWHExTCP::setContrast(uint8_t contrast) {
	if(isReady()) {
		_sendCamRequest("OSD:48:%02X", constrain(contrast, 0, 100));

		return true;
	}
	return false;
}

bool ClientPanaAWHExTCP::setColorBars(bool state) {
	if(isReady()) {
		_sendCamRequest("DCB:%d", state);
		return true;
	}
	return false;
}

bool ClientPanaAWHExTCP::setShutter(uint8_t shutter) {
	if(isReady()) {
		_sendCamRequest("OSH:%X", constrain(shutter, 0, 0xE));
		return true;
	}
	return false;
}

bool ClientPanaAWHExTCP::setSensorGain(uint8_t gain) {
	if(isReady()) {
		if(gain <= 18) {
			gain = 0x08 + gain;
		} else { // Enforce auto gain
			gain = 0x80;
		}
		_sendCamRequest("OGU:%02X", gain);
		return true;
	}
	return false;
}

bool ClientPanaAWHExTCP::setGainR(int8_t gain) {
	if(isReady()) {
		gain = constrain(gain, -100, 100);
		uint16_t g = map((int16_t)gain, -100, 100, 0, 0x12C);
		_sendCamRequest("ORI:%03X", g);
		_gainR = gain;
		return true;
	}	
	return false;
}

bool ClientPanaAWHExTCP::setGainB(int8_t gain) {
	if(isReady()) {
		gain = constrain(gain, -100, 100);
		uint16_t g = map((int16_t)gain, -100, 100, 0, 0x12C);
		_sendCamRequest("OBI:%03X", g);
		_gainB = gain;
		return true;
	}	
	return false;
}

bool ClientPanaAWHExTCP::setPedestalR(int8_t pedestal) {
	if(isReady()) {
		pedestal = constrain(pedestal, -100, 100);
		uint16_t g = map((int16_t)pedestal, -100, 100, 0, 0x12C);
		_sendCamRequest("ORP:%03X", g);
		_pedestalR = pedestal;
		return true;
	}	
	return false;
}

bool ClientPanaAWHExTCP::setPedestalB(int8_t pedestal) {
	if(isReady()) {
		pedestal = constrain(pedestal, -100, 100);
		uint16_t g = map((int16_t)pedestal, -100, 100, 0, 0x12C);
		_sendCamRequest("OBP:%03X", g);
		_pedestalB = pedestal;
		return true;
	}	
	return false;
}

bool ClientPanaAWHExTCP::setIris(uint16_t iris) {
	if(isReady()) {
		_sendCamRequest("ORV:%03X", iris);
		_iris = iris;
		return true;
	}
	return false;
}

int8_t ClientPanaAWHExTCP::getGainR() {
	return _gainR;
}
int8_t ClientPanaAWHExTCP::getGainB() {
	return _gainB;
}
int8_t ClientPanaAWHExTCP::getPedestalR() {
	return _pedestalR;
}
int8_t ClientPanaAWHExTCP::getPedestalB() {
	return _pedestalB;
}
uint16_t ClientPanaAWHExTCP::getIris() {
	return _iris;
}

bool ClientPanaAWHExTCP::isReady()	{
	return !_activeHTTPRequest;
}
void ClientPanaAWHExTCP::_sendPtzRequest(const char* format, ...) {
	va_list args;
	va_start(args, format);

	vsnprintf(_cmdBuf, PanaAWHE_BUFSIZE, format, args);
	_sendRequest(_cmdBuf, strlen(_cmdBuf), false);

	va_end(args);
}

void ClientPanaAWHExTCP::_sendCamRequest(const char* format, ...) {
	va_list args;
	va_start(args, format);

	vsnprintf(_cmdBuf, PanaAWHE_BUFSIZE, format, args);
	Serial << "Request: " << _cmdBuf << "\n";

	_addToQueue(_cmdBuf, 1, true);
	va_end(args);
}

bool ClientPanaAWHExTCP::_sendPing() {
	_activeHTTPRequestTime = millis();
	if(!_activeHTTPRequest) {
		if(_client.connect(_cameraIP, 80)) {
			_client.stop();
			return true;
			_client.stop();
		}
	}
	return false;
}

bool ClientPanaAWHExTCP::isConnected() {
	return millis() - _lastSeen <= 5000;
}

void ClientPanaAWHExTCP::_addToQueue(const char* cmd, uint8_t cam, bool camRequest) {
	if(_queuePtr + strlen(cmd) + 1 >= PanaAWHE_QUEUESIZE) {
		Serial << "No more space in QUEUE, discarding cmd " << cmd << "\n";
	} else {
		memcpy(_queue + _queuePtr, cmd, strlen(cmd));
		_queue[_queuePtr + strlen(cmd)] = 0x09;
		_queuePtr += strlen(cmd) + 1;
	}
}

uint8_t ClientPanaAWHExTCP::_getNextQueue() {
	for(uint8_t i = 0; i < PanaAWHE_QUEUESIZE; i++) {
		if(_queue[i] == 0x09) {
			return i;
		}
	}
	return 0;
}

void ClientPanaAWHExTCP::_popQueue() {
	Serial << "Queue: " << _queue << "\n";
	for(uint8_t i = 0; i < PanaAWHE_QUEUESIZE; i++) {
		if(_queue[i] == 0x09) {
			memmove(_queue, _queue+i+1, PanaAWHE_QUEUESIZE-i-1);
			_queuePtr -= i+1;
			Serial << "Queue pointer: " << _queuePtr << "\n";
			break;
		}
	}
}

void ClientPanaAWHExTCP::_sendRequest(const char* command, uint8_t len, bool camRequest) {

  _activeHTTPRequestTime = millis();
  if (_client.connect(_cameraIP, 80)) {
    if (_serialOutput > 1) Serial.println("connecting...");
    // send the HTTP PUT request:
	//if (_serialOutput) Serial.println(command);

	uint8_t charIdx = 0;

	memset(_charBuf,0,96);

	if (len<=34)	{
		if (camRequest)	{
			strcpy_P(_charBuf+charIdx, PSTR("GET /cgi-bin/aw_cam?cmd="));	// 24 chars
			charIdx+=strlen_P("GET /cgi-bin/aw_cam?cmd=");
		} else {
			strcpy_P(_charBuf+charIdx, PSTR("GET /cgi-bin/aw_ptz?cmd=%23"));	// 27 chars
			charIdx+=strlen_P("GET /cgi-bin/aw_ptz?cmd=%23");
		}

		memcpy(_charBuf+charIdx, command, len);
		charIdx+=len;
	
		strcpy_P(_charBuf+charIdx, PSTR("&res=1 HTTP/1.1\r\n"));	// 17 chars
		charIdx+=strlen_P("&res=1 HTTP/1.1\r\n");

		strcpy_P(_charBuf+charIdx, PSTR("Host: 1.2.3.4\r\n"));	// 15 chars
		charIdx+=strlen_P("Host: 1.2.3.4\r\n");

		strcpy_P(_charBuf+charIdx, PSTR("\r\n"));	// 2 chars
		charIdx+=strlen_P("\r\n");
		/*
		if (_serialOutput) Serial.println("*");
		if (_serialOutput) Serial.println(_charBuf);
		if (_serialOutput) Serial.println("*");
		*/
		_client.print(_charBuf);

		_activeHTTPRequest = true;
	} else {
	    if (_serialOutput) Serial.println("Command too long (>34 chars)");
	}
  } 
  else {
    // if you couldn't make a connection:
    if (_serialOutput) Serial.println("connection failed");
    if (_serialOutput) Serial.println("disconnecting.");
    _client.stop();
	_activeHTTPRequest = false;
  }

}
