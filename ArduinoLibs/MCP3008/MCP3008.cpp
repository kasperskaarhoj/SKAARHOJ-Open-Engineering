/*
  MCP3008.cpp - Library for communicating with MCP3008 Analog to digital converter.
  Created by Uros Petrevski, Nodesign.net 2013
  Released into the public domain.
  
  ported from Python code originaly written by Adafruit learning system for rPI :
  http://learn.adafruit.com/send-raspberry-pi-data-to-cosm/python-script
*/

#include "Arduino.h"
#include "MCP3008.h"

MCP3008::MCP3008(int clockpin, int mosipin, int misopin, int cspin) {
    
    // define SPI outputs and inputs for bitbanging
    
    _cspin = cspin;
    _clockpin = clockpin;
    _mosipin = mosipin;
    _misopin = misopin;
    
    pinMode(_cspin, OUTPUT);
    pinMode(_clockpin, OUTPUT);
    pinMode(_mosipin, OUTPUT);
    pinMode(_misopin, INPUT);
    
}

// read SPI data from MCP3008 chip, 8 possible adc's (0 thru 7)
int MCP3008::readADC(int adcnum) {

  if ((adcnum > 7) || (adcnum < 0)) return -1; // Wrong adc address return -1

  // algo
  digitalWrite(_cspin, HIGH);

  digitalWrite(_clockpin, LOW); //  # start clock low
  digitalWrite(_cspin, LOW); //     # bring CS low

  int commandout = adcnum;
  commandout |= 0x18; //  # start bit + single-ended bit
  commandout <<= 3; //    # we only need to send 5 bits here
 
  for (int i=0; i<5; i++) {
    if (commandout & 0x80) 
      digitalWrite(_mosipin, HIGH);
    else   
      digitalWrite(_mosipin, LOW);
      
    commandout <<= 1;
    digitalWrite(_clockpin, HIGH);
    digitalWrite(_clockpin, LOW);

  }

  int adcout = 0;
  // read in one empty bit, one null bit and 10 ADC bits
  for (int i=0; i<12; i++) {
    digitalWrite(_clockpin, HIGH);
    digitalWrite(_clockpin, LOW);
    adcout <<= 1;
    if (digitalRead(_misopin))
      adcout |= 0x1;
  } 
  digitalWrite(_cspin, HIGH);

  adcout >>= 1; //      # first bit is 'null' so drop it
  return adcout;
}


