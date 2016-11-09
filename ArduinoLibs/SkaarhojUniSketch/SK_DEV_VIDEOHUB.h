#define VIDEOHUB_HOLDGROUPSIZE 5
uint16_t VideoHub_holdGroup[2][VIDEOHUB_HOLDGROUPSIZE];
uint8_t VideoHub_holdGroupHWc[2][VIDEOHUB_HOLDGROUPSIZE];
uint8_t VideoHub_holdGroupPtr[2] = {0, 0};


/**
 * Index to video hub source (aligned with selector box in web interface)
 */
uint16_t VideoHub_idxToNumber(uint8_t idx) {
  if (idx <= 72) {
    return idx;
  } else if (idx > 72 && idx <= 72+4) {
    return _systemMem[idx - 72 - 1];
  } else
    return 0;
}

/**
 * Pushes source to hold group
 */
bool VideoHub_pushToHoldGroup(uint8_t hgIdx, uint16_t src, uint8_t HWc) {
  if (VideoHub_holdGroupPtr[hgIdx] < VIDEOHUB_HOLDGROUPSIZE) {
    VideoHub_holdGroup[hgIdx][VideoHub_holdGroupPtr[hgIdx]] = src;
    VideoHub_holdGroupHWc[hgIdx][VideoHub_holdGroupPtr[hgIdx]] = HWc;
    // Serial << "Pushed " << VideoHub_holdGroup[hgIdx][VideoHub_holdGroupPtr[hgIdx]] << " for " << HWc << "\n";
    VideoHub_holdGroupPtr[hgIdx]++;
    /*
for (uint8_t a = 0; a < VideoHub_holdGroupPtr[hgIdx]; a++) {
  Serial << "Stack " << a << ": Src " << VideoHub_holdGroup[hgIdx][a] << " for HWc " << VideoHub_holdGroupHWc[hgIdx][a] << "\n";
}*/
    return true;
  } else
    return false;
}

/**
 * Pulls source from hold group
 */
uint16_t VideoHub_pullFromHoldGroup(uint8_t hgIdx, uint8_t HWc) {
  uint8_t ai = 0;
  uint8_t origPtr = VideoHub_holdGroupPtr[hgIdx] < VIDEOHUB_HOLDGROUPSIZE ? VideoHub_holdGroupPtr[hgIdx] : VIDEOHUB_HOLDGROUPSIZE;
  uint16_t retVal = 0x8000;
  for (uint8_t a = 0; a < origPtr; a++) {
    if (VideoHub_holdGroupHWc[hgIdx][a] != HWc) {
      VideoHub_holdGroupHWc[hgIdx][ai] = VideoHub_holdGroupHWc[hgIdx][a];
      VideoHub_holdGroup[hgIdx][ai] = VideoHub_holdGroup[hgIdx][a];
      ai++;
      retVal = 0x8000;
    } else {
      if (a + 1 < origPtr) {
        VideoHub_holdGroup[hgIdx][a + 1] = VideoHub_holdGroup[hgIdx][a];
        //        Serial << "Forward...\n";
      }
      VideoHub_holdGroupPtr[hgIdx]--;
      retVal = VideoHub_holdGroup[hgIdx][a];
    }
  }
  /*
  for (uint8_t a = 0; a < VideoHub_holdGroupPtr[hgIdx]; a++) {
    Serial << "Stack " << a << ": Src " << VideoHub_holdGroup[hgIdx][a] << " for HWc " << VideoHub_holdGroupHWc[hgIdx][a] << "\n";
  }
  Serial << "Returns " << retVal << " for " << HWc << "\n";
  */
  return retVal;
}



