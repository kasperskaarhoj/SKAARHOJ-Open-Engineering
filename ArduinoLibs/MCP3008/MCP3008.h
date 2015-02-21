/*
  MCP3008.h - Library for communicating with MCP3008 Analog to digital converter.
  Created by Uros Petrevski, Nodesign.net 2013
  Released into the public domain.
  
  ported from Python code originaly written by Adafruit learning system for rPI :
  http://learn.adafruit.com/send-raspberry-pi-data-to-cosm/python-script
*/

#ifndef MCP3008_h
#define MCP3008_h

#include "Arduino.h"

class MCP3008
{
  public:
    MCP3008(int clockpin, int mosipin, int misopin, int cspin);
    int readADC(int adcnum);
  private:
      int _clockpin, _mosipin, _misopin, _cspin;
};


#endif
