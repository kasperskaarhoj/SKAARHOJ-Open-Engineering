/*
Copyright 2014 Kasper Skårhøj, SKAARHOJ, kasper@skaarhoj.com

This file is part of the Skaarhoj TCP Client Superclass library for Arduino

The SkaarhojTCPClient library is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by the
Free Software Foundation, either version 3 of the License, or (at your
option) any later version.

The SkaarhojTCPClient library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with the SkaarhojTCPClient library. If not, see http://www.gnu.org/licenses/.


IMPORTANT: If you want to use this library in your own projects and/or products,
please play a fair game and heed the license rules! See our web page for a Q&A so
you can keep a clear conscience: http://skaarhoj.com/about/licenses/

*/

#include "Arduino.h"
#include "SkaarhojTCPClient.h"
#include "Streaming.h"
#include "SkaarhojTools.h"

/**
 * Constructor (using arguments is deprecated! Use begin() instead)
 */
SkaarhojTCPClient::SkaarhojTCPClient() { _EOLChar = 10; }

/**
 * Setting up IP address for the videohub (and local port to open telnet connection to)
 */
void SkaarhojTCPClient::begin(IPAddress ip) { begin(ip, 23); }
void SkaarhojTCPClient::begin(IPAddress ip, uint16_t port) {

  // Initialize the Ethernet client library
  EthernetClient client;
  _client = client;

  _IP = ip;          // Set server IP address
  _localPort = port; // Set local port

  _ackMsgResponseTimeout = 3000;
  _exitRunLoop = false;

  TCPReadBuffer_len = 0;
  TCPReadBuffer_ptr = 0;

  _EOLTimeOutTime = 0;
  _EOLtimer = millis();

  _resetDeviceStateVariables();
}

/**
 * Initiating connection handshake to the server
 */
void SkaarhojTCPClient::connect() {
  _pendingAnswer = false;
  _pendingEOT = false;
  _hasInitialized = false;
  _section = 0;
  _resetDeviceStateVariables();

  if (_serialOutput)
    Serial.print(F("Connecting to "));
  if (_serialOutput)
    Serial.print(_IP);

  if (_client.connect(_IP, _localPort)) {
    if (_serialOutput)
      Serial.println(F(" - connected"));
    _isConnected = true;
    _lastIncomingMsg = millis();
    _lastStatusRequest = millis();
    initializeConnection();
  } else {
    if (_serialOutput)
      Serial.println(F(" - connection failed"));
    _isConnected = false;
  }

  _lastReconnectAttempt = millis();
}

/**
 * Disconnecting nicely from server if possible.
 * This depends on whether the server has a command to terminate the connection, the default here is to just stop the ethernet client.
 */
void SkaarhojTCPClient::disconnect() {
  if (_serialOutput)
    Serial.println(F("Disconnecting..."));

  stopClient();

  _lastReconnectAttempt = millis();
}

/**
 * Has initialized?
 */
bool SkaarhojTCPClient::hasInitialized() { return _hasInitialized; }

/**
 * Stopping the ethernet client on the arduino
 */
void SkaarhojTCPClient::stopClient() {
  if (_serialOutput)
    Serial.println(F("Stopping client."));

  _client.stop();

  _lastReconnectAttempt = millis();
  _isConnected = false;
  _hasInitialized = false;
  _pendingAnswer = false;
  _pendingEOT = false;
  _section = 0;

  _resetDeviceStateVariables();
}

/**
 * Initialize Connection
 */
void SkaarhojTCPClient::initializeConnection() {
}

/**
 * Reads information from the server as it arrives, dispatches lines to parsing
 */
