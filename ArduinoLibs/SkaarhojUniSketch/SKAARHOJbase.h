/*
Copyright 2016 Kasper Skårhøj, SKAARHOJ, kasper@skaarhoj.com

This file is part of the Skaarhoj Unisketch for Arduino

The Skaarhoj Unisketch library is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by the
Free Software Foundation, either version 3 of the License, or (at your
option) any later version.

The Skaarhoj Unisketch library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with the Skaarhoj Unisketch library. If not, see http://www.gnu.org/licenses/.


IMPORTANT: If you want to use this library in your own projects and/or products,
please play a fair game and heed the license rules! See our web page for a Q&A so
you can keep a clear conscience: http://skaarhoj.com/about/licenses/

*/

/************************************
 *
 * STANDARD FUNCTIONS and VARIABLES
 *
 ************************************/

#if defined(__arm__)
void resetFunc() {
  const int RSTC_KEY = 0xA5;
  RSTC->RSTC_CR = RSTC_CR_KEY(RSTC_KEY) | RSTC_CR_PROCRST | RSTC_CR_PERRST;
  while (true)
    ;
}
#else
void (*resetFunc)(void) = 0; // declare reset function @ address 0
#endif

// MAC address and IP address for this *particular* SKAARDUINO
byte mac[6] = {};                   // Loaded from EEPROM
IPAddress ip(192, 168, 10, 99);     // This is the default IP address in case config mode 2 is entered
IPAddress subnet(255, 255, 255, 0); // This is the default subnet address

// Status LED codes:
#define LED_RED 1
#define LED_GREEN 2
#define LED_BLUE 3
#define LED_WHITE 4
#define LED_YELLOW 5
#define LED_PURPLE 7
#define LED_OFF 0
#define QUICKBLANK 254
#define DONTCHANGE 255

/**
 * Reading serial buffer for commands
 */
#define SER_BUFFER_SIZE 30
char serialBuffer[SER_BUFFER_SIZE];
uint8_t serialBufferPointer = 255;
bool loadSerialCommandToCRNL() {    // Call in loop() to check for commands
  if (serialBufferPointer == 255) { // Initialization
    serialBuffer[0] = 0;            // Null-terminate empty buffer
    serialBufferPointer = 0;
  }

  while (Serial.available()) {
    char c = Serial.read();
    if (c == 10 || c == 13) {
      serialBufferPointer = 0; // so, we can start over again filling the buffer
      return true;
    } else {
      if (serialBufferPointer < SER_BUFFER_SIZE - 1) { // one byte for null termination reserved
        serialBuffer[serialBufferPointer] = c;
        serialBuffer[serialBufferPointer + 1] = 0;
        serialBufferPointer++;
      } else {
        // Serial.println(F("ERROR: Buffer overflow."));
      }
    }
  }

  return false;
}

/**
 * Check incoming serial commands
 */
void clearPresets();
bool checkIncomingSerial() {
  if (loadSerialCommandToCRNL()) {
    if (!strncmp(serialBuffer, "newmac", 6)) {
      EEPROM.write(10, 0x90);
      EEPROM.write(11, 0xA1);
      EEPROM.write(12, 0xDA);
      for (uint8_t i = 0; i < 3; i++) {
        EEPROM.write(13 + i, random(0, 256));
      }
      Serial << F("MAC address updated to: ");
      for (uint8_t i = 0; i < 6; i++) {
        Serial << _HEXPADL(EEPROM.read(i + 10), 2, "0") << (i != 5 ? F(":") : F("\n"));
      }
    } else if (!strncmp(serialBuffer, "HWvar=", 6)) {
      EEPROM.write(9, atoi(serialBuffer + 6));
      Serial << F("HWVar=") << EEPROM.read(9) << F("\n");
      delay(1000);
      resetFunc();
    } else if (!strncmp(serialBuffer, "debug", 5)) {
      EEPROM.write(1, true);
      Serial << F("Reset -> debug mode...\n");
      delay(1000);
      resetFunc();
    } else if (!strncmp(serialBuffer, "configd", 7)) {
      EEPROM.write(0, 2);
      Serial << F("Reset -> config default mode...\n");
      delay(1000);
      resetFunc();
    } else if (!strncmp(serialBuffer, "config", 6)) {
      EEPROM.write(0, 1);
      Serial << F("Reset -> config mode...\n");
      delay(1000);
      resetFunc();
    } else if (!strncmp(serialBuffer, "clearpresets", 12)) {
      clearPresets();
      Serial << F("Presets clear\n");
      delay(1000);
      resetFunc();
    } else if (!strncmp(serialBuffer, "calibrate", 9)) {
      Serial << F("Start calibration\n");
      _calibrateMode = true;
    } else if (!strncmp(serialBuffer, "reset", 5)) {
      Serial << F("Resetting...\n");
      delay(1000);
      resetFunc();
    } else {
      Serial << F("NAK\n");
    }
  }

  return false;
}

bool variantLED() { return EEPROM.read(9) & 1; }

/**
 * StatusLED function. Call it without parameters to just update the LED flashing. Call it with parameters to set a new value.
 */
void statusLED(uint8_t incolor = 255, uint8_t inblnk = 255) {
  static uint8_t color = 0;
  static uint8_t blnk = 0;

  static uint8_t grn = variantLED() ? 14 : 15;
  static uint8_t blu = variantLED() ? 15 : 14;

  if (incolor < 254) {
    color = incolor;
    blnk = 0;
  }
  if (inblnk != 255) {
    blnk = inblnk;
  }

  for (uint8_t i = 0; i < (incolor == QUICKBLANK ? 2 : 1); i++) {
    if ((blnk == 0 || ((millis() & (2048 >> blnk)) > 0)) && !(incolor == QUICKBLANK && i == 0)) {
#if SK_ETHMEGA
      switch (color) {
      case 1:                //  red
      case 7:                //  purple
        digitalWrite(3, !0); // Red
        digitalWrite(2, !1); // Green
        break;
      case 2:                //  green
        digitalWrite(3, !1); // Red
        digitalWrite(2, !0); // Green
        break;
      case 3:                //  blue
      case 4:                //  white
      case 5:                //  yellow
      case 6:                //  cyan
        digitalWrite(3, !0); // Red
        digitalWrite(2, !0); // Green
        break;
      default:               //  off
        digitalWrite(3, !1); // Red
        digitalWrite(2, !1); // Green
        break;
      }
#else
      switch (color) {
      case 1:                 //  red
        digitalWrite(13, 0);  // Red
        digitalWrite(grn, 1); // Green
        digitalWrite(blu, 1); // Blue
        break;
      case 2:                 //  green
        digitalWrite(13, 1);  // Red
        digitalWrite(grn, 0); // Green
        digitalWrite(blu, 1); // Blue
        break;
      case 3:                 //  blue
        digitalWrite(13, 1);  // Red
        digitalWrite(grn, 1); // Green
        digitalWrite(blu, 0); // Blue
        break;
      case 4:                 //  white
        digitalWrite(13, 0);  // Red
        digitalWrite(grn, 0); // Green
        digitalWrite(blu, 0); // Blue
        break;
      case 5:                 //  yellow
        digitalWrite(13, 0);  // Red
        digitalWrite(grn, 0); // Green
        digitalWrite(blu, 1); // Blue
        break;
      case 6:                 //  cyan
        digitalWrite(13, 1);  // Red
        digitalWrite(grn, 0); // Green
        digitalWrite(blu, 0); // Blue
        break;
      case 7:                 //  purple
        digitalWrite(13, 0);  // Red
        digitalWrite(grn, 1); // Green
        digitalWrite(blu, 0); // Blue
        break;
      default:                //  off
        digitalWrite(13, 1);  // Red
        digitalWrite(grn, 1); // Green
        digitalWrite(blu, 1); // Blue
        break;
      }
#endif
    } else {
#if SK_ETHMEGA
      digitalWrite(3, !1); // Red
      digitalWrite(2, !1); // Green
#else
      digitalWrite(13, 1);  // Red
      digitalWrite(grn, 1); // Green
      digitalWrite(blu, 1); // Blue
#endif
    }
    if (incolor == QUICKBLANK) {
      delay(200);
    }
  }
}

/**
 * Prints a small dot every second on serial monitor as a "keep alive signal"
 */
void statusSerial() {
  static unsigned long timer = millis();
  static unsigned long counter = 0;
  static uint8_t printsSinceLastLinebreak = 0;
  counter++;
  if (sTools.hasTimedOut(timer, 1000, true)) {
    Serial << F(".");
    if (debugMode)
      Serial << counter << F("\n");
    printsSinceLastLinebreak++;
    if (printsSinceLastLinebreak >= 60) { // TODO?: We could consider to rather make a line break whenever a minute has passed. If then less than 60 dots is printed, we know the loop() function has been stalled for longer than a second at some point.
      printsSinceLastLinebreak = 0;
      Serial << F("\n");
    }
    counter = 0;
  }
}

/**
 * Use this to make delays (because it will keep important communication running as you wait by calling deviceRunLoop)
 */
void deviceRunLoop(); // Pre-declare for compilation
void lDelay(uint16_t delayVal) {
  unsigned long timer = millis();
  do {
    statusLED();
    // statusSerial();	// Exclude from lDelay because it shouldn't be updating in the delay routine!!
    deviceRunLoop();
  } while (delayVal > 0 && !sTools.hasTimedOut(timer, delayVal));
}

/**
 * Check if config/reset button is pressed
 */
bool isConfigButtonPushed() {
#if SK_ETHMEGA
  return (analogRead(A1) > 500) || (EEPROM.read(0) != 0);
#else
  return (!digitalRead(18)) || (EEPROM.read(0) != 0);
#endif
}

/**
 * Returns config mode
 */
uint8_t configMode = 0;
uint8_t getConfigMode() { return configMode; }

/**
 * Returns default color for given run mode
 */
uint8_t defaultStatusColor() { return getConfigMode() == 1 ? LED_BLUE : (getConfigMode() == 2 ? LED_WHITE : LED_GREEN); }

/**
 * System bits, setting
 */
void setSystemBit(uint8_t bitNum, bool state) {
  if (bitNum < 64) {
    uint8_t theBit = (B1 << (bitNum & 7));
    uint8_t theByte = (bitNum >> 3);
    if (state) {
      _systemBits[theByte] |= theBit;
    } else {
      _systemBits[theByte] &= ~theBit;
    }
  }
}

/**
 * System bits, getting
 */
bool getSystemBit(uint8_t bitNum) { return (_systemBits[(bitNum >> 3)] & (B1 << (bitNum & 7))) > 0; }

/************************************
 *
 * GRAPHICS
 *
 ************************************/

#if SK_HWEN_GRAPHICS
static const uint8_t welcomeGraphics[4][32 * 8] PROGMEM = {{
                                                               B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111100, B00000000, B00000000, B00000000, B11100000, B00011111, B11111100, B00000000, B11100000, B00000000, B00000000, B00000000, B11100000, B00011111, B11111000, B00000001, B11000000, B00000000, B00000000, B00000000, B11100000, B00011111, B11100000, B00000011, B10000000, B00000000, B00000000, B00000000, B11100000, B00011111, B11000000, B00001111, B00000000, B00011111, B11111111, B11111111, B11100000, B00011111, B10000000, B00011111, B00000000, B00111111, B11111111, B11111111, B11100000, B00011110, B00000000, B01111111, B00000000, B00011111, B11111111, B11111111, B11100000, B00011100, B00000000, B11111111, B10000000, B00000000, B00000000, B00111111, B11100000, B00011000, B00000011, B11111111, B10000000, B00000000, B00000000, B00000011, B11100000, B00000000, B00000111, B11111111, B11100000, B00000000, B00000000, B00000000, B11100000, B00000000, B00011111, B11111111, B11111000, B00000000, B00000000, B00000000, B01100000, B00010000, B00000111, B11111111, B11111111, B11111111, B11111100, B00000000, B00100000, B00011000, B00000011, B11111111, B11111111, B11111111, B11111111, B00000000, B00100000, B00011100, B00000001, B11111111, B11111111, B11111111, B11111111, B10000000, B00100000, B00011110, B00000000, B01111111, B11111111, B11111111, B11111111, B00000000, B00100000, B00011111, B10000000, B00111111, B11000000, B00000000, B00000000, B00000000, B00100000, B00011111, B11000000, B00001111, B11000000, B00000000, B00000000, B00000000, B01100000, B00011111, B11100000, B00000111, B11000000, B00000000, B00000000, B00000001, B11100000, B00011111, B11110000, B00000001, B11000000, B00000000, B00000000, B00000111, B11100000, B00011111, B11111100, B00000000, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111,
                                                           },
                                                           {
                                                               B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11110000, B00000111, B11111111, B11111111, B11100000, B00001111, B11111111, B11111111, B11100000, B00000111, B11111111, B11111111, B11000000, B00000111, B11111111, B11111111, B11100000, B00000011, B11111111, B11111111, B11000000, B00000111, B11111111, B11111111, B11000000, B00000001, B11111111, B11111111, B10000000, B00000011, B11111111, B11111111, B10000000, B00000000, B11111111, B11111111, B00000000, B00000001, B11111111, B11111111, B10000000, B00000000, B11111111, B11111111, B00000000, B00000001, B11111111, B11111111, B00000000, B10000000, B01111111, B11111110, B00000001, B00000000, B11111111, B11111110, B00000001, B11000000, B00111111, B11111100, B00000011, B10000000, B01111111, B11111110, B00000001, B11100000, B00011111, B11111100, B00000011, B11000000, B00111111, B11111100, B00000011, B11100000, B00011111, B11111000, B00000111, B11000000, B00111111, B11111000, B00000111, B11110000, B00001111, B11110000, B00001111, B11100000, B00011111, B11110000, B00000000, B00000000, B00000111, B11110000, B00000000, B00000000, B00001111, B11110000, B00000000, B00000000, B00000111, B11100000, B00000000, B00000000, B00001111, B11100000, B00000000, B00000000, B00000011, B11000000, B00000000, B00000000, B00000111, B11000000, B00000000, B00000000, B00000001, B11000000, B00000000, B00000000, B00000011, B11000000, B00111111, B11111110, B00000000, B10000000, B01111111, B11111100, B00000001, B10000000, B01111111, B11111111, B00000000, B00000000, B01111111, B11111110, B00000001, B00000000, B01111111, B11111111, B10000000, B00000000, B11111111, B11111111, B00000000, B00000000, B11111111, B11111111, B10000000, B00000001, B11111111, B11111111, B00000000, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111,
                                                           },
                                                           {
                                                               B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B00000000, B00000000, B00000000, B00011111, B11000000, B00111111, B11111111, B00000000, B00000000, B00000000, B00000000, B00000111, B11000000, B00111111, B11111111, B00000000, B00000000, B00000000, B00000000, B00000011, B11000000, B00111111, B11111111, B00000000, B00000000, B00000000, B00000000, B00000001, B11000000, B00111111, B11111111, B00000000, B00000000, B11111111, B11111100, B00000000, B11000000, B00111111, B11111111, B00000000, B00000000, B11111111, B11111110, B00000000, B11000000, B00111111, B11111111, B00000000, B00000000, B11111111, B11111110, B00000000, B11000000, B00111111, B11111111, B00000000, B00000000, B11111111, B11111100, B00000000, B11000000, B00111111, B11111111, B00000000, B00000000, B00000000, B00000000, B00000001, B11000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000011, B11000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000111, B11000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B01111111, B11000000, B00000000, B00000000, B00000000, B00000000, B11111111, B10000000, B00111111, B11000000, B00111111, B11111111, B00000000, B00000000, B11111111, B11000000, B00011111, B11000000, B00111111, B11111111, B00000000, B00000000, B11111111, B11100000, B00001111, B11000000, B00111111, B11111111, B00000000, B00000000, B11111111, B11110000, B00000011, B11000000, B00111111, B11111111, B00000000, B00000000, B11111111, B11111000, B00000001, B11000000, B00111111, B11111111, B00000000, B00000000, B11111111, B11111100, B00000000, B11000000, B00111111, B11111111, B00000000, B00000000, B11111111, B11111110, B00000000, B01000000, B00111111, B11111111, B00000000, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111,

                                                           },
                                                           {
                                                               B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11100000, B00000000, B00000111, B11111111, B11111111, B11111000, B00000011, B11111111, B00000000, B00000000, B00000000, B11111111, B11111111, B11111000, B00000011, B11111100, B00000000, B00000000, B00000000, B00111111, B11111111, B11111000, B00000011, B11111000, B00000000, B00000000, B00000000, B00011111, B11111111, B11111000, B00000011, B11110000, B00000011, B11111111, B10000000, B00001111, B11111111, B11111000, B00000011, B11100000, B00001111, B11111111, B11100000, B00001111, B11111111, B11111000, B00000011, B11100000, B00001111, B11111111, B11110000, B00000111, B11111111, B11111000, B00000011, B11000000, B00011111, B11111111, B11110000, B00000111, B11111111, B11111000, B00000011, B11000000, B00011111, B11111111, B11110000, B00000111, B11111111, B11111000, B00000011, B11000000, B00011111, B11111111, B11110000, B00000111, B11111111, B11111000, B00000011, B11000000, B00011111, B11111111, B11110000, B00000111, B11111111, B11111000, B00000011, B11000000, B00011111, B11111111, B11110000, B00000111, B11111111, B11111000, B00000011, B11100000, B00011111, B11111111, B11110000, B00000111, B11111111, B11111000, B00000011, B11100000, B00001111, B11111111, B11100000, B00001111, B11111111, B11111000, B00000011, B11110000, B00000011, B11111111, B11000000, B00001111, B11111111, B11100000, B00000111, B11110000, B00000000, B00000000, B00000000, B00010000, B00000000, B00000000, B00000111, B11111100, B00000000, B00000000, B00000000, B00100000, B00000000, B00000000, B00001111, B11111110, B00000000, B00000000, B00000000, B11100000, B00000000, B00000000, B00111111, B11111111, B11000000, B00000000, B00000111, B11100000, B00000000, B00000001, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111,
                                                           }};
