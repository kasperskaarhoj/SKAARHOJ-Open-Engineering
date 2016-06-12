/*
Copyright 2014 Kasper Skårhøj, SKAARHOJ, kasper@skaarhoj.com

This file is part of the Skaarhoj Encoders library for Arduino

The SkaarhojEncoders library is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by the
Free Software Foundation, either version 3 of the License, or (at your
option) any later version.

The SkaarhojEncoders library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with the SkaarhojEncoders library. If not, see http://www.gnu.org/licenses/.


IMPORTANT: If you want to use this library in your own projects and/or products,
please play a fair game and heed the license rules! See our web page for a Q&A so
you can keep a clear conscience: http://skaarhoj.com/about/licenses/


*/
#include "SkaarhojEncoders.h"
#include "Streaming.h"

/**
 * Constructor
 */
SkaarhojEncoders::SkaarhojEncoders() {
  for (uint8_t i = 0; i < 5; i++) {
    _countOn[i] = false;
    _pushOn[i] = false;
    _pushOnTriggerTimeFired[i] = false;
    _isPushed[i] = false;
    _interruptStateNum[i] = 0;
    _interruptStateNumMem[i] = 0;
    _stateCheckTime[i] = 0;
    _interruptStateTime[i] = 0;
  }
  _stateCheckDelay = 100;

  _serialOutput = 0;
}

/**
 * Initialization
 */
void SkaarhojEncoders::begin(uint8_t address) {
  MCP23017 GPIOchip;
  _GPIOchip = GPIOchip;

  _GPIOchip.begin(address & B111);

  _GPIOchip.init();
  _GPIOchip.internalPullupMask(0xFFFF); // Pull-ups on all pins
  _GPIOchip.inputOutputMask(0xFFFF);    // All Inputs.

  _GPIOchip.setGPInterruptTriggerType(0);                       // (INTCON) Trigger interrupts on change from previous on all.
  _GPIOchip.setGPInterruptEnable((B01010101 << 8) | B00111111); // Set up which pins triggers interrupt (GPINTEN) [GPA7-GPA0,GPB7-GPB0]
}

/**
 * Checking if an interrupt has occured:
 */
void SkaarhojEncoders::runLoop() {
  bool directionUp;
  // Serial.print("*");
  word buttonStatus = _GPIOchip.getGPInterruptTriggerPin();
  if (buttonStatus) {                                 // Some interrupt was triggered at all...
    word capture = _GPIOchip.getGPInterruptCapture(); // Capture states of GPI at the time of interrupt
                                                      //		Serial.print("!");
                                                      /*   Serial << _BINPADL(buttonStatus,16,0) << F("\n");
                                                         Serial << _BINPADL(capture,16,0) << F("\n");
                                                             Serial.println("---");
                                                     */
    // First 4 encoders (GPA0-7), turning:
    for (uint8_t b = 0; b < 4; b++) {
      if ((buttonStatus >> 8) & (B1 << (b << 1))) {  // Was this pin causing the interrupt?
                                                     /*	        if ((capture >> 8) & (B1<<(b<<1)))  {	// Check pin A polarity
                                                                       if ((capture >> 8) & (B10<<(b<<1)))  {	// Check pin B (direction)
                                                                         _interruptStateNum[b]--;
                                                                                     directionUp = false;
                                                                       }
                                                                       else {
                                                                         _interruptStateNum[b]++;
                                                                                     directionUp = true;
                                                                       }
                                             
                                                                               if (_serialOutput)	{
                                                                                       Serial.print(F("Encoder "));
                                                                                       Serial.print(b);
                                                                                       Serial.print(F(": "));
                                                                                       Serial.print(directionUp ? F("UP,   ") : F("DOWN, "));
                                                                                       Serial.print(_interruptStateNum[b]);
                                                                                       Serial.println();
                                                                               }
                                                                               _interruptStateTime[b] = millis();
                                                                     }*/
        if (!((capture >> 8) & (B10 << (b << 1)))) { // Check pin A polarity and pin B direction
          if (((capture >> 8) & (B1 << (b << 1)))) {
            _interruptStateNum[b]++;
            directionUp = true;
          } else {
            _interruptStateNum[b]--;
            directionUp = true;
          }
          if (_serialOutput) {
            Serial.print(F("Encoder "));
            Serial.print(b);
            Serial.print(F(": "));
            Serial.print(directionUp ? F("UP,   ") : F("DOWN, "));
            Serial.print(_interruptStateNum[b]);
            Serial.println();
          }
          _interruptStateTime[b] = millis();
        }
      }
    }

    // Fifth encoder:
    if (buttonStatus & (B100000)) { // Was this pin causing the interrupt?
      if (capture & (B100000)) {    // Check pin A polarity
        if (capture & (B1000000)) { // Check pin B (direction)
          _interruptStateNum[4]--;
          directionUp = false;
        } else {
          _interruptStateNum[4]++;
          directionUp = true;
        }

        if (_serialOutput) {
          Serial.print(F("Encoder "));
          Serial.print(4);
          Serial.print(F(": "));
          Serial.print(directionUp ? F("UP,   ") : F("DOWN, "));
          Serial.print(_interruptStateNum[4]);
          Serial.println();
        }

        _interruptStateTime[4] = millis();
      }
    }

    // 5 buttons:
    for (uint8_t b = 0; b < 5; b++) {
      if (buttonStatus & (B1 << b)) { // Was this pin causing the interrupt?
        if (!(capture & (B1 << b))) { // Check polarity
          _pushOnMillis[b] = millis();
          _isPushed[b] = true;

          if (_serialOutput) {
            Serial.print(F("Button "));
            Serial.print(b);
            Serial.print(F(" pushed"));
            Serial.println();
          }
        } else {
          _isPushed[b] = false;
          if (_serialOutput) {
            Serial.print(F("Button "));
            Serial.print(b);
            Serial.print(F(" released"));
            Serial.println();
          }
        }
      }
    }
  }
}

