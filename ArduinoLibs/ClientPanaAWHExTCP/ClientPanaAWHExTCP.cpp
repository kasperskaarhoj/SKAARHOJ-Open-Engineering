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

  _activeHTTPRequestTime = millis();
  if (_client.connect(_cameraIP, 80)) {
    if (_serialOutput) Serial.println("connecting...");
    // send the HTTP PUT request:
	if (_serialOutput) Serial.println(command);

	String inputString = String("GET /cgi-bin/aw_ptz?cmd=%23") +
						String(command) +
						String("&res=1 HTTP/1.1\r\n") + 
						String("Host: 192.168.10.25\r\n") +
						String("\r\n");
	char charBuf[128];
	inputString.toCharArray(charBuf, 128);
	_client.print(charBuf);

	_activeHTTPRequest = true;
  } 
  else {
    // if you couldn't make a connection:
    if (_serialOutput) Serial.println("connection failed");
    if (_serialOutput) Serial.println("disconnecting.");
    _client.stop();
	_activeHTTPRequest = false;
  }

}

void ClientPanaAWHExTCP::_sendCamRequest(const String command) {

  _activeHTTPRequestTime = millis();
  if (_client.connect(_cameraIP, 80)) {
    if (_serialOutput) Serial.println("connecting...");
    // send the HTTP PUT request:
	if (_serialOutput) Serial.println(command);

	String inputString = String("GET /cgi-bin/aw_cam?cmd=") +
						String(command) +
						String("&res=1 HTTP/1.1\r\n") + 
						String("Host: 192.168.10.25\r\n") +
						String("\r\n");
	char charBuf[128];
	inputString.toCharArray(charBuf, 128);
	_client.print(charBuf);

	_activeHTTPRequest = true;
  } 
  else {
    // if you couldn't make a connection:
    if (_serialOutput) Serial.println("connection failed");
    if (_serialOutput) Serial.println("disconnecting.");
    _client.stop();
	_activeHTTPRequest = false;
  }

}