// width x height = 128,13
static const uint8_t SKAARHOJLogo[] PROGMEM = {
    B00011111, B11111111, B00111000, B00011111, B00000011, B11000000, B00000011, B11000000, B01111111, B11111100, B00111100, B00000111, B10000001, B11111110, B00000000, B00001111, B01111111, B11111111, B00111000, B00111110, B00000011, B11100000, B00000011, B11100000, B01111111, B11111111, B00111100, B00000111, B10000111, B11111111, B11000000, B00001111, B11111111, B11111111, B00111000, B01111100, B00000111, B11110000, B00000111, B11100000, B01111111, B11111111, B10111100, B00000111, B10011111, B11001111, B11100000, B00001111, B11111000, B00000000, B00111000, B11111000, B00000111, B11110000, B00001111, B11110000, B01111000, B00001111, B10111100, B00000111, B10011110, B00000011, B11100000, B00001111, B11111000, B00000000, B00111001, B11110000, B00001111, B01111000, B00001111, B01110000, B01111000, B00000111, B10111100, B00000111, B10111110, B00000001, B11100000, B00001111, B11111111, B11111100, B00111011, B11100000, B00001111, B01111000, B00011110, B01111000, B01111000, B00011111, B10111111, B11111111, B10111100, B00000001, B11110000, B00001111, B01111111, B11111111, B00111111, B11000000, B00011110, B00111100, B00011110, B00111100, B01111111, B11111111, B00111111, B11111111, B10111100, B00000001, B11110000, B00001111, B00011111, B11111111, B10111011, B11100000, B00011110, B00111100, B00111110, B00111100, B01111111, B11111110, B00111111, B11111111, B10111100, B00000001, B11110000, B00001111, B00000000, B00001111, B10111001, B11110000, B00111111, B11111110, B00111111, B11111110, B01111000, B01111100, B00111100, B00000111, B10111110, B00000001, B11100000, B00001111, B00000000, B00001111, B10111000, B11111000, B00111111, B11111110, B01111111, B11111110, B01111000, B00111110, B00111100, B00000111, B10011110, B00000001, B11100000, B00011111, B01111111, B11111111, B10111000, B01111100, B01111000, B00001111, B01111000, B00001111, B01111000, B00011111, B00111100, B00000111, B10011111, B10000111, B11000000, B00111110, B01111111, B11111111, B00111000, B00111110, B01111000, B00001111, B11110000, B00001111, B01111000, B00001111, B10111100, B00000111, B10001111, B11111111, B10011111, B11111110, B01111111, B11111100, B00111000, B00011111, B11110000, B00000111, B11110000, B00000111, B11111000, B00000111, B10111100, B00000111, B10000001, B11111110, B00011111, B11110000,
};

// width x height = 5,2
static const uint8_t speedGraphic[] PROGMEM = {
    B10101000, B01010000,
};

void writeDisplayTile(Adafruit_GFX &disp, uint8_t x, uint8_t y, uint8_t dispMask, uint8_t shrink = 0, uint8_t size = 0);
void writeDisplayTile(Adafruit_GFX &disp, uint8_t x, uint8_t y, uint8_t dispMask, uint8_t shrink, uint8_t size) {

  uint8_t wShrink = shrink & 1 ? 1 : 0;
  uint8_t hShrink = shrink & 2 ? 1 : 0;
  uint8_t tw = size > 0 ? 128 : 64;

  // Write basics:
  disp.setBoundingBox(x, y, tw, 32); // sets tile area
  disp.fillRect(0, 0, tw, 32, 0);    // Black it out
  disp.setTextWrap(false);
  disp.setBoundingBox(x, y, tw - wShrink, 32 - hShrink); // sets tile area
  // Serial << "Bounding Box (x,y,w,h): " << x << "," << y << "," << (tw - wShrink) << "," << (32 - hShrink) << "\n";

  // Write title:
  bool isTitle = (size > 0 ? strlen(_extRetLong) : strlen(_extRetShort)) > 0;
  if (isTitle) {
    if (_extRetValIsLabel) {
      disp.drawFastHLine(2, 10, tw - wShrink - 4, WHITE), disp.setTextColor(WHITE, BLACK);
    } else {
      disp.fillRoundRect(0, 0, tw - wShrink, 11, 1, WHITE);
      disp.setTextColor(BLACK, WHITE);
    }
    disp.setCursor((tw >> 1) - (size > 0 ? strlen(_extRetLong) : strlen(_extRetShort)) * 3, 2);
    disp.setTextSize(1);
    disp << (size > 0 ? _extRetLong : _extRetShort);
  }

  // Print "Fine" icon:
  if (_extRetFormat & 0x10) {
    disp.drawBitmap(tw - 7 - wShrink, 11, speedGraphic, 5, 2, 1, false);
  }

  // Write values
  disp.setTextColor(WHITE, BLACK);
  disp.setTextSize(_extRetPair > 0 ? 1 : 2);

  for (uint8_t a = 0; a < (_extRetPair > 0 ? 2 : 1); a++) {
    // Convert value to string:
    memset(_strCache, 0, 11);
    switch (_extRetFormat & 0xF) {
    case 1:
#ifdef __arm__ /* Arduino DUE */
#else
      dtostrf((float)_extRetValue[a] / 1000, 4, 2, _strCache); // Need to find alternative for Due Platform.
#endif
      break;
    default:
      itoa(_extRetValue[a], _strCache, 10);
      break;
    }
    // Unit:
    switch (_extRetFormat & 0xF) {
    case 2: // %
      if (strlen(_strCache) < 10) {
        strncpy(_strCache + strlen(_strCache), PSTR("%"), 1);
      }
      break;
    case 3: // db
      if (strlen(_strCache) < 9) {
        strncpy(_strCache + strlen(_strCache), PSTR("db"), 2);
      }
      break;
    case 4: // f
      if (strlen(_strCache) < 10) {
        strncpy(_strCache + strlen(_strCache), PSTR("f"), 1);
      }
      break;
    case 6: // K
      if (strlen(_strCache) < 10) {
        strncpy(_strCache + strlen(_strCache), PSTR("K"), 1);
      }
      break;
    case 7: // BLANK! (nothing)
      memset(_strCache, 0, 11);
      break;
    }

    // Print prefix-strings:
    int xOffset = 0;
    if (strlen(_extRetTxt[a])) {
      if (_extRetPair > 0) {
        xOffset = sTools.shapeInt(strlen(_strCache) ? 2 : (tw >> 1) - strlen(_extRetTxt[a]) * 3, 2, tw);
        disp.setCursor(xOffset, 14 - (isTitle ? 0 : 5) + a * 9 - hShrink);
      } else {
        xOffset = sTools.shapeInt(strlen(_strCache) ? 2 : (tw >> 1) - strlen((_extRetPair == 0 && size == 0) ? _extRetTxtShort : _extRetTxt[a]) * 6, 2, tw);
        disp.setCursor(xOffset, 16 - (isTitle ? 0 : 5) - hShrink);
      }
      disp << ((_extRetPair == 0 && size == 0) ? _extRetTxtShort : _extRetTxt[a]);
    }

    // Print value string:
    if (strlen(_strCache)) {
      if (_extRetPair > 0) {
        disp.setCursor(sTools.shapeInt(tw - 2 - strlen(_strCache) * 6 - wShrink, 0, 100), 14 - (isTitle ? 0 : 5) + a * 9 - hShrink);
        disp << _strCache;
      } else {
        xOffset = strlen(_extRetTxt[a]) ? tw - 2 - strlen(_strCache) * 12 : (tw >> 1) - strlen(_strCache) * 6;
        disp.setCursor(sTools.shapeInt(xOffset, 0, 100), 16 - (isTitle ? 0 : 5) - (_extRetScaleType > 0 ? 2 : 0) - hShrink);
        disp << _strCache;
        if ((_extRetFormat & 0xF) == 5) {
          disp.setTextSize(1);
          disp.setCursor(sTools.shapeInt(xOffset - 12, 0, 100), 13 - (isTitle ? 0 : 5) - hShrink);
          disp << F("1/");
        }
      }
    }

    // Set border: (when pair > 1, 1 = two values but none highlighted)
    if (_extRetPair == a + 2) { // 2+3 equals border around upper/lower respectively
      disp.drawRoundRect(0, 12 - (isTitle ? 0 : 5) + a * 9 - hShrink, tw - wShrink, 11, 1, WHITE);
    } else if (_extRetPair == 4) { // 4= border around both
      disp.drawRoundRect(0, 12 - (isTitle ? 0 : 5) - hShrink, tw - wShrink, 20, 1, WHITE);
    }

    if (a == 0) {
      if (_extRetScaleType > 0 && (_extRetRangeHigh - _extRetRangeLow) != 0) {
        //          disp.drawRoundRect(0, 29, 64, 3, 0, WHITE); // Base rectangle
        disp.drawRoundRect(0, 31 - hShrink, tw, 1, 0, WHITE); // Base rectangle

        uint8_t w = sTools.shapeInt(((float)(_extRetValue[a] - _extRetRangeLow) / (_extRetRangeHigh - _extRetRangeLow) * tw), 0, tw - wShrink);
        if (w > 0) {
          if (_extRetScaleType == 1)
            disp.fillRoundRect(0, 29 - hShrink, w, 3, 0, WHITE); // In-fill
        }
        if (_extRetScaleType == 2)
          disp.fillRoundRect(sTools.shapeInt(w - 1, 0, tw - 3), 29 - hShrink, 3, 3, 0, WHITE); // In-fill

        if (_extRetRangeHigh != _extRetLimitHigh) {
          uint8_t w = sTools.shapeInt(((float)(_extRetLimitHigh - _extRetRangeLow) / (_extRetRangeHigh - _extRetRangeLow) * tw), 0, tw - wShrink);
          disp.fillRoundRect(sTools.shapeInt(w, 0, tw - 1 - wShrink), 28 - hShrink, 1, 3, 0, WHITE); // In-fill
        }
        if (_extRetRangeLow != _extRetLimitLow) {
          uint8_t w = sTools.shapeInt(((float)(_extRetLimitLow - _extRetRangeLow) / (_extRetRangeHigh - _extRetRangeLow) * tw), 0, tw - wShrink);
          disp.fillRoundRect(sTools.shapeInt(w, 0, tw - 1 - wShrink), 28 - hShrink, 1, 3, 0, WHITE); // In-fill
        }
      }
    }
  }
  if (_extRetValIsLabel) {
    disp.drawRoundRect(0, 0, tw - wShrink, 32 - hShrink, 1, WHITE);
  }
  if (dispMask)
    disp.display(dispMask);
}
#endif
#if (SK_HWEN_MENU)
void write3x16Display(SkaarhojEADOGMDisplay &disp);
void write3x16Display(SkaarhojEADOGMDisplay &disp) {

  disp.gotoRowCol(0, 0);

  // Write title:
  disp << _extRetLong;
  for (uint8_t a = strlen(_extRetLong); a < 16; a++) {
    disp << F(" ");
  }

  // Print "Fine" icon:
  if (_extRetFormat & 0x10) {
    disp.gotoRowCol(0, 15);
    disp << F("~");
  }

  // Write values
  for (uint8_t a = 0; a < (_extRetPair > 0 ? 2 : 1); a++) {
    // Convert value to string:
    memset(_strCache, 0, 11);
    switch (_extRetFormat & 0xF) {
    case 1:
#ifdef __arm__ /* Arduino DUE */
#else
      dtostrf((float)_extRetValue[a] / 1000, 4, 2, _strCache);
#endif
      break;
    default:
      itoa(_extRetValue[a], _strCache, 10);
      break;
    }
    // Unit:
    switch (_extRetFormat & 0xF) {
    case 2: // %
      if (strlen(_strCache) < 10) {
        strncpy(_strCache + strlen(_strCache), PSTR("%"), 1);
      }
      break;
    case 3: // db
      if (strlen(_strCache) < 9) {
        strncpy(_strCache + strlen(_strCache), PSTR("db"), 2);
      }
      break;
    case 4: // f
      if (strlen(_strCache) < 10) {
        strncpy(_strCache + strlen(_strCache), PSTR("f"), 1);
      }
      break;
    case 6: // K
      if (strlen(_strCache) < 10) {
        strncpy(_strCache + strlen(_strCache), PSTR("K"), 1);
      }
      break;
    case 7: // BLANK! (nothing)
      memset(_strCache, 0, 11);
      break;
    }

    uint8_t m = _extRetPair == a + 2 || _extRetPair == 4 ? 1 : 0;

    disp.gotoRowCol(a + 1, 0);
    if (m)
      disp << F(">");
    disp << _extRetTxt[a];

    for (uint8_t b = strlen(_extRetTxt[a]) + m; b < 16; b++) {
      disp << F(" ");
    }

    if (strlen(_strCache)) {
      disp.gotoRowCol(a + 1, 16 - strlen(_strCache));
      disp << _strCache;
      if ((_extRetFormat & 0xF) == 5) {
        disp.gotoRowCol(a + 1, 16 - strlen(_strCache) - 2);
        disp << F("1") << F("/");
      }
    }
  }
  if (_extRetPair == 0) {
    disp.gotoRowCol(2, 0);
    for (uint8_t a = 0; a < 16; a++) {
      disp << F(" ");
    }
  }
}
#endif
void testGenerateExtRetVal(uint8_t seed) {

  if (seed >= 64 && seed <= 64 + 7) {
    extRetVal(0, 7); // Don't show value, just init
    if (seed & 0x4) {
      extRetValShortLabel(PSTR("Title "));
      extRetValShortLabel(PSTR("Title "));
      extRetValLongLabel(PSTR("Title "));
      extRetValLongLabel(PSTR(" line can be this long"));
    }
    if ((seed & 0x1) > 0) {
      extRetValTxt_P(PSTR("First Long Textline"), 0);
    }
    if ((seed & 0x2) > 0) {
      extRetValTxt_P(PSTR("Second Long Textline"), 1);
      extRetVal2(0); // Must set to activate the "pair" parameter, otherwise we will not see two text lines.
    }
  } else {
    if (seed % 128 < 8) {
      extRetVal(random(-110, 111));
      extRetValScale(((seed & 4) ? 1 : 2), -100, 100, (seed & 1) ? -50 : -100, (seed & 2) ? 50 : 100);
    } else {
      extRetVal(random(-2000, 2111), seed % 8, seed & 1);
    }
    extRetValShortLabel(PSTR("Test "), seed);
    extRetValLongLabel(PSTR("Test format "), seed);
    extRetValLongLabel(PSTR(" shown now on display"));
    if (seed % 16 > 8 && seed % 16 < 16) {
      extRetVal2(random(-110, 111), seed % 64 >= 32 ? (seed % 4) + 1 : 1);
    }
    if (seed % 32 >= 16) {
      if (seed % 64 < 32 || seed & 0x1)
        extRetValTxt_P(PSTR("X:"), 0);
      if (seed % 64 < 32 || seed & 0x2)
        extRetValTxt_P(PSTR("Y:"), 1);
    }
  }
}

