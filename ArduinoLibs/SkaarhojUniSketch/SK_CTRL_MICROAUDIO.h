/**
 * Hardware setup, config mode and preset settings
 */
void HWcfgDisplay() { Serial << F("SK_MODEL: SK_MICROAUDIO\n"); }

/**
 * Hardware setup, config mode and preset settings
 */
uint8_t HWsetupL() {

  audio_a.begin(0);
  audioPot_a1.uniDirectionalSlider_init(10, 35, 35, 0, 0);
  audioPot_a1.uniDirectionalSlider_disableUnidirectionality(true);

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

  // Pot
  bool hasMoved = audioPot_a1.uniDirectionalSlider_hasMoved();
  actionDispatch(1, hasMoved, false, 0, mapPotHelper(audioPot_a1.uniDirectionalSlider_position()));

  // Peak:
  uint16_t retVal = actionDispatch(4);
  audio_a.setChannelIndicatorLight(2, retVal & 0x20 ? constrain((retVal & 0xF) - 1, 0, 3) : 0);

  // Buttons:
  uint16_t bUp = audio_a.buttonUpAll();
  uint16_t bDown = audio_a.buttonDownAll();
  for (int a = 0; a < 2; a++) {
    uint16_t color = actionDispatch(a + 2, bDown & (B1 << a), bUp & (B1 << a));
    audio_a.setButtonLight(a + 1, (color & 0xF) > 0 ? ((!(color & 0x10) || (millis() & 512) > 0) && ((color & 0xF) != 5) ? 1 : 0) : 0);
  }
}
