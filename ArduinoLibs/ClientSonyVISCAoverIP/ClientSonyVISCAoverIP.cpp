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

/**
 * Constructor
 */
ClientSonyVISCAoverIP::ClientSonyVISCAoverIP() {}

/**
 * Setting up IP address for the switcher
 */
void ClientSonyVISCAoverIP::begin(const IPAddress ip) {

// Set up Udp communication object:
#ifdef ESP8266
  WiFiUDP Udp;
#else
  EthernetUDP Udp;
#endif

  _Udp = Udp;

  _cameraIP = ip; // Set Camera IP address

  _lastContact = 0;
  _serialOutput = 0;
}

/**
 * Initiating connection handshake to Sony Visca UDP
 */
void ClientSonyVISCAoverIP::connect() {

  _localPacketIdCounter = 0; // Init localPacketIDCounter to 0;
  _hasInitialized = false;   // Will be true after initial payload of data is resent and received well
  _initPayloadSent = false;
  _isConnected = false;    // Will be true after the initial hello-package handshakes.
  _lastContact = millis(); // Setting this, because even though we haven't had contact, it constitutes an attempt that should be responded to at least
  
  // The cameras will send information back on this port, so we need to listen on it - but it would be OK to send from a different port - but that would require two sockets or what?? TODO check this out.
  uint16_t portNumber = 52381; // random(53100, 65300);

  _Udp.begin(portNumber);	

  // Send connectString to camera:
  if (_serialOutput) {
    Serial.print(F("Sending connect packet to Sony Visca camera on IP "));
    Serial.print(_cameraIP);
    Serial.print(F(" from port "));
    Serial.println(portNumber);
  }

  _wipeCleanPacketBuffer();
  _packetBuffer[8] = 0x01;
  _createCommandHeader(0x0200, 1);
  _sendPacketBuffer(8 + 1);
}

/**
 * Keeps connection to the switcher alive
 * Therefore: Call this in the Arduino loop() function and make sure it gets call at least 2 times a second
 * Other recommendations might come up in the future.
 */
void ClientSonyVISCAoverIP::runLoop(uint16_t delayTime) {

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

        _Udp.read(_packetBuffer, 8); // Read header
        uint16_t payloadType = word(_packetBuffer[0], _packetBuffer[1]);
        uint16_t packetLength = word(_packetBuffer[2], _packetBuffer[3]);
        _Udp.read(_packetBuffer, SONYVISCAIP_packetBufferLength);

        if (packetSize == packetLength + 8) { // Just to make sure these are equal, they should be!
          _lastContact = millis();

          if (payloadType == 0x0201 && packetLength == 1 && _packetBuffer[0] == 1) { // Reponse to connect?
            _isConnected = true;
            if (_serialOutput & 0x80) {
              Serial.println(F("Connected to VISCA camera"));
            }
          }
        } else {
#if SONYVISCAIP_debug
          if (_serialOutput & 0x80) {
            Serial.print(F("ERROR: Packet size mismatch: "));
            Serial.print(packetSize, DEC);
            Serial.print(F(" != "));
            Serial.println(packetLength, DEC);
          }
#endif
          // Flushing larger packets
          while (_Udp.available()) {
            _Udp.read(_packetBuffer, SONYVISCAIP_packetBufferLength);
          }
        }
      } else
        break;
    }

    // After initialization, we check which packages were missed and ask for them:
    if (!_hasInitialized && _initPayloadSent) {
      _hasInitialized = true;
      if (_serialOutput) {
        Serial.println(F("Camera _hasInitialized = TRUE"));
      }
    }
  } while (delayTime > 0 && !hasTimedOut(enterTime, delayTime));

  // If connection is gone anyway, try to reconnect:
  if (hasTimedOut(_lastContact, 5000)) {
    if (_serialOutput)
      Serial.println(F("Connection to Camera has timed out - reconnecting!"));
    connect();
  }
}

/**
 * If true, we had a response from the switcher when trying to send a hello packet.
 */
bool ClientSonyVISCAoverIP::isConnected() { return _isConnected; }

/**
 * If true, the initial handshake and "stressful" information exchange has occured and now the switcher connection should be ready for operation.
 */
bool ClientSonyVISCAoverIP::hasInitialized() { return _hasInitialized; }

/**************
 *
 * Buffer work
 *
 **************/

void ClientSonyVISCAoverIP::_createCommandHeader(const uint16_t headerCmd, const uint16_t lengthOfPayload) {
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

  _localPacketIdCounter++;
  // Actually byte 4+5 is also available for sequence numbering, but I think we may not need it, so I'm just lazy here...
  _packetBuffer[6] = highByte(_localPacketIdCounter); // Local Packet ID, MSB
  _packetBuffer[7] = lowByte(_localPacketIdCounter);  // Local Packet ID, LSB
}
void ClientSonyVISCAoverIP::_sendPacketBuffer(uint8_t length) {
  _Udp.beginPacket(_cameraIP, 52381);
  _Udp.write(_packetBuffer, length);
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







/**
 * Get Iris
 */
uint8_t ClientSonyVISCAoverIP::getIris() { return cameraIris; }

/**
 * Set Iris
 * input 	0x00-0x19: Closed - Open (1.9)
 */
void ClientSonyVISCAoverIP::setIris(uint8_t input) {

  _wipeCleanPacketBuffer();

  _packetBuffer[8] = 0x81;
  _packetBuffer[9] = 0x01;
  _packetBuffer[10] = 0x04;
  _packetBuffer[11] = 0x4B;
  _packetBuffer[14] = input >> 4;
  _packetBuffer[15] = input & 0xF;
  _packetBuffer[16] = 0xFF;
  _createCommandHeader(0x0110, 9);
  _sendPacketBuffer(8 + 9);
}
