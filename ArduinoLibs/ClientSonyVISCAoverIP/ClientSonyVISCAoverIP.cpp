/*
Copyright 2016 Kasper Skårhøj, SKAARHOJ K/S, kasper@skaarhoj.com

This file is part of the Sony Visca UDP Client library for Arduino

The Sony Visca UDP Client library is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by the
Free Software Foundation, either version 3 of the License, or (at your
option) any later version.

The Sony Visca UDP Client library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with the Sony Visca UDP Client library. If not, see http://www.gnu.org/licenses/.


IMPORTANT: If you want to use this library in your own projects and/or products,
please play a fair game and heed the license rules! See our web page for a Q&A so
you can keep a clear conscience: http://skaarhoj.com/about/licenses/


*/

#include "ClientSonyVISCAoverIP.h"
#include "Streaming.h"

/**
 * Constructor
 */
ClientSonyVISCAoverIP::ClientSonyVISCAoverIP() {}

/**
 * Initializing and Setting up base IP address for the camera
 */
void ClientSonyVISCAoverIP::begin(const IPAddress ip) {

// Set up Udp communication object:
#ifdef ESP8266
  WiFiUDP Udp;
#else
  EthernetUDP Udp;
#endif

  _Udp = Udp;

  _cameraIP = ip;        // Set Camera IP base address. Cameras 1-7 is added to last byte, so IP address 192.168.10.200 will be 201-207 for cameras 1-7
  forthByteBase = ip[3]; // Picks up the base address. Used as offset to set dynamically the IP address of cameras

  _serialOutput = 0;

  // Initialize/reset:
  memset(_lastContact, 0, SONYVISCAIP_cams);
  memset(_camLastTX, 0, SONYVISCAIP_cams);
  memset(_camOnline, 0, SONYVISCAIP_cams);
  memset(_applicationCommandBufferCurrentIndex, 0, SONYVISCAIP_cams);
  memset(_applicationCommandBufferLastPositionAdded, 0, SONYVISCAIP_cams);
  memset(_nextStateUpdate, 0, SONYVISCAIP_cams);

  // The cameras will send information back on this port, so we need to listen on it - but it would be OK to send from a different port (like 65000 like the Sony Remotes does)- but that would require two sockets, so we stay with this solution I think...
  uint16_t portNumber = 52381; // random(53100, 65300);
  _Udp.begin(portNumber);
}

/**
 * Initiating connection handshake to Sony Visca UDP: By resetting setting the sequence counter
 * This also puts the camera "online" which indicates that a given camera should be attempted to keep a connection to
 */
void ClientSonyVISCAoverIP::connect(uint8_t cam) {
  if (camNumOK(cam)) {                  // valid range for VISCA
    _camOnline[cam - 1] = true;         // Turn a camera online!
    _localPacketIdCounter[cam - 1] = 0; // Init localPacketIDCounter to 0;
    _lastContact[cam - 1] = millis();
    _pushedStateUpdate[cam - 1] = false;
    _isConnected[cam - 1] = false; // Will be true after the initial hello-package handshakes.
    _nextStateUpdate[cam - 1] = 0;

    // _hasInitialized = false;                 // TODO: Will be true after initial payload of data is resent and received well. Need to implement this at connect time.

    // Send connectString to camera:
    if (_serialOutput) {
      Serial << F("Sending reset counter packet to Sony Visca camera #") << cam << "\n";
    }

    _wipeCleanPacketBuffer();
    _packetBuffer[8] = 0x01;
    _createCommandHeader(cam, 0x0200, 1);
    _sendPacketBuffer(cam, 8 + 1);
  }
}

/**
 * Keeps connection to the cameras alive: Reads UDP incoming and empties/processes the outgoing command queue
 * Therefore: Call this in the loop() function and make sure it gets called multiple times a second, including after sending commands.
 */
