uint16_t getTouchCoordinate(uint8_t i) {
  uint16_t corners[4] = {240, 116, 3820, 3910};

  uint16_t x, y;
  uint8_t z;
  uint8_t retVal = 0;
  if (touch[i].touched()) {
    retVal = 99;
    // read x & y & z;
    while (!touch[i].bufferEmpty()) {
      // Serial.print(touch.bufferSize());
      touch[i].readData(&y, &x, &z);
      /*      Serial.print("");
            Serial.print(x);
            Serial.print(", ");
            Serial.print(y);
            Serial.print(", ");
            Serial.print(z);
            Serial.print(", ");

            Serial.print((x - corners[1]) * 4 / (corners[3] - corners[1]));
            Serial.print(", ");
            Serial.print(3 - (y - corners[0]) * 4 / (corners[2] - corners[0]));
            Serial.println("");
      */
      retVal = ((((x - corners[1]) * 4 / (corners[3] - corners[1]))) | (((3 - (y - corners[0]) * 4 / (corners[2] - corners[0]))) << 2)) + 1;
      if (i != 0)
        retVal = 17 - retVal;
    }
    touch[i].writeRegister8(STMPE_INT_STA, 0xFF); // reset all ints
  }
  return retVal;
}

/**
 * Hardware setup, config mode and preset settings
 */
void HWcfgDisplay() { Serial << F("SK_MODEL: SK_WINGMANTOUCH\n"); }

/**
 * Hardware setup, config mode and preset settings
 */
