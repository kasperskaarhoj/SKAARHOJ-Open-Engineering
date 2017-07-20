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
	_baseAddr = ip[3];
	
	EthernetClient client;
	_client = client;
	_lastPingAttempt = 0;
	
	_serialOutput = 0;
	_activeHTTPRequest = false;
	_lastStateRequest = 0;
	_queuePtr = 0;
	_lastStateCam = 0;
	
	memset(_stateRequestPointer, 0, PanaAWHE_NUMCAMS);
	memset(_lastSeen, 0, PanaAWHE_NUMCAMS*4);
	memset(_lastSentCommand, 0, PanaAWHE_NUMCAMS*4); // 32 bit integer
	memset(_presetState, 0, PanaAWHE_NUMCAMS*4);
	memset(_isOnline, 0, PanaAWHE_NUMCAMS);
	memset(_presetSpeed, 0, PanaAWHE_NUMCAMS);
	memset(_autoIris, 0, PanaAWHE_NUMCAMS);
	memset(_shutter, 0, PanaAWHE_NUMCAMS);
}

/**
 * Enable/Disable debug output to Serial:
 */ 
void ClientPanaAWHExTCP::serialOutput(bool flag){
	_serialOutput = flag;
}

void ClientPanaAWHExTCP::_parseIncoming(char* buffer) {
	uint8_t cam = _cameraIP[3] - _baseAddr;
	if(cam >= PanaAWHE_NUMCAMS) return;

	if(!strncmp(buffer, "ORI:", 4)) {
		uint16_t val = strtol(buffer+4, NULL, 16);
		_gainR[cam] = map(val, 0, 300, -100, 100);
	} else
	if(!strncmp(buffer, "OBI:", 4)) {
		uint16_t val = strtol(buffer+4, NULL, 16);
		_gainB[cam] = map(val, 0, 300, -100, 100);
	} else
	if(!strncmp(buffer, "ORP:", 4)) {
		uint16_t val = strtol(buffer+4, NULL, 16);
		_pedestalR[cam] = map(val, 0, 300, -100, 100);
	} else
	if(!strncmp(buffer, "OBP:", 4)) {
		uint16_t val = strtol(buffer+4, NULL, 16);
		_pedestalB[cam] = map(val, 0, 300, -100, 100);
	} else
	if(!strncmp(buffer, "OGU:", 4)) {
		uint16_t val = strtol(buffer+4, NULL, 16);
		if(val == 0x80) {
			_sensorGain[cam] = 0xFF;
		} else {
			_sensorGain[cam] = val - 0x08;
		}
	} else 
	if(!strncmp(buffer, "OBR:", 4) || !strncmp(buffer, "DCB:", 4)) {
		_colorBars[cam] = buffer[4] == '1';
	} else
	if(!strncmp(buffer, "gi", 2)) {
		_autoIris[cam] = buffer[5] - '0';
		buffer[5] = 0; // Null terminate for strtol
		_iris[cam] = map(strtol(buffer+2, NULL, 16), 0x555, 0xFFF, 0, 0x3FF);
	} else
	if(!strncmp(buffer, "axi", 3)) {
		_iris[cam] = map(strtol(buffer+3, NULL, 16), 0x555, 0xFFF, 0, 0x3FF);
	} else
	if(!strncmp(buffer, "pE00", 4)) {
		_presetState[cam] = strtoul(buffer+4+2, NULL, 16);
	} else
	if(!strncmp(buffer, "uPVS", 4)) {
		_presetSpeed[cam] = map(constrain(strtoul(buffer+4, NULL, 10), 250, 999), 250, 999, 1, 30);
	} else 
	if(!strncmp(buffer, "OSH:", 4)) {
		_shutter[cam] = strtoul(buffer+4, NULL, 16);
	} else {
		if(_serialOutput > 1) {
			Serial << "Unhandled response: " << buffer << "\n";
		}
	}
}

