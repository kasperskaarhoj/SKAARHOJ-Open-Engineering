/*
Copyright 2016 Kasper Skårhøj, SKAARHOJ K/S, kasper@skaarhoj.com

This file is part of the vMix UDP Client library for Arduino

The vMix UDP Client library is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by the
Free Software Foundation, either version 3 of the License, or (at your
option) any later version.

The vMix UDP Client library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with the vMix UDP Client library. If not, see http://www.gnu.org/licenses/.


IMPORTANT: If you want to use this library in your own projects and/or products,
please play a fair game and heed the license rules! See our web page for a Q&A so
you can keep a clear conscience: http://skaarhoj.com/about/licenses/


*/

#include "ClientVMixUDP.h"

/**
 * Constructor
 */
ClientVMixUDP::ClientVMixUDP() {}

/**
 * Setting up IP address for the switcher
 */
void ClientVMixUDP::begin(const IPAddress ip) {

// Set up Udp communication object:
#ifdef ESP8266
  WiFiUDP Udp;
#else
  EthernetUDP Udp;
#endif

  _Udp = Udp;

  _switcherIP = ip; // Set vMix IP address

  _lastContact = 0;
  _serialOutput = 0;

  resetCommandBundle();
}

/**
 * Initiating connection handshake to vMix UDP
 */
void ClientVMixUDP::connect() {

  _localPacketIdCounter = 0; // Init localPacketIDCounter to 0;
  _hasInitialized = false;   // Will be true after initial payload of data is resent and received well
  _initPayloadSent = false;
  _isConnected = false;    // Will be true after the initial hello-package handshakes.
  _lastContact = millis(); // Setting this, because even though we haven't had contact, it constitutes an attempt that should be responded to at least
  uint16_t portNumber = random(50100, 65300);

  _Udp.begin(portNumber);

  // Send connectString to vMix:
  if (_serialOutput) {
    Serial.print(F("Sending connect packet to VMix switcher on IP "));
    Serial.print(_switcherIP);
    Serial.print(F(" from port "));
    Serial.println(portNumber);
  }

  _wipeCleanPacketBuffer();
  _createCommandHeader(VMIX_headerCmd_HelloPacket, 12 + 8);
  _sendPacketBuffer(20);
}

/**
 * Keeps connection to the switcher alive
 * Therefore: Call this in the Arduino loop() function and make sure it gets call at least 2 times a second
 * Other recommendations might come up in the future.
 */
void ClientVMixUDP::runLoop(uint16_t delayTime) {

  static bool neverConnected = true;
  if (neverConnected) {
    neverConnected = false;
    connect();
  }

  unsigned long enterTime = millis();

  do {
    while (true) { // Iterate until UDP buffer is empty
      uint16_t packetSize = _Udp.parsePacket();

      if (_Udp.available()) {

        _Udp.read(_packetBuffer, 12);                          // Read header
        _sessionID = word(_packetBuffer[2], _packetBuffer[3]); // Not used with vMix implementation
        uint8_t headerBitmask = _packetBuffer[0] >> 3;
        _lastRemotePacketID = word(_packetBuffer[10], _packetBuffer[11]);

        uint16_t packetLength = word(_packetBuffer[0] & B00000111, _packetBuffer[1]);

        if (packetSize == packetLength) { // Just to make sure these are equal, they should be!
          _lastContact = millis();

          if (headerBitmask & VMIX_headerCmd_HelloPacket) { // Respond to "Hello" packages:
            _isConnected = true;

            _wipeCleanPacketBuffer();
            _createCommandHeader(VMIX_headerCmd_Ack, 12);
            _packetBuffer[9] = 0x03; // This seems to be what the client should send upon first request.
            _sendPacketBuffer(12);
          }

          // If a packet is 12 bytes long it indicates that all the initial information
          // has been delivered from VMIX and we can begin to answer back on every request
          if (!_initPayloadSent && packetSize == 12 && _lastRemotePacketID > 1) {
            _initPayloadSent = true;
#if VMIX_debug
            if (_serialOutput & 0x80) {
              Serial.print(F("_initPayloadSent=TRUE @rpID "));
              Serial.println(_lastRemotePacketID);
            }
#endif
          }

          if (_initPayloadSent && (headerBitmask & VMIX_headerCmd_AckRequest) && (_hasInitialized || !(headerBitmask & VMIX_headerCmd_Resend))) { // Respond to request for acknowledge	(and to resends also, whatever...
            _wipeCleanPacketBuffer();
            _createCommandHeader(VMIX_headerCmd_Ack, 12, _lastRemotePacketID);
            _sendPacketBuffer(12);

#if VMIX_debug
            if (_serialOutput & 0x80) {
              Serial.print(F("rpID: "));
              Serial.print(_lastRemotePacketID, DEC);
              Serial.print(F(", Head: 0x"));
              Serial.print(headerBitmask, HEX);
              Serial.print(F(", Len: "));
              Serial.print(packetLength, DEC);
              Serial.print(F(" bytes"));

              Serial.println(F(" - ACK!"));
            } else
#endif
                if (_serialOutput > 1) {
              Serial.print(F("rpID: "));
              Serial.print(_lastRemotePacketID, DEC);
              Serial.println(F(" - ACK!"));
            }
          } else {
#if VMIX_debug
            if (_serialOutput & 0x80) {
              Serial.print(F("rpID: "));
              Serial.print(_lastRemotePacketID, DEC);
              Serial.print(F(", Head: 0x"));
              Serial.print(headerBitmask, HEX);
              Serial.print(F(", Len: "));
              Serial.print(packetLength, DEC);
              Serial.println(F(" bytes"));
            } else
#endif
                if (_serialOutput > 1) {
              Serial.print(F("rpID: "));
              Serial.println(_lastRemotePacketID, DEC);
            }
          }

          if (!(headerBitmask & VMIX_headerCmd_HelloPacket) && packetLength > 12) {
            _parsePacket(packetLength);
          }
        } else {
#if VMIX_debug
          if (_serialOutput & 0x80) {
            Serial.print(F("ERROR: Packet size mismatch: "));
            Serial.print(packetSize, DEC);
            Serial.print(F(" != "));
            Serial.println(packetLength, DEC);
          }
#endif
          // Flushing:
          while (_Udp.available()) {
            _Udp.read(_packetBuffer, VMIX_packetBufferLength);
          }
        }
      } else
        break;
    }

    // After initialization, we check which packages were missed and ask for them:
    if (!_hasInitialized && _initPayloadSent) {
      _hasInitialized = true;
      if (_serialOutput) {
        Serial.println(F("VMIX _hasInitialized = TRUE"));
      }
    }
  } while (delayTime > 0 && !hasTimedOut(enterTime, delayTime));

  // If connection is gone anyway, try to reconnect:
  if (hasTimedOut(_lastContact, 5000)) {
    if (_serialOutput)
      Serial.println(F("Connection to VMIX Switcher has timed out - reconnecting!"));
    connect();
  }
}

