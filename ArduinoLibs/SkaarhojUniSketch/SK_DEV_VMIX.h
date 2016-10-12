// Button return colors:
// 0 = off
// 5 = dimmed
// 1,2,3,4 = full (yellow), red, green, yellow
// Bit 4 (16) = blink flag, filter out for KP01 buttons.
// Bit 5 (32) = output bit; If this is set, a binary output will be set if coupled with this hwc.
uint16_t evaluateAction_VMIX(const uint8_t devIndex, const uint16_t actionPtr, const uint8_t HWc, const uint8_t actIdx, bool actDown, bool actUp, int16_t pulses, int16_t value) {
  uint16_t retVal = 0;
  int16_t tempInt = 0;
  uint8_t tempByte = 0;

  if (actDown || actUp) {
    if (debugMode)
      Serial << F("VMIX action ") << globalConfigMem[actionPtr] << F("\n");
  }

  switch (globalConfigMem[actionPtr]) {
  case 0: // Active Source
    if (actDown) {
      if (globalConfigMem[actionPtr + 2] == 3) {       // Source cycle by button push
        _systemHWcActionCacheFlag[HWc][actIdx] = true; // Used to show button is highlighted here
        pulses = 2;
      } else if (globalConfigMem[actionPtr + 2] != 2 || !_systemHWcActionCacheFlag[HWc][actIdx]) {
        _systemHWcActionCacheFlag[HWc][actIdx] = true; // Used for toggle feature
        _systemHWcActionCache[HWc][actIdx] = VMIX[devIndex].getActiveInput();
        VMIX[devIndex].setActiveInput(globalConfigMem[actionPtr + 1]);
      } else {
        VMIX[devIndex].setActiveInput(_systemHWcActionCache[HWc][actIdx]);
        _systemHWcActionCacheFlag[HWc][actIdx] = false;
      }
    }
    if (actUp && globalConfigMem[actionPtr + 2] == 1) { // "Hold Down"
      VMIX[devIndex].setActiveInput(_systemHWcActionCache[HWc][actIdx]);
    }
    if (actUp && globalConfigMem[actionPtr + 2] == 3) { // "Cycle"
      _systemHWcActionCacheFlag[HWc][actIdx] = false;
    }
    if (pulses & 0xFFFE) {
      VMIX[devIndex].setActiveInput(pulsesHelper(VMIX[devIndex].getActiveInput(), globalConfigMem[actionPtr + 3], globalConfigMem[actionPtr + 1], true, pulses, 1, 1));
    }

    retVal = globalConfigMem[actionPtr + 2] == 3 ? (_systemHWcActionCacheFlag[HWc][actIdx] ? (2 | 0x20) : 5) : (VMIX[devIndex].getActiveInput() == globalConfigMem[actionPtr + 1] ? (2 | 0x20) : 5);

    // The availabilty should not affect the color in cycle mode
    /*    if (globalConfigMem[actionPtr + 2] != 3 && globalConfigMem[actionPtr + 1] < VMIX[devIndex].getTopologyInputs()) {
          retVal = 0;
        }
    */
    if (extRetValIsWanted()) {
      extRetVal(0, 7); // , pulses&B1 - not using this because it has no significance for this type of action.
      extRetValShortLabel(PSTR("Active"));
      extRetValLongLabel(PSTR("Active Input"));

      if (_systemHWcActionPrefersLabel[HWc] && globalConfigMem[actionPtr + 2] != 3) {
        extRetValColor(!retVal ? 0 : (retVal & 0x20 ? B110000 : B101010));
        extRetValSetLabel(true);
        extRetValTxt(VMIX[devIndex].getInputPropertiesLongName(globalConfigMem[actionPtr + 1]), 0);
        extRetValTxtShort(VMIX[devIndex].getInputPropertiesShortName(globalConfigMem[actionPtr + 1]));
      } else {
        extRetValColor(!retVal ? 0 : B110101);
        extRetValTxt(VMIX[devIndex].getInputPropertiesLongName(VMIX[devIndex].getActiveInput()), 0);
        extRetValTxtShort(VMIX[devIndex].getInputPropertiesShortName(VMIX[devIndex].getActiveInput()));
      }
    }
    return retVal;
    break;
  case 1: // Preview Source
    if (actDown) {
      if (globalConfigMem[actionPtr + 2] == 3) {       // Source cycle by button push
        _systemHWcActionCacheFlag[HWc][actIdx] = true; // Used to show button is highlighted here
        pulses = 2;
      } else {
        VMIX[devIndex].setPreviewInput(globalConfigMem[actionPtr + 1]);
      }
    }
    if (actUp && globalConfigMem[actionPtr + 2] == 3) { // "Cycle"
      _systemHWcActionCacheFlag[HWc][actIdx] = false;
    }
    if (pulses & 0xFFFE) {
      VMIX[devIndex].setPreviewInput(pulsesHelper(VMIX[devIndex].getPreviewInput(), globalConfigMem[actionPtr + 3], globalConfigMem[actionPtr + 1], true, pulses, 1, 1));
    }

    retVal = globalConfigMem[actionPtr + 2] == 3 ? (_systemHWcActionCacheFlag[HWc][actIdx] ? (2 | 0x20) : 5) : (VMIX[devIndex].getPreviewInput() == globalConfigMem[actionPtr + 1] ? (2 | 0x20) : 5);

    // The availabilty should not affect the color in cycle mode
    /*    if (globalConfigMem[actionPtr + 2] != 3 && globalConfigMem[actionPtr + 1] < VMIX[devIndex].getTopologyInputs()) {
          retVal = 0;
        }
    */
    if (extRetValIsWanted()) {
      extRetVal(0, 7); // , pulses&B1 - not using this because it has no significance for this type of action.
      extRetValShortLabel(PSTR("Preview"));
      extRetValLongLabel(PSTR("Preview Input"));

      if (_systemHWcActionPrefersLabel[HWc] && globalConfigMem[actionPtr + 2] != 3) {
        extRetValColor(!retVal ? 0 : (retVal & 0x20 ? B110000 : B101010));
        extRetValSetLabel(true);
        extRetValTxt(VMIX[devIndex].getInputPropertiesLongName(globalConfigMem[actionPtr + 1]), 0);
        extRetValTxtShort(VMIX[devIndex].getInputPropertiesShortName(globalConfigMem[actionPtr + 1]));
      } else {
        extRetValColor(!retVal ? 0 : B110101);
        extRetValTxt(VMIX[devIndex].getInputPropertiesLongName(VMIX[devIndex].getPreviewInput()), 0);
        extRetValTxtShort(VMIX[devIndex].getInputPropertiesShortName(VMIX[devIndex].getPreviewInput()));
      }
    }
    return retVal;
    break;
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