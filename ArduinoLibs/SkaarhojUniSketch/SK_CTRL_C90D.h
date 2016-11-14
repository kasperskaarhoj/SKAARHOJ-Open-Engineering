/**
 * Hardware setup, config mode and preset settings
 */
void HWcfgDisplay() { Serial << F("SK_MODEL: SK_C90D\n"); }

uint32_t disableMask = SK_C90D_OPTION_DISABLEMASK;

/**
 * Hardware setup, config mode and preset settings
 */
uint8_t HWsetupL() {

  Serial << F("Init BI8 boards\n");
  buttons.begin(0, false, true);
  buttons.setDefaultColor(0); // Off by default
  buttons2.begin(1, false, true);
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

  #if SK_C90D_OPTION_GPIO
  Serial << F("Option: GPIO\n");
  #else
  // These indices are subtracted 1 compared to the web interface
  for (uint8_t a = 33; a < 33 + 16; a++) {  // a=33 is the index of the first GPIO pin in the HWc configuration
    if (a < SK_HWCCOUNT) // Just making sure...
      HWdis[a] = 1; // Removes from web interface
  }
  #endif

  uint32_t variants[5] = {
    0,
    0b11000111110001110000000000000000,
    0b11110000111100000000000000000000,
    0b00001000000010000100000001000000,
    0b00100000001000000100111101001111,
  };

  if(SK_C90D_OPTION_VARIANT <= 4) {
    Serial << F("Option: Variant ") << SK_C90D_OPTION_VARIANT << F("\n");
    disableMask |= variants[SK_C90D_OPTION_VARIANT];
  }

  for(int8_t i=0; i<32; i++) {
    HWdis[i] = (disableMask >> (31-i)) & 1;
  }

  // Look for a button press / sustained button press to bring the device into config/config default modes:
  uint8_t retVal = 0;
  unsigned long timer = millis();
  if ((buttons.buttonIsPressedAll()) > 0) {
    retVal = 1;
    statusLED(LED_BLUE);
    while ((buttons.buttonIsPressedAll()) > 0) {
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

uint8_t buttonMap[] = {4,5,6,7,12,13,14,15,0,1,2,3,8,9,10,11};

uint16_t buttonSequence(uint16_t mask) {
  uint16_t output = 0;
  for(uint8_t i = 0; i<16; i++) {
    output |= (mask >> i) & 1 ? 1 << buttonMap[15-i] : 0;
  }

  return output;
}

/**
 * Hardware test
 */
void HWtestL() {
  buttons.testProgramme(buttonSequence(disableMask >> 16) ^ 0xFFFF);
  buttons2.testProgramme(buttonSequence(disableMask & 0xFFFF) ^ 0xFFFF);
}

/**
 * Hardware runloop
 */
void HWrunLoop() {

  // It is implied that the button numbers in the web interface goes from 1 to 32
  uint8_t b16Map[16], b16Map2[16];
  for(uint8_t i=0; i<16; i++) {
    b16Map[buttonMap[i]] = (disableMask >> (31-i)) & 1 ? 0 : i+1;
    b16Map2[buttonMap[i]] = (disableMask >> (15-i)) & 1 ? 0 : i+17;
  }

  HWrunLoop_BI8(buttons, b16Map, 16);
  HWrunLoop_BI8(buttons2, b16Map2, 16);

  #if SK_HWEN_SLIDER
    HWrunLoop_slider(33);
  #endif

  #if SK_HWEN_GPIO
  static bool gpioStates[] = {false, false, false, false, false, false, false, false};
  HWrunLoop_GPIO(GPIOboard, 34, gpioStates);
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

