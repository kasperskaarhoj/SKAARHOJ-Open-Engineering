/*
Copyright 2014 Kasper Skårhøj, SKAARHOJ, kasper@skaarhoj.com

This file is part of the Skaarhoj Serial Client Superclass library for Arduino

The SkaarhojUDPClient library is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by the
Free Software Foundation, either version 3 of the License, or (at your
option) any later version.

The SkaarhojUDPClient library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with the SkaarhojUDPClient library. If not, see http://www.gnu.org/licenses/.


IMPORTANT: If you want to use this library in your own projects and/or products,
please play a fair game and heed the license rules! See our web page for a Q&A so
you can keep a clear conscience: http://skaarhoj.com/about/licenses/

*/

#include "Arduino.h"
#include "SkaarhojUDPClient.h"

/**
 * Constructor
 */
SkaarhojUDPClient::SkaarhojUDPClient() {}

void SkaarhojUDPClient::begin(const IPAddress ip, const uint16_t localPort, const uint16_t remotePort){

		// Set up Udp communication object:
	#ifdef ESP8266
	WiFiUDP Udp;
	#else
	EthernetUDP Udp;
	#endif

	_Udp = Udp;
	
	_remoteIP = ip;			// Set switcher IP address
	_localPort = localPort;		// Set default local port
	_remotePort = remotePort;		// Set default remote port
}

/**
 * Initiating connection handshake to the server
 */
void SkaarhojUDPClient::connect() {

	if (_serialOutput)
    Serial.println(F("Connecting... (sending ping)"));

  _isConnected = false;
  _hasInitialized = false;
  _pendingAnswer = false;
  _pendingEOT = false;
  _resetDeviceStateVariables();
  
  _Udp.begin(_localPort);
  

  _sendPing(); // The way to connect. The response of a ping will set _isConnected and trigger asking for a status message. When processing the status message, _hasInitialized should be set in subclass

  _lastReconnectAttempt = millis();
}

/**
 * Reads information from the device as it arrives, dispatches packages to parsing
 */
void SkaarhojUDPClient::runLoop() { runLoop(0); }
void SkaarhojUDPClient::runLoop(uint16_t delayTime) {

  unsigned long enterTime = millis();

  do {
    _runSubLoop();

	uint16_t packetSize = _Udp.parsePacket();

    if (_Udp.available()) {
		
      _readFromClient();
    }

    // Requesting status of the device:
    if (_statusRequestInterval > 0 && !_pendingAnswer && hasTimedOut(_lastStatusRequest, _statusRequestInterval)) {
      if (_serialOutput > 1)
        Serial.println(F("Pulling status."));
      _lastStatusRequest = millis();

      _sendStatus();
    }

    // Send a ping / reconnect:
    if (_ackMsgInterval > 0 && !_pendingAnswer && hasTimedOut(_lastIncomingMsg, _ackMsgInterval)) {
      if (_serialOutput > 1)
        Serial.println(F("Sending Ping."));
      _lastReconnectAttempt = millis();
      _pendingAnswer = true;

      _sendPing();
    }

    // Trying to connect, if not:
    if (!isConnected()) {
      if (_reconnectInterval > 0 && hasTimedOut(_lastReconnectAttempt, _reconnectInterval)) {
        connect();
      }
    } else {
      if (_ackMsgInterval > 0 && hasTimedOut(_lastIncomingMsg, _ackMsgInterval + 2000)) {
        if (_serialOutput)
          Serial.println(F("Disconnected."));
        _isConnected = false;
      }
    }
  } while (delayTime > 0 && !hasTimedOut(enterTime, delayTime));
}
void SkaarhojUDPClient::_runSubLoop() { // For overloading
}

/**
 * Read from client buffer
 */
void SkaarhojUDPClient::_readFromClient() {
  while (_Udp.available()) {
	uint8_t _packetBuffer[2];
    char c;
	_Udp.read(_packetBuffer, 1);
	c = (char)_packetBuffer[0];

    if (c == _EOLChar) { // Line feed, always used
      _parselineDispatch();
      _resetLastIncomingMsg();
      _resetBuffer();
    } else if (c == _EOTChar) { // Prompt
      if (_serialOutput > 1)
        Serial.println(F("EOT received..."));
      _initialize();
      _resetLastIncomingMsg();
      _resetBuffer();
      _pendingEOT = false;
    } else if (c == 13 || c == 10) {                  // <CR> and <LF> ignored (they should be captured as _EOLChar if necessary)
                                                      // Ignore.
    } else if (_bufferWriteIndex < _bufferSize - 1) { // one byte for null termination reserved
      _buffer[_bufferWriteIndex] = c;
      _bufferWriteIndex++;
    } else {
      if (_serialOutput)
        Serial.println(F("ERROR: Buffer overflow."));
    }
  }
}

/**
 * Initialize by asking for status
 */
void SkaarhojUDPClient::_initialize() {
  if (!_isConnected) {
    _isConnected = true;
    if (_serialOutput)
      Serial.println(F("Connected"));

    _sendStatus();
  }
}

/**
 * Sends the buffer
 */
void SkaarhojUDPClient::_sendBuffer() {
	_Udp.beginPacket(_remoteIP,  _remotePort);
	_Udp.write(_buffer,_bufferWriteIndex);
	_Udp.endPacket();

  _resetBuffer();
  _pendingAnswer = true;
  _pendingEOT = true;
}
