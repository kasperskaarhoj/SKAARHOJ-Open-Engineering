/**
 * Hardware setup, config mode and preset settings
 */
void HWcfgDisplay() { Serial << F("SK_MODEL: SK_MICROSMARTV\n"); }

/**
 * Hardware setup, config mode and preset settings
 */
uint8_t HWsetupL() {
  uint8_t retVal = 0;

  return retVal;
}

/**
 * Hardware test
 */
void HWtestL() {}

/**
 * Hardware runloop
 */
void HWrunLoop() {

  // SmartSwitch buttons:
  uint8_t b16Map2[] = {2, 1}; // These numbers refer to the drawing in the web interface
  HWrunLoop_SSWbuttons(b16Map2);
}

uint8_t HWnumOfAnalogComponents() { return 0; }

int16_t HWAnalogComponentValue(uint8_t num) {
	return 0;
}

uint16_t *HWMinCalibrationValues(uint8_t num) {
  static uint16_t values[3] = {0,0,0};
  return values;
}
