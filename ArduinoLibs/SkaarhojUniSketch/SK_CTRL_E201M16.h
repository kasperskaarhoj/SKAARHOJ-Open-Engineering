/**
 * Hardware setup, config mode and preset settings
 */
void HWcfgDisplay() { Serial << F("SK_MODEL: SK_E201M16\n"); }

/**
 * Hardware setup, config mode and preset settings
 */
uint8_t HWsetupL() {
  Serial << F("Init BI8 boards\n");
  buttons.begin(0, false, true);  // Extended bi-color
  buttons.setDefaultColor(0);     // Off by default
  buttons2.begin(1, false, true); // Extended bi-color
  buttons2.setDefaultColor(0);    // Off by default
  buttons3.begin(2, false, true); // Extended bi-color
  buttons3.setDefaultColor(0);    // Off by default
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
      if (sTools.hasTimedOut(timer, 5000)) {
        retVal = 2;
        statusLED(LED_WHITE);
      }
    }
  } else {
    // Preset
    if (getNumberOfPresets() > 1) {
      uint8_t presetNum = EEPROM.read(EEPROM_PRESET_START + 1);

      while (!sTools.hasTimedOut(timer, 2000) || buttons.buttonIsPressedAll() > 0) {
        uint8_t b16Map[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10}; // These numbers refer to the drawing in the web interface
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
void HWtestL() {

#if SK_HWEN_MENU
  menuEncoders.runLoop();
#endif
#if SK_HWEN_SSWMENU
  SSWmenuEnc.runLoop();
#endif

  buttons.testProgramme(0x3FF);

#if SK_HWEN_MENU
  menuEncoders.runLoop();
#endif
#if SK_HWEN_SSWMENU
  SSWmenuEnc.runLoop();
#endif

  buttons2.testProgramme(0x3FF);

#if SK_HWEN_MENU
  menuEncoders.runLoop();
#endif
#if SK_HWEN_SSWMENU
  SSWmenuEnc.runLoop();
#endif

  buttons3.testProgramme(0x37);

#if SK_HWEN_MENU
  menuEncoders.runLoop();
#endif
#if SK_HWEN_SSWMENU
  SSWmenuEnc.runLoop();
#endif

  buttons4.testProgramme(0xF);
}

/**
 * Hardware runloop
 */
void HWrunLoop() {

#if SK_HWEN_MENU
  menuEncoders.runLoop();
#endif
#if SK_HWEN_SSWMENU
  SSWmenuEnc.runLoop();
#endif

  // BI8 buttons:
  uint8_t b16Map[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10}; // These numbers refer to the drawing in the web interface
  HWrunLoop_BI8(buttons, b16Map, sizeof(b16Map));

#if SK_HWEN_MENU
  menuEncoders.runLoop();
#endif
#if SK_HWEN_SSWMENU
  SSWmenuEnc.runLoop();
#endif

  // BI8 buttons 2:
  uint8_t b16Map2[] = {11, 12, 13, 14, 15, 16, 17, 18, 19, 20}; // These numbers refer to the drawing in the web interface
  HWrunLoop_BI8(buttons2, b16Map2, sizeof(b16Map2));

#if SK_HWEN_MENU
  menuEncoders.runLoop();
#endif
#if SK_HWEN_SSWMENU
  SSWmenuEnc.runLoop();
#endif

  // BI8 buttons 3:
  uint8_t b16Map3[] = {23, 24, 25, 0, 22, 26}; // These numbers refer to the drawing in the web interface
  HWrunLoop_BI8(buttons3, b16Map3, sizeof(b16Map3));

#if SK_HWEN_MENU
  menuEncoders.runLoop();
#endif
#if SK_HWEN_SSWMENU
  SSWmenuEnc.runLoop();
#endif

  // BI8 buttons 4:
  uint8_t b16Map4[] = {27, 28, 29, 30}; // These numbers refer to the drawing in the web interface
  HWrunLoop_BI8(buttons4, b16Map4, sizeof(b16Map4));

#if SK_HWEN_SLIDER
  HWrunLoop_slider(21);
#endif

#if (SK_HWEN_MENU)
  HWrunLoop_Menu(31);
#endif

#if SK_HWEN_MENU
  menuEncoders.runLoop();
#endif
#if SK_HWEN_SSWMENU
  SSWmenuEnc.runLoop();
#endif

#if (SK_HWEN_SSWMENU)
  lDelay(2); // This makes sure we spend MUCH more time outside this section where we check for the button press of button 5 - that check is pretty bad for the encoder interrupts, so reducing it's relative time in the runloop is important.
  HWrunLoop_SSWMenu(32);
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
      name = "T-bar";
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
