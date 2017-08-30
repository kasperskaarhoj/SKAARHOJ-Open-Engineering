/*
Copyright 2013-2014 Kasper Skårhøj, SKAARHOJ, kasper@skaarhoj.com

This file is part of the Blackmagic Design SmartView Client library for Arduino

The ClientBMDSmartView library is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by the
Free Software Foundation, either version 3 of the License, or (at your
option) any later version.

The ClientBMDSmartView library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with the ClientBMDSmartView library. If not, see http://www.gnu.org/licenses/.


IMPORTANT: If you want to use this library in your own projects and/or products,
please play a fair game and heed the license rules! See our web page for a Q&A so
you can keep a clear conscience: http://skaarhoj.com/about/licenses/


*/

#include "Arduino.h"
#include "ClientBMDSmartView.h"
#include "SkaarhojTCPClient.h"

/**
 * Constructor (using arguments is deprecated! Use begin() instead)
 */
ClientBMDSmartView::ClientBMDSmartView() {}

/**
 * Setting up IP address for the videohub (and local port to open telnet connection to)
 */
void ClientBMDSmartView::begin(IPAddress ip) {

  SkaarhojTCPClient::begin(ip);

  _localPort = 9992; // Set local port

  _statusRequestInterval = 0;
}

/**
 * Returns true if the SmartView is ready to receive a command (otherwise it is waiting for an answer to a previous command)
 */
bool ClientBMDSmartView::isReady() { return !_pendingAnswer; }

/**
 * Calls runLoop until the hasInitialized() flag has been set - or until delayTime has passed (if delay time is larger than 0 in which case it never times out)
 */
bool ClientBMDSmartView::waitForInit(uint16_t delayTime) {
  unsigned long enterTime = millis();

  while (!hasInitialized() && (delayTime == 0 || !hasTimedOut(enterTime, delayTime))) {
    runLoop();
  }

  return hasInitialized();
}

/**
 * Calls runLoop until the isReady() flag has been set - or until delayTime has passed (if delay time is larger than 0 in which case it never times out)
 */
bool ClientBMDSmartView::waitForReady(uint16_t delayTime) {
  unsigned long enterTime = millis();

  while (!isReady() && (delayTime == 0 || !hasTimedOut(enterTime, delayTime))) {
    runLoop();
  }

  return isReady();
}

/**
* Calls runLoop until the isReady() flag has been set - or until 100 ms has passed. Returns the value of isReady()
 */
bool ClientBMDSmartView::waitForReady() { return waitForReady(100); }

/**
 * Resets local device state variables. (Overloading from superclass)
 */
void ClientBMDSmartView::_resetDeviceStateVariables() {
  _section = 0;

  // Resetting device state variables:
  _Smart_scopeMode[0] = 0;
  _Smart_scopeMode[1] = 0;
}

/**
 * Parses a line from client. (Overloading from superclass)
 */