/**
 * Returns last Remote Packet ID
 */
uint16_t ClientVMixUDP::getVMIX_lastRemotePacketId() { return _lastRemotePacketID; }

/**
 * If true, we had a response from the switcher when trying to send a hello packet.
 */
bool ClientVMixUDP::isConnected() { return _isConnected; }

/**
 * If true, the initial handshake and "stressful" information exchange has occured and now the switcher connection should be ready for operation.
 */
bool ClientVMixUDP::hasInitialized() { return _hasInitialized; }

/**************
 *
 * Buffer work
 *
 **************/

void ClientVMixUDP::_createCommandHeader(const uint8_t headerCmd, const uint16_t lengthOfData, const uint16_t remotePacketID) {

  _packetBuffer[0] = (headerCmd << 3) | (highByte(lengthOfData) & 0x07); // Command bits + length MSB
  _packetBuffer[1] = lowByte(lengthOfData);                              // length LSB

  _packetBuffer[2] = highByte(_sessionID); // Session ID
  _packetBuffer[3] = lowByte(_sessionID);  // Session ID

  _packetBuffer[4] = highByte(remotePacketID); // Remote Packet ID, MSB
  _packetBuffer[5] = lowByte(remotePacketID);  // Remote Packet ID, LSB

  if (!(headerCmd & (VMIX_headerCmd_HelloPacket | VMIX_headerCmd_Ack | VMIX_headerCmd_RequestNextAfter))) {
    _localPacketIdCounter++;

    _packetBuffer[10] = highByte(_localPacketIdCounter); // Local Packet ID, MSB
    _packetBuffer[11] = lowByte(_localPacketIdCounter);  // Local Packet ID, LSB
  }
}
void ClientVMixUDP::_sendPacketBuffer(uint8_t length) {
  _Udp.beginPacket(_switcherIP, 9910);
  _Udp.write(_packetBuffer, length);
  _Udp.endPacket();
}

/**
 * Sets all zeros in packet buffer:
 */
void ClientVMixUDP::_wipeCleanPacketBuffer() { memset(_packetBuffer, 0, VMIX_packetBufferLength); }

/**
 * Reads from UDP channel to buffer. Will fill the buffer to the max or to the size of the current segment being parsed
 * Returns false if there are no more bytes, otherwise true
 */
bool ClientVMixUDP::_readToPacketBuffer() { return _readToPacketBuffer(VMIX_packetBufferLength); }
bool ClientVMixUDP::_readToPacketBuffer(uint8_t maxBytes) {
  maxBytes = maxBytes <= VMIX_packetBufferLength ? maxBytes : VMIX_packetBufferLength;
  int remainingBytes = _cmdLength - 8 - _cmdPointer;

  if (remainingBytes > 0) {
    if (remainingBytes <= maxBytes) {
      _Udp.read(_packetBuffer, remainingBytes);
      _cmdPointer += remainingBytes;
      return false; // Returns false if finished.
    } else {
      _Udp.read(_packetBuffer, maxBytes);
      _cmdPointer += maxBytes;
      return true; // Returns true if there are still bytes to be read.
    }
  } else {
    return false;
  }
}

/**
 * If a package longer than a normal acknowledgement is received from the VMIX Switcher we must read through the contents.
 * Usually such a package contains updated state information about the mixer
 * Selected information is extracted in this function and transferred to internal variables in this library.
 */
void ClientVMixUDP::_parsePacket(uint16_t packetLength) {

  // If packet is more than an ACK packet (= if its longer than 12 bytes header), lets parse it:
  uint16_t indexPointer = 12; // 12 bytes has already been read from the packet...
  while (indexPointer < packetLength) {

    // Read the length of segment (first word):
    _Udp.read(_packetBuffer, 8);
    _cmdLength = word(_packetBuffer[0], _packetBuffer[1]);
    _cmdPointer = 0;

    // Get the "command string", basically this is the 4 char variable name in the VMIX memory holding the various state values of the system:
    char cmdStr[] = {_packetBuffer[4], _packetBuffer[5], _packetBuffer[6], _packetBuffer[7], '\0'};

    // If length of segment larger than 8 (should always be...!)
    if (_cmdLength > 8) {
      _parseGetCommands(cmdStr);

      while (_readToPacketBuffer()) {
      } // Empty, if not done yet.
      indexPointer += _cmdLength;
    } else {
      indexPointer = 2000;
#if VMIX_debug
      if (_serialOutput & 0x80)
        Serial.println(F("Bad CMD length, flushing..."));
#endif

      // Flushing the buffer:
      while (_Udp.available()) {
        _Udp.read(_packetBuffer, VMIX_packetBufferLength);
      }
    }
  }
}

void ClientVMixUDP::_prepareCommandPacket(const char *cmdString, uint8_t cmdBytes, bool indexMatch) {

  // First, in case of a command bundle, check if indexes are different OR if it's an entirely different command, then increase offset to accommodate new command:
  if (_cBundle) {
    if (_returnPacketLength > 0 && (!indexMatch || strncmp_P((char *)(_packetBuffer + 12 + _cBBO + 4), cmdString, 4))) {
      _cBBO = _returnPacketLength - 12;
    }
  } else {
    _wipeCleanPacketBuffer(); // For command bundles, this is already done...
  }

  _returnPacketLength = 12 + _cBBO + (4 + 4 + cmdBytes);

  // Because we increased length of command, we need to check for buffer overflow:
  if (_returnPacketLength > VMIX_packetBufferLength) {
    Serial.println(F("FATAL ERROR: Packet Buffer Overflow in the VMIX Library! Too long or too many commands bundled!\n HALT"));
    while (true) {
    } // STOP!
  }

  // Copy Command String:
  if (strlen_P(cmdString) == 4) {
    strncpy_P((char *)(_packetBuffer + 12 + _cBBO + 4), cmdString, 4);
  }
#if VMIX_debug
  else
    Serial.println(F("Command Length > 4 ERROR"));
#endif

  // Command length:
  _packetBuffer[12 + _cBBO] = 0;                    // MSB - but it's always under 256, so....
  _packetBuffer[12 + 1 + _cBBO] = 4 + 4 + cmdBytes; // LSB
}

void ClientVMixUDP::_finishCommandPacket() {
  if (!_cBundle) {

    _createCommandHeader(VMIX_headerCmd_AckRequest, _returnPacketLength);
    _sendPacketBuffer(_returnPacketLength);
    _returnPacketLength = 0;

  } else {
    // Debugging info:
    /*	  for(uint8_t a=0; a<_returnPacketLength; a++)	{
                  if (_packetBuffer[a]<16)	Serial.print("0");
                  Serial.print(_packetBuffer[a], HEX);
                  Serial.print(F("-"));
          }
          Serial.println();
                */
  }
}

/**************
 *
 * Utilities from SkaarhojTools class:
 *
 **************/

