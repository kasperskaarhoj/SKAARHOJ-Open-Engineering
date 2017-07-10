/*
Copyright 2012-2014 Kasper Skårhøj, SKAARHOJ, kasper@skaarhoj.com

This file is part of the Blackmagic Design Videohub Client library for Arduino

The ClientBMDVideohubTCP library is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by the
Free Software Foundation, either version 3 of the License, or (at your
option) any later version.

The ClientBMDVideohubTCP library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with the ClientBMDVideohubTCP library. If not, see http://www.gnu.org/licenses/.


IMPORTANT: If you want to use this library in your own projects and/or products,
please play a fair game and heed the license rules! See our web page for a Q&A so
you can keep a clear conscience: http://skaarhoj.com/about/licenses/


*/

/**
  Version 1.0.0
        (Semantic Versioning)
**/

#ifndef ClientBMDVideohubTCP_h
#define ClientBMDVideohubTCP_h

#include "Arduino.h"
#include "SkaarhojTCPClient.h"

#include <Ethernet.h>
#include "SkaarhojPgmspace.h" - 23 / 2 2014

#define ClientBMDVideohubTCP_NUMINPUTS 72
#define ClientBMDVideohubTCP_NUMOUTPUTS 72
#define ClientBMDVideohubTCP_LABELLEN 21

// How many in/out labels the system should allocate memory for
#define ClientBMDVideohubTCP_LABELCOUNT 16

class ClientBMDVideohubTCP : public SkaarhojTCPClient {
private:
  bool _devicePresent;

  char _labelNA[1];
  char _inputLabels[ClientBMDVideohubTCP_LABELCOUNT][ClientBMDVideohubTCP_LABELLEN];
  char _outputLabels[ClientBMDVideohubTCP_LABELCOUNT][ClientBMDVideohubTCP_LABELLEN];
  char _outputLocks[ClientBMDVideohubTCP_NUMOUTPUTS];
  uint8_t _outputRouting[ClientBMDVideohubTCP_NUMOUTPUTS];

  uint8_t _numInputs;
  uint8_t _numOutputs;
  uint8_t _numMonOutputs;
  uint8_t _numProcUnits;
  uint8_t _serialPorts;

public:
  ClientBMDVideohubTCP();
  void begin(IPAddress ip);

private: // Overloading:
  void _resetDeviceStateVariables();
  void _parseline();
  void _sendStatus();
  void _sendPing();

public:
  // "input" and "output" are the "natural" numbers starting with "1" (same as label on BNC plugs)
  // Internally - also in the protocol - these "natural numbers" actually start at 0. So on a 16 input router (1-16), internally the inputs are referred to with 0-15.
  void routeInputToOutput(uint8_t input, uint8_t output, bool waitForConfirmedChange = false);
  void setLock(uint8_t output, char newState);
  uint8_t getRoute(uint8_t output);
  char getLock(uint8_t output);
  char *getInputLabel(uint8_t input);
  char *getOutputLabel(uint8_t output);
  uint8_t getNumInputs();
  uint8_t getNumOutputs();
};

#endif