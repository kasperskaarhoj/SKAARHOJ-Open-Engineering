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

  neverConnected = true;
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
      uint8_t input,transition,overlay,index;
      
      long temp;
      
      _readToPacketBuffer();  // Default



      
      if(!strcmp_P(cmdStr, PSTR("ActI"))) {
        
          #if VMIX_debug
          temp = vmixActiveInput;
          #endif          
          vmixActiveInput = _packetBuffer[0];
          #if VMIX_debug
          if ((_serialOutput==0x80 && vmixActiveInput!=temp) || (_serialOutput==0x81 && !hasInitialized()) || _serialOutput==0x82)  {
            Serial.print(F("vmixActiveInput = "));
            Serial.println(vmixActiveInput);
          }
          #endif
          
      } else 
      if(!strcmp_P(cmdStr, PSTR("PrvI"))) {
        
          #if VMIX_debug
          temp = vmixPreviewInput;
          #endif          
          vmixPreviewInput = _packetBuffer[0];
          #if VMIX_debug
          if ((_serialOutput==0x80 && vmixPreviewInput!=temp) || (_serialOutput==0x81 && !hasInitialized()) || _serialOutput==0x82) {
            Serial.print(F("vmixPreviewInput = "));
            Serial.println(vmixPreviewInput);
          }
          #endif
          
      } else 
      if(!strcmp_P(cmdStr, PSTR("FtbS"))) {
        
          #if VMIX_debug
          temp = vmixFadeToBlackActive;
          #endif          
          vmixFadeToBlackActive = _packetBuffer[0];
          #if VMIX_debug
          if ((_serialOutput==0x80 && vmixFadeToBlackActive!=temp) || (_serialOutput==0x81 && !hasInitialized()) || _serialOutput==0x82)  {
            Serial.print(F("vmixFadeToBlackActive = "));
            Serial.println(vmixFadeToBlackActive);
          }
          #endif
          
      } else 
      if(!strcmp_P(cmdStr, PSTR("OlAc"))) {
        
        overlay = _packetBuffer[0];
        if (overlay<=5) {
          #if VMIX_debug
          temp = vmixOverlayActive[overlay];
          #endif          
          vmixOverlayActive[overlay] = _packetBuffer[1];
          #if VMIX_debug
          if ((_serialOutput==0x80 && vmixOverlayActive[overlay]!=temp) || (_serialOutput==0x81 && !hasInitialized()) || _serialOutput==0x82) {
            Serial.print(F("vmixOverlayActive[overlay=")); Serial.print(overlay); Serial.print(F("] = "));
            Serial.println(vmixOverlayActive[overlay]);
          }
          #endif
          
          #if VMIX_debug
          temp = vmixOverlayInput[overlay];
          #endif          
          vmixOverlayInput[overlay] = _packetBuffer[2];
          #if VMIX_debug
          if ((_serialOutput==0x80 && vmixOverlayInput[overlay]!=temp) || (_serialOutput==0x81 && !hasInitialized()) || _serialOutput==0x82)  {
            Serial.print(F("vmixOverlayInput[overlay=")); Serial.print(overlay); Serial.print(F("] = "));
            Serial.println(vmixOverlayInput[overlay]);
          }
          #endif
          
        }
      } else 
      if(!strcmp_P(cmdStr, PSTR("RecS"))) {
        
          #if VMIX_debug
          temp = vmixRecordActive;
          #endif          
          vmixRecordActive = _packetBuffer[0];
          #if VMIX_debug
          if ((_serialOutput==0x80 && vmixRecordActive!=temp) || (_serialOutput==0x81 && !hasInitialized()) || _serialOutput==0x82) {
            Serial.print(F("vmixRecordActive = "));
            Serial.println(vmixRecordActive);
          }
          #endif
          
      } else 
      if(!strcmp_P(cmdStr, PSTR("StrS"))) {
        
          #if VMIX_debug
          temp = vmixStreamActive;
          #endif          
          vmixStreamActive = _packetBuffer[0];
          #if VMIX_debug
          if ((_serialOutput==0x80 && vmixStreamActive!=temp) || (_serialOutput==0x81 && !hasInitialized()) || _serialOutput==0x82) {
            Serial.print(F("vmixStreamActive = "));
            Serial.println(vmixStreamActive);
          }
          #endif
          
      } else 
      if(!strcmp_P(cmdStr, PSTR("AuLv"))) {
        
        input = _packetBuffer[0];
        if (input<=99) {
          #if VMIX_debug
          temp = vmixAudioLevelsLeft[input];
          #endif          
          vmixAudioLevelsLeft[input] = (int16_t) word(_packetBuffer[1], _packetBuffer[2]);
          #if VMIX_debug
          if ((_serialOutput==0x80 && vmixAudioLevelsLeft[input]!=temp) || (_serialOutput==0x81 && !hasInitialized()) || _serialOutput==0x82) {
            Serial.print(F("vmixAudioLevelsLeft[input=")); Serial.print(input); Serial.print(F("] = "));
            Serial.println(vmixAudioLevelsLeft[input]);
          }
          #endif
          
          #if VMIX_debug
          temp = vmixAudioLevelsRight[input];
          #endif          
          vmixAudioLevelsRight[input] = (int16_t) word(_packetBuffer[3], _packetBuffer[4]);
          #if VMIX_debug
          if ((_serialOutput==0x80 && vmixAudioLevelsRight[input]!=temp) || (_serialOutput==0x81 && !hasInitialized()) || _serialOutput==0x82)  {
            Serial.print(F("vmixAudioLevelsRight[input=")); Serial.print(input); Serial.print(F("] = "));
            Serial.println(vmixAudioLevelsRight[input]);
          }
          #endif
          
        }
      } else 
      if(!strcmp_P(cmdStr, PSTR("MAuL"))) {
        
          #if VMIX_debug
          temp = vmixMasterAudioLevelsLeft;
          #endif          
          vmixMasterAudioLevelsLeft = (int16_t) word(_packetBuffer[0], _packetBuffer[1]);
          #if VMIX_debug
          if ((_serialOutput==0x80 && vmixMasterAudioLevelsLeft!=temp) || (_serialOutput==0x81 && !hasInitialized()) || _serialOutput==0x82)  {
            Serial.print(F("vmixMasterAudioLevelsLeft = "));
            Serial.println(vmixMasterAudioLevelsLeft);
          }
          #endif
          
          #if VMIX_debug
          temp = vmixMasterAudioLevelsRight;
          #endif          
          vmixMasterAudioLevelsRight = (int16_t) word(_packetBuffer[2], _packetBuffer[3]);
          #if VMIX_debug
          if ((_serialOutput==0x80 && vmixMasterAudioLevelsRight!=temp) || (_serialOutput==0x81 && !hasInitialized()) || _serialOutput==0x82) {
            Serial.print(F("vmixMasterAudioLevelsRight = "));
            Serial.println(vmixMasterAudioLevelsRight);
          }
          #endif
          
      } else 
      if(!strcmp_P(cmdStr, PSTR("InPs"))) {
        
        input = _packetBuffer[0];
        if (input<=99) {
          #if VMIX_debug
          temp = vmixInputPosition[input];
          #endif          
          vmixInputPosition[input] = word(_packetBuffer[1], _packetBuffer[2]);
          #if VMIX_debug
          if ((_serialOutput==0x80 && vmixInputPosition[input]!=temp) || (_serialOutput==0x81 && !hasInitialized()) || _serialOutput==0x82) {
            Serial.print(F("vmixInputPosition[input=")); Serial.print(input); Serial.print(F("] = "));
            Serial.println(vmixInputPosition[input]);
          }
          #endif
          
        }
      } else 
      if(!strcmp_P(cmdStr, PSTR("XaVa"))) {
        
        input = _packetBuffer[0];
        index = _packetBuffer[1];
        if (input<=99 && index<=10) {
          #if VMIX_debug
          temp = vmixXamlValue[input][index];
          #endif          
          vmixXamlValue[input][index] = (int16_t) word(_packetBuffer[2], _packetBuffer[3]);
          #if VMIX_debug
          if ((_serialOutput==0x80 && vmixXamlValue[input][index]!=temp) || (_serialOutput==0x81 && !hasInitialized()) || _serialOutput==0x82)  {
            Serial.print(F("vmixXamlValue[input=")); Serial.print(input); Serial.print(F("][index=")); Serial.print(index); Serial.print(F("] = "));
            Serial.println(vmixXamlValue[input][index]);
          }
          #endif
          
        }
      } else 
      if(!strcmp_P(cmdStr, PSTR("InPr"))) {
        
        input = _packetBuffer[0];
        if (input<=99) {
          memset(vmixInputPropertiesLongName[input],0,21);
          strncpy(vmixInputPropertiesLongName[input], (char *)(_packetBuffer+1), 20);
          #if VMIX_debug
          if ((_serialOutput==0x80 && hasInitialized()) || (_serialOutput==0x81 && !hasInitialized()))  {
            Serial.print(F("vmixInputPropertiesLongName[input=")); Serial.print(input); Serial.print(F("] = "));
            Serial.println(vmixInputPropertiesLongName[input]);
          }
          #endif
          
          memset(vmixInputPropertiesShortName[input],0,5);
          strncpy(vmixInputPropertiesShortName[input], (char *)(_packetBuffer+21), 4);
          #if VMIX_debug
          if ((_serialOutput==0x80 && hasInitialized()) || (_serialOutput==0x81 && !hasInitialized()))  {
            Serial.print(F("vmixInputPropertiesShortName[input=")); Serial.print(input); Serial.print(F("] = "));
            Serial.println(vmixInputPropertiesShortName[input]);
          }
          #endif
          
          #if VMIX_debug
          temp = vmixInputPropertiesLoop[input];
          #endif          
          vmixInputPropertiesLoop[input] = _packetBuffer[25];
          #if VMIX_debug
          if ((_serialOutput==0x80 && vmixInputPropertiesLoop[input]!=temp) || (_serialOutput==0x81 && !hasInitialized()) || _serialOutput==0x82) {
            Serial.print(F("vmixInputPropertiesLoop[input=")); Serial.print(input); Serial.print(F("] = "));
            Serial.println(vmixInputPropertiesLoop[input]);
          }
          #endif
          
          #if VMIX_debug
          temp = vmixInputPropertiesType[input];
          #endif          
          vmixInputPropertiesType[input] = _packetBuffer[26];
          #if VMIX_debug
          if ((_serialOutput==0x80 && vmixInputPropertiesType[input]!=temp) || (_serialOutput==0x81 && !hasInitialized()) || _serialOutput==0x82) {
            Serial.print(F("vmixInputPropertiesType[input=")); Serial.print(input); Serial.print(F("] = "));
            Serial.println(vmixInputPropertiesType[input]);
          }
          #endif
          
          #if VMIX_debug
          temp = vmixInputPropertiesState[input];
          #endif          
          vmixInputPropertiesState[input] = _packetBuffer[27];
          #if VMIX_debug
          if ((_serialOutput==0x80 && vmixInputPropertiesState[input]!=temp) || (_serialOutput==0x81 && !hasInitialized()) || _serialOutput==0x82)  {
            Serial.print(F("vmixInputPropertiesState[input=")); Serial.print(input); Serial.print(F("] = "));
            Serial.println(vmixInputPropertiesState[input]);
          }
          #endif
          
          #if VMIX_debug
          temp = vmixInputPropertiesDuration[input];
          #endif          
          vmixInputPropertiesDuration[input] = word(_packetBuffer[28], _packetBuffer[29]);
          #if VMIX_debug
          if ((_serialOutput==0x80 && vmixInputPropertiesDuration[input]!=temp) || (_serialOutput==0x81 && !hasInitialized()) || _serialOutput==0x82) {
            Serial.print(F("vmixInputPropertiesDuration[input=")); Serial.print(input); Serial.print(F("] = "));
            Serial.println(vmixInputPropertiesDuration[input]);
          }
          #endif
          
        }
      } else 
      if(!strcmp_P(cmdStr, PSTR("AuPr"))) {
        
        input = _packetBuffer[0];
        if (input<=99) {
          #if VMIX_debug
          temp = vmixAudioPropertiesSolo[input];
          #endif          
          vmixAudioPropertiesSolo[input] = _packetBuffer[1];
          #if VMIX_debug
          if ((_serialOutput==0x80 && vmixAudioPropertiesSolo[input]!=temp) || (_serialOutput==0x81 && !hasInitialized()) || _serialOutput==0x82) {
            Serial.print(F("vmixAudioPropertiesSolo[input=")); Serial.print(input); Serial.print(F("] = "));
            Serial.println(vmixAudioPropertiesSolo[input]);
          }
          #endif
          
          #if VMIX_debug
          temp = vmixAudioPropertiesBalance[input];
          #endif          
          vmixAudioPropertiesBalance[input] = (int16_t) word(_packetBuffer[2], _packetBuffer[3]);
          #if VMIX_debug
          if ((_serialOutput==0x80 && vmixAudioPropertiesBalance[input]!=temp) || (_serialOutput==0x81 && !hasInitialized()) || _serialOutput==0x82)  {
            Serial.print(F("vmixAudioPropertiesBalance[input=")); Serial.print(input); Serial.print(F("] = "));
            Serial.println(vmixAudioPropertiesBalance[input]);
          }
          #endif
          
          #if VMIX_debug
          temp = vmixAudioPropertiesVolume[input];
          #endif          
          vmixAudioPropertiesVolume[input] = (int16_t) word(_packetBuffer[4], _packetBuffer[5]);
          #if VMIX_debug
          if ((_serialOutput==0x80 && vmixAudioPropertiesVolume[input]!=temp) || (_serialOutput==0x81 && !hasInitialized()) || _serialOutput==0x82) {
            Serial.print(F("vmixAudioPropertiesVolume[input=")); Serial.print(input); Serial.print(F("] = "));
            Serial.println(vmixAudioPropertiesVolume[input]);
          }
          #endif
          
          #if VMIX_debug
          temp = vmixAudioPropertiesMuted[input];
          #endif          
          vmixAudioPropertiesMuted[input] = _packetBuffer[6];
          #if VMIX_debug
          if ((_serialOutput==0x80 && vmixAudioPropertiesMuted[input]!=temp) || (_serialOutput==0x81 && !hasInitialized()) || _serialOutput==0x82)  {
            Serial.print(F("vmixAudioPropertiesMuted[input=")); Serial.print(input); Serial.print(F("] = "));
            Serial.println(vmixAudioPropertiesMuted[input]);
          }
          #endif
          
          #if VMIX_debug
          temp = vmixAudioPropertiesBusses[input];
          #endif          
          vmixAudioPropertiesBusses[input] = _packetBuffer[7];
          #if VMIX_debug
          if ((_serialOutput==0x80 && vmixAudioPropertiesBusses[input]!=temp) || (_serialOutput==0x81 && !hasInitialized()) || _serialOutput==0x82) {
            Serial.print(F("vmixAudioPropertiesBusses[input=")); Serial.print(input); Serial.print(F("] = "));
            Serial.println(vmixAudioPropertiesBusses[input]);
          }
          #endif
          
        }
      } else 
      if(!strcmp_P(cmdStr, PSTR("MAuP"))) {
        
          #if VMIX_debug
          temp = vmixMasterAudioPropertiesVolume;
          #endif          
          vmixMasterAudioPropertiesVolume = (int16_t) word(_packetBuffer[0], _packetBuffer[1]);
          #if VMIX_debug
          if ((_serialOutput==0x80 && vmixMasterAudioPropertiesVolume!=temp) || (_serialOutput==0x81 && !hasInitialized()) || _serialOutput==0x82)  {
            Serial.print(F("vmixMasterAudioPropertiesVolume = "));
            Serial.println(vmixMasterAudioPropertiesVolume);
          }
          #endif
          
          #if VMIX_debug
          temp = vmixMasterAudioPropertiesHeadphoneVolume;
          #endif          
          vmixMasterAudioPropertiesHeadphoneVolume = (int16_t) word(_packetBuffer[2], _packetBuffer[3]);
          #if VMIX_debug
          if ((_serialOutput==0x80 && vmixMasterAudioPropertiesHeadphoneVolume!=temp) || (_serialOutput==0x81 && !hasInitialized()) || _serialOutput==0x82) {
            Serial.print(F("vmixMasterAudioPropertiesHeadphoneVolume = "));
            Serial.println(vmixMasterAudioPropertiesHeadphoneVolume);
          }
          #endif
          
          #if VMIX_debug
          temp = vmixMasterAudioPropertiesMuted;
          #endif          
          vmixMasterAudioPropertiesMuted = _packetBuffer[4];
          #if VMIX_debug
          if ((_serialOutput==0x80 && vmixMasterAudioPropertiesMuted!=temp) || (_serialOutput==0x81 && !hasInitialized()) || _serialOutput==0x82) {
            Serial.print(F("vmixMasterAudioPropertiesMuted = "));
            Serial.println(vmixMasterAudioPropertiesMuted);
          }
          #endif
          
      } else 
      if(!strcmp_P(cmdStr, PSTR("_ver"))) {
        
          #if VMIX_debug
          temp = vmixvMixVersionA;
          #endif          
          vmixvMixVersionA = word(_packetBuffer[0], _packetBuffer[1]);
          #if VMIX_debug
          if ((_serialOutput==0x80 && vmixvMixVersionA!=temp) || (_serialOutput==0x81 && !hasInitialized()) || _serialOutput==0x82) {
            Serial.print(F("vmixvMixVersionA = "));
            Serial.println(vmixvMixVersionA);
          }
          #endif
          
          #if VMIX_debug
          temp = vmixvMixVersionB;
          #endif          
          vmixvMixVersionB = word(_packetBuffer[2], _packetBuffer[3]);
          #if VMIX_debug
          if ((_serialOutput==0x80 && vmixvMixVersionB!=temp) || (_serialOutput==0x81 && !hasInitialized()) || _serialOutput==0x82) {
            Serial.print(F("vmixvMixVersionB = "));
            Serial.println(vmixvMixVersionB);
          }
          #endif
          
          #if VMIX_debug
          temp = vmixvMixVersionC;
          #endif          
          vmixvMixVersionC = word(_packetBuffer[4], _packetBuffer[5]);
          #if VMIX_debug
          if ((_serialOutput==0x80 && vmixvMixVersionC!=temp) || (_serialOutput==0x81 && !hasInitialized()) || _serialOutput==0x82) {
            Serial.print(F("vmixvMixVersionC = "));
            Serial.println(vmixvMixVersionC);
          }
          #endif
          
          #if VMIX_debug
          temp = vmixvMixVersionD;
          #endif          
          vmixvMixVersionD = word(_packetBuffer[6], _packetBuffer[7]);
          #if VMIX_debug
          if ((_serialOutput==0x80 && vmixvMixVersionD!=temp) || (_serialOutput==0x81 && !hasInitialized()) || _serialOutput==0x82) {
            Serial.print(F("vmixvMixVersionD = "));
            Serial.println(vmixvMixVersionD);
          }
          #endif
          
      } else 
      if(!strcmp_P(cmdStr, PSTR("_top"))) {
        
          #if VMIX_debug
          temp = vmixTopologyInputs;
          #endif          
          vmixTopologyInputs = _packetBuffer[0];
          #if VMIX_debug
          if ((_serialOutput==0x80 && vmixTopologyInputs!=temp) || (_serialOutput==0x81 && !hasInitialized()) || _serialOutput==0x82) {
            Serial.print(F("vmixTopologyInputs = "));
            Serial.println(vmixTopologyInputs);
          }
          #endif
          
      } else 
      {}
    }





      /**
       * Get Active Input; Input
       */
      uint8_t ClientVMixUDP::getActiveInput() {
        return vmixActiveInput;
      }
      
      /**
       * Set Active Input; Input
       * input  0-99: 1-100 Input Index
       */
      void ClientVMixUDP::setActiveInput(uint8_t input) {
      
            _prepareCommandPacket(PSTR("CAcI"),1);
    
        _packetBuffer[12+_cBBO+4+4+0] = input;
        
          _finishCommandPacket();
    
      }
      
      /**
       * Set Transition To; Duration
       * input  0-99: 1-100 Input Index
       * transition   0: Cut, 1: Fade, 2: Zoom, 3: Wipe, 4: Slide, 5: Fly, 6: CrossZoom, 7: FlyRotate, 8: Cube, 9: CubeZoom, 10: VerticalWipe, 11: VerticalSlide, 12: Merge
       * duration   0-9999: Time in ms
       */
      void ClientVMixUDP::setActiveInputTransitionDuration(uint8_t input, uint8_t transition, uint16_t duration) {
      
            _prepareCommandPacket(PSTR("TrTo"),4,(_packetBuffer[12+_cBBO+4+4+0]==input) && (_packetBuffer[12+_cBBO+4+4+1]==transition));
    
        _packetBuffer[12+_cBBO+4+4+0] = input;
        
        _packetBuffer[12+_cBBO+4+4+1] = transition;
        
        _packetBuffer[12+_cBBO+4+4+2] = highByte(duration);
        _packetBuffer[12+_cBBO+4+4+3] = lowByte(duration);
        
          _finishCommandPacket();
    
      }
      
      /**
       * Get Preview Input; Input
       */
      uint8_t ClientVMixUDP::getPreviewInput() {
        return vmixPreviewInput;
      }
      
      /**
       * Set Preview Input; Input
       * input  0-99: 1-100 Input Index
       */
      void ClientVMixUDP::setPreviewInput(uint8_t input) {
      
            _prepareCommandPacket(PSTR("CPvI"),1);
    
        _packetBuffer[12+_cBBO+4+4+0] = input;
        
          _finishCommandPacket();
    
      }
      
      /**
       * Set Cut; Action
       * action   Bit 0: On/Off
       */
      void ClientVMixUDP::performCutAction(bool action) {
      
            _prepareCommandPacket(PSTR("DCut"),1);
    
        _packetBuffer[12+_cBBO+4+4+0] = action;
        
          _finishCommandPacket();
    
      }
      
      /**
       * Set Fade; Fader
       * fader  0-3: Fader 1-4
       */
      void ClientVMixUDP::performFadeFader(uint8_t fader) {
      
            _prepareCommandPacket(PSTR("DFad"),1);
    
        _packetBuffer[12+_cBBO+4+4+0] = fader;
        
          _finishCommandPacket();
    
      }
      
      /**
       * Set Fade-To-Black; Action
       * action   Bit 0: On/Off
       */
      void ClientVMixUDP::performFadeToBlackAction(bool action) {
      
            _prepareCommandPacket(PSTR("FtbA"),1);
    
        _packetBuffer[12+_cBBO+4+4+0] = action;
        
          _finishCommandPacket();
    
      }
      
      /**
       * Get Fade-To-Black; Active
       */
      bool ClientVMixUDP::getFadeToBlackActive() {
        return vmixFadeToBlackActive;
      }
      
      /**
       * Set Transition Position; Position
       * position   0-255
       */
      void ClientVMixUDP::setTransitionPosition(uint8_t position) {
      
            _prepareCommandPacket(PSTR("CTPs"),1);
    
        _packetBuffer[12+_cBBO+4+4+0] = position;
        
          _finishCommandPacket();
    
      }
      
      /**
       * Get Overlay Active; Active
       * overlay  0-5:1-6 Overlay
       */
      bool ClientVMixUDP::getOverlayActive(uint8_t overlay) {
        return vmixOverlayActive[overlay];
      }
      
      /**
       * Get Overlay Active; Input
       * overlay  0-5:1-6 Overlay
       */
      uint8_t ClientVMixUDP::getOverlayInput(uint8_t overlay) {
        return vmixOverlayInput[overlay];
      }
      
      /**
       * Set Overlay Active; InputOn
       * overlay  0-5:1-6 Overlay
       * inputOn  0-99: 1-100 Input Index
       */
      void ClientVMixUDP::setOverlayInputOn(uint8_t overlay, uint8_t inputOn) {
      
            _prepareCommandPacket(PSTR("COlA"),7,(_packetBuffer[12+_cBBO+4+4+1]==overlay));
    
          // Set Mask: 1
        _packetBuffer[12+_cBBO+4+4+0] |= 1;
            
        _packetBuffer[12+_cBBO+4+4+1] = overlay;
        
        _packetBuffer[12+_cBBO+4+4+2] = inputOn;
        
          _finishCommandPacket();
    
      }
      
      /**
       * Set Overlay Active; InputOff
       * overlay  0-5:1-6 Overlay
       * inputOff   Bit 0: On/Off
       */
      void ClientVMixUDP::setOverlayInputOff(uint8_t overlay, bool inputOff) {
      
            _prepareCommandPacket(PSTR("COlA"),7,(_packetBuffer[12+_cBBO+4+4+1]==overlay));
    
          // Set Mask: 2
        _packetBuffer[12+_cBBO+4+4+0] |= 2;
            
        _packetBuffer[12+_cBBO+4+4+1] = overlay;
        
        _packetBuffer[12+_cBBO+4+4+3] = inputOff;
        
          _finishCommandPacket();
    
      }
      
      /**
       * Set Overlay Active; InputInstantOff
       * overlay  0-5:1-6 Overlay
       * inputInstantOff  Bit 0: On/Off
       */
      void ClientVMixUDP::setOverlayInputInstantOff(uint8_t overlay, bool inputInstantOff) {
      
            _prepareCommandPacket(PSTR("COlA"),7,(_packetBuffer[12+_cBBO+4+4+1]==overlay));
    
          // Set Mask: 4
        _packetBuffer[12+_cBBO+4+4+0] |= 4;
            
        _packetBuffer[12+_cBBO+4+4+1] = overlay;
        
        _packetBuffer[12+_cBBO+4+4+4] = inputInstantOff;
        
          _finishCommandPacket();
    
      }
      
      /**
       * Set Overlay Active; InputZoom
       * overlay  0-5:1-6 Overlay
       * inputZoom  0-99: 1-100 Input Index
       */
      void ClientVMixUDP::setOverlayInputZoom(uint8_t overlay, uint8_t inputZoom) {
      
            _prepareCommandPacket(PSTR("COlA"),7,(_packetBuffer[12+_cBBO+4+4+1]==overlay));
    
          // Set Mask: 8
        _packetBuffer[12+_cBBO+4+4+0] |= 8;
            
        _packetBuffer[12+_cBBO+4+4+1] = overlay;
        
        _packetBuffer[12+_cBBO+4+4+5] = inputZoom;
        
          _finishCommandPacket();
    
      }
      
      /**
       * Set Overlay Active; InputPreview
       * overlay  0-5:1-6 Overlay
       * inputPreview   0-99: 1-100 Input Index
       */
      void ClientVMixUDP::setOverlayInputPreview(uint8_t overlay, uint8_t inputPreview) {
      
            _prepareCommandPacket(PSTR("COlA"),7,(_packetBuffer[12+_cBBO+4+4+1]==overlay));
    
          // Set Mask: 16
        _packetBuffer[12+_cBBO+4+4+0] |= 16;
            
        _packetBuffer[12+_cBBO+4+4+1] = overlay;
        
        _packetBuffer[12+_cBBO+4+4+6] = inputPreview;
        
          _finishCommandPacket();
    
      }
      
      /**
       * Get Record; Active
       */
      bool ClientVMixUDP::getRecordActive() {
        return vmixRecordActive;
      }
      
      /**
       * Set Record; Action
       * action   Bit 0: On/Off
       */
      void ClientVMixUDP::performRecordAction(bool action) {
      
            _prepareCommandPacket(PSTR("RecA"),1);
    
        _packetBuffer[12+_cBBO+4+4+0] = action;
        
          _finishCommandPacket();
    
      }
      
      /**
       * Get Stream; Active
       */
      bool ClientVMixUDP::getStreamActive() {
        return vmixStreamActive;
      }
      
      /**
       * Set Stream; Action
       * action   Bit 0: On/Off
       */
      void ClientVMixUDP::performStreamAction(bool action) {
      
            _prepareCommandPacket(PSTR("StrA"),1);
    
        _packetBuffer[12+_cBBO+4+4+0] = action;
        
          _finishCommandPacket();
    
      }
      
      /**
       * Set Audio Levels; Enable
       * enable   Bit 0: On/Off
       */
      void ClientVMixUDP::setAudioLevelsEnable(bool enable) {
      
            _prepareCommandPacket(PSTR("SALN"),1);
    
        _packetBuffer[12+_cBBO+4+4+0] = enable;
        
          _finishCommandPacket();
    
      }
      
      /**
       * Get Audio Levels; Left
       * input  0-99: 1-100 Input Index
       */
      int ClientVMixUDP::getAudioLevelsLeft(uint8_t input) {
        return vmixAudioLevelsLeft[input];
      }
      
      /**
       * Get Audio Levels; Right
       * input  0-99: 1-100 Input Index
       */
      int ClientVMixUDP::getAudioLevelsRight(uint8_t input) {
        return vmixAudioLevelsRight[input];
      }
      
      /**
       * Get Master Audio Levels; Left
       */
      int ClientVMixUDP::getMasterAudioLevelsLeft() {
        return vmixMasterAudioLevelsLeft;
      }
      
      /**
       * Get Master Audio Levels; Right
       */
      int ClientVMixUDP::getMasterAudioLevelsRight() {
        return vmixMasterAudioLevelsRight;
      }
      
      /**
       * Get Input Position; Position
       * input  0-99: 1-100 Input Index
       */
      uint16_t ClientVMixUDP::getInputPosition(uint8_t input) {
        return vmixInputPosition[input];
      }
      
      /**
       * Set Input Position; Position
       * input  0-99: 1-100 Input Index
       * position   Seconds
       */
      void ClientVMixUDP::setInputPositionPosition(uint8_t input, uint16_t position) {
      
            _prepareCommandPacket(PSTR("SInP"),4,(_packetBuffer[12+_cBBO+4+4+1]==input));
    
          // Set Mask: 1
        _packetBuffer[12+_cBBO+4+4+0] |= 1;
            
        _packetBuffer[12+_cBBO+4+4+1] = input;
        
        _packetBuffer[12+_cBBO+4+4+2] = highByte(position);
        _packetBuffer[12+_cBBO+4+4+3] = lowByte(position);
        
          _finishCommandPacket();
    
      }
      
      /**
       * Set Input Position; Enable
       * enable   Bit 0: On/Off
       */
      void ClientVMixUDP::setInputPositionEnable(bool enable) {
      
            _prepareCommandPacket(PSTR("SPoN"),1);
    
        _packetBuffer[12+_cBBO+4+4+0] = enable;
        
          _finishCommandPacket();
    
      }
      
      /**
       * Set Replay; Last event
       * lastevent  Bit 0: On/Off
       */
      void ClientVMixUDP::performReplayLastevent(bool lastevent) {
      
            _prepareCommandPacket(PSTR("RplA"),3);
    
          // Set Mask: 1
        _packetBuffer[12+_cBBO+4+4+0] |= 1;
            
        _packetBuffer[12+_cBBO+4+4+1] = lastevent;
        
          _finishCommandPacket();
    
      }
      
      /**
       * Set Replay; Selected event
       * selectedevent  Bit 0: On/Off
       */
      void ClientVMixUDP::performReplaySelectedevent(bool selectedevent) {
      
            _prepareCommandPacket(PSTR("RplA"),3);
    
          // Set Mask: 2
        _packetBuffer[12+_cBBO+4+4+0] |= 2;
            
        _packetBuffer[12+_cBBO+4+4+2] = selectedevent;
        
          _finishCommandPacket();
    
      }
      
      /**
       * Set Replay Properties; Seconds
       * seconds  
       */
      void ClientVMixUDP::setReplayPropertiesSeconds(uint16_t seconds) {
      
            _prepareCommandPacket(PSTR("RplP"),7);
    
          // Set Mask: 1
        _packetBuffer[12+_cBBO+4+4+0] |= 1;
            
        _packetBuffer[12+_cBBO+4+4+1] = highByte(seconds);
        _packetBuffer[12+_cBBO+4+4+2] = lowByte(seconds);
        
          _finishCommandPacket();
    
      }
      
      /**
       * Set Replay Properties; Speed
       * speed  0-100: 0-1x speed
       */
      void ClientVMixUDP::setReplayPropertiesSpeed(uint16_t speed) {
      
            _prepareCommandPacket(PSTR("RplP"),7);
    
          // Set Mask: 2
        _packetBuffer[12+_cBBO+4+4+0] |= 2;
            
        _packetBuffer[12+_cBBO+4+4+3] = highByte(speed);
        _packetBuffer[12+_cBBO+4+4+4] = lowByte(speed);
        
          _finishCommandPacket();
    
      }
      
      /**
       * Set Replay Properties; Events
       * events   0-7: Event 1-8
       */
      void ClientVMixUDP::setReplayPropertiesEvents(uint8_t events) {
      
            _prepareCommandPacket(PSTR("RplP"),7);
    
          // Set Mask: 4
        _packetBuffer[12+_cBBO+4+4+0] |= 4;
            
        _packetBuffer[12+_cBBO+4+4+5] = events;
        
          _finishCommandPacket();
    
      }
      
      /**
       * Set Replay Properties; Jump
       * jump   0-255: Number of frames
       */
      void ClientVMixUDP::setReplayPropertiesJump(uint8_t jump) {
      
            _prepareCommandPacket(PSTR("RplP"),7);
    
          // Set Mask: 8
        _packetBuffer[12+_cBBO+4+4+0] |= 8;
            
        _packetBuffer[12+_cBBO+4+4+6] = jump;
        
          _finishCommandPacket();
    
      }
      
      /**
       * Set Replay Events Settings; MoveTo
       * moveTo   0-7: Destination bank
       */
      void ClientVMixUDP::setReplayEventsSettingsMoveTo(uint8_t moveTo) {
      
            _prepareCommandPacket(PSTR("RplE"),8);
    
          // Set Mask: 1
        _packetBuffer[12+_cBBO+4+4+0] |= 1;
            
        _packetBuffer[12+_cBBO+4+4+1] = moveTo;
        
          _finishCommandPacket();
    
      }
      
      /**
       * Set Replay Events Settings; CopyTo
       * copyTo   0-7: Destination bank
       */
      void ClientVMixUDP::setReplayEventsSettingsCopyTo(uint8_t copyTo) {
      
            _prepareCommandPacket(PSTR("RplE"),8);
    
          // Set Mask: 2
        _packetBuffer[12+_cBBO+4+4+0] |= 2;
            
        _packetBuffer[12+_cBBO+4+4+2] = copyTo;
        
          _finishCommandPacket();
    
      }
      
      /**
       * Set Replay Events Settings; Next
       * next   Select next event
       */
      void ClientVMixUDP::setReplayEventsSettingsNext(uint8_t next) {
      
            _prepareCommandPacket(PSTR("RplE"),8);
    
          // Set Mask: 4
        _packetBuffer[12+_cBBO+4+4+0] |= 4;
            
        _packetBuffer[12+_cBBO+4+4+3] = next;
        
          _finishCommandPacket();
    
      }
      
      /**
       * Set Replay Events Settings; Previous
       * previous   Select previous event
       */
      void ClientVMixUDP::setReplayEventsSettingsPrevious(uint8_t previous) {
      
            _prepareCommandPacket(PSTR("RplE"),8);
    
          // Set Mask: 8
        _packetBuffer[12+_cBBO+4+4+0] |= 8;
            
        _packetBuffer[12+_cBBO+4+4+4] = previous;
        
          _finishCommandPacket();
    
      }
      
      /**
       * Set Replay Events Settings; Toggle camera
       * togglecamera   Toggle camera
       */
      void ClientVMixUDP::setReplayEventsSettingsTogglecamera(uint8_t togglecamera) {
      
            _prepareCommandPacket(PSTR("RplE"),8);
    
          // Set Mask: 16
        _packetBuffer[12+_cBBO+4+4+0] |= 16;
            
        _packetBuffer[12+_cBBO+4+4+5] = togglecamera;
        
          _finishCommandPacket();
    
      }
      
      /**
       * Set Replay Events Settings; Toggle live
       * togglelive   Bit 0: Toggle replay live/Off
       */
      void ClientVMixUDP::setReplayEventsSettingsTogglelive(bool togglelive) {
      
            _prepareCommandPacket(PSTR("RplE"),8);
    
          // Set Mask: 32
        _packetBuffer[12+_cBBO+4+4+0] |= 32;
            
        _packetBuffer[12+_cBBO+4+4+6] = togglelive;
        
          _finishCommandPacket();
    
      }
      
      /**
       * Set Replay Events Settings; Toggle direction
       * toggledirection  Bit 0: Toggle replay direction/Off
       */
      void ClientVMixUDP::setReplayEventsSettingsToggledirection(bool toggledirection) {
      
            _prepareCommandPacket(PSTR("RplE"),8);
    
          // Set Mask: 64
        _packetBuffer[12+_cBBO+4+4+0] |= 64;
            
        _packetBuffer[12+_cBBO+4+4+7] = toggledirection;
        
          _finishCommandPacket();
    
      }
      
      /**
       * Get Xaml Values; Value
       * input  0-99: 1-100 Input Index
       * index  0-10: 0-10 Value Index
       */
      int ClientVMixUDP::getXamlValue(uint8_t input, uint8_t index) {
        return vmixXamlValue[input][index];
      }
      
      /**
       * Set Xaml Values; Value
       * input  0-30: 1-30 Source Index
       * index  0-10: 0-10 Value Index
       * value  
       */
      void ClientVMixUDP::setXamlValue(uint8_t input, uint8_t index, int value) {
      
            _prepareCommandPacket(PSTR("SXaV"),5,(_packetBuffer[12+_cBBO+4+4+1]==input) && (_packetBuffer[12+_cBBO+4+4+2]==index));
    
          // Set Mask: 1
        _packetBuffer[12+_cBBO+4+4+0] |= 1;
            
        _packetBuffer[12+_cBBO+4+4+1] = input;
        
        _packetBuffer[12+_cBBO+4+4+2] = index;
        
        _packetBuffer[12+_cBBO+4+4+3] = highByte(value);
        _packetBuffer[12+_cBBO+4+4+4] = lowByte(value);
        
          _finishCommandPacket();
    
      }
      
      /**
       * Set Count Down; Mode
       * input  0-30: 1-30 Source Index
       * index  0-5: 1-6 Value Index
       * mode   0: Start, 1: Pause, 2: Stop+Reset
       */
      void ClientVMixUDP::setCountDownMode(uint8_t input, uint8_t index, uint8_t mode) {
      
            _prepareCommandPacket(PSTR("CntD"),4,(_packetBuffer[12+_cBBO+4+4+1]==input) && (_packetBuffer[12+_cBBO+4+4+2]==index));
    
          // Set Mask: 1
        _packetBuffer[12+_cBBO+4+4+0] |= 1;
            
        _packetBuffer[12+_cBBO+4+4+1] = input;
        
        _packetBuffer[12+_cBBO+4+4+2] = index;
        
        _packetBuffer[12+_cBBO+4+4+3] = mode;
        
          _finishCommandPacket();
    
      }
      
      /**
       * Get Input Properties; Long Name
       * input  0-99: 1-100 Input Index
       */
      char *  ClientVMixUDP::getInputPropertiesLongName(uint8_t input) {
        return vmixInputPropertiesLongName[input];
      }
      
      /**
       * Get Input Properties; Short Name
       * input  0-99: 1-100 Input Index
       */
      char *  ClientVMixUDP::getInputPropertiesShortName(uint8_t input) {
        return vmixInputPropertiesShortName[input];
      }
      
      /**
       * Get Input Properties; Loop
       * input  0-99: 1-100 Input Index
       */
      bool ClientVMixUDP::getInputPropertiesLoop(uint8_t input) {
        return vmixInputPropertiesLoop[input];
      }
      
      /**
       * Get Input Properties; Type
       * input  0-99: 1-100 Input Index
       */
      uint8_t ClientVMixUDP::getInputPropertiesType(uint8_t input) {
        return vmixInputPropertiesType[input];
      }
      
      /**
       * Get Input Properties; State
       * input  0-99: 1-100 Input Index
       */
      uint8_t ClientVMixUDP::getInputPropertiesState(uint8_t input) {
        return vmixInputPropertiesState[input];
      }
      
      /**
       * Get Input Properties; Duration
       * input  0-99: 1-100 Input Index
       */
      uint16_t ClientVMixUDP::getInputPropertiesDuration(uint8_t input) {
        return vmixInputPropertiesDuration[input];
      }
      
      /**
       * Set Input Properties; Loop
       * input  0-99: 1-100 Input Index
       * loop   Bit 0: On/Off
       */
      void ClientVMixUDP::setInputPropertiesLoop(uint8_t input, bool loop) {
      
            _prepareCommandPacket(PSTR("CInP"),4,(_packetBuffer[12+_cBBO+4+4+1]==input));
    
          // Set Mask: 1
        _packetBuffer[12+_cBBO+4+4+0] |= 1;
            
        _packetBuffer[12+_cBBO+4+4+1] = input;
        
        _packetBuffer[12+_cBBO+4+4+2] = loop;
        
          _finishCommandPacket();
    
      }
      
      /**
       * Set Input Properties; Play/Pause
       * input  0-99: 1-100 Input Index
       * playPause  Bit 0: On/Off
       */
      void ClientVMixUDP::setInputPropertiesPlayPause(uint8_t input, bool playPause) {
      
            _prepareCommandPacket(PSTR("CInP"),4,(_packetBuffer[12+_cBBO+4+4+1]==input));
    
          // Set Mask: 2
        _packetBuffer[12+_cBBO+4+4+0] |= 2;
            
        _packetBuffer[12+_cBBO+4+4+1] = input;
        
        _packetBuffer[12+_cBBO+4+4+3] = playPause;
        
          _finishCommandPacket();
    
      }
      
      /**
       * Get Audio Properties; Solo
       * input  0-99: 1-100 Input Index
       */
      bool ClientVMixUDP::getAudioPropertiesSolo(uint8_t input) {
        return vmixAudioPropertiesSolo[input];
      }
      
      /**
       * Get Audio Properties; Balance
       * input  0-99: 1-100 Input Index
       */
      int ClientVMixUDP::getAudioPropertiesBalance(uint8_t input) {
        return vmixAudioPropertiesBalance[input];
      }
      
      /**
       * Get Audio Properties; Volume
       * input  0-99: 1-100 Input Index
       */
      int ClientVMixUDP::getAudioPropertiesVolume(uint8_t input) {
        return vmixAudioPropertiesVolume[input];
      }
      
      /**
       * Get Audio Properties; Muted
       * input  0-99: 1-100 Input Index
       */
      bool ClientVMixUDP::getAudioPropertiesMuted(uint8_t input) {
        return vmixAudioPropertiesMuted[input];
      }
      
      /**
       * Get Audio Properties; Busses
       * input  0-99: 1-100 Input Index
       */
      uint8_t ClientVMixUDP::getAudioPropertiesBusses(uint8_t input) {
        return vmixAudioPropertiesBusses[input];
      }
      
      /**
       * Set Audio Properties; Solo
       * input  0-99: 1-100 Input Index
       * solo   Bit 0: On/Off
       */
      void ClientVMixUDP::setAudioPropertiesSolo(uint8_t input, bool solo) {
      
            _prepareCommandPacket(PSTR("SAuP"),9,(_packetBuffer[12+_cBBO+4+4+1]==input));
    
          // Set Mask: 1
        _packetBuffer[12+_cBBO+4+4+0] |= 1;
            
        _packetBuffer[12+_cBBO+4+4+1] = input;
        
        _packetBuffer[12+_cBBO+4+4+2] = solo;
        
          _finishCommandPacket();
    
      }
      
      /**
       * Set Audio Properties; Balance
       * input  0-99: 1-100 Input Index
       * balance  -100-100: Balance
       */
      void ClientVMixUDP::setAudioPropertiesBalance(uint8_t input, int balance) {
      
            _prepareCommandPacket(PSTR("SAuP"),9,(_packetBuffer[12+_cBBO+4+4+1]==input));
    
          // Set Mask: 2
        _packetBuffer[12+_cBBO+4+4+0] |= 2;
            
        _packetBuffer[12+_cBBO+4+4+1] = input;
        
        _packetBuffer[12+_cBBO+4+4+3] = highByte(balance);
        _packetBuffer[12+_cBBO+4+4+4] = lowByte(balance);
        
          _finishCommandPacket();
    
      }
      
      /**
       * Set Audio Properties; Volume
       * input  0-99: 1-100 Input Index
       * volume   -60-0 db: Volume
       */
      void ClientVMixUDP::setAudioPropertiesVolume(uint8_t input, int volume) {
      
            _prepareCommandPacket(PSTR("SAuP"),9,(_packetBuffer[12+_cBBO+4+4+1]==input));
    
          // Set Mask: 4
        _packetBuffer[12+_cBBO+4+4+0] |= 4;
            
        _packetBuffer[12+_cBBO+4+4+1] = input;
        
        _packetBuffer[12+_cBBO+4+4+5] = highByte(volume);
        _packetBuffer[12+_cBBO+4+4+6] = lowByte(volume);
        
          _finishCommandPacket();
    
      }
      
      /**
       * Set Audio Properties; Muted
       * input  0-99: 1-100 Input Index
       * muted  Bit 0: On/Off
       */
      void ClientVMixUDP::setAudioPropertiesMuted(uint8_t input, bool muted) {
      
            _prepareCommandPacket(PSTR("SAuP"),9,(_packetBuffer[12+_cBBO+4+4+1]==input));
    
          // Set Mask: 8
        _packetBuffer[12+_cBBO+4+4+0] |= 8;
            
        _packetBuffer[12+_cBBO+4+4+1] = input;
        
        _packetBuffer[12+_cBBO+4+4+7] = muted;
        
          _finishCommandPacket();
    
      }
      
      /**
       * Set Audio Properties; Busses
       * input  0-99: 1-100 Input Index
       * busses   Bit 0: Master=On/Off, Bit 1: A=On/Off, Bit 2: B=On/Off
       */
      void ClientVMixUDP::setAudioPropertiesBusses(uint8_t input, uint8_t busses) {
      
            _prepareCommandPacket(PSTR("SAuP"),9,(_packetBuffer[12+_cBBO+4+4+1]==input));
    
          // Set Mask: 16
        _packetBuffer[12+_cBBO+4+4+0] |= 16;
            
        _packetBuffer[12+_cBBO+4+4+1] = input;
        
        _packetBuffer[12+_cBBO+4+4+8] = busses;
        
          _finishCommandPacket();
    
      }
      
      /**
       * Get Master Audio Properties; Volume
       */
      int ClientVMixUDP::getMasterAudioPropertiesVolume() {
        return vmixMasterAudioPropertiesVolume;
      }
      
      /**
       * Get Master Audio Properties; Headphone Volume
       */
      int ClientVMixUDP::getMasterAudioPropertiesHeadphoneVolume() {
        return vmixMasterAudioPropertiesHeadphoneVolume;
      }
      
      /**
       * Get Master Audio Properties; Muted
       */
      bool ClientVMixUDP::getMasterAudioPropertiesMuted() {
        return vmixMasterAudioPropertiesMuted;
      }
      
      /**
       * Set Master Audio Properties; Volume
       * volume   -60-0 db: Volume
       */
      void ClientVMixUDP::setMasterAudioPropertiesVolume(int volume) {
      
            _prepareCommandPacket(PSTR("MSAP"),6);
    
          // Set Mask: 1
        _packetBuffer[12+_cBBO+4+4+0] |= 1;
            
        _packetBuffer[12+_cBBO+4+4+1] = highByte(volume);
        _packetBuffer[12+_cBBO+4+4+2] = lowByte(volume);
        
          _finishCommandPacket();
    
      }
      
      /**
       * Set Master Audio Properties; Headphone Volume
       * headphoneVolume  -60-0 db: Volume
       */
      void ClientVMixUDP::setMasterAudioPropertiesHeadphoneVolume(int headphoneVolume) {
      
            _prepareCommandPacket(PSTR("MSAP"),6);
    
          // Set Mask: 2
        _packetBuffer[12+_cBBO+4+4+0] |= 2;
            
        _packetBuffer[12+_cBBO+4+4+3] = highByte(headphoneVolume);
        _packetBuffer[12+_cBBO+4+4+4] = lowByte(headphoneVolume);
        
          _finishCommandPacket();
    
      }
      
      /**
       * Set Master Audio Properties; Muted
       * muted  Bit 0: On/Off
       */
      void ClientVMixUDP::setMasterAudioPropertiesMuted(bool muted) {
      
            _prepareCommandPacket(PSTR("MSAP"),6);
    
          // Set Mask: 4
        _packetBuffer[12+_cBBO+4+4+0] |= 4;
            
        _packetBuffer[12+_cBBO+4+4+5] = muted;
        
          _finishCommandPacket();
    
      }
      
      /**
       * Get vMix Version; A
       */
      uint16_t ClientVMixUDP::getvMixVersionA() {
        return vmixvMixVersionA;
      }
      
      /**
       * Get vMix Version; B
       */
      uint16_t ClientVMixUDP::getvMixVersionB() {
        return vmixvMixVersionB;
      }
      
      /**
       * Get vMix Version; C
       */
      uint16_t ClientVMixUDP::getvMixVersionC() {
        return vmixvMixVersionC;
      }
      
      /**
       * Get vMix Version; D
       */
      uint16_t ClientVMixUDP::getvMixVersionD() {
        return vmixvMixVersionD;
      }
      
      /**
       * Get Topology; Inputs
       */
      uint8_t ClientVMixUDP::getTopologyInputs() {
        return vmixTopologyInputs;
      }
      