/**
 * Setter method: If _serialOutput is set, the library may use Serial.print() to give away information about its operation - mostly for debugging.
 * 0= no output
 * 1= normal output (info)
 * 2= verbose
 * &0x80 (bit 7 set): verbose initial connection information
 */
void ClientVMixUDP::serialOutput(uint8_t level) { _serialOutput = level; }

/**
 * Timeout check
 */
bool ClientVMixUDP::hasTimedOut(unsigned long time, unsigned long timeout) {
  if ((unsigned long)(time + timeout) <= (unsigned long)millis()) { // This should "wrap around" if time+timout is larger than the size of unsigned-longs, right?
    return true;
  } else {
    return false;
  }
}

void ClientVMixUDP::commandBundleStart() {
  resetCommandBundle();
  _wipeCleanPacketBuffer();
  _cBundle = true;
}
void ClientVMixUDP::commandBundleEnd() {
  if (_cBundle && _returnPacketLength > 0) {

    _createCommandHeader(VMIX_headerCmd_AckRequest, _returnPacketLength);
    _sendPacketBuffer(_returnPacketLength);
    _returnPacketLength = 0;
  }
  resetCommandBundle();
}
void ClientVMixUDP::resetCommandBundle() {
  _cBundle = false;
  _cBBO = 0;
}

// *********************************
// **
// ** Implementations in ClientVMixUDP.c:
// **
// *********************************