// Button return colors:
// 0 = off
// 5 = dimmed
// 1,2,3,4 = full (yellow), red, green, yellow
// Bit 4 (16) = blink flag, filter out for KP01 buttons.
uint16_t evaluateAction_VIDEOHUB(const uint8_t devIndex, const uint16_t actionPtr, const uint8_t HWc, const uint8_t actIdx, bool actDown, bool actUp, int16_t pulses, int16_t value) {
  uint16_t retVal = 0;
  if (actDown || actUp) {
    Serial << "VIDEO HUB action " << globalConfigMem[actionPtr] << "\n";
  }
  switch (globalConfigMem[actionPtr]) {
  case 0:
    if (actDown && value == BINARY_EVENT) {
      if (globalConfigMem[actionPtr + 3] == 5) { // Source cycle by button push
        pulses = 2;
        _systemHWcActionCacheFlag[HWc][actIdx] = true;
      } else if (globalConfigMem[actionPtr + 3] != 2 || !_systemHWcActionCacheFlag[HWc][actIdx]) {
        _systemHWcActionCacheFlag[HWc][actIdx] = true; // Used for toggle feature
        _systemHWcActionCache[HWc][actIdx] = VideoHub[devIndex].getRoute(VideoHub_idxToNumber(globalConfigMem[actionPtr + 2]));
        if (globalConfigMem[actionPtr + 3] == 3 || globalConfigMem[actionPtr + 3] == 4) {
          VideoHub_pushToHoldGroup(globalConfigMem[actionPtr + 3] - 3, VideoHub[devIndex].getRoute(VideoHub_idxToNumber(globalConfigMem[actionPtr + 2])), HWc);
        }
        VideoHub[devIndex].routeInputToOutput(VideoHub_idxToNumber(globalConfigMem[actionPtr + 1]), VideoHub_idxToNumber(globalConfigMem[actionPtr + 2]),true);	// Waiting for confirmation is important for really solid hold group functionality.
      } else {
		VideoHub[devIndex].routeInputToOutput(_systemHWcActionCache[HWc][actIdx], VideoHub_idxToNumber(globalConfigMem[actionPtr + 2]),true);
        _systemHWcActionCacheFlag[HWc][actIdx] = false;
      }
    }
    if (actUp && globalConfigMem[actionPtr + 3] == 1) { // "Hold Down"
  	  VideoHub[devIndex].routeInputToOutput(_systemHWcActionCache[HWc][actIdx], VideoHub_idxToNumber(globalConfigMem[actionPtr + 2]),true);
    }
    if (actUp && (globalConfigMem[actionPtr + 3] == 3 || globalConfigMem[actionPtr + 3] == 4)) { // "Hold Groups"
      uint16_t fallBackSrc = VideoHub_pullFromHoldGroup(globalConfigMem[actionPtr + 3] - 3, HWc);
      if (fallBackSrc != 0x8000)
        VideoHub[devIndex].routeInputToOutput(fallBackSrc, VideoHub_idxToNumber(globalConfigMem[actionPtr + 2]),true);
    }
    if (actUp && globalConfigMem[actionPtr + 3] == 5) { // "Cycle"
      _systemHWcActionCacheFlag[HWc][actIdx] = false;
    }

    if (pulses & 0xFFFE) {
	  VideoHub[devIndex].routeInputToOutput(pulsesHelper(VideoHub[devIndex].getRoute(VideoHub_idxToNumber(globalConfigMem[actionPtr + 2])), 1, globalConfigMem[actionPtr + 3] == 5 ? globalConfigMem[actionPtr + 1] : 72, true, pulses, 1, 1), VideoHub_idxToNumber(globalConfigMem[actionPtr + 2]));
    }

    retVal = globalConfigMem[actionPtr + 3] == 5 ? (_systemHWcActionCacheFlag[HWc][actIdx] ? (4 | 0x20) : 5) : (VideoHub[devIndex].getRoute(VideoHub_idxToNumber(globalConfigMem[actionPtr + 2])) == VideoHub_idxToNumber(globalConfigMem[actionPtr + 1]) ? (4 | 0x20) : 5);

    break;
  }
  return retVal;
}