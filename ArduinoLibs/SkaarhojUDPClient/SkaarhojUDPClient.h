/*
Copyright 2014 Kasper Skårhøj, SKAARHOJ, kasper@skaarhoj.com

This file is part of the Skaarhoj Serial Client Superclass library for Arduino

The SkaarhojUDPClient library is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by the
Free Software Foundation, either version 3 of the License, or (at your
option) any later version.

The SkaarhojUDPClient library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with the SkaarhojUDPClient library. If not, see http://www.gnu.org/licenses/.


IMPORTANT: If you want to use this library in your own projects and/or products,
please play a fair game and heed the license rules! See our web page for a Q&A so
you can keep a clear conscience: http://skaarhoj.com/about/licenses/

*/

/**
  Version 1.0.0
        (Semantic Versioning)
**/

#ifndef SkaarhojUDPClient_h
#define SkaarhojUDPClient_h

#include "Arduino.h"
#include "SkaarhojASCIIClient.h"

#ifdef ESP8266
#include <WifiUDP.h>
#else
#include <EthernetUdp.h>
#endif

class SkaarhojUDPClient : public SkaarhojASCIIClient {

protected:
#ifdef ESP8266
  WiFiUDP _Udp;
#else
  EthernetUDP _Udp; // UDP object for communication, see constructor.
#endif
  uint16_t _localPort;  // Default local port to send from.
  uint16_t _remotePort; // Default remote port to send to.
  IPAddress _remoteIP;  // IP address of the remote device

public:
  SkaarhojUDPClient();
  void begin(const IPAddress ip, const uint16_t localPort, const uint16_t remotePort);

  void connect();

  void runLoop();                   //  Keeps connection alive; Looks for incoming traffic and processes that, if any.
  void runLoop(uint16_t delayTime); // As runLoop but keeps running for delayTime ms before returning (thus implementing a small delay function while still keeping connection alive. )

protected:
  virtual void _runSubLoop();
  virtual void _readFromClient();
  virtual void _initialize();

  virtual void _sendBuffer();
};

#endif
