
uint8_t HWnumOfAnalogComponents() { return 0; }

int16_t HWAnalogComponentValue(uint8_t num) { return 0; }

uint16_t *HWMinCalibrationValues(uint8_t num) {
  static uint16_t values[3] = {0,0,0};
  return values;
}

/**
 * Hardware setup, config mode and preset settings
 */
void HWcfgDisplay() { Serial << F("SK_MODEL: SK_WINGMAN\n"); }

/**
 * Hardware setup, config mode and preset settings
 */
bool panelFound[2] = {false, false}; // Used to track which panel is found and should be used
uint8_t HWsetupL() {
  uint8_t retVal = 0;

  for (uint8_t i = 0; i < 1; i++) {

    // Set up I2C port for A or B monitor:
    Wire.beginTransmission(0x70);
    Wire.write(1 << i); // Port 0 or 1
    Wire.endTransmission();

    Serial << F("Init Dome buttons ") << (i == 0 ? F("A") : F("B")) << "\n";
    buttons[i].begin(0, B100000); // Side row
    buttons[i].setDefaultColor(0);
    //    if (getConfigMode()) {
    Serial << F("Test: Dome buttons Boards ") << (i == 0 ? F("A") : F("B")) << "\n";
    buttons[i].testSequence(); // Runs LED test sequence
                                  //  } else
    //  delay(500);
    buttons[i].setButtonColorsToDefault();
    statusLED(QUICKBLANK);

    if (i == 0) {
      // Look for a button press / sustained button press to bring the device into config/config default modes:
      unsigned long timer = millis();
      if ((buttons[i].buttonIsPressedAll() & 0xFF) > 0) {
        retVal = 1;
        statusLED(LED_BLUE);
        while ((buttons[i].buttonIsPressedAll() & 0xFF) > 0) {
          if (sTools.hasTimedOut(timer, 2000)) {
            retVal = 2;
            statusLED(LED_WHITE);
          }
        }
      } else {
        // Preset
        if (getNumberOfPresets() > 1) {
          uint8_t presetNum = EEPROM.read(EEPROM_PRESET_START + 1);
          while (!sTools.hasTimedOut(timer, 2000) || buttons[i].buttonIsPressedAll() > 0) {
            uint8_t b16Map[] = {1, 2, 3, 4, 5};
            for (uint8_t a = 0; a < 5; a++) {
              uint8_t color = b16Map[a] <= getNumberOfPresets() ? (b16Map[a] == presetNum ? 4 : 5) : 0;
              buttons[i].setButtonColor(a + 1, color);
              if (color > 0 && buttons[i].buttonIsHeldFor(a + 1, 2000)) {
                setCurrentPreset(b16Map[a]);
                buttons[i].setButtonColor(a + 1, 2);
                while (buttons[i].buttonIsPressedAll() > 0)
                  ;
              }
            }
          }
          buttons[i].setButtonColorsToDefault();
        }
      }
    }


    // Label displays (must come before the OLED displays so the display power is not shot off.
    Serial << F("Init Label Display ") << (i == 0 ? F("A") : F("B")) << "\n";
    labelDisp[i].begin(0, 2);
    labelDisp[i].clearDisplay(); // clears the screen and buffer
    for (uint8_t a = 0; a < 8; a++) {
      if (a == 4)
        labelDisp[i].setRotation(2);
      labelDisp[i].drawBitmap((a & 3) << 6, 0, welcomeGraphics[a & 3], 64, 32, 1, true);
    }
    labelDisp[i].display(B10);

    // Set Contrast and Current:
    labelDisp[i].sendCommand(0xC1, B10);
    labelDisp[i].sendData(15 << 4, B10);

    // Master Contrast Current Control:
    labelDisp[i].sendCommand(0xC7, B10);
    labelDisp[i].sendData(15, B10);
    statusLED(QUICKBLANK);
  }

  labelDisp[0].clearDisplay();
  labelDisp[0].display(B10);
  labelDisp[0].setRotation(3);
  labelDisp[1].clearDisplay();
  labelDisp[1].display(B10);
  labelDisp[1].setRotation(1);

  return retVal;
}

/**
 * Hardware test
 */
void HWtestL() {
  for (uint8_t i = 0; i < 1; i++) {
    // Set up I2C port for A or B monitor:
    Wire.beginTransmission(0x70);
    Wire.write(1 << i); // Port 1 or 2
    Wire.endTransmission();

    buttons[i].testProgramme(0x1F);

    static uint8_t ptr = 0;

    labelDisp[i].clearDisplay();
    for (uint8_t a = 0; a < 5; a++) {
      testGenerateExtRetVal(ptr + a);
      writeDisplayTile(labelDisp[i], 0, 2 + a * 53, B0, 0, 0);
    }
    labelDisp[i].display(B10);

    ptr += 5;
  }
}

/**
 * Hardware runloop
 */
void HWrunLoop() {
  uint16_t bUp;
  uint16_t bDown;
  uint8_t retVal;

  static uint16_t touch_bUp[2] = {0, 0};
  static uint16_t touch_bDown[2] = {0, 0};
  static uint8_t touch_bUpQ[2] = {0, 0};
  static uint8_t touch_bDownQ[2] = {0, 0};
  uint16_t touch_status, buttonChange;
  uint8_t coordinate;

  for (uint8_t i = 0; i < 1; i++) {
    // Set up I2C port for A or B monitor:
    Wire.beginTransmission(0x70);
    Wire.write(1 << i); // Port 0 or 1
    Wire.endTransmission();

    // Dome buttons, side
    bUp = buttons[i].buttonUpAll();
    bDown = buttons[i].buttonDownAll();
    uint8_t ai;
    uint8_t b16Map[] = {7 + i * (40 - 1), 8 + i * (40 - 1), 9 + i * (40 - 1), 10 + i * (40 - 1), 11 + i * (40 - 1)}; // These numbers refer to the drawing in the web interface
    for (uint8_t a = 0; a < 5; a++) {
      ai = (i == 0 ? a : 4 - a);
      extRetValPrefersLabel(b16Map[a]);
      uint8_t color = actionDispatch(b16Map[a], bDown & (B1 << ai), bUp & (B1 << ai));
      buttons[i].setButtonColor(ai + 1, color & 0xF);
    }

    bool rewrite = false;

    // Big OLED:
    static uint16_t detailsDisplay_prevHash[2][5];
    static uint8_t detailsDisplay_lastDisplayWritten = 0;
    bool updateDisplay = false;
    do {
      rewrite = false;
      for (uint8_t a = 0; a < 5; a++) {
        extRetValIsWanted(true);
        retVal = actionDispatch(a + 2 + i * (40 - 1));

        if (detailsDisplay_prevHash[i][a] != extRetValHash()) {
          if (detailsDisplay_lastDisplayWritten != i) {
            detailsDisplay_lastDisplayWritten = i;
            rewrite = true;
            labelDisp[i].clearDisplay();
            for (uint8_t b = 0; b < 5; b++) {
              detailsDisplay_prevHash[i][b]++; // Force updates...
            }
          }
          detailsDisplay_prevHash[i][a] = extRetValHash();

          writeDisplayTile(labelDisp[i], 0, 2 + a * 53, B0, 0, 0);
          updateDisplay = true;
          Serial << "Write! " << i << "," << a << "\n";
        }
      }
    } while (rewrite);
    if (updateDisplay)
      labelDisp[i].display(B10);

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
