/*
Copyright 2013-2014 Kasper Skårhøj, SKAARHOJ, kasper@skaarhoj.com

This file is part of the Blackmagic Design Teranex Client library for Arduino

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

#ifndef ClientSKAARHOJUniSketchTCP_h
#define ClientSKAARHOJUniSketchTCP_h

#include "Arduino.h"
#include "SkaarhojTCPClient.h"

#include <Ethernet.h>

#define ClientSKAARHOJUniSketchTCP_IMGBUF 4
#define ClientSKAARHOJUniSketchTCP_TXTBUF 16


class ClientSKAARHOJUniSketchTCP : public SkaarhojTCPClient {
private:
  uint8_t _deviceState_HWC[128];
  uint8_t _deviceState_HWCc[128];
  uint8_t _deviceState_MEM[12];
  bool _deviceState_FLAG[64];
  uint8_t _deviceState_SHIFT[5];
  uint8_t _deviceState_STATE[5];
  bool _deviceState_INACTIVEPANEL;

  uint8_t _deviceState_HWCmap[128];
  uint8_t _deviceState_HWCmap_prev[128];

  uint8_t _imgBuf[ClientSKAARHOJUniSketchTCP_IMGBUF][256];
  uint8_t _imgBufHWC[ClientSKAARHOJUniSketchTCP_IMGBUF];
  uint8_t _imgBufRdy[ClientSKAARHOJUniSketchTCP_IMGBUF];

  char _txtBuf[ClientSKAARHOJUniSketchTCP_TXTBUF][64];
  uint8_t _txtBufHWC[ClientSKAARHOJUniSketchTCP_TXTBUF];

  bool busy;

public:
  ClientSKAARHOJUniSketchTCP();
  void begin(IPAddress ip);
  void initializeConnection();

private: // Overloading:
  void _resetDeviceStateVariables();
  void _parseline();
  void _sendPing();
  void _sendStatus();
  void _sendCmdRequest(const String command);

  void _sendBusy();
  void _sendReady();

public:
  bool getFlag(uint8_t flag);
  void setFlag(uint8_t flag, bool value);
  uint8_t getMem(uint8_t idx);
  void setMem(uint8_t idx, uint8_t value);
  uint8_t getShift(uint8_t idx);
  void setShift(uint8_t idx, uint8_t value);
  uint8_t getState(uint8_t idx);
  void setState(uint8_t idx, uint8_t value);
  uint8_t getInactivePanel();
  void setInactivePanel(bool value);

  uint8_t getHWCoutput(uint8_t hwc);
  uint8_t getHWCcolor(uint8_t hwc);
  void sendHWC_Press(uint8_t hwc);
  void sendHWC_Down(uint8_t hwc);
  void sendHWC_Up(uint8_t hwc);
  void sendHWC_Enc(uint8_t hwc, int pulseCount);
  void sendHWC_Abs(uint8_t hwc, int value);
  void sendHWC_Speed(uint8_t hwc, int value);

  void base64_decodeImgPart(uint8_t slot, uint8_t part);

  uint8_t getGfxSlotForHWC(uint8_t HWC);
  uint8_t findGfxSlotForHWC(uint8_t HWC, bool mustBeReady=false);
  uint8_t *getGfxForIdx(uint8_t idx);
  void resetGfxSlots();
  void resetTxtSlots();
  void resetReadyGfxSlots();
  bool isFreeGfxSlots();

  uint8_t findTxtSlotForHWC(uint8_t HWC);
  char *getTxtForIdx(uint8_t idx);
  bool isFreeTxtSlots();
  uint8_t getTxtSlotForHWC(uint8_t HWC);



  void setHWCmap(uint8_t hwcLocal, uint8_t hwcRemote);
  void resetHWCmap();
  void sendAndUpdateServerHWCMap();

};

#endif
