
uint16_t AtemSwitcher_vSrcMap[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 1000, 2001, 2002, 3010, 3020, 6000, 7001, 7002, 8001, 8002, 8003, 8004, 8005, 8006, 10010, 10011, 10020, 10021}; //, "MV1/1", "MV1/2", "MV1/3", "MV1/4", "MV1/5", "MV1/6", "MV1/7", "MV1/8", "MV2/1", "MV2/2", "MV2/3", "MV2/4", "MV2/5", "MV2/6", "MV2/7", "MV2/8","Mem A","Mem B","Mem C","Mem D"}



// Button return colors: 
// 0 = off
// 5 = dimmed
// 1,2,3,4 = full (yellow), red, green, yellow
uint16_t evaluateAction_ATEM(const uint8_t devIndex, const uint16_t actionPtr, const uint8_t HWc, const uint8_t actIdx, const bool actDown = false, const bool actUp = false, const uint8_t pulses = 0, const uint16_t value = 0)    {
  uint16_t retVal=0;
  if (actDown || actUp) {
    Serial << "ATEM action " << globalConfigMem[actionPtr] << ", " << actDown << ", " << actUp << "\n";
  }
  switch (globalConfigMem[actionPtr])   {
    case 0: // Program Source
      if (actDown)   {
        _systemHWcActionCache[HWc][actIdx] = AtemSwitcher[devIndex].getProgramInputVideoSource(globalConfigMem[actionPtr + 1]);
        AtemSwitcher[devIndex].setProgramInputVideoSource(globalConfigMem[actionPtr + 1], AtemSwitcher_vSrcMap[globalConfigMem[actionPtr + 2]]);
      }
      if (actUp && globalConfigMem[actionPtr + 3])    { // globalConfigMem[actionPtr + 3] == "Hold Down"
		 AtemSwitcher[devIndex].setProgramInputVideoSource(globalConfigMem[actionPtr + 1], _systemHWcActionCache[HWc][actIdx]);
      }
	  return AtemSwitcher[devIndex].getProgramInputVideoSource(globalConfigMem[actionPtr + 1]) == AtemSwitcher_vSrcMap[globalConfigMem[actionPtr + 2]] ? 2 : 5;
      break;
    case 1: // Preview Source
      if (actDown)   {
		 AtemSwitcher[devIndex].setPreviewInputVideoSource(globalConfigMem[actionPtr + 1], AtemSwitcher_vSrcMap[globalConfigMem[actionPtr + 2]]);
      }
	  return AtemSwitcher[devIndex].getPreviewInputVideoSource(globalConfigMem[actionPtr + 1]) == AtemSwitcher_vSrcMap[globalConfigMem[actionPtr + 2]] ? 3 : 5;
      break;
    case 2: // Program/Preview Source
      if (actDown)   {
        _systemHWcActionCacheFlag[HWc][actIdx] = true;
        _systemHWcActionCache[HWc][actIdx] = millis();
        AtemSwitcher[devIndex].setPreviewInputVideoSource(globalConfigMem[actionPtr + 1], AtemSwitcher_vSrcMap[globalConfigMem[actionPtr + 2]]);
      }
      if (_systemHWcActionCacheFlag[HWc][actIdx])   {
        uint16_t t = millis();
        if (t - _systemHWcActionCache[HWc][actIdx] > 2000)    {
		    AtemSwitcher[devIndex].setProgramInputVideoSource(globalConfigMem[actionPtr + 1], AtemSwitcher_vSrcMap[globalConfigMem[actionPtr + 2]]);
          _systemHWcActionCacheFlag[HWc][actIdx] = false;
        }
      }
      if (actUp)    {
        _systemHWcActionCacheFlag[HWc][actIdx] = false;
      }
	  retVal = AtemSwitcher[devIndex].getProgramInputVideoSource(globalConfigMem[actionPtr + 1]) == AtemSwitcher_vSrcMap[globalConfigMem[actionPtr + 2]] ? 2 : (AtemSwitcher[devIndex].getPreviewInputVideoSource(globalConfigMem[actionPtr + 1]) == AtemSwitcher_vSrcMap[globalConfigMem[actionPtr + 2]] ? 3 : 5);
	  return retVal==3 ? (retVal | B10000) : retVal;	// Bit 4 is a "mono-color blink flag" so a mono color button can indicate this special state.
      break;
    case 3: // AUX
      if (actDown)   {
        _systemHWcActionCache[HWc][actIdx] = AtemSwitcher[devIndex].getAuxSourceInput(globalConfigMem[actionPtr + 1]);
        AtemSwitcher[devIndex].setAuxSourceInput(globalConfigMem[actionPtr + 1], AtemSwitcher_vSrcMap[globalConfigMem[actionPtr + 2]]);
      }
      if (actUp && globalConfigMem[actionPtr + 3] == 1)    { // "Hold Down"
        AtemSwitcher[devIndex].setAuxSourceInput(globalConfigMem[actionPtr + 1], _systemHWcActionCache[HWc][actIdx]);
      } // TODO: Hold group
      break;
    case 4: // USK settings
      if (globalConfigMem[actionPtr + 3] != 4)  {
        if (actDown)   {
          bool state = false;
          switch (globalConfigMem[actionPtr + 3])       {
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
        if (actUp && globalConfigMem[actionPtr + 3] == 3)   {
          AtemSwitcher[devIndex].setKeyerOnAirEnabled(globalConfigMem[actionPtr + 1], globalConfigMem[actionPtr + 2], false);
        }
      } else {
        // TODO: perform auto on USK
      }
	  return (globalConfigMem[actionPtr + 3]==2 ^ AtemSwitcher[devIndex].getKeyerOnAirEnabled(globalConfigMem[actionPtr + 1], globalConfigMem[actionPtr + 2])) ? 4 : 5;
      break;
    case 5: // Upstream Keyer Fill
      if (actDown)   {
        AtemSwitcher[devIndex].setKeyerFillSource(globalConfigMem[actionPtr + 1], globalConfigMem[actionPtr + 2], AtemSwitcher_vSrcMap[globalConfigMem[actionPtr + 3]]);
      }
      break;
    case 6: // Upstream Keyer Key
      if (actDown)   {
        AtemSwitcher[devIndex].setKeyerKeySource(globalConfigMem[actionPtr + 1], globalConfigMem[actionPtr + 2], AtemSwitcher_vSrcMap[globalConfigMem[actionPtr + 3]]);
      }
      break;
    case 7: // DSK settings
      if (globalConfigMem[actionPtr + 3] != 4)  {
        if (actDown)   {
          bool state = false;
          switch (globalConfigMem[actionPtr + 3])       {
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
        if (actUp && globalConfigMem[actionPtr + 3] == 3)   {
          AtemSwitcher[devIndex].setDownstreamKeyerOnAir(globalConfigMem[actionPtr + 1], false);
        }
      } else {
        AtemSwitcher[devIndex].performDownstreamKeyerAutoKeyer(globalConfigMem[actionPtr + 1]);
      }
      break;
    case 8: // Downstream Keyer Fill
      if (actDown)   {
        AtemSwitcher[devIndex].setDownstreamKeyerFillSource(globalConfigMem[actionPtr + 1], AtemSwitcher_vSrcMap[globalConfigMem[actionPtr + 2]]);
      }
      break;
	case 9: // Downstream Keyer Key
      if (actDown)   {
        AtemSwitcher[devIndex].setDownstreamKeyerKeySource(globalConfigMem[actionPtr + 1], AtemSwitcher_vSrcMap[globalConfigMem[actionPtr + 2]]);
      }
      break;
	case 10: // MP Still
	  if (actDown)   {
	    AtemSwitcher[devIndex].setMediaPlayerSourceStillIndex(globalConfigMem[actionPtr + 1], globalConfigMem[actionPtr + 2]);
	  }
	  return AtemSwitcher[devIndex].getMediaPlayerSourceStillIndex(globalConfigMem[actionPtr + 1]) == globalConfigMem[actionPtr + 2] ? 4 : 5;
	  break;

    case 14: // CUT
      if (actDown)   {
		_systemHWcActionCacheFlag[HWc][actIdx] = true;
        AtemSwitcher[devIndex].performCutME(globalConfigMem[actionPtr + 1]);
      }
      if (actUp)   {
		  _systemHWcActionCacheFlag[HWc][actIdx] = false;
	  }
	  return _systemHWcActionCacheFlag[HWc][actIdx] ? 4 : 5;
      break;
    case 15: // AUTO
      if (actDown)   {
        AtemSwitcher[devIndex].performAutoME(globalConfigMem[actionPtr + 1]);
      }
	  return AtemSwitcher[devIndex].getTransitionInTransition(globalConfigMem[actionPtr + 1]) ? 2 : 5;
      break;
    case 16: // FTB
      if (actDown)   {
        AtemSwitcher[devIndex].performFadeToBlackME(globalConfigMem[actionPtr + 1]);
      }
	  return AtemSwitcher[devIndex].getFadeToBlackStateFullyBlack(globalConfigMem[actionPtr + 1]) ? ((millis()&512)>0?2:0) : (AtemSwitcher[devIndex].getFadeToBlackStateInTransition(globalConfigMem[actionPtr + 1]) ? 2 : 5);

      break;
    case 18: // Transition Pos
	    if (actDown)   {  // Use actDown as "has moved"
	      AtemSwitcher[devIndex].setTransitionPosition(globalConfigMem[actionPtr + 1], value*10);
    	    if (actUp)   {  // Use actUp as "at end"
    	      AtemSwitcher[devIndex].setTransitionPosition(globalConfigMem[actionPtr + 1], 0);
    	    }
	    }
      break;
  }
  return retVal;
}