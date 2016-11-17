/**
 * Hardware setup, config mode and preset settings
 */
void HWcfgDisplay() { Serial << F("SK_MODEL: SK_E21CMB6M\n"); }

uint8_t chainIndexToModuleIdMap[SK_MODCOUNT];
uint8_t chainIndexToModuleIdOrder[SK_MODCOUNT];

void HWsetupModules(bool noTest = false) { // Will sweep the chain for modules and index them
  modular_ResetChain();                    // Reset chain back to first
  modular_clock();                         // Forward to first module (index 0)

  // Initialize
  memset(chainIndexToModuleIdMap, 255, SK_MODCOUNT);
  memset(MODdis, 1, SK_MODCOUNT);

  for (uint8_t a = 0; a < SK_MODCOUNT; a++) {
    Serial << F("Module #") << (a + 1) << F(": ");
    uint8_t modId = modular_readID();
    switch (modId) {
    case 1:                                                       // AUX
    case 6:                                                       // Display module
    case 3:                                                       // CCU module
    case 5:                                                       // SSW module
    case 4:                                                       // Cut Module
    case 2:                                                       // Audio Module
    case 7:                                                       // GPIO module
      chainIndexToModuleIdMap[a] = modId;                         // Set module id - this must match in the run loop.
      chainIndexToModuleIdOrder[a] = modular_enableModule(modId); // Enable the module found in MODdis array - returns 255 if the module was not found, otherwise the index from moduleIdOrder array - in this way we can also know if the same module is the first, second, third etc. instance (if supported)
      break;
    }

    // Initialize
    switch (modId) {
    case 1: // AUX
      Serial << F("'AUX'\n");
      buttons3[0].begin(1);
      buttons4[0].begin(2);
      buttons3[0].setDefaultColor(0); // Off by default
      buttons4[0].setDefaultColor(0); // Off by default
      if (getConfigMode()) {
        Serial << F("Test: BI8 Boards\n");
        buttons3[0].testSequence(10);
        buttons4[0].testSequence(10);
      }
      buttons3[0].setButtonColorsToDefault();
      buttons4[0].setButtonColorsToDefault();
      break;
    case 6: // Display module
      Serial << F("'Display'\n");
      break;
    case 3: // CCU module
      Serial << F("'CCU'\n");

      // Encoders
      mainEncoders.begin(5);
      mainEncoders.setStateCheckDelay(250);

      irisDisplay.begin(5, 1);
      masterBlackDisplay.begin(4, 1);
      if (!noTest) {
        for (uint8_t i = 0; i < 8; i++) {
          irisDisplay.clearDisplay();
          irisDisplay.drawBitmap(0, -7 + i, SKAARHOJLogo, 128, 13, 1, false);
          irisDisplay.display(B1); // Write to all

          masterBlackDisplay.clearDisplay();
          masterBlackDisplay.drawBitmap(0, -7 + i, SKAARHOJLogo, 128, 13, 1, false);
          masterBlackDisplay.display(B1); // Write to all
        }
      }
      irisDisplay.setTextSize(1);
      irisDisplay.setTextColor(1, 0);
      irisDisplay.setCursor(40, 24);
      irisDisplay << "CMB6M";
      irisDisplay.display(B1);

      buttons5.begin(0);
      buttons5.setDefaultColor(0); // Off by default
      if (getConfigMode()) {
        Serial << F("Test: BI16 board color sequence\n");
        buttons5.testSequence();
      }
      buttons5.setButtonColorsToDefault();

      break;
    case 5: // SSW module
      Serial << F("'SSW'\n");
      break;
    case 4: // Cut Module
      Serial << F("'Cut'\n");
      break;
    case 2: // Audio Module
      Serial << F("'Audio'\n");
      break;
    case 7: // GPIO module
      Serial << F("'GPIO'\n");
      GPIOboard.begin(7);
      break;
    default:
      Serial << F("Not found\n");
      break;
    }

    modular_clock();
  }
}

