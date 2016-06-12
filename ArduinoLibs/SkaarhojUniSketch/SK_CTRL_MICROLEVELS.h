/**
 * Hardware setup, config mode and preset settings
 */
void HWcfgDisplay() { Serial << F("SK_MODEL: SK_MICROLEVELS\n"); }

/**
 * Hardware setup, config mode and preset settings
 */
uint8_t HWsetupL() {

  audio_a.begin(0);
  audio_a.setIsMasterBoard();

  return 0;
}

/**
 * Hardware test
 */
void HWtestL() {}

/**
 * Hardware runloop
 */
void HWrunLoop() {

  uint16_t retVal = actionDispatch(1);
  audio_a.VUmeterDB((int)highByte(retVal) - 60, (int)lowByte(retVal) - 60);

  // Buttons:
  uint16_t bUp = audio_a.buttonUpAll();
  uint16_t bDown = audio_a.buttonDownAll();
  for (int a = 0; a < 2; a++) {
    uint16_t color = actionDispatch(a + 2, bDown & (B1 << a), bUp & (B1 << a));
    audio_a.setButtonLight(a + 1, (color & 0xF) > 0 ? ((!(color & 0x10) || (millis() & 512) > 0) && ((color & 0xF) != 5) ? 1 : 0) : 0);
  }
}
