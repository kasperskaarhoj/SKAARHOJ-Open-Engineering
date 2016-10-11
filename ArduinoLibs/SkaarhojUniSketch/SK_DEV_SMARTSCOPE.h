// Button return colors:
// 0 = off
// 5 = dimmed
// 1,2,3,4 = full (yellow), red, green, yellow
// Bit 4 (16) = blink flag, filter out for KP01 buttons.
uint16_t evaluateAction_SMARTSCOPE(const uint8_t devIndex, const uint16_t actionPtr, const uint8_t HWc, const uint8_t actIdx, const bool actDown = false, const bool actUp = false, const uint8_t pulses = 0, const uint16_t value = 0) {
  uint16_t retVal = 0;
  if (actDown || actUp) {
    Serial << "SMART SCOPE action " << globalConfigMem[actionPtr] << "\n";
  }
  int16_t val;

  if (globalConfigMem[actionPtr] <= 2) {
    if (actDown) {
      switch (globalConfigMem[actionPtr]) {
      case 0:
        val = SmartView[devIndex].getBrightness(globalConfigMem[actionPtr + 1]);
        break;
      case 1:
        val = SmartView[devIndex].getContrast(globalConfigMem[actionPtr + 1]);
        break;
      case 2:
        val = SmartView[devIndex].getSaturation(globalConfigMem[actionPtr + 1]);
        break;
      }
      Serial << val << ",";

      val += (globalConfigMem[actionPtr + 2] == 1 ? -7 : 7); // Change value
      Serial << val << ",";
      if (globalConfigMem[actionPtr + 2] == 2) { // Cycle value
        val += val < 0 ? 200 : (val > 100 ? -200 : 0);
        Serial << val << ",";
      }
      val = constrain(val, 0, 100); // Conform to 0-100
      Serial << val << "\n";

      switch (globalConfigMem[actionPtr]) {
      case 0:
        SmartView[devIndex].setBrightness(globalConfigMem[actionPtr + 1], val);
        break;
      case 1:
        SmartView[devIndex].setContrast(globalConfigMem[actionPtr + 1], val);
        break;
      case 2:
        SmartView[devIndex].setSaturation(globalConfigMem[actionPtr + 1], val);
        break;
      }
      _systemHWcActionCacheFlag[HWc][actIdx] = true;
    }
    if (actUp) {
      _systemHWcActionCacheFlag[HWc][actIdx] = false;
    }

    extRetVal(SmartView[devIndex].getBrightness(globalConfigMem[actionPtr + 1]));
    extRetValLongLabel(PSTR("Brightness "));
    extRetValLongLabel(PSTR("AB"));
    extRetValLongLabel(PSTR("CD"));
    extRetValLongLabel(PSTR("EF"));
    extRetValLongLabel(PSTR("GH"));
    //	extRetValShortLabel(PSTR("Bright. "));
    //	extRetValAddNumber(1);

    return _systemHWcActionCacheFlag[HWc][actIdx] ? 4 : 5;
  } else if (globalConfigMem[actionPtr] == 3) {
    if (actDown) {
      uint8_t scopeMap[] = {0, 8, 7, 6, 4, 5, 3, 1, 2};
      uint8_t revScopeMap[] = {0, 7, 8, 6, 4, 5, 3, 2, 1};
      if (globalConfigMem[actionPtr + 2] > 8) { // Cycle
        val = scopeMap[(revScopeMap[SmartView[devIndex].getScopeMode(globalConfigMem[actionPtr + 1])] + 1) % 9];
        Serial << val << ",";
      } else {
        val = scopeMap[globalConfigMem[actionPtr + 2]];
        Serial << val << ",";

        if (SmartView[devIndex].getScopeMode(globalConfigMem[actionPtr + 1]) != val)
          _systemHWcActionCache[HWc][actIdx] = SmartView[devIndex].getScopeMode(globalConfigMem[actionPtr + 1]);
        if (globalConfigMem[actionPtr + 3] == 1 && val != SmartView[devIndex].getScopeMode(globalConfigMem[actionPtr + 1])) {
          val = _systemHWcActionCache[HWc][actIdx];
        }
        Serial << val << ",";
      }
      Serial << val << "\n";
      SmartView[devIndex].setScopeMode(globalConfigMem[actionPtr + 1], val);
    }
    if (actUp && globalConfigMem[actionPtr + 3] == 2 && globalConfigMem[actionPtr + 2] <= 8) {
      SmartView[devIndex].setScopeMode(globalConfigMem[actionPtr + 1], _systemHWcActionCache[HWc][actIdx]);
      Serial << _systemHWcActionCache[HWc][actIdx] << "\n";
    }
    return SmartView[devIndex].getScopeMode(globalConfigMem[actionPtr + 1]) == val ? 4 : 5;
  }
  return retVal;
}