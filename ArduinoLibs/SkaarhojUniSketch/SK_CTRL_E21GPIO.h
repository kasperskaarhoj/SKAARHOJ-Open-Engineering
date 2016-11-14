/**
 * Hardware setup, config mode and preset settings
 */
void HWcfgDisplay() { Serial << F("SK_MODEL=SK_E21GPIO\n"); }

/**
 * Hardware setup, config mode and preset settings
 */
uint8_t HWsetupL() {

#if SK_E21GPIO_OPTION_GPIO2
  Serial << F("Option: GPIO2\n");
  Serial << F("Init GPIO 2 board\n");
  GPIOboard2.begin(1);
#else
  for (uint8_t a = 16; a < 16 + 16; a++) {
    if (a < SK_HWCCOUNT) // Just making sure...
      HWdis[a] = 1;	// Removes from web interface
  }
#endif

  return 0;
}

/**
 * Hardware test
 */
void HWtestL() {
#if SK_E21GPIO_OPTION_GPIO2
  GPIOboard2.setOutputAll(GPIOboard2.inputIsActiveAll() ^ ((millis() + 500) >> 12));
#endif
}

/**
 * Hardware runloop
 */
void HWrunLoop() {

#if SK_HWEN_GPIO
  static bool gpioStates[] = {false, false, false, false, false, false, false, false};
  HWrunLoop_GPIO(GPIOboard, 1, gpioStates);
#endif

#if SK_E21GPIO_OPTION_GPIO2
  static bool gpioStates2[] = {false, false, false, false, false, false, false, false};
  HWrunLoop_GPIO(GPIOboard2, 17, gpioStates2);
#endif
}

uint8_t HWnumOfAnalogComponents() { return 0; }

int16_t HWAnalogComponentValue(uint8_t num) { return 0; }

void HWanalogComponentName(uint8_t num, char* buffer, uint8_t len) {
  memset(buffer, 0, len);
}

uint16_t *HWMinCalibrationValues(uint8_t num) {
  static uint16_t values[3] = {0, 0, 0};
  return values;
}
