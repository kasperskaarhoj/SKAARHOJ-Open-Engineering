
/**
 * Displaying controller ID in serial monitor during boot:
 */
void HWcfgDisplay() { Serial << F("SK_MODEL: SK_REFERENCE\n"); }

/**
 * Selecting I2C chain:
 */
void setI2Cchain(uint8_t group) {
  if (group > 7)
    return;

  Wire.beginTransmission(0x71);
  Wire.write(1 << group);
  Wire.endTransmission();

  delay(2);
}

/**
 * Hardware setup, config mode and preset settings
 */
uint8_t HWsetupL() {
  uint8_t retVal = 0;

  // Chain 0:
  setI2Cchain(0);
  Serial << F("I2C Chain 0 setup:\n");

  Serial << F("Init BI16-1x4 board @ ( 32) = 0100000_ = 32+0 // (120) = 1111000_ = 120+0\n");
  buttons.begin(0);
  buttons.setDefaultColor(0); // Off by default
  buttons.setButtonColorsToDefault();
  if (getConfigMode()) {
    Serial << F("Test: BI16 board color sequence\n");
    buttons.testSequence();
  }
  statusLED(QUICKBLANK);

  // Look for a button press / sustained button press to bring the device into config/config default modes:
  unsigned long timer = millis();
  if (buttons.buttonIsPressedAll() > 0) {
    retVal = 1;
    statusLED(LED_BLUE);
    while (buttons.buttonIsPressedAll() > 0) {
      if (sTools.hasTimedOut(timer, 2000)) {
        retVal = 2;
        statusLED(LED_WHITE);
        break;
      }
    }
  } else {
    // Preset
    if (getNumberOfPresets() > 1) {
      uint8_t presetNum = EEPROM.read(EEPROM_PRESET_START + 1);
      while (!sTools.hasTimedOut(timer, 2000) || buttons.buttonIsPressedAll() > 0) {
        uint8_t b16Map[] = {1, 2, 3, 4};
        for (uint8_t a = 0; a < 4; a++) {
          uint8_t color = b16Map[a] <= getNumberOfPresets() ? (b16Map[a] == presetNum ? 1 : 2) : 0;
          buttons.setButtonColor(a + 1, color);
          if (color > 0 && buttons.buttonIsHeldFor(a + 1, 2000)) {
            setCurrentPreset(b16Map[a]);
            buttons.setButtonColor(a + 1, 1);
            while (buttons.buttonIsPressedAll() > 0)
              ;
          }
        }
      }
      buttons.setButtonColorsToDefault();
    }
  }

  Serial << F("Init BI8-2x5RGB-SE-VC board @ ( 34) = 0100010_ = 32+2 // ( 98) = 1100010_ = 96+2 // (122) = 1111010_ = 120+2\n");
  buttons2.begin(2);
  buttons2.setDefaultColor(0); // Off by default
  buttons2.setButtonColorsToDefault();
  if (getConfigMode()) {
    Serial << F("Test: BI8 board color sequence\n");
    buttons2.testSequence();
  }
  statusLED(QUICKBLANK);

  Serial << F("Init DISP256x64 Display @ ( 88) = 1011000_ = 88+0\n");
  detailsDisplay.begin(0);
  detailsDisplay.clearDisplay(); // clears the screen and buffer
  for (uint8_t a = 0; a < 8; a++) {
    if (a == 4)
      detailsDisplay.setRotation(2);
    detailsDisplay.drawBitmap((a & 3) << 6, 0, welcomeGraphics[a & 3], 64, 32, 1, true);
  }
  detailsDisplay.display(B1);

  // Set Contrast and Current:
  detailsDisplay.sendCommand(0xC1, B1);
  detailsDisplay.sendData(15 << 4, B1);

  // Master Contrast Current Control:
  detailsDisplay.sendCommand(0xC7, B1);
  detailsDisplay.sendData(15, B1);

  // Chain 1:
  setI2Cchain(1);
  Serial << F("I2C Chain 1 setup:\n");

  // Encoders
  Serial << F("Init ENCODER4N @ ( 33) = 0100001_ = 32+1\n");
  encoders.begin(1);
  encoders.setStateCheckDelay(250);
  statusLED(QUICKBLANK);

  Serial << F("Init ENCODER4N @ ( 34) = 0100010_ = 32+2\n");
  encoders2.begin(2);
  encoders2.setStateCheckDelay(250);
  statusLED(QUICKBLANK);

  Serial << F("Init ID Display\n");
  idDisplay.begin(7, 0, 1); // DOGM163
  idDisplay.cursor(false);
  idDisplay.print("SKAARHOJ");
  idDisplay.setBacklight(1, 0, 0);
  delay(500);
  idDisplay.setBacklight(0, 1, 0);
  delay(500);
  idDisplay.setBacklight(1, 1, 1);
  statusLED(QUICKBLANK);

  Serial << F("Init DISP128x32 Display @ ( 93) = 1011101_ = 88+5\n");
  infoDisplay2.begin(5, 1);
  infoDisplay2.setRotation(2);

  for (uint8_t i = 0; i < 8; i++) {
    infoDisplay2.clearDisplay();
    infoDisplay2.drawBitmap(0, -7 + i, SKAARHOJLogo, 128, 13, 1, false);
    infoDisplay2.display(B1); // Write to all
  }
  infoDisplay2.setTextSize(1);
  infoDisplay2.setTextColor(1, 0);
  infoDisplay2.setCursor(40, 24);
  infoDisplay2 << "REFERENCE";
  infoDisplay2.display(B1); // Write to all

  Serial << F("Init 4-axis joystick JOY-4AXIS @ ( 73) = 1001001_ = 72+1\n");
  Joystick.joystick_init(50, 1, 0);


  Serial << "I2C Chain 2 setup:\n";
  setI2Cchain(2);

  Serial << F("Init BI8-2x4 board\n");
  buttons3.begin(4);
  buttons3.setDefaultColor(0); // Off by default
  buttons3.setButtonColorsToDefault();
  if (getConfigMode()) {
    Serial << F("Test: BI8 board color sequence\n");
    buttons3.testSequence();
  }
  statusLED(QUICKBLANK);

  // Serial << F("Init Fader\n");

  // uint16_t *cal1 = getAnalogComponentCalibration(3);
  // //  joystick.uniDirectionalSlider_init(10, 35, 35, 0, 1);
  // Fader_y.uniDirectionalSlider_init(cal1[2], cal1[0], cal1[1], 0, 2);
  // Fader_y.uniDirectionalSlider_disableUnidirectionality(true);

  // uint16_t *cal2 = getAnalogComponentCalibration(4);
  // Fader_theta.uniDirectionalSlider_init(cal2[2], cal1[0], cal2[1], 0, 3);
  // Fader_theta.uniDirectionalSlider_disableUnidirectionality(true);

  // Fader_btn.uniDirectionalSlider_init(15, 80, 80, 0, 0);
  // Fader_btn.uniDirectionalSlider_disableUnidirectionality(true);
  
  // Force config mode for the time being:
  retVal = 2;

  // Make sure default chain is selected
  setI2Cchain(0);

  return retVal;
}

