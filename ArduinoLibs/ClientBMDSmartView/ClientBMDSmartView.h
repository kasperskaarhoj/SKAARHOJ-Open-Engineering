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

/**
  Version 1.0.0
        (Semantic Versioning)



  SmartView Ethernet Protocol:
        - One telnet connection at a time (port 9992)
        - Cannot Accept LF-CR (13-10), only CR (10) - therefore a Mac cannot use telnet under Terminal to send commands!!!

**/

#ifndef ClientBMDSmartView_h
#define ClientBMDSmartView_h

#include "Arduino.h"
#include "SkaarhojTCPClient.h"

#include <Ethernet.h>
//  #include "SkaarhojPgmspace.h"  - 23/2 2014

class ClientBMDSmartView : public SkaarhojTCPClient {
private:
  // States:
  uint8_t _Smart_brightness[2];
  uint8_t _Smart_contrast[2];
  uint8_t _Smart_saturation[2];
  bool _Smart_identify[2];
  uint8_t _Smart_border[2];
  uint8_t _Smart_widescreenSD[2];
  uint8_t _Smart_scopeMode[2];
  uint8_t _Smart_audioChannel[2];

public:
  ClientBMDSmartView();
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
  void _sendCmdRequest(uint8_t monId, const char *str);
  void _sendCmdRequest(uint8_t monId, const char *str, const String command);

public:
  void printStateToSerial(); // Outputs known state of device to serial (for debugging)

  // get* returns the last known state ( an internal variable in class, so it's quick); The internal state is normally updated when a value is positively sent back from the device. In this class, that happens only when connecting and the value will often stem from set* because that also sets the value internally as well as sending it to the device. If in doubt, reconnect to get a full report. In theory, if sent messages are not wrong they should always be accepted and only one device can connect at a time to SmartView, so no other device can change a value.
  // set* sends the value to the device (no matter if device is ready or not - it must/should be checked on beforehand if necessary, see isReady() and isConnected)
  uint8_t getBrightness(uint8_t monId);
  void setBrightness(uint8_t monId, uint8_t brightness);

  uint8_t getContrast(uint8_t monId);
  void setContrast(uint8_t monId, uint8_t contrast);

  uint8_t getSaturation(uint8_t monId);
  void setSaturation(uint8_t monId, uint8_t saturation);

  bool getIdentify(uint8_t monId); // Remember, getIdentify() will return what was the last known status of setIdentify - and since "identify" is turned off automatically by the monitor without further notice, we cannot count on it.
  void setIdentify(uint8_t monId, bool identify);

  uint8_t getBorder(uint8_t monId);
  void setBorder(uint8_t monId, uint8_t border);

  uint8_t getWidescreenSD(uint8_t monId);
  void setWidescreenSD(uint8_t monId, uint8_t widescreenSD);

  uint8_t getScopeMode(uint8_t monId);
  void setScopeMode(uint8_t monId, uint8_t ScopeModeIndex);

  uint8_t getAudioChannel(uint8_t monId);
  void setAudioChannel(uint8_t monId, uint8_t audioChannel);
};

#endif