bool SkaarhojEncoders::reset(uint8_t encNum) {
  _interruptStateTime[encNum] = 0;
  _stateCheckTime[encNum] = 0;
  _interruptStateLastCount[encNum] = 0;
  _isPushed[encNum] = false;
  _pushOn[encNum] = false;
  
  return true;
}


/**
 *
 */
int SkaarhojEncoders::state(uint8_t encNum) { return state(encNum, 0); }

/**
 *
 */
int SkaarhojEncoders::state(uint8_t encNum, unsigned int buttonPushTriggerDelay) {
  // Check:
  int retVal = 0;
  if (encNum < 5 && hasTimedOut(_stateCheckTime[encNum], _stateCheckDelay)) { // This delay allows us a way to detect "fast" rotations because the interrupt may have increased the counters more than one time between each check.
                                                                              // If rotations has been detected in either direction:
    if (_interruptStateNumMem[encNum] != _interruptStateNum[encNum]) {

      // Check if the last interrupt generated signal is younger than 1000 ms, only then will we accept it, otherwise we ignore.
      if (!hasTimedOut(_interruptStateTime[encNum], 1000)) {
        _interruptStateLastCount[encNum] = _interruptStateNum[encNum] - _interruptStateNumMem[encNum];
      } else {
        _interruptStateLastCount[encNum] = 0;
      }
      _interruptStateNumMem[encNum] = _interruptStateNum[encNum];

      retVal = _interruptStateLastCount[encNum] != 0 ? (_interruptStateLastCount[encNum] < 0 ? -1 : 1) : 0;
    }

    //		bool isPushed = !_GPIOchip.digitalRead(encNum);
    if (retVal == 0) {
      if (_isPushed[encNum]) {
        if (!_pushOn[encNum]) {
          _pushOn[encNum] = true;
          _pushOnTriggerTimeFired[encNum] = false;
          retVal = 2;
        }
        if (!_pushOnTriggerTimeFired[encNum] && buttonPushTriggerDelay > 0 && hasTimedOut(_pushOnMillis[encNum], buttonPushTriggerDelay)) {
          _pushOnTriggerTimeFired[encNum] = true;
          retVal = (unsigned long)millis() - _pushOnMillis[encNum];
        }
      } else {
        if (_pushOn[encNum]) {
          _pushOn[encNum] = false;
          if (!_pushOnTriggerTimeFired[encNum]) {
            retVal = (unsigned long)millis() - _pushOnMillis[encNum];
          }
        }
      }
    }
  }
  // Default return
  if (retVal != 0) {
    _stateCheckTime[encNum] = millis();
  }
  return retVal;
}

/**
 *
 */
int SkaarhojEncoders::lastCount(uint8_t encNum, uint8_t boost) {
  if (boost <= 1) {
    return _interruptStateLastCount[encNum];
  } else {
    if (_interruptStateLastCount[encNum] != 0) {
      return (_interruptStateLastCount[encNum] > 0 ? 1 : -1) * ((abs(_interruptStateLastCount[encNum]) - 1) * boost + 1);
    } else {
      return 0;
    }
  }
}

/**
 *
 */
int SkaarhojEncoders::totalCount(uint8_t encNum) { return _interruptStateNum[encNum]; }

/**
 *
 */
void SkaarhojEncoders::setStateCheckDelay(uint16_t delayTime) { _stateCheckDelay = delayTime; }

/**
 * Timeout check
 */
bool SkaarhojEncoders::hasTimedOut(unsigned long time, unsigned long timeout) {
  if ((unsigned long)(time + timeout) <= (unsigned long)millis()) { // This should "wrap around" if time+timout is larger than the size of unsigned-longs, right?
    return true;
  } else {
    return false;
  }
}

/**
 * Setter method: If _serialOutput is set, the library may use Serial.print() to give away information about its operation - mostly for debugging.
 * 0= no output
 * 1= normal output (info)
 * 2= verbose (development)
 * 3= verbose! (development, bugfixing)
 */
void SkaarhojEncoders::serialOutput(uint8_t level) { _serialOutput = level; }

void SkaarhojEncoders::testProgramme(uint8_t buttonMask) {
  runLoop();

  for (uint8_t i = 0; i < 5; i++) {
    if (buttonMask & (B1 << i)) {

      int encValue = state(i, 1000);
      if (encValue) {
        Serial << F("Enc ") << i << F(": ") << encValue;
        if (encValue == 1 || encValue == -1) {
          Serial << F(" Count: ") << lastCount(i);
        }
        Serial << F("\n");
      }
    }
  }
}