bool panelFound[2] = {false, false}; // Used to track which panel is found and should be used
uint8_t HWsetupL() {
  uint8_t retVal = 0;

  for (uint8_t i = 0; i < 2; i++) {

    // Set up I2C port for A or B monitor:
    Wire.beginTransmission(0x70);
    Wire.write(1 << i); // Port 0 or 1
    Wire.endTransmission();

    // Touch
    if (!touch[i].begin()) {
      Serial << F("Touch ") << (i == 0 ? F("A") : F("B")) << F(" not found!\n");
    } else {
      panelFound[i] = true;
      Serial << F("Touch ") << (i == 0 ? F("A") : F("B")) << F(" found\n");
    }
    statusLED(QUICKBLANK);

    Serial << F("Init Dome buttons ") << (i == 0 ? F("A") : F("B")) << "\n";
    buttons[i][0].begin(1, B100011); // Bottom/Top row
    buttons[i][0].setDefaultColor(0);
    buttons[i][1].begin(0, B100000); // Side row
    buttons[i][1].setDefaultColor(0);
    //    if (getConfigMode()) {
    Serial << F("Test: Dome buttons Boards ") << (i == 0 ? F("A") : F("B")) << "\n";
    buttons[i][0].testSequence(); // Runs LED test sequence
    buttons[i][1].testSequence(); // Runs LED test sequence
                                  //  } else
    //  delay(500);
    buttons[i][0].setButtonColorsToDefault();
    buttons[i][1].setButtonColorsToDefault();
    statusLED(QUICKBLANK);

    if (i == 0) {
      // Look for a button press / sustained button press to bring the device into config/config default modes:
      unsigned long timer = millis();
      if ((buttons[i][0].buttonIsPressedAll() & 0xFF) > 0) {
        retVal = 1;
        statusLED(LED_BLUE);
        while ((buttons[i][0].buttonIsPressedAll() & 0xFF) > 0) {
          if (sTools.hasTimedOut(timer, 2000)) {
            retVal = 2;
            statusLED(LED_WHITE);
          }
        }
      } else {
        // Preset
        if (getNumberOfPresets() > 1) {
          uint8_t presetNum = EEPROM.read(EEPROM_PRESET_START + 1);
          while (!sTools.hasTimedOut(timer, 2000) || buttons[i][0].buttonIsPressedAll() > 0) {
            uint8_t b16Map[] = {1, 2, 3, 4, 5};
            for (uint8_t a = 0; a < 5; a++) {
              uint8_t color = b16Map[a] <= getNumberOfPresets() ? (b16Map[a] == presetNum ? 4 : 5) : 0;
              buttons[i][0].setButtonColor(a + 1, color);
              if (color > 0 && buttons[i][0].buttonIsHeldFor(a + 1, 2000)) {
                setCurrentPreset(b16Map[a]);
                buttons[i][0].setButtonColor(a + 1, 2);
                while (buttons[i][0].buttonIsPressedAll() > 0)
                  ;
              }
            }
          }
          buttons[i][0].setButtonColorsToDefault();
        }
      }
    }

    statusLED(QUICKBLANK);

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

    // OLED Displays:
    Serial << F("Init Info Display ") << (i == 0 ? F("A") : F("B")) << "\n";
    infoDisplay[i].begin(0, 1, 1, false, true);
    infoDisplay[i].setRotation(i == 0 ? 0 : 2);
    for (uint8_t a = 0; a < 8; a++) {
      infoDisplay[i].clearDisplay();
      infoDisplay[i].drawBitmap(0, -7 + a, SKAARHOJLogo, 128, 13, 1, false);
      infoDisplay[i].display(B1); // Write to all
    }
    infoDisplay[i].setTextSize(1);
    infoDisplay[i].setTextColor(1, 0);
    infoDisplay[i].setCursor(40, 24);
    infoDisplay[i] << "WINGMAN " << (i == 0 ? F("A") : F("B"));
    infoDisplay[i].display(B1); // Write to all
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
  for (uint8_t i = 0; i < 2; i++) {
    // Set up I2C port for A or B monitor:
    Wire.beginTransmission(0x70);
    Wire.write(1 << i); // Port 1 or 2
    Wire.endTransmission();

    buttons[i][0].testProgramme(0x1F);
    buttons[i][1].testProgramme(0x1F);

    uint8_t coordinate;
    if (panelFound[i]) {
      coordinate = getTouchCoordinate(i);
      if (coordinate && coordinate != 99)
        Serial << F("Coordinate ") << (i == 0 ? F("A") : F("B")) << F(": ") << coordinate << "\n";
    }

    static uint8_t ptr = 0;

    labelDisp[i].clearDisplay();
    for (uint8_t a = 0; a < 5; a++) {
      testGenerateExtRetVal(ptr + a);
      if (coordinate) {
        extRetVal(coordinate, 0);
        extRetValShortLabel(PSTR("Coordinate"));
      }
      writeDisplayTile(labelDisp[i], 0, 2 + a * 53, B0, 0, 0);
    }
    labelDisp[i].display(B10);

    ptr += 5;

    if (i == 0) {
      bool displayWritten = false;
      if ((millis() & 0x4000) == 0x4000) {
        testGenerateExtRetVal(millis() >> 11);
        writeDisplayTile(infoDisplay[i], 0, 0, B1, 0, 1);

        displayWritten = true;
      }
      if (!displayWritten) {
        infoDisplay[i].clearDisplay();
        infoDisplay[i].fillRoundRect(0, 0, 128, 11, 1, 1);
        infoDisplay[i].setTextSize(1);
        infoDisplay[i].setTextColor(0, 1);
        infoDisplay[i].setCursor(10, 2);
        infoDisplay[i] << F("Configuration Mode");
        infoDisplay[i].setTextColor(1, 0);
        infoDisplay[i].setCursor(0, 14);
        infoDisplay[i] << F("http://") << ip;
        infoDisplay[i].setCursor(95, 24);
        infoDisplay[i] << _DECPADL((millis() / 1000) / 60, 2, "0") << (((millis() / 500) % 2) == 1 ? F(" ") : F(":")) << _DECPADL((millis() / 1000) % 60, 2, "0");
        infoDisplay[i].display(B1); // Write to all
      }
    } else {
      testGenerateExtRetVal((millis() >> 11) + 27);
      writeDisplayTile(infoDisplay[i], 0, 0, B1, 0, 1);
    }
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

  for (uint8_t i = 0; i < 2; i++) {
    // Set up I2C port for A or B monitor:
    Wire.beginTransmission(0x70);
    Wire.write(1 << i); // Port 0 or 1
    Wire.endTransmission();

    // Dome buttons, side
    bUp = buttons[i][1].buttonUpAll();
    bDown = buttons[i][1].buttonDownAll();
    uint8_t ai;
    uint8_t b16Map[] = {7 + i * (40 - 1), 8 + i * (40 - 1), 9 + i * (40 - 1), 10 + i * (40 - 1), 11 + i * (40 - 1)}; // These numbers refer to the drawing in the web interface
    for (uint8_t a = 0; a < 5; a++) {
      ai = (i == 0 ? a : 4 - a);
      extRetValPrefersLabel(b16Map[a]);
      uint8_t color = actionDispatch(b16Map[a], bDown & (B1 << ai), bUp & (B1 << ai));
      buttons[i][1].setButtonColor(ai + 1, color & 0xF);
    }

    // Dome buttons, bottom/top
    bUp = buttons[i][0].buttonUpAll();
    bDown = buttons[i][0].buttonDownAll();
    uint8_t b16Map2[] = {15 + i * (40 - 1), 16 + i * (40 - 1), 17 + i * (40 - 1), 18 + i * (40 - 1), 19 + i * (40 - 1)}; // These numbers refer to the drawing in the web interface
    for (uint8_t a = 0; a < 5; a++) {
      ai = (i == 0 ? a : 4 - a);
      extRetValPrefersLabel(b16Map2[a]);
      uint8_t color = actionDispatch(b16Map2[a], bDown & (B1 << ai), bUp & (B1 << ai));
      buttons[i][0].setButtonColor(ai + 1, color & 0xF);
    }

    if (panelFound[i]) {
      // Touching:
      uint8_t coordinate = getTouchCoordinate(i);
      if (coordinate != 99) {
        // First, for 4x4:
        touch_status = coordinate > 0 ? (B1 << (coordinate - 1)) : 0;
        buttonChange = touch_bUp[i] ^ touch_status;
        touch_bUp[i] = touch_status;
        bUp = buttonChange & ~touch_status;
        buttonChange = touch_bDown[i] ^ touch_status;
        touch_bDown[i] = touch_status;
        bDown = buttonChange & touch_status;
      } else {
        bDown = bUp = 0;
      }
      for (uint8_t a = 0; a < 16; a++) {
        actionDispatch(a + 24 + i * (40 - 1), bDown & (B1 << a), bUp & (B1 << a));
      }

      // Then, for 2x2
      if (coordinate != 99) {
        uint8_t Qcoordinate = coordinate ? (((((coordinate - 1) >> 1) & B100) >> 1) | (((coordinate - 1) >> 1) & B1)) + 1 : 0;
        touch_status = Qcoordinate > 0 ? (B1 << (Qcoordinate - 1)) : 0;
        buttonChange = touch_bUpQ[i] ^ touch_status;
        touch_bUpQ[i] = touch_status;
        bUp = buttonChange & ~touch_status;
        buttonChange = touch_bDownQ[i] ^ touch_status;
        touch_bDownQ[i] = touch_status;
        bDown = buttonChange & touch_status;
      } else {
        bDown = bUp = 0;
      }
      for (uint8_t a = 0; a < 4; a++) {
        actionDispatch(a + 20 + i * (40 - 1), bDown & (B1 << a), bUp & (B1 << a));
      }
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

    // Info display, 128x32 OLED:
    static uint16_t infoDisplay_prevHash[2][3];
    static bool infoDisplay_written[2];
    static uint8_t infoDisplay_lastDisplayWritten = 0;
    bool infoDisplay_update = false;
    do {
      rewrite = false;
      for (uint8_t a = 0; a < 3; a++) {
        extRetValIsWanted(true);
        retVal = actionDispatch(a + 12 + i * (40 - 1));
        if (a == 0)
          infoDisplay_written[i] = false;

        if (infoDisplay_prevHash[i][a] != extRetValHash()) {
          if (a > 0 && infoDisplay_lastDisplayWritten != i) {
            infoDisplay_lastDisplayWritten = i;
            rewrite = true;
            infoDisplay[i].clearDisplay();
            for (uint8_t b = 1; b <= 2; b++) {
              infoDisplay_prevHash[i][b]++; // Force updates...
            }
          }
          infoDisplay_prevHash[i][a] = extRetValHash();
          infoDisplay_update = true;

          if (a == 0) { // Main display
            infoDisplay_written[i] = retVal != 0;
            writeDisplayTile(infoDisplay[i], 0, 0, B0, 0, 1);
            if (!retVal) { // In case blanking, make sure other display layers are evaluated:
              for (uint8_t b = 1; b < 3; b++) {
                infoDisplay_prevHash[i][b]++;
              }
            }
          } else {
            if (!infoDisplay_written[i] || retVal != 0) { // Write if a) previous display was not written with non-blank content and b) if this display has non-blank content
              writeDisplayTile(infoDisplay[i], a == 2 ? 64 : 0, 0, B0, a == (i == 0 ? 1 : 2), 0);
            }
          }
          Serial << "Write info! " << i << "," << a << "\n";
        }
      }
    } while (rewrite);
    if (infoDisplay_update)
      infoDisplay[i].display(B1);
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