/**
 * Hardware setup, config mode and preset settings
 */
uint8_t HWsetupL() {

#if SK_E21CMB6M_OPTION_VSLIDER
  HWdis[9 - 1] = 1;
  HWdis[10 - 1] = 1;
  HWdis[19 - 1] = 1;
  HWdis[20 - 1] = 1;
  HWdis[28 - 1] = 1;
  Serial << F("Option: Vertical Slider\n");
#else
  HWdis[33 - 1] = 1;
#endif

  Serial << F("Initialize module chain...\n");
  HWsetupModules();

  Serial << F("Initialize main controller\n");
  modular_ResetChain(); // Reset chain back to first, since this is what we will initialize now

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

  //  MODdis[1]=1;
  //  MODdis[3]=1;

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

  // Testing modules:
  modular_ResetChain(); // Reset chain back to first, since this is what we will test now:
  modular_clock();      // Forward to first module (index 0)

  for (uint8_t a = 0; a < SK_MODCOUNT; a++) {
    uint8_t modId = modular_readID();
    if (chainIndexToModuleIdMap[a] != modId) {
      Serial << F("Modules reconfigured, reinitializing...\n");
      delay(500);
      HWsetupModules(true); // Re-initialize modules if there is disorder...
      break;
    } else if (chainIndexToModuleIdMap[a] == 255) {
      break; // No more modules...
    } else {

      bool displayWritten = false;

      // Initialize
      switch (modId) {
      case 1: // AUX
        buttons3[0].testProgramme(0x3F);
        buttons4[0].testProgramme(0x3F);
        break;
      case 6: // Display module
        break;
      case 3: // CCU module

        mainEncoders.testProgramme(B11111);

        // Iris display:
        if ((millis() & 0x4000) == 0x4000) {
          testGenerateExtRetVal(millis() >> 11);
          writeDisplayTile(irisDisplay, 0, 0, B1, 0, 1);

          displayWritten = true;
        }
        if (!displayWritten) {
          irisDisplay.clearDisplay();
          irisDisplay.fillRoundRect(0, 0, 128, 11, 1, 1);
          irisDisplay.setTextSize(1);
          irisDisplay.setTextColor(0, 1);
          irisDisplay.setCursor(10, 2);
          irisDisplay << F("Configuration Mode");
          irisDisplay.setTextColor(1, 0);
          irisDisplay.setCursor(0, 14);
          irisDisplay << F("http://") << ip;
          irisDisplay.setCursor(95, 24);
          irisDisplay << _DECPADL((millis() / 1000) / 60, 2, "0") << (((millis() / 500) % 2) == 1 ? F(" ") : F(":")) << _DECPADL((millis() / 1000) % 60, 2, "0");
          irisDisplay.display(B1); // Write to all
        }

        mainEncoders.testProgramme(B11111);

        // Master Black display:
        testGenerateExtRetVal((millis() >> 11) + 27);
        writeDisplayTile(masterBlackDisplay, 0, 0, B1, 0, 1);

        mainEncoders.testProgramme(B11111);

        buttons5.testProgramme(0xF);
        break;
      case 5: // SSW module
        break;
      case 4: // Cut Module
        break;
      case 2: // Audio Module
        break;
      case 7: // GPIO module
        GPIOboard.setOutputAll(GPIOboard.inputIsActiveAll() ^ ((millis() + 500) >> 12));
        break;
      }

      modular_clock();
    }
  }

  modular_ResetChain(); // Reset chain back to first, since this is what we will test now:

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

  // Testing modules:
  modular_ResetChain(); // Reset chain back to first, since this is what we will test now:
  modular_clock();      // Forward to first module (index 0)

  for (uint8_t a = 0; a < SK_MODCOUNT; a++) {
    uint8_t modId = modular_readID();
    if (chainIndexToModuleIdMap[a] != modId) {
      Serial << F("Modules reconfigured, reinitializing...\n");
      delay(500);
      HWsetupModules(true); // Re-initialize modules if there is disorder...
      break;
    } else if (chainIndexToModuleIdMap[a] == 255) {
      break; // No more modules...
    } else {

      uint8_t bMap_30[] = {45, 44, 43, 42, 41, 40}; // These numbers refer to the drawing in the web interface
      uint8_t bMap_40[] = {39, 38, 37, 36, 35, 34}; // These numbers refer to the drawing in the web interface
      uint8_t encMap2[] = {0, 73, 0, 74, 0};  // These numbers refer to the drawing in the web interface
      uint8_t b16Map5[] = {77, 75, 78, 76};
	  static bool gpioStates[] = {false, false, false, false, false, false, false, false};

      // Initialize
      switch (modId) {
      case 1: // AUX

        // BI8 buttons:
        HWrunLoop_BI8(buttons3[0], bMap_30, sizeof(bMap_30));

        // BI8 buttons:
        HWrunLoop_BI8(buttons4[0], bMap_40, sizeof(bMap_40));
        break;
      case 6: // Display module
        break;
      case 3: // CCU module

        mainEncoders.runLoop();

        // Main Encoders
        HWrunLoop_encoders(mainEncoders, encMap2, sizeof(encMap2));

        mainEncoders.runLoop();

        // Iris display, 128x32 OLED:
        static uint16_t irisDisplay_prevHash[3];
        static bool irisDisplay_written;
        HWrunLoop_128x32OLED(irisDisplay, 70, irisDisplay_prevHash, irisDisplay_written);

        mainEncoders.runLoop();

        // MasterBlack display, 128x32 OLED:
        static uint16_t masterBlackDisplay_prevHash[3];
        static bool masterBlackDisplay_written;
        HWrunLoop_128x32OLED(masterBlackDisplay, 67, masterBlackDisplay_prevHash, masterBlackDisplay_written);

        mainEncoders.runLoop();

        // BI16 buttons:
        bUp = buttons5.buttonUpAll();
        bDown = buttons5.buttonDownAll();
        for (uint8_t a = 0; a < 4; a++) {
          uint8_t color = actionDispatch(b16Map5[a], bDown & (B1 << a), bUp & (B1 << a));
          buttons5.setButtonColor(a + 1, (color & 15) > 0 ? ((!(color & 16) || (millis() & 512) > 0) && ((color & 15) != 5) ? 1 : 3) : 0); // This implements the mono color blink bit
        }

        break;
      case 5: // SSW module
        break;
      case 4: // Cut Module
        break;
      case 2: // Audio Module
        break;
      case 7: // GPIO module
        HWrunLoop_GPIO(GPIOboard, 113, gpioStates);
        break;
      }

      modular_clock();
    }
  }

  modular_ResetChain(); // Reset chain back to first, since this is what we will provide run loop for now:

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
#if SK_E21CMB6M_OPTION_VSLIDER
  uint8_t b16Map2[] = {7, 8, 0, 0, 0, 21, 11, 12, 13, 14, 15, 16, 17, 18, 0, 0}; // These numbers refer to the drawing in the web interface
#else
  uint8_t b16Map2[] = {7, 8, 9, 10, 0, 21, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20}; // These numbers refer to the drawing in the web interface
#endif
  HWrunLoop_BI8(buttons2, b16Map2, sizeof(b16Map2));

#if SK_HWEN_SLIDER
#if SK_E21CMB6M_OPTION_VSLIDER
  HWrunLoop_slider(33);
#else
  HWrunLoop_slider(28);
#endif
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

void HWanalogComponentName(uint8_t num, char *buffer, uint8_t len) {
  char *name;
  switch (num) {
  case 1:
    name = "Slider";
    break;
  }
  strncpy(buffer, name, len);
}

uint16_t *HWMinCalibrationValues(uint8_t num) {
  static uint16_t values[3] = {0, 0, 0};
  switch (num) {
  case 1:
    values[0] = 35; // Start
    values[1] = 35; // End
    values[2] = 15; // Hysteresis
    break;
  }
  return values;
}
