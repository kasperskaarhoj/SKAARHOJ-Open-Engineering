/**
 * Hardware setup, config mode and preset settings
 */
void HWcfgDisplay() { Serial << F("SK_MODEL: SK_MICROKP01\n"); }

/**
 * Hardware setup, config mode and preset settings
 */
uint8_t HWsetupL() {
  Serial << F("Init BI8 boards\n");
  buttons.begin(0, false);
  buttons.setDefaultColor(0); // Off by default
  buttons.setColorBalanceRGB(4, 100, 100, 100);
  buttons.setColorBalanceRGB(5, 10, 10, 10);
  if (getConfigMode()) {
    Serial << F("Test: BI8 Boards\n");
    buttons.testSequence(10);
  } else
    delay(500);
  buttons.setButtonColorsToDefault();
  statusLED(QUICKBLANK);

  // Look for a button press / sustained button press to bring the device into config/config default modes:
  uint8_t retVal = 0;
  unsigned long timer = millis();
  if (buttons.buttonIsPressedAll() > 0) {
    retVal = 1;
    statusLED(LED_BLUE);
    while (buttons.buttonIsPressedAll() > 0) {
      if (sTools.hasTimedOut(timer, 2000)) {
        retVal = 2;
        statusLED(LED_WHITE);
      }
    }
  } else {
    // Preset
    if (getNumberOfPresets() > 1) {
      uint8_t presetNum = EEPROM.read(EEPROM_PRESET_START + 1);

      while (!sTools.hasTimedOut(timer, 2000) || buttons.buttonIsPressedAll() > 0) {
        uint8_t b16Map[] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2}; // These numbers refer to the drawing in the web interface
        for (uint8_t a = 0; a < 10; a++) {
          uint8_t color = (b16Map[a] > 0 && b16Map[a] <= getNumberOfPresets()) ? (b16Map[a] == presetNum ? 4 : 5) : 0;
          buttons.setButtonColor(a + 1, color);
          if (color > 0 && buttons.buttonIsHeldFor(a + 1, 2000)) {
            setCurrentPreset(b16Map[a]);
            buttons.setButtonColor(a + 1, 2);
            while (buttons.buttonIsPressedAll() > 0)
              ;
          }
        }
      }
      buttons.setButtonColorsToDefault();
    }
  }
  return retVal;
}

/**
 * Hardware test
 */
void HWtestL() { buttons.testProgramme(0x3FF); }

/**
 * Hardware runloop
 */
void HWrunLoop() {

  // BI8 buttons:
  uint8_t b16Map[] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2}; // These numbers refer to the drawing in the web interface
  HWrunLoop_BI8(buttons, b16Map, sizeof(b16Map));
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

