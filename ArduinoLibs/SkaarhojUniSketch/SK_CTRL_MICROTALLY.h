/**
 * Hardware setup, config mode and preset settings
 */
void HWcfgDisplay() { Serial << F("SK_MODEL=SK_MICROTALLY\n"); }

/**
 * Hardware setup, config mode and preset settings
 */
uint8_t HWsetupL() {

  Serial << F("Init GPIO board\n");
  GPIOboard.begin(0);

  return 0;
}

/**
 * Hardware test
 */
void HWtestL() { GPIOboard.setOutputAll(GPIOboard.inputIsActiveAll() ^ (millis() >> 9)); }

/**
 * Hardware runloop
 */
void HWrunLoop() {

  // GPIO Outputs
  static bool gpioStates[] = {false, false, false, false, false, false};
  for (uint8_t a = 0; a < 6; a++) {
    extRetValPrefersLabel(1 + a);
    uint8_t state = actionDispatch(1 + a);
    bool stateB = state & 0x20; // Output bit

    if (stateB != gpioStates[a]) {
      gpioStates[a] = stateB;
      GPIOboard.setOutput(a + 1, stateB);
      if (debugMode)
        Serial << F("Write GPIO ") << a + 1 << F("\n");
    }
  }
}

uint8_t HWnumOfAnalogComponents() { return 0; }

int16_t HWAnalogComponentValue(uint8_t num) {
  return 0;
}

void HWanalogComponentName(uint8_t num, char* buffer, uint8_t len) {
  memset(buffer, 0, len);
}

uint16_t *HWMinCalibrationValues(uint8_t num) {
  static uint16_t values[3] = {0,0,0};
  return values;
}