bool ClientPanaAWHExTCP::_requestState(uint8_t cam) {
	if(cam >= PanaAWHE_NUMCAMS) return false;

	char* stateGetters[] = {
		"QRI", // Gain R
		"QBI", // Gain B
		"QRP", // Pedestal R
		"QBP", // Pedestal B
		"QGU", // Sensor Gain
		"QBR", // Color bars
		"QSH", // Shutter
		"#GI",  // Iris
		"#PE00", // Presets 01-40
		"#UPVS", // Preset recall speed
	};

	if(millis() - _lastStateRequest > 100 && isReady()) {
		_stateRequestPointer[cam]++;
		_stateRequestPointer[cam] %= sizeof(stateGetters)/sizeof(char*);
		if(stateGetters[_stateRequestPointer[cam]][0] == '#') {
			_sendPtzRequest(cam, 3, stateGetters[_stateRequestPointer[cam]]+1);
		} else {
			_sendCamRequest(cam, 3, stateGetters[_stateRequestPointer[cam]]);
		}

		_lastStateRequest = millis();
		return true;
	}
	return false;
}

void ClientPanaAWHExTCP::runLoop() {
	char buffer[21];
	uint8_t pos = 0;
	memset(buffer, 0, 20);
	uint8_t NLc = 0;

	uint8_t cam = _cameraIP[3] - _baseAddr;
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
		_lastSeen[cam] = millis();
		_parseIncoming(buffer);
	}

	if(isReady()) {
		if(_queuePtr > 0) {
			uint8_t startPos = 0;
			uint8_t nextPos = 0;
			while((nextPos = _getNextQueue(startPos)) != 0) {
				uint8_t cam = _queue[startPos] - '0';
				if(millis() - _lastSentCommand[cam] > 130) {
			  		_sendRequest(cam, _queue+startPos+2, nextPos-startPos-3, _queue[startPos+1] - '0');
			  		_lastSentCommand[cam] = millis();
				  	_popQueue(startPos);
				  	break;
		  		}
		  		startPos = nextPos;
			}
		} else if(millis() - _lastStateRequest > 130) { // _queuePtr == 0
			// Request state from camera
			uint8_t nextCam = (_lastStateCam+1)%PanaAWHE_NUMCAMS;
			if(_isOnline[nextCam] || millis() - _lastSentCommand[nextCam] > 5000) { // Shouldn't take too long time for offline cameras
				if(_requestState(nextCam)) {
					_lastSentCommand[nextCam] = millis();
				}
			}
			_lastStateCam = nextCam;
		}
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
	// if(_sendPing()) {
	// 	if(_serialOutput > 0) {
	// 		Serial << "Connected to PanaAWHE camera\n";
	// 	}
	// 	_lastSeen = millis();
	// } else {
	// 	if(_serialOutput > 0) {
	// 		Serial << "Connection to PanaAWHE camera failed\n";
	// 	}
	// }
}

void ClientPanaAWHExTCP::changeLastIPBytes(uint8_t lastByte) {
	_cameraIP[3] = lastByte;
}


