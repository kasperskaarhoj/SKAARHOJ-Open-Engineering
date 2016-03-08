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
	
	_serialOutput = false;
	_activeHTTPRequest = false;
}

/**
 * Enable/Disable debug output to Serial:
 */ 
void ClientPanaAWHExTCP::serialOutput(bool flag){
	_serialOutput = flag;
}

void ClientPanaAWHExTCP::runLoop() {
  // if there's incoming data from the net connection.
  // send it out the serial port.  This is for debugging
  // purposes only:
  while(_client.available()) {
    char c = _client.read();
    //Serial.print(c);
  }	

  // If the server's disconnected, stop the client:
  if (!_client.connected() && _activeHTTPRequest) {
	_activeHTTPRequest = false;
	if (_serialOutput) {
		Serial.println(millis()-_activeHTTPRequestTime);
    	Serial.println();
    	Serial.println("disconnecting.");
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
		_sendPtzRequest("P"+String(panSpeed < 10?"0":"")+String(panSpeed,DEC));
		return true;
	}
	return false;
}
bool ClientPanaAWHExTCP::doTilt(uint8_t tiltSpeed) {	// 01-99, 50 is neutral
	if (isReady())	{
		_sendPtzRequest("T"+String(tiltSpeed < 10?"0":"")+String(tiltSpeed,DEC));
		return true;
	}
	return false;
}
bool ClientPanaAWHExTCP::doZoom(uint8_t zoomSpeed) {	// 01-99, 50 is neutral
	if (isReady())	{
		_sendPtzRequest("Z"+String(zoomSpeed < 10?"0":"")+String(zoomSpeed,DEC));
		return true;
	}
	return false;
}
bool ClientPanaAWHExTCP::doPanTilt(uint8_t panSpeed,uint8_t tiltSpeed) {	// 01-99, 50 is neutral
	if (isReady())	{
		_sendPtzRequest("PTS"+String(panSpeed < 10?"0":"")+String(panSpeed,DEC)+String(tiltSpeed < 10?"0":"")+String(tiltSpeed,DEC));
		return true;
	}
	return false;
}
bool ClientPanaAWHExTCP::setAutoFocus(bool enable) {
	if (isReady())	{
		_sendPtzRequest("D1"+String(enable?"1":"0"));
		return true;
	}
	return false;
}
bool ClientPanaAWHExTCP::doFocus(uint8_t focusPos) {	// 01-99, >50 = Far
	if (isReady())	{
		_sendPtzRequest("F"+String(focusPos < 10?"0":"")+String(focusPos,DEC));
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
		_sendPtzRequest("D3"+String(enable?"1":"0"));
		return true;
	}
	return false;
}
bool ClientPanaAWHExTCP::deletePreset(uint8_t presetNum) {	// 00-99
	if (isReady())	{
		_sendPtzRequest("C"+String(presetNum < 10?"0":"")+String(presetNum,DEC));
		return true;
	}
	return false;
}
bool ClientPanaAWHExTCP::storePreset(uint8_t presetNum) {	// 00-99
	if (isReady())	{
		_sendPtzRequest("M"+String(presetNum < 10?"0":"")+String(presetNum,DEC));
		return true;
	}
	return false;
}
bool ClientPanaAWHExTCP::recallPreset(uint8_t presetNum) {	// 00-99
	if (isReady())	{
		_sendPtzRequest("R"+String(presetNum < 10?"0":"")+String(presetNum,DEC));
		return true;
	}
	return false;
}
bool ClientPanaAWHExTCP::power(bool enable) {
	if (isReady())	{
		_sendPtzRequest("O"+String(enable?"1":"0"));
		return true;
	}
	return false;
}

bool ClientPanaAWHExTCP::isReady()	{
	return !_activeHTTPRequest;
}
void ClientPanaAWHExTCP::_sendPtzRequest(const String command) {
	_sendRequest(command, false);
}

void ClientPanaAWHExTCP::_sendCamRequest(const String command) {
	_sendRequest(command, true);
}

void ClientPanaAWHExTCP::_sendRequest(const String command, bool camRequest) {

  _activeHTTPRequestTime = millis();
  if (_client.connect(_cameraIP, 80)) {
    if (_serialOutput) Serial.println("connecting...");
    // send the HTTP PUT request:
	if (_serialOutput) Serial.println(command);

	uint8_t charIdx = 0;

	memset(_charBuf,0,96);

	if (command.length()<=34)	{
		if (camRequest)	{
			strcpy_P(_charBuf+charIdx, PSTR("GET /cgi-bin/aw_cam?cmd="));	// 24 chars
			charIdx+=strlen_P("GET /cgi-bin/aw_cam?cmd=");
		} else {
			strcpy_P(_charBuf+charIdx, PSTR("GET /cgi-bin/aw_ptz?cmd=%23"));	// 27 chars
			charIdx+=strlen_P("GET /cgi-bin/aw_ptz?cmd=%23");
		}

		command.toCharArray(_charBuf+charIdx, command.length()+1);	// total chars = 61 + NULL + length of command (max 34)
		charIdx+=command.length();
	
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
