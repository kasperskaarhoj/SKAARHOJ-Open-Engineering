/**
 * Hardware setup, config mode and preset settings
 */
void HWcfgDisplay() { Serial << F("SK_MODEL: SK_C90A\n"); }

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


  audio_a.begin(2, 0);
  audio_b.begin(3, 0);
  audio_c.begin(4, 0);
  uint16_t *cal1 = getAnalogComponentCalibration(3);
  audioPot_a1.uniDirectionalSlider_init(cal1[2], cal1[0], cal1[1], 1, 0);
  audioPot_a1.uniDirectionalSlider_disableUnidirectionality(true);
  uint16_t *cal2 = getAnalogComponentCalibration(4);
  audioPot_a2.uniDirectionalSlider_init(cal2[2], cal2[0], cal2[1], 1, 1);
  audioPot_a2.uniDirectionalSlider_disableUnidirectionality(true);
  uint16_t *cal3 = getAnalogComponentCalibration(5);
  audioPot_b1.uniDirectionalSlider_init(cal3[2], cal3[0], cal3[1], 2, 0);
  audioPot_b1.uniDirectionalSlider_disableUnidirectionality(true);
  uint16_t *cal4 = getAnalogComponentCalibration(6);
  audioPot_b2.uniDirectionalSlider_init(cal4[2], cal4[0], cal4[1], 2, 1);
  audioPot_b2.uniDirectionalSlider_disableUnidirectionality(true);
  uint16_t *cal5 = getAnalogComponentCalibration(7);
  audioPot_c1.uniDirectionalSlider_init(cal5[2], cal5[0], cal5[1], 3, 0);
  audioPot_c1.uniDirectionalSlider_disableUnidirectionality(true);
  uint16_t *cal6 = getAnalogComponentCalibration(8);
  audioPot_c2.uniDirectionalSlider_init(cal6[2], cal6[0], cal6[1], 3, 1);
  audioPot_c2.uniDirectionalSlider_disableUnidirectionality(true);

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

  uint8_t b16Map[] = {5 + 29, 6 + 29, 7 + 29, 8 + 29, 1 + 29, 2 + 29, 3 + 29, 4 + 29}; // These numbers refer to the drawing in the web interface
  HWrunLoop_BI8(buttons, b16Map, sizeof(b16Map));

  uint8_t b16Map2[] = {9 + 29, 0, 12 + 29, 13 + 29, 0, 0, 10 + 29, 11 + 29}; // These numbers refer to the drawing in the web interface
  HWrunLoop_BI8(buttons2, b16Map2, sizeof(b16Map2));

#if SK_HWEN_SLIDER
  HWrunLoop_slider(14 + 29);
#endif

#if SK_HWEN_ACM
  uint8_t b16MapACM[] = {24, 25, 26, 0, 0, 29, 28, 27}; // These numbers refer to the drawing in the web interface in this order: Potmeter, Peak Diode, VU, left button, right button, lower button, middle button, top button
  HWrunLoop_AudioControlMaster(AudioMasterControl, AudioMasterPot, b16MapACM, sizeof(b16MapACM));
#endif

  uint8_t b16MapAC2a[] = {1, 5, 2, 6, 3, 4, 7, 8}; // These numbers refer to the drawing in the web interface in this order: Potmeter A, Potmeter B, Peak Diode A, Peak Diode B, left button A, right button A, left button B, right button B button A
  HWrunLoop_AudioControl(audio_a, audioPot_a1, audioPot_a2, b16MapAC2a, sizeof(b16MapAC2a));

  uint8_t b16MapAC2b[] = {9, 13, 10, 14, 11, 12, 15, 16}; // These numbers refer to the drawing in the web interface in this order: Potmeter A, Potmeter B, Peak Diode A, Peak Diode B, left button A, right button A, left button B, right button B right button A
  HWrunLoop_AudioControl(audio_b, audioPot_b1, audioPot_b2, b16MapAC2b, sizeof(b16MapAC2b));

  uint8_t b16MapAC2c[] = {17, 21, 18, 22, 19, 20, 23, 0}; // These numbers refer to the drawing in the web interface in this order: Potmeter A, Potmeter B, Peak Diode A, Peak Diode B, left button A, right button A, left button B, right button B right button A
  HWrunLoop_AudioControl(audio_c, audioPot_c1, audioPot_c2, b16MapAC2c, sizeof(b16MapAC2c));
}



uint8_t HWnumOfAnalogComponents() { return 8; }

int16_t HWAnalogComponentValue(uint8_t num) {
  switch (num) {
  case 1:
    return analogRead(A0);
    break;
  case 2:
    return AudioMasterPot.uniDirectionalSlider_rawValue();
    break;
  case 3:
    return audioPot_a1.uniDirectionalSlider_rawValue();
    break;
  case 4:
    return audioPot_a2.uniDirectionalSlider_rawValue();
    break;
  case 5:
    return audioPot_b1.uniDirectionalSlider_rawValue();
    break;
  case 6:
    return audioPot_b2.uniDirectionalSlider_rawValue();
    break;
  case 7:
    return audioPot_c1.uniDirectionalSlider_rawValue();
    break;
  case 8:
    return audioPot_c2.uniDirectionalSlider_rawValue();
    break;
  }
}

uint16_t *HWMinCalibrationValues(uint8_t num) {
  static uint16_t values[3] = {35,65,15}; // Default for audio knobs
  switch(num) {
    case 1: // Slider
      values[0] = 35;
      values[1] = 35;
      values[2] = 15;
      break;
  }
  return values;
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

