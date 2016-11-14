/**
 * Hardware setup, config mode and preset settings
 */
void HWcfgDisplay() { Serial << F("SK_MODEL: SK_C90SM\n"); }

/**
 * Hardware setup, config mode and preset settings
 */
uint8_t HWsetupL() {
  Serial << F("Init BI8 boards\n");
  buttons.begin(0, false);
  buttons.setDefaultColor(0); // Off by default
  buttons2.begin(1, false);
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
  if ((buttons.buttonIsPressedAll() & 0xFF) > 0) {
    retVal = 1;
    statusLED(LED_BLUE);
    while ((buttons.buttonIsPressedAll() & 0xFF) > 0) {
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
        uint8_t b16Map[] = {1, 3, 5, 7, 2, 4, 6, 8};
        for (uint8_t a = 0; a < 8; a++) {
          uint8_t color = b16Map[a] <= getNumberOfPresets() ? (b16Map[a] == presetNum ? 4 : 5) : 0;
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
  buttons2.testProgramme(B11001101);
}

/**
 * Hardware runloop
 */
void HWrunLoop() {
  uint16_t bUp;
  uint16_t bDown;

#if SK_HWEN_MENU
  menuEncoders.runLoop();
#endif
#if SK_HWEN_SSWMENU
  SSWmenuEnc.runLoop();
#endif

  uint8_t b16Map[] = {5, 6, 7, 8, 1, 2, 3, 4}; // These numbers refer to the drawing in the web interface
  HWrunLoop_BI8(buttons, b16Map, sizeof(b16Map));

#if SK_HWEN_MENU
  menuEncoders.runLoop();
#endif
#if SK_HWEN_SSWMENU
  SSWmenuEnc.runLoop();
#endif

  uint8_t b16Map2[] = {9, 0, 12, 13, 0, 0, 10, 11}; // These numbers refer to the drawing in the web interface
  HWrunLoop_BI8(buttons2, b16Map2, sizeof(b16Map2));

#if SK_HWEN_SLIDER
  HWrunLoop_slider(14);
#endif

#if (SK_HWEN_MENU)
  HWrunLoop_Menu(24);
#endif

#if SK_HWEN_MENU
  menuEncoders.runLoop();
#endif
#if SK_HWEN_SSWMENU
  SSWmenuEnc.runLoop();
#endif

#if (SK_HWEN_SSWMENU)
  lDelay(2); // This makes sure we spend MUCH more time outside this section where we check for the button press of button 5 - that check is pretty bad for the encoder interrupts, so reducing it's relative time in the runloop is important.
  HWrunLoop_SSWMenu(15);
#endif

#if SK_HWEN_GPIO
  static bool gpioStates[] = {false, false, false, false, false, false, false, false};
  HWrunLoop_GPIO(GPIOboard, 25, gpioStates);
#endif

#if SK_HWEN_MENU
  menuEncoders.runLoop();
#endif
#if SK_HWEN_SSWMENU
  SSWmenuEnc.runLoop();
#endif

#if SK_HWEN_ACM
  uint8_t b16MapACM[] = {16, 19, 20, 17, 18, 23, 22, 21}; // These numbers refer to the drawing in the web interface in this order: Potmeter, Peak Diode, VU, left button, right button, lower button, middle button, top button
  HWrunLoop_AudioControlMaster(AudioMasterControl, AudioMasterPot, b16MapACM, sizeof(b16MapACM));
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

