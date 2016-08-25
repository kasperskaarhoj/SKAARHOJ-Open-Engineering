/**
 * Hardware setup, config mode and preset settings
 */
void HWcfgDisplay() { Serial << F("SK_MODEL=SK_E21GPIO\n"); }

/**
 * Hardware setup, config mode and preset settings
 */
uint8_t HWsetupL() {

  Serial << F("Init GPIO 2 board\n");
  GPIOboard2.begin(1);

  return 0;
}

/**
 * Hardware test
 */
void HWtestL() { GPIOboard2.setOutputAll(GPIOboard2.inputIsActiveAll() ^ ((millis() + 500) >> 12)); }

/**
 * Hardware runloop
 */
void HWrunLoop() {

#if SK_HWEN_GPIO
  static bool gpioStates[] = {false, false, false, false, false, false, false, false};
  HWrunLoop_GPIO(GPIOboard, 1, gpioStates);
#endif

  static bool gpioStates2[] = {false, false, false, false, false, false, false, false};
  HWrunLoop_GPIO(GPIOboard2, 17, gpioStates2);
}