void ClientSonyVISCAoverIP::runLoop(uint16_t delayTime) {

  unsigned long enterTime = millis();
  do {

    // Reading incoming UDP data on port:
    while (true) { // Iterate until UDP buffer is empty
      uint16_t packetSize = _Udp.parsePacket();

      if (_Udp.available()) {
        IPAddress remote = _Udp.remoteIP();
        uint8_t cam = remote[3] - forthByteBase; // Select the cam from the IP address.

        if (camNumOK(cam)) {           // valid range for VISCA
          _Udp.read(_packetBuffer, 8); // Read header
          uint16_t payloadType = word(_packetBuffer[0], _packetBuffer[1]);
          uint16_t packetLength = word(_packetBuffer[2], _packetBuffer[3]);
          uint16_t sequenceNumber = word(_packetBuffer[6], _packetBuffer[7]); // Actually, this is probably a 32 bit unsigned long...
          if (packetSize == packetLength + 8) {                               // Just to make sure these are equal, they should be!

            if (_serialOutput & 0x80) {
              Serial << "Incoming Packet from cam #" << cam << " (" << packetLength << " bytes): 0x" << _HEXPADL(payloadType, 4, "0") << ": ";
              for (uint8_t a = 4; a < 8; a++) {
                Serial << _HEXPADL(_packetBuffer[a], 2, "0") << " ";
              }
              Serial << " / ";
            }

            // Read the VISCA packet itself:
            _Udp.read(_packetBuffer, SONYVISCAIP_packetBufferLength);

            if (_serialOutput & 0x80) {
              for (uint8_t a = 0; a < packetLength; a++) {
                Serial << _HEXPADL(_packetBuffer[a], 2, "0") << " ";
              }
              Serial << ": ";
            }

            if (!_isConnected[cam - 1]) {                                                   // If not connected, look for reply to reset counter package
              if (payloadType == 0x0201 && packetLength == 1 && _packetBuffer[0] == 0x01) { // Response to sequence number "RESET" connect package
                _isConnected[cam - 1] = true;
                if (_serialOutput & 0x80) {
                  Serial << F("Connected to VISCA camera #") << cam << "\n";
                }
              }
            } else {
              _lastContact[cam - 1] = millis();
              _pushedStateUpdate[cam - 1] = false;
              if (_camLastTX[cam - 1] > 0) { // Waiting for response
                // if (_localPacketIdCounter[cam - 1] == sequenceNumber) {	// the returned sequence number is a reference to the sequence number we sent to the camera - but notice that an ack and complete package will refer to the same number. At the moment I see no need to track the package number - we just look for acks and ignores "completes"
                if (payloadType == 0x0111 && _packetBuffer[0] == 0x90) {
                  if (_applicationCommandBuffer[cam - 1][_applicationCommandBufferCurrentIndex[cam - 1]][0] == 0x01 && packetLength == 3 && (_packetBuffer[1] & 0xF0) == 0x40) { // an ack for a command
                    if (_serialOutput & 0x80) {
                      Serial << "ACK in " << (millis() - _camLastTX[cam - 1]) << "ms\n";
                    }
                    _camLastTX[cam - 1] = 0;
                  }
                  if (_applicationCommandBuffer[cam - 1][_applicationCommandBufferCurrentIndex[cam - 1]][0] == 0x01 && packetLength == 4 && (_packetBuffer[1] & 0xF0) == 0x60) { // an error for a command
                    if (_serialOutput) {
                      Serial << "ERROR in " << (millis() - _camLastTX[cam - 1]) << "ms\n";
                    }
                    _camLastTX[cam - 1] = 0;
                  }
                  if (_applicationCommandBuffer[cam - 1][_applicationCommandBufferCurrentIndex[cam - 1]][0] == 0x09 && (_packetBuffer[1] & 0xF0) == 0x50) { // a inquery answer
                    if (_serialOutput & 0x80) {
                      Serial << "INQUERY ANS in " << (millis() - _camLastTX[cam - 1]) << "ms\n";
                    }
                    _camLastTX[cam - 1] = 0;
                    parsePacketBufferInqueryData(cam);
                  }
                }
              }
            }

            if (_serialOutput & 0x80) {
              Serial << "\n";
            }
          } else {
            //#if SONYVISCAIP_debug
            if (_serialOutput & 0x80) {
              Serial.print(F("ERROR: Packet size mismatch: "));
              Serial.print(packetSize, DEC);
              Serial.print(F(" != "));
              Serial.println(packetLength, DEC);
            }
            //#endif
            // Flushing larger packets
            while (_Udp.available()) {
              _Udp.read(_packetBuffer, SONYVISCAIP_packetBufferLength);
            }
          }
        } else {
          Serial << "ERROR: Cam number " << cam << " out of range!\n";
        }
      } else
        break;
    }

    // Send a command from buffer to cameras:
    for (uint8_t cam = 1; cam <= SONYVISCAIP_cams; cam++) {
      sendCommand(cam);
    }

    // If connection is gone anyway, try to reconnect:
    for (uint8_t cam = 1; cam <= SONYVISCAIP_cams; cam++) {
      if (_camOnline[cam - 1]) {
        if (hasTimedOut(_lastContact[cam - 1], 400) && _applicationCommandBufferLastPositionAdded[cam - 1] == _applicationCommandBufferCurrentIndex[cam - 1]) { // Send a "get state update" when nothing else happens (output buffer is empty)
          // if (_serialOutput)
          //             Serial.println(F("pushNextStateUpdate()"));
          pushNextStateUpdate(cam);
        } else if (hasTimedOut(_lastContact[cam - 1], 1000 + 2000)) { // Allow a get state update 2000 ms to register
          if (_serialOutput)
            Serial.println(F("Connection to Camera has timed out - reconnecting!"));
          connect(cam);
        }
      }
    }
  } while (delayTime > 0 && !hasTimedOut(enterTime, delayTime));
}