void ClientVMixUDP::_parseGetCommands(const char *cmdStr) {
  uint8_t input, overlay, index, fader;

  long temp;

  _readToPacketBuffer(); // Default

  if (!strcmp_P(cmdStr, PSTR("_ver"))) {

#if VMIX_debug
    temp = atemvMixVersionA;
#endif
    atemvMixVersionA = word(_packetBuffer[0], _packetBuffer[1]);
#if VMIX_debug
    if ((_serialOutput == 0x80 && atemvMixVersionA != temp) || (_serialOutput == 0x81 && !hasInitialized())) {
      Serial.print(F("atemvMixVersionA = "));
      Serial.println(atemvMixVersionA);
    }
#endif

#if VMIX_debug
    temp = atemvMixVersionB;
#endif
    atemvMixVersionB = word(_packetBuffer[2], _packetBuffer[3]);
#if VMIX_debug
    if ((_serialOutput == 0x80 && atemvMixVersionB != temp) || (_serialOutput == 0x81 && !hasInitialized())) {
      Serial.print(F("atemvMixVersionB = "));
      Serial.println(atemvMixVersionB);
    }
#endif

#if VMIX_debug
    temp = atemvMixVersionC;
#endif
    atemvMixVersionC = word(_packetBuffer[4], _packetBuffer[5]);
#if VMIX_debug
    if ((_serialOutput == 0x80 && atemvMixVersionC != temp) || (_serialOutput == 0x81 && !hasInitialized())) {
      Serial.print(F("atemvMixVersionC = "));
      Serial.println(atemvMixVersionC);
    }
#endif

#if VMIX_debug
    temp = atemvMixVersionD;
#endif
    atemvMixVersionD = word(_packetBuffer[6], _packetBuffer[7]);
#if VMIX_debug
    if ((_serialOutput == 0x80 && atemvMixVersionD != temp) || (_serialOutput == 0x81 && !hasInitialized())) {
      Serial.print(F("atemvMixVersionD = "));
      Serial.println(atemvMixVersionD);
    }
#endif

  } else if (!strcmp_P(cmdStr, PSTR("_top"))) {

#if VMIX_debug
    temp = atemTopologyInputs;
#endif
    atemTopologyInputs = _packetBuffer[0];
#if VMIX_debug
    if ((_serialOutput == 0x80 && atemTopologyInputs != temp) || (_serialOutput == 0x81 && !hasInitialized())) {
      Serial.print(F("atemTopologyInputs = "));
      Serial.println(atemTopologyInputs);
    }
#endif

  } else if (!strcmp_P(cmdStr, PSTR("InPr"))) {

    input = _packetBuffer[0];
    if (input <= 99) {
      memset(atemInputPropertiesLongName[input], 0, 21);
      strncpy(atemInputPropertiesLongName[input], (char *)(_packetBuffer + 1), 20);
#if VMIX_debug
      if ((_serialOutput == 0x80 && hasInitialized()) || (_serialOutput == 0x81 && !hasInitialized())) {
        Serial.print(F("atemInputPropertiesLongName[input="));
        Serial.print(input);
        Serial.print(F("] = "));
        Serial.println(atemInputPropertiesLongName[input]);
      }
#endif

      memset(atemInputPropertiesShortName[input], 0, 5);
      strncpy(atemInputPropertiesShortName[input], (char *)(_packetBuffer + 21), 4);
#if VMIX_debug
      if ((_serialOutput == 0x80 && hasInitialized()) || (_serialOutput == 0x81 && !hasInitialized())) {
        Serial.print(F("atemInputPropertiesShortName[input="));
        Serial.print(input);
        Serial.print(F("] = "));
        Serial.println(atemInputPropertiesShortName[input]);
      }
#endif

#if VMIX_debug
      temp = atemInputPropertiesLoop[input];
#endif
      atemInputPropertiesLoop[input] = _packetBuffer[25];
#if VMIX_debug
      if ((_serialOutput == 0x80 && atemInputPropertiesLoop[input] != temp) || (_serialOutput == 0x81 && !hasInitialized())) {
        Serial.print(F("atemInputPropertiesLoop[input="));
        Serial.print(input);
        Serial.print(F("] = "));
        Serial.println(atemInputPropertiesLoop[input]);
      }
#endif

#if VMIX_debug
      temp = atemInputPropertiesType[input];
#endif
      atemInputPropertiesType[input] = _packetBuffer[26];
#if VMIX_debug
      if ((_serialOutput == 0x80 && atemInputPropertiesType[input] != temp) || (_serialOutput == 0x81 && !hasInitialized())) {
        Serial.print(F("atemInputPropertiesType[input="));
        Serial.print(input);
        Serial.print(F("] = "));
        Serial.println(atemInputPropertiesType[input]);
      }
#endif

#if VMIX_debug
      temp = atemInputPropertiesState[input];
#endif
      atemInputPropertiesState[input] = _packetBuffer[27];
#if VMIX_debug
      if ((_serialOutput == 0x80 && atemInputPropertiesState[input] != temp) || (_serialOutput == 0x81 && !hasInitialized())) {
        Serial.print(F("atemInputPropertiesState[input="));
        Serial.print(input);
        Serial.print(F("] = "));
        Serial.println(atemInputPropertiesState[input]);
      }
#endif

#if VMIX_debug
      temp = atemInputPropertiesDuration[input];
#endif
      atemInputPropertiesDuration[input] = word(_packetBuffer[28], _packetBuffer[29]);
#if VMIX_debug
      if ((_serialOutput == 0x80 && atemInputPropertiesDuration[input] != temp) || (_serialOutput == 0x81 && !hasInitialized())) {
        Serial.print(F("atemInputPropertiesDuration[input="));
        Serial.print(input);
        Serial.print(F("] = "));
        Serial.println(atemInputPropertiesDuration[input]);
      }
#endif
    }
  } else if (!strcmp_P(cmdStr, PSTR("AuPr"))) {

    input = _packetBuffer[0];
    if (input <= 99) {
#if VMIX_debug
      temp = atemAudioPropertiesSolo[input];
#endif
      atemAudioPropertiesSolo[input] = _packetBuffer[1];
#if VMIX_debug
      if ((_serialOutput == 0x80 && atemAudioPropertiesSolo[input] != temp) || (_serialOutput == 0x81 && !hasInitialized())) {
        Serial.print(F("atemAudioPropertiesSolo[input="));
        Serial.print(input);
        Serial.print(F("] = "));
        Serial.println(atemAudioPropertiesSolo[input]);
      }
#endif

#if VMIX_debug
      temp = atemAudioPropertiesBalance[input];
#endif
      atemAudioPropertiesBalance[input] = word(_packetBuffer[2], _packetBuffer[3]);
#if VMIX_debug
      if ((_serialOutput == 0x80 && atemAudioPropertiesBalance[input] != temp) || (_serialOutput == 0x81 && !hasInitialized())) {
        Serial.print(F("atemAudioPropertiesBalance[input="));
        Serial.print(input);
        Serial.print(F("] = "));
        Serial.println(atemAudioPropertiesBalance[input]);
      }
#endif

#if VMIX_debug
      temp = atemAudioPropertiesVolume[input];
#endif
      atemAudioPropertiesVolume[input] = _packetBuffer[4];
#if VMIX_debug
      if ((_serialOutput == 0x80 && atemAudioPropertiesVolume[input] != temp) || (_serialOutput == 0x81 && !hasInitialized())) {
        Serial.print(F("atemAudioPropertiesVolume[input="));
        Serial.print(input);
        Serial.print(F("] = "));
        Serial.println(atemAudioPropertiesVolume[input]);
      }
#endif

#if VMIX_debug
      temp = atemAudioPropertiesMuted[input];
#endif
      atemAudioPropertiesMuted[input] = _packetBuffer[5];
#if VMIX_debug
      if ((_serialOutput == 0x80 && atemAudioPropertiesMuted[input] != temp) || (_serialOutput == 0x81 && !hasInitialized())) {
        Serial.print(F("atemAudioPropertiesMuted[input="));
        Serial.print(input);
        Serial.print(F("] = "));
        Serial.println(atemAudioPropertiesMuted[input]);
      }
#endif
    }
  } else if (!strcmp_P(cmdStr, PSTR("AuLv"))) {

    input = _packetBuffer[0];
    if (input <= 99) {
#if VMIX_debug
      temp = atemAudioLevelsLeft[input];
#endif
      atemAudioLevelsLeft[input] = word(_packetBuffer[1], _packetBuffer[2]);
#if VMIX_debug
      if ((_serialOutput == 0x80 && atemAudioLevelsLeft[input] != temp) || (_serialOutput == 0x81 && !hasInitialized())) {
        Serial.print(F("atemAudioLevelsLeft[input="));
        Serial.print(input);
        Serial.print(F("] = "));
        Serial.println(atemAudioLevelsLeft[input]);
      }
#endif

#if VMIX_debug
      temp = atemAudioLevelsRight[input];
#endif
      atemAudioLevelsRight[input] = word(_packetBuffer[3], _packetBuffer[4]);
#if VMIX_debug
      if ((_serialOutput == 0x80 && atemAudioLevelsRight[input] != temp) || (_serialOutput == 0x81 && !hasInitialized())) {
        Serial.print(F("atemAudioLevelsRight[input="));
        Serial.print(input);
        Serial.print(F("] = "));
        Serial.println(atemAudioLevelsRight[input]);
      }
#endif
    }
  } else if (!strcmp_P(cmdStr, PSTR("MAuP"))) {

#if VMIX_debug
    temp = atemMasterAudioPropertiesVolume;
#endif
    atemMasterAudioPropertiesVolume = _packetBuffer[0];
#if VMIX_debug
    if ((_serialOutput == 0x80 && atemMasterAudioPropertiesVolume != temp) || (_serialOutput == 0x81 && !hasInitialized())) {
      Serial.print(F("atemMasterAudioPropertiesVolume = "));
      Serial.println(atemMasterAudioPropertiesVolume);
    }
#endif

#if VMIX_debug
    temp = atemMasterAudioPropertiesHeadphoneVolume;
#endif
    atemMasterAudioPropertiesHeadphoneVolume = _packetBuffer[1];
#if VMIX_debug
    if ((_serialOutput == 0x80 && atemMasterAudioPropertiesHeadphoneVolume != temp) || (_serialOutput == 0x81 && !hasInitialized())) {
      Serial.print(F("atemMasterAudioPropertiesHeadphoneVolume = "));
      Serial.println(atemMasterAudioPropertiesHeadphoneVolume);
    }
#endif

#if VMIX_debug
    temp = atemMasterAudioPropertiesMuted;
#endif
    atemMasterAudioPropertiesMuted = _packetBuffer[2];
#if VMIX_debug
    if ((_serialOutput == 0x80 && atemMasterAudioPropertiesMuted != temp) || (_serialOutput == 0x81 && !hasInitialized())) {
      Serial.print(F("atemMasterAudioPropertiesMuted = "));
      Serial.println(atemMasterAudioPropertiesMuted);
    }
#endif

  } else if (!strcmp_P(cmdStr, PSTR("MAuL"))) {

#if VMIX_debug
    temp = atemMasterAudioLevelsLeft;
#endif
    atemMasterAudioLevelsLeft = word(_packetBuffer[0], _packetBuffer[1]);
#if VMIX_debug
    if ((_serialOutput == 0x80 && atemMasterAudioLevelsLeft != temp) || (_serialOutput == 0x81 && !hasInitialized())) {
      Serial.print(F("atemMasterAudioLevelsLeft = "));
      Serial.println(atemMasterAudioLevelsLeft);
    }
#endif

#if VMIX_debug
    temp = atemMasterAudioLevelsRight;
#endif
    atemMasterAudioLevelsRight = word(_packetBuffer[2], _packetBuffer[3]);
#if VMIX_debug
    if ((_serialOutput == 0x80 && atemMasterAudioLevelsRight != temp) || (_serialOutput == 0x81 && !hasInitialized())) {
      Serial.print(F("atemMasterAudioLevelsRight = "));
      Serial.println(atemMasterAudioLevelsRight);
    }
#endif

  } else if (!strcmp_P(cmdStr, PSTR("InPs"))) {

    input = _packetBuffer[0];
    if (input <= 99) {
#if VMIX_debug
      temp = atemInputPosition[input];
#endif
      atemInputPosition[input] = word(_packetBuffer[1], _packetBuffer[2]);
#if VMIX_debug
      if ((_serialOutput == 0x80 && atemInputPosition[input] != temp) || (_serialOutput == 0x81 && !hasInitialized())) {
        Serial.print(F("atemInputPosition[input="));
        Serial.print(input);
        Serial.print(F("] = "));
        Serial.println(atemInputPosition[input]);
      }
#endif
    }
  } else if (!strcmp_P(cmdStr, PSTR("ActI"))) {

#if VMIX_debug
    temp = atemActiveInput;
#endif
    atemActiveInput = _packetBuffer[0];
#if VMIX_debug
    if ((_serialOutput == 0x80 && atemActiveInput != temp) || (_serialOutput == 0x81 && !hasInitialized())) {
      Serial.print(F("atemActiveInput = "));
      Serial.println(atemActiveInput);
    }
#endif

  } else if (!strcmp_P(cmdStr, PSTR("PrvI"))) {

#if VMIX_debug
    temp = atemPreviewInput;
#endif
    atemPreviewInput = _packetBuffer[0];
#if VMIX_debug
    if ((_serialOutput == 0x80 && atemPreviewInput != temp) || (_serialOutput == 0x81 && !hasInitialized())) {
      Serial.print(F("atemPreviewInput = "));
      Serial.println(atemPreviewInput);
    }
#endif

  } else if (!strcmp_P(cmdStr, PSTR("OlAc"))) {

#if VMIX_debug
    temp = atemOverlayActiveOverlay;
#endif
    atemOverlayActiveOverlay = _packetBuffer[0];
#if VMIX_debug
    if ((_serialOutput == 0x80 && atemOverlayActiveOverlay != temp) || (_serialOutput == 0x81 && !hasInitialized())) {
      Serial.print(F("atemOverlayActiveOverlay = "));
      Serial.println(atemOverlayActiveOverlay);
    }
#endif

  } else if (!strcmp_P(cmdStr, PSTR("FtbS"))) {

#if VMIX_debug
    temp = atemFadeToBlackActive;
#endif
    atemFadeToBlackActive = _packetBuffer[0];
#if VMIX_debug
    if ((_serialOutput == 0x80 && atemFadeToBlackActive != temp) || (_serialOutput == 0x81 && !hasInitialized())) {
      Serial.print(F("atemFadeToBlackActive = "));
      Serial.println(atemFadeToBlackActive);
    }
#endif

  } else if (!strcmp_P(cmdStr, PSTR("RecS"))) {

#if VMIX_debug
    temp = atemRecordActive;
#endif
    atemRecordActive = _packetBuffer[0];
#if VMIX_debug
    if ((_serialOutput == 0x80 && atemRecordActive != temp) || (_serialOutput == 0x81 && !hasInitialized())) {
      Serial.print(F("atemRecordActive = "));
      Serial.println(atemRecordActive);
    }
#endif

  } else if (!strcmp_P(cmdStr, PSTR("StrS"))) {

#if VMIX_debug
    temp = atemStreamActive;
#endif
    atemStreamActive = _packetBuffer[0];
#if VMIX_debug
    if ((_serialOutput == 0x80 && atemStreamActive != temp) || (_serialOutput == 0x81 && !hasInitialized())) {
      Serial.print(F("atemStreamActive = "));
      Serial.println(atemStreamActive);
    }
#endif

  } else if (!strcmp_P(cmdStr, PSTR("RplS"))) {

#if VMIX_debug
    temp = atemReplayActive;
#endif
    atemReplayActive = _packetBuffer[0];
#if VMIX_debug
    if ((_serialOutput == 0x80 && atemReplayActive != temp) || (_serialOutput == 0x81 && !hasInitialized())) {
      Serial.print(F("atemReplayActive = "));
      Serial.println(atemReplayActive);
    }
#endif

  } else if (!strcmp_P(cmdStr, PSTR("XaVa"))) {

    input = _packetBuffer[0];
    index = _packetBuffer[1];
    if (input <= 99 && index <= 9) {
#if VMIX_debug
      temp = atemXamlValue[input][index];
#endif
      atemXamlValue[input][index] = word(_packetBuffer[2], _packetBuffer[3]);
#if VMIX_debug
      if ((_serialOutput == 0x80 && atemXamlValue[input][index] != temp) || (_serialOutput == 0x81 && !hasInitialized())) {
        Serial.print(F("atemXamlValue[input="));
        Serial.print(input);
        Serial.print(F("][index="));
        Serial.print(index);
        Serial.print(F("] = "));
        Serial.println(atemXamlValue[input][index]);
      }
#endif
    }
  } else if (!strcmp_P(cmdStr, PSTR("TrSt"))) {

    fader = _packetBuffer[0];
    if (fader <= 3) {
#if VMIX_debug
      temp = atemTransitionStyleStyle[fader];
#endif
      atemTransitionStyleStyle[fader] = _packetBuffer[1];
#if VMIX_debug
      if ((_serialOutput == 0x80 && atemTransitionStyleStyle[fader] != temp) || (_serialOutput == 0x81 && !hasInitialized())) {
        Serial.print(F("atemTransitionStyleStyle[fader="));
        Serial.print(fader);
        Serial.print(F("] = "));
        Serial.println(atemTransitionStyleStyle[fader]);
      }
#endif

#if VMIX_debug
      temp = atemTransitionStyleDuration[fader];
#endif
      atemTransitionStyleDuration[fader] = word(_packetBuffer[2], _packetBuffer[3]);
#if VMIX_debug
      if ((_serialOutput == 0x80 && atemTransitionStyleDuration[fader] != temp) || (_serialOutput == 0x81 && !hasInitialized())) {
        Serial.print(F("atemTransitionStyleDuration[fader="));
        Serial.print(fader);
        Serial.print(F("] = "));
        Serial.println(atemTransitionStyleDuration[fader]);
      }
#endif
    }
  } else if (!strcmp_P(cmdStr, PSTR("TrPs"))) {

#if VMIX_debug
    temp = atemTransitionInTransition;
#endif
    atemTransitionInTransition = _packetBuffer[0];
#if VMIX_debug
    if ((_serialOutput == 0x80 && atemTransitionInTransition != temp) || (_serialOutput == 0x81 && !hasInitialized())) {
      Serial.print(F("atemTransitionInTransition = "));
      Serial.println(atemTransitionInTransition);
    }
#endif

#if VMIX_debug
    temp = atemTransitionPosition;
#endif
    atemTransitionPosition = _packetBuffer[1];
#if VMIX_debug
    if ((_serialOutput == 0x80 && atemTransitionPosition != temp) || (_serialOutput == 0x81 && !hasInitialized())) {
      Serial.print(F("atemTransitionPosition = "));
      Serial.println(atemTransitionPosition);
    }
#endif

  } else {
  }
}

