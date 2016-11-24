/**
 * Hardware setup, config mode and preset settings
 */
void HWcfgDisplay() { Serial << F("SK_MODEL=SK_TALLY\n"); }

/**
 * Hardware setup, config mode and preset settings
 */
uint8_t HWsetupL() {

  Serial << F("Init Tally GPIO boards\n");
  GPIOboard.begin(0, 1); // 16 outputs, address 0
  if (getConfigMode()) {
    Serial << F("Test: GPIO boards\n");
    for (int16_t i = 1; i <= 16; i++) {
      GPIOboard.setOutput(i, HIGH);
      delay(200);
      GPIOboard.setOutput(i, LOW);
    }
  } else
    delay(500);
  statusLED(QUICKBLANK);

  return 0;
}

/**
 * Hardware test
 */
void HWtestL() {

  for (int16_t i = 1; i <= 16; i++) {
    GPIOboard.setOutput(i, ((millis() >> (i + 9)) % 16) + 1 == i);
  }
}

/**
 * Hardware runloop
 */
void HWrunLoop() {

  // GPIO Outputs
  static bool gpioStates[16] = {false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false};
  for (uint8_t a = 0; a < 16; a++) {
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

