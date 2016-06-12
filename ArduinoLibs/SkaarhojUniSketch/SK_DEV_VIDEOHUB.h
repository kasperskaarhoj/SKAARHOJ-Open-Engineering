// Button return colors:
// 0 = off
// 5 = dimmed
// 1,2,3,4 = full (yellow), red, green, yellow
// Bit 4 (16) = blink flag, filter out for KP01 buttons.
uint16_t evaluateAction_VIDEOHUB(const uint8_t devIndex, const uint16_t actionPtr, const uint8_t HWc, const uint8_t actIdx, const bool actDown = false, const bool actUp = false, const uint8_t pulses = 0, const uint16_t value = 0) {
  uint16_t retVal = 0;
  if (actDown || actUp) {
    Serial << "VIDEO HUB action " << globalConfigMem[actionPtr] << "\n";
  }
  switch (globalConfigMem[actionPtr]) {
  case 0:
    if (actDown) {
      uint8_t input = globalConfigMem[actionPtr + 1];
      uint8_t output = globalConfigMem[actionPtr + 2];
      VideoHub[devIndex].routeInputToOutput(input, output);
    }
    break;
  }
  return retVal;
}