/*
Copyright 2017 Kasper Skårhøj, SKAARHOJ, kasper@skaarhoj.com

This file is part of the Blackmagic Design Multiview Client library for Arduino

The ClientBMDMultiviewTCP library is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by the
Free Software Foundation, either version 3 of the License, or (at your
option) any later version.

The ClientBMDMultiviewTCP library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with the ClientBMDMultiviewTCP library. If not, see http://www.gnu.org/licenses/.


IMPORTANT: If you want to use this library in your own projects and/or products,
please play a fair game and heed the license rules! See our web page for a Q&A so
you can keep a clear conscience: http://skaarhoj.com/about/licenses/


*/

/**
  Version 1.0.0
        (Semantic Versioning)
**/

#ifndef ClientBMDMultiviewTCP_h
#define ClientBMDMultiviewTCP_h

#include "Arduino.h"
#include "SkaarhojTCPClient.h"

#include <Ethernet.h>
#include "SkaarhojPgmspace.h"

#define ClientBMDMultiviewTCP_NUMINPUTS 16
#define ClientBMDMultiviewTCP_NUMOUTPUTS 18
#define ClientBMDMultiviewTCP_LABELLEN 21

// How many in/out labels the system should allocate memory for
#define ClientBMDMultiviewTCP_LABELCOUNT 18

class ClientBMDMultiviewTCP : public SkaarhojTCPClient {
private:
  bool _devicePresent;

  char _inputLabels[ClientBMDMultiviewTCP_LABELCOUNT][ClientBMDMultiviewTCP_LABELLEN];
  char _outputLabels[ClientBMDMultiviewTCP_LABELCOUNT][ClientBMDMultiviewTCP_LABELLEN];
  char _outputLocks[ClientBMDMultiviewTCP_NUMOUTPUTS];
  uint8_t _outputRouting[ClientBMDMultiviewTCP_NUMOUTPUTS];

  uint8_t _numInputs;
  uint8_t _numOutputs;
  uint8_t _numProcUnits;
  uint8_t _serialPorts;


  bool _soloEnabled;
  bool _wideSrcSDEnabled;
  bool _displayBorder;
  bool _displayLabels;
  bool _displayAudioMeters;
  bool _displaySDITally;  

  uint8_t _layout;

public:
  ClientBMDMultiviewTCP();
  void begin(IPAddress ip);

private: // Overloading:
  void _resetDeviceStateVariables();
  void _parseline();
  void _sendStatus();
  void _sendPing();

public:
  // "input" and "output" are the "natural" numbers starting with "1" (same as label on BNC plugs)
  // Internally - also in the protocol - these "natural numbers" actually start at 0. So on a 16 input router (1-16), internally the inputs are referred to with 0-15.
  void routeInputToOutput(uint8_t input, uint8_t output);
  void setLock(uint8_t output, char newState);
  uint8_t getRoute(uint8_t output);
  char getLock(uint8_t output);
  char *getInputLabel(uint8_t input);
  char *getOutputLabel(uint8_t output);
  uint8_t getNumInputs();
  uint8_t getNumOutputs();

  void setSolo(bool newState);
  bool getSolo();
  void setWideScreenSD(bool newState);
  bool getWideScreenSD();
  void setDisplayBorder(bool newState);
  bool getDisplayBorder();
  void setDisplayLabels(bool newState);
  bool getDisplayLabels();
  void setDisplayAudioMeters(bool newState);
  bool getDisplayAudioMeters();
  void setDisplaySDITally(bool newState);
  bool getDisplaySDITally();
  void setLayout(uint8_t layout);
  uint8_t getLayout();

};

#endif