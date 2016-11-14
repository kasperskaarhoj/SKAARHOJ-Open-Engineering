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
  for (int16_t a = 0; a < 2; a++) {
    uint16_t color = actionDispatch(a + 2, bDown & (B1 << a), bUp & (B1 << a));
    audio_a.setButtonLight(a + 1, (color & 0xF) > 0 ? ((!(color & 0x10) || (millis() & 512) > 0) && ((color & 0xF) != 5) ? 1 : 0) : 0);
  }
}

uint8_t HWnumOfAnalogComponents() { return 0; }

int16_t HWAnalogComponentValue(uint8_t num) {
  return 0;
}

void HWanalogComponentName(uint8_t num, char* buffer, uint8_t len) {
  memset(buffer, 0, len);
}

uint16_t *HWMinCalibrationValues(uint8_t num) {
  static uint16_t values[3] = {0,0,0};
  return values;
}

