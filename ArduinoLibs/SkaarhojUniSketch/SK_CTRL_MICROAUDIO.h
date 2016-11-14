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
  for (int16_t a = 0; a < 2; a++) {
    uint16_t color = actionDispatch(a + 2, bDown & (B1 << a), bUp & (B1 << a));
    audio_a.setButtonLight(a + 1, (color & 0xF) > 0 ? ((!(color & 0x10) || (millis() & 512) > 0) && ((color & 0xF) != 5) ? 1 : 0) : 0);
  }
}

uint8_t HWnumOfAnalogComponents() { return 1; }

int16_t HWAnalogComponentValue(uint8_t num) {
  switch(num) {
    case 1:
      return analogRead(A0);
  }
}

void HWanalogComponentName(uint8_t num, char* buffer, uint8_t len) {
  char *name;
  switch(num) {
    case 1:
      name = "Audio pot";
      break;
  }
  strncpy(buffer, name, len);
}

uint16_t *HWMinCalibrationValues(uint8_t num) {
  static uint16_t values[3] = {35,65,15};
  return values;
}

