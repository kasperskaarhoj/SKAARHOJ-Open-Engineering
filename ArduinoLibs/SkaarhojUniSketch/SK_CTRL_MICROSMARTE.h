/**
 * Hardware setup, config mode and preset settings
 */
void HWcfgDisplay() { Serial << F("SK_MODEL: SK_MICROSMARTE\n"); }

/**
 * Hardware setup, config mode and preset settings
 */
uint8_t HWsetupL() { return 0; }

/**
 * Hardware test
 */
void HWtestL() {
#if SK_HWEN_SSWMENU
  SSWmenuEnc.runLoop();
#endif
}

/**
 * Hardware runloop
 */
void HWrunLoop() {
  uint16_t bDown;

  SSWmenuEnc.runLoop();

#if (SK_HWEN_SSWMENU)
  lDelay(10); // This makes sure we spend MUCH more time outside this section where we check for the button press of button 5 - that check is pretty bad for the encoder interrupts, so reducing it's relative time in the runloop is important.
  SSWmenuEnc.runLoop();
  lDelay(10); // This makes sure we spend MUCH more time outside this section where we check for the button press of button 5 - that check is pretty bad for the encoder interrupts, so reducing it's relative time in the runloop is important.
  HWrunLoop_SSWMenu(1);
#endif
}