/************************************
 *
 * CONFIG MEMORY HANDLING
 *
 ************************************/

/**
 * Returns index to position in configuration array where a given Hardware Component state behaviour is described.
 */
uint16_t getConfigMemIndex(uint8_t HWcIdx, uint8_t stateIdx = 0) {

  uint16_t ptr = 2;
  int HWcIndex = -1;

  while (ptr < SK_CONFIG_MEMORY_SIZE && globalConfigMem[ptr] != 255) { // Traverses HW components
    uint8_t HWcSegmentLength = globalConfigMem[ptr];
    uint16_t HWcSegmentStartPtr = ptr;
    HWcIndex++;
    int stateIndex = -1;

    if (HWcIdx == HWcIndex) { // Found it...
      ptr++;
      while (ptr < HWcSegmentStartPtr + HWcSegmentLength + 1) { // Traverses state index
        uint8_t stateSegmentLength = globalConfigMem[ptr];
        uint16_t stateSegmentStartPtr = ptr;
        stateIndex++;

        if (stateIdx == stateIndex) { // If state index matches, return either state behaviour configuration - or the default (Normal) behaviour
          return stateSegmentLength > 0 ? stateSegmentStartPtr + 1 : HWcSegmentStartPtr + 2;
        } else {
          ptr += stateSegmentLength + 1;
        }
      }

      return HWcSegmentStartPtr + 2;
    } else {
      ptr += HWcSegmentLength + 1;
    }
  }
  return 0;
}

/**
 * Returns index to position in configuration array where the controller IP and subnet mask is stored
 */
uint16_t getConfigMemIPIndex() {
  uint16_t ptr = globalConfigMem[0];
  return ((ptr << 8) | globalConfigMem[1]) + 2;
}

/**
 * Returns index to position in configuration array where a given device is configured
 */
uint16_t getConfigMemDevIndex(uint8_t devNum) {
  uint16_t ptr = globalConfigMem[0];
  return ((ptr << 8) | globalConfigMem[1]) + 2 + 8 + 1 + devNum * 5;
}

/**
 * Returns index to position in configuration array where number of states is stored (and subsequently labels are stored)
 */
uint16_t getConfigMemStateIndex() {
  uint16_t ptr = globalConfigMem[0];
  return ((ptr << 8) | globalConfigMem[1]) + 2 + 8 + 1 + (SK_DEVICES * 5);
}

/**
 * Returns index to position in configuration array where preset label is stored
 */
uint16_t getConfigMemPresetTitleIndex() {
  uint16_t ptr = getConfigMemStateIndex();
  return ptr + globalConfigMem[ptr] + 2;
}

// Preset memory: [NumberofPresets][CurrentPreset][PresetStoreLen-MSB][PresetStoreLen-LSB][CSC of previous 4 bytes + token][Preset1][Preset2][Preset...][PresetX], where [PresetX] = [Len-MSB][Len-LSB][Byte1][Byte2][Byte...][ByteX][CSC of bytes + token]
// On a global level, we can verify the preset store by checking if a traversal of the presets and lengths will match the global length
// On an individual level, presets validity is confirmed by the xor-checksum byte stored prior to the preset data
// EEPROM memory layout:
/*
 * 0: Boot in Config mode flag. (If 1 on boot, config mode, if 2 on boot, config default mode. Is always reset back to zero )
 * 1: Boot in debug mode flag. (Is always reset back to zero)
 * 2-5: Not used in UniSketch, but in SKAARDUINO TestRig sketch, this is the IP address of the SKAARDUINO. Reserved.
 * 9: HW Variant: bit0: If set, swaps green and blue status LED pins.
 * 10-16: MAC address (+ checksum on 16, used in SKAARDUINO TestRig sketch only)
 * EEPROM_PRESET_START: Start of presets
 */
#define EEPROM_PRESET_START 100
#define EEPROM_PRESET_TOKEN 0x24 // Just some random value that is used for a checksum offset. Change this and existing configuration will be invalidated and has to be rewritten.
void loadDefaultConfig() {
  // Copy default controller configuration to the global Config memory.
  memcpy_P(globalConfigMem, defaultControllerConfig, sizeof(defaultControllerConfig));
}
void moveEEPROMMemoryBlock(uint16_t from, uint16_t to, int offset) { // From is inclusive, To is exclusive, To>From, offset>0 = move forward
                                                                     //	Serial << "moveEEPROMMemoryBlock (" << from << "," << to << "," << offset << " )\n";
  if (offset > 0) {
    for (uint16_t a = to; a > from; a--) {
      EEPROM.write(a - 1 + offset, EEPROM.read(a - 1));
    }
  } else if (offset < 0) {
    for (uint16_t a = from; a < to; a++) {
      EEPROM.write(a + offset, EEPROM.read(a));
    }
  }
}
uint16_t getPresetOffsetAddress(uint8_t presetNum) {
  uint16_t offset = 0;
  for (uint8_t a = 1; a < presetNum; a++) {
    offset += ((uint16_t)EEPROM.read(EEPROM_PRESET_START + 5 + offset) << 8) | EEPROM.read(EEPROM_PRESET_START + 6 + offset) + 2; // Length of stored preset + 2 for addressing
  }
  return offset;
}
void updatePresetChecksum() {
  uint8_t csc = EEPROM_PRESET_TOKEN;
  for (uint8_t a = 0; a < 4; a++) {
    csc ^= EEPROM.read(EEPROM_PRESET_START + a);
  }
  //	Serial << "Writing CSC:" << csc << "\n";
  EEPROM.write(EEPROM_PRESET_START + 4, csc);
}
void setNumberOfPresets(uint8_t n) {
  EEPROM.write(EEPROM_PRESET_START, n); // Number of...
                                        //	Serial << "setNumberOfPresets:" << n << "\n";
  updatePresetChecksum();
}
void setCurrentPreset(uint8_t n) {
  EEPROM.write(EEPROM_PRESET_START + 1, n); // Current
                                            //	Serial << "setCurrentPreset:" << n << "\n";
  updatePresetChecksum();
}
void setPresetStoreLength(uint16_t len) {
  EEPROM.write(EEPROM_PRESET_START + 2, highByte(len));
  EEPROM.write(EEPROM_PRESET_START + 3, lowByte(len));
  //	Serial << "setPresetStoreLength:" << len << "\n";
  updatePresetChecksum();
}
uint8_t getNumberOfPresets() {
  uint8_t csc = EEPROM_PRESET_TOKEN;
  for (uint8_t a = 0; a < 5; a++) {
    csc ^= EEPROM.read(EEPROM_PRESET_START + a);
  }
  if (csc != 0) {
    Serial << F("CSC mismatch:") << csc << F("\n");
    clearPresets();
  }

  return EEPROM.read(EEPROM_PRESET_START);
}
void clearPresets() {
  setNumberOfPresets(0);
  setCurrentPreset(0);
  setPresetStoreLength(0);
}
void getPresetName(char *buf, uint8_t preset) {
  buf[0] = 0;
  if (getNumberOfPresets() > 0) {
    uint16_t base = EEPROM_PRESET_START + getPresetOffsetAddress(preset) + 7;
    uint16_t ptr = EEPROM.read(base);
    ptr = ((ptr << 8) | EEPROM.read(base + 1)) + 2 + 8 + 1 + (SK_DEVICES * 5);
    ptr = ptr + EEPROM.read(base + ptr) + 2;
    uint8_t a = 0;
    do {
      buf[a] = (char)EEPROM.read(base + ptr + a);
      a++;
    } while (EEPROM.read(base + ptr + a - 1) != 0 && a < 22);
    if (buf[0] == 0) {
      buf[0] = 'P';
      buf[1] = 'r';
      buf[2] = 'e';
      buf[3] = 's';
      buf[4] = 'e';
      buf[5] = 't';
      buf[6] = ' ';
      buf[7] = 48 + preset;
      buf[8] = 0;
    }
  }
}
void getStateName(char *buf, uint8_t state) {
  buf[0] = 0;

  uint16_t ptr = getConfigMemStateIndex();
  uint8_t nStates = globalConfigMem[ptr + 1];

  for (uint8_t a = 0; a < nStates; a++) {
    if (state == a) {
      strncpy(buf, ((char *)&globalConfigMem[ptr + 2]), 21);
      break;
    }
    ptr += strlen((char *)&globalConfigMem[ptr + 2]) + 1;
  }
}
bool loadPreset(uint8_t presetNum = 0) {
  //	Serial << "Load preset: " << presetNum << "\n";
  if (getNumberOfPresets() > 0) { // If there are valid presets, consider to load one...
                                  //		Serial << "There are valid presets, in fact "<< getNumberOfPresets() << " are found.\n";
    if (presetNum == 0) {         // Load current preset if nothing is given
      presetNum = EEPROM.read(EEPROM_PRESET_START + 1);
      //			Serial << "Current preset: "<< presetNum << "\n";
    }
    //		Serial << "Preset: "<< presetNum << "\n";
    if (presetNum > 0 && presetNum <= getNumberOfPresets()) { // preset num must be equal to or less than the total number
      uint16_t presetOffset = getPresetOffsetAddress(presetNum);
      //			Serial << "presetOffset: "<< presetOffset << "\n";
      uint16_t presetLen = ((uint16_t)EEPROM.read(EEPROM_PRESET_START + 5 + presetOffset) << 8) | EEPROM.read(EEPROM_PRESET_START + 6 + presetOffset);
      //			Serial << "presetLen: "<< presetLen << "\n";
      uint8_t csc = EEPROM_PRESET_TOKEN;
      for (uint16_t a = 0; a < presetLen; a++) {
        if (a >= SK_CONFIG_MEMORY_SIZE)
          break;                                                                      // ERROR!
        globalConfigMem[a] = EEPROM.read(EEPROM_PRESET_START + 7 + presetOffset + a); // Loading memory with preset, byte by byte.
        csc ^= globalConfigMem[a];
      }
      if (csc == 0) {
        //				Serial << "All good...\n";
        // All is good, exit with the new config in memory!
        Serial << F("Preset ") << presetNum << F(" loaded\n");
        return true;
      }
    }
  }
  //	Serial << "Load default config...";
  loadDefaultConfig();
  return false;
}
uint16_t getPresetStoreLength() { return ((uint16_t)EEPROM.read(EEPROM_PRESET_START + 2) << 8) | EEPROM.read(EEPROM_PRESET_START + 3); }
void savePreset(uint8_t presetNum, uint16_t len) { // Len is excluding CSC byte. presetNum=255 to make new preset.
                                                   //		Serial << "savePreset() \n";
  if (getNumberOfPresets() > 0) {
    if (presetNum == 0) { // Load current preset if nothing is given
      presetNum = EEPROM.read(EEPROM_PRESET_START + 1);
    }
  } else {
    presetNum = 255; // New preset initiated
  }
  uint16_t presetOffset;
  if (presetNum > 0) {
    if (presetNum > getNumberOfPresets()) {
      if (len > 0) {
        presetNum = getNumberOfPresets() + 1; // New preset
                                              //				Serial << "New preset: " << presetNum << "\n";
        setNumberOfPresets(presetNum);
        setCurrentPreset(presetNum);
        presetOffset = getPresetOffsetAddress(presetNum);
        //				Serial << "New presetOffset: " << presetOffset << "\n";

        setPresetStoreLength(presetOffset + len + 1 + 2);
        //	Serial << "getPresetStoreLength(): " << getPresetStoreLength() << "\n";
      }
    } else {
      setCurrentPreset(presetNum);
      // Serial << "Existing preset: " << presetNum << "\n";
      presetOffset = getPresetOffsetAddress(presetNum);
      //		Serial << "New presetOffset: " << presetOffset << "\n";
      uint16_t nextPresetOffset = getPresetOffsetAddress(presetNum + 1);
      //		Serial << "nextPresetOffset: " << nextPresetOffset << "\n";
      uint16_t presetLen = ((uint16_t)EEPROM.read(EEPROM_PRESET_START + 5 + presetOffset) << 8) | EEPROM.read(EEPROM_PRESET_START + 6 + presetOffset); // Includes CSC byte
      if (len > 0) {
        moveEEPROMMemoryBlock(EEPROM_PRESET_START + 5 + nextPresetOffset, EEPROM_PRESET_START + 5 + getPresetStoreLength(), (len + 1) - presetLen);
        setPresetStoreLength(getPresetStoreLength() + (len + 1) - presetLen);
      } else {
        // Serial << "Deleting a preset \n";
        moveEEPROMMemoryBlock(EEPROM_PRESET_START + 5 + nextPresetOffset, EEPROM_PRESET_START + 5 + getPresetStoreLength(), -presetLen - 2);
        setPresetStoreLength(getPresetStoreLength() - presetLen - 2);
        setNumberOfPresets(getNumberOfPresets() - 1);
        setCurrentPreset(presetNum - 1);
      }
    }
    if (len > 0) {
      // Store memory:
      uint8_t csc = EEPROM_PRESET_TOKEN;
      for (uint16_t a = 0; a < len; a++) {
        EEPROM.write(EEPROM_PRESET_START + 7 + presetOffset + a, globalConfigMem[a]); // Loading memory with preset, byte by byte.
        csc ^= globalConfigMem[a];
      }
      EEPROM.write(EEPROM_PRESET_START + 7 + presetOffset + len, csc); // Checksum byte
      EEPROM.write(EEPROM_PRESET_START + 5 + presetOffset, highByte(len + 1));
      EEPROM.write(EEPROM_PRESET_START + 6 + presetOffset, lowByte(len + 1));
    }
  }
}
/*
void presetCheck()	{
        Serial << "Number of presets: " << EEPROM.read(EEPROM_PRESET_START+0) << "\n";
        Serial << "Current preset: " << EEPROM.read(EEPROM_PRESET_START+1) << "\n";
        uint16_t accumLength = 0;
        for(uint8_t a=0; a<EEPROM.read(EEPROM_PRESET_START+0); a++)	{
                Serial << "Preset: " << (a+1)<< "\n";
                uint16_t presetOffset = getPresetOffsetAddress(a+1);
                Serial << "Preset offset: " << presetOffset << "\n";
                uint16_t length = ((EEPROM.read(EEPROM_PRESET_START+5+presetOffset)<<8)|EEPROM.read(EEPROM_PRESET_START+6+presetOffset));
                Serial << "Length " << length << ":\n";
                accumLength+=length+2;

                uint8_t csc=EEPROM_PRESET_TOKEN;
                for(uint16_t b=0; b<length;b++)	{
                        csc^=EEPROM.read(EEPROM_PRESET_START+7+presetOffset+b);
                }
                Serial << "csc: 0=" << csc << "\n";
        }
        Serial << "Pointer at: " << accumLength << "\n";
        Serial << "Total Length of preset store: " << getPresetStoreLength() << "\n";
}
*/