/**
 * Parse incoming data:
 */
void ClientSonyVISCAoverIP::parsePacketBufferInqueryData(uint8_t cam) {
  if (camNumOK(cam)) { // valid range for VISCA
    uint8_t QQ = _applicationCommandBuffer[cam - 1][_applicationCommandBufferCurrentIndex[cam - 1]][1];
    uint16_t RR = QQ == 0x7E ? ((_applicationCommandBuffer[cam - 1][_applicationCommandBufferCurrentIndex[cam - 1]][2] << 8) | _applicationCommandBuffer[cam - 1][_applicationCommandBufferCurrentIndex[cam - 1]][3]) : _applicationCommandBuffer[cam - 1][_applicationCommandBufferCurrentIndex[cam - 1]][2];
    long tempLong;
    int16_t tempInt;
    uint16_t tempUInt;
    bool tempBool;

    switch (QQ) { // Act on various "category codes"
    case 0x00:
      switch (RR) { // Act on the command:
      case 0x02:
        // CAM_VersionInq
        _CAM_VersionInq_model[cam - 1] = (_packetBuffer[4] << 8) | _packetBuffer[5];
        //    Serial << F("CAM_VersionInq Model: ") << _HEXPADL(_CAM_VersionInq_model[cam - 1], 4, "0") << "\n";
        break;
      }
      break;
    case 0x06:
      switch (RR) { // Act on the command:
      case 0x12:    // PAN-TILT (notice, this is specific for the particular camera model! BRC-H900 has larger numbers than fx. SRG-300H!)
                    // Pan position:
        tempLong = (_packetBuffer[3] << 12) | (_packetBuffer[4] << 8) | (_packetBuffer[5] << 4) | _packetBuffer[6];
        if (_packetBuffer[2] == 0)
          tempLong &= 0x0000FFFF;
        if (_CAM_PanPos[cam - 1] != tempLong) {
          _CAM_PanPos[cam - 1] = tempLong;
          Serial << F("_CAM_PanPos: ") << tempLong << F("\n");
        }

        // Tilt position:
        tempInt = (_packetBuffer[7] << 12) | (_packetBuffer[8] << 8) | (_packetBuffer[9] << 4) | _packetBuffer[10];
        if (_CAM_TiltPos[cam - 1] != tempInt) {
          _CAM_TiltPos[cam - 1] = tempInt;
          Serial << F("_CAM_TiltPos: ") << tempInt << F("\n");
        }
        break;
      }
      break;
    case 0x7E:
      switch (RR) { // Act on the command:
      case 0x7E00:  // Block Inquery Command 0:
        /*
      // Zoom position
      tempUInt = (_packetBuffer[2] << 12) | (_packetBuffer[3] << 8) | (_packetBuffer[4] << 4) | _packetBuffer[5];
      if (_CAM_ZoomPos[cam - 1] != tempUInt) {
        _CAM_ZoomPos[cam - 1] = tempUInt;
        Serial << F("_CAM_ZoomPos: ") << tempUInt << F("\n");
      }

      // Focus position
      tempUInt = (_packetBuffer[8] << 12) | (_packetBuffer[9] << 8) | (_packetBuffer[10] << 4) | _packetBuffer[11];
      if (_CAM_FocusPos[cam - 1] != tempUInt) {
        _CAM_FocusPos[cam - 1] = tempUInt;
        Serial << F("_CAM_FocusPos: ") << tempUInt << F("\n");
      }

      // Focus mode
      if (_CAM_FocusModeAuto[cam - 1] != (_packetBuffer[13] & B1)) {
        _CAM_FocusModeAuto[cam - 1] = (_packetBuffer[13] & B1);
        Serial << F("_CAM_FocusModeAuto: ") << (_packetBuffer[13] & B1) << F("\n");
      }
*/
        break;
      case 0x7E01: // Block Inquery Command 1:
        /*
      // WB R Gain
      tempUInt = (_packetBuffer[2] << 4) | _packetBuffer[3];
      if (_CAM_WB_Rgain[cam - 1] != (tempUInt & 0xFF)) {
        _CAM_WB_Rgain[cam - 1] = (tempUInt & 0xFF);
        Serial << F("_CAM_WB_Rgain: ") << _HEXPADL(_CAM_WB_Rgain[cam - 1], 2, "0") << F("\n");
      }

      // WB B Gain
      tempUInt = (_packetBuffer[4] << 4) | _packetBuffer[5];
      if (_CAM_WB_Bgain[cam - 1] != (tempUInt & 0xFF)) {
        _CAM_WB_Bgain[cam - 1] = (tempUInt & 0xFF);
        Serial << F("_CAM_WB_Bgain: ") << _HEXPADL(_CAM_WB_Bgain[cam - 1], 2, "0") << F("\n");
      }

      // Color Speed:
      tempUInt = (_packetBuffer[6] >> 3) & B111;
      if (_CAM_Color_Speed[cam - 1] != tempUInt) {
        _CAM_Color_Speed[cam - 1] = tempUInt);
        Serial << F("_CAM_Color_Speed: ") << _CAM_Color_Speed[cam - 1] << F("\n");
      }

      // WB Mode
      tempUInt = _packetBuffer[6] & B111;
      if (_CAM_WB_Mode[cam - 1] != tempUInt) {
        _CAM_WB_Mode[cam - 1] = tempUInt);
        Serial << F("_CAM_WB_Mode: ") << _CAM_WB_Mode[cam - 1] << F("\n");
      }

      // Detail Setting
      if (_CAM_DetailSetting[cam - 1] != (_packetBuffer[7] & B1)) {
        _CAM_DetailSetting[cam - 1] = (_packetBuffer[7] & B1);
        Serial << F("_CAM_DetailSetting: ") << _CAM_DetailSetting[cam - 1] << F("\n");
      }
*/
        // Exposure Mode
        tempUInt = _packetBuffer[8] & B1111;
        if (_ExposureMode[cam - 1] != tempUInt) {
          _ExposureMode[cam - 1] = tempUInt;
          Serial << F("_ExposureMode: ") << _ExposureMode[cam - 1] << F("\n");
        }
        /*
// Spot light mode
if (_CAM_SpotlightMode[cam - 1] != ((_packetBuffer[9] & B1000) ? true : false)) {
  _CAM_SpotlightMode[cam - 1] = ((_packetBuffer[9] & B1000) ? true : false);
  Serial << F("_CAM_SpotlightMode: ") << _CAM_SpotlightMode[cam - 1] << F("\n");
}

// Back Light mode
if (_CAM_BacklightMode[cam - 1] != ((_packetBuffer[9] & B100) ? true : false)) {
  _CAM_BacklightMode[cam - 1] = ((_packetBuffer[9] & B100) ? true : false);
  Serial << F("_CAM_BacklightMode: ") << _CAM_BacklightMode[cam - 1] << F("\n");
}

// Manual Shutter Position
tempUInt = _packetBuffer[10] & B11111;
if (_CAM_ShutterPosition[cam - 1] != tempUInt) {
  _CAM_ShutterPosition[cam - 1] = tempUInt);
  Serial << F("_CAM_ShutterPosition: ") << _CAM_ShutterPosition[cam - 1] << F("\n");
}

// Manual Iris Position
tempUInt = _packetBuffer[11] & B11111;
if (_CAM_IrisPosition[cam - 1] != tempUInt) {
  _CAM_IrisPosition[cam - 1] = tempUInt);
  Serial << F("_CAM_IrisPosition: ") << _CAM_IrisPosition[cam - 1] << F("\n");
}

// Manual Gain Position
tempUInt = _packetBuffer[12] & B11111;
if (_CAM_GainPosition[cam - 1] != tempUInt) {
  _CAM_GainPosition[cam - 1] = tempUInt);
  Serial << F("_CAM_GainPosition: ") << _CAM_GainPosition[cam - 1] << F("\n");
}
*/
        break;
      case 0x7E04: // Block Inquery Command 1:

        // Current Shutter Position
        tempUInt = _packetBuffer[10] & B11111;
        if (_ExposureShutter[cam - 1] != tempUInt) {
          _ExposureShutter[cam - 1] = tempUInt;
          Serial << F("_ExposureShutter: ") << _ExposureShutter[cam - 1] << F("\n");
        }

        // Current Iris Position
        tempUInt = _packetBuffer[11] & B11111;
        if (_ExposureIris[cam - 1] != tempUInt) {
          _ExposureIris[cam - 1] = tempUInt;
          Serial << F("_ExposureIris: ") << _ExposureIris[cam - 1] << F("\n");
        }
        break;
      }
      break;
    }
  }
}

