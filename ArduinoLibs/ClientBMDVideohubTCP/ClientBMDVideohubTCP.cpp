/*
Copyright 2012-2014 Kasper Skårhøj, SKAARHOJ, kasper@skaarhoj.com

This file is part of the Blackmagic Design Videohub Client library for Arduino

The ClientBMDVideohubTCP library is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by the
Free Software Foundation, either version 3 of the License, or (at your
option) any later version.

The ClientBMDVideohubTCP library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with the ClientBMDVideohubTCP library. If not, see http://www.gnu.org/licenses/.


IMPORTANT: If you want to use this library in your own projects and/or products,
please play a fair game and heed the license rules! See our web page for a Q&A so
you can keep a clear conscience: http://skaarhoj.com/about/licenses/


*/

#include "Arduino.h"
#include "ClientBMDVideohubTCP.h"
#include "SkaarhojTCPClient.h"

/**
 * Constructor (using arguments is deprecated! Use begin() instead)
 */
ClientBMDVideohubTCP::ClientBMDVideohubTCP() {}

/**
 * Overloading method: Setting up IP address for the videohub (and local port to open telnet connection to)
 */
void ClientBMDVideohubTCP::begin(IPAddress ip) {

  SkaarhojTCPClient::begin(ip, 9990);

  _statusRequestInterval = 60000;
}

/**
 * Resets local device state variables. (Overloading from superclass)
 */
void ClientBMDVideohubTCP::_resetDeviceStateVariables() {
  _section = 0;

  _devicePresent = false;

  _numInputs = 0;
  _numOutputs = 0;
  _numMonOutputs = 0;
  _numProcUnits = 0;
  _serialPorts = 0;

  for (uint8_t inputNum = 0; inputNum < ClientBMDVideohubTCP_LABELCOUNT; inputNum++) {
    memset(_inputLabels[inputNum], 0, ClientBMDVideohubTCP_LABELLEN);
  }
  for (uint8_t outputNum = 0; outputNum < ClientBMDVideohubTCP_LABELCOUNT; outputNum++) {
    memset(_outputLabels[outputNum], 0, ClientBMDVideohubTCP_LABELLEN);
    _outputLocks[outputNum] = ' ';
    _outputRouting[outputNum] = 255;
  }
}

/**
 * Parses a line from client. (Overloading from superclass)
 */