/**
 * Hardware test
 */
void HWtestL() {
  static uint16_t testVal = 0;

  setI2Cchain(0);

  // Testing BI16-1x4 buttons:
  buttons.testProgramme(0xF);

  // Testing BI8-2x5RGB-SE-VC buttons:
  buttons2.testProgramme(0x3FF);

  // Testing DISP256x64:
  static uint8_t ptr = 0;
  switch (ptr % 6) {
  case 1:
    testGenerateExtRetVal(ptr + 4);
    writeDisplayTile(detailsDisplay, 0, 0, B1, 3, 1);
    break;
  case 2:
    testGenerateExtRetVal(ptr + 6);
    writeDisplayTile(detailsDisplay, 128, 0, B1, 3);
    break;
  case 3:
    testGenerateExtRetVal(ptr + 7);
    writeDisplayTile(detailsDisplay, 192, 0, B1, 2);
    break;
  case 4:
    testGenerateExtRetVal(ptr + 99);
    writeDisplayTile(detailsDisplay, 0, 32, B1, 1);
    break;
  case 5:
    testGenerateExtRetVal(ptr + 99);
    writeDisplayTile(detailsDisplay, 64, 32, B1, 1);
    break;
  default:
    testGenerateExtRetVal(ptr + 77);
    writeDisplayTile(detailsDisplay, 128, 32, B1, 0, 1);
    break;
  }
  ptr++;

  setI2Cchain(1);
  
  encoders.runLoop();
  encoders2.runLoop();
  encoders.testProgramme(B11111);
  encoders2.testProgramme(B11111);

  idDisplay.gotoRowCol(0, 0);
  idDisplay << "M:" << _DECPADL(millis()>>9, 5, " ") << " ";
  idDisplay.setBacklight(millis() & 0x2000 ? 1 : 0, millis() & 0x1000 ? 1 : 0, millis() & 0x500 ? 1 : 0);

  /*
  // Now, joystick seems pretty noisy, supposedly fixed with new PCB
  
  if (Joystick.joystick_hasMoved(0)) {
    Serial << "Joystick 0: " << Joystick.joystick_position(0) << "\n";
  }
  if (Joystick.joystick_hasMoved((int)1)) {
    Serial << "Joystick 1: " << Joystick.joystick_position(1) << "\n";
  }
  if (Joystick.joystick_hasMoved((int)2)) {
    Serial << "Joystick 2: " << Joystick.joystick_position(2) << "\n";
  }
  if (Joystick.joystick_buttonUp()) {
    Serial << "Joystick Up\n";
  }
  if (Joystick.joystick_buttonDown()) {
    Serial << "Joystick Down\n";
  }  
  */

  setI2Cchain(2);
  buttons3.testProgramme(0xFF);
  
  // Return to chain 0 - otherwise the general test routines in SKAARHOJbase will not work right.
  setI2Cchain(0);
}

