/**
 * Index to camera source (aligned with selector box in web interface)
 */
uint16_t BMDCAMCTRL_idxToCamera(uint8_t idx) {
  if (idx < 10) {
    return idx + 1;
  } else if (idx >= 10 && idx <= 13) {
    return _systemMem[idx - 10];
  } else
    return 0;
}

// Button return colors:
// 0 = off
// 5 = dimmed
// 1,2,3,4 = full (yellow), red, green, yellow
// Bit 4 (16) = blink flag, filter out for KP01 buttons.
uint16_t evaluateAction_BMDCAMCTRL(const uint8_t devIndex, const uint16_t actionPtr, const uint8_t HWc, const uint8_t actIdx, bool actDown, bool actUp, int16_t pulses, int16_t value) {
  uint16_t retVal = 0;
  int16_t tempInt = 0;
  uint8_t tempByte = 0;

  uint8_t cam = 0;
  uint8_t option = 0;
  uint16_t aSrc = 0;
  char yrgbLabels[5] = "YRGB";

  if (actDown || actUp) {
    if (debugMode)
      Serial << F("BMDCAM action ") << globalConfigMem[actionPtr] << F("\n");
  }

  switch (globalConfigMem[actionPtr]) {
  case 0: // Focus
    cam = BMDCAMCTRL_idxToCamera(globalConfigMem[actionPtr + 1]);
    if (actDown) {
      if (value != BINARY_EVENT) {
        BMDCamCtrl[devIndex].setFocus(cam, (float)value / 1000.0);
      } else {
        Serial << F("Performing auto focus...");
        BMDCamCtrl[devIndex].setAutoFocus(cam);
      }
    }

    if (pulses & 0xFFFE) {
      BMDCamCtrl[devIndex].setFocus(cam, pulsesHelper(BMDCamCtrl[devIndex].getFocus(cam) * 100.0, 0, 100, false, pulses, 1, 10) / 100.0);
    }

    if (extRetValIsWanted()) {
      extRetVal((int)(BMDCamCtrl[devIndex].getFocus(cam) * 1000.0), 1, _systemHWcActionFineFlag[HWc]);
      extRetValShortLabel(PSTR("Focus"));
      extRetValLongLabel(PSTR("Focus Cam "), cam);
      extRetValColor(B011011);
    }

    break;

  case 1: {// Iris
    cam = BMDCAMCTRL_idxToCamera(globalConfigMem[actionPtr + 1]);

    uint16_t limLo, limHi;
    if (globalConfigMem[actionPtr + 2] > 0 && globalConfigMem[actionPtr + 2] <= 4) {
      limLo = _systemRangeLower[globalConfigMem[actionPtr + 2] - 1] * 100 / 255;
      limHi = _systemRangeUpper[globalConfigMem[actionPtr + 2] - 1] * 100 / 255;
    } else {
      limLo = 0;
      limHi = 100;
    }

    float startVal = BMDCamCtrl[devIndex].getIris(cam);
    float outVal = startVal;

    float scaler = 1.0;
    
    // Scalers
    if(globalConfigMem[actionPtr + 3] > 0 && globalConfigMem[actionPtr + 3] <= 4) {
      switch(_systemScaler[globalConfigMem[actionPtr + 3] - 1]) {
        case 1:
          scaler = 0.5;
          break;
        case 2:
          scaler = 0.25;
          break;
        case 3:
          scaler = 2;
          break;
      }
    }

    if(scaler == 1.0) {
      _systemHWcActionCacheFlag[HWc][actIdx] = false;
      _systemHWcActionCache[HWc][actIdx] = 500;
    } else {
      if(!_systemHWcActionCacheFlag[HWc][actIdx]) {
        _systemHWcActionCache[HWc][actIdx] = value-((value-500)*scaler);
        _systemHWcActionCacheFlag[HWc][actIdx] = true;
      }
    }

    if (actDown) {
      if (value != BINARY_EVENT) { // Value input
        if(scaler != 1.0) {
          outVal = 1.0 - (float)(_systemHWcActionCache[HWc][actIdx]+(value-500)*scaler)/1000.0;
        } else {
          outVal = 1.0 - (float)value / 1000.0;
        }
      } else { // Binary - auto iris
        Serial << F("Perform Auto Iris... \n");
        BMDCamCtrl[devIndex].setAutoIris(cam);
      }
    }

    if (pulses & 0xFFFE) {
      outVal = pulsesHelper(BMDCamCtrl[devIndex].getIris(cam) * 1000.0, 0, 1000, false, ((-(pulses >> 1)) << 1) | (pulses & B1), 10, 100) / 1000.0;
    }


    if (round(100.0 - outVal * 100.0) > limHi) {
      if(scaler != 1.0) {
        _systemHWcActionCache[HWc][actIdx] -= ((1000.0 - outVal * 1000.0) - limHi*10);
        _systemHWcActionCache[HWc][actIdx] = constrain(_systemHWcActionCache[HWc][actIdx], 10*limLo, 10*limHi);
      }
      outVal = (100.0 - (float)limHi)/100.0;
    }
    if (round(100.0 - outVal * 100.0) < limLo) {
      if(scaler != 1.0) {
        _systemHWcActionCache[HWc][actIdx] -= ((1000.0 - outVal * 1000.0) - limLo*10);
        _systemHWcActionCache[HWc][actIdx] = constrain(_systemHWcActionCache[HWc][actIdx], 10*limLo, 10*limHi);
      }
      outVal = (100.0 - (float)limLo)/100.0;
    }

    if(startVal != outVal) {
      BMDCamCtrl[devIndex].setIris(cam, outVal);
    }

    if (extRetValIsWanted()) {
      extRetVal(round(100.0 - BMDCamCtrl[devIndex].getIris(cam) * 100.0), 2, _systemHWcActionFineFlag[HWc]);
      extRetValScale(1, 0, 100, limLo, limHi);
      extRetValShortLabel(PSTR("Iris"));
      extRetValLongLabel(PSTR("Iris Cam "), cam);
      extRetValColor(B011011);
    }
    break;
  }
  case 2: // Sensor gain
  {
    cam = BMDCAMCTRL_idxToCamera(globalConfigMem[actionPtr + 1]);
    option = globalConfigMem[actionPtr + 2];
    int8_t currentValue = BMDCamCtrl[devIndex].getSensorGain(cam);

    uint8_t c = 0;
    for (; (currentValue >> c) ^ 1; c++)
      ;

    if (c > 0)
      c -= 1;

    if (actDown && value == BINARY_EVENT) {
      if (option == 0) { // Cycle
        pulses = 1;
      } else {
        // BMDCamCtrl[devIndex].setSensorGain(cam, 1 << ((c + pulses) % 4));
        Serial.print("TODO: Setting specific sensor gain value");
        // Serial.println(1 << ((c + pulses) % 4));
      }
    }

    if ((pulses & 0xFFFE)) {
      BMDCamCtrl[devIndex].setSensorGain(cam, 1 << (((c + (pulses >> 1)) % 4) + 1));
    }

    if (extRetValIsWanted()) {
      // Original value 19.9315
      extRetVal(round(log10((((int)BMDCamCtrl[devIndex].getSensorGain(cam) >> 1))) * 20.0), 3, _systemHWcActionFineFlag[HWc]);
      extRetValShortLabel(PSTR("Sens.Gain"));
      extRetValLongLabel(PSTR("Sens.Gain Cam "), cam);
      extRetValColor(B011110);
    }

    break;
  }
  case 3: { // Shutter
    static const uint16_t shutterSpeeds[] PROGMEM = {20000, 16667, 13333, 11111, 10000, 8333, 6667, 5556, 4000, 2778, 2000, 1379, 1000, 690, 500};
    cam = BMDCAMCTRL_idxToCamera(globalConfigMem[actionPtr + 1]);
    if (actDown || (pulses & 0xFFFE)) {
      // Find current value index:
      uint8_t currentShutterSpeedIndex = 0;
      for (uint8_t b = 0; b < 15; b++) {
        if (BMDCamCtrl[devIndex].getExposure(cam) >= (pgm_read_word_near(shutterSpeeds + b) - 1)) {
          currentShutterSpeedIndex = b;
          break;
        }
      }

      if (actDown && value == BINARY_EVENT) {      // Binary (never value)
        if (globalConfigMem[actionPtr + 2] == 0) { // cycle
          pulses = 2;
        } else { // Set
          BMDCamCtrl[devIndex].setExposure(cam, pgm_read_word_near(shutterSpeeds + globalConfigMem[actionPtr + 2] - 1));
        }
      }
      if ((pulses & 0xFFFE)) {
        BMDCamCtrl[devIndex].setExposure(cam, pgm_read_word_near(shutterSpeeds + pulsesHelper(currentShutterSpeedIndex, 0, 15 - 1, false, pulses, 1, 1)));
      }
    }
    if (extRetValIsWanted()) {
      extRetVal(round((float)1000000 / BMDCamCtrl[devIndex].getExposure(cam)), 5, _systemHWcActionFineFlag[HWc]);
      extRetValShortLabel(PSTR("Shutter"));
      extRetValLongLabel(PSTR("Shutter Cam "), cam);
      extRetValColor(B011110);
    }

    break;
  }
  case 4: {                                                                                                                               // White balance
    static const uint16_t whiteBalances[] PROGMEM = {3200, 3400, 3600, 4000, 4500, 4800, 5000, 5200, 5400, 5600, 6000, 6500, 7000, 7500}; // 18
    cam = BMDCAMCTRL_idxToCamera(globalConfigMem[actionPtr + 1]);
    if (actDown || (pulses & 0xFFFE)) {
      // Find current value index:
      uint8_t currentWhiteBalanceIndex = 0;
      for (uint8_t b = 0; b < 18; b++) {
        if (BMDCamCtrl[devIndex].getWhiteBalance(cam) <= (pgm_read_word_near(whiteBalances + b) + 1)) {
          currentWhiteBalanceIndex = b;
          break;
        }
      }

      if (actDown && value == BINARY_EVENT) {      // Binary (never value)
        if (globalConfigMem[actionPtr + 2] == 0) { // cycle
          pulses = 2;
        } else { // Set
          BMDCamCtrl[devIndex].setWhiteBalance(cam, pgm_read_word_near(whiteBalances + globalConfigMem[actionPtr + 2] - 1));
        }
      }
      if ((pulses & 0xFFFE)) {
        BMDCamCtrl[devIndex].setWhiteBalance(cam, pgm_read_word_near(whiteBalances + pulsesHelper(currentWhiteBalanceIndex, 0, 18 - 1, false, pulses, 1, 1)));
      }
    }
    if (extRetValIsWanted()) {
      extRetVal(BMDCamCtrl[devIndex].getWhiteBalance(cam), 6, _systemHWcActionFineFlag[HWc]);
      extRetValShortLabel(PSTR("WhiteBal"));
      extRetValLongLabel(PSTR("WhiteBal Cam "), cam);
      extRetValColor(B011110);
    }
    break;
  }
  case 5: { // Lift
    cam = BMDCAMCTRL_idxToCamera(globalConfigMem[actionPtr + 2]);
    option = globalConfigMem[actionPtr + 1];
    float(&val)[4] = BMDCamCtrl[devIndex].getCameraLift(cam);
    if (actDown) {
      if (value != BINARY_EVENT) {
        val[(option + 3) % 4] = (float)map(value, 0, 1000, -500, 500) / (250.0 * 16);
      } else {
        val[(option + 3) % 4] = 0.0;
      }
      BMDCamCtrl[devIndex].setCameraLift(cam, val);
    }

    if (pulses & 0xFFFE) {
      val[(option + 3) % 4] = ((float)pulsesHelper((int)(val[(option + 3) % 4] * 100.0 * 2.0), -200, 200, false, pulses, 1, 10)) / (100.0 * 2.0);
      BMDCamCtrl[devIndex].setCameraLift(cam, val);
    }

    if (extRetValIsWanted()) {
      extRetVal(val[(option + 3) % 4] * 1e3, 1, _systemHWcActionFineFlag[HWc]);
      extRetValShortLabel(PSTR("Lift-"));
      extRetValLongLabel(PSTR("Lift-"));
      _extRetShort[5] = yrgbLabels[option];
      _extRetLong[5] = yrgbLabels[option];
      extRetValColor(B011011);
    }
    break;
  }
  case 6: { // Gamma
    cam = BMDCAMCTRL_idxToCamera(globalConfigMem[actionPtr + 2]);
    option = globalConfigMem[actionPtr + 1];
    float(&val)[4] = BMDCamCtrl[devIndex].getCameraGamma(cam);
    if (actDown) {
      if (value != BINARY_EVENT) {
        val[(option + 3) % 4] = (float)map(value, 0, 1000, -500, 500) / (250.0 * 16);
      } else {
        val[(option + 3) % 4] = 0.0;
      }
      BMDCamCtrl[devIndex].setCameraGamma(cam, val);
    }

    if (pulses & 0xFFFE) {
      val[(option + 3) % 4] = ((float)pulsesHelper((int)(val[(option + 3) % 4] * 100.0 * 2.0), -200, 200, false, pulses, 1, 10)) / (100.0 * 2.0);
      BMDCamCtrl[devIndex].setCameraGamma(cam, val);
    }

    if (extRetValIsWanted()) {
      extRetVal(val[(option + 3) % 4] * 1e3, 1, _systemHWcActionFineFlag[HWc]);
      extRetValShortLabel(PSTR("Gamma-"));
      extRetValLongLabel(PSTR("Gamma-"));
      _extRetShort[6] = yrgbLabels[option];
      _extRetLong[6] = yrgbLabels[option];
      extRetValColor(B011011);
    }
    break;
  }
  case 7: { // Gain
    cam = BMDCAMCTRL_idxToCamera(globalConfigMem[actionPtr + 2]);
    option = globalConfigMem[actionPtr + 1];
    float(&val)[4] = BMDCamCtrl[devIndex].getCameraGain(cam);
    if (actDown) {
      if (value != BINARY_EVENT) {
        val[(option + 3) % 4] = (float)map(value, 0, 1000, -500, 500) / 250.0;
      } else {
        val[(option + 3) % 4] = 1.0;
      }
      BMDCamCtrl[devIndex].setCameraGain(cam, val);
    }

    if (pulses & 0xFFFE) {
      val[(option + 3) % 4] = ((float)pulsesHelper((int)(val[(option + 3) % 4] * 100), 0, 1600, false, pulses, 1, 10)) / 100.0;
      BMDCamCtrl[devIndex].setCameraGain(cam, val);
    }

    if (extRetValIsWanted()) {
      extRetVal(val[(option + 3) % 4] * 1e3, 1, _systemHWcActionFineFlag[HWc]);
      extRetValShortLabel(PSTR("Gain-"));
      extRetValLongLabel(PSTR("Gain-"));
      _extRetShort[5] = yrgbLabels[option];
      _extRetLong[5] = yrgbLabels[option];
      extRetValColor(B011011);
    }
    break;
  }

  case 8: { // Hue
    cam = BMDCAMCTRL_idxToCamera(globalConfigMem[actionPtr + 1]);
    float newValue[2] = {NAN, NAN};

    if (actDown) {
      if (value != BINARY_EVENT) { // Value input
        newValue[0] = (float)(value - 500) / 500.0;
        BMDCamCtrl[devIndex].setCameraColourAdjust(cam, newValue);
      } else { // Binary - reset
        newValue[0] = 0.0;
        BMDCamCtrl[devIndex].setCameraColourAdjust(cam, newValue);
      }
    }
    if (pulses & 0xFFFE) {
      newValue[0] = (float)pulsesHelper((int)(BMDCamCtrl[devIndex].getCameraColourAdjust(cam)[0] * 360.0), -360, 360, true, pulses, 1, 10) / 360.0;
      BMDCamCtrl[devIndex].setCameraColourAdjust(cam, newValue);
    }
    if (extRetValIsWanted()) {
      extRetVal(map((int)(BMDCamCtrl[devIndex].getCameraColourAdjust(cam)[0] * 2048.0), -2048, 2048, 0, 360), 0, _systemHWcActionFineFlag[HWc]);
      extRetValScale(2, 0, 360, 0, 360);
      extRetValShortLabel(PSTR("Hue"));
      extRetValLongLabel(PSTR("Hue Cam "), cam);
      extRetValColor(B100111);
    }
    break;
  }
  // case 9: { // Contrast
  //   cam = BMDCAMCTRL_idxToCamera(globalConfigMem[actionPtr + 1]);
  //   float newValue[2] = {0.50, NAN};
  //   float(&oldValue)[2] = BMDCamCtrl[devIndex].getCameraContrast(cam);
  //
  //   if (actDown) {
  //     if (value != BINARY_EVENT) {
  //       newValue[1] = (float)value / 500.0;
  //       BMDCamCtrl[devIndex].setCameraContrast(cam, newValue);
  //     } else {
  //       newValue[1] = 1.00;
  //       BMDCamCtrl[devIndex].setCameraContrast(cam, newValue);
  //     }
  //   }
  //
  //   if (pulses & 0xFFFE) {
  //     newValue[1] = (float)pulsesHelper(oldValue[1] * 100, 0, 200, false, pulses, 1, 10) / 100.0;
  //     BMDCamCtrl[devIndex].setCameraContrast(cam, newValue);
  //   }
  //
  //   if (extRetValIsWanted()) {
  //     extRetVal(oldValue[1] * 50, 2, _systemHWcActionFineFlag[HWc]);
  //     extRetValScale(1, 0, 100, 0, 100);
  //     extRetValShortLabel(PSTR("Contrast"));
  //     extRetValLongLabel(PSTR("Contrast"));
  //     extRetValColor(B011011);
  //   }
  //   break;
  // }
  case 10: { // Saturation
    cam = BMDCAMCTRL_idxToCamera(globalConfigMem[actionPtr + 1]);
    float newValue[2] = {NAN, NAN};

    if (actDown) {
      if (value != BINARY_EVENT) { // Value input
        newValue[1] = (float)(value) / 500.0;
        BMDCamCtrl[devIndex].setCameraColourAdjust(cam, newValue);
      } else { // Binary - reset
        newValue[1] = 1.0;
        BMDCamCtrl[devIndex].setCameraColourAdjust(cam, newValue);
      }
    }
    if (pulses & 0xFFFE) {
      newValue[1] = (float)pulsesHelper((int)(BMDCamCtrl[devIndex].getCameraColourAdjust(cam)[1] * 2048.0), 0, 4096, true, pulses, 2, 20) / 2048.0;
      BMDCamCtrl[devIndex].setCameraColourAdjust(cam, newValue);
    }
    if (extRetValIsWanted()) {
      extRetVal(map((int)(BMDCamCtrl[devIndex].getCameraColourAdjust(cam)[1] * 2048.0), 0, 4096, 0, 100), 0, _systemHWcActionFineFlag[HWc]);
      extRetValScale(2, 0, 100, 0, 100);
      extRetValShortLabel(PSTR("Saturate"));
      extRetValLongLabel(PSTR("Saturate Cam "), cam);
      extRetValColor(B100111);
    }
    break;
  }

  case 15: { // Servo
    cam = BMDCAMCTRL_idxToCamera(globalConfigMem[actionPtr + 1]);
    uint8_t servo = globalConfigMem[actionPtr + 2];
    uint8_t direction = globalConfigMem[actionPtr + 3];

    if (actDown) {
      if (value != BINARY_EVENT) { // Analog input
        Serial << "Servo " << servo << " set to value " << value << "\n";
        BMDCamCtrl[devIndex].setServoSpeed(cam, servo, value);
      }
    }
    break;
  }

  case 16:{ // Tally
    retVal = 5;
    cam = BMDCAMCTRL_idxToCamera(globalConfigMem[actionPtr + 1]);
    bool prv, pgm;
    BMDCamCtrl[devIndex].getTally(cam, pgm, prv);

    if(actDown && value == BINARY_EVENT) {
      bool toggle = globalConfigMem[actionPtr + 3] > 0;

      switch(globalConfigMem[actionPtr + 2]) {
        case 0: // Tally off
          BMDCamCtrl[devIndex].setTally(cam, false, false);
          break;
        case 1: // Red tally
          BMDCamCtrl[devIndex].setTally(cam, (toggle?!pgm:pgm), false);
          break;
        case 2: // Green tally
          BMDCamCtrl[devIndex].setTally(cam, false, (toggle?!prv:prv));
          break;
        case 3: // Both
          BMDCamCtrl[devIndex].setTally(cam, (toggle?!pgm:pgm), (toggle?!prv:prv));
          break;
      }

      _systemHWcActionCacheFlag[HWc][actIdx] = (pgm << 2) | (prv << 1) | 1;
    }

    if(pulses & 0xFFFE) {
      uint8_t currentTally = pgm << 1 | prv;
      switch(globalConfigMem[actionPtr + 2]) {
          case 0: // Tally off
            BMDCamCtrl[devIndex].setTally(cam, false, false);
            break;
          case 1: // Red tally
            BMDCamCtrl[devIndex].setTally(cam, !pgm, prv);
            break;
          case 2: // Green tally
            BMDCamCtrl[devIndex].setTally(cam, pgm, !prv);
            break;
          case 3: // Both
            uint8_t tally = pulsesHelper(currentTally, 0, 3, true, pulses, 1, 1);
            BMDCamCtrl[devIndex].setTally(cam, (tally >> 1) & 1, tally & 1);
            break;
      }
    }

    if(actUp && _systemHWcActionCacheFlag[HWc][actIdx] && globalConfigMem[actionPtr+3]==1) {
        if(globalConfigMem[actionPtr + 3] == 1) {
          BMDCamCtrl[devIndex].setTally(cam, (_systemHWcActionCacheFlag[HWc][actIdx] >> 2) & 1, (_systemHWcActionCacheFlag[HWc][actIdx] >> 1) & 1);
        }
        _systemHWcActionCacheFlag[HWc][actIdx] = false;
    }

    // Set return values 
    switch(globalConfigMem[actionPtr + 2]) {
        case 1: // Red tally
          retVal = pgm ? (2 | 0x20) : 5;
          break;
        case 2: // Green tally
          retVal = prv ? (3 | 0x20) : 5;
          break;
        case 3: // Both
          retVal = pgm ? (2 | 0x20) : (prv ? (3 | 0x20) : 5);
          break;
    }

    if (extRetValIsWanted()) {
      extRetVal(0, 7);
      switch (globalConfigMem[actionPtr + 2]) {
      case 1:
        extRetValColor(retVal & 0x20 ? B110101 : B101010);
        extRetValLongLabel(PSTR("Red Tally"));
        break;
      case 2:
        extRetValColor(retVal & 0x20 ? B011101 : B101010);
        extRetValShortLabel(PSTR("Grn Tally"));
        extRetValLongLabel(PSTR("Green Tally"));
        break;
      case 3:
        extRetValColor((retVal & 0xF) == 3 ? B011101 : ((retVal & 0xF) == 2 ? B110101 : B101010));
        extRetValShortLabel(PSTR("RedGrn Tly"));
        extRetValLongLabel(PSTR("Red/Green Tally"));
        break;
      }

      extRetValSetLabel(true);
      extRetValTxt("Cam ", 0);
      sprintf(_extRetTxt[0]+4, "%d", cam);
    }

    return retVal;
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
