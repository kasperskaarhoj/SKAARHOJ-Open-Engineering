/*
Copyright 2013-2014 Kasper Skårhøj, SKAARHOJ, kasper@skaarhoj.com

This file is part of the Blackmagic Design Teranex Client library for Arduino

The ClientBMDTeranex library is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by the
Free Software Foundation, either version 3 of the License, or (at your
option) any later version.

The ClientBMDTeranex library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with the ClientBMDTeranex library. If not, see http://www.gnu.org/licenses/.


IMPORTANT: If you want to use this library in your own projects and/or products,
please play a fair game and heed the license rules! See our web page for a Q&A so
you can keep a clear conscience: http://skaarhoj.com/about/licenses/


*/

#include "Arduino.h"
#include "ClientBMDTeranex.h"
#include "SkaarhojTCPClient.h"

/**
 * Constructor (using arguments is deprecated! Use begin() instead)
 */
ClientBMDTeranex::ClientBMDTeranex() {}

/**
 * Setting up IP address for the videohub (and local port to open telnet connection to)
 */
void ClientBMDTeranex::begin(IPAddress ip) {

  SkaarhojTCPClient::begin(ip);

  _localPort = 9995; // Set local port

  _statusRequestInterval = 0;
}

/**
 * Returns true if the Teranex is ready to receive a command (otherwise it is waiting for an answer to a previous command)
 */
bool ClientBMDTeranex::isReady() { return !_pendingAnswer; }

/**
 * Calls runLoop until the hasInitialized() flag has been set - or until delayTime has passed (if delay time is larger than 0 in which case it never times out)
 */
bool ClientBMDTeranex::waitForInit(uint16_t delayTime) {
  unsigned long enterTime = millis();

  while (!hasInitialized() && (delayTime == 0 || !hasTimedOut(enterTime, delayTime))) {
    runLoop();
  }

  return hasInitialized();
}

/**
 * Calls runLoop until the isReady() flag has been set - or until delayTime has passed (if delay time is larger than 0 in which case it never times out)
 */
bool ClientBMDTeranex::waitForReady(uint16_t delayTime) {
  unsigned long enterTime = millis();

  while (!isReady() && (delayTime == 0 || !hasTimedOut(enterTime, delayTime))) {
    runLoop();
  }

  return isReady();
}

/**
* Calls runLoop until the isReady() flag has been set - or until 100 ms has passed. Returns the value of isReady()
 */
bool ClientBMDTeranex::waitForReady() { return waitForReady(100); }

/**
 * Resets local device state variables. (Overloading from superclass)
 */
void ClientBMDTeranex::_resetDeviceStateVariables() {
  _section = 0;
}

/**
 * Parses a line from client. (Overloading from superclass)
 */
void ClientBMDTeranex::_parseline() {

  if (!strcmp(_buffer, "")) {
    _section = 0;
  } else if (!strcmp_P(_buffer, PSTR("PROTOCOL PREAMBLE:"))) { // strcmp_P("RAM STRING", PSTR("FLASH STRING"));
    _section = 1;
    _hasInitialized = true;
    if (_serialOutput) {
      Serial.print(F("Connection to Teranex "));
      Serial.print(_IP);
      Serial.println(F(" confirmed, pulling status"));
    }
  } else if (!strcmp_P(_buffer, PSTR("TERANEX MINI DEVICE:"))) {
    _section = 2;
  } else if (!strcmp_P(_buffer, PSTR("NETWORK:"))) {
    _section = 3;
  } else if (!strcmp_P(_buffer, PSTR("AUDIO OUTPUT:"))) {
    _section = 4;
  } else if (!strcmp_P(_buffer, PSTR("VIDEO OUTPUT:"))) {
    _section = 5;
  } else {

    _bufferReadIndex = 0;

    switch (_section) {
    case 5: // Video Output
      if (isNextPartOfBuffer_P(PSTR("Lut selection: "))) {
        if (isNextPartOfBuffer_P(PSTR("Lut 0"))) {
          _Teranex_LUT = 1;
        } else if (isNextPartOfBuffer_P(PSTR("Lut 1"))) {
          _Teranex_LUT = 2;
        } else {
          _Teranex_LUT = 0;
        }

        if (_serialOutput > 1) {
          Serial.print(F("LUT: "));
          Serial.println(_Teranex_LUT);
        }
      }
      break;
    }
  }
}

/**
 * Sending a command to Teranex
 */
void ClientBMDTeranex::_sendCmdRequest(uint8_t section, const char *str) { _sendCmdRequest(section, str, String("")); }

/**
 * Sends a command request
 * Notice that str is in PROGMEM (pass string wrapped in PSTR("")) and command is a String object (useful for variable data, but more memory intensive.)
 */
void ClientBMDTeranex::_sendCmdRequest(uint8_t section, const char *str, const String command) {

  _resetBuffer();
  switch (section) {
  case 5:
    _addToBuffer_P(PSTR("VIDEO OUTPUT:\n\r"));
    _addToBuffer_P(str);
    _addToBuffer(command);
    _addToBuffer_P(PSTR("\n\r\n\r"));
    break;
  default:
    _addToBuffer_P(str);
    _addToBuffer(command);
    _addToBuffer_P(PSTR("\n\r\n\r"));
    break;
  }

  _sendBuffer();
}

/**
 * Sends ping command. (Overloading from superclass)
 */
void ClientBMDTeranex::_sendPing() {
  _sendCmdRequest(0, PSTR("PING")); // Unfortunately there is no "PING" command on this protocol, so we just send one and get NAK in return - that's a life signal at least...
}

void ClientBMDTeranex::printStateToSerial() {
  Serial.print(F("Lut: "));
  Serial.println(_Teranex_LUT);
}

/**************************************
 *
 * State getter and setters. Public.
 *
 **************************************/

uint8_t ClientBMDTeranex::getLut() { return _Teranex_LUT; }
void ClientBMDTeranex::setLut(uint8_t lut) {
  _Teranex_LUT = lut;
  _sendCmdRequest(5, PSTR("Lut selection: "), (uint8_t)(_Teranex_LUT-1) < 2 ? String("Lut ") + String(_Teranex_LUT-1) : String("none"));
}
