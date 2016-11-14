/**
 * Hardware setup, config mode and preset settings
 */
void HWcfgDisplay() { Serial << F("SK_MODEL=SK_E21TVS\n"); }

/**
 * Hardware setup, config mode and preset settings
 */
uint8_t HWsetupL() {
  Serial << F("Init BI16 board\n");
  buttons.begin(1);
  buttons.setDefaultColor(0); // Off by default
  buttons.setButtonColorsToDefault();
  if (getConfigMode()) {
    Serial << F("Test: BI16 board color sequence\n");
    buttons.testSequence();
  }
  statusLED(QUICKBLANK);

#if SK_E21TVS_OPTION_GPIO
  Serial << F("Option: GPIO\n");
#else
  for (uint8_t a = 17; a < 17 + 16; a++) {	// a=17 is the index of the first GPIO pin in the HWc configuration
    if (a < SK_HWCCOUNT) // Just making sure...
      HWdis[a] = 1;	// Removes from web interface
  }
#endif


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
        uint8_t b16Map[] = {1, 2, 3, 4, 5, 6}; // These numbers refer to the drawing in the web interface
        for (uint8_t a = 0; a < 6; a++) {
          uint8_t color = b16Map[a] <= getNumberOfPresets() ? (b16Map[a] == presetNum ? 1 : 2) : 0;
          buttons.setButtonColor(a + 1, color);
          if (color > 0 && buttons.buttonIsHeldFor(a + 1, 2000)) {
            setCurrentPreset(b16Map[a]);
            buttons.setButtonColor(a + 1, 1);
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
void HWtestL() { buttons.testProgramme(0xFFFF); }

/**
 * Hardware runloop
 */
void HWrunLoop() {
  uint16_t bUp;
  uint16_t bDown;

  // BI16 buttons:
  bUp = buttons.buttonUpAll();
  bDown = buttons.buttonDownAll();
  uint8_t b16Map[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16}; // These numbers refer to the drawing in the web interface
  for (uint8_t a = 0; a < 16; a++) {
    uint8_t color = actionDispatch(b16Map[a], bDown & (B1 << a), bUp & (B1 << a));
    buttons.setButtonColor(a + 1, (color & 15) > 0 ? ((!(color & 16) || (millis() & 512) > 0) && ((color & 15) != 5) ? 1 : 3) : 0); // This implements the mono color blink bit
  }

#if SK_HWEN_SLIDER
  HWrunLoop_slider(17);
#endif

#if SK_HWEN_GPIO
  static bool gpioStates[] = {false, false, false, false, false, false, false, false};
  HWrunLoop_GPIO(GPIOboard, 18, gpioStates);
#endif
}

uint8_t HWnumOfAnalogComponents() { return 1; }

int16_t HWAnalogComponentValue(uint8_t num) {
  switch (num) {
  case 1:
    return analogRead(A0);
    break;
  }
}

void HWanalogComponentName(uint8_t num, char* buffer, uint8_t len) {
  char *name;
  switch(num) {
    case 1:
      name = "Slider";
      break;
  }
  strncpy(buffer, name, len);
}

uint16_t *HWMinCalibrationValues(uint8_t num) {
  static uint16_t values[3] = {35, 35, 15};
  return values;
}

