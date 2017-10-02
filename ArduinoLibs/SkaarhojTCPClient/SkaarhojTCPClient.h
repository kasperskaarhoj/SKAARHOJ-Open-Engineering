/*
Copyright 2014 Kasper Skårhøj, SKAARHOJ, kasper@skaarhoj.com

This file is part of the Skaarhoj TCP Client Superclass library for Arduino

The SkaarhojTCPClient library is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by the
Free Software Foundation, either version 3 of the License, or (at your
option) any later version.

The SkaarhojTCPClient library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with the SkaarhojTCPClient library. If not, see http://www.gnu.org/licenses/.


IMPORTANT: If you want to use this library in your own projects and/or products,
please play a fair game and heed the license rules! See our web page for a Q&A so
you can keep a clear conscience: http://skaarhoj.com/about/licenses/

*/

/**
  Version 1.0.0
        (Semantic Versioning)
**/

#ifndef SkaarhojTCPClient_h
#define SkaarhojTCPClient_h

#include "Arduino.h"
#include "SkaarhojASCIIClient.h"

#include <Ethernet.h>
//  #include "SkaarhojPgmspace.h"  - 23/2 2014

class SkaarhojTCPClient : public SkaarhojASCIIClient {
protected:
  IPAddress _IP;       // server IP address
  uint16_t _localPort; // local port on server

  uint16_t _ackMsgResponseTimeout;
  bool _exitRunLoop;

  uint8_t TCPReadBuffer[128];
  uint8_t TCPReadBuffer_len;
  uint8_t TCPReadBuffer_ptr;

  uint16_t _EOLTimeOutTime;
  unsigned long _EOLtimer;

public:
  EthernetClient _client; // Object for communication, see constructor.

public:
  SkaarhojTCPClient();
  void begin(IPAddress ip);
  void begin(IPAddress ip, uint16_t port);

  virtual void connect();    // Connects to device
  virtual void disconnect(); // Disconnects from device
  virtual void stopClient(); // Stop ethernet client
  virtual void initializeConnection(); // Initialize connection

  void runLoop();                   //  Keeps connection alive; Looks for incoming traffic and processes that, if any.
  void runLoop(uint16_t delayTime); // As runLoop but keeps running for delayTime ms before returning (thus implementing a small delay function while still keeping connection alive. )
  bool hasInitialized();

  // Has to put "virtual" in front of all methods that I want to potentially overload in subclass. Otherwise it doesn't get called super class. Slightly weird I think...
protected:
  virtual void _runSubLoop() {};
  virtual void _sendBusy()  {};
  virtual void _sendReady()  {};

  virtual void _readFromClient();

  void _EOLTimeOutReset();
  bool _EOLTimeOut();

  bool incomingAvailable();
  char incomingRead();

  virtual void _sendBuffer();
};

#endif