void ClientBMDSmartView::_parseline() {

  if (!strcmp(_buffer, "")) {
    _section = 0;
  } else if (!strcmp_P(_buffer, PSTR("PROTOCOL PREAMBLE:"))) { // strcmp_P("RAM STRING", PSTR("FLASH STRING"));
    _section = 1;
    _hasInitialized = true;
    if (_serialOutput) {
      Serial.print(F("Connection to SmartView "));
      Serial.print(_IP);
      Serial.println(F(" confirmed, pulling status"));
    }
  } else if (!strcmp_P(_buffer, PSTR("SMARTVIEW DEVICE:"))) {
    _section = 2;
  } else if (!strcmp_P(_buffer, PSTR("NETWORK:"))) {
    _section = 3;
  } else if (!strcmp_P(_buffer, PSTR("MONITOR A:"))) {
    _section = 4;
  } else if (!strcmp_P(_buffer, PSTR("MONITOR B:"))) {
    _section = 5;
  } else {

    _bufferReadIndex = 0;

    switch (_section) {
    case 4: // Monitor A or B:
    case 5:
      if (isNextPartOfBuffer_P(PSTR("Brightness: "))) {
        _Smart_brightness[_section - 4] = parseInt();

        if (_serialOutput > 1) {
          Serial.print(F("Brightness, Mon "));
          Serial.print(_section - 4);
          Serial.print(F(": "));
          Serial.println(_Smart_brightness[_section - 4]);
        }
      }

      if (isNextPartOfBuffer_P(PSTR("Contrast: "))) {
        _Smart_contrast[_section - 4] = parseInt();

        if (_serialOutput > 1) {
          Serial.print(F("Contrast, Mon "));
          Serial.print(_section - 4);
          Serial.print(F(": "));
          Serial.println(_Smart_contrast[_section - 4]);
        }
      }

      if (isNextPartOfBuffer_P(PSTR("Saturation: "))) {
        _Smart_saturation[_section - 4] = parseInt();

        if (_serialOutput > 1) {
          Serial.print(F("Saturation, Mon "));
          Serial.print(_section - 4);
          Serial.print(F(": "));
          Serial.println(_Smart_saturation[_section - 4]);
        }
      }

      if (isNextPartOfBuffer_P(PSTR("Identify: "))) {
        if (isNextPartOfBuffer_P(PSTR("true"))) {
          _Smart_identify[_section - 4] = true;
        } else {
          _Smart_identify[_section - 4] = false;
        }

        if (_serialOutput > 1) {
          Serial.print(F("Identify, Mon "));
          Serial.print(_section - 4);
          Serial.print(F(": "));
          Serial.println(_Smart_identify[_section - 4]);
        }
      }

      if (isNextPartOfBuffer_P(PSTR("Border: "))) {
        if (isNextPartOfBuffer_P(PSTR("None"))) {
          _Smart_border[_section - 4] = 0;
        } else if (isNextPartOfBuffer_P(PSTR("Red"))) {
          _Smart_border[_section - 4] = 1;
        } else if (isNextPartOfBuffer_P(PSTR("Green"))) {
          _Smart_border[_section - 4] = 2;
        } else if (isNextPartOfBuffer_P(PSTR("Blue"))) {
          _Smart_border[_section - 4] = 3;
/*					} else if (isNextPartOfBuffer_P(PSTR("White")))	{	// Doesn't work yet?
						_Smart_border[_section-4] = 4;
*/					} else {
  _Smart_border[_section - 4] = 255;
}

if (_serialOutput > 1) {
  Serial.print(F("Identify, Mon "));
  Serial.print(_section - 4);
  Serial.print(F(": "));
  Serial.println(_Smart_border[_section - 4]);
}
      }

      if (isNextPartOfBuffer_P(PSTR("WidescreenSD: "))) {
        if (isNextPartOfBuffer_P(PSTR("auto"))) {
          _Smart_widescreenSD[_section - 4] = 0;
        } else if (isNextPartOfBuffer_P(PSTR("on"))) {
          _Smart_widescreenSD[_section - 4] = 1;
        } else if (isNextPartOfBuffer_P(PSTR("off"))) {
          _Smart_widescreenSD[_section - 4] = 2;
        } else {
          _Smart_widescreenSD[_section - 4] = 255;
        }

        if (_serialOutput > 1) {
          Serial.print(F("WidescreenSD, Mon "));
          Serial.print(_section - 4);
          Serial.print(F(": "));
          Serial.println(_Smart_widescreenSD[_section - 4]);
        }
      }

      if (isNextPartOfBuffer_P(PSTR("ScopeMode: "))) {
        if (isNextPartOfBuffer_P(PSTR("Picture"))) {
          _Smart_scopeMode[_section - 4] = 0;
        } else if (isNextPartOfBuffer_P(PSTR("AudioDbfs"))) {
          _Smart_scopeMode[_section - 4] = 1;
        } else if (isNextPartOfBuffer_P(PSTR("AudioDbvu"))) {
          _Smart_scopeMode[_section - 4] = 2;
        } else if (isNextPartOfBuffer_P(PSTR("Histogram"))) {
          _Smart_scopeMode[_section - 4] = 3;
        } else if (isNextPartOfBuffer_P(PSTR("ParadeRGB"))) {
          _Smart_scopeMode[_section - 4] = 4;
        } else if (isNextPartOfBuffer_P(PSTR("ParadeYUV"))) {
          _Smart_scopeMode[_section - 4] = 5;
        } else if (isNextPartOfBuffer_P(PSTR("Vector100"))) {
          _Smart_scopeMode[_section - 4] = 6;
        } else if (isNextPartOfBuffer_P(PSTR("Vector75"))) {
          _Smart_scopeMode[_section - 4] = 7;
        } else if (isNextPartOfBuffer_P(PSTR("WaveformLuma"))) {
          _Smart_scopeMode[_section - 4] = 8;
        } else {
          _Smart_scopeMode[_section - 4] = 255;
        }

        if (_serialOutput > 1) {
          Serial.print(F("ScopeMode, Mon "));
          Serial.print(_section - 4);
          Serial.print(F(": "));
          Serial.println(_Smart_scopeMode[_section - 4]);
        }
      }

      if (isNextPartOfBuffer_P(PSTR("AudioChannel: "))) {
        _Smart_audioChannel[_section - 4] = parseInt();

        if (_serialOutput > 1) {
          Serial.print(F("AudioChannel, Mon "));
          Serial.print(_section - 4);
          Serial.print(F(": "));
          Serial.println(_Smart_audioChannel[_section - 4]);
        }
      }
      break;
    }
  }
}