/**
 * Get vMix Version; A
 */
uint16_t ClientVMixUDP::getvMixVersionA() { return atemvMixVersionA; }

/**
 * Get vMix Version; B
 */
uint16_t ClientVMixUDP::getvMixVersionB() { return atemvMixVersionB; }

/**
 * Get vMix Version; C
 */
uint16_t ClientVMixUDP::getvMixVersionC() { return atemvMixVersionC; }

/**
 * Get vMix Version; D
 */
uint16_t ClientVMixUDP::getvMixVersionD() { return atemvMixVersionD; }

/**
 * Get Topology; Inputs
 */
uint8_t ClientVMixUDP::getTopologyInputs() { return atemTopologyInputs; }

/**
 * Get Input Properties; Long Name
 * input 	0-99: 1-100 Input Index
 */
char *ClientVMixUDP::getInputPropertiesLongName(uint8_t input) { return atemInputPropertiesLongName[input]; }

/**
 * Get Input Properties; Short Name
 * input 	0-99: 1-100 Input Index
 */
char *ClientVMixUDP::getInputPropertiesShortName(uint8_t input) { return atemInputPropertiesShortName[input]; }

/**
 * Get Input Properties; Loop
 * input 	0-99: 1-100 Input Index
 */
bool ClientVMixUDP::getInputPropertiesLoop(uint8_t input) { return atemInputPropertiesLoop[input]; }

