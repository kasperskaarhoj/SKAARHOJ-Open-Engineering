/*
Copyright 2017 Kasper Skårhøj, SKAARHOJ, kasper@skaarhoj.com

This file is part of the SKAARHOJ UniSketch Client library for Arduino

The ClientSKAARHOJUniSketchTCP library is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by the
Free Software Foundation, either version 3 of the License, or (at your
option) any later version.

The ClientSKAARHOJUniSketchTCP library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with the ClientSKAARHOJUniSketchTCP library. If not, see http://www.gnu.org/licenses/.


IMPORTANT: If you want to use this library in your own projects and/or products,
please play a fair game and heed the license rules! See our web page for a Q&A so
you can keep a clear conscience: http://skaarhoj.com/about/licenses/


*/

#include "Arduino.h"
#include "ClientSKAARHOJUniSketchTCP.h"
#include "SkaarhojTCPClient.h"
#include "Streaming.h"

/**
 * Constructor (using arguments is deprecated! Use begin() instead)
 */
ClientSKAARHOJUniSketchTCP::ClientSKAARHOJUniSketchTCP() {}

/**
 * Setting up IP address for the videohub (and local port to open telnet connection to)
 */
void ClientSKAARHOJUniSketchTCP::begin(IPAddress ip) {

  SkaarhojTCPClient::begin(ip);

  _localPort = 9923; // Set local port

  _statusRequestInterval = 60000;
}

/**
 * Resets local device state variables. (Overloading from superclass)
 */
void ClientSKAARHOJUniSketchTCP::_resetDeviceStateVariables() {
  memset(_deviceState_HWC,0,128); // I think we can extend this to 256, at least HWC numbers up to 256 is allowed.
  memset(_deviceState_MEM,0,12); 
  memset(_deviceState_FLAG,0,64);
  memset(_deviceState_SHIFT,0,5);
  memset(_deviceState_STATE,0,5);
  _deviceState_INACTIVEPANEL = false;
}

/**
 * Parses a line from client. (Overloading from superclass)
 */
void ClientSKAARHOJUniSketchTCP::_parseline() {
  uint8_t idx ; 

  if (isNextPartOfBuffer_P(PSTR("HWC#"))) {
    idx = parseInt();
    if (idx >=1 && idx <= 128 && isNextPartOfBuffer_P(PSTR("="))) {
      _deviceState_HWC[idx-1] = parseInt();
      if (_serialOutput > 1) {
        Serial << F("HWC #") << idx << F("=") << _deviceState_HWC[idx-1] << F("\n");
      }
    }
  }

  else if (isNextPartOfBuffer_P(PSTR("Mem")))  {
    char* next = getRemainingBuffer();
    if(next[1] == '=' && next[0] >= 'A' && next[0] <= 'L') {
      idx = constrain(next[0] - 'A',0,11);  // Safety...
      advancePointer(2);
      _deviceState_MEM[idx] = constrain(parseInt(), 0, 255);
      if (_serialOutput > 1) {
        Serial << F("MEM #") << idx << F("=") << _deviceState_MEM[idx] << F("\n");
      }
    }
  }

  else if (isNextPartOfBuffer_P(PSTR("Flag#")))  {
    idx = parseInt();
    if (idx < 64 && isNextPartOfBuffer_P(PSTR("=")))  {
      _deviceState_FLAG[idx] = parseInt() ? 1 : 0;
      if (_serialOutput > 1) {
        Serial << F("FLAG #") << idx << F("=") << _deviceState_FLAG[idx] << F("\n");
      }
    }
  }
  
  else if (isNextPartOfBuffer_P(PSTR("Shift")))  {
    idx = 0;
    if (isNextPartOfBuffer_P(PSTR("=")))  {
      idx = 0;
    }
    else if (isNextPartOfBuffer_P(PSTR("A=")))  {
      idx = 1;
    }
    else if (isNextPartOfBuffer_P(PSTR("B=")))  {
      idx = 2;
    }
    else if (isNextPartOfBuffer_P(PSTR("C=")))  {
      idx = 3;
    }
    else if (isNextPartOfBuffer_P(PSTR("D=")))  {
      idx = 4;
    }
    _deviceState_SHIFT[idx] = parseInt();
    if (_serialOutput > 1) {
      Serial << F("SHIFT #") << idx << F("=") << _deviceState_SHIFT[idx] << F("\n");
    }
  }
  
  else if (isNextPartOfBuffer_P(PSTR("State")))  {
    idx = 0;
    if (isNextPartOfBuffer_P(PSTR("=")))  {
      idx = 0;
    }
    else if (isNextPartOfBuffer_P(PSTR("P=")))  {
      idx = 1;
    }
    else if (isNextPartOfBuffer_P(PSTR("Q=")))  {
      idx = 2;
    }
    else if (isNextPartOfBuffer_P(PSTR("R=")))  {
      idx = 3;
    }
    else if (isNextPartOfBuffer_P(PSTR("S=")))  {
      idx = 4;
    }
    _deviceState_STATE[idx] = parseInt();
    if (_serialOutput > 1) {
      Serial << F("STATE #") << idx << F("=") << _deviceState_STATE[idx] << F("\n");
    }
  }

  else if (isNextPartOfBuffer_P(PSTR("ActivePanel=")))  {
    _deviceState_INACTIVEPANEL = !parseInt();

    if (!_hasInitialized) {
      _hasInitialized = true; // Assuming this is the last piece of data to receive.
      if (_serialOutput) {
        Serial << F("UniSketch TCP Client has initialized\n");
      }
    }
  }
}