/************************************
 *
 * DEVICES SETUP
 *
 ************************************/
uint8_t deviceMap[sizeof(deviceArray)]; // Maps indexes of the individual device arrays. (first index 0 unused)
uint8_t deviceEn[sizeof(deviceArray)];  // Sets the enabled-flag for the devices. (first index 0 unused)
bool devicesStopped = false;
uint8_t deviceReady[sizeof(deviceArray)]; // Flag that lets the system know if a device is connected and ready for commands. (first index 0 unused)
IPAddress deviceIP[sizeof(deviceArray)];  // Flag that lets the system know if a device is connected and ready for commands. (first index 0 unused)

/**
 * Set up devices
 */
uint8_t getDevUnconnected() {
  uint8_t c = 0;
  for (uint8_t a = 1; a < sizeof(deviceArray); a++) {
    if (deviceEn[a] && !deviceReady[a])
      c++;
  }
  return c;
}

/**
 * Set up devices
 * ...Call ONLY once - or we will overflow memory...
 */
void deviceSetup() {

  W5100.setRetransmissionTime(1000); // Milli seconds
  W5100.setRetransmissionCount(2);

  for (uint8_t a = 1; a < sizeof(deviceArray); a++) {
    deviceReady[a] = 0;

    uint16_t ptr = getConfigMemDevIndex(a - 1);
    deviceEn[a] = globalConfigMem[ptr];
    for (uint8_t b = 0; b < 4; b++) {
      deviceIP[a][b] = globalConfigMem[ptr + 1 + b];
    }
    if (deviceEn[a]) {
      Serial << F("Setup HW#") << a;
      switch (pgm_read_byte_near(deviceArray + a)) {
      case SK_DEV_ATEM:
#if SK_DEVICES_ATEM
        Serial << F(": ATEM") << AtemSwitcher_initIdx;
        deviceMap[a] = AtemSwitcher_initIdx++;
        AtemSwitcher[deviceMap[a]].begin(deviceIP[a]);
        AtemSwitcher[deviceMap[a]].serialOutput(debugMode);
#endif
        break;
      case SK_DEV_HYPERDECK:
#if SK_DEVICES_HYPERDECK
        Serial << F(": HYPERDECK") << HyperDeck_initIdx;
        deviceMap[a] = HyperDeck_initIdx++;
        HyperDeck[deviceMap[a]].begin(deviceIP[a]);
        HyperDeck[deviceMap[a]].serialOutput(debugMode);
#endif
        break;
      case SK_DEV_VIDEOHUB:
#if SK_DEVICES_VIDEOHUB
        Serial << F(": VIDEOHUB") << VideoHub_initIdx;
        deviceMap[a] = VideoHub_initIdx++;
        VideoHub[deviceMap[a]].begin(deviceIP[a]);
        VideoHub[deviceMap[a]].serialOutput(debugMode);
#endif
        break;
      case SK_DEV_SMARTSCOPE:
#if SK_DEVICES_SMARTSCOPE
        Serial << F(": SMARTSCOPE") << SmartView_initIdx;
        deviceMap[a] = SmartView_initIdx++;
        SmartView[deviceMap[a]].begin(deviceIP[a]);
        SmartView[deviceMap[a]].serialOutput(debugMode);
#endif
        break;
        case SK_DEV_BMDCAMCTRL:
  #if SK_DEVICES_BMDCAMCTRL
          Serial << F(": BMDCAMCONTRL") << BMDCamCtrl_initIdx;
          deviceMap[a] = BMDCamCtrl_initIdx++;
          BMDCamCtrl[deviceMap[a]].begin(deviceIP[a]);	// TODO doesn't make sense
          BMDCamCtrl[deviceMap[a]].serialOutput(debugMode);
  #endif
          break;
		
      }
      Serial << F(", IP=") << deviceIP[a] << F("\n");
    }
  }
}

/**
 * Device run loop
 */
void deviceRunLoop() {
  // Keeping up the connections:
  for (uint8_t a = 1; a < sizeof(deviceArray); a++) {
    if (deviceEn[a]) {
      switch (pgm_read_byte_near(deviceArray + a)) {
      case SK_DEV_ATEM:
#if SK_DEVICES_ATEM
        AtemSwitcher[deviceMap[a]].runLoop();
        deviceReady[a] = AtemSwitcher[deviceMap[a]].hasInitialized();
#endif
        break;
      case SK_DEV_HYPERDECK:
#if SK_DEVICES_HYPERDECK
        HyperDeck[deviceMap[a]].runLoop();
        deviceReady[a] = HyperDeck[deviceMap[a]].hasInitialized();
#endif
        break;
      case SK_DEV_VIDEOHUB:
#if SK_DEVICES_VIDEOHUB
        VideoHub[deviceMap[a]].runLoop();
        deviceReady[a] = VideoHub[deviceMap[a]].hasInitialized();
#endif
        break;
      case SK_DEV_SMARTSCOPE:
#if SK_DEVICES_SMARTSCOPE
        SmartView[deviceMap[a]].runLoop();
        deviceReady[a] = SmartView[deviceMap[a]].hasInitialized();
#endif
        break;
      }
    }
  }
}

/************************************
 *
 * HARDWARE SETUP and TEST
 *
 ************************************/
/**
 * Hardware Setup
 */
uint8_t HWsetup() {
  uint8_t retVal = 0;

  retVal = HWsetupL();

// ++++++++++++++++++++++
// Generic HW feature
// ++++++++++++++++++++++
#if (SK_HWEN_STDOLEDDISPLAY)
  Serial << F("Init Info OLED Display\n");
#if SK_MODEL == SK_MICROMONITOR || SK_MODEL == SK_RCP
  infoDisplay.begin(0, 1);
#else
  infoDisplay.begin(4, 1);
#endif
#if SK_MODEL == SK_RCP || SK_MODEL == SK_MICROMONITOR
  infoDisplay.setRotation(2);
#endif
  for (uint8_t i = 0; i < 8; i++) {
    infoDisplay.clearDisplay();
    infoDisplay.drawBitmap(0, -7 + i, SKAARHOJLogo, 128, 13, 1, false);
    infoDisplay.display(B1); // Write to all
  }
  infoDisplay.setTextSize(1);
  infoDisplay.setTextColor(1, 0);
  infoDisplay.setCursor(40, 24);
#if SK_MODEL == SK_E21SLD
  infoDisplay << "E21-SLD";
#elif SK_MODEL == SK_E21KP01
  infoDisplay << "E21-KP01";
#elif SK_MODEL == SK_E21SSW
  infoDisplay << "E21-SSW";
#elif SK_MODEL == SK_E21CMB6
  infoDisplay << "E21-CMB6";
#elif SK_MODEL == SK_RCP
  infoDisplay << "RCP";
#endif
  infoDisplay.display(B1); // Write to all
  statusLED(QUICKBLANK);
#endif
#if (SK_HWEN_SLIDER)
  Serial << F("Init Slider\n");
  slider.uniDirectionalSlider_init();
  slider.uniDirectionalSlider_hasMoved();
  statusLED(QUICKBLANK);
#endif
#if (SK_HWEN_JOYSTICK)
  Serial << F("Init Joystick\n");
  joystick.joystick_init(10, 0);
  statusLED(QUICKBLANK);
#endif
#if (SK_HWEN_MENU)
  Serial << F("Init Encoder Menu\n");
  menuDisplay.begin(4, 0, 3); // DOGM163
  menuDisplay.cursor(false);
  menuDisplay.contrast(5);
  menuDisplay.print("SKAARHOJ");
  menuEncoders.begin(5);
  //  menuEncoders.serialOutput(debugMode);
  menuEncoders.setStateCheckDelay(250);
  statusLED(QUICKBLANK);
#endif
#if (SK_HWEN_SSWMENU)
  Serial << F("Init SmartSwitch Menu\n");
#if (SK_MODEL == SK_C90SM)
  SSWmenu.begin(2);
  SSWmenuEnc.begin(2);
  SSWmenuChip.begin(2);
#elif(SK_MODEL == SK_E201M16)
  SSWmenu.begin(6);
  SSWmenuEnc.begin(6);
  SSWmenuChip.begin(6);
#else
  SSWmenu.begin(0);
  SSWmenuEnc.begin(0);
  SSWmenuChip.begin(0);
#endif
  // SSWmenuEnc.serialOutput(SK_SERIAL_OUTPUT);
  SSWmenuEnc.setStateCheckDelay(250);

  SSWmenuChip.inputOutputMask((B11111111 << 8) | B11101111);      // Set up input and output pins [GPA7-GPA0,GPB7-GPB0]
  SSWmenuChip.inputPolarityMask((B00010000 << 8) | B00000000);    // Reverse polarity for inputs.
  SSWmenuChip.setGPInterruptEnable((B00000001 << 8) | B00000001); // Set up which pins triggers interrupt (GPINTEN)

#if (SK_MODEL == SK_MICROSMARTE)
  SSWmenu.setRotation(0);
#else
  SSWmenu.setRotation(2);
#endif
  SSWmenu.setButtonBrightness(7, B00010000);
  SSWmenu.setButtonBrightness(7, B00010000);
  SSWmenu.setButtonColor(0, 2, 3, B00010000);
  for (uint8_t i = 0; i < 64; i++) {
    SSWmenu.clearDisplay();
    SSWmenu.drawBitmap(64 - i - 1, 0, welcomeGraphics[0], 64, 32, 1, true);
    SSWmenu.display(B00010000); // Write
  }
  statusLED(QUICKBLANK);
#endif
#if (SK_HWEN_SSWBUTTONS)
  Serial << F("Init SmartSwitch Buttons\n");
#if (SK_MODEL == SK_MICROSMARTH || SK_MODEL == SK_MICROSMARTV)
  SSWbuttons.begin(0);
#else
  SSWbuttons.begin(1); // SETUP: Board address
#endif
#if (SK_MODEL == SK_MICROSMARTV)
  SSWbuttons.setRotation(2);
#else
  SSWbuttons.setRotation(0);
#endif
  SSWbuttons.setButtonBrightness(7, B11);
  SSWbuttons.setButtonBrightness(7, B11);
  SSWbuttons.setButtonColor(0, 2, 3, B11);
  for (uint8_t i = 0; i < 64; i++) {
    SSWbuttons.clearDisplay();
    SSWbuttons.drawBitmap(64 - i - 1, 0, welcomeGraphics[0], 64, 32, 1, true);
    SSWbuttons.display(B10); // Write
    SSWbuttons.clearDisplay();
    SSWbuttons.drawBitmap(-(64 - i - 1), 0, welcomeGraphics[1], 64, 32, 1, true);
    SSWbuttons.display(B01); // Write
  }
  statusLED(QUICKBLANK);
#endif
#if (SK_HWEN_ACM)
  Serial << F("Init Audio Master Control\n");
#if (SK_MODEL == SK_C90A)
  AudioMasterControl.begin(5, 0);
#elif(SK_MODEL == SK_MICROLEVELS)
//  AudioMasterControl.begin(0, 0);	// MICROLEVELS NOT FINISHED - TODO
#else
  AudioMasterControl.begin(3, 0);
#endif
  AudioMasterControl.setIsMasterBoard();
  AudioMasterPot.uniDirectionalSlider_init(10, 35, 35, 0, 0);
  AudioMasterPot.uniDirectionalSlider_disableUnidirectionality(true);

  if (getConfigMode()) {
    Serial << F("Test: AudioMasterControl\n");
    for (int j = 0; j < 5; j++) {
      AudioMasterControl.setButtonLight(j + 1, true);
      delay(100);
    }
    for (int j = 0; j < 5; j++) {
      AudioMasterControl.setButtonLight(j + 1, false);
      delay(100);
    }
    for (int i = 1; i <= 2; i++) {
      for (int j = 0; j < 4; j++) {
        AudioMasterControl.setChannelIndicatorLight(i, j);
        delay(100);
      }
    }
    AudioMasterControl.setChannelIndicatorLight(1, 0);
    AudioMasterControl.setChannelIndicatorLight(2, 0);
    for (uint16_t i = 0; i <= 16445; i += 100) {
      AudioMasterControl.VUmeter(16445 - i, i);
    }
    AudioMasterControl.VUmeter(16445, 16445);
    delay(500);
    AudioMasterControl.VUmeter(0, 0);
  } else
    delay(500);
  statusLED(QUICKBLANK);
#endif
#if (SK_HWEN_GPIO)
  Serial << F("Init GPIO board\n");
#if (SK_MODEL == SK_C90SM)
  GPIOboard.begin(7);
#else
  GPIOboard.begin(0);
#endif

  /*  if (getConfigMode()) {	// Cannot allow this in normal mode!
      Serial << F("Test: GPIO relays\n");
      // Set:
      for (int i = 1; i <= 8; i++) {
        GPIOboard.setOutput(i, HIGH);
        delay(100);
      }
      // Clear:
      for (int i = 1; i <= 8; i++) {
        GPIOboard.setOutput(i, LOW);
        delay(100);
      }
    }*/
  statusLED(QUICKBLANK);
#endif

  return retVal;
}