/**
 * Get Input Properties; Type
 * input 	0-99: 1-100 Input Index
 */
uint8_t ClientVMixUDP::getInputPropertiesType(uint8_t input) { return atemInputPropertiesType[input]; }

/**
 * Get Input Properties; State
 * input 	0-99: 1-100 Input Index
 */
uint8_t ClientVMixUDP::getInputPropertiesState(uint8_t input) { return atemInputPropertiesState[input]; }

/**
 * Get Input Properties; Duration
 * input 	0-99: 1-100 Input Index
 */
uint16_t ClientVMixUDP::getInputPropertiesDuration(uint8_t input) { return atemInputPropertiesDuration[input]; }

/**
 * Set Input Properties; Long Name
 * input 	0-99: 1-100 Input Index
 * longName
 */
void ClientVMixUDP::setInputPropertiesLongName(uint8_t input, char *longName) {

  _prepareCommandPacket(PSTR("CInP"), 27, (_packetBuffer[12 + _cBBO + 4 + 4 + 1] == input));

  // Set Mask: 1
  _packetBuffer[12 + _cBBO + 4 + 4 + 0] |= 1;

  _packetBuffer[12 + _cBBO + 4 + 4 + 1] = input;

  strncpy((char *)(_packetBuffer + 12 + 4 + 4 + 2), longName, 20);

  _finishCommandPacket();
}

/**
 * Set Input Properties; Short Name
 * input 	0-99: 1-100 Input Index
 * shortName
 */
void ClientVMixUDP::setInputPropertiesShortName(uint8_t input, char *shortName) {

  _prepareCommandPacket(PSTR("CInP"), 27, (_packetBuffer[12 + _cBBO + 4 + 4 + 1] == input));

  // Set Mask: 2
  _packetBuffer[12 + _cBBO + 4 + 4 + 0] |= 2;

  _packetBuffer[12 + _cBBO + 4 + 4 + 1] = input;

  strncpy((char *)(_packetBuffer + 12 + 4 + 4 + 22), shortName, 4);

  _finishCommandPacket();
}

/**
 * Set Input Properties; Loop
 * input 	0-99: 1-100 Input Index
 * loop 	Bit 0: On/Off
 */
void ClientVMixUDP::setInputPropertiesLoop(uint8_t input, bool loop) {

  _prepareCommandPacket(PSTR("CInP"), 27, (_packetBuffer[12 + _cBBO + 4 + 4 + 1] == input));

  // Set Mask: 4
  _packetBuffer[12 + _cBBO + 4 + 4 + 0] |= 4;

  _packetBuffer[12 + _cBBO + 4 + 4 + 1] = input;

  _packetBuffer[12 + _cBBO + 4 + 4 + 26] = loop;

  _finishCommandPacket();
}

/**
 * Get Audio Properties; Solo
 * input 	0-99: 1-100 Input Index
 */
bool ClientVMixUDP::getAudioPropertiesSolo(uint8_t input) { return atemAudioPropertiesSolo[input]; }

/**
 * Get Audio Properties; Balance
 * input 	0-99: 1-100 Input Index
 */
uint16_t ClientVMixUDP::getAudioPropertiesBalance(uint8_t input) { return atemAudioPropertiesBalance[input]; }

/**
 * Get Audio Properties; Volume
 * input 	0-99: 1-100 Input Index
 */
uint8_t ClientVMixUDP::getAudioPropertiesVolume(uint8_t input) { return atemAudioPropertiesVolume[input]; }

/**
 * Get Audio Properties; Muted
 * input 	0-99: 1-100 Input Index
 */
bool ClientVMixUDP::getAudioPropertiesMuted(uint8_t input) { return atemAudioPropertiesMuted[input]; }

/**
 * Set Audio Properties; Solo
 * input 	0-99: 1-100 Input Index
 * solo 	Bit 0: On/Off
 */
void ClientVMixUDP::setAudioPropertiesSolo(uint8_t input, bool solo) {

  _prepareCommandPacket(PSTR("SAuP"), 7, (_packetBuffer[12 + _cBBO + 4 + 4 + 1] == input));

  // Set Mask: 1
  _packetBuffer[12 + _cBBO + 4 + 4 + 0] |= 1;

  _packetBuffer[12 + _cBBO + 4 + 4 + 1] = input;

  _packetBuffer[12 + _cBBO + 4 + 4 + 2] = solo;

  _finishCommandPacket();
}

/**
 * Set Audio Properties; Balance
 * input 	0-99: 1-100 Input Index
 * balance 	-100-100: Balance
 */
void ClientVMixUDP::setAudioPropertiesBalance(uint8_t input, uint16_t balance) {

  _prepareCommandPacket(PSTR("SAuP"), 7, (_packetBuffer[12 + _cBBO + 4 + 4 + 1] == input));

  // Set Mask: 2
  _packetBuffer[12 + _cBBO + 4 + 4 + 0] |= 2;

  _packetBuffer[12 + _cBBO + 4 + 4 + 1] = input;

  _packetBuffer[12 + _cBBO + 4 + 4 + 3] = highByte(balance);
  _packetBuffer[12 + _cBBO + 4 + 4 + 4] = lowByte(balance);

  _finishCommandPacket();
}

/**
 * Set Audio Properties; Volume
 * input 	0-99: 1-100 Input Index
 * volume 	0-100: Volume
 */
void ClientVMixUDP::setAudioPropertiesVolume(uint8_t input, uint8_t volume) {

  _prepareCommandPacket(PSTR("SAuP"), 7, (_packetBuffer[12 + _cBBO + 4 + 4 + 1] == input));

  // Set Mask: 4
  _packetBuffer[12 + _cBBO + 4 + 4 + 0] |= 4;

  _packetBuffer[12 + _cBBO + 4 + 4 + 1] = input;

  _packetBuffer[12 + _cBBO + 4 + 4 + 5] = volume;

  _finishCommandPacket();
}

