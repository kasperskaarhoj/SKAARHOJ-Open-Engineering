/**
 * Hardware setup, config mode and preset settings
 */
void HWcfgDisplay() { Serial << F("SK_MODEL: SK_MICROMONITOR\n"); }

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
#if (SK_HWEN_STDOLEDDISPLAY)
  static uint16_t infoDisplay_prevHash[3];
  static bool infoDisplay_written;
  HWrunLoop_128x32OLED(infoDisplay, 1, infoDisplay_prevHash, infoDisplay_written);
#endif
}