/**
 * Sends a command request
 */
void ClientSKAARHOJUniSketchTCP::_sendCmdRequest(const String command) {
  _resetBuffer();

  _addToBuffer(command);
  _addToBuffer_P(PSTR("\n"));

  _sendBuffer();
}

/**
 * Sends ping command. (Overloading from superclass)
 */
void ClientSKAARHOJUniSketchTCP::_sendPing() {
  _sendCmdRequest(String("ping"));
}

/**
 * Sends ping command. (Overloading from superclass)
 */
void ClientSKAARHOJUniSketchTCP::_sendStatus() {
  _sendCmdRequest(String("list"));
  if (_serialOutput > 1)
    Serial << "Asking for status....\n";
  //_hasInitialized = true; // Assuming this is the last piece of data to receive.
}

/**
 * Initialize Connection
 */
void ClientSKAARHOJUniSketchTCP::initializeConnection() {
  _resetDeviceStateVariables();
  _sendStatus();
}


/**************************************
 *
 * State getter and setters. Public.
 *
 **************************************/

bool ClientSKAARHOJUniSketchTCP::getFlag(uint8_t flag) { return _deviceState_FLAG[flag]; }
void ClientSKAARHOJUniSketchTCP::setFlag(uint8_t flag, bool value) {
  if (flag<64)  {
//    _deviceState_FLAG[flag] = value;

    _client.beginPacket();
    _client << "Flag#" << flag << "=" << value << "\n";
    _client.endPacket();

    // _sendCmdRequest(String("Flag#") + String(flag) + String("=") + String(_deviceState_FLAG[flag]));
  }
}

uint8_t ClientSKAARHOJUniSketchTCP::getMem(uint8_t idx) { return _deviceState_MEM[idx]; }
void ClientSKAARHOJUniSketchTCP::setMem(uint8_t idx, uint8_t value) {
  if (idx<12)  {
//    _deviceState_MEM[idx] = value;

    _client.beginPacket();
    _client << "Mem" << char(idx+65) << "=" << value << "\n";
    _client.endPacket();

    // _sendCmdRequest(String("Flag#") + String(flag) + String("=") + String(_deviceState_FLAG[flag]));
  }
}

uint8_t ClientSKAARHOJUniSketchTCP::getShift(uint8_t idx) { return _deviceState_SHIFT[idx]; }
void ClientSKAARHOJUniSketchTCP::setShift(uint8_t idx, uint8_t value) {
  if (idx<5)  {
//    _deviceState_SHIFT[idx] = value;

    _client.beginPacket();
    _client << "Shift";
    if (idx>0)
      _client << char(idx+64);
    _client << "=" << value << "\n";
    _client.endPacket();

    // _sendCmdRequest(String("Flag#") + String(flag) + String("=") + String(_deviceState_FLAG[flag]));
  }
}