/**
 * Sending a command to SmartView
 */
void ClientBMDSmartView::_sendCmdRequest(uint8_t monId, const char *str) { _sendCmdRequest(monId, str, String("")); }

/**
 * Sends a command request
 * Notice that str is in PROGMEM (pass string wrapped in PSTR("")) and command is a String object (useful for variable data, but more memory intensive.)
 */
void ClientBMDSmartView::_sendCmdRequest(uint8_t monId, const char *str, const String command) {
  _resetBuffer();
  _addToBuffer_P(PSTR("MONITOR "));
  _addToBuffer_P(monId == 0 ? PSTR("A:\n") : PSTR("B:\n"));
  _addToBuffer_P(str);
  _addToBuffer(command);
  _addToBuffer_P(PSTR("\n\n"));
  _sendBuffer();
}

/**
 * Sends ping command. (Overloading from superclass)
 */
void ClientBMDSmartView::_sendPing() {
  setBorder(0, getBorder(0)); // Unfortunately there is no "PING" command on this protocol, so we just set a random of the parameters to its current value to provoke an "ACK" in return.
}

void ClientBMDSmartView::printStateToSerial() {
  for (uint8_t monId = 0; monId <= 1; monId++) {
    Serial.print(F("Monitor "));
    Serial.println(monId == 0 ? F("A:") : F("B:"));

    Serial.print(F("Brightness: "));
    Serial.println(getBrightness(monId));

    Serial.print(F("Contrast: "));
    Serial.println(getContrast(monId));

    Serial.print(F("Saturation: "));
    Serial.println(getSaturation(monId));

    Serial.print(F("Identify: "));
    Serial.println(getIdentify(monId));

    Serial.print(F("Border: "));
    Serial.println(getBorder(monId));

    Serial.print(F("WidescreenSD: "));
    Serial.println(getWidescreenSD(monId));

    Serial.print(F("ScopeMode: "));
    Serial.println(getScopeMode(monId));

    Serial.print(F("AudioChannel: "));
    Serial.println(getAudioChannel(monId));

    Serial.println();
  }
}

/**************************************
 *
 * State getter and setters. Public.
 *
 **************************************/

uint8_t ClientBMDSmartView::getBrightness(uint8_t monId) { return _Smart_brightness[monId]; }
void ClientBMDSmartView::setBrightness(uint8_t monId, uint8_t brightness) {
  if (monId <= 1) {
    _Smart_brightness[monId] = brightness;
    _sendCmdRequest(monId, PSTR("Brightness: "), String(brightness));
  }
}

uint8_t ClientBMDSmartView::getContrast(uint8_t monId) { return _Smart_contrast[monId]; }
void ClientBMDSmartView::setContrast(uint8_t monId, uint8_t contrast) { // 127 is normal
  if (monId <= 1) {
    _Smart_contrast[monId] = contrast;
    _sendCmdRequest(monId, PSTR("Contrast: "), String(contrast));
  }
}

uint8_t ClientBMDSmartView::getSaturation(uint8_t monId) { return _Smart_saturation[monId]; }
void ClientBMDSmartView::setSaturation(uint8_t monId, uint8_t saturation) { // 127 is normal
  if (monId <= 1) {
    _Smart_saturation[monId] = saturation;
    _sendCmdRequest(monId, PSTR("Saturation: "), String(saturation));
  }
}

