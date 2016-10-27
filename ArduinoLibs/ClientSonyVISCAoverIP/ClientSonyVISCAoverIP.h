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

#ifndef ClientSonyVISCAoverIP_h
#define ClientSonyVISCAoverIP_h

#include "Arduino.h"

#ifdef ESP8266
#include <WifiUDP.h>
#else
#include <EthernetUdp.h>
#endif

#include <SkaarhojPgmspace.h>

#define SONYVISCAIP_packetBufferLength 96  // Size of packet buffer

#define SONYVISCAIP_debug 1 // If "1" (true), more debugging information may hit the serial monitor, in particular when _serialDebug = 0x80. Setting this to "0" is recommended for production environments since it saves on flash memory.

class ClientSonyVISCAoverIP {
protected:
#ifdef ESP8266
  WiFiUDP _Udp;
#else
  EthernetUDP _Udp; // UDP object for communication, see constructor.
#endif
  IPAddress _cameraIP; // IP address of the camera
  uint8_t _serialOutput; // If set, the library will print status/debug information to the Serial object

  // SONYVISCAIP Connection Basics
  uint16_t _localPacketIdCounter; // This is our counter for the command packages we might like to send to camera
  boolean _hasInitialized;        // If true, all initial payload packets has been received during requests for resent - and we are completely ready to rock!
  boolean _isConnected;           // Set true if we have received a response from the camera.
  boolean _initPayloadSent;
  unsigned long _lastContact;     // Last time (millis) the camera sent a packet to us.
  uint8_t _returnPacketLength;
  uint16_t _sessionID;

  uint8_t _packetBuffer[SONYVISCAIP_packetBufferLength]; // Buffer for storing segments of the packets from camera and creating answer packets.

  uint16_t _cmdLength;  // Used when parsing packets
  uint16_t _cmdPointer; // Used when parsing packets

  bool _cBundle; // If set, we are building a set-command bundle.
  uint8_t _cBBO; // Bundle Buffer Offset; This is an offset if you want to add more commands.

public:
  ClientSonyVISCAoverIP();
  void begin(const IPAddress ip);
  void connect();
  void runLoop(uint16_t delayTime = 0);

  bool isConnected();
  bool hasInitialized();

  void serialOutput(uint8_t level);
  bool hasTimedOut(unsigned long time, unsigned long timeout);

protected:
  void _createCommandHeader(const uint16_t headerCmd, const uint16_t lengthOfPayload);
  void _sendPacketBuffer(uint8_t length);
  void _wipeCleanPacketBuffer();



// *********************************
// **
// ** Implementations in ClientSonyVISCAoverIP.h:
// **
// *********************************

private:
	
			// Private Variables in ClientSonyVISCAoverIP.h:
			uint8_t cameraIris;
	
public:
	
			uint8_t getIris();
			void setIris(uint8_t input);
};









#endif