bool ClientPanaAWHExTCP::doPan(uint8_t cam, uint8_t panSpeed) {	// 01-99, 50 is neutral
	_sendPtzRequest(cam, 1, "P%02d", constrain(panSpeed,1,99));
	return true;
}
bool ClientPanaAWHExTCP::doTilt(uint8_t cam, uint8_t tiltSpeed) {	// 01-99, 50 is neutral
	_sendPtzRequest(cam, 1, "T%02d", constrain(tiltSpeed, 1, 99));
	return true;
}
bool ClientPanaAWHExTCP::doZoom(uint8_t cam, uint8_t zoomSpeed) {	// 01-99, 50 is neutral
	_sendPtzRequest(cam, 1, "Z%02d", constrain(zoomSpeed, 1, 99));
	return true;
}
bool ClientPanaAWHExTCP::doPanTilt(uint8_t cam, uint8_t panSpeed,uint8_t tiltSpeed) {	// 01-99, 50 is neutral
	_sendPtzRequest(cam, 3, "PTS%02d%02d", constrain(panSpeed, 1, 99), constrain(tiltSpeed, 1, 99));
	return true;
}
bool ClientPanaAWHExTCP::setAutoFocus(uint8_t cam, bool enable) {
	_sendPtzRequest(cam, 1, "D1%d", enable);
	return true;
}
bool ClientPanaAWHExTCP::doFocus(uint8_t cam, uint8_t focusPos) {	// 01-99, >50 = Far
	_sendPtzRequest(cam, 1, "F%02d", constrain(focusPos, 1, 99));
	return true;
}
bool ClientPanaAWHExTCP::onTouchAutofocus(uint8_t cam) {
	_sendCamRequest(cam, 4, "OSE:69:1");
	return true;
}
bool ClientPanaAWHExTCP::setAutoIris(uint8_t cam, bool enable) {
	if(cam < PanaAWHE_NUMCAMS) {
		_sendPtzRequest(cam, 1, "D3%d", enable);
		_autoIris[cam] = enable;
		return true;
	}
	return false;
}
bool ClientPanaAWHExTCP::deletePreset(uint8_t cam, uint8_t presetNum) {	// 00-99
	_sendPtzRequest(cam, 3, "C%02d", constrain(presetNum, 0, 99));
	if(cam < PanaAWHE_NUMCAMS && presetNum < 32) {
		_presetState[cam] &= ~(1UL << presetNum);
	}
	return true;
}
bool ClientPanaAWHExTCP::storePreset(uint8_t cam, uint8_t presetNum) {	// 00-99
	_sendPtzRequest(cam, 3, "M%02d", constrain(presetNum, 0, 99));
	if(cam < PanaAWHE_NUMCAMS && presetNum < 32) {
		_presetState[cam] |= 1UL << presetNum;
	}
	return true;
}
bool ClientPanaAWHExTCP::recallPreset(uint8_t cam, uint8_t presetNum) {	// 00-99
	_sendPtzRequest(cam, 1, "R%02d", constrain(presetNum, 0, 99));
	return true;
}
bool ClientPanaAWHExTCP::power(uint8_t cam, bool enable) {
	if (cam < PanaAWHE_NUMCAMS)	{
		_sendPtzRequest(cam, 1, "O%d", enable);
		return true;
	}
	return false;
}

bool ClientPanaAWHExTCP::setContrast(uint8_t cam, uint8_t contrast) {
	if(cam < PanaAWHE_NUMCAMS) {
		_sendCamRequest(cam, 7, "OSD:48:%02X", constrain(contrast, 0, 100));
		return true;
	}
	return false;
}

bool ClientPanaAWHExTCP::setColorBars(uint8_t cam, bool state) {
	if(cam < PanaAWHE_NUMCAMS) {
		_colorBars[cam] = state;
		_sendCamRequest(cam, 4, "DCB:%d", state);
		return true;
	}
	return false;
}

bool ClientPanaAWHExTCP::setShutter(uint8_t cam, uint8_t shutter) {
	if(cam < PanaAWHE_NUMCAMS) {
		_sendCamRequest(cam, 4, "OSH:%X", constrain(shutter, 0, 0xE));
		_shutter[cam] = shutter;
		return true;
	}
	return false;
}

bool ClientPanaAWHExTCP::setSensorGain(uint8_t cam, uint8_t gain) {
	if(cam < PanaAWHE_NUMCAMS) {
		if(gain <= 18) {
			gain = 0x08 + gain;
		} else { // Enforce auto gain
			gain = 0x80;
		}
		_sendCamRequest(cam, 4, "OGU:%02X", gain);
		_sensorGain[cam] = gain <= 0x80 ? gain-8 : 0xFF;
		return true;
	}
	return false;
}

bool ClientPanaAWHExTCP::setGainR(uint8_t cam, int8_t gain) {
	if(cam < PanaAWHE_NUMCAMS) {
		gain = constrain(gain, -100, 100);
		uint16_t g = map((int16_t)gain, -100, 100, 0, 0x12C);
		_sendCamRequest(cam, 4, "ORI:%03X", g);
		_gainR[cam] = gain;
		return true;
	}	
	return false;
}

bool ClientPanaAWHExTCP::setGainB(uint8_t cam, int8_t gain) {
	if(cam < PanaAWHE_NUMCAMS) {
		gain = constrain(gain, -100, 100);
		uint16_t g = map((int16_t)gain, -100, 100, 0, 0x12C);
		_sendCamRequest(cam, 4, "OBI:%03X", g);
		_gainB[cam] = gain;
		return true;
	}	
	return false;
}

