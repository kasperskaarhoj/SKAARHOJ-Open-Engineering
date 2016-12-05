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

namespace BMDCAMCTRL {
  static uint32_t lastSettingsRecall;
  static uint8_t lastLoadedPreset = 0;
  static uint8_t lastLoadedCamera = 0;

  void storeCameraPreset(const uint8_t devIndex, uint8_t camera, uint8_t num) {
    Serial << "Storing camera preset " << num << "\n";
    uint8_t preset[45];
    int16_t *p16 = (int16_t *)preset;

    memset(preset, 0, 45);
    float gamma[4], lift[4], gain[4], contrast[2], adjust[2];

    BMDCamCtrl[devIndex].getCameraLift(camera, lift);
    BMDCamCtrl[devIndex].getCameraGamma(camera, gamma);
    BMDCamCtrl[devIndex].getCameraGain(camera, gain);
    BMDCamCtrl[devIndex].getCameraContrast(camera, contrast);
    BMDCamCtrl[devIndex].getCameraColourAdjust(camera, adjust);

    p16[0] = lift[3] * (1<<11); // Y
    p16[1] = lift[0] * (1<<11); // R
    p16[2] = lift[1] * (1<<11); // G
    p16[3] = lift[2] * (1<<11); // B

    p16[4] = gamma[3] * (1<<11);
    p16[5] = gamma[0] * (1<<11);
    p16[6] = gamma[1] * (1<<11);
    p16[7] = gamma[2] * (1<<11);

    p16[8] = gain[3] * (1<<11);
    p16[9] = gain[0] * (1<<11);
    p16[10] = gain[1] * (1<<11);
    p16[11] = gain[2] * (1<<11);

    p16[12] = contrast[1] * (1<<11);
    p16[13] = adjust[1] * (1<<11);
    p16[14] = adjust[0] * (1<<11);
    p16[15] = BMDCamCtrl[devIndex].getCameraLumaMix(camera) * (1<<11);

    p16[16] = (uint16_t)BMDCamCtrl[devIndex].getExposure(camera);
    p16[17] = BMDCamCtrl[devIndex].getWhiteBalance(camera);
    p16[18] = BMDCamCtrl[devIndex].getIris(camera) * (1<<11);

    p16[19] = BMDCamCtrl[devIndex].getSensorGain(camera);


    storeUserMemory(num, PRESET_CCU, preset);
  }

  bool recallCameraPreset(const uint8_t devIndex, uint8_t camera, uint8_t num) {
    if (num < EEPROM_FILEBANK_NUM) {
      if (userMemoryExists(num, PRESET_CCU) && userMemoryChecksumMatches(num)) {
        if (num != 0) {
          if (millis() - lastSettingsRecall > 10000) {
            storeCameraPreset(devIndex, camera, 0);
          }
          lastSettingsRecall = millis();
          lastLoadedPreset = num;
          lastLoadedCamera = camera;
        } else {
          lastSettingsRecall = 0;
        }

        // Recall logic:
        uint8_t preset[45];

        recallUserMemory(num, PRESET_CCU, preset);

        int16_t *p16 = (int16_t *)preset;

        float lift[4], gamma[4], gain[4];
        float contrast[2], adjust[2];

        for(uint8_t i=0; i < 4; i++) {
          lift[(i+3)%4] = (float)p16[i]/(1<<11);
          gamma[(i+3)%4] = (float)p16[i+4]/(1<<11);
          gain[(i+3)%4] = (float)p16[i+8]/(1<<11); 
        }

        contrast[0] = 0.5;
        contrast[1] = (float)p16[12]/(1<<11);

        adjust[0] = (float)p16[14]/(1<<11);
        adjust[1] = (float)p16[13]/(1<<11);

        BMDCamCtrl[devIndex].setCameraLift(camera, lift);
        BMDCamCtrl[devIndex].setCameraGamma(camera, gamma);
        BMDCamCtrl[devIndex].setCameraGain(camera, gain);
        BMDCamCtrl[devIndex].setCameraContrast(camera, contrast);
        BMDCamCtrl[devIndex].setCameraColourAdjust(camera, adjust);
        BMDCamCtrl[devIndex].setCameraLumaMix(camera, (float)p16[15]/(1<<11));
        BMDCamCtrl[devIndex].setExposure(camera, p16[16]);
        BMDCamCtrl[devIndex].setWhiteBalance(camera, p16[17]);
        BMDCamCtrl[devIndex].setIris(camera, (float)p16[18]/(1<<11));
        BMDCamCtrl[devIndex].setSensorGain(camera, p16[19]);

        return true;
      }
    }

    return false;
  }

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
      BMDCamCtrl[devIndex].setFocus(cam, pulsesHelper(BMDCamCtrl[devIndex].getFocus(cam) * 1000.0, 0, 1000, false, pulses, 10, 100) / 1000.0);
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

