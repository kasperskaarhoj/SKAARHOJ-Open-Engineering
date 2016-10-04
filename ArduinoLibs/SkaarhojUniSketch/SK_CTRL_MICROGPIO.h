/**
 * Hardware setup, config mode and preset settings
 */
void HWcfgDisplay() { Serial << F("SK_MODEL=SK_MICROGPIO\n"); }

/**
 * Hardware setup, config mode and preset settings
 */
uint8_t HWsetupL() { return 0; }

/**
 * Hardware test
 */
void HWtestL() {}

/**
 * Hardware runloop
 */
void HWrunLoop() {

#if SK_HWEN_GPIO
  static bool gpioStates[] = {false, false, false, false, false, false, false, false};
  HWrunLoop_GPIO(GPIOboard, 1, gpioStates);
#endif
}

uint8_t HWnumOfAnalogComponents() { return 0; }

int16_t HWAnalogComponentValue(uint8_t num) {
	return 0;
}

uint16_t *HWMinCalibrationValues(uint8_t num) {
  static uint16_t values[3] = {0,0,0};
  return values;
}
