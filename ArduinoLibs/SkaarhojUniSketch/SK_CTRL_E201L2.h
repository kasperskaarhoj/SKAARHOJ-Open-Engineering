/**
 * Hardware setup, config mode and preset settings
 */
void HWcfgDisplay() { Serial << F("SK_MODEL: SK_E201L2\n"); }

/**
 * Hardware setup, config mode and preset settings
 */
uint8_t HWsetupL() {
  Serial << F("Init BI8 boards\n");
  buttons.begin(0, false);
  buttons.setDefaultColor(0); // Off by default
  buttons2.begin(1, false);
  buttons2.setDefaultColor(0); // Off by default
  buttons3.begin(2, false);
  buttons3.setDefaultColor(0); // Off by default
  buttons4.begin(3, false);
  buttons4.setDefaultColor(0); // Off by default
  if (getConfigMode()) {
    Serial << F("Test: BI8 Boards\n");
    buttons.testSequence(10);
    buttons2.testSequence(10);
    buttons3.testSequence(10);
    buttons4.testSequence(10);
  } else
    delay(500);
  buttons.setButtonColorsToDefault();
  buttons2.setButtonColorsToDefault();
  buttons3.setButtonColorsToDefault();
  buttons4.setButtonColorsToDefault();
  statusLED(QUICKBLANK);

  // Look for a button press / sustained button press to bring the device into config/config default modes:
  uint8_t retVal = 0;
  unsigned long timer = millis();
  if (buttons.buttonIsPressedAll() > 0 || buttons2.buttonIsPressedAll() > 0 || buttons3.buttonIsPressedAll() > 0 || buttons4.buttonIsPressedAll() > 0) {
    retVal = 1;
    statusLED(LED_BLUE);
    while (buttons.buttonIsPressedAll() > 0 || buttons2.buttonIsPressedAll() > 0 || buttons3.buttonIsPressedAll() > 0 || buttons4.buttonIsPressedAll() > 0) {
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
        uint8_t b16Map[] = {1, 2, 3, 4, 5, 6, 7, 8}; // These numbers refer to the drawing in the web interface
        for (uint8_t a = 0; a < 8; a++) {
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
void HWtestL() {
  buttons.testProgramme(0xFF);
  buttons2.testProgramme(0xFF);
  buttons3.testProgramme(0xFF);
  buttons4.testProgramme(0xFF);
}

/**
 * Hardware runloop
 */
void HWrunLoop() {
  uint16_t bUp;
  uint16_t bDown;

  // BI8 buttons:
  bUp = buttons.buttonUpAll();
  bDown = buttons.buttonDownAll();
  uint8_t b16Map[] = {1, 2, 3, 4, 5, 6, 7, 8}; // These numbers refer to the drawing in the web interface
  for (uint8_t a = 0; a < 8; a++) {
    uint8_t color = actionDispatch(b16Map[a], bDown & (B1 << a), bUp & (B1 << a));
    buttons.setButtonColor(a + 1, color & 0xF);
  }

  // BI8 buttons 2:
  bUp = buttons2.buttonUpAll();
  bDown = buttons2.buttonDownAll();
  uint8_t b16Map2[] = {9, 10, 11, 12, 13, 14, 15, 16}; // These numbers refer to the drawing in the web interface
  for (uint8_t a = 0; a < 8; a++) {
    uint8_t color = actionDispatch(b16Map2[a], bDown & (B1 << a), bUp & (B1 << a));
    buttons2.setButtonColor(a + 1, color & 0xF);
  }

  // BI8 buttons 3:
  bUp = buttons3.buttonUpAll();
  bDown = buttons3.buttonDownAll();
  uint8_t b16Map3[] = {18, 19, 22, 25, 20, 21, 24, 23}; // These numbers refer to the drawing in the web interface
  for (uint8_t a = 0; a < 8; a++) {
    uint8_t color = actionDispatch(b16Map3[a], bDown & (B1 << a), bUp & (B1 << a));
    buttons3.setButtonColor(a + 1, color & 0xF);
  }

  // BI8 buttons 4:
  bUp = buttons4.buttonUpAll();
  bDown = buttons4.buttonDownAll();
  uint8_t b16Map4[] = {26, 27, 28, 29, 30, 31, 32, 33}; // These numbers refer to the drawing in the web interface
  for (uint8_t a = 0; a < 8; a++) {
    uint8_t color = actionDispatch(b16Map4[a], bDown & (B1 << a), bUp & (B1 << a));
    buttons4.setButtonColor(a + 1, color & 0xF);
  }

#if SK_HWEN_SLIDER
  HWrunLoop_slider(17);
#endif
}

uint8_t HWnumOfAnalogComponents() { return 1; }

int16_t HWAnalogComponentValue(uint8_t num) {
  return analogRead(A0);
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