/**
 * Hardware Test
 */
void HWtest() {
  bool displayWritten = false;
  if (millis() > 120000 || getConfigMode() == 2) {

    HWtestL();

#if (SK_HWEN_STDOLEDDISPLAY)
    if ((millis() & 0x4000) == 0x4000) {
      // infoDisplay.testProgramme(B1);
      testGenerateExtRetVal(millis() >> 11);
      writeDisplayTile(infoDisplay, 0, 0, B1, 0, 1);

      displayWritten = true;
    }
#endif
#if (SK_HWEN_SLIDER)
    if (slider.uniDirectionalSlider_hasMoved()) {
      Serial << F("New position ") << slider.uniDirectionalSlider_position() << F("\n");
      if (slider.uniDirectionalSlider_isAtEnd()) {
        Serial << F("Slider at end\n");
      }
    }
#endif
#if (SK_HWEN_JOYSTICK)
    if (joystick.joystick_hasMoved(0) || joystick.joystick_hasMoved(1)) {
      Serial << F("New joystick position:") << joystick.joystick_position(0) << "," << joystick.joystick_position(1) << F("\n");
    }
    if (joystick.joystick_buttonUp()) {
      Serial << F("Joystick Button Up\n");
    }
    if (joystick.joystick_buttonDown()) {
      Serial << F("Joystick Button Down\n");
    }
    if (joystick.joystick_buttonIsPressed()) {
      Serial << F("Joystick Button Pressed\n");
    }
#endif
#if (SK_HWEN_SSWMENU)
    static uint8_t SSWmenuEncValueSum = 0;
    static bool SSWmenuEncX = false;
    int SSWmenuEncValue = SSWmenuEnc.state(0, 1000);
    switch (SSWmenuEncValue) {
    case 1:
    case -1:
      SSWmenuEncValueSum += SSWmenuEnc.lastCount(0, 2) * (1 + SSWmenuEncX * 4);
      break;
    case 2:
      SSWmenuEncX = !SSWmenuEncX;
      break;
    default:
      if (SSWmenuEncValue > 1000)
        SSWmenuEncValueSum = 0; // Hold
      break;
    }

    if (!SSWmenuEncValueSum) {
      if ((millis() & 0x4000) == 0x4000) {
        testGenerateExtRetVal(millis() >> 11);
        writeDisplayTile(SSWmenu, 0, 0, B10000);
      } else {
        SSWmenu.testProgramme(B10000);
      }
    } else {
      static uint16_t prevHash = 0;
      extRetVal(SSWmenuEncValueSum);
      extRetValShortLabel(PSTR("TEST Enc"));
      if (prevHash != extRetValHash()) {
        prevHash = extRetValHash();
        writeDisplayTile(SSWmenu, 0, 0, B10000);
        Serial << F("Write SSWMenu Display\n");
      }
    }

#endif
#if (SK_HWEN_SSWBUTTONS)
    if ((millis() & 0x4000) == 0x4000) {
      testGenerateExtRetVal(millis() >> 11);
      writeDisplayTile(SSWbuttons, 0, 0, B10000);
    } else {
      SSWbuttons.testProgramme(B10000);
    }
#endif
#if (SK_HWEN_MENU)
    menuDisplay.gotoRowCol(0, 0);
    menuDisplay << millis() << F(" millis");

    static uint8_t menuEncValue[2] = {0, 0};
    for (uint8_t a = 0; a < 2; a++) {
      menuDisplay.gotoRowCol((1 - a) + 1, 0);
      int encValue = menuEncoders.state(a, 1000);
      if (encValue) {
        Serial << F("Enc") << a << F(": ") << encValue << "," << menuEncoders.lastCount(a, 2) << F("=> ") << menuEncValue[a] << F("\n");
      }
      switch (encValue) {
      case 1:
      case -1:
        menuEncValue[a] += menuEncoders.lastCount(a, 2);
        menuDisplay << F("Enc") << a << F(": ") << menuEncValue[a] << F(" (") << menuEncoders.lastCount(a, 2) << F(")   ");
        break;
      default:
        if (encValue > 2) {
          menuDisplay << F("Enc") << a << F(": ");
          if (encValue < 1000) {
            menuDisplay.print(F("Press "));
          } else {
            menuDisplay.print(F("Hold  "));
          }
        }
        break;
      }
    }

#endif
#if (SK_HWEN_GPIO)
    GPIOboard.setOutputAll(GPIOboard.inputIsActiveAll() ^ (millis() >> 12));
#endif

#if (SK_HWEN_ACM)
    if (AudioMasterPot.uniDirectionalSlider_hasMoved()) {
      Serial << AudioMasterPot.uniDirectionalSlider_position() << F("\n");

      AudioMasterControl.VUmeter(AudioMasterPot.uniDirectionalSlider_position() * 17, (1000 - AudioMasterPot.uniDirectionalSlider_position()) * 17);
    }

    AudioMasterControl.setChannelIndicatorLight(1, (millis() >> 10) & B11);

    // Buttons:
    for (int a = 0; a < 5; a++) {
      AudioMasterControl.setButtonLight(a + 1, (millis() >> (10 + a)) & B1);
    }
#endif

    delay(40);
  }

#if (SK_HWEN_STDOLEDDISPLAY)
  if (!displayWritten) {
    infoDisplay.clearDisplay();
    infoDisplay.fillRoundRect(0, 0, 128, 11, 1, 1);
    infoDisplay.setTextSize(1);
    infoDisplay.setTextColor(0, 1);
    infoDisplay.setCursor(10, 2);
    infoDisplay << F("Configuration Mode");
    infoDisplay.setTextColor(1, 0);
    infoDisplay.setCursor(0, 14);
    infoDisplay << F("http://") << ip;
    infoDisplay.setCursor(95, 24);
    infoDisplay << _DECPADL((millis() / 1000) / 60, 2, "0") << (((millis() / 500) % 2) == 1 ? F(" ") : F(":")) << _DECPADL((millis() / 1000) % 60, 2, "0");
    infoDisplay.display(B1); // Write to all
  }
#endif
}

#if (SK_HWEN_SSWMENU)
void HWrunLoop_SSWMenu(const uint8_t HWc) {
  uint16_t bDown;
  if (getNumOfActions(HWc) > 0) {
    static uint8_t SSWMenuItemPtr = 0;

    SSWmenuEnc.runLoop();
    if (SSWmenu.buttonDown(5)) {
      SSWMenuItemPtr = (SSWMenuItemPtr + 1) % getNumOfActions(HWc);
    }

    static bool voidVar = SSWmenuEnc.reset(0);

    int clicks = 0;
    bool actDown = false;
    bDown = SSWmenuEnc.state(0, 1000);

    switch (bDown) {
    case 1:
    case -1:
      clicks = SSWmenuEnc.lastCount(0, 2);
      break;
    case 2:
      _systemHWcActionFineFlag[HWc - 1] = !_systemHWcActionFineFlag[HWc - 1];
      break;
    default: // reset
      if (bDown >= 1000) {
        actDown = true;
      }
      break;
    }
    extRetValIsWanted(true);
    actionDispatch(HWc, actDown, actDown, (clicks << 1) | _systemHWcActionFineFlag[HWc - 1], 0x8000, SSWMenuItemPtr + 1);

    SSWmenuEnc.runLoop();

    static uint16_t prevHash = 0;
    static uint8_t prevColor = 0;
    if (prevHash != extRetValHash()) {
      prevHash = extRetValHash();
      writeDisplayTile(SSWmenu, 0, 0, 0);
      SSWmenuEnc.runLoop();
      SSWmenu.display(B10000);
      Serial << F("Write SSWmenu gfx!\n");
    }
    if (prevColor != _extRetColor) {
      prevColor = _extRetColor;
      SSWmenuEnc.runLoop();
      SSWmenu.setButtonColor((_extRetColor >> 4) & 3, (_extRetColor >> 2) & 3, _extRetColor & 3, B10000);
      if (debugMode)
        Serial << F("Write SSWmenu color\n");
    }
  }
}
#endif

#if (SK_HWEN_MENU)
void HWrunLoop_Menu(const uint8_t HWc) {
  uint16_t bDown;
  if (getNumOfActions(HWc) > 0) {
    static uint8_t menuItemPtr = 0;
    switch (menuEncoders.state(1, 1000)) {
    case 1:
    case -1:
      menuItemPtr = (menuItemPtr + getNumOfActions(HWc) + menuEncoders.lastCount(1, 2)) % getNumOfActions(HWc);
      Serial << menuItemPtr;
      break;
    }

    int clicks = 0;
    bool actDown = false;
    bDown = menuEncoders.state(0, 1000);
    switch (bDown) {
    case 1:
    case -1:
      clicks = menuEncoders.lastCount(0, 2);
      break;
    case 2:
      _systemHWcActionFineFlag[HWc - 1] = !_systemHWcActionFineFlag[HWc - 1];
      break;
    default: // reset
      if (bDown >= 1000) {
        actDown = true;
      }
      break;
    }
    extRetValIsWanted(true);
    actionDispatch(HWc, actDown, actDown, (clicks << 1) | _systemHWcActionFineFlag[HWc - 1], 0x8000, menuItemPtr + 1);

    static uint16_t prevHashTD = 0;
    if (prevHashTD != extRetValHash()) {
      prevHashTD = extRetValHash();
      write3x16Display(menuDisplay);
      if (debugMode)
        Serial << F("Write Text display!\n");
    }
  }
}
#endif

#if (SKAARHOJDISPARRAY_LCDWIDTH)
void HWrunLoop_128x32OLED(SkaarhojDisplayArray &display, const uint8_t HWc, uint16_t *display_prevHash, bool &display_written) {
  uint16_t retVal;

  // Info display, 128x32 OLED:
  bool display_update = false;
  for (uint8_t a = 0; a < 3; a++) {
    extRetValIsWanted(true);
    retVal = actionDispatch(a + HWc);
    if (a == 0)
      display_written = false;

    if (display_prevHash[a] != extRetValHash()) {
      display_prevHash[a] = extRetValHash();
      display_update = true;

      if (a == 0) { // Main display
        display_written = retVal != 0;
        writeDisplayTile(display, 0, 0, B0, 0, 1);
        if (!retVal) { // In case blanking, make sure other display layers are evaluated:
          for (uint8_t b = 1; b < 3; b++) {
            display_prevHash[b]++;
          }
        }
      } else {
        if (!display_written || retVal != 0) { // Write if a) previous display was not written with non-blank content and b) if this display has non-blank content
          writeDisplayTile(display, ((a - 1) & 1) << 6, 0, B0, a == 1, 0);
        }
      }
      if (debugMode)
        Serial << F("Write info disp! ") << a << F("\n");
    }
  }
  if (display_update)
    display.display(B1);
}
#endif

#if SK_HWEN_SLIDER
void HWrunLoop_slider(const uint8_t HWc) {
  // Slider:
  bool hasMoved = slider.uniDirectionalSlider_hasMoved();
  actionDispatch(HWc, hasMoved, hasMoved && slider.uniDirectionalSlider_isAtEnd(), 0, slider.uniDirectionalSlider_position());
}
#endif

#if SK_HWEN_GPIO
void HWrunLoop_GPIO(SkaarhojGPIO2x8 &gpioBoard, const uint8_t HWc, bool *gpioStates) {
  uint16_t bUp, bDown;

  // GPIO Inputs
  bUp = gpioBoard.inputUpAll();
  bDown = gpioBoard.inputDownAll();
  for (uint8_t a = 0; a < 8; a++) {
    extRetValPrefersLabel(HWc + 8 + a);
    actionDispatch(HWc + 8 + a, bDown & (B1 << a), bUp & (B1 << a));
  }

  // GPIO Outputs
  for (uint8_t a = 0; a < 8; a++) {
    extRetValPrefersLabel(HWc + a);
    uint8_t state = actionDispatch(HWc + a);
    bool stateB = state & 0x20; // Output bit
    if (stateB != gpioStates[a]) {
      gpioStates[a] = stateB;
      gpioBoard.setOutput(a + 1, stateB);
      if (debugMode)
        Serial << F("Write GPIO ") << a + 1 << F("\n");
    }
  }
}
#endif