/**
 * Pushes next state update
 * TODO: Must be rearranged so CAM_VersionInq is first and that is driving what packages to ask for anyway since the Block Inquery Commands depend on this.
 */
uint8_t ClientSonyVISCAoverIP::pushNextStateUpdate(uint8_t cam) {
  if (camNumOK(cam) && _isConnected[cam - 1]) { // valid range for VISCA
    _nextStateUpdate[cam - 1] = (_nextStateUpdate[cam - 1] + 1) % 10;

    if (_nextStateUpdate[cam - 1] < 8) {
      addCommand(cam, 0x09, 0x7E, 0x7E00 | _nextStateUpdate[cam - 1]);
    } else {
      switch (_nextStateUpdate[cam - 1]) {
      case 8: // PANTILT position
        addCommand(cam, 0x09, 0x06, 0x12);
        break;
      case 9: // CAM_VersionInq
        addCommand(cam, 0x09, 0x00, 0x02);
        break;
      }
    }

    return _nextStateUpdate[cam - 1];
  }
  return 0xFF;
}

/**
 * If true, we had a response from the camera when trying to send a hello packet.
 */
bool ClientSonyVISCAoverIP::isConnected(uint8_t cam) { return _isConnected[cam - 1]; }

/**
 * If true, the initial handshake and information exchange has occured and now the camera connection should be ready for operation.
 */
