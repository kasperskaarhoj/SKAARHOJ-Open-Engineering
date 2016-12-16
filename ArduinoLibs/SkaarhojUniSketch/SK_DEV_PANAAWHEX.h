// Button return colors:
// 0 = off
// 5 = dimmed
// 1,2,3,4 = full (yellow), red, green, yellow
// Bit 4 (16) = blink flag, filter out for KP01 buttons.
// Bit 5 (32) = output bit; If this is set, a binary output will be set if coupled with this hwc.
uint16_t evaluateAction_PANAAWHEX(const uint8_t devIndex, const uint16_t actionPtr, const uint8_t HWc, const uint8_t actIdx, bool actDown, bool actUp, int16_t pulses, int16_t value) {
  uint16_t retVal = 0;
  int16_t tempInt = 0;
  uint8_t tempByte = 0;

  if (actDown || actUp) {
    if (debugMode)
      Serial << F("Panasonic AW-HEx action ") << globalConfigMem[actionPtr] << F("\n");
  }

  switch (globalConfigMem[actionPtr]) {
  case 0: // Focus
    break;
  case 1: // Iris
    if(actDown) {
      if(value == BINARY_EVENT) {

      } else {
        PANAAWHEX[devIndex].setIris(map(value, 0, 1000, 0, 1023));
      }
    }
    break;
  case 2: // Preset
    break;
  case 3: // Move
    break;

  case 4: // Gain
    if(actDown) {
      int8_t setValue = 0;
      if(value == BINARY_EVENT) {
        setValue = 0;
      } else {
        setValue = map(constrain(value, 0, 1000), 0, 1000, -100, 100);
      }

      switch(globalConfigMem[actionPtr+2]) {
        case 0:
          PANAAWHEX[devIndex].setGainR(setValue);
          break;
        case 1:
          PANAAWHEX[devIndex].setGainB(setValue);
          break;
      }
    }

    if(pulses & 0xFFFE) {
      switch(globalConfigMem[actionPtr + 2]) {
        case 0:
          PANAAWHEX[devIndex].setGainR(pulsesHelper(PANAAWHEX[devIndex].getGainR(), -100, 100, false, pulses, 2, 10));
          break;
        case 1:
          PANAAWHEX[devIndex].setGainB(pulsesHelper(PANAAWHEX[devIndex].getGainB(), -100, 100, false, pulses, 2, 10));
          break;
      }
    }


    if (extRetValIsWanted()) { // Update displays:
      switch (globalConfigMem[actionPtr + 2]) {
      case 0:
        extRetVal(map((int16_t)PANAAWHEX[devIndex].getGainR(), -100, 100, -1000,1000), 1, _systemHWcActionFineFlag[HWc]);
        break;
      case 1:
        extRetVal(map((int16_t)PANAAWHEX[devIndex].getGainB(), -100, 100, -1000, 1000), 1, _systemHWcActionFineFlag[HWc]);
        break;
      }
      extRetValShortLabel(PSTR("Gain-"));
      extRetValLongLabel(PSTR("Gain-"));
      _extRetShort[5] = globalConfigMem[actionPtr + 2] == 0 ? 'R' : 'B';
      _extRetLong[5] =  globalConfigMem[actionPtr + 2] == 0 ? 'R' : 'B';
      _extRetShortPtr++;
      _extRetLongPtr++;
      extRetValLongLabel(PSTR(" Cam"), 1);

      switch (globalConfigMem[actionPtr + 2]) {
      case 0:
        extRetValColor(B110101);
        break;
      case 1:
        extRetValColor(B010111);
        break;
      }
    }

    case 5: // Pedestal
      if(actDown) {
        int8_t setValue = 0;
        if(value == BINARY_EVENT) {
          setValue = 0;
        } else {
         setValue = map(constrain(value, 0, 1000), 0, 1000, -100, 100);
        }

        switch(globalConfigMem[actionPtr+2]) {
          case 0:
            PANAAWHEX[devIndex].setPedestalR(setValue);
            break;
          case 1:
            PANAAWHEX[devIndex].setPedestalB(setValue);
            break;
          }
        }

        if(pulses & 0xFFFE) {
          switch(globalConfigMem[actionPtr + 2]) {
          case 0:
            PANAAWHEX[devIndex].setPedestalR(pulsesHelper(PANAAWHEX[devIndex].getPedestalR(), -100, 100, false, pulses, 2, 10));
            break;
          case 1:
            PANAAWHEX[devIndex].setPedestalB(pulsesHelper(PANAAWHEX[devIndex].getPedestalB(), -100, 100, false, pulses, 2, 10));
            break;
        }
      }


      if (extRetValIsWanted()) { // Update displays:
        switch (globalConfigMem[actionPtr + 2]) {
          case 0:
            extRetVal(map((int16_t)PANAAWHEX[devIndex].getPedestalR(), -100, 100, -1000,1000), 1, _systemHWcActionFineFlag[HWc]);
            break;
          case 1:
            extRetVal(map((int16_t)PANAAWHEX[devIndex].getPedestalB(), -100, 100, -1000, 1000), 1, _systemHWcActionFineFlag[HWc]);
            break;
        }
        extRetValShortLabel(PSTR("Pedestal-"));
        extRetValLongLabel(PSTR("Pedestal-"));
        _extRetShort[9] = globalConfigMem[actionPtr + 2] == 0 ? 'R' : 'B';
        _extRetLong[9] =  globalConfigMem[actionPtr + 2] == 0 ? 'R' : 'B';
        
        _extRetShortPtr++;
        _extRetLongPtr++;
        extRetValLongLabel(PSTR(" Cam"), 1);

        switch (globalConfigMem[actionPtr + 2]) {
          case 0:
            extRetValColor(B110101);
            break;
          case 1:
            extRetValColor(B010111);
            break;
        }
      }
  }

  // Default:
  if (actDown) {
    _systemHWcActionCacheFlag[HWc][actIdx] = true;
  }
  if (actUp) {
    _systemHWcActionCacheFlag[HWc][actIdx] = false;
  }
  return _systemHWcActionCacheFlag[HWc][actIdx] ? (4 | 0x20) : 5;
}