void ClientBMDVideohubTCP::_parseline() {

  uint8_t inputNum;
  uint8_t outputNum;

  /*
          Additional sections in the protocol not (yet) supported:
                  MONITORING OUTPUT LABELS:
                  VIDEO MONITORING OUTPUT ROUTING:
                  MONITORING OUTPUT LOCKS:
                  SERIAL PORT LABELS:
                  SERIAL PORT ROUTING:
                  SERIAL PORT LOCKS:
                  SERIAL PORT DIRECTIONS:
                  PROCESSING UNIT ROUTING:
                  PROCESSING UNIT LOCKS:
                  FRAME LABELS:
                  FRAME BUFFER ROUTING:
                  FRAME BUFFER LOCKS:
  */

  if (!strcmp(_buffer, "")) {
    _section = 0;
  } else if (!strcmp_P(_buffer, PSTR("PROTOCOL PREAMBLE:"))) { // strcmp_P("RAM STRING", PSTR("FLASH STRING"));
    if (_serialOutput) {
      Serial.print(F("Connection to Videohub "));
      Serial.print(_IP);
      Serial.println(F(" confirmed, pulling status"));
    }
    _section = 1;
    _hasInitialized = true;
    _lastStatusRequest = millis();
  } else if (!strcmp_P(_buffer, PSTR("VIDEOHUB DEVICE:"))) {
    _section = 2;
  } else if (!strcmp_P(_buffer, PSTR("INPUT LABELS:"))) {
    _section = 3;
  } else if (!strcmp_P(_buffer, PSTR("OUTPUT LABELS:"))) {
    _section = 4;
  } else if (!strcmp_P(_buffer, PSTR("VIDEO OUTPUT LOCKS:"))) {
    _section = 5;
  } else if (!strcmp_P(_buffer, PSTR("VIDEO OUTPUT ROUTING:"))) {
    _section = 6;
  } else {
    _bufferReadIndex = 0;
    switch (_section) {
    case 2: // VIDEOHUB DEVICE
      if (isNextPartOfBuffer_P(PSTR("Device present: "))) {
        _devicePresent = isNextPartOfBuffer_P(PSTR("true"));
        if (_serialOutput > 1)
          Serial.print(F("_devicePresent="));
        if (_serialOutput > 1)
          Serial.println(_devicePresent);
      }
      if (isNextPartOfBuffer_P(PSTR("Video inputs: "))) {
        _numInputs = parseInt();
        if (_serialOutput > 1)
          Serial.print(F("_numInputs="));
        if (_serialOutput > 1)
          Serial.println(_numInputs);
      }
      if (isNextPartOfBuffer_P(PSTR("Video processing units: "))) {
        _numProcUnits = parseInt();
        if (_serialOutput > 1)
          Serial.print(F("_numProcUnits="));
        if (_serialOutput > 1)
          Serial.println(_numProcUnits);
      }
      if (isNextPartOfBuffer_P(PSTR("Video outputs: "))) {
        _numOutputs = parseInt();
        if (_serialOutput > 1)
          Serial.print(F("_numOutputs="));
        if (_serialOutput > 1)
          Serial.println(_numOutputs);
      }
      if (isNextPartOfBuffer_P(PSTR("Video monitoring outputs: "))) {
        _numMonOutputs = parseInt();
        if (_serialOutput > 1)
          Serial.print(F("_numMonOutputs="));
        if (_serialOutput > 1)
          Serial.println(_numMonOutputs);
      }
      if (isNextPartOfBuffer_P(PSTR("Serial ports: "))) {
        _serialPorts = parseInt();
        if (_serialOutput > 1)
          Serial.print(F("_serialPorts="));
        if (_serialOutput > 1)
          Serial.println(_serialPorts);
      }
      break;
    case 3: // INPUT LABELS
      inputNum = parseInt();
      if (inputNum < ClientBMDVideohubTCP_LABELCOUNT) {
        _bufferReadIndex++;

        uint8_t idx = 0;
        memset(_inputLabels[inputNum], 0, ClientBMDVideohubTCP_LABELLEN);
        while (idx < ClientBMDVideohubTCP_LABELLEN - 1 && _bufferReadIndex < _bufferWriteIndex) {
          _inputLabels[inputNum][idx++] = _buffer[_bufferReadIndex];
          _bufferReadIndex++;
        }

        if (_serialOutput > 1)
          Serial.print(F("_inputLabels "));
        if (_serialOutput > 1)
          Serial.print(inputNum);
        if (_serialOutput > 1)
          Serial.print(F("="));
        if (_serialOutput > 1)
          Serial.println(_inputLabels[inputNum]);
      }
      break;
    case 4: // OUTPUT LABELS
      outputNum = parseInt();
      if (outputNum < ClientBMDVideohubTCP_LABELCOUNT) {
        _bufferReadIndex++;

        uint8_t idx = 0;
        memset(_outputLabels[outputNum], 0, ClientBMDVideohubTCP_LABELLEN);
        while (idx < ClientBMDVideohubTCP_LABELLEN - 1 && _bufferReadIndex < _bufferWriteIndex) {
          _outputLabels[outputNum][idx++] = _buffer[_bufferReadIndex];
          _bufferReadIndex++;
        }

        if (_serialOutput > 1)
          Serial.print(F("_outputLabels "));
        if (_serialOutput > 1)
          Serial.print(outputNum);
        if (_serialOutput > 1)
          Serial.print(F("="));
        if (_serialOutput > 1)
          Serial.println(_outputLabels[outputNum]);
      }
      break;
    case 5: // VIDEO OUTPUT LOCKS
      outputNum = parseInt();
      if (outputNum < ClientBMDVideohubTCP_NUMOUTPUTS) {
        _outputLocks[outputNum] = _buffer[_bufferReadIndex + 1];
        if (_serialOutput > 1)
          Serial.print(F("_outputLock "));
        if (_serialOutput > 1)
          Serial.print(outputNum);
        if (_serialOutput > 1)
          Serial.print(F("="));
        if (_serialOutput > 1)
          Serial.println(_outputLocks[outputNum]);
      }
      break;
    case 6: // VIDEO OUTPUT ROUTING
      outputNum = parseInt();
      if (outputNum < ClientBMDVideohubTCP_NUMOUTPUTS) {
        _bufferReadIndex++;
        _outputRouting[outputNum] = parseInt();
        if (_serialOutput > 1)
          Serial.print(F("_outputRouting "));
        if (_serialOutput > 1)
          Serial.print(outputNum);
        if (_serialOutput > 1)
          Serial.print(F("="));
        if (_serialOutput > 1)
          Serial.println(_outputRouting[outputNum]);
      }
      break;
    }
  }
}

