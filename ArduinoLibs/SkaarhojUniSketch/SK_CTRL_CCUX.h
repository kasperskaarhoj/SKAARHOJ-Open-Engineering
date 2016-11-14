/**
 * Hardware setup, config mode and preset settings
 */
void HWcfgDisplay() { Serial << F("SK_MODEL: SK_CCUX\n"); }

/**
 * Hardware setup, config mode and preset settings
 */
uint8_t HWsetupL() {
  uint8_t retVal = 0;

  Serial << F("Init BI8 boards\n");
  buttons.begin(0, false);
  buttons.setDefaultColor(0); // Off by default
  if (getConfigMode()) {
    Serial << F("Test: BI8 Boards\n");
    buttons.testSequence(10);
  } else
    delay(500);
  buttons.setButtonColorsToDefault();
  statusLED(QUICKBLANK);

  // Look for a button press / sustained button press to bring the device into config/config default modes:
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
        uint8_t b16Map[] = {8, 6, 4, 2, 7, 5, 3, 1}; // These numbers refer to the drawing in the web interface
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

  // Encoders
  Serial << F("Main Encoders\n");
  mainEncoders.begin(1);
  mainEncoders.setStateCheckDelay(250);
  statusLED(QUICKBLANK);

  Serial << F("Settings Encoders\n");
  settingsEncoders.begin(2);
  settingsEncoders.setStateCheckDelay(250);
  statusLED(QUICKBLANK);

  // OLED Displays:
  Serial << F("Init Master Black + Iris Display\n");

  irisDisplay.begin(5, 1);
  masterBlackDisplay.begin(4, 1);
  for (uint8_t i = 0; i < 8; i++) {
    irisDisplay.clearDisplay();
    irisDisplay.drawBitmap(0, -7 + i, SKAARHOJLogo, 128, 13, 1, false);
    irisDisplay.display(B1); // Write to all

    masterBlackDisplay.clearDisplay();
    masterBlackDisplay.drawBitmap(0, -7 + i, SKAARHOJLogo, 128, 13, 1, false);
    masterBlackDisplay.display(B1); // Write to all
  }
  irisDisplay.setTextSize(1);
  irisDisplay.setTextColor(1, 0);
  irisDisplay.setCursor(40, 24);
  irisDisplay << "CCU-X";
  irisDisplay.display(B1); // Write to all
  statusLED(QUICKBLANK);

  Serial << F("Init OLED Menu Display\n");
  OLEDmenuDisplay.begin(6);
  OLEDmenuDisplay.clearDisplay();
  OLEDmenuDisplay.drawBitmap(0, 8, welcomeGraphics[0], 64, 32, 1, true);
  OLEDmenuDisplay.drawBitmap(64, 8, welcomeGraphics[1], 64, 32, 1, true);
  OLEDmenuDisplay.drawBitmap(-32, 32 - 8, welcomeGraphics[1], 64, 32, 1, true);
  OLEDmenuDisplay.drawBitmap(32, 32 - 8, welcomeGraphics[2], 64, 32, 1, true);

  OLEDmenuDisplay.drawBitmap(0, 0, OLEDarrows[0], 96, 12, 1, true);
  OLEDmenuDisplay.drawBitmap(0, 64 - 12, OLEDarrows[1], 96, 12, 1, true);

  OLEDmenuDisplay.display(B1); // Write

  // Smart Displays:
  Serial << F("Init SmartSwitch Buttons\n");
  boardDisplayArray.begin(3); // SETUP: Board address
  boardDisplayArray.setButtonBrightness(7, B1111);
  boardDisplayArray.setButtonBrightness(7, B1111);
  boardDisplayArray.setButtonColor(0, 2, 3, B1111);
  for (uint8_t i = 0; i < 64; i += 2) {
    boardDisplayArray.clearDisplay();
    boardDisplayArray.drawBitmap(64 - i - 1, 0, welcomeGraphics[2], 64, 32, 1, true);
    boardDisplayArray.display(B100); // Write
    boardDisplayArray.clearDisplay();
    boardDisplayArray.drawBitmap(0, -(32 - (i >> 1) - 1), welcomeGraphics[3], 64, 32, 1, true);
    boardDisplayArray.display(B1000); // Write
    boardDisplayArray.clearDisplay();
    boardDisplayArray.drawBitmap(-(64 - i - 1), 0, welcomeGraphics[0], 64, 32, 1, true);
    boardDisplayArray.display(B1); // Write
    boardDisplayArray.clearDisplay();
    boardDisplayArray.drawBitmap(0, 32 - (i >> 1) - 1, welcomeGraphics[1], 64, 32, 1, true);
    boardDisplayArray.display(B10); // Write
  }
  statusLED(QUICKBLANK);

  Serial << F("Init Joystick\n");
  joystick.joystick_init(10, 0);

  return retVal;
}

/**
 * Hardware test
 */