bool ClientSonyVISCAoverIP::hasInitialized(uint8_t cam) { return _hasInitialized[cam - 1]; }

/**************
 *
 * Buffer work
 *
 **************/

/**
 * Adds a command to the command queue. Notice if RR is 0x7E, the cmd is 16 bits, otherwise it's just 8 bit. This is an observation from the way commands and inqueries seems to be constructed.
 */
bool ClientSonyVISCAoverIP::addCommand(uint8_t cam, uint8_t QQ, uint8_t RR, uint16_t cmd, uint16_t p1, uint16_t p2, uint16_t p3, uint16_t p4, uint16_t p5, uint16_t p6, uint16_t p7, uint16_t p8, uint16_t p9, uint16_t p10, uint16_t p11) {
  if (camNumOK(cam) && _isConnected[cam - 1]) { // valid range for VISCA
                                                //    Serial << "CAM: " << cam << "\n";
    if (((_applicationCommandBufferLastPositionAdded[cam - 1] + 1) % SONYVISCAIP_comBufSize) == _applicationCommandBufferCurrentIndex[cam - 1]) {
      //     Serial << "Buffer full...\n";
      return false; // Buffer full
    } else {
      uint8_t substituteCmdOffset = 0;
      //      Serial << "Buffer pointers: " << _applicationCommandBufferCurrentIndex[cam - 1] << "-" << _applicationCommandBufferLastPositionAdded[cam - 1] << "\n";
      if (_applicationCommandBufferLastPositionAdded[cam - 1] != _applicationCommandBufferCurrentIndex[cam - 1]) { // there is something in buffer, check it out first:
                                                                                                                   //      Serial << "Something in buffer, checking it:";
        for (uint8_t a = 1; a < SONYVISCAIP_comBufSize; a++) {                                                     // Start from the next non-sent command
          // Serial << " " << a;
          if (_applicationCommandBuffer[cam - 1][(_applicationCommandBufferCurrentIndex[cam - 1] + a) % SONYVISCAIP_comBufSize][0] == QQ && _applicationCommandBuffer[cam - 1][(_applicationCommandBufferCurrentIndex[cam - 1] + a) % SONYVISCAIP_comBufSize][1] == RR) {
            switch (RR) {
            case 0x00:
            case 0x04:
            case 0x06:
              if (_applicationCommandBuffer[cam - 1][(_applicationCommandBufferCurrentIndex[cam - 1] + a) % SONYVISCAIP_comBufSize][2] == cmd) {
                substituteCmdOffset = a;
                //          Serial << "!";
                break;
              }
              break;
            case 0x7E:
              if (_applicationCommandBuffer[cam - 1][(_applicationCommandBufferCurrentIndex[cam - 1] + a) % SONYVISCAIP_comBufSize][2] == highByte(cmd) && _applicationCommandBuffer[cam - 1][(_applicationCommandBufferCurrentIndex[cam - 1] + a) % SONYVISCAIP_comBufSize][3] == lowByte(cmd)) {
                substituteCmdOffset = a;
                //           Serial << "!";
                break;
              }
              break;
            }
          }

          if ((_applicationCommandBufferCurrentIndex[cam - 1] + a) % SONYVISCAIP_comBufSize == _applicationCommandBufferLastPositionAdded[cam - 1]) {
            //        Serial << "Last Position reached and checked, breaking...";
            break;
          }
        }
        //    Serial << "\n";
      }

      uint8_t writeToIndex;
      if (substituteCmdOffset > 0) {
        writeToIndex = (_applicationCommandBufferCurrentIndex[cam - 1] + substituteCmdOffset) % SONYVISCAIP_comBufSize;
        //   Serial << "writeToIndex (overwriting): " << writeToIndex << "\n";
      } else {
        _applicationCommandBufferLastPositionAdded[cam - 1] = (_applicationCommandBufferLastPositionAdded[cam - 1] + 1) % SONYVISCAIP_comBufSize;
        writeToIndex = _applicationCommandBufferLastPositionAdded[cam - 1];
        //   Serial << "writeToIndex (new): " << writeToIndex << "\n";
      }

      // Fill buffer:
      memset(_applicationCommandBuffer[cam - 1][writeToIndex], 0xFF, 15);
      _applicationCommandBuffer[cam - 1][writeToIndex][0] = QQ;
      _applicationCommandBuffer[cam - 1][writeToIndex][1] = RR;
      uint8_t dataOffset = 1;
      switch (RR) {
      case 0x00:
      case 0x04:
      case 0x06:
        _applicationCommandBuffer[cam - 1][writeToIndex][2] = cmd;
        break;
      case 0x7E:
        _applicationCommandBuffer[cam - 1][writeToIndex][2] = highByte(cmd);
        _applicationCommandBuffer[cam - 1][writeToIndex][3] = lowByte(cmd);
        dataOffset = 2;
        break;
      }
      if (p1 < 0x100) {
        _applicationCommandBuffer[cam - 1][writeToIndex][2 + dataOffset] = p1;
        if (p2 < 0x100) {
          _applicationCommandBuffer[cam - 1][writeToIndex][3 + dataOffset] = p2;
          if (p3 < 0x100) {
            _applicationCommandBuffer[cam - 1][writeToIndex][4 + dataOffset] = p3;
            if (p4 < 0x100) {
              _applicationCommandBuffer[cam - 1][writeToIndex][5 + dataOffset] = p4;
              if (p5 < 0x100) {
                _applicationCommandBuffer[cam - 1][writeToIndex][6 + dataOffset] = p5;
                if (p6 < 0x100) {
                  _applicationCommandBuffer[cam - 1][writeToIndex][7 + dataOffset] = p6;
                  if (p7 < 0x100) {
                    _applicationCommandBuffer[cam - 1][writeToIndex][8 + dataOffset] = p7;
                    if (p8 < 0x100) {
                      _applicationCommandBuffer[cam - 1][writeToIndex][9 + dataOffset] = p8;
                      if (p9 < 0x100) {
                        _applicationCommandBuffer[cam - 1][writeToIndex][10 + dataOffset] = p9;
                        if (p10 < 0x100) {
                          _applicationCommandBuffer[cam - 1][writeToIndex][11 + dataOffset] = p10;
                          if (p11 < 0x100) {
                            _applicationCommandBuffer[cam - 1][writeToIndex][12 + dataOffset] = p11;
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
      return true;
    }
  } else
    return false;
}

/**
 * Sends next command in command buffer for all connected cameras
 */
void ClientSonyVISCAoverIP::sendCommand(uint8_t cam) {
  if (camNumOK(cam) && _isConnected[cam - 1]) { // valid range for VISCA
    if (_camOnline[cam - 1]) {
      int sendIndex = -1;
      if (_camLastTX[cam - 1] > 0) {                 // Waiting for answer
        if (hasTimedOut(_camLastTX[cam - 1], 200)) { // 200 ms wait for answer - could be a bit shorter?
          // retransmit: TODO
          // Retransmits seems to be dependent on other controllers messing with the camera - at least I had a very "stable" pattern of retransmits when Sonys controller was not connected - exactly every 10th package needed a retransmit....
          // But if other controllers were connected it was a different story. Hmm... well, actually I guess retransmits could be avoided if we kept track of whether the buffer has also COMPLETED an instruction. We can send a new instruction as soon as we get ack, but it may not take a new instruction before it has completed both buffers.
          // However, we couldn't know if OTHER controllers are sending stuff to the camera and therefore we cannot know for sure if buffers are available for our incoming commands - hence we always need to handle retransmits. But assuming we don't have other controllers, we could at least operate more clearly in this regard.
          // Also consider if the IF_clear command should ever be used...?
          sendIndex = _applicationCommandBufferCurrentIndex[cam - 1];
          Serial << "RETRANSMIT INDEX: " << sendIndex << "\n";
        }
      } else {
        if (_applicationCommandBufferLastPositionAdded[cam - 1] != _applicationCommandBufferCurrentIndex[cam - 1]) { // Something in buffer:
          sendIndex = _applicationCommandBufferCurrentIndex[cam - 1] = (_applicationCommandBufferCurrentIndex[cam - 1] + 1) % SONYVISCAIP_comBufSize;
          //      Serial << "New index to send: " << sendIndex << "\n";
        }
      }

      if (sendIndex >= 0) {
        _wipeCleanPacketBuffer();
        _packetBuffer[8] = 0x81;
        for (uint8_t a = 0; a < 15; a++) {
          _packetBuffer[9 + a] = _applicationCommandBuffer[cam - 1][sendIndex][a];
          if (_applicationCommandBuffer[cam - 1][sendIndex][a] == 0xFF) { // Found end:
            _createCommandHeader(cam, _applicationCommandBuffer[cam - 1][sendIndex][0] == 0x01 ? 0x0100 : (_applicationCommandBuffer[cam - 1][sendIndex][0] == 0x09 ? 0x0110 : 0x0000), a + 2);
            //    Serial << "Sending package, long " << (a + 2) << "\n";
            _sendPacketBuffer(cam, 8 + a + 2);
            _camLastTX[cam - 1] = millis();
            break;
          }
        }
      }
    }
  }
}

/**
 * Prints command queue - convenience and bugfixing.
 */
void ClientSonyVISCAoverIP::printCommandQueue(uint8_t cam) {
  for (uint8_t a = 1; a < SONYVISCAIP_comBufSize; a++) { // Start from the next non-sent command
    Serial << a << ": idx:" << ((_applicationCommandBufferCurrentIndex[cam - 1] + a) % SONYVISCAIP_comBufSize) << ": ";
    for (uint8_t b = 0; b < 15; b++) {
      Serial << _HEXPADL(_applicationCommandBuffer[cam - 1][(_applicationCommandBufferCurrentIndex[cam - 1] + a) % SONYVISCAIP_comBufSize][b], 2, "0") << " ";
    }
    Serial << "\n";

    if ((_applicationCommandBufferCurrentIndex[cam - 1] + a) % SONYVISCAIP_comBufSize == _applicationCommandBufferLastPositionAdded[cam - 1]) {
      break;
    }
  }
}

/**
 * Creates IP command header
 */
void ClientSonyVISCAoverIP::_createCommandHeader(uint8_t cam, const uint16_t headerCmd, const uint16_t lengthOfPayload) {
  if (camNumOK(cam)) { // valid range for VISCA
    // IP header is 8 bytes (see document "SRG-300H_Technical_Manual")
    // Payload type:
    // 0x0100: VISCA command (sent by camera)
    // 0x0110: VISCA inquiry (sent by camera)
    // 0x0111: VISCA reply (sent by peripheral)
    // 0x0120: VISCA device settings command
    // 0x0200: VISCA control command (from camera)
    // 0x0201: VISCA control reply (from peripheral)

    _packetBuffer[0] = highByte(headerCmd); // Payload type
    _packetBuffer[1] = lowByte(headerCmd);  // Payload type

    _packetBuffer[2] = highByte(lengthOfPayload); // length MSB
    _packetBuffer[3] = lowByte(lengthOfPayload);  // length LSB

    _localPacketIdCounter[cam - 1]++;
    // Actually byte 4+5 is also available for sequence numbering, but I think we may not need it, so I'm just lazy here...
    _packetBuffer[6] = highByte(_localPacketIdCounter[cam - 1]); // Local Packet ID, MSB
    _packetBuffer[7] = lowByte(_localPacketIdCounter[cam - 1]);  // Local Packet ID, LSB
  }
}

/**
 * Sends packet to camera
 */
void ClientSonyVISCAoverIP::_sendPacketBuffer(uint8_t cam, uint8_t length) {
  _cameraIP[3] = forthByteBase + cam;

  //  Serial << _cameraIP << "\n";
  _Udp.beginPacket(_cameraIP, 52381);
  _Udp.write(_packetBuffer, length);

  //  Serial << _packetBuffer << "\n";
  _Udp.endPacket();
}

/**
 * Sets all zeros in packet buffer:
 */
void ClientSonyVISCAoverIP::_wipeCleanPacketBuffer() { memset(_packetBuffer, 0, SONYVISCAIP_packetBufferLength); }

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
void ClientSonyVISCAoverIP::serialOutput(uint8_t level) { _serialOutput = level; }

/**
 * Timeout check
 */
bool ClientSonyVISCAoverIP::hasTimedOut(unsigned long time, unsigned long timeout) {
  if ((unsigned long)(time + timeout) <= (unsigned long)millis()) { // This should "wrap around" if time+timout is larger than the size of unsigned-longs, right?
    return true;
  } else {
    return false;
  }
}

bool ClientSonyVISCAoverIP::camNumOK(uint8_t cam) { return cam >= 1 && cam <= SONYVISCAIP_cams; }

/**************
 *
 * Getters and setters
 *
 **************/

/**
 * Set absolute pan-tilt position
 * TODO: Make some function that allows us to set pan and tilt separately. This only sets pan at the moment....
 */
/*
void ClientSonyVISCAoverIP::setPanPosition(uint8_t cam, long input) {

  addCommand(cam, 0x01, 0x06, 0x02,
             0x18,                                                                                                // Speed
             0x00, (input >> 16) & 0xF, (input >> 12) & 0xF, (input >> 8) & 0xF, (input >> 4) & 0xF, input & 0xF, // Pan
             0x00, 0x00, 0x00, 0x00                                                                               // Tilt
             );
  sendCommand(cam);
}

*/

void ClientSonyVISCAoverIP::setExposureMode(uint8_t cam, uint8_t mode) {
  addCommand(cam, 0x01, 0x04, 0x39, mode);
  if (camNumOK(cam))
    _ExposureMode[cam - 1] = mode;
  sendCommand(cam);
}
uint8_t ClientSonyVISCAoverIP::getExposureMode(uint8_t cam) { return _ExposureMode[cam - 1]; }
void ClientSonyVISCAoverIP::setExposureIrisUp(uint8_t cam) {
  addCommand(cam, 0x01, 0x04, 0x0B, 0x02);
  sendCommand(cam);
}
void ClientSonyVISCAoverIP::setExposureIrisDown(uint8_t cam) {
  addCommand(cam, 0x01, 0x04, 0x0B, 0x03);
  sendCommand(cam);
}
void ClientSonyVISCAoverIP::setExposureIrisDirect(uint8_t cam, uint8_t iris) {
  addCommand(cam, 0x01, 0x04, 0x4B, 0x00, 0x00, iris >> 4, iris & 0xF);
  if (camNumOK(cam))
    _ExposureIris[cam - 1] = iris;
  sendCommand(cam);
}
uint8_t ClientSonyVISCAoverIP::getExposureIris(uint8_t cam) { return _ExposureIris[cam - 1]; }
void ClientSonyVISCAoverIP::setExposureShutterUp(uint8_t cam) {
  addCommand(cam, 0x01, 0x04, 0x0A, 0x02);
  sendCommand(cam);
}
void ClientSonyVISCAoverIP::setExposureShutterDown(uint8_t cam) {
  addCommand(cam, 0x01, 0x04, 0x0A, 0x03);
  sendCommand(cam);
}
void ClientSonyVISCAoverIP::setExposureShutterDirect(uint8_t cam, uint8_t shutter) {
  addCommand(cam, 0x01, 0x04, 0x4A, 0x00, 0x00, shutter >> 4, shutter & 0xF);
  if (camNumOK(cam))
    _ExposureShutter[cam - 1] = shutter;
  sendCommand(cam);
}
uint8_t ClientSonyVISCAoverIP::getExposureShutter(uint8_t cam) { return _ExposureShutter[cam - 1]; }