    float startVal = (float)BMDCamCtrl[devIndex].getIris(cam) / (1<<11);
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
          scaler = 2.0;
          break;
      }
    }

    if(scaler == 1.0) {
      _systemHWcActionCacheFlag[HWc][actIdx] &= ~1;
      _systemHWcActionCache[HWc][actIdx] = 500;
    } else {
      if((_systemHWcActionCacheFlag[HWc][actIdx] & 1) == 0) {
        _systemHWcActionCache[HWc][actIdx] = value-((value-500)*scaler);
        _systemHWcActionCacheFlag[HWc][actIdx] |= 1;
      }
    }

    float tempVal = (float)(_systemHWcActionCache[HWc][actIdx]+(value-500)*scaler)/1000.0;
    if(tempVal > 1.0) tempVal = 1.0;
    if(tempVal < 0.0) tempVal = 0.0;


    if (actDown) {
      if (value != BINARY_EVENT) { // Value input
        outVal = 1.0 - (tempVal*((float)(limHi - limLo)/100.0) + (float)limLo/100.0);
      } else { // Binary - auto iris
        Serial << F("Perform Auto Iris... \n");
        BMDCamCtrl[devIndex].setAutoIris(cam);
        _systemHWcActionCacheFlag[HWc][actIdx] |= 2;
      }
    }

    if(actUp) {
      _systemHWcActionCacheFlag[HWc][actIdx] &= ~2;
    }

    if (pulses & 0xFFFE) {
      outVal = pulsesHelper(BMDCamCtrl[devIndex].getIris(cam) * 1000.0, 0, 1000, false, ((-(pulses >> 1)) << 1) | (pulses & B1), 10, 100) / 1000.0;
    }

    if(!actDown && value != BINARY_EVENT) {
      if(round(outVal*1000) != round((1.0 - (tempVal*((float)(limHi - limLo)/100.0) + (float)limLo/100.0)) * 1000)) {
        outVal = 1.0 - (tempVal*((float)(limHi - limLo)/100.0) + (float)limLo/100.0);
      }
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

    return _systemHWcActionCacheFlag[HWc][actIdx] & 2 ? (4 | 0x20) : 5;
    break;
  }
  case 2: // Sensor gain
  {
    cam = BMDCAMCTRL_idxToCamera(globalConfigMem[actionPtr + 1]);
    option = globalConfigMem[actionPtr + 2];
    int8_t currentValue = BMDCamCtrl[devIndex].getSensorGain(cam);
    uint8_t c = 0;
  
    if(currentValue > 0) {
      for (;(currentValue >> c) ^ 1; c++);
    }

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
        BMDCamCtrl[devIndex].setSensorGain(cam, 1 << (pulsesHelper(c, 0, 3, false, pulses, 1, 1) + 1));
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
  case 4: { // White balance
    static const uint16_t whiteBalances[] PROGMEM = {3200, 3400, 3600, 4000, 4500, 4800, 5000, 5200, 5400, 5600, 6000, 6500, 7000, 7500}; // 14
    cam = BMDCAMCTRL_idxToCamera(globalConfigMem[actionPtr + 1]);
    if (actDown || (pulses & 0xFFFE)) {
      // Find current value index:
      uint8_t currentWhiteBalanceIndex = 0;
      for (uint8_t b = 0; b < 14; b++) {
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
        BMDCamCtrl[devIndex].setWhiteBalance(cam, pgm_read_word_near(whiteBalances + pulsesHelper(currentWhiteBalanceIndex, 0, 14 - 1, false, pulses, 1, 1)));
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
    float val[4];
    BMDCamCtrl[devIndex].getCameraLift(cam, val);

    if (actDown) {
      if (value != BINARY_EVENT) {
        val[(option + 3) % 4] = (float)map(value, 0, 1000, -500, 500) / (270.0 * 16);
      } else {
        val[(option + 3) % 4] = 0.0;
      }
      BMDCamCtrl[devIndex].setCameraLift(cam, val);
    }

    if (pulses & 0xFFFE) {
      val[(option + 3) % 4] = ((float)pulsesHelper((int)(val[(option + 3) % 4] * 1000.0 * 2.0), -2000, 2000, false, pulses, 20, 100)) / (1000.0 * 2.0);
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
    float val[4];
    BMDCamCtrl[devIndex].getCameraGamma(cam, val);

    if (actDown) {
      if (value != BINARY_EVENT) {
        val[(option + 3) % 4] = (float)map(value, 0, 1000, -500, 500) / (250.0 * 16);
      } else {
        val[(option + 3) % 4] = 0.0;
      }
      BMDCamCtrl[devIndex].setCameraGamma(cam, val);
    }

    if (pulses & 0xFFFE) {
      val[(option + 3) % 4] = ((float)pulsesHelper((int)(val[(option + 3) % 4] * 1000.0 * 2.0), -2000, 2000, false, pulses, 20, 100)) / (1000.0 * 2.0);
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
    float val[4];
    BMDCamCtrl[devIndex].getCameraGain(cam, val);
    if (actDown) {
      if (value != BINARY_EVENT) {
        val[(option + 3) % 4] = (float)map(value, 0, 1000, -500, 500) / 250.0;
      } else {
        val[(option + 3) % 4] = 1.0;
      }
      BMDCamCtrl[devIndex].setCameraGain(cam, val);
    }

    if (pulses & 0xFFFE) {
      val[(option + 3) % 4] = ((float)pulsesHelper((int)(val[(option + 3) % 4] * 1000), 0, 16000, false, pulses, 10, 100)) / 1000.0;
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

    float val[2];
    BMDCamCtrl[devIndex].getCameraColourAdjust(cam, val);

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
      newValue[0] = (float)pulsesHelper((int)(val[0] * 360.0), -360, 360, true, pulses, 2, 10) / 360.0;
      BMDCamCtrl[devIndex].setCameraColourAdjust(cam, newValue);
    }

    if (extRetValIsWanted()) {
      extRetVal(map((int)(val[0] * 2048.0), -2048, 2048, 0, 360), 0, _systemHWcActionFineFlag[HWc]);
      extRetValScale(2, 0, 360, 0, 360);
      extRetValShortLabel(PSTR("Hue"));
      extRetValLongLabel(PSTR("Hue Cam "), cam);
      extRetValColor(B100111);
    }
    break;
  }
  case 9: { // Contrast
    cam = BMDCAMCTRL_idxToCamera(globalConfigMem[actionPtr + 1]);
    float newValue[2] = {0.50, NAN};
    float oldValue[2];
    BMDCamCtrl[devIndex].getCameraContrast(cam, oldValue);
  
    if (actDown) {
      if (value != BINARY_EVENT) {
        newValue[1] = (float)value / 500.0;
        BMDCamCtrl[devIndex].setCameraContrast(cam, newValue);
      } else {
        newValue[1] = 1.00;
        BMDCamCtrl[devIndex].setCameraContrast(cam, newValue);
      }
    }
  
    if (pulses & 0xFFFE) {
      newValue[1] = (float)pulsesHelper(oldValue[1] * 100, 0, 200, false, pulses, 2, 10) / 100.0;
      BMDCamCtrl[devIndex].setCameraContrast(cam, newValue);
    }
  
    if (extRetValIsWanted()) {
      extRetVal(oldValue[1] * 50, 2, _systemHWcActionFineFlag[HWc]);
      extRetValScale(1, 0, 100, 0, 100);
      extRetValShortLabel(PSTR("Contrast"));
      extRetValLongLabel(PSTR("Contrast"));
      extRetValColor(B011011);
    }
    break;
  }
  case 10: { // Saturation
    cam = BMDCAMCTRL_idxToCamera(globalConfigMem[actionPtr + 1]);
    float newValue[2] = {NAN, NAN};
    float val[2];
    BMDCamCtrl[devIndex].getCameraColourAdjust(cam, val);

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
      newValue[1] = (float)pulsesHelper((int)(val[1] * 2048.0), 0, 4096, false, pulses, 41, 410) / 2048.0;
      BMDCamCtrl[devIndex].setCameraColourAdjust(cam, newValue);
    }
    if (extRetValIsWanted()) {
      extRetVal(map((int)(val[1] * 2048.0), 0, 4096, 0, 100), 0, _systemHWcActionFineFlag[HWc]);
      extRetValScale(2, 0, 100, 0, 100);
      extRetValShortLabel(PSTR("Saturate"));
      extRetValLongLabel(PSTR("Saturate Cam "), cam);
      extRetValColor(B100111);
    }
    break;
  }
  case 11: { // Bars
    cam = BMDCAMCTRL_idxToCamera(globalConfigMem[actionPtr + 1]);
    uint8_t duration = globalConfigMem[actionPtr + 3];
    if (actDown && value == BINARY_EVENT) {
      switch (globalConfigMem[actionPtr + 2]) {
      case 0: // Toggle
        if (BMDCamCtrl[devIndex].getDisplayColorBarsTime(cam) == 0) {
          BMDCamCtrl[devIndex].setDisplayColorBarsTime(cam, duration);
        } else {
          BMDCamCtrl[devIndex].setDisplayColorBarsTime(cam, 0);
        }
        break;
      case 1: // On
        BMDCamCtrl[devIndex].setDisplayColorBarsTime(cam, duration);
        break;
      case 2: // Off
        BMDCamCtrl[devIndex].setDisplayColorBarsTime(cam, 0);
        break;
      case 3: // Hold down
        BMDCamCtrl[devIndex].setDisplayColorBarsTime(cam, 30);
        break;
      }
    }

    if (actUp && globalConfigMem[actionPtr + 2] == 3) { // Hold down activated
      BMDCamCtrl[devIndex].setDisplayColorBarsTime(cam, 0);
    }

    break;
  }
  case 12: // Detail
    cam = BMDCAMCTRL_idxToCamera(globalConfigMem[actionPtr + 1]);
    if (actDown && value == BINARY_EVENT) { // Button push
      if(globalConfigMem[actionPtr + 2] == 0) { // cycle
        BMDCamCtrl[devIndex].setVideoSharpening(cam, BMDCamCtrl[devIndex].getVideoSharpening((cam+1)%4));
      } else {
        if(globalConfigMem[actionPtr + 2] < 4) {
          BMDCamCtrl[devIndex].setVideoSharpening(cam, globalConfigMem[actionPtr + 2]-1);
        }
      }
    }

    if (pulses & 0xFFFE) {
      BMDCamCtrl[devIndex].setVideoSharpening(cam, pulsesHelper(BMDCamCtrl[devIndex].getVideoSharpening(cam), 0, 3, false, pulses, 1, 1));
    }

    if (extRetValIsWanted()) {
      extRetVal(0, 7);
      extRetValShortLabel(PSTR("Detail"));
      extRetValLongLabel(PSTR("Detail cam "), cam);

      if (_systemHWcActionPrefersLabel[HWc] && globalConfigMem[actionPtr + 2] != 0) {
        extRetValColor(retVal & 0x20 ? B011111 : B101010);
        extRetValSetLabel(true);
      } else {
        extRetValColor(B011111);
      }

      switch (BMDCamCtrl[devIndex].getVideoSharpening(cam)) {
      case 0:
        extRetValTxt_P(PSTR("Off"), 0);
        break;
      case 1:
        extRetValTxt_P(PSTR("Low"), 0);
        break;
      case 2:
        extRetValTxt_P(PSTR("Med"), 0);
        break;
      case 3:
        extRetValTxt_P(PSTR("High"), 0);
        break;
      }
    }

    break;
    break;
  case 13: // CCU Settings
    cam = BMDCAMCTRL_idxToCamera(globalConfigMem[actionPtr + 1]);

    if (actDown && value == BINARY_EVENT) {
      _systemHWcActionCache[HWc][actIdx] = millis();
      switch (globalConfigMem[actionPtr + 2]) {
      case 0:
        // For holddown events, bit 0 must be set
        _systemHWcActionCacheFlag[HWc][actIdx] = 16 | 1;
        break;
      case 1: // Recall
        if (_systemHWcActionCacheFlag[HWc][actIdx] == 4 && millis() - BMDCAMCTRL::lastSettingsRecall < 10000) {
          BMDCAMCTRL::recallCameraPreset(devIndex, cam, 0);
          _systemHWcActionCacheFlag[HWc][actIdx] = 0;
        } else {
          if (BMDCAMCTRL::recallCameraPreset(devIndex, cam, globalConfigMem[actionPtr + 3])) {
            _systemHWcActionCacheFlag[HWc][actIdx] = 4;
          } else {
            _systemHWcActionCacheFlag[HWc][actIdx] = 0;
          }
        }
        break;
      case 2: // Store
        BMDCAMCTRL::storeCameraPreset(devIndex, cam, globalConfigMem[actionPtr + 3]);
        _systemHWcActionCacheFlag[HWc][actIdx] = 2;
        break;
      }
    }

    if (_systemHWcActionCacheFlag[HWc][actIdx] & 1 && (uint16_t)millis() - _systemHWcActionCache[HWc][actIdx] > 1000) {
      switch (globalConfigMem[actionPtr + 2]) {
      case 0:
        BMDCAMCTRL::storeCameraPreset(devIndex, cam, globalConfigMem[actionPtr + 3]);
        _systemHWcActionCacheFlag[HWc][actIdx] = 2;
        _systemHWcActionCache[HWc][actIdx] = millis();
        break;
      }
    }

    if (actUp) {
      if (_systemHWcActionCacheFlag[HWc][actIdx] & 1) {
        if (globalConfigMem[actionPtr + 2] == 0) {
          if (millis() - BMDCAMCTRL::lastSettingsRecall < 10000 && BMDCAMCTRL::lastLoadedPreset == globalConfigMem[actionPtr + 3]) {
            BMDCAMCTRL::recallCameraPreset(devIndex, cam, 0);
            _systemHWcActionCacheFlag[HWc][actIdx] = 0;
          } else {
            if (BMDCAMCTRL::recallCameraPreset(devIndex, cam, globalConfigMem[actionPtr + 3])) {
              _systemHWcActionCacheFlag[HWc][actIdx] = 4;
            } else {
              _systemHWcActionCacheFlag[HWc][actIdx] = 0; 
            }
          }
        }
      } else if (globalConfigMem[actionPtr + 2] == 2) {
        _systemHWcActionCacheFlag[HWc][actIdx] = 0;
      }
    }

    if (_systemHWcActionCacheFlag[HWc][actIdx] & 16) {
      retVal = 1;
    } else if (_systemHWcActionCacheFlag[HWc][actIdx] & 8) {
      if(millis() - _systemHWcActionCache[HWc][actIdx] < 2000) {
        retVal = 2 | 0x10;
      } else {
        retVal = 5;
        _systemHWcActionCacheFlag[HWc][actIdx] = 0;
      }
    } else if (_systemHWcActionCacheFlag[HWc][actIdx] & 4) {
      if (millis() - BMDCAMCTRL::lastSettingsRecall < 10000 && BMDCAMCTRL::lastLoadedCamera == cam && BMDCAMCTRL::lastLoadedPreset == globalConfigMem[actionPtr + 3]) {
        retVal = (millis() & 512 ? 4 : 5);
      } else {
        _systemHWcActionCacheFlag[HWc][actIdx] = 0;
      }
    } else if (_systemHWcActionCacheFlag[HWc][actIdx] & 2) {
      if(millis() - _systemHWcActionCache[HWc][actIdx] < 2000) {
        retVal = 3 | 0x20;
      } else {
        retVal = 5;
        _systemHWcActionCacheFlag[HWc][actIdx] = 0;
      }
    } else if (_systemHWcActionCacheFlag[HWc][actIdx] & 1) {
      retVal = 5;
    } else {
      if (globalConfigMem[actionPtr + 2] == 2) {
        retVal = 5;
      } else {
        retVal = userMemoryExists(globalConfigMem[actionPtr + 3], PRESET_CCU) ? 5 : 0;
      }
    }

    return retVal;

    break;
  case 14: { // Reset
    cam = BMDCAMCTRL_idxToCamera(globalConfigMem[actionPtr + 1]);
    if (actDown && value == BINARY_EVENT) {
      switch (globalConfigMem[actionPtr + 2]) {
        case 0: {
          Serial << F("Resetting Lift...\n");
          float lift[4] = {0.0, 0.0, 0.0, 0.0};
          BMDCamCtrl[devIndex].setCameraLift(cam, lift);
          break;
        }
        case 1: {
          Serial << F("Resetting Gamma...\n");
          float gamma[4] = {0.0, 0.0, 0.0, 0.0};
          BMDCamCtrl[devIndex].setCameraGamma(cam, gamma);
          break;
        }
        case 2: {
          Serial << F("Resetting Gain...\n");
          float gain[4] = {1.0, 1.0, 1.0, 1.0};
          BMDCamCtrl[devIndex].setCameraGain(cam, gain);
          break;
        }
        case 3:
          Serial << F("Resetting colour settings...\n");
          BMDCamCtrl[devIndex].setCameraCorrectionReset(cam);
          break;
      }
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