#if SK_HWEN_SSWBUTTONS
void HWrunLoop_SSWbuttons(const uint8_t *HWcMap) {
  uint16_t bUp, bDown;
  bUp = SSWbuttons.buttonUpAll();
  bDown = SSWbuttons.buttonDownAll();
  static uint16_t prevHash[2];
  static uint8_t prevColor[2];
  for (uint8_t a = 0; a < 2; a++) {
    extRetValIsWanted(true);          // Requesting generation of graphics
    extRetValPrefersLabel(HWcMap[a]); // Telling that this is buttons which prefer to have a label rather than status shown in graphics.
    actionDispatch(HWcMap[a], bDown & (B1 << a), bUp & (B1 << a));

    if (prevHash[a] != extRetValHash()) {
      prevHash[a] = extRetValHash();
      writeDisplayTile(SSWbuttons, 0, 0, B1 << a);
      if (debugMode)
        Serial << F("Write SSW ") << a << F("\n");
    }

    if (prevColor[a] != _extRetColor) {
      prevColor[a] = _extRetColor;
      SSWbuttons.setButtonColor((_extRetColor >> 4) & 3, (_extRetColor >> 2) & 3, _extRetColor & 3, B1 << a);
      if (debugMode)
        Serial << F("Write SSWcolor! ") << a << F("\n");
    }
  }
}
#endif
uint16_t mapPotHelper(uint16_t potValue) {
  // -60, -30, -12, -6, -3, 0, 3, 6
  uint16_t in[] = {0, 122, 343, 492, 629, 770, 866, 1000};
  uint16_t out[] = {0, 497, 719, 815, 855, 900, 964, 1000};
  for (uint8_t a = 0; a < 7; a++) {
    if (potValue >= in[a] && potValue < in[a + 1]) {
      return map(potValue, in[a], in[a + 1], out[a], out[a + 1]);
    }
  }
  return potValue;
}

#if (SK_HWEN_ACM || defined(SkaarhojAudioControl2_H))
void HWrunLoop_AudioControlMaster(SkaarhojAudioControl2 &control, SkaarhojAnalog &pot, const uint8_t *HWcMap, uint8_t theSize) {
  // HWcMap: These numbers refer to the drawing in the web interface in this order: Potmeter, Peak Diode, VU, left button, right button, lower button, middle button, top button

  if (HWcMap[0]) { // Pot
    bool hasMoved = pot.uniDirectionalSlider_hasMoved();
    actionDispatch(HWcMap[0], hasMoved, false, 0, mapPotHelper(pot.uniDirectionalSlider_position()));
  }

  if (HWcMap[1]) { // Channel Indicator light
    uint16_t retVal = actionDispatch(HWcMap[1]);
    control.setChannelIndicatorLight(1, retVal & 0x20 ? constrain((retVal & 0xF) - 1, 0, 3) : 0);
  }

  if (HWcMap[2]) { // VU
    uint16_t retVal = actionDispatch(HWcMap[2]);
    control.VUmeterDB((int)highByte(retVal) - 60, (int)lowByte(retVal) - 60);
  }

  // Buttons:
  uint16_t bUp = control.buttonUpAll();
  uint16_t bDown = control.buttonDownAll();
  for (int a = 0; a < 5; a++) {
    if (HWcMap[3 + a]) {
      uint16_t color = actionDispatch(HWcMap[3 + a], bDown & (B1 << a), bUp & (B1 << a));
      control.setButtonLight(a + 1, (color & 0xF) > 0 ? ((!(color & 0x10) || (millis() & 512) > 0) && ((color & 0xF) != 5) ? 1 : 0) : 0);
    }
  }
}
#endif

#if defined(SkaarhojAudioControl2_H)
void HWrunLoop_AudioControl(SkaarhojAudioControl2 &control, SkaarhojAnalog &pot1, SkaarhojAnalog &pot2, const uint8_t *HWcMap, uint8_t theSize) {
  // HWcMap: These numbers refer to the drawing in the web interface in this order: Potmeter A, Potmeter B, Peak Diode A, Peak Diode B, left button A, right button A, left button B, right button B

  if (HWcMap[0]) { // Pot1
    bool hasMoved = pot1.uniDirectionalSlider_hasMoved();
    actionDispatch(HWcMap[0], hasMoved, false, 0, mapPotHelper(pot1.uniDirectionalSlider_position()));
  }

  if (HWcMap[1]) { // Pot2
    bool hasMoved = pot2.uniDirectionalSlider_hasMoved();
    actionDispatch(HWcMap[1], hasMoved, false, 0, mapPotHelper(pot2.uniDirectionalSlider_position()));
  }

  for (int a = 0; a < 2; a++) {
    if (HWcMap[2 + a]) { // Channel Indicator light
      uint16_t retVal = actionDispatch(HWcMap[2 + a]);
      control.setChannelIndicatorLight(1 + a, retVal & 0x20 ? constrain((retVal & 0xF) - 1, 0, 3) : 0);
    }
  }

  // Buttons:
  uint16_t bUp = control.buttonUpAll();
  uint16_t bDown = control.buttonDownAll();
  for (int a = 0; a < 4; a++) {
    if (HWcMap[4 + a]) {
      uint16_t color = actionDispatch(HWcMap[4 + a], bDown & (B1 << a), bUp & (B1 << a));
      control.setButtonLight(a + 1, (color & 0xF) > 0 ? ((!(color & 0x10) || (millis() & 512) > 0) && ((color & 0xF) != 5) ? 1 : 0) : 0);
    }
  }
}
#endif

void HWrunLoop_BI8(SkaarhojBI8 &buttons, const uint8_t *HWcMap, uint8_t theSize) {
  uint16_t bUp, bDown;
  bUp = buttons.buttonUpAll();
  bDown = buttons.buttonDownAll();
  for (uint8_t a = 0; a < theSize; a++) {
    extRetValPrefersLabel(HWcMap[a]);
    uint8_t color = actionDispatch(HWcMap[a], bDown & (B1 << a), bUp & (B1 << a));
    buttons.setButtonColor(a + 1, color & 0xF);
  }
}

void HWrunLoop_encoders(SkaarhojEncoders &encoders, const uint8_t *encMap, uint8_t theSize, bool reverseFirst = false) {
  uint16_t bDown;
  for (uint8_t a = 0; a < theSize; a++) {
    if (encMap[a]) {
      int clicks = 0;
      bool actDown = false;
      bDown = encoders.state(a, 1000);
      switch (bDown) {
      case 1:
      case -1:
        clicks = encoders.lastCount(a, 2);
        break;
      case 2:
        _systemHWcActionFineFlag[encMap[a] - 1] = !_systemHWcActionFineFlag[encMap[a] - 1];
        break;
      default: // reset
        if (bDown >= 1000) {
          actDown = true;
        }
        break;
      }
      actionDispatch(encMap[a], actDown, actDown, (((a == 0 && reverseFirst ? -1 : 1) * clicks) << 1) | _systemHWcActionFineFlag[encMap[a] - 1]);
    }
  }
}

/************************************
 *
 * ACTION HANDLING
 *
 ************************************/
/**
 * Action cache initialization
 */
void initActionCache() {
  memset(_systemHWcActionCache, 0, SK_HWCCOUNT * SK_MAXACTIONS); // 16 bit - does it clear it all???
  memset(_systemHWcActionCacheFlag, 0, SK_HWCCOUNT * SK_MAXACTIONS);
  memset(_systemHWcActionFineFlag, 0, SK_HWCCOUNT);
}

long pulsesHelper(long inValue, const long lower, const long higher, const bool cycle, const int pulses, const int scaleFine, const int scaleNormal) {
  int scale = pulses & B1 ? scaleFine : scaleNormal;

  inValue += (pulses >> 1) * scale;
  if (cycle) {
    if (inValue < lower) {
      inValue = higher - (lower - inValue - 1);
    } else if (inValue > higher) {
      inValue = lower + (inValue - higher - 1);
    }
  }
  return constrain(inValue, lower, higher);
}
void storeMemory(uint8_t memPtr) {
  EEPROM.write(16 + memPtr, _systemMem[memPtr]);
  EEPROM.write(20, 193 ^ EEPROM.read(16) ^ EEPROM.read(17) ^ EEPROM.read(18) ^ EEPROM.read(19));
  if (debugMode)
    Serial << F("Memory ") << char(65 + memPtr) << F(" saved\n");
}
uint8_t cycleMemHelper(uint8_t actionPtr, uint8_t idx = 255) {
  int values;
  values += (globalConfigMem[actionPtr + 3] != 0 || abs((int)globalConfigMem[actionPtr + 2] - (int)globalConfigMem[actionPtr + 3])) ? abs((int)globalConfigMem[actionPtr + 2] - (int)globalConfigMem[actionPtr + 3]) + 1 : 0;
  if (idx < values) {
    return (globalConfigMem[actionPtr + 2] < globalConfigMem[actionPtr + 3] ? globalConfigMem[actionPtr + 3] - (values - idx - 1) : globalConfigMem[actionPtr + 3] + (values - idx - 1));
  }
  values += (globalConfigMem[actionPtr + 5] != 0 || abs((int)globalConfigMem[actionPtr + 4] - (int)globalConfigMem[actionPtr + 5])) ? abs((int)globalConfigMem[actionPtr + 4] - (int)globalConfigMem[actionPtr + 5]) + 1 : 0;
  if (idx < values) {
    return (globalConfigMem[actionPtr + 4] < globalConfigMem[actionPtr + 5] ? globalConfigMem[actionPtr + 5] - (values - idx - 1) : globalConfigMem[actionPtr + 5] + (values - idx - 1));
  }
  values += (globalConfigMem[actionPtr + 6] != 0) ? 1 : 0;
  if (idx < values) {
    return globalConfigMem[actionPtr + 6];
  }

  return values; // Count of values
}

/**
 * Evaluates System Actions
 */