/**
 * Hardware runloop
 */
void HWrunLoop() {
  uint16_t bUp;
  uint16_t bDown;
  uint8_t retVal;

  // setI2Cchain(0);

  // // BI16 buttons:
  // bUp = buttons.buttonUpAll();
  // bDown = buttons.buttonDownAll();
  // uint8_t b16Map[] = {122, 123, 124, 125}; // These numbers refer to the drawing in the web interface
  // for (uint8_t a = 0; a < 4; a++) {
  //   extRetValPrefersLabel(b16Map[a]);
  //   uint8_t color = actionDispatch(b16Map[a], bDown & (B1 << a), bUp & (B1 << a));
  //   buttons.setButtonColor(a + 1, (color & 15) > 0 ? ((!(color & 16) || (millis() & 512) > 0) && ((color & 15) != 5) ? 1 : 3) : 0); // This implements the mono color blink bit
  // }

  // setI2Cchain(1);
  // encoders.runLoop();
  // encoders2.runLoop();
  // setI2Cchain(0);

  // // UHB buttons:
  // bUp = buttons2.buttonUpAll();
  // bDown = buttons2.buttonDownAll();

  // uint8_t b16Map2[] = {52, 53, 54, 55, 56, 57, 58, 59, 60, 61}; // These numbers refer to the drawing in the web interface
  // for (uint8_t a = 0; a < 10; a++) {
  //   extRetValPrefersLabel(b16Map2[a]);
  //   uint8_t color = actionDispatch(b16Map2[a], bDown & (B1 << a), bUp & (B1 << a));
  //   buttons2.setButtonColor(a + 1, color & 0xF);
  // }

  // setI2Cchain(1);
  // encoders.runLoop();
  // encoders2.runLoop();

  // // Encoders
  // uint8_t encMap[] = {22, 23, 24, 25}; // These numbers refer to the drawing in the web interface
  // HWrunLoop_encoders(encoders, encMap, sizeof(encMap), true);

  // // Encoders2
  // uint8_t encMap2[] = {5, 6, 7, 8}; // These numbers refer to the drawing in the web interface
  // HWrunLoop_encoders(encoders2, encMap2, sizeof(encMap2), true);

  // encoders.runLoop();
  // encoders2.runLoop();

  // setI2Cchain(0);

  // // Details display, 256x64 OLED:
  // static uint16_t detailsDisplay_prevHash[13];
  // static bool detailsDisplay_written[5];
  // bool detailsDisplay_update = false;
  // for (uint8_t a = 0; a < 13; a++) {
  //   extRetValIsWanted(true);
  //   retVal = actionDispatch(a + 9);
  //   if (a < 5)
  //     detailsDisplay_written[a] = false;

  //   if (detailsDisplay_prevHash[a] != extRetValHash()) {
  //     detailsDisplay_prevHash[a] = extRetValHash();
  //     detailsDisplay_update = true;

  //     if (a == 0) { // Main display
  //       detailsDisplay_written[a] = retVal != 0;
  //       detailsDisplay.zoom2x(true);
  //       writeDisplayTile(detailsDisplay, 0, 0, B0, 0, 2);
  //       if (!retVal) { // In case blanking, make sure other display layers are evaluated:
  //         for (uint8_t b = 1; b < 13; b++) {
  //           detailsDisplay_prevHash[b]++;
  //         }
  //       }
  //     } else if (a >= 1 && a <= 4) { // Q1-Q4
  //       detailsDisplay_written[a] = retVal != 0;
  //       if (!detailsDisplay_written[0] || retVal != 0) { // Write if a) 256x64 was not written with non-blank content and b) if this display has non-blank content
  //         detailsDisplay.zoom2x(false);
  //         writeDisplayTile(detailsDisplay, ((a - 1) & 1) << 7, ((a - 1) & 2) << 4, B0, ~(a - 1), 1);
  //         if (!retVal) { // In case blanking, make sure other display layers are evaluated:
  //           for (uint8_t b = 5; b < 13; b++) {
  //             detailsDisplay_prevHash[b]++;
  //           }
  //         }
  //       }
  //     } else {
  //       if (!(detailsDisplay_written[0] || detailsDisplay_written[((a - 5) >> 1) + 1]) || retVal != 0) { // Write if a) previous displays was not written with non-blank content and b) if this display has non-blank content
  //         detailsDisplay.zoom2x(false);
  //         writeDisplayTile(detailsDisplay, ((a - 5) & 3) << 6, ((a - 5) & 4) << 3, B0, (((a - 5) & 3) != 3) | ((a - 5) & 4 ? 0 : 2), 0);
  //       }
  //     }
  //     Serial << "Write! " << a << "\n";
  //   }
  // }
  // if (detailsDisplay_update)
  //   detailsDisplay.display(B1);

  setI2Cchain(1);
  // encoders.runLoop();
  // encoders2.runLoop();

  static bool lastPosNotPressed;
  bool hasMoved;
  // bool hasMoved;
  // // Fader

  // hasMoved = Fader_y.uniDirectionalSlider_hasMoved();
  // actionDispatch(41, hasMoved, false, 0, 1000 - Fader_y.uniDirectionalSlider_position());

  // // Wheel
  // hasMoved = Fader_theta.uniDirectionalSlider_hasMoved();
  // actionDispatch(101, hasMoved, false, 0, Fader_theta.uniDirectionalSlider_position());

  // // Button
  // //  Serial << (joystickbutton.uniDirectionalSlider_position() < 500) << "\n";
  // Fader_btn.uniDirectionalSlider_hasMoved();
  // static bool lastPosNotPressed = Fader_btn.uniDirectionalSlider_position() < 500;
  // actionDispatch(102, lastPosNotPressed && (Fader_btn.uniDirectionalSlider_position() > 500), !lastPosNotPressed && (Fader_btn.uniDirectionalSlider_position() < 500));
  // lastPosNotPressed = Fader_btn.uniDirectionalSlider_position() < 500;
  // 4-Axis joystick
  hasMoved = Joystick.joystick_hasMoved(0);
  actionDispatch(103, hasMoved, false, 0, Joystick.joystick_position(0));

  hasMoved = Joystick.joystick_hasMoved(1);
  actionDispatch(104, hasMoved, false, 0, Joystick.joystick_position(1));
  hasMoved = Joystick.joystick_hasMoved(2);
  actionDispatch(105, hasMoved, false, 0, Joystick.joystick_position(2));
  static bool lastButtonPressed = false;
  if (Joystick.joystick_buttonIsPressed() != lastButtonPressed) {
    actionDispatch(106, lastButtonPressed, !lastButtonPressed);
    lastButtonPressed = Joystick.joystick_buttonIsPressed();
  }
  // // ID Display:
  // extRetValIsWanted(true);
  // retVal = (actionDispatch(38) & 0xF);
  // bool idDisplay_clrs[6][3] = {{0, 0, 0}, {1, 1, 0}, {1, 0, 0}, {0, 1, 0}, {1, 1, 1}, {1, 0, 1}};
  // static uint8_t idDisplay_prevVal = 0;
  // if (idDisplay_prevVal != retVal) {
  //   idDisplay_prevVal = retVal;
  //   idDisplay.setBacklight(idDisplay_clrs[retVal][0], idDisplay_clrs[retVal][1], idDisplay_clrs[retVal][2]);
  //   Serial << "Set backlight!";
  // }

  // static uint16_t idDisplay_prevHash = 0;
  // if (idDisplay_prevHash != extRetValHash()) {
  //   idDisplay_prevHash = extRetValHash();
  //   idDisplay.clearDisplay();
  //   idDisplay.gotoRowCol(0, 0);
  //   idDisplay << _extRetTxt[0];
  //   Serial << "Write!\n";
  // }

  // // GPI
  // bUp = bDown = 0;
  // static bool gpiCache = false;
  // if (addressSwitch_getGPI()) {
  //   if (!gpiCache) {
  //     gpiCache = true;
  //     bDown = 1;
  //   }
  // } else {
  //   if (gpiCache) {
  //     gpiCache = false;
  //     bUp = 1;
  //   }
  // }

  // actionDispatch(52, bDown, bUp);

  // // GPO:
  // static bool gpoCache = false;
  // uint8_t state = actionDispatch(51) & 0xF;
  // if (state != 5 && state != 0) {
  //   if (!gpoCache) {
  //     gpoCache = true;
  //     addressSwitch_setGPO(true);
  //   }
  // } else {
  //   if (gpoCache) {
  //     gpoCache = false;
  //     addressSwitch_setGPO(false);
  //   }
  // }

  setI2Cchain(0);
}

uint8_t HWnumOfAnalogComponents() { return 3; }

int16_t HWAnalogComponentValue(uint8_t num) {
  switch (num) {
  case 1:
    return analogRead(A0); // Slider
  }
}

void HWanalogComponentName(uint8_t num, char *buffer, uint8_t len) {
  char *name;
  switch (num) {
  case 1:
    name = "Slider";
    break;
  default:
    name = "";
  }
  strncpy(buffer, name, len);
}

uint16_t *HWMinCalibrationValues(uint8_t num) {
  static uint16_t values[3] = {0, 0, 0};
  switch (num) {
  default:
    values[0] = 35; // Start
    values[1] = 35; // End
    values[2] = 15; // Hysteresis
    break;
  }
  return values;
}