bool ClientPanaAWHExTCP::setPedestalR(uint8_t cam, int8_t pedestal) {
	if(cam < PanaAWHE_NUMCAMS) {
		pedestal = constrain(pedestal, -100, 100);
		uint16_t g = map((int16_t)pedestal, -100, 100, 0, 0x12C);
		_sendCamRequest(cam, 4, "ORP:%03X", g);
		_pedestalR[cam] = pedestal;
		return true;
	}	
	return false;
}

bool ClientPanaAWHExTCP::setPedestalB(uint8_t cam, int8_t pedestal) {
	if(cam < PanaAWHE_NUMCAMS) {
		pedestal = constrain(pedestal, -100, 100);
		uint16_t g = map((int16_t)pedestal, -100, 100, 0, 0x12C);
		_sendCamRequest(cam, 4, "OBP:%03X", g);
		_pedestalB[cam] = pedestal;
		return true;
	}	
	return false;
}

bool ClientPanaAWHExTCP::setIris(uint8_t cam, uint16_t iris) {
	if(cam < PanaAWHE_NUMCAMS && !_autoIris[cam]) {
		_sendPtzRequest(cam, 3, "AXI%03X", map(iris, 0, 1023, 0x555, 0xFFF));
		_iris[cam] = iris;
		return true;
	}
	return false;
}

bool ClientPanaAWHExTCP::setPresetSpeed(uint8_t cam, uint8_t speed) {
	if(cam < PanaAWHE_NUMCAMS) {

		uint16_t value = constrain(250 + constrain(speed, 1, 30) * 25, 275, 999);
		_sendPtzRequest(cam, 4, "UPVS%03d", value);
		_presetSpeed[cam] = speed;
	}
}

int8_t ClientPanaAWHExTCP::getGainR(uint8_t cam) {
	return _gainR[cam];
}
int8_t ClientPanaAWHExTCP::getGainB(uint8_t cam) {
	return _gainB[cam];
}
int8_t ClientPanaAWHExTCP::getPedestalR(uint8_t cam) {
	return _pedestalR[cam];
}
int8_t ClientPanaAWHExTCP::getPedestalB(uint8_t cam) {
	return _pedestalB[cam];
}
uint16_t ClientPanaAWHExTCP::getIris(uint8_t cam) {
	return _iris[cam];
}
bool ClientPanaAWHExTCP::getAutoIris(uint8_t cam) {
	return _autoIris[cam];
}
uint8_t ClientPanaAWHExTCP::getPresetSpeed(uint8_t cam) {
	return _presetSpeed[cam];
}
uint8_t ClientPanaAWHExTCP::getShutter(uint8_t cam) {
	return _shutter[cam];
}
uint8_t ClientPanaAWHExTCP::getSensorGain(uint8_t cam) {
	return _sensorGain[cam];
}

bool ClientPanaAWHExTCP::presetExists(uint8_t cam, uint8_t preset) {
	return _presetState[cam] & 1UL << preset; 
}

bool ClientPanaAWHExTCP::isReady()	{
	// This _may_ need to be increased if the response time is greater than 200 ms
	if(_activeHTTPRequest && millis() - _activeHTTPRequestTime > 200) {
		_client.stop();
		_activeHTTPRequest = false;
	}
	return !_activeHTTPRequest;
}
void ClientPanaAWHExTCP::_sendPtzRequest(uint8_t cam, uint8_t cmdlen, const char* format, ...) {
	va_list args;
	va_start(args, format);

	vsnprintf(_cmdBuf, PanaAWHE_BUFSIZE, format, args);
	_addToQueue(cam, cmdlen, _cmdBuf, false);
	//Serial << "PTZ Request: " << _cmdBuf << "\n";

	va_end(args);
}

