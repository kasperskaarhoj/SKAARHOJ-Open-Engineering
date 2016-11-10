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

  case 1: // Iris
    cam = BMDCAMCTRL_idxToCamera(globalConfigMem[actionPtr + 1]);
    if (actDown) {
      if (value != BINARY_EVENT) { // Value input
        BMDCamCtrl[devIndex].setIris(cam, 1.0 - (float)value / 1000.0);
      } else { // Binary - auto iris
        Serial << F("Perform Auto Iris... \n");
        BMDCamCtrl[devIndex].setAutoIris(cam);
      }
    }
    if (pulses & 0xFFFE) {
      BMDCamCtrl[devIndex].setIris(cam, pulsesHelper(BMDCamCtrl[devIndex].getIris(cam) * 100.0, 0, 100, false, ((-(pulses >> 1)) << 1) | (pulses & B1), 1, 10) / 100.0);
    }
    if (extRetValIsWanted()) {
      extRetVal((int)(100.0 - BMDCamCtrl[devIndex].getIris(cam) * 100.0), 2, _systemHWcActionFineFlag[HWc]);
      extRetValScale(1, 0, 100, 0, 100);
      extRetValShortLabel(PSTR("Iris"));
      extRetValLongLabel(PSTR("Iris Cam "), cam);
      extRetValColor(B011011);
    }
    break;

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

      if (actDown && value == BINARY_EVENT) {            // Binary (never value)
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

      if (actDown && value == BINARY_EVENT) {            // Binary (never value)
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

    if(actDown) {
      if(value != BINARY_EVENT) { // Analog input
        Serial << "Servo " << servo << " set to value " << value << "\n";
        BMDCamCtrl[devIndex].setServoSpeed(cam, servo, value);
      }
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