void HWtestL() {
  buttons.testProgramme(0xFF);
  settingsEncoders.testProgramme(B11111);
  mainEncoders.testProgramme(B11111);

  boardDisplayArray.testProgramme(B1111);
  settingsEncoders.testProgramme(B11111);
  mainEncoders.testProgramme(B11111);

  // Iris display:
  bool displayWritten = false;
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

  settingsEncoders.testProgramme(B11111);
  mainEncoders.testProgramme(B11111);

  // Master Black display:
  testGenerateExtRetVal((millis() >> 11) + 27);
  writeDisplayTile(masterBlackDisplay, 0, 0, B1, 0, 1);

  settingsEncoders.testProgramme(B11111);
  mainEncoders.testProgramme(B11111);

  // OLED display
  //  OLEDmenuDisplay.testProgramme(B1);
  testGenerateExtRetVal((millis() >> 11) + 14);
  writeDisplayTile(OLEDmenuDisplay, 16, 16, B1, 0, 0);

  settingsEncoders.testProgramme(B11111);
  mainEncoders.testProgramme(B11111);

  // Joystick test:
  if (joystick.joystick_hasMoved(0) || joystick.joystick_hasMoved(1)) {
    Serial << F("New joystick position:") << joystick.joystick_position(0) << "," << joystick.joystick_position(1) << "\n";
  }
  if (joystick.joystick_buttonUp()) {
    Serial << F("Joystick Button Up") << "\n";
  }
  if (joystick.joystick_buttonDown()) {
    Serial << F("Joystick Button Down") << "\n";
  }
  if (joystick.joystick_buttonIsPressed()) {
    Serial << F("Joystick Button Pressed") << "\n";
  }
}

/**
 * Hardware runloop
 */
void HWrunLoop() {
  uint16_t bUp;
  uint16_t bDown;
  uint8_t retVal;

  // BI8 buttons:
  uint8_t b16Map[] = {8, 6, 4, 2, 7, 5, 3, 1}; // These numbers refer to the drawing in the web interface
  HWrunLoop_BI8(buttons, b16Map, sizeof(b16Map));

  settingsEncoders.runLoop();
  mainEncoders.runLoop();

  // OLED Rocker
  static uint16_t oledDisplay_prevHash = 0;
  bDown = OLEDmenuDisplay.buttonDownAll(B1);
  int16_t pulses = 0;
  if (bDown & B1)
    pulses = -1;
  if (bDown & B100)
    pulses = 1;
  extRetValIsWanted(true);
  actionDispatch(27, false, false, pulses << 1);
  if (oledDisplay_prevHash != extRetValHash()) {
    oledDisplay_prevHash = extRetValHash();
    writeDisplayTile(OLEDmenuDisplay, 16, 16, B1, 0, 0);
    Serial << "Write OLED Rocker!\n";
  }

  settingsEncoders.runLoop();
  mainEncoders.runLoop();

  uint8_t encMap[] = {9, 10, 11, 12, 23}; // These numbers refer to the drawing in the web interface
  HWrunLoop_encoders(settingsEncoders, encMap, sizeof(encMap));

  settingsEncoders.runLoop();
  mainEncoders.runLoop();

  // Main Encoders
  uint8_t encMap2[] = {0, 24, 0, 25, 26}; // These numbers refer to the drawing in the web interface
  HWrunLoop_encoders(mainEncoders, encMap2, sizeof(encMap2));

  settingsEncoders.runLoop();
  mainEncoders.runLoop();

  // Settings displays, 64x32:
  static uint16_t settingsDisplay_prevHash[4];
  static uint8_t settingsDisplay_prevColor[4];
  for (uint8_t a = 0; a < 4; a++) {
    extRetValIsWanted(true);
    retVal = actionDispatch(a + 13);
    if (settingsDisplay_prevHash[a] != extRetValHash()) {
      settingsDisplay_prevHash[a] = extRetValHash();
      writeDisplayTile(boardDisplayArray, 0, 0, B1 << a, 0, 0);
      if (debugMode)
        Serial << "Write 64x32 " << a << "\n";
    }

    if (settingsDisplay_prevColor[a] != _extRetColor) {
      settingsDisplay_prevColor[a] = _extRetColor;
      boardDisplayArray.setButtonColor((_extRetColor >> 4) & 3, (_extRetColor >> 2) & 3, _extRetColor & 3, B1 << a);
      if (debugMode)
        Serial << F("Write SSWcolor! ") << a << F("\n");
    }
  }

  // Iris display, 128x32 OLED:
  static uint16_t irisDisplay_prevHash[3];
  static bool irisDisplay_written;
  HWrunLoop_128x32OLED(irisDisplay, 20, irisDisplay_prevHash,  irisDisplay_written);

  settingsEncoders.runLoop();
  mainEncoders.runLoop();

  // MasterBlack display, 128x32 OLED:
  static uint16_t masterBlackDisplay_prevHash[3];
  static bool masterBlackDisplay_written;
  HWrunLoop_128x32OLED(masterBlackDisplay, 17, masterBlackDisplay_prevHash,  masterBlackDisplay_written);

  settingsEncoders.runLoop();
  mainEncoders.runLoop();
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

