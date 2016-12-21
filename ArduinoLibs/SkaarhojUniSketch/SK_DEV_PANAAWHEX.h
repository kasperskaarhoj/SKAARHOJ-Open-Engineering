namespace PANAAWHEX {
  /**
   * Index to camera source (aligned with selector box in web interface)
   */
  uint16_t idxToCamera(uint8_t idx) {
    if (idx < 8) {
      return idx;
    } else if (idx >= 8 && idx <= 11) {
      return _systemMem[idx - 8];
    } else
      return 0;
  }

  // Button return colors:
  // 0 = off
  // 5 = dimmed
  // 1,2,3,4 = full (yellow), red, green, yellow
  // Bit 4 (16) = blink flag, filter out for KP01 buttons.
  // Bit 5 (32) = output bit; If this is set, a binary output will be set if coupled with this hwc.
  uint16_t evaluateAction(const uint8_t devIndex, const uint16_t actionPtr, const uint8_t HWc, const uint8_t actIdx, bool actDown, bool actUp, int16_t pulses, int16_t value, uint8_t HWcType) {
    uint16_t retVal = 0;
    int16_t tempInt = 0;
    uint8_t tempByte = 0;
    uint8_t cam = 0;

    if (actDown || actUp) {
      if (debugMode)
        Serial << F("Panasonic AW-HEx action ") << globalConfigMem[actionPtr] << F("\n");
    }

    switch (globalConfigMem[actionPtr]) {
    case 0: // Focus
      cam = idxToCamera(globalConfigMem[actionPtr+1]);
      if(actDown) {
        if(HWcType & HWC_BINARY) {
          PanaAWHEX[devIndex].onTouchAutofocus(cam);
        }
        if(HWcType & HWC_ANALOG) {
          PanaAWHEX[devIndex].doFocus(cam, map(value, 0, 1000, 1, 99));
        }
      }
      break;
    case 1: {// Iris
      cam = idxToCamera(globalConfigMem[actionPtr+1]);
      bool autoIris = PanaAWHEX[devIndex].getAutoIris(cam);
      if(actDown) {
        if(HWcType & HWC_BINARY) {
          PanaAWHEX[devIndex].setAutoIris(cam, !autoIris);
          _systemHWcActionCacheFlag[HWc][actIdx] = true;
        }
        if(HWcType & HWC_ANALOG) {
          PanaAWHEX[devIndex].setIris(cam, map(value, 0, 1000, 0, 1023));
        }
      }

      if(actUp && HWcType & HWC_BINARY) {
        _systemHWcActionCacheFlag[HWc][actIdx] = false;
      }

      if(pulses & 0xFFFE) {
        uint16_t iris = PanaAWHEX[devIndex].getIris(cam);
        iris = pulsesHelper(iris, 0, 1023, false, pulses, 10, 100);
        PanaAWHEX[devIndex].setIris(cam, iris);
      }

      if (extRetValIsWanted()) {
        uint16_t value = PanaAWHEX[devIndex].getIris(cam);
        extRetVal(map(value, 0, 1023, 0, 100), 2, _systemHWcActionFineFlag[HWc]);
        extRetValScale(1, 0, 100, 0, 100);
        if(autoIris) {
          extRetValShortLabel("Auto");
          extRetValLongLabel("Auto");
          extRetValSetLabel(true);
        }
        extRetValShortLabel(PSTR("Iris"));
        extRetValLongLabel(PSTR("Iris camera "), cam);
        
        extRetValColor(B011011);
      }

      return (_systemHWcActionCacheFlag[HWc][actIdx] || autoIris) ? (4 | 0x20) : 5;
      break;
    }
    case 2: // Preset
      cam = idxToCamera(globalConfigMem[actionPtr+1]);
      tempByte = globalConfigMem[actionPtr+2]; // Preset selection
      if(HWcType & HWC_BINARY) {
        if(actDown) {
          switch(globalConfigMem[actionPtr + 3]) {
            case 0:
            case 2: // Recall
              PanaAWHEX[devIndex].recallPreset(cam, tempByte-1);
              break;
            case 1: // Store
              PanaAWHEX[devIndex].storePreset(cam, tempByte-1);
              break;
            case 3: // Delete
              PanaAWHEX[devIndex].deletePreset(cam, tempByte-1);
              break;
          }
          _systemHWcActionCacheFlag[HWc][actIdx] = true;
        }
        if(actUp) {
          _systemHWcActionCacheFlag[HWc][actIdx] = false;
        }
      }

      if (extRetValIsWanted()) { // Update displays:
        extRetVal(tempByte, 0);

        snprintf(_extRetShort, 11, "Cam %d: ", cam);
        _extRetShortPtr += 6;
        switch (globalConfigMem[actionPtr + 3]) {
          case 0:
          case 2:
            extRetValShortLabel("Rec");
            break;
          case 1:
            extRetValShortLabel("Str");
            break;
          case 3:
            extRetValShortLabel("Del");
            break;
        }

        switch (globalConfigMem[actionPtr + 3]) {
          case 0:
          case 2:
            extRetValColor(0b011101);
            break;
          case 1:
            extRetValColor(0b010111);
            break;
          case 3:
            extRetValColor(0b110101);
            break;
        }
      }

      retVal = _systemHWcActionCacheFlag[HWc][actIdx] ? (4|0x20) : 5;
      return PanaAWHEX[devIndex].presetExists(cam, tempByte-1) ? retVal : 0;
      break;
    case 3: // Move
      cam = idxToCamera(globalConfigMem[actionPtr+1]);
      if(actDown) {
        if(HWcType & HWC_SPEED) {
          uint8_t val = map(constrain(value, -500, 500), -500, 500, 1, 99);
          switch(globalConfigMem[actionPtr + 2]) {
            case 0: // Pan
              PanaAWHEX[devIndex].doPan(cam, val);
              break;
            case 1: // Tilt
              PanaAWHEX[devIndex].doTilt(cam, val);
              break;
            case 2: // Zoom
              PanaAWHEX[devIndex].doZoom(cam, 100-val);
              break;
          }
        }
      }
      break;
    case 4: // Gain
      cam = idxToCamera(globalConfigMem[actionPtr+1]);
      if(actDown) {
        int8_t setValue = 0;
        if(HWcType & HWC_BINARY) {
          setValue = 0;
        } 
        if(HWcType & HWC_ANALOG) {
          setValue = map(constrain(value, 0, 1000), 0, 1000, -100, 100);
        }

        switch(globalConfigMem[actionPtr+2]) {
          case 0:
            PanaAWHEX[devIndex].setGainR(cam, setValue);
            break;
          case 1:
            PanaAWHEX[devIndex].setGainB(cam, setValue);
            break;
        }
      }

      if(pulses & 0xFFFE) {
        switch(globalConfigMem[actionPtr + 2]) {
          case 0:
            PanaAWHEX[devIndex].setGainR(cam, pulsesHelper(PanaAWHEX[devIndex].getGainR(cam), -100, 100, false, pulses, 2, 10));
            break;
          case 1:
            PanaAWHEX[devIndex].setGainB(cam, pulsesHelper(PanaAWHEX[devIndex].getGainB(cam), -100, 100, false, pulses, 2, 10));
            break;
        }
      }


      if (extRetValIsWanted()) { // Update displays:
        switch (globalConfigMem[actionPtr + 2]) {
          case 0:
            extRetVal(map((int16_t)PanaAWHEX[devIndex].getGainR(cam), -100, 100, -1000,1000), 1, _systemHWcActionFineFlag[HWc]);
            break;
          case 1:
            extRetVal(map((int16_t)PanaAWHEX[devIndex].getGainB(cam), -100, 100, -1000, 1000), 1, _systemHWcActionFineFlag[HWc]);
            break;
        }
        extRetValShortLabel(PSTR("Gain-"));
        extRetValLongLabel(PSTR("Gain-"));
        _extRetShort[5] = globalConfigMem[actionPtr + 2] == 0 ? 'R' : 'B';
        _extRetLong[5] =  globalConfigMem[actionPtr + 2] == 0 ? 'R' : 'B';
        _extRetShortPtr++;
        _extRetLongPtr++;
        extRetValLongLabel(PSTR(" Cam"), cam);

        switch (globalConfigMem[actionPtr + 2]) {
        case 0:
          extRetValColor(B110101);
          break;
        case 1:
          extRetValColor(B010111);
          break;
        }
      }
      break;
    case 5: // Pedestal
      cam = idxToCamera(globalConfigMem[actionPtr+1]);
      if(actDown) {
        int8_t setValue = 0;
        if(HWcType & HWC_BINARY) {
          setValue = 0;
        }
        if(HWcType & HWC_ANALOG) {
         setValue = map(constrain(value, 0, 1000), 0, 1000, -100, 100);
        }
        switch(globalConfigMem[actionPtr+2]) {
          case 0:
            PanaAWHEX[devIndex].setPedestalR(cam, setValue);
            break;
          case 1:
            PanaAWHEX[devIndex].setPedestalB(cam, setValue);
            break;
        }

      }

      if(pulses & 0xFFFE) {
        switch(globalConfigMem[actionPtr + 2]) {
        case 0:
          PanaAWHEX[devIndex].setPedestalR(cam, pulsesHelper(PanaAWHEX[devIndex].getPedestalR(cam), -100, 100, false, pulses, 2, 10));
          break;
        case 1:
          PanaAWHEX[devIndex].setPedestalB(cam, pulsesHelper(PanaAWHEX[devIndex].getPedestalB(cam), -100, 100, false, pulses, 2, 10));
          break;
        }
      }


      if (extRetValIsWanted()) { // Update displays:
        switch (globalConfigMem[actionPtr + 2]) {
          case 0:
            extRetVal(map((int16_t)PanaAWHEX[devIndex].getPedestalR(cam), -100, 100, -1000,1000), 1, _systemHWcActionFineFlag[HWc]);
            break;
          case 1:
            extRetVal(map((int16_t)PanaAWHEX[devIndex].getPedestalB(cam), -100, 100, -1000, 1000), 1, _systemHWcActionFineFlag[HWc]);
            break;
        }
        extRetValShortLabel(PSTR("Pedestal-"));
        extRetValLongLabel(PSTR("Pedestal-"));
        _extRetShort[9] = globalConfigMem[actionPtr + 2] == 0 ? 'R' : 'B';
        _extRetLong[9] =  globalConfigMem[actionPtr + 2] == 0 ? 'R' : 'B';
        
        _extRetShortPtr++;
        _extRetLongPtr++;
        extRetValLongLabel(PSTR(" Cam"), cam);

        switch (globalConfigMem[actionPtr + 2]) {
          case 0:
            extRetValColor(B110101);
            break;
          case 1:
            extRetValColor(B010111);
            break;
        }
      }
      break;
    case 6: // Preset recall speed
      cam = idxToCamera(globalConfigMem[actionPtr + 1]);
      if(actDown) {
        if(HWcType & HWC_BINARY) {
          uint8_t value = constrain(globalConfigMem[actionPtr + 2], 1, 30);
          PanaAWHEX[devIndex].setPresetSpeed(cam, value);
        }
        if(HWcType & HWC_ANALOG) {
          uint8_t value = map(value, 0, 1000, 0, 255);
          PanaAWHEX[devIndex].setPresetSpeed(cam, value);
        }
      }

      if(pulses & 0xFFFE) {
        uint8_t value = PanaAWHEX[devIndex].getPresetSpeed(cam);
        value = pulsesHelper(value, 1, 30, false, pulses, 1, 10);
        PanaAWHEX[devIndex].setPresetSpeed(cam, value);
      }

      if (extRetValIsWanted()) {
        uint8_t value = PanaAWHEX[devIndex].getPresetSpeed(cam);
        extRetVal(map(constrain(value, 1, 30), 1, 30, 0, 100), 2, _systemHWcActionFineFlag[HWc]);
        extRetValScale(1, 0, 100, 0, 100);
        extRetValShortLabel(PSTR("Recall spd"));
        extRetValLongLabel(PSTR("Recall speed cam"), cam);
        extRetValColor(B011011);
      }
      break;
    case 7: {// Shutter
      cam = idxToCamera(globalConfigMem[actionPtr+1]);
      int8_t valueMap[] = {-1,0,-1,2,3,4,5,6,7,8,9,0xA,0xB,0xC};
      uint8_t currentVal = PanaAWHEX[devIndex].getShutter(cam);
      uint8_t currentIdx = 0;
      
      // Find the index of the current setting
      for(uint8_t i = 0; i < sizeof(valueMap); i++) {
        if(currentVal == valueMap[i]) {
          currentIdx = i;
          break;
        }
      }

      if(actDown) {
        if(HWcType & HWC_BINARY) {
          if(globalConfigMem[actionPtr + 2] < sizeof(valueMap)) {
            uint8_t val = valueMap[globalConfigMem[actionPtr + 2]];
            PanaAWHEX[devIndex].setShutter(cam, val);
          }
        }
      }

      if(pulses & 0xFFFE) {
        int8_t newIdx = currentIdx;
        while(valueMap[newIdx = pulsesHelper(newIdx, 1, sizeof(valueMap)-1, false, pulses, 1, 1)] == -1);
        PanaAWHEX[devIndex].setShutter(cam, valueMap[newIdx]);
      }

      if (extRetValIsWanted()) {
        extRetVal(0,7);
        switch(valueMap[currentIdx]) {
          case 0:
            extRetValTxt("Off", 0);
            break;
          case 2: 
            extRetVal(60, 5);
            break;
          case 3: 
            extRetVal(100, 5);
            break;
          case 4:
            extRetVal(120, 5);
            break;
          case 5: 
            extRetVal(250, 5);
            break;
          case 6: 
            extRetVal(500, 5);
            break;
          case 7: 
            extRetVal(1000, 5);
            break;
          case 8: 
            extRetVal(2000, 5);
            break;
          case 9: 
            extRetVal(4000, 5);
            break;
          case 0xA: 
            extRetVal(10000, 5);
            break;
          case 0xB:
            extRetValTxt("Syncr", 0);
            break;
          case 0xC:
            extRetValTxt("ELC", 0);
            break;
          default:
            extRetValTxt("N/A", 0);
        }

        extRetValShortLabel(PSTR("Shutter"));
        extRetValLongLabel(PSTR("Shutter Cam "), cam);
        extRetValColor(B011110);
      }

      break;
    }
    case 8: { // Sensor gain
      cam = idxToCamera(globalConfigMem[actionPtr + 1]);
      uint8_t currentVal = PanaAWHEX[devIndex].getSensorGain(cam);

      if(actDown) {
        if(HWcType & HWC_BINARY) {
          uint8_t value = globalConfigMem[actionPtr + 2] * 3;
          PanaAWHEX[devIndex].setSensorGain(cam, value); // Values > 18 sets auto gain
        }
      }

      if(pulses & 0xFFFE) {
        if(currentVal == 0xFF) currentVal = 21;
        currentVal /= 3; // Reduce value to 0-7
        uint8_t value = pulsesHelper(currentVal, 0, 7, false, pulses, 1, 1) * 3;
        PanaAWHEX[devIndex].setSensorGain(cam, value);
      }

      if(extRetValIsWanted()) {
        extRetVal(0, 7);
        if(currentVal <= 18) {
          extRetVal(currentVal, 3);
        } else {
          extRetValTxt("Auto", 0);
        }

        extRetValShortLabel(PSTR("Sens.Gain"));
        extRetValLongLabel(PSTR("Sensor Gain cam "), cam);
        extRetValColor(B011110);
      }

      break;
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

}