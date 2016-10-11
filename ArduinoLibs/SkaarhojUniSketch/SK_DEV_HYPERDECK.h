// Button return colors:
// 0 = off
// 5 = dimmed
// 1,2,3,4 = full (yellow), red, green, yellow
// Bit 4 (16) = blink flag, filter out for KP01 buttons.
uint16_t evaluateAction_HYPERDECK(const uint8_t devIndex, const uint16_t actionPtr, const uint8_t HWc, const uint8_t actIdx, const bool actDown = false, const bool actUp = false, const uint8_t pulses = 0, const uint16_t value = 0) {
  uint16_t retVal = 0;
  if (actDown || actUp) {
    Serial << "HYPERDECK action " << globalConfigMem[actionPtr] << "\n";
  }
  int16_t playspeeds[] = {100, 25, 50, 200, 400, 800, 1600, -25, -50, -100, -200, -400, -800, -1600};
  int16_t speed = 0;

  switch (globalConfigMem[actionPtr]) {
  case 0:
    speed = playspeeds[globalConfigMem[actionPtr + 1]];
    if (actDown) {
      _systemHWcActionCache[HWc][actIdx] = HyperDeck[devIndex].getPlaySpeed();
      if (globalConfigMem[actionPtr + 2] == 1 && HyperDeck[devIndex].isPlaying()) {
        HyperDeck[devIndex].stop();
      } else {
        HyperDeck[devIndex].playWithSpeed(speed);
      }
    }
    if (actUp && globalConfigMem[actionPtr + 2] == 2) { // globalConfigMem[actionPtr + 2] == "Hold Down"
      HyperDeck[devIndex].playWithSpeed(_systemHWcActionCache[HWc][actIdx]);
    }
    return HyperDeck[devIndex].getPlaySpeed() == speed ? 3 : 5;
    break;
  case 1:
    if (actDown) {
      if (HyperDeck[devIndex].isStopped() && globalConfigMem[actionPtr + 1] == 1) {
        HyperDeck[devIndex].previewEnable(!HyperDeck[devIndex].isInPreview());
      } else if (HyperDeck[devIndex].isStopped() && globalConfigMem[actionPtr + 1] == 2) {
        HyperDeck[devIndex].gotoClipStart();
      } else {
        HyperDeck[devIndex].stop();
      }
    }
    return HyperDeck[devIndex].isStopped() ? 4 : 5;
    break;
  case 2: // Record
    if (actDown) {
      if (globalConfigMem[actionPtr + 1] == 1 && HyperDeck[devIndex].isRecording()) {
        HyperDeck[devIndex].stop();
      } else {
        HyperDeck[devIndex].record();
      }
    }
    if (actUp && globalConfigMem[actionPtr + 1] == 2) { // globalConfigMem[actionPtr + 2] == "Hold Down"
      HyperDeck[devIndex].stop();
    }
    return HyperDeck[devIndex].isRecording() ? 2 : 5;
    break;
  case 3:
    if (actDown) {
      HyperDeck[devIndex].previewEnable(true);
    }
    return HyperDeck[devIndex].isInPreview() ? 4 : 5;
    break;
  }
  return retVal;
}