uint16_t evaluateAction_system(const uint16_t actionPtr, const uint8_t HWc, const uint8_t actIdx, bool actDown, bool actUp, int pulses, int value) {
  if (debugMode && (actDown || actUp)) {
    Serial << F("System action ") << globalConfigMem[actionPtr] << F("\n");
  }
  uint8_t temp;
  uint16_t retVal = 0;

  switch (globalConfigMem[actionPtr]) {
  case 0: // Set Shift
    if (actDown) {
      if (value != 0x8000) { // Value input
        _systemShift = sTools.shapeInt(constrain(map(value, 0, 1000, 0, sTools.shapeInt(globalConfigMem[actionPtr + 1], 0, SK_MAXACTIONS - 1) + 1), 0, sTools.shapeInt(globalConfigMem[actionPtr + 1], 0, SK_MAXACTIONS - 1)), 0, SK_MAXACTIONS - 1);
      } else {
        if (globalConfigMem[actionPtr + 2] == 3 || globalConfigMem[actionPtr + 2] == 4) { // Cycle up/down
          _systemHWcActionCacheFlag[HWc][actIdx] = true;                                  // Used to show button is highlighted here
          pulses = (globalConfigMem[actionPtr + 2] == 3 ? 1 : -1) << 1;
        } else if (globalConfigMem[actionPtr + 2] != 2 || !_systemHWcActionCacheFlag[HWc][actIdx]) {
          _systemHWcActionCacheFlag[HWc][actIdx] = true; // Used for toggle feature
          _systemHWcActionCache[HWc][actIdx] = _systemShift;
          _systemShift = sTools.shapeInt(globalConfigMem[actionPtr + 1], 0, SK_MAXACTIONS - 1);
        } else {
          _systemShift = sTools.shapeInt(_systemHWcActionCache[HWc][actIdx], 0, SK_MAXACTIONS - 1);
          _systemHWcActionCacheFlag[HWc][actIdx] = false;
        }
      }
    }
    if (actUp && globalConfigMem[actionPtr + 2] == 1) { // "Hold Down"
      _systemShift = sTools.shapeInt(_systemHWcActionCache[HWc][actIdx], 0, SK_MAXACTIONS - 1);
    }
    if (actUp && (globalConfigMem[actionPtr + 2] == 3 || globalConfigMem[actionPtr + 2] == 4)) { // "Cycle"
      _systemHWcActionCacheFlag[HWc][actIdx] = false;
    }
    if (pulses & 0xFFFE) {
      _systemShift = pulsesHelper(_systemShift, 0, sTools.shapeInt(globalConfigMem[actionPtr + 1], 0, SK_MAXACTIONS - 1), true, pulses, 1, 1);
    }
    if (debugMode && (actDown || (pulses & 0xFFFE)))
      Serial << F("SHIFT: ") << _systemShift << F("\n");

    retVal = (globalConfigMem[actionPtr + 2] == 3 || globalConfigMem[actionPtr + 2] == 4) ? (_systemHWcActionCacheFlag[HWc][actIdx] ? (4 | 0x20) : 5) : (_systemShift == globalConfigMem[actionPtr + 1] ? (4 | 0x20) : 5);

    if (extRetValIsWanted()) {
      temp = (_systemHWcActionPrefersLabel[HWc] && !(globalConfigMem[actionPtr + 2] == 3 || globalConfigMem[actionPtr + 2] == 4)) ? globalConfigMem[actionPtr + 1] : _systemShift;

      extRetVal(temp, temp == 0 || (temp == 1 && globalConfigMem[actionPtr + 1] < 2) ? 7 : 0); // , pulses&B1 - not using this because it has no significance for this type of action.
      extRetValShortLabel(PSTR("Shift"));
      extRetValLongLabel(PSTR("Shift Level"));

      if (_systemHWcActionPrefersLabel[HWc] && !(globalConfigMem[actionPtr + 2] == 3 || globalConfigMem[actionPtr + 2] == 4)) {
        extRetValColor(retVal & 0x20 ? B100110 : B101010);
        extRetValSetLabel(true);
      } else {
        extRetValColor(B100110);
      }

      if (temp == 0) {
        extRetValTxt_P(PSTR("Off"), 0);
      } else if (temp == 1 && globalConfigMem[actionPtr + 1] < 2) {
        extRetValTxt_P(PSTR("On"), 0);
      }
    }
    return retVal;
    break;
  case 1: // Set State
    if (actDown) {
      if (value != 0x8000) { // Value input
        _systemState = sTools.shapeInt(constrain(map(value, 0, 1000, 0, sTools.shapeInt(globalConfigMem[actionPtr + 1], 0, SK_MAXSTATES - 1) + 1), 0, sTools.shapeInt(globalConfigMem[actionPtr + 1], 0, SK_MAXSTATES - 1)), 0, SK_MAXSTATES - 1);
      } else {
        if (globalConfigMem[actionPtr + 2] == 3 || globalConfigMem[actionPtr + 2] == 4) { // Cycle up/down
          _systemHWcActionCacheFlag[HWc][actIdx] = true;                                  // Used to show button is highlighted here
          pulses = (globalConfigMem[actionPtr + 2] == 3 ? 1 : -1) << 1;
        } else if (globalConfigMem[actionPtr + 2] != 2 || !_systemHWcActionCacheFlag[HWc][actIdx]) {
          _systemHWcActionCacheFlag[HWc][actIdx] = true; // Used for toggle feature
          _systemPrevState = _systemState;
          _systemState = sTools.shapeInt(globalConfigMem[actionPtr + 1], 0, SK_MAXSTATES - 1);
        } else {
          _systemState = sTools.shapeInt(_systemPrevState, 0, SK_MAXSTATES - 1);
          _systemHWcActionCacheFlag[HWc][actIdx] = false;
        }
      }
    }
    if (actUp && globalConfigMem[actionPtr + 2] == 1) { // "Hold Down"
      _systemState = sTools.shapeInt(_systemPrevState, 0, SK_MAXSTATES - 1);
    }
    if (actUp && (globalConfigMem[actionPtr + 2] == 3 || globalConfigMem[actionPtr + 2] == 4)) { // "Cycle"
      _systemHWcActionCacheFlag[HWc][actIdx] = false;
    }
    if (pulses & 0xFFFE) {
      _systemState = pulsesHelper(_systemState, 0, sTools.shapeInt(globalConfigMem[actionPtr + 1], 0, SK_MAXSTATES - 1), true, pulses, 1, 1);
    }
    if (debugMode && (actDown || (pulses & 0xFFFE)))
      Serial << F("STATE: ") << _systemState << F("\n");

    retVal = (globalConfigMem[actionPtr + 2] == 3 || globalConfigMem[actionPtr + 2] == 4) ? (_systemHWcActionCacheFlag[HWc][actIdx] ? (4 | 0x20) : 5) : (_systemState == globalConfigMem[actionPtr + 1] ? (4 | 0x20) : 5);

    if (extRetValIsWanted()) {
      temp = (_systemHWcActionPrefersLabel[HWc] && !(globalConfigMem[actionPtr + 2] == 3 || globalConfigMem[actionPtr + 2] == 4)) ? globalConfigMem[actionPtr + 1] : _systemState;

      extRetVal(temp, 7); // , pulses&B1 - not using this because it has no significance for this type of action.
      extRetValShortLabel(PSTR("State "), temp);
      extRetValLongLabel(PSTR("State Level"), temp);

      if (_systemHWcActionPrefersLabel[HWc] && !(globalConfigMem[actionPtr + 2] == 3 || globalConfigMem[actionPtr + 2] == 4)) {
        extRetValColor(retVal & 0x20 ? B010111 : B101010);
        extRetValSetLabel(true);
      } else {
        extRetValColor(B010111);
      }

      getStateName(_strCache, temp);
      char *item = strtok(_strCache, "|");
      if (strlen(item)) {
        extRetValTxt(item, 0);
        if (strlen(item) > 5)
          extRetVal2(0);
        item = strtok(NULL, "|");
        if (item != NULL) {
          extRetValTxt(item, 1);
          extRetVal2(0);
        }
      } else {
        _extRetFormat = 0;
      }
    }
    return retVal;
    break;
  case 2: // Set Memory
    if (globalConfigMem[actionPtr + 1] < 4) {
      if (actDown) {
        if (value != 0x8000) { // Value input
          _systemMem[globalConfigMem[actionPtr + 1]] = constrain(map(value, 0, 1000, 0, globalConfigMem[actionPtr + 2] + 1), 0, globalConfigMem[actionPtr + 2]);
        } else {
          if (globalConfigMem[actionPtr + 3] == 3 || globalConfigMem[actionPtr + 3] == 4) { // Cycle up/down
            _systemHWcActionCacheFlag[HWc][actIdx] = true;                                  // Used to show button is highlighted here
            pulses = (globalConfigMem[actionPtr + 3] == 3 ? 1 : -1) << 1;
          } else if (globalConfigMem[actionPtr + 3] != 2 || !_systemHWcActionCacheFlag[HWc][actIdx]) {
            _systemHWcActionCacheFlag[HWc][actIdx] = true; // Used for toggle feature
            _systemHWcActionCache[HWc][actIdx] = _systemMem[globalConfigMem[actionPtr + 1]];
            _systemMem[globalConfigMem[actionPtr + 1]] = globalConfigMem[actionPtr + 2];
          } else {
            _systemMem[globalConfigMem[actionPtr + 1]] = _systemHWcActionCache[HWc][actIdx];
            _systemHWcActionCacheFlag[HWc][actIdx] = false;
          }
        }
      }
      if (actUp && globalConfigMem[actionPtr + 3] == 1) { // "Hold Down"
        _systemMem[globalConfigMem[actionPtr + 1]] = _systemHWcActionCache[HWc][actIdx];
      }
      if (actUp && (globalConfigMem[actionPtr + 3] == 3 || globalConfigMem[actionPtr + 3] == 4)) { // "Cycle"
        _systemHWcActionCacheFlag[HWc][actIdx] = false;
      }
      if (pulses & 0xFFFE) {
        _systemMem[globalConfigMem[actionPtr + 1]] = pulsesHelper(_systemMem[globalConfigMem[actionPtr + 1]], 0, globalConfigMem[actionPtr + 2], true, pulses, 1, constrain(globalConfigMem[actionPtr + 2] / 10, 1, 10));
      }
      if (actDown || actUp || (pulses & 0xFFFE)) {
        if (globalConfigMem[actionPtr + 4])
          storeMemory(globalConfigMem[actionPtr + 1]);
        if (debugMode)
          Serial << F("Mem ") << char(globalConfigMem[actionPtr + 1] + 65) << F(": ") << _systemMem[globalConfigMem[actionPtr + 1]] << F("\n");
      }

      retVal = (globalConfigMem[actionPtr + 3] == 3 || globalConfigMem[actionPtr + 3] == 4) ? (_systemHWcActionCacheFlag[HWc][actIdx] ? (4 | 0x20) : 5) : (_systemMem[globalConfigMem[actionPtr + 1]] == globalConfigMem[actionPtr + 2] ? (4 | 0x20) : 5);

      if (extRetValIsWanted()) {
        temp = (_systemHWcActionPrefersLabel[HWc] && !(globalConfigMem[actionPtr + 3] == 3 || globalConfigMem[actionPtr + 3] == 4)) ? globalConfigMem[actionPtr + 2] : _systemMem[globalConfigMem[actionPtr + 1]];

        extRetVal(temp, 0, pulses & B1);
        extRetValShortLabel(PSTR("Memory "), globalConfigMem[actionPtr + 1]);
        extRetValLongLabel(PSTR("Memory "), globalConfigMem[actionPtr + 1]);
        _extRetShort[7] = _extRetLong[7] = char(globalConfigMem[actionPtr + 1] + 65);

        if (_systemHWcActionPrefersLabel[HWc] && !(globalConfigMem[actionPtr + 3] == 3 || globalConfigMem[actionPtr + 3] == 4)) {
          extRetValColor(retVal & 0x20 ? B010111 : B101010);
          extRetValSetLabel(true);
        } else {
          extRetValColor(B010111);
        }
      }
    }
    return retVal;
    break;
  case 3: // Cycle Memory
    if (globalConfigMem[actionPtr + 1] < 4) {
      uint8_t numValues = cycleMemHelper(actionPtr);

      if (actDown) {
        if (value == 0x8000) { // Binary
          _systemHWcActionCache[HWc][actIdx] = pulsesHelper(_systemHWcActionCache[HWc][actIdx], 0, numValues - 1, true, 1 << 1);
        } else { // Value:
          _systemHWcActionCache[HWc][actIdx] = constrain(map(value, 0, 1000, 0, numValues), 0, numValues - 1);
        }
      }
      if (pulses & 0xFFFE) { // Encoder:
        _systemHWcActionCache[HWc][actIdx] = pulsesHelper(_systemHWcActionCache[HWc][actIdx], 0, numValues - 1, true, pulses, 1, 1);
      }
      if (actDown || (pulses & 0xFFFE)) {
        _systemMem[globalConfigMem[actionPtr + 1]] = cycleMemHelper(actionPtr, _systemHWcActionCache[HWc][actIdx]);
        if (debugMode)
          Serial << F("Mem ") << char(globalConfigMem[actionPtr + 1] + 65) << F(": (") << _systemHWcActionCache[HWc][actIdx] << F("/") << numValues << F(") ") << _systemMem[globalConfigMem[actionPtr + 1]] << F("\n");
        if (globalConfigMem[actionPtr + 7])
          storeMemory(globalConfigMem[actionPtr + 1]);
      }

      if (extRetValIsWanted()) {
        extRetVal(_systemMem[globalConfigMem[actionPtr + 1]]);
        extRetValShortLabel(PSTR("Memory "), globalConfigMem[actionPtr + 1]);
        extRetValLongLabel(PSTR("Memory "), globalConfigMem[actionPtr + 1]);
        _extRetShort[7] = _extRetLong[7] = char(globalConfigMem[actionPtr + 1] + 65);
        extRetValColor(B010111);
      }
    }
    break;
  case 4: // Flag bit, array(9, "Flip bit", array(1,64,"Bit"), array("","Set", "Clear", "Toggle", "Hold Down"), array("","1ms","10ms","100ms","500ms","1s","2s"), array(1,64,"Feedback")),
          // array(1+2+8, "Flag", array(0,63,"Flag"), array("Set","Hold Down","Toggle"), array("","Invert"), array("","1ms","10ms","100ms","500ms","1s","2s"), array(0,63,"Feedback Flag"), array("","Invert")),
    if (actDown || (pulses & 0xFFFE)) {
      if (globalConfigMem[actionPtr + 1] < 64) {
        _systemHWcActionCacheFlag[HWc][actIdx] = true;
        _systemHWcActionCache[HWc][actIdx] = (unsigned long)millis();
        switch (globalConfigMem[actionPtr + 2]) {
        case 0:
        case 1:
          setSystemBit(globalConfigMem[actionPtr + 1], (!globalConfigMem[actionPtr + 3]) ^ ((value == 0x8000) ? false : (value < 500)));
          //   Serial << "BIT A: " << ((!globalConfigMem[actionPtr + 3]) ^ ((value == 0x8000) ? false : (value < 500))) << "\n";
          break;
        case 2:
          setSystemBit(globalConfigMem[actionPtr + 1], !getSystemBit(globalConfigMem[actionPtr + 1]));
          //    Serial << "BIT B: " << !getSystemBit(globalConfigMem[actionPtr + 1]) << "\n";
          break;
        }
      }
    }

    if (globalConfigMem[actionPtr + 4] > 0 && globalConfigMem[actionPtr + 4] <= 6 && _systemHWcActionCacheFlag[HWc][actIdx] && globalConfigMem[actionPtr + 1] < 64) { // Fall back timer, works for set and clear
      uint16_t millisArray[] = {0, 1, 10, 100, 500, 1000, 2000};
      if ((uint16_t)millis() - (unsigned long)_systemHWcActionCache[HWc][actIdx] > millisArray[globalConfigMem[actionPtr + 4]]) {
        _systemHWcActionCacheFlag[HWc][actIdx] = false;
        setSystemBit(globalConfigMem[actionPtr + 1], globalConfigMem[actionPtr + 3]);
        // Serial << "Timer bit" << (bool)globalConfigMem[actionPtr + 3] << "\n";
      }
    }

    if (actUp && globalConfigMem[actionPtr + 2] == 1) { // "Hold Down"
      setSystemBit(globalConfigMem[actionPtr + 1], globalConfigMem[actionPtr + 3]);
      //  Serial << "Hold Down" << (bool)globalConfigMem[actionPtr + 3] << "\n";
    }

    if (actDown || actUp || (pulses & 0xFFFE)) {
      if (debugMode)
        Serial << F("Flag ") << globalConfigMem[actionPtr + 1] << F(": ") << getSystemBit(globalConfigMem[actionPtr + 1]) << F("\n");
    }

    retVal = (getSystemBit(globalConfigMem[actionPtr + 5]) ^ globalConfigMem[actionPtr + 6]) ? (4 | 0x20) : 5;

    if (extRetValIsWanted()) {
      extRetVal(0, 7);
      extRetValShortLabel(PSTR("Flag "), globalConfigMem[actionPtr + 1]);
      extRetValLongLabel(PSTR("Flag "), globalConfigMem[actionPtr + 1]);

      extRetValColor(retVal & 0x20 ? B011100 : B101010);
      if (_systemHWcActionPrefersLabel[HWc] && !(globalConfigMem[actionPtr + 2] == 2)) {
        extRetValSetLabel(true);
        extRetValTxt_P(!globalConfigMem[actionPtr + 3] ? PSTR("On") : PSTR("Off"), 0);
      } else {
        extRetValTxt_P(retVal & 0x20 ? PSTR("On") : PSTR("Off"), 0);
      }
    }

    return retVal;
    break;
  case 5: // Mirrow #HWC
    actionMirror = globalConfigMem[actionPtr + 1];
    return 0;
    break;
  case 6: // System info
    if (extRetValIsWanted()) {
      extRetVal(0, 7);
      extRetValShortLabel(PSTR("SysInfo"));
      extRetValLongLabel(PSTR("System Info"));
      extRetVal2(0);
      if (getDevUnconnected()) {
        extRetValTxt_P(PSTR(" ! Unconnected"), 0);
        _extRetTxt[0][0] = getDevUnconnected() + 48;
      } else {
        extRetValTxt_P(PSTR("OK Connections"), 0);
      }
    }
    break;
  case 7: // No action
    return 0;
    break;
  case 8: // Wait
    if (actDown || (pulses & 0xFFFE))
      lDelay(sTools.shapeInt(globalConfigMem[actionPtr + 1], 0, 20) * 100);
    break;
  case 9: // Custom function
    return customActionHandler(actionPtr, HWc, actIdx, actDown, actUp, pulses, value);
    break;
  case 10: // Inactivate
    if (_inactivePanel_actDown)
      _inactivePanel = !_inactivePanel;

    retVal = _inactivePanel ? ((((millis() & 512) > 0 ? 2 : 0) | B10000) | 0x20) : 3;

    if (extRetValIsWanted()) {
      extRetVal(0, 7);
      extRetValShortLabel(PSTR("Inactivate"));
      extRetValLongLabel(PSTR("Inactivate Panel"));
      extRetValColor((retVal & 0xF) == 2 ? B110000 : ((retVal & 0xF) == 3 ? B011101 : B101000));
      extRetValTxt_P(_inactivePanel ? PSTR("Inactive") : PSTR("Active"), 0);
      extRetValTxtShort_P(_inactivePanel ? PSTR("Inact") : PSTR("Activ"));
    }

    return retVal;
    break;
  case 11: // Stop connect
    if (actDown || (pulses & 0xFFFE)) {
      for (uint8_t a = 1; a < sizeof(deviceArray); a++) {
        if (deviceEn[a] && !deviceReady[a])
          deviceEn[a] = false; // Simply disable non-connected devices...
        devicesStopped = true;
      }
    }

    retVal = getDevUnconnected() ? (((millis() & 0x200) > 0 ? 1 : 0) | 0x20) : (devicesStopped ? (2 | 0x20) : 3);

    if (extRetValIsWanted()) {
      extRetVal(0, 7);
      extRetValShortLabel(PSTR("Stop Conn."));
      extRetValLongLabel(PSTR("Stop Connections"));
      extRetValColor((retVal & 0xF) == 2 ? B110000 : ((retVal & 0xF) == 3 ? B011101 : B101000));

      if (_systemHWcActionPrefersLabel[HWc]) {
        extRetValSetLabel(true);
        extRetValTxt(PSTR("Stop"), 0);
      } else {
        extRetVal2(0);
        extRetValTxt_P(!devicesStopped ? PSTR("Will keep") : PSTR("Will not"), 0);
        extRetValTxt_P(!devicesStopped ? PSTR("connecting") : PSTR("connect"), 1);
      }
    }

    return retVal;
    break;
  }

  if (actDown) {
    _systemHWcActionCacheFlag[HWc][actIdx] = true;
  }
  if (actUp) {
    _systemHWcActionCacheFlag[HWc][actIdx] = false;
  }
  return _systemHWcActionCacheFlag[HWc][actIdx] ? 4 : 5;
}