/**
 * Returns the identify state of monitor monID (0=A, 1=B)
 */
bool ClientBMDSmartView::getIdentify(uint8_t monId) { return _Smart_identify[monId]; }

/**
 * Sets the scope mode of monitor monID (0=A, 1=B)
 */
void ClientBMDSmartView::setIdentify(uint8_t monId, bool identify) {
  if (monId <= 1) {
    _Smart_identify[monId] = identify;

    if (identify) {
      _sendCmdRequest(monId, PSTR("Identify: true"));
    } else {
      _sendCmdRequest(monId, PSTR("Identify: false"));
    }
  }
}

uint8_t ClientBMDSmartView::getBorder(uint8_t monId) { return _Smart_border[monId]; }
void ClientBMDSmartView::setBorder(uint8_t monId, uint8_t border) {
  if (border <= 3 && monId <= 1) {
    _Smart_border[monId] = border;

    switch (border) {
    case 0:
      _sendCmdRequest(monId, PSTR("Border: None"));
      break;
    case 1:
      _sendCmdRequest(monId, PSTR("Border: Red"));
      break;
    case 2:
      _sendCmdRequest(monId, PSTR("Border: Green"));
      break;
    case 3:
      _sendCmdRequest(monId, PSTR("Border: Blue"));
      break;
      /*			case 4: // Not functional yet
                                      _sendCmdRequest(monId, PSTR("Border: White"));
                              break;*/
    }
  }
}

uint8_t ClientBMDSmartView::getWidescreenSD(uint8_t monId) { return _Smart_widescreenSD[monId]; }
void ClientBMDSmartView::setWidescreenSD(uint8_t monId, uint8_t widescreenSD) {
  if (widescreenSD <= 2 && monId <= 1) {
    _Smart_widescreenSD[monId] = widescreenSD;

    switch (widescreenSD) {
    case 0:
      _sendCmdRequest(monId, PSTR("WidescreenSD: auto"));
      break;
    case 1:
      _sendCmdRequest(monId, PSTR("WidescreenSD: on"));
      break;
    case 2:
      _sendCmdRequest(monId, PSTR("WidescreenSD: off"));
      break;
    }
  }
}

/**
 * Returns the scope mode of monitor monID (0=A, 1=B)
 */
uint8_t ClientBMDSmartView::getScopeMode(uint8_t monId) { return _Smart_scopeMode[monId]; }

/**
 * Sets the scope mode of monitor monID (0=A, 1=B)
 */
void ClientBMDSmartView::setScopeMode(uint8_t monId, uint8_t ScopeModeIndex) {
  if (ScopeModeIndex <= 8 && monId <= 1) {
    _Smart_scopeMode[monId] = ScopeModeIndex;

    switch (ScopeModeIndex) {
    case 0:
      _sendCmdRequest(monId, PSTR("ScopeMode: Picture"));
      break;
    case 1:
      _sendCmdRequest(monId, PSTR("ScopeMode: AudioDbfs"));
      break;
    case 2:
      _sendCmdRequest(monId, PSTR("ScopeMode: AudioDbvu"));
      break;
    case 3:
      _sendCmdRequest(monId, PSTR("ScopeMode: Histogram"));
      break;
    case 4:
      _sendCmdRequest(monId, PSTR("ScopeMode: ParadeRGB"));
      break;
    case 5:
      _sendCmdRequest(monId, PSTR("ScopeMode: ParadeYUV"));
      break;
    case 6:
      _sendCmdRequest(monId, PSTR("ScopeMode: Vector100"));
      break;
    case 7:
      _sendCmdRequest(monId, PSTR("ScopeMode: Vector75"));
      break;
    case 8:
      _sendCmdRequest(monId, PSTR("ScopeMode: WaveformLuma"));
      break;
    }
  }
}

uint8_t ClientBMDSmartView::getAudioChannel(uint8_t monId) { return _Smart_audioChannel[monId]; }

void ClientBMDSmartView::setAudioChannel(uint8_t monId, uint8_t audioChannel) {
  if (audioChannel <= 7 && monId <= 1) {
    _Smart_audioChannel[monId] = audioChannel;
    _sendCmdRequest(monId, PSTR("AudioChannel: "), String(audioChannel));
  }
}