uint8_t ClientSKAARHOJUniSketchTCP::getState(uint8_t idx) { return _deviceState_STATE[idx]; }
void ClientSKAARHOJUniSketchTCP::setState(uint8_t idx, uint8_t value) {
  if (idx<5)  {
//    _deviceState_STATE[idx] = value;

    _client.beginPacket();
    _client << "State";
    if (idx>0)
      _client << char(idx+64+15);
    _client << "=" << value << "\n";
    
    _client.endPacket();

    // _sendCmdRequest(String("Flag#") + String(flag) + String("=") + String(_deviceState_FLAG[flag]));
  }
}

uint8_t ClientSKAARHOJUniSketchTCP::getInactivePanel() { return _deviceState_INACTIVEPANEL; }
void ClientSKAARHOJUniSketchTCP::setInactivePanel(bool value) {
  //return;
  //_deviceState_INACTIVEPANEL = value;

  _client.beginPacket();
  _client << "ActivePanel=" << (!value) << "\n";
  _client.endPacket();

  // _sendCmdRequest(String("Flag#") + String(flag) + String("=") + String(_deviceState_FLAG[flag]));
}

uint8_t ClientSKAARHOJUniSketchTCP::getHWCoutput(uint8_t hwc) { return _deviceState_HWC[hwc-1]; }
void ClientSKAARHOJUniSketchTCP::sendHWC_Press(uint8_t hwc) {
  _client.beginPacket();
  _client << "HWC#" << hwc << "=Press\n";
//  Serial << "OUTGOING: HWC#" << hwc << "=Press\n";
  _client.endPacket();
}
void ClientSKAARHOJUniSketchTCP::sendHWC_Down(uint8_t hwc) {
  _client.beginPacket();
  _client << "HWC#" << hwc << "=Down\n";
//  Serial << "OUTGOING: HWC#" << hwc << "=Down\n";
  _client.endPacket();
}
void ClientSKAARHOJUniSketchTCP::sendHWC_Up(uint8_t hwc) {
  _client.beginPacket();
  _client << "HWC#" << hwc << "=Up\n";
//  Serial << "OUTGOING: HWC#" << hwc << "=Up\n";
  _client.endPacket();
}
void ClientSKAARHOJUniSketchTCP::sendHWC_Enc(uint8_t hwc, int pulseCount) {
  _client.beginPacket();
  _client << "HWC#" << hwc << "=Enc:" << pulseCount << "\n";
//  Serial << "OUTGOING: HWC#" << hwc << "=Enc:" << pulseCount << "\n";
  _client.endPacket();
}
void ClientSKAARHOJUniSketchTCP::sendHWC_Abs(uint8_t hwc, int value) {
  _client.beginPacket();
  _client << "HWC#" << hwc << "=Abs:" << value << "\n";
//  Serial << "OUTGOING: HWC#" << hwc << "=Abs:" << value << "\n";
  _client.endPacket();
}
void ClientSKAARHOJUniSketchTCP::sendHWC_Speed(uint8_t hwc, int value) {
  _client.beginPacket();
  _client << "HWC#" << hwc << "=Speed:" << value << "\n";
//  Serial << "OUTGOING: HWC#" << hwc << "=Speed:" << value << "\n";
  _client.endPacket();
}

/*

              << F("- HWC#xx=Down : Simulate a Down trigger for HWC xx\n")
              << F("- HWC#xx=Up : Simulate an Up trigger for HWC xx\n")
              << F("- HWC#xx=Press : Simulate an immediate Down+Up trigger for HWC xx\n")
              << F("- HWC#xx=Abs:y : Send absolute value (y=0-1000) to HWC xx\n")
              << F("- HWC#xx=Enc:y : Send encoder pulses (y=integer) to HWC xx\n")
              << F("- HWC#xx=Speed:y : Send speed value (y=integer) to HWC xx\n")

              */