/**
 * Set Audio Properties; Muted
 * input 	0-99: 1-100 Input Index
 * muted 	Bit 0: On/Off
 */
void ClientVMixUDP::setAudioPropertiesMuted(uint8_t input, bool muted) {

  _prepareCommandPacket(PSTR("SAuP"), 7, (_packetBuffer[12 + _cBBO + 4 + 4 + 1] == input));

  // Set Mask: 8
  _packetBuffer[12 + _cBBO + 4 + 4 + 0] |= 8;

  _packetBuffer[12 + _cBBO + 4 + 4 + 1] = input;

  _packetBuffer[12 + _cBBO + 4 + 4 + 6] = muted;

  _finishCommandPacket();
}

/**
 * Get Audio Levels; Left
 * input 	0-99: 1-100 Input Index
 */
uint16_t ClientVMixUDP::getAudioLevelsLeft(uint8_t input) { return atemAudioLevelsLeft[input]; }

/**
 * Get Audio Levels; Right
 * input 	0-99: 1-100 Input Index
 */
uint16_t ClientVMixUDP::getAudioLevelsRight(uint8_t input) { return atemAudioLevelsRight[input]; }

/**
 * Get Master Audio Properties; Volume
 */
uint8_t ClientVMixUDP::getMasterAudioPropertiesVolume() { return atemMasterAudioPropertiesVolume; }

/**
 * Get Master Audio Properties; Headphone Volume
 */
uint8_t ClientVMixUDP::getMasterAudioPropertiesHeadphoneVolume() { return atemMasterAudioPropertiesHeadphoneVolume; }

/**
 * Get Master Audio Properties; Muted
 */
bool ClientVMixUDP::getMasterAudioPropertiesMuted() { return atemMasterAudioPropertiesMuted; }

/**
 * Set Master Audio Properties; Volume
 * volume 	0-100: Volume
 */
void ClientVMixUDP::setMasterAudioPropertiesVolume(uint8_t volume) {

  _prepareCommandPacket(PSTR("MSAP"), 4);

  // Set Mask: 1
  _packetBuffer[12 + _cBBO + 4 + 4 + 0] |= 1;

  _packetBuffer[12 + _cBBO + 4 + 4 + 1] = volume;

  _finishCommandPacket();
}

/**
 * Set Master Audio Properties; Headphone Volume
 * headphoneVolume 	0-100: Volume
 */
void ClientVMixUDP::setMasterAudioPropertiesHeadphoneVolume(uint8_t headphoneVolume) {

  _prepareCommandPacket(PSTR("MSAP"), 4);

  // Set Mask: 2
  _packetBuffer[12 + _cBBO + 4 + 4 + 0] |= 2;

  _packetBuffer[12 + _cBBO + 4 + 4 + 2] = headphoneVolume;

  _finishCommandPacket();
}

/**
 * Set Master Audio Properties; Muted
 * muted 	Bit 0: On/Off
 */
void ClientVMixUDP::setMasterAudioPropertiesMuted(bool muted) {

  _prepareCommandPacket(PSTR("MSAP"), 4);

  // Set Mask: 4
  _packetBuffer[12 + _cBBO + 4 + 4 + 0] |= 4;

  _packetBuffer[12 + _cBBO + 4 + 4 + 3] = muted;

  _finishCommandPacket();
}

/**
 * Get Master Audio Levels; Left
 */
uint16_t ClientVMixUDP::getMasterAudioLevelsLeft() { return atemMasterAudioLevelsLeft; }

/**
 * Get Master Audio Levels; Right
 */
uint16_t ClientVMixUDP::getMasterAudioLevelsRight() { return atemMasterAudioLevelsRight; }

/**
 * Get Input Position; Position
 * input 	0-99: 1-100 Input Index
 */
uint16_t ClientVMixUDP::getInputPosition(uint8_t input) { return atemInputPosition[input]; }

/**
 * Get Active Input; Input
 */
uint8_t ClientVMixUDP::getActiveInput() { return atemActiveInput; }

/**
 * Set Active Input; Input
 * input 	0-99: 1-100 Input Index
 */
void ClientVMixUDP::setActiveInput(uint8_t input) {

  _prepareCommandPacket(PSTR("CAcI"), 1);

  _packetBuffer[12 + _cBBO + 4 + 4 + 0] = input;

  _finishCommandPacket();
}

/**
 * Get Preview Input; Input
 */
uint8_t ClientVMixUDP::getPreviewInput() { return atemPreviewInput; }

/**
 * Set Preview Input; Input
 * input 	0-99: 1-100 Input Index
 */
void ClientVMixUDP::setPreviewInput(uint8_t input) {

  _prepareCommandPacket(PSTR("CPvI"), 1);

  _packetBuffer[12 + _cBBO + 4 + 4 + 0] = input;

  _finishCommandPacket();
}

/**
 * Get Overlay Active; Overlay
 */
uint8_t ClientVMixUDP::getOverlayActiveOverlay() { return atemOverlayActiveOverlay; }

/**
 * Set Overlay Active; Input
 * overlay 	0-5:1-6 Overlay
 * input 	0-99: 1-100 Input Index
 */
void ClientVMixUDP::setOverlayActiveInput(uint8_t overlay, uint8_t input) {

  _prepareCommandPacket(PSTR("COlA"), 2, (_packetBuffer[12 + _cBBO + 4 + 4 + 0] == overlay));

  _packetBuffer[12 + _cBBO + 4 + 4 + 0] = overlay;

  _packetBuffer[12 + _cBBO + 4 + 4 + 1] = input;

  _finishCommandPacket();
}

/**
 * Set Cut; Action
 * action 	Bit 0: On/Off
 */
void ClientVMixUDP::performCutAction(bool action) {

  _prepareCommandPacket(PSTR("DCut"), 1);

  _packetBuffer[12 + _cBBO + 4 + 4 + 0] = action;

  _finishCommandPacket();
}

/**
 * Set Fade; Fader
 * fader 	0-3: Fader 1-4
 */
void ClientVMixUDP::performFadeFader(uint8_t fader) {

  _prepareCommandPacket(PSTR("DFad"), 1);

  _packetBuffer[12 + _cBBO + 4 + 4 + 0] = fader;

  _finishCommandPacket();
}

/**
 * Set Fade-To-Black; Action
 * action 	Bit 0: On/Off
 */
void ClientVMixUDP::performFadeToBlackAction(bool action) {

  _prepareCommandPacket(PSTR("FtbA"), 1);

  _packetBuffer[12 + _cBBO + 4 + 4 + 0] = action;

  _finishCommandPacket();
}

/**
 * Get Fade-To-Black; Active
 */
bool ClientVMixUDP::getFadeToBlackActive() { return atemFadeToBlackActive; }

/**
 * Set Record; Action
 * action 	Bit 0: On/Off
 */
