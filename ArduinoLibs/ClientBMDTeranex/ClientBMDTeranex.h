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

#ifndef ClientBMDTeranex_h
#define ClientBMDTeranex_h

#include "Arduino.h"
#include "SkaarhojTCPClient.h"

#include <Ethernet.h>


class ClientBMDTeranex : public SkaarhojTCPClient {
private:
  // States:
  uint8_t _Teranex_LUT;

public:
  ClientBMDTeranex();
  void begin(IPAddress ip);

  bool isReady();      // Returns true if device can send a new message (may return false if it's currently waiting for a sent message to be acknowledged)
  bool waitForReady(); // Waits (while calling runLoop()) until device is ready to receive new message (or max 100 ms) and returns
  bool waitForReady(uint16_t delayTime);
  bool waitForInit(uint16_t delayTime);

private: // Overloading:
  void _resetDeviceStateVariables();
  void _parseline();
  void _sendPing();

private:
  void _sendCmdRequest(uint8_t section, const char *str);
  void _sendCmdRequest(uint8_t section, const char *str, const String command);

public:
  void printStateToSerial(); // Outputs known state of device to serial (for debugging)

  // get* returns the last known state ( an internal variable in class, so it's quick); The internal state is normally updated when a value is positively sent back from the device. In this class, that happens only when connecting and the value will often stem from set* because that also sets the value internally as well as sending it to the device. If in doubt, reconnect to get a full report. In theory, if sent messages are not wrong they should always be accepted and only one device can connect at a time to Teranex, so no other device can change a value.
  // set* sends the value to the device (no matter if device is ready or not - it must/should be checked on beforehand if necessary, see isReady() and isConnected)
  uint8_t getLut();
  void setLut(uint8_t section);
};

#endif