/**
 * Dispatching actions in general
 * (Notice; is already declared in main sketch!)
 */
uint16_t actionDispatch(uint8_t HWcNum, bool actDown, bool actUp, int pulses, int value, const uint8_t specificAction) {
  uint8_t actionMirrorC = 0;
  actionMirror = 0;

  _inactivePanel_actDown = actDown;
  if (_inactivePanel) {
    actDown = false;
    actUp = false;
    pulses = pulses & B1;
    value = 0x8000;
  }

  do {

    if (HWcNum > SK_HWCCOUNT || HWcNum == 0)
      return 0; // Invalid values check - would leak memory if not checked. HWcNum is passed on minus 1 (incoming range is 1-x, outgoing range is 0-(x-1))

    if (debugMode && (actDown || actUp || (pulses & 0xFFFE))) {
      Serial << F("HWc#") << HWcNum << (actDown ? F(" Down") : F("")) << (actUp ? F(" Up") : F(""));
      if (pulses & 0xFFFE)
        Serial << F(" Pulses: ") << (pulses >> 1);
      Serial << F("\n");
    }

    uint16_t stateBehaviourPtr = getConfigMemIndex(HWcNum - 1, _systemState); // Fetching pointer to state behaviour
    uint16_t retValue = 0;                                                    // Returns zero by default (inactive element)
    uint16_t retValueT = 0;                                                   // Preliminary return value
    if (stateBehaviourPtr > 0) {                                              // If there is some configuration to relate to....
      uint8_t stateLen = globalConfigMem[stateBehaviourPtr - 1];
      bool shiftLevelMatch = false;
      uint8_t matchShiftValue = _systemShift;

      // Traverse actions in state behaviour
      while (!shiftLevelMatch) {
        uint8_t sShift = 0;
        uint8_t lptr = 0;
        uint8_t actIdx = 0;
        while (lptr < stateLen) {
          if (actIdx >= SK_MAXACTIONS)
            break; // actIdx at or over SK_MAXACTIONS would result in memory leaks in various evaluation functions which would trust HWc and actIdx to not exceed the bounds of the _systemHWcActionCache array
          if (lptr > 0 && (globalConfigMem[stateBehaviourPtr + lptr] & 16) > 0)
            sShift++; // If a shift divider is found (cannot be the first element)
          if ((specificAction == 0 && matchShiftValue == sShift) || (specificAction == actIdx + 1)) {
            shiftLevelMatch = true;
            // Traverse actions in shift level:

            uint8_t devIdx = globalConfigMem[stateBehaviourPtr + lptr] & 15;
            if (devIdx > 0 && devIdx < sizeof(deviceArray) && deviceEn[devIdx]) {
              if (deviceEn[devIdx] && deviceReady[devIdx]) {
                switch (pgm_read_byte_near(deviceArray + devIdx)) {
                case SK_DEV_ATEM:
#if SK_DEVICES_ATEM
                  retValueT = evaluateAction_ATEM(deviceMap[devIdx], stateBehaviourPtr + lptr + 1, HWcNum - 1, actIdx, actDown, actUp, pulses, value);
                  if (retValue == 0)
                    retValue = retValueT; // Use first ever return value in case of multiple actions.
#endif
                  break;
                case SK_DEV_HYPERDECK:
#if SK_DEVICES_HYPERDECK
                  retValueT = evaluateAction_HYPERDECK(deviceMap[devIdx], stateBehaviourPtr + lptr + 1, HWcNum - 1, actIdx, actDown, actUp, pulses, value);
                  if (retValue == 0)
                    retValue = retValueT; // Use first ever return value in case of multiple actions.
#endif
                  break;
                case SK_DEV_VIDEOHUB:
#if SK_DEVICES_VIDEOHUB
                  retValueT = evaluateAction_VIDEOHUB(deviceMap[devIdx], stateBehaviourPtr + lptr + 1, HWcNum - 1, actIdx, actDown, actUp, pulses, value);
                  if (retValue == 0)
                    retValue = retValueT; // Use first ever return value in case of multiple actions.
#endif
                  break;
                case SK_DEV_SMARTSCOPE:
#if SK_DEVICES_SMARTSCOPE
                  retValueT = evaluateAction_SMARTSCOPE(deviceMap[devIdx], stateBehaviourPtr + lptr + 1, HWcNum - 1, actIdx, actDown, actUp, pulses, value);
                  if (retValue == 0)
                    retValue = retValueT; // Use first ever return value in case of multiple actions.
#endif
                  break;
                  case SK_DEV_BMDCAMCTRL:
  #if SK_DEVICES_BMDCAMCTRL
                    retValueT = evaluateAction_BMDCAMCTRL(deviceMap[devIdx], stateBehaviourPtr + lptr + 1, HWcNum - 1, actIdx, actDown, actUp, pulses, value);
                    if (retValue == 0)
                      retValue = retValueT; // Use first ever return value in case of multiple actions.
  #endif
                    break;
                }
              } else {
                // Serial << "Device disabled!\n";
              }
            } else if (devIdx == 15) {
              retValueT = evaluateAction_system(stateBehaviourPtr + lptr + 1, HWcNum - 1, actIdx, actDown, actUp, pulses, value);
              if (retValue == 0)
                retValue = retValueT; // Use first ever return value in case of multiple actions.
            }
          } else if ((specificAction == 0 && matchShiftValue < sShift) || (specificAction > 0 && specificAction < actIdx + 1)) {
            break;
          }

          lptr += (globalConfigMem[stateBehaviourPtr + lptr] >> 5) + 2; // Add length
          actIdx++;
        }
        if (!shiftLevelMatch) {
          matchShiftValue = 0;
        }
      }
    }

    deviceRunLoop();

    if (actionMirror == 0) { // If no mirror was specified, just return value
      return retValue;
    } else { // If a mirror was specified, change HWcNum
      HWcNum = actionMirror;
      actionMirror = 0;
    }
    actionMirrorC++;
  } while (actionMirrorC < 4); // Allow 4 recursions
  return 0;
}

/**
 */
uint16_t getNumOfActions(const uint8_t HWcNum) {
  if (HWcNum > SK_HWCCOUNT || HWcNum == 0)
    return 0; // Invalid values check - would leak memory if not checked. HWcNum is passed on minus 1 (incoming range is 1-x, outgoing range is 0-(x-1))

  uint16_t stateBehaviourPtr = getConfigMemIndex(HWcNum - 1, _systemState); // Fetching pointer to state behaviour
  if (stateBehaviourPtr > 0) {                                              // If there is some configuration to relate to....
    uint8_t stateLen = globalConfigMem[stateBehaviourPtr - 1];

    uint8_t lptr = 0;
    uint8_t actIdx = 0;
    while (lptr < stateLen) {
      if (actIdx >= SK_MAXACTIONS)
        break; // actIdx at or over SK_MAXACTIONS would result in memory leaks in various evaluation functions which would trust HWc and actIdx to not exceed the bounds of the _systemHWcActionCache array

      lptr += (globalConfigMem[stateBehaviourPtr + lptr] >> 5) + 2; // Add length
      actIdx++;
    }
    return actIdx;
  }
  return 0;
}

/************************************
 *
 * INITIALIZE IT ALL
 *
 ************************************/

/**
 * First thing to call in setup() is the device initialization:
 */
void initController() {

  // Random Seed:
  randomSeed(analogRead(0) + analogRead(1) + analogRead(5) + analogRead(6) + analogRead(7));
  memset(_systemHWcActionPrefersLabel, 0, SK_HWCCOUNT);

  // Initialize serial communication at 115200 bits per second:
  Serial.begin(115200);
  Serial << F("\n\n*****************************\nSKAARHOJ Controller Booting \n*****************************\n");

// Setup Config:
#if SK_ETHMEGA
  pinMode(A1, INPUT_PULLUP); // CFG input on ethermega MMBOS
#else
  pinMode(18, INPUT);  // CFG input (active low)  - if you set it to INPUT_PULLUP, the resistor on the Bottom part will not be strong enough to pull it down!!
#endif

// Setup LED pins:
#if SK_ETHMEGA
  pinMode(3, OUTPUT); // Red Status LED, active high
  pinMode(2, OUTPUT); // Green Status LED, active high
#else
  statusLED(LED_OFF);  // To prevent brief light in LEDs upon boot
  pinMode(13, OUTPUT); // Red Status LED, active low
  pinMode(15, OUTPUT); // Green Status LED, active low
  pinMode(14, OUTPUT); // Blue Status LED, active low
#endif

  HWcfgDisplay();

  statusLED(LED_PURPLE); // Normal mode, so far...

  // I2C setup:
  Wire.begin();
  statusLED(QUICKBLANK);

  // Initializes the actual hardware components / modules on the controller
  configMode = isConfigButtonPushed(); // Temporary - to inspire an initialization cycle
  uint8_t buttonPressUponBoot = HWsetup();
  configMode = 0;
  statusLED(QUICKBLANK);

  // Check Config Button press (or hardware button press)
  delay(500); // Let people have time to release the button in case they just want to reset
  if (isConfigButtonPushed() || buttonPressUponBoot > 0) {

    configMode = EEPROM.read(0) == 2 ? 2 : (buttonPressUponBoot > 1 ? 2 : 1); // Current IP address
    if (EEPROM.read(0) != 0)
      EEPROM.write(0, 0);
    statusLED(LED_BLUE); // In config mode 1, LED will be blue
    unsigned long cfgButtonPressTime = millis();
    Serial << F("Config Mode=1\n");

    while (configMode == 2 || isConfigButtonPushed()) {
      if (configMode == 2 || (millis() - cfgButtonPressTime > 2000)) {
        statusLED(LED_WHITE); // In config mode 2, LED will be white and blink
        Serial << F("Config Mode=2\n");
        configMode = 2;
        break;
      }
    }
  } else {
    statusLED(LED_PURPLE); // Normal mode
  }

  loadPreset(); // Current preset
                //  Serial << "sizeof: " << sizeof(defaultControllerConfig) << "\n";
                //  presetCheck();

  Serial << F("HWvar:") << EEPROM.read(9) << F("\n");

  // Setting IP:
  if (configMode != 2) {
    ip[0] = globalConfigMem[getConfigMemIPIndex()];
    ip[1] = globalConfigMem[getConfigMemIPIndex() + 1];
    ip[2] = globalConfigMem[getConfigMemIPIndex() + 2];
    ip[3] = globalConfigMem[getConfigMemIPIndex() + 3];
  }
  Serial << F("IP address: ");
  for (uint8_t i = 0; i < 4; i++) {
    Serial << ip[i] << (i != 3 ? F(".") : F("\n"));
  }

  // Setting Subnet
  if (configMode != 2) {
    subnet[0] = globalConfigMem[getConfigMemIPIndex() + 4];
    subnet[1] = globalConfigMem[getConfigMemIPIndex() + 5];
    subnet[2] = globalConfigMem[getConfigMemIPIndex() + 6];
    subnet[3] = globalConfigMem[getConfigMemIPIndex() + 7];
  }
  Serial << F("Subnet mask: ");
  for (uint8_t i = 0; i < 4; i++) {
    Serial << subnet[i] << (i != 3 ? F(".") : F("\n"));
  }

  // Setting MAC:
  mac[0] = EEPROM.read(10);
  mac[1] = EEPROM.read(11);
  mac[2] = EEPROM.read(12);
  mac[3] = EEPROM.read(13);
  mac[4] = EEPROM.read(14);
  mac[5] = EEPROM.read(15);
  //  if (getConfigMode())  {
  Serial << F("MAC address: ");
  for (uint8_t i = 0; i < 6; i++) {
    Serial << _HEXPADL(mac[i], 2, "0") << (i != 5 ? F(":") : F("\n"));
  }
  //  }

  // TODO: Add subnet - but only if it's valid!!
  Ethernet.begin(mac, ip, subnet);
  statusLED(QUICKBLANK);
  delay(500);

  // Check if debug mode is on, if so reset it for next reset:
  if (EEPROM.read(1) != 0) {
    Serial << F("Debug Mode=1\n");
    debugMode = true;
    EEPROM.write(1, 0);
  }

  initActionCache();

  // Memory:
  if ((193 ^ EEPROM.read(16) ^ EEPROM.read(17) ^ EEPROM.read(18) ^ EEPROM.read(19)) == EEPROM.read(20)) {
    for (uint8_t i = 0; i < 4; i++) {
      _systemMem[i] = EEPROM.read(16 + i);
    }
    Serial << F("Memory A-D restored\n");
  } else {
    for (uint8_t i = 0; i < 4; i++) {
      storeMemory(i);
    }
    Serial << F("Memory A-D initialized\n");
  }
}