void SkaarhojTCPClient::runLoop() { runLoop(0); }
void SkaarhojTCPClient::runLoop(uint16_t delayTime) {

  unsigned long enterTime = millis();

  do {
    // if there are incoming bytes available
    // from the server, read them and process them:
    if (_client.connected()) {

      _runSubLoop();

      // An assumption here is, that the WHOLE line arrives and is processed immediately by _parselineDispatch() whereafter the buffer is emptied. If say the line data arrives in two turns, in the meantime we risk, that the buffer memory was used to send a command. I guess the issue is more theoretical than practical, but it's good to keep in mind that the same memory area is used for read and write and there is no check in the write-routines if the buffer is currently being filled with incoming data.
      if (_client.available()) {
        _readFromClient();
        if (_exitRunLoop) {
          _exitRunLoop = false;
          _sendBusy();
          return;
        }
      }

      if (_EOLTimeOut())  {
        //Serial << "_EOLTimeOut...\n";
        _parselineDispatch();
        _resetLastIncomingMsg();
        _resetBuffer();
      }


      // Requesting status of the machine:
      if (_statusRequestInterval > 0 && hasTimedOut(_lastStatusRequest, _statusRequestInterval) && hasInitialized()) {
        if (_serialOutput > 1)
          Serial.println(F("Pulling status."));
        _lastStatusRequest = millis();

        _sendStatus();
      }

      // Send a ping:
      if (_ackMsgInterval > 0 && hasTimedOut(_lastIncomingMsg, _ackMsgInterval + (_pendingAnswer ? _ackMsgResponseTimeout : 0))) { // exclude "&& hasInitialized()" because otherwise it can get stuck in the moment between connection and reception of the initial payload. If so, we will not catch if being disconnected!
        if (_pendingAnswer) {
          disconnect();
        } else {
          if (_serialOutput > 1)
            Serial.println(F("Sending Ping."));
          _pendingAnswer = true;

          _sendPing();
        }
      }

      _sendReady();
    }

    // If the server's disconnected, stop the client:
    // Actually, _client.connected() returns true EVEN if you remove the cable!
    // For _client.connected() to report false, an attempt to communicate with the device has to be made - hence the "pings" above.
    if (!_client.connected()) {
      if (_isConnected) {
        if (_serialOutput)
          Serial.println(F("ERROR: Client not connected anymore."));
        stopClient();
      } else {
        if (_reconnectInterval > 0 && hasTimedOut(_lastReconnectAttempt, _reconnectInterval)) {
          connect();
        }
      }
    }
  } while (delayTime > 0 && !hasTimedOut(enterTime, delayTime));
}

/**
 * Read from client buffer
 */
void SkaarhojTCPClient::_readFromClient() {

  uint8_t loopCounter = 0;
  while (incomingAvailable()) {
    _EOLTimeOutReset();
    char c = incomingRead();
    //Serial << "INCOMING CHAR: " << c << "\n";

    if (c == _EOLChar) { // Line feed, always used
      _parselineDispatch();
      _resetLastIncomingMsg();
      _resetBuffer();
      loopCounter++;
      if (loopCounter>50) {
        _exitRunLoop = true;
        Serial << "Looped through 50 times, exiting...\n";
      }
      if (_exitRunLoop) return;
    } else if (c == 13 || c == 10) {                  // <CR> and <LF> ignored (they should be captured as _EOLChar if necessary)
                                                      // Ignore.
    } else if (_bufferWriteIndex < _bufferSize - 1) { // one byte for null termination reserved
      _buffer[_bufferWriteIndex] = c;
      _bufferWriteIndex++;
    } else {
      if (_serialOutput)  {
        Serial.println(F("ERROR: Buffer overflow.:"));
        Serial << _buffer << "\n"; 
        _resetBuffer();
      }
    }
  }
}

void SkaarhojTCPClient::_EOLTimeOutReset()  {
  _EOLtimer = millis();
}

bool SkaarhojTCPClient::_EOLTimeOut()  {
  return _bufferWriteIndex > 0 && _EOLTimeOutTime > 0 && hasTimedOut(_EOLtimer, _EOLTimeOutTime);
}

/**
 * Returns true if there are "incoming data" (either in buffer or from wiznet)
 */
bool SkaarhojTCPClient::incomingAvailable() {
  // return _client.available();

  if (TCPReadBuffer_ptr < TCPReadBuffer_len)  return true;

  if(_client.available()) {
    TCPReadBuffer_len = _client.read(TCPReadBuffer, 128);
    TCPReadBuffer_ptr = 0;
    return true;
  }
  return false;
}

/**
 * Return next char from buffer
 */
char SkaarhojTCPClient::incomingRead() {
  // return _client.read();

  return TCPReadBuffer[TCPReadBuffer_ptr++];
}

/**
 * Sends the line buffer
 */
void SkaarhojTCPClient::_sendBuffer() {
  _client.print(_buffer);
  if (_serialOutput > 2) {
    Serial.print(F("_sendBuffer: "));
    Serial.print(_buffer);
  }
  _resetBuffer();
  _pendingAnswer = true;
  _pendingEOT = true;
}
  