/**
 * Sends status command. (Overloading from superclass)
 */
void ClientBMDVideohubTCP::_sendStatus() {
  _resetBuffer();
  _addToBuffer_P(PSTR("VIDEO OUTPUT ROUTING:\n\n"));
  _addToBuffer_P(PSTR("VIDEO OUTPUT LOCKS:\n\n"));
  // If more are added, the buffer will be too small and they must be sent in multiple batches...
  _sendBuffer();
}

/**
 * Sends ping command. (Overloading from superclass)
 */
void ClientBMDVideohubTCP::_sendPing() {
  _resetBuffer();
  _addToBuffer_P(PSTR("PING:\n\n"));
  _sendBuffer();
}

void ClientBMDVideohubTCP::routeInputToOutput(uint8_t input, uint8_t output, bool waitForConfirmedChange) {
  _resetBuffer();
  _addToBuffer_P(PSTR("VIDEO OUTPUT ROUTING:\n"));
  _addToBuffer(String(output - 1));
  _addToBuffer_P(PSTR(" "));
  _addToBuffer(String(input - 1));
  _addToBuffer_P(PSTR("\n\n"));
  _sendBuffer();
  
  if (waitForConfirmedChange)	{
	  unsigned long timer = millis();
	  while(_outputRouting[output - 1] != input-1 && millis()-500 < timer)	{	// 500 ms timeout if route is not set...
		  runLoop();
	  }
  }
}
void ClientBMDVideohubTCP::setLock(uint8_t output, char newState) {
  _resetBuffer();
  _addToBuffer_P(PSTR("VIDEO OUTPUT LOCKS:\n"));
  _addToBuffer(String(output - 1));
  _addToBuffer_P(PSTR(" "));
  _addToBuffer(String(newState));
  _addToBuffer_P(PSTR("\n\n"));
  _sendBuffer();
}
uint8_t ClientBMDVideohubTCP::getRoute(uint8_t output) { return _outputRouting[output - 1] + 1; }
char ClientBMDVideohubTCP::getLock(uint8_t output) { return _outputLocks[output - 1]; }
char *ClientBMDVideohubTCP::getInputLabel(uint8_t input) { 
  static char _label[] = "Input XX";
  if (input<99) itoa(input, _label+6, 10);
  return input > 0 && input <= ClientBMDVideohubTCP_LABELCOUNT ? _inputLabels[input - 1] : _label; 
}
char *ClientBMDVideohubTCP::getOutputLabel(uint8_t output) { 
  static char _label[] = "Output XX";
  if (output<99) itoa(output, _label+7, 10);
  return output > 0 && output <= ClientBMDVideohubTCP_LABELCOUNT ? _outputLabels[output - 1] : _label; 
}
uint8_t ClientBMDVideohubTCP::getNumInputs() { return _numInputs; }
uint8_t ClientBMDVideohubTCP::getNumOutputs() { return _numOutputs; }