void ClientVMixUDP::performRecordAction(bool action) {

  _prepareCommandPacket(PSTR("RecA"), 1);

  _packetBuffer[12 + _cBBO + 4 + 4 + 0] = action;

  _finishCommandPacket();
}

/**
 * Get Record; Active
 */
bool ClientVMixUDP::getRecordActive() { return atemRecordActive; }

/**
 * Set Stream; Action
 * action 	Bit 0: On/Off
 */
void ClientVMixUDP::performStreamAction(bool action) {

  _prepareCommandPacket(PSTR("StrA"), 1);

  _packetBuffer[12 + _cBBO + 4 + 4 + 0] = action;

  _finishCommandPacket();
}

/**
 * Get Stream; Active
 */
bool ClientVMixUDP::getStreamActive() { return atemStreamActive; }

/**
 * Set Replay; Action
 * action 	Bit 0: On/Off
 */
void ClientVMixUDP::performReplayAction(bool action) {

  _prepareCommandPacket(PSTR("RplA"), 1);

  _packetBuffer[12 + _cBBO + 4 + 4 + 0] = action;

  _finishCommandPacket();
}

/**
 * Get Replay; Active
 */
bool ClientVMixUDP::getReplayActive() { return atemReplayActive; }

/**
 * Get Xaml Values; Value
 * input 	0-99: 1-100 Input Index
 * index 	0-9: 1-10 Value Index
 */
uint16_t ClientVMixUDP::getXamlValue(uint8_t input, uint8_t index) { return atemXamlValue[input][index]; }

/**
 * Set Xaml Values; Value
 * input 	0-30: 1-30 Source Index
 * index 	0-5: 1-6 Value Index
 * value
 */
void ClientVMixUDP::setXamlValue(uint8_t input, uint8_t index, uint16_t value) {

  _prepareCommandPacket(PSTR("SXaV"), 4, (_packetBuffer[12 + _cBBO + 4 + 4 + 0] == input) && (_packetBuffer[12 + _cBBO + 4 + 4 + 1] == index));

  _packetBuffer[12 + _cBBO + 4 + 4 + 0] = input;

  _packetBuffer[12 + _cBBO + 4 + 4 + 1] = index;

  _packetBuffer[12 + _cBBO + 4 + 4 + 2] = highByte(value);
  _packetBuffer[12 + _cBBO + 4 + 4 + 3] = lowByte(value);

  _finishCommandPacket();
}

/**
 * Set Count Down; Mode
 * input 	0-30: 1-30 Source Index
 * index 	0-5: 1-6 Value Index
 * mode 	0: Start, 1: Pause, 2: Stop+Reset
 */
void ClientVMixUDP::setCountDownMode(uint8_t input, uint8_t index, uint8_t mode) {

  _prepareCommandPacket(PSTR("CntD"), 3, (_packetBuffer[12 + _cBBO + 4 + 4 + 0] == input) && (_packetBuffer[12 + _cBBO + 4 + 4 + 1] == index));

  _packetBuffer[12 + _cBBO + 4 + 4 + 0] = input;

  _packetBuffer[12 + _cBBO + 4 + 4 + 1] = index;

  _packetBuffer[12 + _cBBO + 4 + 4 + 2] = mode;

  _finishCommandPacket();
}

/**
 * Set Replay Properties; Seconds
 * seconds
 */
void ClientVMixUDP::setReplayPropertiesSeconds(uint16_t seconds) {

  _prepareCommandPacket(PSTR("RplP"), 5);

  // Set Mask: 1
  _packetBuffer[12 + _cBBO + 4 + 4 + 0] |= 1;

  _packetBuffer[12 + _cBBO + 4 + 4 + 1] = highByte(seconds);
  _packetBuffer[12 + _cBBO + 4 + 4 + 2] = lowByte(seconds);

  _finishCommandPacket();
}

/**
 * Set Replay Properties; Speed
 * speed 	-1000-1000: -1-1x speed
 */
void ClientVMixUDP::setReplayPropertiesSpeed(uint16_t speed) {

  _prepareCommandPacket(PSTR("RplP"), 5);

  // Set Mask: 2
  _packetBuffer[12 + _cBBO + 4 + 4 + 0] |= 2;

  _packetBuffer[12 + _cBBO + 4 + 4 + 3] = highByte(speed);
  _packetBuffer[12 + _cBBO + 4 + 4 + 4] = lowByte(speed);

  _finishCommandPacket();
}

/**
 * Get Transition Style; Style
 * fader 	0-3: Fader 1-4
 */
uint8_t ClientVMixUDP::getTransitionStyleStyle(uint8_t fader) { return atemTransitionStyleStyle[fader]; }

/**
 * Get Transition Style; Duration
 * fader 	0-3: Fader 1-4
 */
uint16_t ClientVMixUDP::getTransitionStyleDuration(uint8_t fader) { return atemTransitionStyleDuration[fader]; }

/**
 * Set Transition Style; Style
 * fader 	0-3: Fader 1-4
 * style 	0: Fade, 1: Merge, 2: Wipe, 3: CubeZoom
 */
void ClientVMixUDP::setTransitionStyleStyle(uint8_t fader, uint8_t style) {

  _prepareCommandPacket(PSTR("CTrS"), 5, (_packetBuffer[12 + _cBBO + 4 + 4 + 1] == fader));

  // Set Mask: 1
  _packetBuffer[12 + _cBBO + 4 + 4 + 0] |= 1;

  _packetBuffer[12 + _cBBO + 4 + 4 + 1] = fader;

  _packetBuffer[12 + _cBBO + 4 + 4 + 2] = style;

  _finishCommandPacket();
}

/**
 * Set Transition Style; Duration
 * fader 	0-3: Fader 1-4
 * duration
 */
void ClientVMixUDP::setTransitionStyleDuration(uint8_t fader, uint16_t duration) {

  _prepareCommandPacket(PSTR("CTrS"), 5, (_packetBuffer[12 + _cBBO + 4 + 4 + 1] == fader));

  // Set Mask: 2
  _packetBuffer[12 + _cBBO + 4 + 4 + 0] |= 2;

  _packetBuffer[12 + _cBBO + 4 + 4 + 1] = fader;

  _packetBuffer[12 + _cBBO + 4 + 4 + 3] = highByte(duration);
  _packetBuffer[12 + _cBBO + 4 + 4 + 4] = lowByte(duration);

  _finishCommandPacket();
}

/**
 * Get Transition Position; In Transition
 */
bool ClientVMixUDP::getTransitionInTransition() { return atemTransitionInTransition; }

/**
 * Get Transition Position; Position
 */
uint8_t ClientVMixUDP::getTransitionPosition() { return atemTransitionPosition; }

/**
 * Set Transition Position; Position
 * position 	0-255
 */
void ClientVMixUDP::setTransitionPosition(uint8_t position) {

  _prepareCommandPacket(PSTR("CTPs"), 1);

  _packetBuffer[12 + _cBBO + 4 + 4 + 0] = position;

  _finishCommandPacket();
}

