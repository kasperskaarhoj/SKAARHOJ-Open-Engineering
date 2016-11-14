/**
 * Hardware setup, config mode and preset settings
 */
void HWcfgDisplay() { Serial << F("SK_MODEL: SK_E21CMB6\n"); }

/**
 * Hardware setup, config mode and preset settings
 */
uint8_t HWsetupL() {
  Serial << F("Init BI8 boards\n");
  buttons.begin(1, false, true);
  buttons.setDefaultColor(0); // Off by default
  buttons2.begin(2, false, true);
  buttons2.setDefaultColor(0); // Off by default
  if (getConfigMode()) {
    Serial << F("Test: BI8 Boards\n");
    buttons.testSequence(10);
    buttons2.testSequence(10);
  } else
    delay(500);
  buttons.setButtonColorsToDefault();
  buttons2.setButtonColorsToDefault();
  statusLED(QUICKBLANK);

  // Look for a button press / sustained button press to bring the device into config/config default modes:
  uint8_t retVal = 0;
  unsigned long timer = millis();
  if (buttons.buttonIsPressedAll() > 0 || buttons2.buttonIsPressedAll() > 0) {
    retVal = 1;
    statusLED(LED_BLUE);
    while (buttons.buttonIsPressedAll() > 0 || buttons2.buttonIsPressedAll() > 0) {
      if (sTools.hasTimedOut(timer, 2000)) {
        retVal = 2;
        statusLED(LED_WHITE);
      }
    }
  } else {
    // Preset
    if (getNumberOfPresets() > 1) {
      uint8_t presetNum = EEPROM.read(EEPROM_PRESET_START + 1);

      while (!sTools.hasTimedOut(timer, 2000) || (buttons.buttonIsPressedAll() > 0 || buttons2.buttonIsPressedAll() > 0)) {
        uint8_t b16Map[] = {1, 2, 3, 4, 0, 0, 0, 0, 0, 5, 6, 0, 0, 0, 0, 0}; // These numbers refer to the drawing in the web interface
        for (uint8_t a = 0; a < 16; a++) {
          uint8_t color = (b16Map[a] > 0 && b16Map[a] <= getNumberOfPresets()) ? (b16Map[a] == presetNum ? 4 : 5) : 0;
          buttons.setButtonColor(a + 1, color);
          if (color > 0 && buttons.buttonIsHeldFor(a + 1, 2000)) {
            setCurrentPreset(b16Map[a]);
            buttons.setButtonColor(a + 1, 2);
            while (buttons.buttonIsPressedAll() > 0)
              ;
          }
        }
        uint8_t b16Map2[] = {7, 8, 9, 10}; // These numbers refer to the drawing in the web interface
        for (uint8_t a = 0; a < 4; a++) {
          uint8_t color = (b16Map2[a] > 0 && b16Map2[a] <= getNumberOfPresets()) ? (b16Map2[a] == presetNum ? 4 : 5) : 0;
          buttons2.setButtonColor(a + 1, color);
          if (color > 0 && buttons2.buttonIsHeldFor(a + 1, 2000)) {
            setCurrentPreset(b16Map2[a]);
            buttons2.setButtonColor(a + 1, 2);
            while (buttons2.buttonIsPressedAll() > 0)
              ;
          }
        }
      }
      buttons.setButtonColorsToDefault();
      buttons2.setButtonColorsToDefault();
    }
  }
  return retVal;
}

/**
 * Hardware test
 */
void HWtestL() {
  buttons.testProgramme(0x9F5F);

#if SK_HWEN_SSWMENU
  SSWmenuEnc.runLoop();
#endif
  buttons2.testProgramme(0xFFEF);

#if SK_HWEN_SSWMENU
  SSWmenuEnc.runLoop();
#endif
}

/**
 * Hardware runloop
 */
void HWrunLoop() {
  uint16_t bUp;
  uint16_t bDown;
  uint16_t retVal;

#if SK_HWEN_SSWMENU
  SSWmenuEnc.runLoop();
#endif

  // BI8 buttons:
  uint8_t b16Map[] = {1, 2, 3, 4, 27, 0, 22, 0, 23, 5, 6, 24, 25, 0, 0, 26}; // These numbers refer to the drawing in the web interface
  HWrunLoop_BI8(buttons, b16Map, sizeof(b16Map));

#if SK_HWEN_SSWMENU
  SSWmenuEnc.runLoop();
#endif

  // BI8 buttons 2:
  uint8_t b16Map2[] = {7, 8, 9, 10, 0, 21, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20}; // These numbers refer to the drawing in the web interface
  HWrunLoop_BI8(buttons2, b16Map2, sizeof(b16Map2));

#if SK_HWEN_SLIDER
  HWrunLoop_slider(28);
#endif

#if (SK_HWEN_SSWMENU)
  lDelay(10); // This makes sure we spend MUCH more time outside this section where we check for the button press of button 5 - that check is pretty bad for the encoder interrupts, so reducing it's relative time in the runloop is important.
  HWrunLoop_SSWMenu(29);
#endif

#if (SK_HWEN_STDOLEDDISPLAY)
  static uint16_t infoDisplay_prevHash[3];
  static bool infoDisplay_written;
  HWrunLoop_128x32OLED(infoDisplay, 30, infoDisplay_prevHash, infoDisplay_written);
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
  static uint16_t values[3] = {0,0,0};
  switch(num) {
    case 1:
      values[0] = 35; // Start
      values[1] = 35; // End
      values[2] = 15; // Hysteresis
      break;
  }
  return values;
}