void ClientPanaAWHExTCP::_sendCamRequest(uint8_t cam, uint8_t cmdlen, const char* format, ...) {
	va_list args;
	va_start(args, format);

	vsnprintf(_cmdBuf, PanaAWHE_BUFSIZE, format, args);
	//Serial << "CAM " << cam << " Request: " << _cmdBuf << "\n";

	_addToQueue(cam, cmdlen, _cmdBuf, true);
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

bool ClientPanaAWHExTCP::isConnected(uint8_t cam) {
	if(cam < PanaAWHE_NUMCAMS) {
		//return millis() - _lastSeen[cam] <= 5000;
		return _isOnline[cam];
	}
	return false;
}

void ClientPanaAWHExTCP::_addToQueue(uint8_t cam, uint8_t cmdlen, const char* cmd, bool camRequest) {
	if(_queuePtr + strlen(cmd) + 3 >= PanaAWHE_QUEUESIZE) {
		Serial << "No more space in QUEUE, discarding cmd " << cmd << "\n";
	} else {
		if(_queuePtr > 0) {
			for(uint8_t i = 0; i < PanaAWHE_QUEUESIZE-3; i++) {
				if(_queue[i] == 0x09 || i == 0) {
					if(i > 0) i++;
					if(_queue[i] == '0'+cam && _queue[i+1] == '0'+camRequest) {
						if(!strncmp(_queue+i+2, cmd, cmdlen) && _queue[i+2+strlen(cmd)] == 0x09) {
							memcpy(_queue+i+2, cmd, strlen(cmd));
							Serial << "Replaced command in queue\n";
							return;
						}
					}
					i += 2;
				}
			}
		}

		_queue[_queuePtr] = '0' + cam;
		_queue[_queuePtr + 1] = '0' + camRequest;
		memcpy(_queue + _queuePtr + 2, cmd, strlen(cmd));
		_queue[_queuePtr + strlen(cmd) + 2] = 0x09;
		_queuePtr += strlen(cmd) + 3;
	}
}

uint8_t ClientPanaAWHExTCP::_getNextQueue(uint8_t start) {
	for(uint8_t i = start; i < PanaAWHE_QUEUESIZE-1; i++) {
		if(_queue[i] == 0x09) {
			return i+1;
		}
	}
	return 0;
}

void ClientPanaAWHExTCP::_popQueue(uint8_t startPos) {
	//Serial << "Queue: " << _queue << " (StartPos: " << startPos << ")\n";
	for(uint8_t i = startPos; i < PanaAWHE_QUEUESIZE; i++) {
		if(_queue[i] == 0x09) {
			memmove(_queue+startPos, _queue+i+1, PanaAWHE_QUEUESIZE-i-1-startPos);
			_queuePtr -= i+1-startPos;
			//Serial << "Queue pointer: " << _queuePtr << "\n";
			break;
		}
	}
}

void ClientPanaAWHExTCP::_sendRequest(uint8_t cam, const char* command, uint8_t len, bool camRequest) {
	if(cam >= PanaAWHE_NUMCAMS) return;
	_activeHTTPRequestTime = millis();
	_cameraIP[3] = _baseAddr + cam;

	uint16_t timeout = W5100.readRTR();
	uint8_t retry = W5100.readRCR();

	W5100.setRetransmissionTime(500); // 50 ms
	W5100.setRetransmissionCount(1);
	if(_isOnline[cam] = _client.connect(_cameraIP, 80)) {
		if (_serialOutput > 1) Serial.println("connecting...");
		// send the HTTP PUT request:
		//if (_serialOutput) Serial.println(command);

		uint8_t charIdx = 0;

		memset(_charBuf,0,96);

		if (len<=34) {
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
			
			//if (_serialOutput) Serial.println("*");
			//if (_serialOutput) Serial.println(_charBuf);
			//if (_serialOutput) Serial.println("*");
			
			_client.print(_charBuf);

			_activeHTTPRequest = true;
		} else {
		    if (_serialOutput) Serial.println("Command too long (>34 chars)");
		}
	} else {
		// if you couldn't make a connection:
		if (_serialOutput > 1) Serial.println("connection failed");
		_client.stop();
		_activeHTTPRequest = false;
	}

	W5100.setRetransmissionTime(timeout);
	W5100.setRetransmissionCount(retry);
}
