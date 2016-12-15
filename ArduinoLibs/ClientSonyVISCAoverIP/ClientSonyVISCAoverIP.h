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

#define SONYVISCAIP_packetBufferLength 24 // Size of packet buffer
#define SONYVISCAIP_cams 7                // Number of cameras in chain
#define SONYVISCAIP_comBufSize 8          // Number of commands in queue

#define SONYVISCAIP_debug 1 // If "1" (true), more debugging information may hit the serial monitor, in particular when _serialDebug = 0x80. Setting this to "0" is recommended for production environments since it saves on flash memory.

class ClientSonyVISCAoverIP {
protected:
#ifdef ESP8266
  WiFiUDP _Udp;
#else
  EthernetUDP _Udp; // UDP object for communication, see constructor.
#endif

  IPAddress _cameraIP; // IP base address of the cameras
  uint8_t forthByteBase;
  uint8_t _serialOutput; // If set, the library will print status/debug information to the Serial object

  // SONYVISCAIP Connection Basics
  boolean _hasInitialized[SONYVISCAIP_cams]; // If true, all initial payload packets has been received during requests for resent - and we are completely ready to rock!

  uint8_t _packetBuffer[SONYVISCAIP_packetBufferLength]; // Buffer for storing segments of the packets from cameras and creating answer packets. Shared between all cameras.

  boolean _isConnected[SONYVISCAIP_cams];           // Set true if we have received a response from the camera.
  uint16_t _localPacketIdCounter[SONYVISCAIP_cams]; // This is our counter for the outgoing command packages we might like to send to camera
  uint8_t _applicationCommandBuffer[SONYVISCAIP_cams][SONYVISCAIP_comBufSize][15];
  uint8_t _applicationCommandBufferCurrentIndex[SONYVISCAIP_cams];      // Current index from command buffer under execution, waiting for ack.
  uint8_t _applicationCommandBufferLastPositionAdded[SONYVISCAIP_cams]; // Last added index from command buffer.
  unsigned long _lastContact[SONYVISCAIP_cams];                         // Last time (millis) the camera sent a packet to us.
  unsigned long _camLastTX[SONYVISCAIP_cams];
  uint8_t _camOnline[SONYVISCAIP_cams]; // If true, a given camera (cam-1) is "online" which means we will try to send commands to it and keep coms going.
  uint8_t _nextStateUpdate[SONYVISCAIP_cams];
  bool _pushedStateUpdate[SONYVISCAIP_cams];

  // Camera state data:
  uint16_t _CAM_VersionInq_model[SONYVISCAIP_cams];
  long _CAM_PanPos[SONYVISCAIP_cams];
  int16_t _CAM_TiltPos[SONYVISCAIP_cams];

public:
  ClientSonyVISCAoverIP();
  void begin(const IPAddress ip);
  void connect(uint8_t cam);
  void runLoop(uint16_t delayTime = 0);
  void parsePacketBufferInqueryData(uint8_t cam);
  uint8_t pushNextStateUpdate(uint8_t cam);

  bool isConnected(uint8_t cam);
  bool hasInitialized(uint8_t cam);

  bool addCommand(uint8_t cam, uint8_t QQ, uint8_t RR, uint16_t cmd, uint16_t p1 = 0x100, uint16_t p2 = 0x100, uint16_t p3 = 0x100, uint16_t p4 = 0x100, uint16_t p5 = 0x100, uint16_t p6 = 0x100, uint16_t p7 = 0x100, uint16_t p8 = 0x100, uint16_t p9 = 0x100, uint16_t p10 = 0x100, uint16_t p11 = 0x100);
  void sendCommand(uint8_t cam);
  void printCommandQueue(uint8_t cam);

  void serialOutput(uint8_t level);
  bool hasTimedOut(unsigned long time, unsigned long timeout);
  bool camNumOK(uint8_t cam);

protected:
  void _createCommandHeader(uint8_t cam, const uint16_t headerCmd, const uint16_t lengthOfPayload);
  void _sendPacketBuffer(uint8_t cam, uint8_t length);
  void _wipeCleanPacketBuffer();

  // *********************************
  // **
  // ** Implementations in ClientSonyVISCAoverIP.h:
  // **
  // *********************************

private:
  // Private Variables in ClientSonyVISCAoverIP.h:
  uint8_t _ExposureMode[SONYVISCAIP_cams];
  uint8_t _ExposureIris[SONYVISCAIP_cams];
  uint8_t _ExposureShutter[SONYVISCAIP_cams];

public:
  //  void setPanPosition(uint8_t cam, long input);

  void setExposureMode(uint8_t cam, uint8_t mode);
  uint8_t getExposureMode(uint8_t cam);
  void setExposureIrisUp(uint8_t cam);
  void setExposureIrisDown(uint8_t cam);
  void setExposureIrisDirect(uint8_t cam, uint8_t iris);
  uint8_t getExposureIris(uint8_t cam);
  void setExposureShutterUp(uint8_t cam);
  void setExposureShutterDown(uint8_t cam);
  void setExposureShutterDirect(uint8_t cam, uint8_t shutter);
  uint8_t getExposureShutter(uint8_t cam);
};

#endif
