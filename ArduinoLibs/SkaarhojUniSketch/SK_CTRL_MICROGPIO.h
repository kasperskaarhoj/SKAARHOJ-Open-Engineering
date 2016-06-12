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
