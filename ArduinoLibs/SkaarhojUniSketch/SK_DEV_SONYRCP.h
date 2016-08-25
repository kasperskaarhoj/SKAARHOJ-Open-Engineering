
// Button return colors:
// 0 = off
// 5 = dimmed
// 1,2,3,4 = full (yellow), red, green, yellow
// Bit 4 (16) = blink flag, filter out for KP01 buttons.
uint16_t evaluateAction_SONYRCP(const uint8_t devIndex, const uint16_t actionPtr, const uint8_t HWc, const uint8_t actIdx, bool actDown, bool actUp, int pulses, int value) {
  uint16_t retVal = 0;
  int tempInt = 0;
  uint8_t tempByte = 0;

  uint8_t cam = 0;
  uint8_t option = 0;
  uint16_t aSrc = 0;
  char yrgbLabels[5] = "YRGB";

  if (actDown || actUp) {
    if (debugMode)
      Serial << F("SonyRCP action ") << globalConfigMem[actionPtr] << F("\n");
  }

  switch (globalConfigMem[actionPtr]) {
  case 0:

    break;
  }
  /*case 0: // Program Source
    if (actDown) {
      if (globalConfigMem[actionPtr + 3] == 3) {       // Source cycle by button push
        _systemHWcActionCacheFlag[HWc][actIdx] = true; // Used to show button is highlighted here
        pulses = 2;
      } else if (globalConfigMem[actionPtr + 3] != 2 || !_systemHWcActionCacheFlag[HWc][actIdx]) {
        _systemHWcActionCacheFlag[HWc][actIdx] = true; // Used for toggle feature
        _systemHWcActionCache[HWc][actIdx] = AtemSwitcher[devIndex].getProgramInputVideoSource(globalConfigMem[actionPtr + 1]);
        AtemSwitcher[devIndex].setProgramInputVideoSource(globalConfigMem[actionPtr + 1], ATEM_idxToVideoSrc(devIndex, globalConfigMem[actionPtr + 2]));
      } else {
        AtemSwitcher[devIndex].setProgramInputVideoSource(globalConfigMem[actionPtr + 1], _systemHWcActionCache[HWc][actIdx]);
        _systemHWcActionCacheFlag[HWc][actIdx] = false;
      }
    }
    if (actUp && globalConfigMem[actionPtr + 3] == 1) { // "Hold Down"
      AtemSwitcher[devIndex].setProgramInputVideoSource(globalConfigMem[actionPtr + 1], _systemHWcActionCache[HWc][actIdx]);
    }
    if (actUp && globalConfigMem[actionPtr + 3] == 3) { // "Cycle"
      _systemHWcActionCacheFlag[HWc][actIdx] = false;
    }
    if (pulses & 0xFFFE) {
      AtemSwitcher[devIndex].setProgramInputVideoSource(globalConfigMem[actionPtr + 1], ATEM_searchVideoSrc(devIndex, AtemSwitcher[devIndex].getProgramInputVideoSource(globalConfigMem[actionPtr + 1]), (pulses >> 1), 0, B1 << globalConfigMem[actionPtr + 1], globalConfigMem[actionPtr + 3] == 3 ? globalConfigMem[actionPtr + 2] : AtemSwitcher[devIndex].maxAtemSeriesVideoInputs()));
    }

    retVal = globalConfigMem[actionPtr + 3] == 3 ? (_systemHWcActionCacheFlag[HWc][actIdx] ? (2 | 0x20) : 5) : (AtemSwitcher[devIndex].getProgramInputVideoSource(globalConfigMem[actionPtr + 1]) == ATEM_idxToVideoSrc(devIndex, globalConfigMem[actionPtr + 2]) ? (2 | 0x20) : 5);
    if (!(AtemSwitcher[devIndex].getInputMEAvailability(ATEM_idxToVideoSrc(devIndex, globalConfigMem[actionPtr + 2])) & (B1 << globalConfigMem[actionPtr + 1]))) {
      retVal = 0;
    }

    if (extRetValIsWanted()) {
      extRetVal(0, 7); // , pulses&B1 - not using this because it has no significance for this type of action.
      extRetValShortLabel(PSTR("ME"), (globalConfigMem[actionPtr + 1]) + 1);
      extRetValShortLabel(PSTR(" PGM"));
      extRetValLongLabel(PSTR("ME"), (globalConfigMem[actionPtr + 1]) + 1);
      extRetValLongLabel(PSTR(" Program Src"));

      if (_systemHWcActionPrefersLabel[HWc] && globalConfigMem[actionPtr + 3] != 3) {
        extRetValColor(!retVal ? 0 : (retVal & 0x20 ? B110000 : B101010));
        extRetValSetLabel(true);
        extRetValTxt(AtemSwitcher[devIndex].getInputLongName(ATEM_idxToVideoSrc(devIndex, globalConfigMem[actionPtr + 2])), 0);
        extRetValTxtShort(AtemSwitcher[devIndex].getInputShortName(ATEM_idxToVideoSrc(devIndex, globalConfigMem[actionPtr + 2])));
      } else {
        extRetValColor(!retVal ? 0 : B110101);
        extRetValTxt(AtemSwitcher[devIndex].getInputLongName(AtemSwitcher[devIndex].getProgramInputVideoSource(globalConfigMem[actionPtr + 1])), 0);
        extRetValTxtShort(AtemSwitcher[devIndex].getInputShortName(AtemSwitcher[devIndex].getProgramInputVideoSource(globalConfigMem[actionPtr + 1])));
      }
    }
    return retVal;
    break;
  case 1: // Preview Source
    if (actDown) {
      if (globalConfigMem[actionPtr + 3] == 1) { // Source cycle by button push
        pulses = 2;
        _systemHWcActionCacheFlag[HWc][actIdx] = true;
      } else
        AtemSwitcher[devIndex].setPreviewInputVideoSource(globalConfigMem[actionPtr + 1], ATEM_idxToVideoSrc(devIndex, globalConfigMem[actionPtr + 2]));
    }
    if (actUp && globalConfigMem[actionPtr + 3] == 1) {
      _systemHWcActionCacheFlag[HWc][actIdx] = false;
    }
    if (pulses & 0xFFFE) {
      AtemSwitcher[devIndex].setPreviewInputVideoSource(globalConfigMem[actionPtr + 1], ATEM_searchVideoSrc(devIndex, AtemSwitcher[devIndex].getPreviewInputVideoSource(globalConfigMem[actionPtr + 1]), (pulses >> 1), 0, B1 << globalConfigMem[actionPtr + 1], globalConfigMem[actionPtr + 3] == 1 ? globalConfigMem[actionPtr + 2] : AtemSwitcher[devIndex].maxAtemSeriesVideoInputs()));
    }

    retVal = globalConfigMem[actionPtr + 3] == 1 ? (_systemHWcActionCacheFlag[HWc][actIdx] ? (3 | 0x20) : 5) : (AtemSwitcher[devIndex].getPreviewInputVideoSource(globalConfigMem[actionPtr + 1]) == ATEM_idxToVideoSrc(devIndex, globalConfigMem[actionPtr + 2]) ? (3 | 0x20) : 5);
    if (!(AtemSwitcher[devIndex].getInputMEAvailability(ATEM_idxToVideoSrc(devIndex, globalConfigMem[actionPtr + 2])) & (B1 << globalConfigMem[actionPtr + 1]))) {
      retVal = 0;
    }

    if (extRetValIsWanted()) {
      extRetVal(0, 7);
      extRetValShortLabel(PSTR("ME"), (globalConfigMem[actionPtr + 1]) + 1);
      extRetValShortLabel(PSTR(" PRV"));
      extRetValLongLabel(PSTR("ME"), (globalConfigMem[actionPtr + 1]) + 1);
      extRetValLongLabel(PSTR(" Preview Src"));

      if (_systemHWcActionPrefersLabel[HWc] && globalConfigMem[actionPtr + 3] != 1) {
        extRetValColor(!retVal ? 0 : (retVal & 0x20 ? B001100 : B101010));
        extRetValSetLabel(true);
        extRetValTxt(AtemSwitcher[devIndex].getInputLongName(ATEM_idxToVideoSrc(devIndex, globalConfigMem[actionPtr + 2])), 0);
        extRetValTxtShort(AtemSwitcher[devIndex].getInputShortName(ATEM_idxToVideoSrc(devIndex, globalConfigMem[actionPtr + 2])));
      } else {
        extRetValColor(!retVal ? 0 : B011101);
        extRetValTxt(AtemSwitcher[devIndex].getInputLongName(AtemSwitcher[devIndex].getPreviewInputVideoSource(globalConfigMem[actionPtr + 1])), 0);
        extRetValTxtShort(AtemSwitcher[devIndex].getInputShortName(AtemSwitcher[devIndex].getPreviewInputVideoSource(globalConfigMem[actionPtr + 1])));
      }
    }
    return retVal;
    break;
  case 2: // Program/Preview Source
    if (actDown) {
      _systemHWcActionCacheFlag[HWc][actIdx] = true;
      _systemHWcActionCache[HWc][actIdx] = millis();
      if (globalConfigMem[actionPtr + 3] != 1) { // Source cycle by button push
        AtemSwitcher[devIndex].setPreviewInputVideoSource(globalConfigMem[actionPtr + 1], ATEM_idxToVideoSrc(devIndex, globalConfigMem[actionPtr + 2]));
      }
    }
    if (_systemHWcActionCacheFlag[HWc][actIdx]) {
      uint16_t t = millis();
      if (t - _systemHWcActionCache[HWc][actIdx] > 1000) {
        AtemSwitcher[devIndex].performCutME(globalConfigMem[actionPtr + 1]);
        //   AtemSwitcher[devIndex].setProgramInputVideoSource(globalConfigMem[actionPtr + 1], ATEM_idxToVideoSrc(devIndex, globalConfigMem[actionPtr + 2]));
        _systemHWcActionCacheFlag[HWc][actIdx] = false;
      }
    }
    if (actUp && _systemHWcActionCacheFlag[HWc][actIdx]) {
      if (globalConfigMem[actionPtr + 3] == 1)
        pulses = 2;

      _systemHWcActionCacheFlag[HWc][actIdx] = false;
    }
    if (pulses & 0xFFFE) {
      AtemSwitcher[devIndex].setPreviewInputVideoSource(globalConfigMem[actionPtr + 1], ATEM_searchVideoSrc(devIndex, AtemSwitcher[devIndex].getPreviewInputVideoSource(globalConfigMem[actionPtr + 1]), (pulses >> 1), 0, B1 << globalConfigMem[actionPtr + 1], globalConfigMem[actionPtr + 3] == 1 ? globalConfigMem[actionPtr + 2] : AtemSwitcher[devIndex].maxAtemSeriesVideoInputs()));
    }

    retVal = globalConfigMem[actionPtr + 3] == 1 ? (_systemHWcActionCacheFlag[HWc][actIdx] ? (4 | 0x20) : 5) : (AtemSwitcher[devIndex].getProgramInputVideoSource(globalConfigMem[actionPtr + 1]) == ATEM_idxToVideoSrc(devIndex, globalConfigMem[actionPtr + 2]) ? 2 : (AtemSwitcher[devIndex].getPreviewInputVideoSource(globalConfigMem[actionPtr + 1]) == ATEM_idxToVideoSrc(devIndex, globalConfigMem[actionPtr + 2]) ? 3 : 5));
    if (retVal != 5)
      retVal |= 0x20; // Add binary output bit
    if ((retVal & 0xF) == 3)
      retVal |= 0x10; // Bit 4 is a "mono-color blink flag" so a mono color button can indicate this special state.
    if (!(AtemSwitcher[devIndex].getInputMEAvailability(ATEM_idxToVideoSrc(devIndex, globalConfigMem[actionPtr + 2])) & (B1 << globalConfigMem[actionPtr + 1]))) {
      retVal = 0;
    }

    if (extRetValIsWanted()) {
      extRetVal(0, 7);
      extRetValShortLabel(PSTR("ME"), (globalConfigMem[actionPtr + 1]) + 1);
      extRetValShortLabel(PSTR(" PG/PV"));
      extRetValLongLabel(PSTR("ME"), (globalConfigMem[actionPtr + 1]) + 1);
      extRetValLongLabel(PSTR(" Prg/Prv Src"));

      if (_systemHWcActionPrefersLabel[HWc] && globalConfigMem[actionPtr + 3] != 1) {
        extRetValColor(!retVal ? 0 : ((retVal & 0xF) == 3 ? B001100 : ((retVal & 0xF) == 2 ? B110000 : B101010)));
        extRetValSetLabel(true);
        extRetValTxt(AtemSwitcher[devIndex].getInputLongName(ATEM_idxToVideoSrc(devIndex, globalConfigMem[actionPtr + 2])), 0);
        extRetValTxtShort(AtemSwitcher[devIndex].getInputShortName(ATEM_idxToVideoSrc(devIndex, globalConfigMem[actionPtr + 2])));
      } else {
        extRetValColor(!retVal ? 0 : B101010);
        extRetValTxt(AtemSwitcher[devIndex].getInputLongName(AtemSwitcher[devIndex].getProgramInputVideoSource(globalConfigMem[actionPtr + 1])), 0);
        extRetVal2(0);
        extRetValTxt(AtemSwitcher[devIndex].getInputLongName(AtemSwitcher[devIndex].getPreviewInputVideoSource(globalConfigMem[actionPtr + 1])), 1);
      }
    }
    return retVal;
    break;
  case 3: // AUX
    if (actDown) {
      if (globalConfigMem[actionPtr + 3] == 5) { // Source cycle by button push
        pulses = 2;
        _systemHWcActionCacheFlag[HWc][actIdx] = true;
      } else if (globalConfigMem[actionPtr + 3] != 2 || !_systemHWcActionCacheFlag[HWc][actIdx]) {
        _systemHWcActionCacheFlag[HWc][actIdx] = true; // Used for toggle feature
        _systemHWcActionCache[HWc][actIdx] = AtemSwitcher[devIndex].getAuxSourceInput(globalConfigMem[actionPtr + 1]);
        if (globalConfigMem[actionPtr + 3] == 3 || globalConfigMem[actionPtr + 3] == 4) {
          ATEM_pushToHoldGroup(globalConfigMem[actionPtr + 3] - 3, AtemSwitcher[devIndex].getAuxSourceInput(globalConfigMem[actionPtr + 1]), HWc);
        }
        AtemSwitcher[devIndex].setAuxSourceInput(globalConfigMem[actionPtr + 1], ATEM_idxToVideoSrc(devIndex, globalConfigMem[actionPtr + 2]));
      } else {
        AtemSwitcher[devIndex].setAuxSourceInput(globalConfigMem[actionPtr + 1], _systemHWcActionCache[HWc][actIdx]);
        _systemHWcActionCacheFlag[HWc][actIdx] = false;
      }
    }
    if (actUp && globalConfigMem[actionPtr + 3] == 1) { // "Hold Down"
      AtemSwitcher[devIndex].setAuxSourceInput(globalConfigMem[actionPtr + 1], _systemHWcActionCache[HWc][actIdx]);
    }
    if (actUp && (globalConfigMem[actionPtr + 3] == 3 || globalConfigMem[actionPtr + 3] == 4)) { // "Hold Groups"
      uint16_t fallBackSrc = ATEM_pullFromHoldGroup(globalConfigMem[actionPtr + 3] - 3, HWc);
      if (fallBackSrc != 0x8000)
        AtemSwitcher[devIndex].setAuxSourceInput(globalConfigMem[actionPtr + 1], fallBackSrc);
    }
    if (actUp && globalConfigMem[actionPtr + 3] == 5) { // "Cycle"
      _systemHWcActionCacheFlag[HWc][actIdx] = false;
    }

    if (pulses & 0xFFFE) {
      AtemSwitcher[devIndex].setAuxSourceInput(globalConfigMem[actionPtr + 1], ATEM_searchVideoSrc(devIndex, AtemSwitcher[devIndex].getAuxSourceInput(globalConfigMem[actionPtr + 1]), (pulses >> 1), B1, 0, globalConfigMem[actionPtr + 3] == 5 ? globalConfigMem[actionPtr + 2] : AtemSwitcher[devIndex].maxAtemSeriesVideoInputs()));
    }

    retVal = globalConfigMem[actionPtr + 3] == 5 ? (_systemHWcActionCacheFlag[HWc][actIdx] ? (4 | 0x20) : 5) : (AtemSwitcher[devIndex].getAuxSourceInput(globalConfigMem[actionPtr + 1]) == ATEM_idxToVideoSrc(devIndex, globalConfigMem[actionPtr + 2]) ? (4 | 0x20) : 5);
    if (!(AtemSwitcher[devIndex].getInputAvailability(ATEM_idxToVideoSrc(devIndex, globalConfigMem[actionPtr + 2])) & B1)) {
      retVal = 0;
    }

    if (extRetValIsWanted()) {
      extRetVal(0, 7);
      extRetValShortLabel(PSTR("AUX "), (globalConfigMem[actionPtr + 1]) + 1);
      extRetValLongLabel(PSTR("AUX "), (globalConfigMem[actionPtr + 1]) + 1);
      extRetValLongLabel(PSTR(" Source"));

      if (_systemHWcActionPrefersLabel[HWc] && globalConfigMem[actionPtr + 3] != 5) {
        extRetValColor(!retVal ? 0 : (retVal & 0x20 ? B010111 : B101010));
        extRetValSetLabel(true);
        extRetValTxt(AtemSwitcher[devIndex].getInputLongName(ATEM_idxToVideoSrc(devIndex, globalConfigMem[actionPtr + 2])), 0);
        extRetValTxtShort(AtemSwitcher[devIndex].getInputShortName(ATEM_idxToVideoSrc(devIndex, globalConfigMem[actionPtr + 2])));
      } else {
        extRetValColor(!retVal ? 0 : B101010);
        extRetValTxt(AtemSwitcher[devIndex].getInputLongName(AtemSwitcher[devIndex].getAuxSourceInput(globalConfigMem[actionPtr + 1])), 0);
        extRetValTxtShort(AtemSwitcher[devIndex].getInputShortName(AtemSwitcher[devIndex].getAuxSourceInput(globalConfigMem[actionPtr + 1])));
      }
    }
    return retVal;
    break;
  case 4: // USK settings
    if (globalConfigMem[actionPtr + 3] != 4) {
      if (actDown) {
        bool state = false;
        switch (globalConfigMem[actionPtr + 3]) {
        case 0: // Toggle
          state = !AtemSwitcher[devIndex].getKeyerOnAirEnabled(globalConfigMem[actionPtr + 1], globalConfigMem[actionPtr + 2]);
          break;
        case 1: // On
        case 3: // Hold down
          state = true;
          break;
        }
        AtemSwitcher[devIndex].setKeyerOnAirEnabled(globalConfigMem[actionPtr + 1], globalConfigMem[actionPtr + 2], state);
      }
      if (actUp && globalConfigMem[actionPtr + 3] == 3) {
        AtemSwitcher[devIndex].setKeyerOnAirEnabled(globalConfigMem[actionPtr + 1], globalConfigMem[actionPtr + 2], false);
      }
    } else {
      // TODO: perform auto on USK
    }
    if (pulses & 0xFFFE) {
      AtemSwitcher[devIndex].setKeyerOnAirEnabled(globalConfigMem[actionPtr + 1], globalConfigMem[actionPtr + 2], !AtemSwitcher[devIndex].getKeyerOnAirEnabled(globalConfigMem[actionPtr + 1], globalConfigMem[actionPtr + 2]));
    }
    retVal = (AtemSwitcher[devIndex].getKeyerOnAirEnabled(globalConfigMem[actionPtr + 1], globalConfigMem[actionPtr + 2]) ^ (globalConfigMem[actionPtr + 3] == 2)) ? (4 | 0x20) : 5;

    if (extRetValIsWanted()) {
      extRetVal(0, 7);
      extRetValShortLabel(PSTR("ME"), (globalConfigMem[actionPtr + 1]) + 1);
      extRetValShortLabel(PSTR(" Keyer"), (globalConfigMem[actionPtr + 2]) + 1);
      extRetValLongLabel(PSTR("ME"), (globalConfigMem[actionPtr + 1]) + 1);
      extRetValLongLabel(PSTR(" UpstrKey"), (globalConfigMem[actionPtr + 2]) + 1);
      if (_systemHWcActionPrefersLabel[HWc]) {
        extRetValColor(retVal & 0x20 ? B110111 : B101010);
        extRetValSetLabel(true);
        switch (globalConfigMem[actionPtr + 3]) {
        case 0: // Toggle
          extRetValTxt_P(PSTR("OnOff"), 0);
          break;
        case 1: // On
          extRetValTxt_P(PSTR("On"), 0);
          break;
        case 2: // Off
          extRetValTxt_P(PSTR("Off"), 0);
          break;
        case 3: // Hold down
          extRetValTxt_P(PSTR("Hold"), 0);
          extRetValTxt_P(PSTR("Down"), 1);
          extRetVal2(0);
          break;
        case 4: // Auto
          extRetValTxt_P(PSTR("Auto"), 0);
          break;
        }
      } else {
        extRetValColor(B101010);
        extRetValTxt_P(retVal & 0x20 ? PSTR("On") : PSTR("Off"), 0);
      }
    }
    return retVal;
    break;
  case 5: // Upstream Keyer Fill
    if (actDown) {
      AtemSwitcher[devIndex].setKeyerFillSource(globalConfigMem[actionPtr + 1], globalConfigMem[actionPtr + 2], ATEM_idxToVideoSrc(devIndex, globalConfigMem[actionPtr + 3]));
    }
    if (pulses & 0xFFFE) {
      AtemSwitcher[devIndex].setKeyerFillSource(globalConfigMem[actionPtr + 1], globalConfigMem[actionPtr + 2], ATEM_searchVideoSrc(devIndex, AtemSwitcher[devIndex].getKeyerFillSource(globalConfigMem[actionPtr + 1], globalConfigMem[actionPtr + 2]), (pulses >> 1), 0, B1, globalConfigMem[actionPtr + 3]));
    }

    retVal = AtemSwitcher[devIndex].getKeyerFillSource(globalConfigMem[actionPtr + 1], globalConfigMem[actionPtr + 2]) == ATEM_idxToVideoSrc(devIndex, globalConfigMem[actionPtr + 3]) ? (4 | 0x20) : 5;

    if (extRetValIsWanted()) {
      extRetVal(0, 7);
      extRetValShortLabel(PSTR("ME"), (globalConfigMem[actionPtr + 1]) + 1);
      extRetValShortLabel(PSTR(" Keyer"), (globalConfigMem[actionPtr + 2]) + 1);
      extRetValLongLabel(PSTR("ME"), (globalConfigMem[actionPtr + 1]) + 1);
      extRetValLongLabel(PSTR(" UpstrKey"), (globalConfigMem[actionPtr + 2]) + 1);
      extRetValTxt_P(PSTR("Fill Src:"), 0);
      extRetVal2(0);

      if (_systemHWcActionPrefersLabel[HWc]) {
        extRetValColor(retVal & 0x20 ? B110111 : B101010);
        extRetValSetLabel(true);
        extRetValTxt(AtemSwitcher[devIndex].getInputLongName(ATEM_idxToVideoSrc(devIndex, globalConfigMem[actionPtr + 3])), 1);
      } else {
        extRetValColor(B101010);
        extRetValTxt(AtemSwitcher[devIndex].getInputLongName(AtemSwitcher[devIndex].getKeyerFillSource(globalConfigMem[actionPtr + 1], globalConfigMem[actionPtr + 2])), 1);
      }
    }
    return retVal;
    break;
  case 6: // Upstream Keyer Key
    if (actDown) {
      AtemSwitcher[devIndex].setKeyerKeySource(globalConfigMem[actionPtr + 1], globalConfigMem[actionPtr + 2], ATEM_idxToVideoSrc(devIndex, globalConfigMem[actionPtr + 3]));
    }
    if (pulses & 0xFFFE) {
      AtemSwitcher[devIndex].setKeyerKeySource(globalConfigMem[actionPtr + 1], globalConfigMem[actionPtr + 2], ATEM_searchVideoSrc(devIndex, AtemSwitcher[devIndex].getKeyerKeySource(globalConfigMem[actionPtr + 1], globalConfigMem[actionPtr + 2]), (pulses >> 1), B10000, 0, globalConfigMem[actionPtr + 3]));
    }

    retVal = AtemSwitcher[devIndex].getKeyerKeySource(globalConfigMem[actionPtr + 1], globalConfigMem[actionPtr + 2]) == ATEM_idxToVideoSrc(devIndex, globalConfigMem[actionPtr + 3]) ? (4 | 0x20) : 5;

    if (extRetValIsWanted()) {
      extRetVal(0, 7);
      extRetValShortLabel(PSTR("ME"), (globalConfigMem[actionPtr + 1]) + 1);
      extRetValShortLabel(PSTR(" Keyer"), (globalConfigMem[actionPtr + 2]) + 1);
      extRetValLongLabel(PSTR("ME"), (globalConfigMem[actionPtr + 1]) + 1);
      extRetValLongLabel(PSTR(" UpstrKey"), (globalConfigMem[actionPtr + 2]) + 1);
      extRetValTxt_P(PSTR("Key Src:"), 0);
      extRetVal2(0);

      if (_systemHWcActionPrefersLabel[HWc]) {
        extRetValColor(retVal & 0x20 ? B110111 : B101010);
        extRetValSetLabel(true);
        extRetValTxt(AtemSwitcher[devIndex].getInputLongName(ATEM_idxToVideoSrc(devIndex, globalConfigMem[actionPtr + 3])), 1);
      } else {
        extRetValColor(B101010);
        extRetValTxt(AtemSwitcher[devIndex].getInputLongName(AtemSwitcher[devIndex].getKeyerKeySource(globalConfigMem[actionPtr + 1], globalConfigMem[actionPtr + 2])), 1);
      }
    }
    return retVal;
    break;
  case 7: // DSK settings
    if (globalConfigMem[actionPtr + 2] != 4) {
      if (actDown) {
        bool state = false;
        switch (globalConfigMem[actionPtr + 2]) {
        case 0: // Toggle
          state = !AtemSwitcher[devIndex].getDownstreamKeyerOnAir(globalConfigMem[actionPtr + 1]);
          break;
        case 1: // On
        case 3: // Hold down
          state = true;
          break;
        }
        AtemSwitcher[devIndex].setDownstreamKeyerOnAir(globalConfigMem[actionPtr + 1], state);
      }
      if (actUp && globalConfigMem[actionPtr + 2] == 3) {
        AtemSwitcher[devIndex].setDownstreamKeyerOnAir(globalConfigMem[actionPtr + 1], false);
      }
      if (pulses & 0xFFFE) {
        AtemSwitcher[devIndex].setDownstreamKeyerOnAir(globalConfigMem[actionPtr + 1], !AtemSwitcher[devIndex].getDownstreamKeyerOnAir(globalConfigMem[actionPtr + 1]));
      }
    } else {
      if (actDown) {
        AtemSwitcher[devIndex].performDownstreamKeyerAutoKeyer(globalConfigMem[actionPtr + 1]);
      }
    }

    retVal = AtemSwitcher[devIndex].getDownstreamKeyerOnAir(globalConfigMem[actionPtr + 1]) ^ (globalConfigMem[actionPtr + 2] == 2) ? (4 | 0x20) : 5;

    if (extRetValIsWanted()) {
      extRetVal(0, 7);
      extRetValShortLabel(PSTR("DSK"), (globalConfigMem[actionPtr + 1]) + 1);
      extRetValLongLabel(PSTR("DownstrKeyer "), (globalConfigMem[actionPtr + 1]) + 1);

      if (_systemHWcActionPrefersLabel[HWc]) {
        extRetValColor(retVal & 0x20 ? B011111 : B101010);
        extRetValSetLabel(true);
        switch (globalConfigMem[actionPtr + 2]) {
        case 0: // Toggle
          extRetValTxt_P(PSTR("OnOff"), 0);
          break;
        case 1: // On
          extRetValTxt_P(PSTR("On"), 0);
          break;
        case 2: // Off
          extRetValTxt_P(PSTR("Off"), 0);
          break;
        case 3: // Hold down
          extRetValTxt_P(PSTR("Hold"), 0);
          extRetValTxt_P(PSTR("Down"), 1);
          extRetVal2(0);
          break;
        case 4: // Auto
          extRetValTxt_P(PSTR("Auto"), 0);
          break;
        }
      } else {
        extRetValColor(B101010);
        extRetValTxt_P(retVal & 0x20 ? PSTR("On") : PSTR("Off"), 0);
      }
    }

    return retVal;
    break;
  case 8: // Downstream Keyer Fill
    if (actDown) {
      AtemSwitcher[devIndex].setDownstreamKeyerFillSource(globalConfigMem[actionPtr + 1], ATEM_idxToVideoSrc(devIndex, globalConfigMem[actionPtr + 2]));
    }
    if (pulses & 0xFFFE) {
      AtemSwitcher[devIndex].setDownstreamKeyerFillSource(globalConfigMem[actionPtr + 1], ATEM_searchVideoSrc(devIndex, AtemSwitcher[devIndex].getDownstreamKeyerKeySource(globalConfigMem[actionPtr + 1]), (pulses >> 1), 0, B1, globalConfigMem[actionPtr + 2]));
    }

    retVal = AtemSwitcher[devIndex].getDownstreamKeyerFillSource(globalConfigMem[actionPtr + 1]) == ATEM_idxToVideoSrc(devIndex, globalConfigMem[actionPtr + 2]) ? (4 | 0x20) : 5;

    if (extRetValIsWanted()) {
      extRetVal(0, 7);
      extRetValShortLabel(PSTR("DSK"), (globalConfigMem[actionPtr + 1]) + 1);
      extRetValLongLabel(PSTR("DownstrKeyer "), (globalConfigMem[actionPtr + 1]) + 1);
      extRetValTxt_P(PSTR("Fill Src:"), 0);
      extRetVal2(0);

      if (_systemHWcActionPrefersLabel[HWc]) {
        extRetValSetLabel(true);
        extRetValColor(retVal & 0x20 ? B011111 : B101010);
        extRetValTxt(AtemSwitcher[devIndex].getInputLongName(ATEM_idxToVideoSrc(devIndex, globalConfigMem[actionPtr + 2])), 1);
      } else {
        extRetValColor(B101010);
        extRetValTxt(AtemSwitcher[devIndex].getInputLongName(AtemSwitcher[devIndex].getDownstreamKeyerFillSource(globalConfigMem[actionPtr + 1])), 1);
      }
    }
    return retVal;
    break;
  case 9: // Downstream Keyer Key
    if (actDown) {
      AtemSwitcher[devIndex].setDownstreamKeyerKeySource(globalConfigMem[actionPtr + 1], ATEM_idxToVideoSrc(devIndex, globalConfigMem[actionPtr + 2]));
    }
    if (pulses & 0xFFFE) {
      AtemSwitcher[devIndex].setDownstreamKeyerKeySource(globalConfigMem[actionPtr + 1], ATEM_searchVideoSrc(devIndex, AtemSwitcher[devIndex].getDownstreamKeyerKeySource(globalConfigMem[actionPtr + 1]), (pulses >> 1), B10000, 0, globalConfigMem[actionPtr + 2]));
    }

    retVal = AtemSwitcher[devIndex].getDownstreamKeyerKeySource(globalConfigMem[actionPtr + 1]) == ATEM_idxToVideoSrc(devIndex, globalConfigMem[actionPtr + 2]) ? (4 | 0x20) : 5;

    if (extRetValIsWanted()) {
      extRetVal(0, 7);
      extRetValShortLabel(PSTR("DSK"), (globalConfigMem[actionPtr + 1]) + 1);
      extRetValLongLabel(PSTR("DownstrKeyer "), (globalConfigMem[actionPtr + 1]) + 1);
      extRetValTxt_P(PSTR("Key Src:"), 0);
      extRetVal2(0);

      if (_systemHWcActionPrefersLabel[HWc]) {
        extRetValSetLabel(true);
        extRetValColor(retVal & 0x20 ? B011111 : B101010);
        extRetValTxt(AtemSwitcher[devIndex].getInputLongName(ATEM_idxToVideoSrc(devIndex, globalConfigMem[actionPtr + 2])), 1);
      } else {
        extRetValColor(B101010);
        extRetValTxt(AtemSwitcher[devIndex].getInputLongName(AtemSwitcher[devIndex].getDownstreamKeyerKeySource(globalConfigMem[actionPtr + 1])), 1);
      }
    }
    return retVal;
    break;
  case 10: // MP Still
    if (actDown) {
      if (globalConfigMem[actionPtr + 3] == 3) {
        pulses = 2;
        _systemHWcActionCacheFlag[HWc][actIdx] = true;
      } else if (globalConfigMem[actionPtr + 3] != 2 || !_systemHWcActionCacheFlag[HWc][actIdx]) {
        if (AtemSwitcher[devIndex].getMediaPlayerStillFilesIsUsed(globalConfigMem[actionPtr + 2] - 1)) {
          _systemHWcActionCacheFlag[HWc][actIdx] = true; // Used for toggle feature
          _systemHWcActionCache[HWc][actIdx] = AtemSwitcher[devIndex].getMediaPlayerSourceStillIndex(globalConfigMem[actionPtr + 1]);
          AtemSwitcher[devIndex].setMediaPlayerSourceType(globalConfigMem[actionPtr + 1], 1);
          AtemSwitcher[devIndex].setMediaPlayerSourceStillIndex(globalConfigMem[actionPtr + 1], globalConfigMem[actionPtr + 2] - 1);
        }
      } else {
        AtemSwitcher[devIndex].setMediaPlayerSourceStillIndex(globalConfigMem[actionPtr + 1], _systemHWcActionCache[HWc][actIdx]);
        _systemHWcActionCacheFlag[HWc][actIdx] = false;
      }
    }
    if (actUp && globalConfigMem[actionPtr + 3] == 1 && _systemHWcActionCacheFlag[HWc][actIdx]) { // "Hold Down"
      AtemSwitcher[devIndex].setMediaPlayerSourceStillIndex(globalConfigMem[actionPtr + 1], _systemHWcActionCache[HWc][actIdx]);
    }
    if (actUp && globalConfigMem[actionPtr + 3] == 5) { // "Cycle"
      _systemHWcActionCacheFlag[HWc][actIdx] = false;
    }

    if (pulses & 0xFFFE) {
      AtemSwitcher[devIndex].setMediaPlayerSourceStillIndex(globalConfigMem[actionPtr + 1], ATEM_searchMediaStill(devIndex, AtemSwitcher[devIndex].getMediaPlayerSourceStillIndex(globalConfigMem[actionPtr + 1]), (pulses >> 1), globalConfigMem[actionPtr + 2] - 1));
    }

    retVal = globalConfigMem[actionPtr + 3] == 5 ? (_systemHWcActionCacheFlag[HWc][actIdx] ? (4 | 0x20) : 5) : (AtemSwitcher[devIndex].getMediaPlayerSourceStillIndex(globalConfigMem[actionPtr + 1]) == (globalConfigMem[actionPtr + 2] - 1) ? (4 | 0x20) : 5);
    if (!AtemSwitcher[devIndex].getMediaPlayerStillFilesIsUsed(globalConfigMem[actionPtr + 2] - 1))
      retVal = 0;

    if (extRetValIsWanted()) {
      extRetVal(0, 7);
      extRetValShortLabel(PSTR("MPlayer "), (globalConfigMem[actionPtr + 1]) + 1);
      extRetValLongLabel(PSTR("Media Player "), (globalConfigMem[actionPtr + 2]) + 1);
      memset(_strCache, 0, 22);
      strcpy_P(_strCache, PSTR("Index "));

      if (_systemHWcActionPrefersLabel[HWc] && globalConfigMem[actionPtr + 3] != 3) {
        extRetValColor(!retVal ? 0 : (retVal & 0x20 ? B111101 : B101010));
        extRetValSetLabel(true);
        itoa(constrain(globalConfigMem[actionPtr + 2] - 1, 0, 31) + 1, _strCache + 6, 10);
        extRetValTxt(_strCache, 0);
        extRetVal2(0);
        extRetValTxt(AtemSwitcher[devIndex].getMediaPlayerStillFilesFileName(globalConfigMem[actionPtr + 2] - 1), 1);
      } else {
        extRetValColor(B111101);
        itoa(constrain(AtemSwitcher[devIndex].getMediaPlayerSourceStillIndex(globalConfigMem[actionPtr + 1]), 0, 31) + 1, _strCache + 6, 10);
        extRetValTxt(_strCache, 0);
        extRetVal2(0);
        extRetValTxt(AtemSwitcher[devIndex].getMediaPlayerStillFilesFileName(AtemSwitcher[devIndex].getMediaPlayerSourceStillIndex(globalConfigMem[actionPtr + 1])), 1);
      }
    }
    return retVal;
    break;
  case 11: // MP Still cycle
    break;
  case 12: // MP Clip
    break;
  case 13: // MP Control
    break;
  case 14: // CUT
    if (actDown || (pulses & 0xFFFE)) {
      AtemSwitcher[devIndex].performCutME(globalConfigMem[actionPtr + 1]);
      _systemHWcActionCacheFlag[HWc][actIdx] = true;
    }
    if (actUp) {
      _systemHWcActionCacheFlag[HWc][actIdx] = false;
    }
    retVal = _systemHWcActionCacheFlag[HWc][actIdx] ? (4 | 0x20) : 5;

    if (extRetValIsWanted()) {
      extRetVal(0, 7);
      extRetValColor(retVal & 0x20 ? B111111 : B101010);
      if (_systemHWcActionPrefersLabel[HWc])
        extRetValSetLabel(true);
      extRetValShortLabel(PSTR("ME"), (globalConfigMem[actionPtr + 1]) + 1);
      extRetValTxt_P(PSTR("CUT"), 0);
    }

    return retVal;
    break;
  case 15: // AUTO
    if (actDown || (pulses & 0xFFFE)) {
      AtemSwitcher[devIndex].performAutoME(globalConfigMem[actionPtr + 1]);
    }

    retVal = AtemSwitcher[devIndex].getTransitionInTransition(globalConfigMem[actionPtr + 1]) ? (2 | 0x20) : 5;

    if (extRetValIsWanted()) {
      extRetVal(0, 7);
      extRetValColor(retVal & 0x20 ? B110101 : B101010);
      if (_systemHWcActionPrefersLabel[HWc])
        extRetValSetLabel(true);
      extRetValShortLabel(PSTR("ME"), (globalConfigMem[actionPtr + 1]) + 1);
      extRetValTxt_P(PSTR("AUTO"), 0);
    }
    return retVal;
    break;
  case 16: // FTB
    if (actDown || (pulses & 0xFFFE)) {
      AtemSwitcher[devIndex].performFadeToBlackME(globalConfigMem[actionPtr + 1]);
    }

    retVal = AtemSwitcher[devIndex].getFadeToBlackStateFullyBlack(globalConfigMem[actionPtr + 1]) ? ((millis() & 512) > 0 ? 2 : 0) : (AtemSwitcher[devIndex].getFadeToBlackStateInTransition(globalConfigMem[actionPtr + 1]) ? 2 : 5);
    if (retVal != 5)
      retVal |= 0x20;

    if (extRetValIsWanted()) {
      extRetVal(0, 7);
      extRetValColor(retVal & 0x20 ? B110101 : B101010);
      if (_systemHWcActionPrefersLabel[HWc])
        extRetValSetLabel(true);
      extRetValShortLabel(PSTR("ME"), (globalConfigMem[actionPtr + 1]) + 1);
      extRetValTxt_P(PSTR("FTB"), 0);
    }
    return retVal;
    break;
  case 17: // Transition Style
    if (actDown) {
      if (globalConfigMem[actionPtr + 2] == 0) { // Cycle
        pulses = 2;
        _systemHWcActionCacheFlag[HWc][actIdx] = true;
      } else {
        AtemSwitcher[devIndex].setTransitionStyle(globalConfigMem[actionPtr + 1], globalConfigMem[actionPtr + 2] - 1);
      }
    }
    if (actUp) {
      _systemHWcActionCacheFlag[HWc][actIdx] = false;
    }
    if (pulses & 0xFFFE) {
      AtemSwitcher[devIndex].setTransitionStyle(globalConfigMem[actionPtr + 1], pulsesHelper(AtemSwitcher[devIndex].getTransitionStyle(globalConfigMem[actionPtr + 1]), 0, 4, true, pulses, 1, 1));
    }

    retVal = (globalConfigMem[actionPtr + 2] == 0 ? _systemHWcActionCacheFlag[HWc][actIdx] : AtemSwitcher[devIndex].getTransitionStyle(globalConfigMem[actionPtr + 1]) == globalConfigMem[actionPtr + 2] - 1) ? (4 | 0x20) : 5;

    if (extRetValIsWanted()) {
      extRetVal(0, 7);
      extRetValShortLabel(PSTR("ME"), (globalConfigMem[actionPtr + 1]) + 1);
      extRetValShortLabel(PSTR(" Trans"));
      extRetValLongLabel(PSTR("ME"), (globalConfigMem[actionPtr + 1]) + 1);
      extRetValLongLabel(PSTR(" Transition"));

      if (_systemHWcActionPrefersLabel[HWc] && globalConfigMem[actionPtr + 2] != 0) {
        extRetValColor(retVal & 0x20 ? B011111 : B101010);
        extRetValSetLabel(true);
      } else {
        extRetValColor(B011111);
      }
      switch (_systemHWcActionPrefersLabel[HWc] && globalConfigMem[actionPtr + 2] != 0 ? globalConfigMem[actionPtr + 2] - 1 : AtemSwitcher[devIndex].getTransitionStyle(globalConfigMem[actionPtr + 1])) {
      case 0: // Mix
        extRetValTxt_P(PSTR("Mix"), 0);
        break;
      case 1: // Dip
        extRetValTxt_P(PSTR("Dip"), 0);
        break;
      case 2: // Wipe
        extRetValTxt_P(PSTR("Wipe"), 0);
        break;
      case 3: // DVE
        extRetValTxt_P(PSTR("DVE"), 0);
        break;
      case 4: // Stinger
        extRetValTxt_P(PSTR("Sting"), 0);
        break;
      }
    }

    return retVal;
    break;
  case 18:                   // Transition Pos
    if (actDown) {           // Use actDown as "has moved"
      if (value != 0x8000) { // Value input
        AtemSwitcher[devIndex].setTransitionPosition(globalConfigMem[actionPtr + 1], value * 10);
        if (actUp) { // Use actUp as "at end"
          AtemSwitcher[devIndex].setTransitionPosition(globalConfigMem[actionPtr + 1], 0);
        }
      } else { // Binary - reset
        AtemSwitcher[devIndex].setTransitionPosition(globalConfigMem[actionPtr + 1], 0);
      }
    }
    return AtemSwitcher[devIndex].getTransitionInTransition(globalConfigMem[actionPtr + 1]) ? 0x20 : 0;
    break;
  case 19:                                                  // Play Macro
    if (!(_systemHWcActionCacheFlag[HWc][actIdx] & 0x80)) { // Set initial macro number: (using the "flag" variable to also hold the macro number and flag for cycling!)
      _systemHWcActionCacheFlag[HWc][actIdx] = (globalConfigMem[actionPtr + 1] - 1) | 0x80;
      Serial << "Set macro: " << (_systemHWcActionCacheFlag[HWc][actIdx] & 0x3F) << "\n";
    }

    if (actDown) {
      switch (globalConfigMem[actionPtr + 2]) {
      case 0:                                                                                    // Play
        AtemSwitcher[devIndex].setMacroAction(_systemHWcActionCacheFlag[HWc][actIdx] & 0x3F, 0); // playing
        AtemSwitcher[devIndex].setMacroRunChangePropertiesLooping(globalConfigMem[actionPtr + 3]);
        break;
      case 1: // Stop
        AtemSwitcher[devIndex].setMacroAction(0xFFFF, 1);
        break;
      case 2: // Toggle Play
      case 3: // Hold down Play
        if (AtemSwitcher[devIndex].getMacroRunStatusState() > 0) {
          AtemSwitcher[devIndex].setMacroAction(0xFFFF, 1);
        } else {
          AtemSwitcher[devIndex].setMacroAction(_systemHWcActionCacheFlag[HWc][actIdx] & 0x3F, 0); // playing
          AtemSwitcher[devIndex].setMacroRunChangePropertiesLooping(globalConfigMem[actionPtr + 3]);
        }
        break;
      case 4:
        if (AtemSwitcher[devIndex].getMacroRunStatusState() > 0) {
          AtemSwitcher[devIndex].setMacroAction(0xFFFF, 1);
          _systemHWcActionCacheFlag[HWc][actIdx] &= ~0x40;
        } else {
          _systemHWcActionCache[HWc][actIdx] = millis();
          _systemHWcActionCacheFlag[HWc][actIdx] |= 0x40;
        }
        break;
      }
    }
    if (globalConfigMem[actionPtr + 2] == 4 && _systemHWcActionCacheFlag[HWc][actIdx] & 0x40) { // Holding down...
      uint16_t t = millis();
      if (t - _systemHWcActionCache[HWc][actIdx] > 1000) {
        AtemSwitcher[devIndex].setMacroAction(_systemHWcActionCacheFlag[HWc][actIdx] & 0x3F, 0); // playing
        AtemSwitcher[devIndex].setMacroRunChangePropertiesLooping(globalConfigMem[actionPtr + 3]);

        _systemHWcActionCacheFlag[HWc][actIdx] &= ~0x40;
      }
    }
    if (actUp && globalConfigMem[actionPtr + 2] == 3) {
      AtemSwitcher[devIndex].setMacroAction(0xFFFF, 1);
    }
    if (actUp && globalConfigMem[actionPtr + 2] == 4) {
      if (_systemHWcActionCacheFlag[HWc][actIdx] & 0x40)
        pulses = 2;
      _systemHWcActionCacheFlag[HWc][actIdx] &= ~0x40;
    }
    if (pulses & 0xFFFE) {
      _systemHWcActionCacheFlag[HWc][actIdx] = ATEM_searchMacro(devIndex, _systemHWcActionCacheFlag[HWc][actIdx] & 0x3F, (pulses >> 1), globalConfigMem[actionPtr + 1] - 1) | (_systemHWcActionCacheFlag[HWc][actIdx] & 0xB0);
    }

    retVal = (AtemSwitcher[devIndex].getMacroRunStatusIndex() == (_systemHWcActionCacheFlag[HWc][actIdx] & 0x3F)) ^ (globalConfigMem[actionPtr + 2] == 1) ? (4 | 0x20) : 5;
    if (!AtemSwitcher[devIndex].getMacroPropertiesIsUsed(_systemHWcActionCacheFlag[HWc][actIdx] & 0x3F))
      retVal = 0;

    if (extRetValIsWanted()) {
      extRetVal(0, 7);
      extRetValShortLabel(PSTR("Macro #"), (_systemHWcActionCacheFlag[HWc][actIdx] & 0x3F) + 1);
      extRetValLongLabel(PSTR("Macro #"), (_systemHWcActionCacheFlag[HWc][actIdx] & 0x3F) + 1);
      extRetValTxt(AtemSwitcher[devIndex].getMacroPropertiesName(_systemHWcActionCacheFlag[HWc][actIdx] & 0x3F), 0);
      extRetVal2(0);

      if (_systemHWcActionPrefersLabel[HWc]) {
        extRetValColor(!retVal ? 0 : (retVal & 0x20 ? B011110 : B101010));
        extRetValSetLabel(true);
        switch (globalConfigMem[actionPtr + 2]) {
        case 0: // Play
          extRetValTxt_P(PSTR("Run"), 1);
          break;
        case 1: // Stop
          extRetValTxt_P(PSTR("Stop"), 1);
          break;
        case 2: // Toggle Play
          extRetValTxt_P(PSTR("Run/Stop"), 1);
          break;
        case 3: // Hold down Play
          extRetValTxt_P(PSTR("Hold2Run"), 1);
          break;
        case 4: // Cycle + Run (when held long enough)
          extRetValTxt_P(PSTR("Cycle/Run"), 1);
          break;
        }
      } else {
        extRetValTxt_P(retVal & 0x20 ? PSTR("Running...") : PSTR("Stopped"), 1);
        extRetValColor(B011110);
      }
    }
    return retVal;
    break;
  case 20: // Record Macro
    break;
  case 21: // Source cycle
    break;
  case 22: // Audio
    aSrc = ATEM_idxToAudioSrc(devIndex, globalConfigMem[actionPtr + 1]);

    if (actDown) {
      if (globalConfigMem[actionPtr + 3] == 3) {
        pulses = 2;
        _systemHWcActionCacheFlag[HWc][actIdx] = true;
      } else {
        switch (globalConfigMem[actionPtr + 2]) {
        case 2: // solo
          AtemSwitcher[devIndex].setAudioMixerMonitorSoloInput(aSrc);
          AtemSwitcher[devIndex].setAudioMixerMonitorSolo(globalConfigMem[actionPtr + 3] == 2 && AtemSwitcher[devIndex].getAudioMixerMonitorSolo() ? false : true);
          break;
        default:
          AtemSwitcher[devIndex].setAudioMixerInputMixOption(aSrc, globalConfigMem[actionPtr + 3] == 2 && AtemSwitcher[devIndex].getAudioMixerInputMixOption(aSrc) == (globalConfigMem[actionPtr + 2] + 1) ? 0 : (globalConfigMem[actionPtr + 2] + 1));
          break;
        }
      }
    }
    if (actUp) {
      _systemHWcActionCacheFlag[HWc][actIdx] = false;
    }
    if (actUp && globalConfigMem[actionPtr + 3] == 1) { // Hold down
      switch (globalConfigMem[actionPtr + 2]) {
      case 2: // solo
        AtemSwitcher[devIndex].setAudioMixerMonitorSolo(false);
        break;
      default:
        AtemSwitcher[devIndex].setAudioMixerInputMixOption(aSrc, 0);
        break;
      }
    }
    if (pulses & 0xFFFE) {
      switch (globalConfigMem[actionPtr + 2]) {
      case 2: // solo
        AtemSwitcher[devIndex].setAudioMixerMonitorSoloInput(aSrc);
        AtemSwitcher[devIndex].setAudioMixerMonitorSolo(pulsesHelper(AtemSwitcher[devIndex].getAudioMixerMonitorSolo(), 0, 1, true, pulses, 1, 1));
        break;
      default:
        AtemSwitcher[devIndex].setAudioMixerInputMixOption(aSrc, pulsesHelper(AtemSwitcher[devIndex].getAudioMixerInputMixOption(aSrc), 0, 2, true, pulses, 1, 1));
        break;
      }
    }

    switch (globalConfigMem[actionPtr + 2]) {
    case 2: // solo
      retVal = AtemSwitcher[devIndex].getAudioMixerMonitorSolo() ? (4 | 0x20) : 5;
      break;
    default:
      retVal = AtemSwitcher[devIndex].getAudioMixerInputMixOption(aSrc) == (globalConfigMem[actionPtr + 2] + 1) ? (4 | 0x20) : 5;
      break;
    }

    if (extRetValIsWanted()) {
      extRetVal(0, 7);
      extRetValShortLabel(PSTR("AudCh "));
      extRetValLongLabel(PSTR("Audio Ch "));
      switch (aSrc) {
      case 1001:
        extRetValShortLabel(PSTR("XLR"));
        extRetValLongLabel(PSTR("XLR"));
        break;
      case 1101:
        extRetValShortLabel(PSTR("AES"));
        extRetValLongLabel(PSTR("AES"));
        break;
      case 1201:
        extRetValShortLabel(PSTR("RCA"));
        extRetValLongLabel(PSTR("RCA"));
        break;
      case 2001:
        extRetValShortLabel(PSTR("MP1"));
        extRetValLongLabel(PSTR("MP1"));
        break;
      case 2002:
        extRetValShortLabel(PSTR("MP2"));
        extRetValLongLabel(PSTR("MP2"));
        break;
      default:
        extRetValShortLabel(PSTR(""), aSrc);
        extRetValLongLabel(PSTR(""), aSrc);
        break;
      }

      if (_systemHWcActionPrefersLabel[HWc] && globalConfigMem[actionPtr + 3] != 3) {
        extRetValColor(retVal & 0x20 ? B011110 : B101010);
        extRetValSetLabel(true);

        switch (globalConfigMem[actionPtr + 2]) {
        case 0:
          extRetValTxt_P(PSTR("On"), 0);
          break;
        case 1:
          extRetValTxt_P(PSTR("AFV"), 0);
          break;
        case 2:
          extRetValTxt_P(PSTR("Solo"), 0);
          break;
        }
      } else {
        extRetValColor(B011110);
        if (globalConfigMem[actionPtr + 2] == 2 && AtemSwitcher[devIndex].getAudioMixerMonitorSolo()) {
          extRetValTxt_P(PSTR("Solo"), 0);
        } else {
          switch (AtemSwitcher[devIndex].getAudioMixerInputMixOption(aSrc)) {
          case 1:
            extRetValTxt_P(PSTR("On"), 0);
            break;
          case 2:
            extRetValTxt_P(PSTR("AFV"), 0);
            break;
          case 0:
            extRetValTxt_P(PSTR("Off"), 0);
            break;
          }
        }
      }
    }

    return retVal;
    break;
  case 23: // Audio volume
    uint16_t audioVol;
    aSrc = ATEM_idxToAudioSrc(devIndex, globalConfigMem[actionPtr + 1]);
    switch (globalConfigMem[actionPtr + 1]) {
    case 25:
      audioVol = AtemSwitcher[devIndex].getAudioMixerMasterVolume();
      break;
    case 26:
      audioVol = AtemSwitcher[devIndex].getAudioMixerMonitorVolume();
      break;
    default:
      audioVol = AtemSwitcher[devIndex].getAudioMixerInputVolume(aSrc);
      break;
    }

    if (actDown || (pulses & 0xFFFE)) {
      int outValue;
      if (actDown) {
        if (value != 0x8000) { // Value input
          outValue = constrain(map(value, 0, 1000, -600, 60), -600, 60);
        } else { // Binary - reset / toggle
          outValue = (int)AtemSwitcher[devIndex].audioWord2Db(audioVol) != 0 ? 0 : -600;
        }
      }
      if (pulses & 0xFFFE) {
        outValue = pulsesHelper(AtemSwitcher[devIndex].audioWord2Db(audioVol), -600, 60, false, pulses, 2, 10);
      }
      switch (globalConfigMem[actionPtr + 1]) {
      case 25:
        AtemSwitcher[devIndex].setAudioMixerMasterVolume(AtemSwitcher[devIndex].audioDb2Word(outValue/10));
        break;
      case 26:
        AtemSwitcher[devIndex].setAudioMixerMonitorVolume(AtemSwitcher[devIndex].audioDb2Word(outValue/10));
        break;
      default:
        AtemSwitcher[devIndex].setAudioMixerInputVolume(aSrc, AtemSwitcher[devIndex].audioDb2Word(outValue/10));
        break;
      }
    }

    retVal = (int)AtemSwitcher[devIndex].audioWord2Db(audioVol) == 0 ? (4 | 0x20) : 5;

    if (extRetValIsWanted()) {
      extRetVal(AtemSwitcher[devIndex].audioWord2Db(audioVol), 3, _systemHWcActionFineFlag[HWc]);
      extRetValColor((int)AtemSwitcher[devIndex].audioWord2Db(audioVol) == 0 ? B111110 : ((int)AtemSwitcher[devIndex].audioWord2Db(audioVol) > 0 ? B111010 : (audioVol > AtemSwitcher[devIndex].audioDb2Word(-48) ? B101110 : B010101)));
      extRetValScale(1, -48, 6, -60, 0);
      extRetValShortLabel(PSTR("Vol. "));
      extRetValLongLabel(PSTR("AudioVol "));
      switch (globalConfigMem[actionPtr + 1]) {
      case 25:
        extRetValShortLabel(PSTR("Mast"));
        extRetValLongLabel(PSTR("Master"));
        break;
      case 26:
        extRetValShortLabel(PSTR("Mon"));
        extRetValLongLabel(PSTR("Monitor"));
        break;
      default:
        switch (aSrc) {
        case 1001:
          extRetValShortLabel(PSTR("XLR"));
          extRetValLongLabel(PSTR("XLR"));
          break;
        case 1101:
          extRetValShortLabel(PSTR("AES"));
          extRetValLongLabel(PSTR("AES"));
          break;
        case 1201:
          extRetValShortLabel(PSTR("RCA"));
          extRetValLongLabel(PSTR("RCA"));
          break;
        case 2001:
          extRetValShortLabel(PSTR("MP1"));
          extRetValLongLabel(PSTR("MP1"));
          break;
        case 2002:
          extRetValShortLabel(PSTR("MP2"));
          extRetValLongLabel(PSTR("MP2"));
          break;
        default:
          extRetValShortLabel(PSTR(""), aSrc);
          extRetValLongLabel(PSTR(""), aSrc);
          break;
        }
        break;
      }
    }
    return retVal;
    break;
  case 24: // Audio Balance
    aSrc = ATEM_idxToAudioSrc(devIndex, globalConfigMem[actionPtr + 1]);
    if (actDown) {
      if (value != 0x8000) { // Value input
        int outValue = constrain(map(value, 0, 1000, -10000, 10000), -10000, 10000);
        AtemSwitcher[devIndex].setAudioMixerInputBalance(aSrc, outValue);
      } else { // Binary - reset
        AtemSwitcher[devIndex].setAudioMixerInputBalance(aSrc, 0);
      }
    }
    if (pulses & 0xFFFE) {
      AtemSwitcher[devIndex].setAudioMixerInputBalance(aSrc, pulsesHelper(AtemSwitcher[devIndex].getAudioMixerInputBalance(aSrc), -10000, 10000, false, pulses, 100, 1000));
    }

    retVal = AtemSwitcher[devIndex].getAudioMixerInputBalance(aSrc) == 0 ? (4 | 0x20) : 5;

    if (extRetValIsWanted()) {
      extRetVal(map(AtemSwitcher[devIndex].getAudioMixerInputBalance(aSrc), -10000, 10000, -50, 50), 0, _systemHWcActionFineFlag[HWc]);
      extRetValColor(retVal & 0x20 ? B111110 : B101110);
      extRetValScale(2, -50, 50, -50, 50);
      extRetValShortLabel(PSTR("Bal. "));
      extRetValLongLabel(PSTR("AudioBal "));
      switch (aSrc) {
      case 1001:
        extRetValShortLabel(PSTR("XLR"));
        extRetValLongLabel(PSTR("XLR"));
        break;
      case 1101:
        extRetValShortLabel(PSTR("AES"));
        extRetValLongLabel(PSTR("AES"));
        break;
      case 1201:
        extRetValShortLabel(PSTR("RCA"));
        extRetValLongLabel(PSTR("RCA"));
        break;
      case 2001:
        extRetValShortLabel(PSTR("MP1"));
        extRetValLongLabel(PSTR("MP1"));
        break;
      case 2002:
        extRetValShortLabel(PSTR("MP2"));
        extRetValLongLabel(PSTR("MP2"));
        break;
      default:
        extRetValShortLabel(PSTR(""), aSrc);
        extRetValLongLabel(PSTR(""), aSrc);
        break;
      }
    }
    return retVal;
    break;
  case 25: // Audio Levels
    if (_systemHWcActionCacheFlag[HWc][actIdx] == 0) {
      AtemSwitcher[devIndex].setAudioLevelsEnable(true);
      _systemHWcActionCacheFlag[HWc][actIdx] = 1;
    } else if (_systemHWcActionCacheFlag[HWc][actIdx] == 1) {
      if ((millis() & 0x2000) == 0x2000) {
        _systemHWcActionCacheFlag[HWc][actIdx] = 2;
      }
    } else { // ==2
      if ((millis() & 0x2000) != 0x2000) {
        _systemHWcActionCacheFlag[HWc][actIdx] = 0;
      }
    }

    switch (globalConfigMem[actionPtr + 1]) {
    case 25:
      retVal = ((((int)AtemSwitcher[devIndex].audioWord2Db(AtemSwitcher[devIndex].getAudioMixerLevelsMasterLeft()) + 60) & 0xFF) << 8) | (((int)AtemSwitcher[devIndex].audioWord2Db(AtemSwitcher[devIndex].getAudioMixerLevelsMasterRight()) + 60) & 0xFF);
      break;
    default:
      retVal = ((((int)AtemSwitcher[devIndex].audioWord2Db(AtemSwitcher[devIndex].getAudioMixerLevelsSourceLeft(ATEM_idxToAudioSrc(devIndex, globalConfigMem[actionPtr + 1]))) + 60) & 0xFF) << 8) | (((int)AtemSwitcher[devIndex].audioWord2Db(AtemSwitcher[devIndex].getAudioMixerLevelsSourceRight(ATEM_idxToAudioSrc(devIndex, globalConfigMem[actionPtr + 1]))) + 60) & 0xFF);
      break;
    }
    return retVal;
    break;
  case 26: // Transitions Rate
    tempByte = globalConfigMem[actionPtr + 2] == 0 ? constrain(_systemHWcActionCacheFlag[HWc][actIdx] & 0xF, 1, 5) : globalConfigMem[actionPtr + 2];

    switch (tempByte) {
    case 1:
      retVal = AtemSwitcher[devIndex].getTransitionMixRate(globalConfigMem[actionPtr + 1]);
      break;
    case 2:
      retVal = AtemSwitcher[devIndex].getTransitionDipRate(globalConfigMem[actionPtr + 1]);
      break;
    case 3:
      retVal = AtemSwitcher[devIndex].getTransitionWipeRate(globalConfigMem[actionPtr + 1]);
      break;
    case 4:
      retVal = AtemSwitcher[devIndex].getTransitionDVERate(globalConfigMem[actionPtr + 1]);
      break;
    case 5:
      retVal = AtemSwitcher[devIndex].getFadeToBlackRate(globalConfigMem[actionPtr + 1]);
      break;
    }
    tempInt = 0;
    if (actDown || (pulses & 0xFFFE)) {
      if (actDown) {
        if (globalConfigMem[actionPtr + 2] == 0) {
          _systemHWcActionCacheFlag[HWc][actIdx] |= 0x80;                                                                                                                                            // Button is pressed
          _systemHWcActionCacheFlag[HWc][actIdx] = _systemHWcActionCacheFlag[HWc][actIdx] & 0x40 ? _systemHWcActionCacheFlag[HWc][actIdx] & (~0x40) : _systemHWcActionCacheFlag[HWc][actIdx] | 0x40; // Changing direction
          _systemHWcActionCacheFlag[HWc][actIdx] &= ~0x20;                                                                                                                                           // Clear "initial hold" flag
          _systemHWcActionCache[HWc][actIdx] = millis();                                                                                                                                             // Reset counter
        } else
          tempInt = constrain(value != 0x8000 ? map(value, 0, 1000, 1, 250) : globalConfigMem[actionPtr + 3], 1, 250);
      }
      if (pulses & 0xFFFE) {
        tempInt = pulsesHelper(retVal, 1, 250, true, pulses, 1, 5);
      }
    }
    if (_systemHWcActionCacheFlag[HWc][actIdx] & 0x80) { // Button is pressed
      if (_systemHWcActionCacheFlag[HWc][actIdx] & 0x20) {
        if ((uint16_t)millis() - _systemHWcActionCache[HWc][actIdx] > 300) {
          _systemHWcActionCache[HWc][actIdx] = millis();
          tempInt = pulsesHelper(retVal, 1, 250, true, (((int)(_systemHWcActionCacheFlag[HWc][actIdx] & 0x40) ? -1 : 1) << 1), 1, 1);
        }
      } else { // Initial hold
        if ((uint16_t)millis() - _systemHWcActionCache[HWc][actIdx] > 1000) {
          _systemHWcActionCache[HWc][actIdx] = millis();
          _systemHWcActionCacheFlag[HWc][actIdx] |= 0x20;
        }
      }
    }
    if (tempInt) {
      switch (tempByte) {
      case 1:
        AtemSwitcher[devIndex].setTransitionMixRate(globalConfigMem[actionPtr + 1], tempInt);
        break;
      case 2:
        AtemSwitcher[devIndex].setTransitionDipRate(globalConfigMem[actionPtr + 1], tempInt);
        break;
      case 3:
        AtemSwitcher[devIndex].setTransitionWipeRate(globalConfigMem[actionPtr + 1], tempInt);
        break;
      case 4:
        AtemSwitcher[devIndex].setTransitionDVERate(globalConfigMem[actionPtr + 1], tempInt);
        break;
      case 5:
        AtemSwitcher[devIndex].setFadeToBlackRate(globalConfigMem[actionPtr + 1], tempInt);
        break;
      }
      Serial << "Set trans frame rate";
    }

    if (actUp && globalConfigMem[actionPtr + 2] == 0 && !(_systemHWcActionCacheFlag[HWc][actIdx] & 0x20)) {
      tempByte = _systemHWcActionCacheFlag[HWc][actIdx] = pulsesHelper(tempByte, 1, 5, true, 2, 1, 1);
    }
    if (actUp) {
      _systemHWcActionCacheFlag[HWc][actIdx] &= ~0x80;
    }

    if (extRetValIsWanted()) {
      if (globalConfigMem[actionPtr + 2] != 0) {
        extRetVal(globalConfigMem[actionPtr + 3], 4, _systemHWcActionFineFlag[HWc]);
        extRetValColor(retVal == globalConfigMem[actionPtr + 3] ? B011110 : B101010);
        extRetValSetLabel(true);
      } else {
        extRetVal(retVal, 4, _systemHWcActionFineFlag[HWc]);
        extRetValColor(B011110);
      }
      extRetValShortLabel(PSTR("ME"), (globalConfigMem[actionPtr + 1]) + 1);
      extRetValLongLabel(PSTR("ME"), (globalConfigMem[actionPtr + 1]) + 1);
      switch (tempByte) {
      case 1:
        extRetValShortLabel(PSTR(" Mix"));
        extRetValLongLabel(PSTR(" Mix Rate"));
        break;
      case 2:
        extRetValShortLabel(PSTR(" Dip"));
        extRetValLongLabel(PSTR(" Dip Rate"));
        break;
      case 3:
        extRetValShortLabel(PSTR(" Wipe"));
        extRetValLongLabel(PSTR(" Wipe Rate"));
        break;
      case 4:
        extRetValShortLabel(PSTR(" DVE"));
        extRetValLongLabel(PSTR(" DVE Rate"));
        break;
      case 5:
        extRetValShortLabel(PSTR(" FTB"));
        extRetValLongLabel(PSTR(" FTB Rate"));
        break;
      }
    }
    return retVal == globalConfigMem[actionPtr + 3] ? (4 | 0x20) : 5;
    break;
  case 27: //
    break;
  case 28: // Upstream Keyer Parameters
    break;
  case 29: // Downstream Keyer Parameters
    break;
  case 30: // Focus
    cam = ATEM_idxToCamera(globalConfigMem[actionPtr + 1]);
    if (actDown) {
      if (value == 0x8000) { // Binary input
        Serial << F("Perform Auto Focus... TODO\n");
      }
    }
    if (pulses & 0xFFFE) {
      AtemSwitcher[devIndex].setCameraControlFocus(cam, pulsesHelper(0, -1000, 1000, false, pulses, 20, 200));
    }
    if (extRetValIsWanted()) {
      extRetVal(0, 7);
      extRetValShortLabel(PSTR("Focus"));
      extRetValLongLabel(PSTR("Focus Cam "), cam);
      extRetValTxt(PSTR("Focus"), 0);
      extRetValColor(B101000);
    }
    break;
  case 31: // Iris
    cam = ATEM_idxToCamera(globalConfigMem[actionPtr + 1]);
    if (actDown) {
      if (value != 0x8000) { // Value input
        int outValue = constrain(map(value, 1000, 0, 0, 2048), 0, 2048);
        AtemSwitcher[devIndex].setCameraControlIris(cam, outValue);
      } else { // Binary - auto iris
        Serial << F("Perform Auto Iris... TODO\n");
      }
    }
    if (pulses & 0xFFFE) {
      AtemSwitcher[devIndex].setCameraControlIris(cam, pulsesHelper(AtemSwitcher[devIndex].getCameraControlIris(cam), 0, 2048, false, ((-(pulses >> 1)) << 1) | (pulses & B1), 20, 200));
    }
    if (extRetValIsWanted()) {
      extRetVal(100 - constrain(((long)AtemSwitcher[devIndex].getCameraControlIris(cam) * 100) >> 11, 0, 100), 2, _systemHWcActionFineFlag[HWc]);
      extRetValScale(1, 0, 100, 0, 100);
      extRetValShortLabel(PSTR("Iris"));
      extRetValLongLabel(PSTR("Iris Cam "), cam);
      extRetValColor(B011011);
    }
    break;
  case 32:                                                                 // Sensor Gain
    static const uint16_t sensorGains[] PROGMEM = {512, 1024, 2048, 4096}; // 4
    cam = ATEM_idxToCamera(globalConfigMem[actionPtr + 1]);
    if (actDown || (pulses & 0xFFFE)) {
      // Find current value index:
      uint8_t currentGainIndex = 0;
      for (uint8_t b = 0; b < 4; b++) {
        if ((AtemSwitcher[devIndex].getCameraControlGain(cam) & 0xFE00) <= (pgm_read_word_near(sensorGains + b) + 1)) {
          currentGainIndex = b;
          break;
        }
      }

      if (actDown && value == 0x8000) {            // Binary (never value)
        if (globalConfigMem[actionPtr + 2] == 0) { // cycle
          pulses = 2;
        } else { // Set
          AtemSwitcher[devIndex].setCameraControlGain(cam, pgm_read_word_near(sensorGains + globalConfigMem[actionPtr + 2] - 1));
        }
      }
      if ((pulses & 0xFFFE)) {
        AtemSwitcher[devIndex].setCameraControlGain(cam, pgm_read_word_near(sensorGains + pulsesHelper(currentGainIndex, 0, 4 - 1, true, pulses, 1, 1)));
      }
    }
    if (extRetValIsWanted()) {
      extRetVal(round(log10((AtemSwitcher[devIndex].getCameraControlGain(cam) & 0xFE00) >> 9) * 19.9315), 3, _systemHWcActionFineFlag[HWc]);
      extRetValShortLabel(PSTR("Sens.Gain"));
      extRetValLongLabel(PSTR("Sens.Gain Cam "), cam);
      extRetValColor(B011110);
    }
    break;
  case 33: // Shutter
    static const uint16_t shutterSpeeds[] PROGMEM = {20000, 16667, 13333, 11111, 10000, 8333, 6667, 5556, 4000, 2778, 2000, 1379, 1000, 690, 500};
    cam = ATEM_idxToCamera(globalConfigMem[actionPtr + 1]);
    if (actDown || (pulses & 0xFFFE)) {
      // Find current value index:
      uint8_t currentShutterSpeedIndex = 0;
      for (uint8_t b = 0; b < 15; b++) {
        if (AtemSwitcher[devIndex].getCameraControlShutter(cam) >= (pgm_read_word_near(shutterSpeeds + b) - 1)) {
          currentShutterSpeedIndex = b;
          break;
        }
      }

      if (actDown && value == 0x8000) {            // Binary (never value)
        if (globalConfigMem[actionPtr + 2] == 0) { // cycle
          pulses = 2;
        } else { // Set
          AtemSwitcher[devIndex].setCameraControlShutter(cam, pgm_read_word_near(shutterSpeeds + globalConfigMem[actionPtr + 2] - 1));
        }
      }
      if ((pulses & 0xFFFE)) {
        AtemSwitcher[devIndex].setCameraControlShutter(cam, pgm_read_word_near(shutterSpeeds + pulsesHelper(currentShutterSpeedIndex, 0, 15 - 1, true, pulses, 1, 1)));
      }
    }
    if (extRetValIsWanted()) {
      extRetVal(round((float)1000000 / AtemSwitcher[devIndex].getCameraControlShutter(cam)), 5, _systemHWcActionFineFlag[HWc]);
      extRetValShortLabel(PSTR("Shutter"));
      extRetValLongLabel(PSTR("Shutter Cam "), cam);
      extRetValColor(B011110);
    }
    break;
  case 34:                                                                                                                                                        // White Balance
    static const uint16_t whiteBalances[] PROGMEM = {2500, 2800, 3000, 3200, 3400, 3600, 4000, 4500, 4800, 5000, 5200, 5400, 5600, 6000, 6500, 7000, 7500, 8000}; // 18
    cam = ATEM_idxToCamera(globalConfigMem[actionPtr + 1]);
    if (actDown || (pulses & 0xFFFE)) {
      // Find current value index:
      uint8_t currentWhiteBalanceIndex = 0;
      for (uint8_t b = 0; b < 18; b++) {
        if (AtemSwitcher[devIndex].getCameraControlWhiteBalance(cam) <= (pgm_read_word_near(whiteBalances + b) + 1)) {
          currentWhiteBalanceIndex = b;
          break;
        }
      }

      if (actDown && value == 0x8000) {            // Binary (never value)
        if (globalConfigMem[actionPtr + 2] == 0) { // cycle
          pulses = 2;
        } else { // Set
          AtemSwitcher[devIndex].setCameraControlWhiteBalance(cam, pgm_read_word_near(whiteBalances + globalConfigMem[actionPtr + 2] - 1));
        }
      }
      if ((pulses & 0xFFFE)) {
        AtemSwitcher[devIndex].setCameraControlWhiteBalance(cam, pgm_read_word_near(whiteBalances + pulsesHelper(currentWhiteBalanceIndex, 0, 18 - 1, true, pulses, 1, 1)));
      }
    }
    if (extRetValIsWanted()) {
      extRetVal(AtemSwitcher[devIndex].getCameraControlWhiteBalance(cam), 6, _systemHWcActionFineFlag[HWc]);
      extRetValShortLabel(PSTR("WhiteBal"));
      extRetValLongLabel(PSTR("WhiteBal Cam "), cam);
      extRetValColor(B011110);
    }
    break;
  case 35: // Lift
  case 36: // Gamma
  case 37: // Gain
    cam = ATEM_idxToCamera(globalConfigMem[actionPtr + 2]);
    if (actDown) {           // Binary or Value input...
      int outValue = globalConfigMem[actionPtr]==37?2048:0;      // Binary (reset) value by default
      if (value != 0x8000) { // Value input different from -32768
        switch (globalConfigMem[actionPtr]) {
        case 35: // Lift:
          outValue = constrain(map(value, 0, 1000, -4096/8, 4096/8), -4096, 4096);
          break;
        case 36: // Gamma:
          outValue = constrain(map(value, 0, 1000, -8192, 8192), -8192, 8192);
          break;
        case 37: // Gain:
          outValue = constrain(map(value, 0, 1000, 0, 32767), 0, 32767);
          break;
        }
      }
      switch (globalConfigMem[actionPtr]) {
      case 35: // Lift:
        switch (globalConfigMem[actionPtr + 1]) {
        case 0:
          AtemSwitcher[devIndex].setCameraControlLiftY(cam, outValue);
          break;
        case 1:
          AtemSwitcher[devIndex].setCameraControlLiftR(cam, outValue);
          break;
        case 2:
          AtemSwitcher[devIndex].setCameraControlLiftG(cam, outValue);
          break;
        case 3:
          AtemSwitcher[devIndex].setCameraControlLiftB(cam, outValue);
          break;
        }
        break;
      case 36: // Gamma:
        switch (globalConfigMem[actionPtr + 1]) {
        case 0:
          AtemSwitcher[devIndex].setCameraControlGammaY(cam, outValue);
          break;
        case 1:
          AtemSwitcher[devIndex].setCameraControlGammaR(cam, outValue);
          break;
        case 2:
          AtemSwitcher[devIndex].setCameraControlGammaG(cam, outValue);
          break;
        case 3:
          AtemSwitcher[devIndex].setCameraControlGammaB(cam, outValue);
          break;
        }
        break;
      case 37: // Gain:
        switch (globalConfigMem[actionPtr + 1]) {
        case 0:
          AtemSwitcher[devIndex].setCameraControlGainY(cam, outValue);
          break;
        case 1:
          AtemSwitcher[devIndex].setCameraControlGainR(cam, outValue);
          break;
        case 2:
          AtemSwitcher[devIndex].setCameraControlGainG(cam, outValue);
          break;
        case 3:
          AtemSwitcher[devIndex].setCameraControlGainB(cam, outValue);
          break;
        }
        break;
      }
    }

    if (pulses & 0xFFFE) { // Encoder value (excluding the fast/slow bit 0)
      switch (globalConfigMem[actionPtr]) {
      case 35: // Lift:
        switch (globalConfigMem[actionPtr + 1]) {
        case 0:
          AtemSwitcher[devIndex].setCameraControlLiftY(cam, pulsesHelper(AtemSwitcher[devIndex].getCameraControlLiftY(cam), -4096, 4096, false, pulses, 20, 200));
          break;
        case 1:
          AtemSwitcher[devIndex].setCameraControlLiftR(cam, pulsesHelper(AtemSwitcher[devIndex].getCameraControlLiftR(cam), -4096, 4096, false, pulses, 20, 200));
          break;
        case 2:
          AtemSwitcher[devIndex].setCameraControlLiftG(cam, pulsesHelper(AtemSwitcher[devIndex].getCameraControlLiftG(cam), -4096, 4096, false, pulses, 20, 200));
          break;
        case 3:
          AtemSwitcher[devIndex].setCameraControlLiftB(cam, pulsesHelper(AtemSwitcher[devIndex].getCameraControlLiftB(cam), -4096, 4096, false, pulses, 20, 200));
          break;
        }
        break;
      case 36: // Gamma:
        switch (globalConfigMem[actionPtr + 1]) {
        case 0:
          AtemSwitcher[devIndex].setCameraControlGammaY(cam, pulsesHelper(AtemSwitcher[devIndex].getCameraControlGammaY(cam), -8192, 8192, false, pulses, 50, 500));
          break;
        case 1:
          AtemSwitcher[devIndex].setCameraControlGammaR(cam, pulsesHelper(AtemSwitcher[devIndex].getCameraControlGammaR(cam), -8192, 8192, false, pulses, 50, 500));
          break;
        case 2:
          AtemSwitcher[devIndex].setCameraControlGammaG(cam, pulsesHelper(AtemSwitcher[devIndex].getCameraControlGammaG(cam), -8192, 8192, false, pulses, 50, 500));
          break;
        case 3:
          AtemSwitcher[devIndex].setCameraControlGammaB(cam, pulsesHelper(AtemSwitcher[devIndex].getCameraControlGammaB(cam), -8192, 8192, false, pulses, 50, 500));
          break;
        }
        break;
      case 37: // Gain:
        switch (globalConfigMem[actionPtr + 1]) {
        case 0:
          AtemSwitcher[devIndex].setCameraControlGainY(cam, pulsesHelper(AtemSwitcher[devIndex].getCameraControlGainY(cam), 0, 32767, false, pulses, 50, 500));
          break;
        case 1:
          AtemSwitcher[devIndex].setCameraControlGainR(cam, pulsesHelper(AtemSwitcher[devIndex].getCameraControlGainR(cam), 0, 32767, false, pulses, 50, 500));
          break;
        case 2:
          AtemSwitcher[devIndex].setCameraControlGainG(cam, pulsesHelper(AtemSwitcher[devIndex].getCameraControlGainG(cam), 0, 32767, false, pulses, 50, 500));
          break;
        case 3:
          AtemSwitcher[devIndex].setCameraControlGainB(cam, pulsesHelper(AtemSwitcher[devIndex].getCameraControlGainB(cam), 0, 32767, false, pulses, 50, 500));
          break;
        }
        break;
      }
    }
    if (extRetValIsWanted()) { // Update displays:
      switch (globalConfigMem[actionPtr]) {
      case 35: // Lift:
        switch (globalConfigMem[actionPtr + 1]) {
        case 0:
          extRetVal(map(AtemSwitcher[devIndex].getCameraControlLiftY(cam), -4096, 4096, -1000, 1000), 1, _systemHWcActionFineFlag[HWc]);
          break;
        case 1:
          extRetVal(map(AtemSwitcher[devIndex].getCameraControlLiftR(cam), -4096, 4096, -1000, 1000), 1, _systemHWcActionFineFlag[HWc]);
          break;
        case 2:
          extRetVal(map(AtemSwitcher[devIndex].getCameraControlLiftG(cam), -4096, 4096, -1000, 1000), 1, _systemHWcActionFineFlag[HWc]);
          break;
        case 3:
          extRetVal(map(AtemSwitcher[devIndex].getCameraControlLiftB(cam), -4096, 4096, -1000, 1000), 1, _systemHWcActionFineFlag[HWc]);
          break;
        }
        extRetValShortLabel(PSTR("Lift-"));
        extRetValLongLabel(PSTR("Lift-"));
        _extRetShort[5] = yrgbLabels[globalConfigMem[actionPtr + 1]];
        _extRetLong[5] = yrgbLabels[globalConfigMem[actionPtr + 1]];
        break;
      case 36: // Gamma:
        switch (globalConfigMem[actionPtr + 1]) {
        case 0:
          extRetVal(map(AtemSwitcher[devIndex].getCameraControlGammaY(cam), -8192, 8192, -1000, 1000), 1, _systemHWcActionFineFlag[HWc]);
          break;
        case 1:
          extRetVal(map(AtemSwitcher[devIndex].getCameraControlGammaR(cam), -8192, 8192, -1000, 1000), 1, _systemHWcActionFineFlag[HWc]);
          break;
        case 2:
          extRetVal(map(AtemSwitcher[devIndex].getCameraControlGammaG(cam), -8192, 8192, -1000, 1000), 1, _systemHWcActionFineFlag[HWc]);
          break;
        case 3:
          extRetVal(map(AtemSwitcher[devIndex].getCameraControlGammaB(cam), -8192, 8192, -1000, 1000), 1, _systemHWcActionFineFlag[HWc]);
          break;
        }
        extRetValShortLabel(PSTR("Gamma-"));
        extRetValLongLabel(PSTR("Gamma-"));
        _extRetShort[6] = yrgbLabels[globalConfigMem[actionPtr + 1]];
        _extRetLong[6] = yrgbLabels[globalConfigMem[actionPtr + 1]];
        break;
      case 37: // Gain:
        switch (globalConfigMem[actionPtr + 1]) {
        case 0:
          extRetVal(map(AtemSwitcher[devIndex].getCameraControlGainY(cam), 0, 32767, 0, 16000), 1, _systemHWcActionFineFlag[HWc]);
          break;
        case 1:
          extRetVal(map(AtemSwitcher[devIndex].getCameraControlGainR(cam), 0, 32767, 0, 16000), 1, _systemHWcActionFineFlag[HWc]);
          break;
        case 2:
          extRetVal(map(AtemSwitcher[devIndex].getCameraControlGainG(cam), 0, 32767, 0, 16000), 1, _systemHWcActionFineFlag[HWc]);
          break;
        case 3:
          extRetVal(map(AtemSwitcher[devIndex].getCameraControlGainB(cam), 0, 32767, 0, 16000), 1, _systemHWcActionFineFlag[HWc]);
          break;
        }
        extRetValShortLabel(PSTR("Gain-"));
        extRetValLongLabel(PSTR("Gain-"));
        _extRetShort[5] = yrgbLabels[globalConfigMem[actionPtr + 1]];
        _extRetLong[5] = yrgbLabels[globalConfigMem[actionPtr + 1]];
        break;
      }
      _extRetShortPtr++;
      _extRetLongPtr++;
      extRetValLongLabel(PSTR(" Cam"), cam);

      switch (globalConfigMem[actionPtr + 1]) {
      case 0:
        extRetValColor(B111111);
        break;
      case 1:
        extRetValColor(B110101);
        break;
      case 2:
        extRetValColor(B011101);
        break;
      case 3:
        extRetValColor(B010111);
        break;
      }
    }
    break;
  case 38: // Hue
    cam = ATEM_idxToCamera(globalConfigMem[actionPtr + 1]);
    if (actDown) {
      if (value != 0x8000) { // Value input
        int outValue = constrain(map(value, 0, 1000, -2048, 2048), -2048, 2048);
        AtemSwitcher[devIndex].setCameraControlHue(cam, outValue);
      } else { // Binary - reset
        AtemSwitcher[devIndex].setCameraControlHue(cam, 0);
      }
    }
    if (pulses & 0xFFFE) {
      AtemSwitcher[devIndex].setCameraControlHue(cam, pulsesHelper(AtemSwitcher[devIndex].getCameraControlHue(cam), -2048, 2048, true, pulses, 20, 200));
    }
    if (extRetValIsWanted()) {
      extRetVal(map(AtemSwitcher[devIndex].getCameraControlHue(cam), -2048, 2048, 0, 360), 0, _systemHWcActionFineFlag[HWc]);
      extRetValScale(2, 0, 360, 0, 360);
      extRetValShortLabel(PSTR("Hue"));
      extRetValLongLabel(PSTR("Hue Cam "), cam);
      extRetValColor(B100111);
    }
    break;
  case 39: // Contrast
    cam = ATEM_idxToCamera(globalConfigMem[actionPtr + 1]);
    if (actDown) {
      if (value != 0x8000) { // Value input
        int outValue = constrain(map(value, 0, 1000, 0, 4096), 0, 4096);
        AtemSwitcher[devIndex].setCameraControlContrast(cam, outValue);
      } else { // Binary - reset
        AtemSwitcher[devIndex].setCameraControlContrast(cam, 2048);
      }
    }
    if (pulses & 0xFFFE) {
      AtemSwitcher[devIndex].setCameraControlContrast(cam, pulsesHelper(AtemSwitcher[devIndex].getCameraControlContrast(cam), 0, 4096, false, pulses, 20, 200));
    }
    if (extRetValIsWanted()) {
      extRetVal(map(AtemSwitcher[devIndex].getCameraControlContrast(cam), 0, 4096, 0, 100), 2, _systemHWcActionFineFlag[HWc]);
      extRetValScale(2, 0, 100, 0, 100);
      extRetValShortLabel(PSTR("Contrast"));
      extRetValLongLabel(PSTR("Contrast Cam "), cam);
      extRetValColor(B100111);
    }
    break;
  case 40: // Saturation
    cam = ATEM_idxToCamera(globalConfigMem[actionPtr + 1]);
    if (actDown) {
      if (value != 0x8000) { // Value input
        int outValue = constrain(map(value, 0, 1000, 0, 4096), 0, 4096);
        AtemSwitcher[devIndex].setCameraControlSaturation(cam, outValue);
      } else { // Binary - reset
        AtemSwitcher[devIndex].setCameraControlSaturation(cam, globalConfigMem[actionPtr + 2] == 1 ? 2048 : 0);
      }
    }
    if (pulses & 0xFFFE) {
      AtemSwitcher[devIndex].setCameraControlSaturation(cam, pulsesHelper(AtemSwitcher[devIndex].getCameraControlSaturation(cam), 0, 4096, false, pulses, 20, 200));
    }
    if (extRetValIsWanted()) {
      extRetVal(map(AtemSwitcher[devIndex].getCameraControlSaturation(cam), 0, 4096, 0, 100), 2, _systemHWcActionFineFlag[HWc]);
      extRetValScale(2, 0, 100, 0, 100);
      extRetValShortLabel(PSTR("Saturate"));
      extRetValLongLabel(PSTR("Saturate Cam "), cam);
      extRetValColor(B100111);
    }
    break;
  case 41: // Bars
    break;
  case 42: // Video Tally
    retVal = 5;
    switch (globalConfigMem[actionPtr + 2]) {
    case 1:
    case 2:
      retVal = AtemSwitcher[devIndex].getTallyFlags(ATEM_idxToVideoSrc(devIndex, globalConfigMem[actionPtr + 1])) & B10 ? (3 | 0x20) : 5;
      if (globalConfigMem[actionPtr + 2] == 1) {
        break;
      }
    }
    switch (globalConfigMem[actionPtr + 2]) {
    case 0:
    case 2:
      retVal = AtemSwitcher[devIndex].getTallyFlags(ATEM_idxToVideoSrc(devIndex, globalConfigMem[actionPtr + 1])) & B1 ? (2 | 0x20) : retVal;
      break;
    }
    if (globalConfigMem[actionPtr + 2] == 2 && (retVal & 0xF) == 3)
      retVal |= 0x10; // Bit 4 is a "mono-color blink flag" so a mono color button can indicate this special state.

    if (extRetValIsWanted()) {
      extRetVal(0, 7);
      switch (globalConfigMem[actionPtr + 2]) {
      case 0:
        extRetValColor(retVal & 0x20 ? B110101 : B101010);
        extRetValShortLabel(PSTR("Prog Tally"));
        extRetValLongLabel(PSTR("Program Tally"));
        break;
      case 1:
        extRetValColor(retVal & 0x20 ? B011101 : B101010);
        extRetValShortLabel(PSTR("Prev Tally"));
        extRetValLongLabel(PSTR("Preview Tally"));
        break;
      case 2:
        extRetValColor((retVal & 0xF) == 3 ? B011101 : ((retVal & 0xF) == 2 ? B110101 : B101010));
        extRetValShortLabel(PSTR("PrgPrv Tly"));
        extRetValLongLabel(PSTR("Prog/Prev Tally"));
        break;
      }

      extRetValSetLabel(true);
      extRetValTxt(AtemSwitcher[devIndex].getInputLongName(ATEM_idxToVideoSrc(devIndex, globalConfigMem[actionPtr + 1])), 0);
      extRetValTxtShort(AtemSwitcher[devIndex].getInputShortName(ATEM_idxToVideoSrc(devIndex, globalConfigMem[actionPtr + 1])));
    }
    return retVal;
    break;
  case 43: // Audio Tally
    aSrc = ATEM_idxToAudioSrc(devIndex, globalConfigMem[actionPtr + 1]);
    retVal = AtemSwitcher[devIndex].getAudioTallyFlags(ATEM_idxToAudioSrc(devIndex, globalConfigMem[actionPtr + 1])) ? (4 | 0x20) : 5;

    if (extRetValIsWanted()) {
      extRetVal(0, 7);
      extRetValShortLabel(PSTR("Audio Tly"));
      extRetValLongLabel(PSTR("Audio Tally"));

      extRetValColor(retVal & 0x20 ? B111100 : B101010);
      extRetValSetLabel(true);

      switch (globalConfigMem[actionPtr + 1]) {
      case 25:
        extRetValTxt(PSTR("Master"), 0);
        break;
      case 26:
        extRetValTxt(PSTR("Monitor"), 0);
        break;
      default:
        switch (aSrc) {
        case 1001:
          extRetValTxt_P(PSTR("XLR"), 0);
          break;
        case 1101:
          extRetValTxt_P(PSTR("AES"), 0);
          break;
        case 1201:
          extRetValTxt_P(PSTR("RCA"), 0);
          break;
        case 2001:
          extRetValTxt_P(PSTR("MP1"), 0);
          break;
        case 2002:
          extRetValTxt_P(PSTR("MP2"), 0);
          break;
        default:
          memset(_strCache, 0, 22);
          strcpy_P(_strCache, PSTR("Ch "));
          itoa(constrain(globalConfigMem[actionPtr + 1], 1, 20), _strCache + 3, 10);
          extRetValTxt(_strCache, 0);
          break;
        }
        break;
      }
    }

    return retVal;
    break;
  case 44: // Chroma Settings
    break;
  case 45: // CCU Settings
    break;
  case 46: // PIP
    break;
  case 47: // DVE
           // This makes pushes to the encoder change which parameter to adjust:
    if ((pulses & B1) != _systemHWcActionCacheFlag[HWc][actIdx]) {
      _systemHWcActionCacheFlag[HWc][actIdx] = pulses & B1;
      _systemHWcActionCache[HWc][actIdx] = (_systemHWcActionCache[HWc][actIdx] + 1) % (globalConfigMem[actionPtr + 3] == 0 ? 2 : 3);
    }
    _systemHWcActionCache[HWc][actIdx] = _systemHWcActionCache[HWc][actIdx] % (globalConfigMem[actionPtr + 3] == 0 ? 2 : 3);
    if (globalConfigMem[actionPtr + 3] == 1 && globalConfigMem[actionPtr + 4] == 1)
      _systemHWcActionCache[HWc][actIdx] = 2; // Always only adjust w+H together in case of 100-200% zoom option for scaling.

    if (actDown) {
      if (value == 0x8000) { // Binary input = reset:
        switch (globalConfigMem[actionPtr + 3]) {
        case 0:                                                                                                         // Position
          AtemSwitcher[devIndex].setKeyDVEPositionX(globalConfigMem[actionPtr + 1], globalConfigMem[actionPtr + 2], 0); // center
          AtemSwitcher[devIndex].setKeyDVEPositionY(globalConfigMem[actionPtr + 1], globalConfigMem[actionPtr + 2], 0); // center
          break;
        case 1:                                                                                                                                                    // Size
          AtemSwitcher[devIndex].setKeyDVESizeX(globalConfigMem[actionPtr + 1], globalConfigMem[actionPtr + 2], globalConfigMem[actionPtr + 4] == 1 ? 1000 : 500); // 100% or 50%
          AtemSwitcher[devIndex].setKeyDVESizeY(globalConfigMem[actionPtr + 1], globalConfigMem[actionPtr + 2], globalConfigMem[actionPtr + 4] == 1 ? 1000 : 500); // 100% or 50%
          if (globalConfigMem[actionPtr + 4] == 1) {
            AtemSwitcher[devIndex].setKeyDVEPositionX(globalConfigMem[actionPtr + 1], globalConfigMem[actionPtr + 2], 0); // center
            AtemSwitcher[devIndex].setKeyDVEPositionY(globalConfigMem[actionPtr + 1], globalConfigMem[actionPtr + 2], 0); // center
          }
          break;
        }
      }
    }
    if (pulses & 0xFFFE) {
      uint16_t DVEsizeX = AtemSwitcher[devIndex].getKeyDVESizeX(globalConfigMem[actionPtr + 1], globalConfigMem[actionPtr + 2]);
      uint16_t DVEsizeY = AtemSwitcher[devIndex].getKeyDVESizeY(globalConfigMem[actionPtr + 1], globalConfigMem[actionPtr + 2]);
      int DVEposX = AtemSwitcher[devIndex].getKeyDVEPositionX(globalConfigMem[actionPtr + 1], globalConfigMem[actionPtr + 2]) / 10;
      int DVEposY = AtemSwitcher[devIndex].getKeyDVEPositionY(globalConfigMem[actionPtr + 1], globalConfigMem[actionPtr + 2]) / 10;
      int DVEposXlim = globalConfigMem[actionPtr + 4] == 1 ? abs((long)(DVEsizeX - 1000) * 16 / 10) : 6400;
      int DVEposYlim = globalConfigMem[actionPtr + 4] == 1 ? abs((long)(DVEsizeY - 1000) * 9 / 10) : 3600;

      switch (globalConfigMem[actionPtr + 3]) {
      case 0: // Position
        if ((_systemHWcActionCache[HWc][actIdx] + 1) & B1) {
          DVEposX = pulsesHelper(DVEposX, -DVEposXlim, DVEposXlim, false, pulses, 20, 20);
          AtemSwitcher[devIndex].setKeyDVEPositionX(globalConfigMem[actionPtr + 1], globalConfigMem[actionPtr + 2], DVEposX * 10);
        }
        if ((_systemHWcActionCache[HWc][actIdx] + 1) & B10) {
          DVEposY = pulsesHelper(DVEposY, -DVEposYlim, DVEposYlim, false, pulses, 20, 20);
          AtemSwitcher[devIndex].setKeyDVEPositionY(globalConfigMem[actionPtr + 1], globalConfigMem[actionPtr + 2], DVEposY * 10);
        }
        break;
      case 1: // Size
        if ((_systemHWcActionCache[HWc][actIdx] + 1) & B1) {
          DVEsizeX = pulsesHelper(DVEsizeX, globalConfigMem[actionPtr + 4] == 1 ? 1000 : 0, globalConfigMem[actionPtr + 4] == 1 ? 2000 : 4000, false, pulses, 30, 30);
          AtemSwitcher[devIndex].setKeyDVESizeX(globalConfigMem[actionPtr + 1], globalConfigMem[actionPtr + 2], DVEsizeX);
        }
        if ((_systemHWcActionCache[HWc][actIdx] + 1) & B10) {
          DVEsizeY = ((_systemHWcActionCache[HWc][actIdx] + 1) & B1) ? DVEsizeX : pulsesHelper(DVEsizeY, globalConfigMem[actionPtr + 4] == 1 ? 1000 : 0, globalConfigMem[actionPtr + 4] == 1 ? 2000 : 4000, false, pulses, 30, 30);
          AtemSwitcher[devIndex].setKeyDVESizeY(globalConfigMem[actionPtr + 1], globalConfigMem[actionPtr + 2], DVEsizeY);
        }
        if (globalConfigMem[actionPtr + 4] == 1) {
          DVEposXlim = abs((long)(DVEsizeX - 1000) * 16 / 10);
          DVEposYlim = abs((long)(DVEsizeY - 1000) * 9 / 10);
          if (DVEposX != constrain(DVEposX, -DVEposXlim, DVEposXlim)) {
            AtemSwitcher[devIndex].setKeyDVEPositionX(globalConfigMem[actionPtr + 1], globalConfigMem[actionPtr + 2], constrain(DVEposX, -DVEposXlim, DVEposXlim) * 10);
            Serial << "Update X Pos...\n";
          }
          if (DVEposY != constrain(DVEposY, -DVEposYlim, DVEposYlim)) {
            AtemSwitcher[devIndex].setKeyDVEPositionY(globalConfigMem[actionPtr + 1], globalConfigMem[actionPtr + 2], constrain(DVEposY, -DVEposYlim, DVEposYlim) * 10);
            Serial << "Update Y Pos...\n";
          }
        }

        break;
      }
    }

    if (extRetValIsWanted()) {
      switch (globalConfigMem[actionPtr + 3]) {
      case 0: // Position
        extRetVal(AtemSwitcher[devIndex].getKeyDVEPositionX(globalConfigMem[actionPtr + 1], globalConfigMem[actionPtr + 2]), 1);
        extRetVal2(AtemSwitcher[devIndex].getKeyDVEPositionY(globalConfigMem[actionPtr + 1], globalConfigMem[actionPtr + 2]), _systemHWcActionCache[HWc][actIdx] + 2);
        break;
      case 1: // Size
        extRetVal(AtemSwitcher[devIndex].getKeyDVESizeX(globalConfigMem[actionPtr + 1], globalConfigMem[actionPtr + 2]) / 10, 2);
        extRetVal2(AtemSwitcher[devIndex].getKeyDVESizeY(globalConfigMem[actionPtr + 1], globalConfigMem[actionPtr + 2]) / 10, _systemHWcActionCache[HWc][actIdx] + 2);
        break;
      }

      extRetValShortLabel(PSTR("ME"), (globalConfigMem[actionPtr + 1]) + 1);
      extRetValShortLabel(PSTR("Key"), (globalConfigMem[actionPtr + 2]) + 1);
      extRetValShortLabel(PSTR("DVE"));
      extRetValLongLabel(PSTR("ME"), (globalConfigMem[actionPtr + 1]) + 1);
      extRetValLongLabel(PSTR(" UpKey"), (globalConfigMem[actionPtr + 2]) + 1);
      extRetValLongLabel(PSTR(" DVE"));

      switch (globalConfigMem[actionPtr + 3]) {
      case 0: // Position
        extRetValTxt_P(PSTR("X:"), 0);
        extRetValTxt_P(PSTR("Y:"), 1);
        break;
      case 1: // Size
        extRetValTxt_P(PSTR("W:"), 0);
        extRetValTxt_P(PSTR("H:"), 1);
        break;
      }

      extRetValColor(B011010);
    }
    break;
  }

        */

  // Default:
  if (actDown) {
    _systemHWcActionCacheFlag[HWc][actIdx] = true;
  }
  if (actUp) {
    _systemHWcActionCacheFlag[HWc][actIdx] = false;
  }
  return _systemHWcActionCacheFlag[HWc][actIdx] ? (4 | 0x20) : 5;
}
