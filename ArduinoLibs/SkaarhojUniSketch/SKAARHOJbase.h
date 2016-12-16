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

#if defined(ARDUINO_SKAARDUINO_DUE)
#include <avr/dtostrf.h>
void resetFunc() {
  EEPROM.commitPage();
  const int16_t RSTC_KEY = 0xA5;
  RSTC->RSTC_CR = RSTC_CR_KEY(RSTC_KEY) | RSTC_CR_PROCRST | RSTC_CR_PERRST;
  while (true)
    ;
}
#else
void (*resetFunc)(void) = 0; // declare reset function @ address 0
#endif

#define EEPROM_PRESET_START 100
#define EEPROM_PRESET_TOKEN 0x24 // Just some random value that is used for a checksum offset. Change this and existing configuration will be invalidated and has to be rewritten.

// The constrain macro takes up too much ram
#define constrain constrain
int32_t constrain(int32_t amt, int32_t low, int32_t high) { return ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt))); }

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

#if defined(ARDUINO_SKAARDUINO_DUE)
static const uint8_t PIN_RED = 2;
static const uint8_t PIN_GREEN = 22;
static const uint8_t PIN_BLUE = 13;
#else
static const uint8_t PIN_RED = 13;
static const uint8_t PIN_GREEN = 15;
static const uint8_t PIN_BLUE = 14;
#endif

#if SK_MODCOUNT > 0

#define SK_MODULAR_CSCSEED B10101101
#define SK_MODULAR_CLKPIN 19 // 10
#define SK_MODULAR_ENPIN 18  // 11

#endif

// Pre declaration
void deviceDebugLevel(uint8_t debugLevel);
uint16_t getPresetOffsetAddress(uint8_t presetNum);
uint8_t getNumberOfPresets();
uint16_t getConfigMemIPIndex();
uint8_t getCurrentPreset();
uint16_t getConfigMemDevIndex(uint8_t devNum);
uint16_t getPresetLength(uint8_t preset);
void savePreset(uint8_t presetNum, uint16_t len);
void statusLED(uint8_t incolor = 255, uint8_t inblnk = 255);
bool checkIncomingSerial();
void clearUserMemory();

uint16_t fletcher16(uint8_t *data, int16_t count) {
  uint16_t sum1 = 0;
  uint16_t sum2 = 0;
  int16_t index;

  for (index = 0; index < count; ++index) {
    sum1 = (sum1 + data[index]) % 255;
    sum2 = (sum2 + sum1) % 255;
  }

  return (sum2 << 8) | sum1;
}

uint16_t fletcher16stream(uint8_t byte, bool lastByte = false) {
  static uint16_t s1 = 0, s2 = 0;

  s1 = (s1 + byte) % 255;
  s2 = (s2 + s1) % 255;

  if (lastByte) {
    uint16_t returnValue = (s2 << 8) | s1;
    s1 = 0;
    s2 = 0;
  }
}

void setDeviceState(uint8_t device, bool enabled) {
  if (device > 0 && device < SK_DEVICES) {
    globalConfigMem[getConfigMemDevIndex(device - 1)] = enabled;
    savePreset(getCurrentPreset(), getPresetLength(getCurrentPreset()));
  }
}

void setIP(uint8_t *ip) {
  for (uint8_t i = 0; i < 4; i++) {
    globalConfigMem[getConfigMemIPIndex() + i] = ip[i];
  }
  savePreset(getCurrentPreset(), getPresetLength(getCurrentPreset()));
}

void setDeviceIP(uint8_t device, uint8_t *ip) {
  if (device > 0 && device < SK_DEVICES) {
    for (uint8_t i = 0; i < 4; i++) {
      globalConfigMem[getConfigMemDevIndex(device - 1) + 1 + i] = ip[i];
    }
    savePreset(getCurrentPreset(), getPresetLength(getCurrentPreset()));
  }
}

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
    if ((c == 10 || c == 13)) {
      if (serialBufferPointer > 0) { // Ignore consecutive newline/carriage returns
        serialBufferPointer = 0;     // so, we can start over again filling the buffer
        return true;
      }
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

void setAnalogComponentCalibration(uint16_t num, uint16_t start, uint16_t end, uint8_t hysteresis) {
  num -= 1;
  if (num > 9)
    return;

  Serial << F("Saving calibration for analog component #") << num + 1 << "\n";
  EEPROM.write(20 + num * 4 + 1, start >> 1 & 0xFF);
  EEPROM.write(20 + num * 4 + 2, end >> 1 & 0xFF);
  EEPROM.write(20 + num * 4 + 3, hysteresis & 0x3F | (start & 1) << 7 | (end & 1) << 6);

  EEPROM.write(20 + num * 4 + 4, 193 ^ EEPROM.read(20 + num * 4 + 1) ^ EEPROM.read(20 + num * 4 + 2) ^ EEPROM.read(20 + num * 4 + 3));
}

void clearAnalogComponentCalibration(uint16_t num) {
  uint16_t *minimumValues = HWMinCalibrationValues(num);
  setAnalogComponentCalibration(num, minimumValues[0], minimumValues[1], minimumValues[2]);
}

uint16_t *getAnalogComponentCalibration(uint8_t num) {
  static uint16_t *calibration = HWMinCalibrationValues(num);

  num -= 1;
  if (num > 9)
    return calibration;

  uint8_t c1, c2, c3, c4;
  c1 = EEPROM.read(20 + num * 4 + 1);
  c2 = EEPROM.read(20 + num * 4 + 2);
  c3 = EEPROM.read(20 + num * 4 + 3);
  c4 = EEPROM.read(20 + num * 4 + 4);

  if ((193 ^ c1 ^ c2 ^ c3) != c4) {
    Serial << F("Initialized calibration for analog component ") << num + 1 << "\n";
    setAnalogComponentCalibration(num + 1, calibration[0], calibration[1], calibration[2]);
  }

  calibration[0] = EEPROM.read(20 + num * 4 + 1) << 1 | EEPROM.read(20 + num * 4 + 3) >> 7;     // Start
  calibration[1] = EEPROM.read(20 + num * 4 + 2) << 1 | EEPROM.read(20 + num * 4 + 3) >> 6 & 1; // End
  calibration[2] = EEPROM.read(20 + num * 4 + 3) & 0x3F;                                        // Hysteresis

  Serial << "Calibration for analog component #" << num + 1 << ": Start: " << calibration[0] << ", End: " << calibration[1] << ", Hysteresis: " << calibration[2] << "\n";
  return calibration;
}

uint8_t currentAnalogComponent = 0;
unsigned long lastAnalogPrint = 0;
uint8_t hysteresis = 0;
int16_t prevSensorValue;
int16_t recentDiffValues[40];
uint16_t recentDiffValuesCounter = 0;
uint8_t initialDebugState;
// uint16_t hysteresis = 0;

void listAnalogHWComponent(uint8_t num = 0) {
  if (num > 0) {
    currentAnalogComponent = num;

    initialDebugState = debugMode;
    deviceDebugLevel(0);
    debugMode = 0;
  }

  // Read the input on analog pin 0:
  int16_t sensorValue = HWAnalogComponentValue(currentAnalogComponent); // Change this to the analog input pin used by the T-bar/Slider (if different)

  // Print out the value you read:
  Serial.print(sensorValue);

  // Print padding:
  if (sensorValue < 1000) {
    Serial.print(" ");
  }
  if (sensorValue < 100) {
    Serial.print(" ");
  }
  if (sensorValue < 10) {
    Serial.print(" ");
  }

  Serial.print(": ");

  // Min/Max:
  int16_t maxValue = 0;
  int16_t minValue = 0;
  for (int16_t i = 0; i < 40; i++) {
    if (recentDiffValues[i] > maxValue) {
      maxValue = recentDiffValues[i];
    }
    if (recentDiffValues[i] < minValue) {
      minValue = recentDiffValues[i];
    }
  }
  Serial.print("[ ");
  Serial.print(minValue);
  Serial.print(" | ");
  Serial.print(maxValue);
  Serial.print(" ] ");

  if (minValue > -1000) {
    Serial.print(" ");
  }
  if (minValue > -100) {
    Serial.print(" ");
  }
  if (minValue > -10) {
    Serial.print(" ");
  }
  if (minValue == 0) {
    Serial.print(" ");
  }
  if (maxValue < 1000) {
    Serial.print(" ");
  }
  if (maxValue < 100) {
    Serial.print(" ");
  }
  if (maxValue < 10) {
    Serial.print(" ");
  }

  Serial.print(" ");

  // Meter:
  Serial.print(sensorValue - prevSensorValue + 15 + 1 <= 0 ? "!" : " ");
  for (int16_t i = 15; i > 0; i--) {
    Serial.print(sensorValue - prevSensorValue + i <= 0 ? "=" : " ");
  }

  Serial.print("|");

  for (int16_t i = 1; i <= 15; i++) {
    Serial.print(sensorValue - prevSensorValue - i >= 0 ? "=" : " ");
  }
  Serial.print(sensorValue - prevSensorValue - 15 - 1 >= 0 ? "!" : " ");

  Serial.print(" (");
  Serial.print(sensorValue - prevSensorValue);
  Serial.print(" )");
  Serial.println("");

  recentDiffValues[(recentDiffValuesCounter % 40)] = sensorValue - prevSensorValue;
  recentDiffValuesCounter++;

  prevSensorValue = sensorValue;
}

void hideAnalogHWComponent() {
  currentAnalogComponent = 0;
  deviceDebugLevel(initialDebugState);
  debugMode = initialDebugState;
  // average = 0xFFFF;
}

uint8_t calibrationState = 0;

bool sampleComponent(uint8_t num, uint16_t *average, uint16_t *maxDeviation) {
  static uint8_t avgCounter = 0;
  static uint8_t sampleCounter = 0;

  if (avgCounter++ < 10) {
    *average += HWAnalogComponentValue(currentAnalogComponent);
  } else {
    if (sampleCounter++ == 0) {
      *average = *average / 10;
    } else if (sampleCounter < 50) {
      int16_t deviation = HWAnalogComponentValue(currentAnalogComponent) - *average;

      if (abs(deviation) > *maxDeviation) {
        *maxDeviation = abs(deviation);
      }
    } else {
      Serial << F("Results are average = ") << average[0] << " and max deviation = " << maxDeviation[0] << "\n";
      calibrationState++;
      avgCounter = 0;
      sampleCounter = 0;
      return true;
    }
  }

  return false;
}

uint8_t serialState = 0; // 1: show analog, 2: calibrate

void calibrateAnalogHWComponent(uint8_t num = 0) {
  static uint16_t average[5];
  static uint16_t maxDeviation[5];
  static uint16_t start, end, hysteresis;
  static uint8_t initialDebugState = debugMode;
  uint16_t *minimumValues;

  if (num > 0) {
    currentAnalogComponent = num;
    calibrationState = 1;
  }

  if (num > HWnumOfAnalogComponents()) {
    Serial << F("Analog component #") << num << F(" does not exist!\n");
    return;
  }

  switch (calibrationState) {
  case 1: // Start calibration
    deviceDebugLevel(0);
    debugMode = 0;
    memset(average, 0x00, sizeof(average));
    memset(maxDeviation, 0x00, sizeof(maxDeviation));

    calibrationState = 2;
    Serial << F("Calibrating analog component #") << currentAnalogComponent << ". Please move the control to the first endpoint, and send 'ok'\n";
    break;
  case 3: // First endpoint 1 sampling
    if (sampleComponent(currentAnalogComponent, average, maxDeviation)) {
      Serial << F("Please move the control to other endpoint, and send 'ok'\n");
    }
    break;
  case 5: // First endpoint 2 sampling
    if (sampleComponent(currentAnalogComponent, average + 1, maxDeviation + 1)) {
      Serial << F("Please move the control to the first endpoint, and send 'ok'\n");
    }
    break;
  case 7: // Second endpoint 1 sampling
    if (sampleComponent(currentAnalogComponent, average + 2, maxDeviation + 2)) {
      Serial << F("Please move the control to the other endpoint, and send 'ok'\n");
    }
    break;
  case 9: // Second endpoint 2 sampling
    if (sampleComponent(currentAnalogComponent, average + 3, maxDeviation + 3)) {
      Serial << F("Please move the control to a roughly central position, and send 'ok'\n");
    }
    break;
  case 11: // Central position sampling
    if (sampleComponent(currentAnalogComponent, average + 4, maxDeviation + 4)) {
      Serial << F("Please move the control to a roughly central position, and send 'ok'\n");
    }
    break;
  case 12:
    int16_t a, b;
    a = (average[0] + average[2]) / 2;
    b = (average[1] + average[3]) / 2;

    if (a > b) {
      start = b;
      end = (1023 - a);
    } else {
      start = a;
      end = (1023 - b);
    }

    hysteresis = 0;
    for (uint8_t i = 0; i < 5; i++) {
      if (maxDeviation[i] > hysteresis) {
        hysteresis = maxDeviation[i];
      }
    }

    if (hysteresis == 0) {
      hysteresis = 1;
    }

    hysteresis = hysteresis * 2;

    start += hysteresis;
    end += hysteresis;

    minimumValues = HWMinCalibrationValues(currentAnalogComponent);

    if (start < minimumValues[0])
      start = minimumValues[0];
    if (end < minimumValues[1])
      end = minimumValues[1];
    if (hysteresis < minimumValues[2])
      hysteresis = minimumValues[2];

    Serial << F("Calibration results:\n");
    Serial << F("Start offset: ") << start << "\n";
    Serial << F("End offset: ") << end << "\n";
    Serial << F("Hysteresis: ") << hysteresis << " (Safety factor = 2)\n";

    Serial << F("\n Send 'ok' to save this calibration.");
    calibrationState++;
    break;
  case 14:
    Serial << F("Calibration saved! Please restart the controller for new new settings to take effect.\n");
    setAnalogComponentCalibration(currentAnalogComponent, start, end, hysteresis);
    serialState = 0;
    deviceDebugLevel(initialDebugState);
    debugMode = initialDebugState;
    break;
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
  int16_t HWcIndex = -1;

  while (ptr < SK_CONFIG_MEMORY_SIZE && globalConfigMem[ptr] != 255) { // Traverses HW components
    uint8_t HWcSegmentLength = globalConfigMem[ptr];
    uint16_t HWcSegmentStartPtr = ptr;
    HWcIndex++;
    int16_t stateIndex = -1;

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
 * 10-15: MAC address (+ checksum on 16, used in SKAARDUINO TestRig sketch only)
 * 16-20: Memory Bank A-D + checksum byte
 * 21-60: Analog Component calibration (10 pcs)
 *
 * EEPROM_PRESET_START: Start of presets
 */

void loadDefaultConfig() {
  // Copy default controller configuration to the global Config memory.
  memcpy_P(globalConfigMem, defaultControllerConfig, sizeof(defaultControllerConfig));
}
void moveEEPROMMemoryBlock(uint16_t from, uint16_t to, int16_t offset) { // From is inclusive, To is exclusive, To>From, offset>0 = move forward
                                                                         // Serial << "moveEEPROMMemoryBlock (" << from << "," << to << "," << offset << " )\n";
  if (offset > 0) {
    for (uint16_t a = to; a > from; a--) {
#ifdef ARDUINO_SKAARDUINO_DUE
      EEPROM.writeBuffered(a - 1 + offset, EEPROM.read(a - 1));
#else
      EEPROM.write(a - 1 + offset, EEPROM.read(a - 1));
#endif
    }
  } else if (offset < 0) {
    for (uint16_t a = from; a < to; a++) {
#ifdef ARDUINO_SKAARDUINO_DUE
      EEPROM.writeBuffered(a + offset, EEPROM.read(a));
#else
      EEPROM.write(a + offset, EEPROM.read(a));
#endif
    }
  }
#ifdef ARDUINO_SKAARDUINO_DUE
  EEPROM.commitPage();
#endif
}

uint16_t getPresetStoreLength() { return ((uint16_t)EEPROM.read(EEPROM_PRESET_START + 2) << 8) | EEPROM.read(EEPROM_PRESET_START + 3); }

uint16_t getPresetLength(uint8_t preset) { // Excluding CSC byte
  uint16_t presetOffset = getPresetOffsetAddress(preset);
  uint16_t presetLen = ((uint16_t)EEPROM.read(EEPROM_PRESET_START + 5 + presetOffset) << 8) | EEPROM.read(EEPROM_PRESET_START + 6 + presetOffset);

  return presetOffset - 1;
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
  //  Serial << "Writing CSC:" << csc << "\n";
  EEPROM.write(EEPROM_PRESET_START + 4, csc);
}
void setNumberOfPresets(uint8_t n) {
  EEPROM.write(EEPROM_PRESET_START, n); // Number of...
                                        //  Serial << "setNumberOfPresets:" << n << "\n";
  updatePresetChecksum();
}
void setCurrentPreset(uint8_t n) {
  EEPROM.write(EEPROM_PRESET_START + 1, n); // Current
                                            //  Serial << "setCurrentPreset:" << n << "\n";
  updatePresetChecksum();
}

uint8_t getCurrentPreset() { return EEPROM.read(EEPROM_PRESET_START + 1); }

void CurrentPreset(uint8_t n) {
  EEPROM.write(EEPROM_PRESET_START + 1, n); // Current
                                            //  Serial << "setCurrentPreset:" << n << "\n";
  updatePresetChecksum();
}

void setPresetStoreLength(uint16_t len) {
  EEPROM.write(EEPROM_PRESET_START + 2, highByte(len));
  EEPROM.write(EEPROM_PRESET_START + 3, lowByte(len));
  //  Serial << "setPresetStoreLength:" << len << "\n";
  updatePresetChecksum();
}

void clearPresets() {
  setNumberOfPresets(0);
  setCurrentPreset(0);
  setPresetStoreLength(0);
}

uint8_t getNumberOfPresets() {
  bool presetsLoaded = false;

  for (uint8_t i = 0; i < 2; i++) {
    uint8_t csc = EEPROM_PRESET_TOKEN;
    for (uint8_t a = 0; a < 5; a++) {
      csc ^= EEPROM.read(EEPROM_PRESET_START + a);
    }
    if (csc != 0) {
      Serial << F("Presets checksum mismatch. Attempt #") << i << " Data: ";
      for (uint8_t j = 0; j < 5; j++) {
        Serial << _HEXPADL(EEPROM.read(EEPROM_PRESET_START + j), 2, "0") << (j < 4 ? ":" : "");
      }
      Serial << "\n";

      delay(20);
    } else {
      presetsLoaded = true;
      break;
    }
  }

  if (!presetsLoaded) {
    Serial << F("Could not read presets in 2 tries. Serial recovery mode.\n");
    while (true) {
      statusLED(millis() & 128 ? LED_RED : LED_OFF);
      checkIncomingSerial();
    }
  }

  return EEPROM.read(EEPROM_PRESET_START);
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
  if (getNumberOfPresets() > 0) { // If there are valid presets, consider to load one...
                                  //    Serial << "There are valid presets, in fact "<< getNumberOfPresets() << " are found.\n";
    if (presetNum == 0) {         // Load current preset if nothing is given
      presetNum = EEPROM.read(EEPROM_PRESET_START + 1);
      //      Serial << "Current preset: "<< presetNum << "\n";
    }
    //    Serial << "Preset: "<< presetNum << "\n";
    if (presetNum > 0 && presetNum <= getNumberOfPresets()) { // preset num must be equal to or less than the total number
      uint16_t presetOffset = getPresetOffsetAddress(presetNum);
      //      Serial << "presetOffset: "<< presetOffset << "\n";
      uint16_t presetLen = ((uint16_t)EEPROM.read(EEPROM_PRESET_START + 5 + presetOffset) << 8) | EEPROM.read(EEPROM_PRESET_START + 6 + presetOffset);
      //      Serial << "presetLen: "<< presetLen << "\n";
      uint8_t csc = EEPROM_PRESET_TOKEN;
      for (uint16_t a = 0; a < presetLen; a++) {
        if (a >= SK_CONFIG_MEMORY_SIZE)
          break;                                                                      // ERROR!
        globalConfigMem[a] = EEPROM.read(EEPROM_PRESET_START + 7 + presetOffset + a); // Loading memory with preset, byte by byte.
        csc ^= globalConfigMem[a];
      }
      if (csc == 0) {
        //        Serial << "All good...\n";
        // All is good, exit with the new config in memory!
        Serial << F("Preset ") << presetNum << F(" loaded\n");
        return true;
      }
    }
  }
  //  Serial << "Load default config...";
  loadDefaultConfig();
  return false;
}

void savePreset(uint8_t presetNum, uint16_t len) { // Len is excluding CSC byte. presetNum=255 to make new preset.
                                                   //   Serial << "savePreset() \n";
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
                                              //        Serial << "New preset: " << presetNum << "\n";
        setNumberOfPresets(presetNum);
        setCurrentPreset(presetNum);
        presetOffset = getPresetOffsetAddress(presetNum);
        //        Serial << "New presetOffset: " << presetOffset << "\n";

        setPresetStoreLength(presetOffset + len + 1 + 2);
        //  Serial << "getPresetStoreLength(): " << getPresetStoreLength() << "\n";
      }
    } else {
      setCurrentPreset(presetNum);
      // Serial << "Existing preset: " << presetNum << "\n";
      presetOffset = getPresetOffsetAddress(presetNum);
      //    Serial << "New presetOffset: " << presetOffset << "\n";
      uint16_t nextPresetOffset = getPresetOffsetAddress(presetNum + 1);
      //    Serial << "nextPresetOffset: " << nextPresetOffset << "\n";
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
#ifdef ARDUINO_SKAARDUINO_DUE
        EEPROM.writeBuffered(EEPROM_PRESET_START + 7 + presetOffset + a, globalConfigMem[a]); // Loading memory with preset, byte by byte.
#else
        EEPROM.write(EEPROM_PRESET_START + 7 + presetOffset + a, globalConfigMem[a]);
#endif
        csc ^= globalConfigMem[a];
      }
#ifdef ARDUINO_SKAARDUINO_DUE
      EEPROM.commitPage();
#endif
      EEPROM.write(EEPROM_PRESET_START + 7 + presetOffset + len, csc); // Checksum byte
      EEPROM.write(EEPROM_PRESET_START + 5 + presetOffset, highByte(len + 1));
      EEPROM.write(EEPROM_PRESET_START + 6 + presetOffset, lowByte(len + 1));
    }
  }
}

uint8_t hexcharToNibble(char c) {
  if (c >= '0' && c <= '9') {
    return c - '0';
  }
  if (c >= 'A' && c <= 'F') {
    return c - 'A' + 10;
  }
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
    } else if (!strncmp(serialBuffer, "clearusermemory", 15)) {
      Serial << "Clearing user memory...\n";
      clearUserMemory();
    } else if (!strncmp(serialBuffer, "reset", 5)) {
      Serial << F("Resetting...\n");
      delay(1000);
      resetFunc();
    } else if (!strncmp(serialBuffer, "list analog", 11)) {
      char buffer[10];
      Serial << F("Number of analog components: ") << HWnumOfAnalogComponents() << "\n";
      for (uint8_t i = 1; i <= HWnumOfAnalogComponents(); i++) {
        HWanalogComponentName(i, buffer, 10);
        Serial << F("Analog #") << i << F(": ") << buffer;

        uint16_t calibration[3];
        calibration[0] = EEPROM.read(20 + (i - 1) * 4 + 1) << 1 | EEPROM.read(20 + (i - 1) * 4 + 3) >> 7;     // Start
        calibration[1] = EEPROM.read(20 + (i - 1) * 4 + 2) << 1 | EEPROM.read(20 + (i - 1) * 4 + 3) >> 6 & 1; // End
        calibration[2] = EEPROM.read(20 + (i - 1) * 4 + 3) & 0x3F;                                            // Hysteresis

        Serial << ": Start: " << calibration[0] << ", End: " << calibration[1] << ", Hysteresis: " << calibration[2] << "\n";
      }
    } else if (!strncmp(serialBuffer, "show analog ", 12)) {
      uint8_t num = serialBuffer[12] - '0';
      if (num == 0) {
        Serial << F("Invalid analog component number\n");
      } else {
        Serial << F("Analog component chosen: ") << num << "\n";
        listAnalogHWComponent(num);
        serialState = 1;
      }
    } else if (!strncmp(serialBuffer, "clear analog", 12)) {
      if (serialBuffer[13] == 0) { // reset all
        Serial << F("Clearing all analog component calibrations...\n");
        for (uint8_t i = 0; i < HWnumOfAnalogComponents(); i++) {
          clearAnalogComponentCalibration(i + 1);
        }
      } else {
        uint8_t num = serialBuffer[13] - '0';
        if (num == 0 || num >= HWnumOfAnalogComponents()) {
          Serial << F("Invalid analog component number\n");
        } else {
          Serial << F("Resetting analog component ") << num << "\n";
          clearAnalogComponentCalibration(num);
        }
      }
    } else if (!strncmp(serialBuffer, "set analog ", 11)) {
      uint16_t cal[3];
      uint8_t num = serialBuffer[11] - '0';
      char *tok = strtok(serialBuffer + 13, ",");
      uint8_t index = 0;
      while (tok != NULL && index < 3) {
        cal[index++] = atoi(tok);
        tok = strtok(NULL, ",");
      }
      if (index == 3) {
        setAnalogComponentCalibration(num, cal[0], cal[1], cal[2]);
      } else {
        Serial << F("ERROR: Invalid command format\n");
      }
    } else if (!strncmp(serialBuffer, "hide analog", 11)) {
      hideAnalogHWComponent();
      serialState = 0;
    } else if (!strncmp(serialBuffer, "calibrate analog ", 17)) {
      uint8_t num = serialBuffer[17] - '0';
      if (num == 0) {
        Serial << F("Invalid analog component number\n");
      } else {
        calibrateAnalogHWComponent(num);
        serialState = 2;
      }
    } else if (!strncmp(serialBuffer, "ok", 2)) {
      if (serialState == 2) { // Calibration in progress
        calibrationState++;
      }
    } else if (!strncmp(serialBuffer, "exportPresets", 13)) {
      Serial << "\n ---------  START OF PRESET DUMP ---------\n";
      uint16_t presetLength = getPresetStoreLength();
      for (uint16_t i = 0; i < presetLength; i++) {
        if (i % 8 == 0) {
          Serial << "\n";
        }
        uint8_t byte = EEPROM.read(EEPROM_PRESET_START + i);
        Serial << "0x" << _HEXPADL(byte, 2, "0") << ", ";
        if (i == presetLength - 1) {
          uint16_t checksum = fletcher16stream(byte, true);
          Serial << "0x" << _HEXPADL(checksum >> 8, 2, "0") << ", 0x" << _HEXPADL(checksum & 0xFF, 2, "0") << "\n";
        } else {
          fletcher16stream(byte);
        }
      }

      Serial << "\n ---------  END OF PRESET DUMP ---------\n";
    } else if (!strncmp(serialBuffer, "importPresets", 13)) {
      uint16_t pos = 0;
      uint16_t len = 0;
      uint8_t nextByte = 0;
      uint8_t state = 0;
      bool gotLength = false;
      bool receivedData = false;

      Serial << "Please paste the preset configuration into the serial console:\n\n";

      uint16_t checksum;
      uint16_t incomingChecksum;
      while (true) {
        if (!(Serial.available() > 0))
          continue;

        char c = Serial.read();
        if (c == 'x') {
          state = 1;
          continue;
        }
        if (c == '10' || c == '13')
          continue;

        switch (state) {
        case 1: // First char of ASCII encoded byte
          nextByte = hexcharToNibble(c) << 4;
          state = 2;
          break;
        case 2: // Second char
          nextByte |= hexcharToNibble(c) & 0xF;
          // Serial << "Received byte " << pos << ": " << nextByte << "\n";
          if (pos == 2) {
            len = nextByte << 8;
          } else if (pos == 3) {
            len |= nextByte;
            Serial << "Expecting to receive " << len << " + 2 bytes \n";
            gotLength = true;
          }

          if (gotLength) {
            if (pos == len - 1) {
              Serial << "Received main payload. Validating presets..\n";
              EEPROM.write(EEPROM_PRESET_START + pos, nextByte);
              checksum = fletcher16stream(nextByte, true);
              receivedData = true;
            }
            if (pos == len) {
              incomingChecksum = nextByte << 8;
            } else if (pos == len + 1) {
              incomingChecksum |= nextByte;
              if (checksum != incomingChecksum) {
                Serial << "Checksum mismatch! Clearing presets..\n";
                delay(200);
                clearPresets();
                resetFunc();
              } else {
                Serial << "Checksum matched! Rebooting controller..";
                delay(200);
                resetFunc();
              }
            }
          }

          if (!receivedData) {
            fletcher16stream(nextByte, false);
            EEPROM.write(EEPROM_PRESET_START + pos, nextByte);
          }

          pos++;
          state = 0;
          break;
        }
      }
    } else if (!strncmp(serialBuffer, "preset ", 7)) {
      uint8_t num = serialBuffer[7] - '0';
      char name[22];
      if (num > 0 && num <= getNumberOfPresets()) {
        getPresetName(name, num);
        Serial << "Setting preset: " << name << "\n";
        setCurrentPreset(num);
        delay(200);
        resetFunc();
      } else {
        Serial << "Invalid preset index\n";
      }
    } else if (!strncmp(serialBuffer, "ip=", 3)) {
      uint8_t ip[4];
      char *tok = strtok(serialBuffer + 3, ".");
      uint8_t index = 0;
      while (tok != NULL && index < 4) {
        ip[index++] = atoi(tok);
        tok = strtok(NULL, ".");
      }
      if (index == 4) {
        Serial << "Setting IP...\n";
        setIP(ip);
        delay(200);
        resetFunc();
      } else {
        Serial << "Invalid IP specified\n";
      }
    } else if (!strncmp(serialBuffer, "ipDevice", 8)) {
      uint8_t num = serialBuffer[8] - '0';
      if (num > SK_DEVICES || num < 1) {
        Serial << "Invalid device selected\n";
      } else {
        uint8_t ip[4];
        char *tok = strtok(serialBuffer + 10, ".");
        uint8_t index = 0;
        while (tok != NULL && index < 4) {
          ip[index++] = atoi(tok);
          tok = strtok(NULL, ".");
        }
        if (index == 4) {
          Serial << "Setting IP for device " << num << " \n";
          setDeviceIP(num, ip);
          delay(200);
          resetFunc();
        } else {
          Serial << "Invalid IP specified\n";
        }
      }
    } else if (!strncmp(serialBuffer, "enableDevice", 12)) {
      uint8_t num = serialBuffer[12] - '0';
      uint8_t enable = serialBuffer[14] - '0';

      if (num > SK_DEVICES || num < 1) {
        Serial << "Invalid device selected\n";
      } else {
        setDeviceState(num, enable);
        Serial << (enable ? "Enabling" : "Disabling") << " device " << num << "\n";
        delay(200);
        resetFunc();
      }
    } else if (!strncmp(serialBuffer, "_resetAll", 9)) {
      Serial << "Clearing user memory...\n";
      clearUserMemory();
      Serial << "Clearing presets...\n";
      clearPresets();
      Serial << "Clearing Mem A-D...\n";
      EEPROM.write(20, EEPROM.read(20) + 1);

      for (uint8_t i = 0; i < HWnumOfAnalogComponents(); i++) {
        Serial << "Clearing calibration for analog component #" << i + 1 << "\n";
        clearAnalogComponentCalibration(i + 1);
      }

      Serial << "\nController now rebooting...\n";
      delay(1000);
      resetFunc();
    } else {
      Serial << F("NAK\n");
    }
  }

  switch (serialState) {
  case 1: // List values
    listAnalogHWComponent();
    break;
  case 2: // Calibrate component
    calibrateAnalogHWComponent();
    break;
  }

  return false;
}

bool variantLED() {
#if defined(ARDUINO_SKAARDUINO_DUE)
  return false; // No change in LED type on SKAARDUINO DUE, just fixed.
#else
  return EEPROM.read(9) & 1;
#endif;
}

static uint32_t lastAlarmLED = 0;
void alarmLED() {
#if SK_ETHMEGA
  digitalWrite(3, 1);
  digitalWrite(2, 0);
#else
  static uint8_t grn = variantLED() ? PIN_BLUE : PIN_GREEN;
  static uint8_t blu = variantLED() ? PIN_GREEN : PIN_BLUE;
  digitalWrite(PIN_RED, 0);
  digitalWrite(grn, 1); // Green
  digitalWrite(blu, 1); // Blue
#endif

  lastAlarmLED = millis();
}

/**
 * StatusLED function. Call it without parameters to just update the LED flashing. Call it with parameters to set a new value.
 */
void statusLED(uint8_t incolor, uint8_t inblnk) {
  if (!sTools.hasTimedOut(lastAlarmLED, 200)) {
    return;
  }

  static uint8_t color = 0;
  static uint8_t blnk = 0;

  uint8_t grn = variantLED() ? PIN_BLUE : PIN_GREEN;
  uint8_t blu = variantLED() ? PIN_GREEN : PIN_BLUE;

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
      case 1:                     //  red
        digitalWrite(PIN_RED, 0); // Red
        digitalWrite(grn, 1);     // Green
        digitalWrite(blu, 1);     // Blue
        break;
      case 2:                     //  green
        digitalWrite(PIN_RED, 1); // Red
        digitalWrite(grn, 0);     // Green
        digitalWrite(blu, 1);     // Blue
        break;
      case 3:                     //  blue
        digitalWrite(PIN_RED, 1); // Red
        digitalWrite(grn, 1);     // Green
        digitalWrite(blu, 0);     // Blue
        break;
      case 4:                     //  white
        digitalWrite(PIN_RED, 0); // Red
        digitalWrite(grn, 0);     // Green
        digitalWrite(blu, 0);     // Blue
        break;
      case 5:                     //  yellow
        digitalWrite(PIN_RED, 0); // Red
        digitalWrite(grn, 0);     // Green
        digitalWrite(blu, 1);     // Blue
        break;
      case 6:                     //  cyan
        digitalWrite(PIN_RED, 1); // Red
        digitalWrite(grn, 0);     // Green
        digitalWrite(blu, 0);     // Blue
        break;
      case 7:                     //  purple
        digitalWrite(PIN_RED, 0); // Red
        digitalWrite(grn, 1);     // Green
        digitalWrite(blu, 0);     // Blue
        break;
      default:                    //  off
        digitalWrite(PIN_RED, 1); // Red
        digitalWrite(grn, 1);     // Green
        digitalWrite(blu, 1);     // Blue
        break;
      }
#endif
    } else {
#if SK_ETHMEGA
      digitalWrite(3, !1); // Red
      digitalWrite(2, !1); // Green
#else
      digitalWrite(PIN_RED, 1); // Red
      digitalWrite(grn, 1);     // Green
      digitalWrite(blu, 1);     // Blue
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
    if (debugMode) {
      Serial << counter << F("\n");
      Serial << F(".");
      printsSinceLastLinebreak++;
    }
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
  return (analogRead(A1) < 500) || (EEPROM.read(0) != 0);
#elif defined(ARDUINO_SKAARDUINO_DUE)
  return (!digitalRead(23)) || (EEPROM.read(0) != 0);
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
  int16_t tw = size > 0 ? 128 : 64; // Needs to be signed, as string length is subtracted

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
      dtostrf((float)_extRetValue[a] / 1000, 4, 2, _strCache); // Need to find alternative for Due Platform.
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
    int16_t xOffset = 0;
    if (strlen(_extRetTxt[a])) {
      if (_extRetPair > 0) {
        xOffset = constrain(strlen(_strCache) ? 2 : (tw >> 1) - (int16_t)constrain(strlen(_extRetTxt[a]), 1, tw / 6) * 3, 2, tw);
        disp.setCursor(xOffset, 14 - (isTitle ? 0 : 5) + a * 9 - hShrink);
      } else {
        xOffset = constrain(strlen(_strCache) ? 2 : (tw >> 1) - (int16_t)strlen((_extRetPair == 0 && size == 0) ? _extRetTxtShort : _extRetTxt[a]) * 6, 2, tw);
        disp.setCursor(xOffset, 16 - (isTitle ? 0 : 5) - hShrink);
      }
      disp << ((_extRetPair == 0 && size == 0) ? _extRetTxtShort : _extRetTxt[a]);
    }

    // Print value string:
    if (strlen(_strCache)) {
      if (_extRetPair > 0) {
        disp.setCursor(constrain(tw - 2 - (int16_t)strlen(_strCache) * 6 - wShrink, 0, 100), 14 - (isTitle ? 0 : 5) + a * 9 - hShrink);
        disp << _strCache;
      } else {
        xOffset = strlen(_extRetTxt[a]) ? tw - 2 - (int16_t)strlen(_strCache) * 12 : (tw >> 1) - (int16_t)strlen(_strCache) * 6;
        disp.setCursor(constrain(xOffset, 0, 100), 16 - (isTitle ? 0 : 5) - (_extRetScaleType > 0 ? 2 : 0) - hShrink);
        disp << _strCache;
        if ((_extRetFormat & 0xF) == 5) {
          disp.setTextSize(1);
          disp.setCursor(constrain(xOffset - 12, 0, 100), 13 - (isTitle ? 0 : 5) - hShrink);
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

        uint8_t w = constrain(((float)(_extRetValue[a] - _extRetRangeLow) / (_extRetRangeHigh - _extRetRangeLow) * tw), 0, tw - wShrink);
        if (w > 0) {
          if (_extRetScaleType == 1)
            disp.fillRoundRect(0, 29 - hShrink, w, 3, 0, WHITE); // In-fill
        }
        if (_extRetScaleType == 2)
          disp.fillRoundRect(constrain(w - 1, 0, tw - 3), 29 - hShrink, 3, 3, 0, WHITE); // In-fill

        if (_extRetRangeHigh != _extRetLimitHigh) {
          uint8_t w = constrain(((float)(_extRetLimitHigh - _extRetRangeLow) / (_extRetRangeHigh - _extRetRangeLow) * tw), 0, tw - wShrink);
          disp.fillRoundRect(constrain(w, 0, tw - 1 - wShrink), 28 - hShrink, 1, 3, 0, WHITE); // In-fill
        }
        if (_extRetRangeLow != _extRetLimitLow) {
          uint8_t w = constrain(((float)(_extRetLimitLow - _extRetRangeLow) / (_extRetRangeHigh - _extRetRangeLow) * tw), 0, tw - wShrink);
          disp.fillRoundRect(constrain(w, 0, tw - 1 - wShrink), 28 - hShrink, 1, 3, 0, WHITE); // In-fill
        }
      }
    }
  }
  if (_extRetValIsLabel) {
    disp.drawRoundRect(0, 0, tw - wShrink, 32 - hShrink, 1, WHITE);
  }
  if (dispMask) {
    disp.display(dispMask);
  }
}
#endif
#if (SK_HWEN_MENU)
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
      dtostrf((float)_extRetValue[a] / 1000, 4, 2, _strCache);
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
 * EEPROM USER MEMORY
 *
 ************************************/

bool userMemoryExists(uint8_t index, uint8_t type) {
  if (index < EEPROM_FILEBANK_NUM) {
    if (EEPROM.read(EEPROM_FILEBANK_START + index * 48) == type) {
      return true;
    }
  }
  return false;
}

void clearUserMemory() {
  for (uint8_t i = 0; i < EEPROM_FILEBANK_NUM; i++) {
    EEPROM.write(EEPROM_FILEBANK_START + i * 48, 0);
  }
}

bool userMemoryChecksumMatches(uint8_t index) {
  uint8_t bytes[48];
  for (int16_t i = 0; i < 48; i++) {
    bytes[i] = EEPROM.read(EEPROM_FILEBANK_START + index * 48 + i);
  }
  uint16_t checksum = (bytes[46] << 8) | bytes[47];
  bool match = fletcher16(bytes, 46) == checksum;

  if (!match) {
    Serial << "Checksum mismatch for preset #" << index << "\n";
  }

  return match;
}

// The supplied buffer must be 45 bytes long
bool recallUserMemory(uint8_t index, uint8_t type, uint8_t *buffer) {
  if (index < EEPROM_FILEBANK_NUM) {
    if (userMemoryExists(index, type) && userMemoryChecksumMatches(index)) {
      // Recall logic:

      // Don't include type byte or checksum, already checked
      for (uint8_t i = 1; i < 48 - 2; i++) {
        buffer[i - 1] = EEPROM.read(EEPROM_FILEBANK_START + index * 48 + i);
      }

      Serial << "Recalled preset #" << index << "\n";
      return true;
    }
  }

  return false;
}

void storeUserMemory(uint8_t index, uint8_t type, uint8_t *buffer) {
  uint8_t preset[48];
  memset(preset, 0, 48);

  preset[0] = type;

  memcpy(preset + 1, buffer, 45);

  uint16_t checksum = fletcher16(preset, 46);
  preset[46] = checksum >> 8;
  preset[47] = checksum & 0xFF;

  for (int16_t i = 0; i < 48; i++) {
#ifdef ARDUINO_SKAARDUINO_DUE
    EEPROM.writeBuffered(EEPROM_FILEBANK_START + index * 48 + i, preset[i]);
#else
    EEPROM.write(EEPROM_FILEBANK_START + index * 48 + i, preset[i]);
#endif
  }
#ifdef ARDUINO_SKAARDUINO_DUE
  EEPROM.commitPage();
#endif
}
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
 * Set up devices debug state
 */
void deviceDebugLevel(uint8_t debugLevel) {
  for (uint8_t a = 1; a < sizeof(deviceArray); a++) {
    if (deviceEn[a]) {
      switch (pgm_read_byte_near(deviceArray + a)) {
#if SK_DEVICES_ATEM
      case SK_DEV_ATEM:
        AtemSwitcher[deviceMap[a]].serialOutput(debugLevel);
        break;
#endif
#if SK_DEVICES_HYPERDECK
      case SK_DEV_HYPERDECK:
        HyperDeck[deviceMap[a]].serialOutput(debugLevel);
        break;
#endif
#if SK_DEVICES_VIDEOHUB
      case SK_DEV_VIDEOHUB:
        VideoHub[deviceMap[a]].serialOutput(debugLevel);
        break;
#endif
#if SK_DEVICES_SMARTSCOPE
      case SK_DEV_SMARTSCOPE:
        SmartView[deviceMap[a]].serialOutput(debugLevel);
        break;
#endif
#if SK_DEVICES_BMDCAMCTRL
      case SK_DEV_BMDCAMCTRL:
        BMDCamCtrl[deviceMap[a]].serialOutput(debugLevel);
        break;
#endif
#if SK_DEVICES_SONYRCP
      case SK_DEV_SONYRCP:
        SonyRCP[deviceMap[a]].serialOutput(debugLevel);
        break;
#endif
#if SK_DEVICES_VMIX
      case SK_DEV_VMIX:
        VMIX[deviceMap[a]].serialOutput(debugLevel);
        break;
#endif
#if SK_DEVICES_ROLANDVR50
      case SK_DEV_ROLANDVR50:
        ROLANDVR50[deviceMap[a]].serialOutput(debugLevel);
        break;
#endif
#if SK_DEVICES_PANAAWHEX
      case SK_DEV_PANAAWHEX:
        PANAAWHEX[deviceMap[a]].serialOutput(debugLevel);
        break;
#endif
#if SK_DEVICES_MATROXMONARCH
      case SK_DEV_MATROXMONARCH:
        MATROXMONARCH[deviceMap[a]].serialOutput(debugLevel);
        break;
#endif
#if SK_DEVICES_H264REC
      case SK_DEV_H264REC:
        H264REC[deviceMap[a]].serialOutput(debugLevel);
        break;
#endif
#if SK_DEVICES_SONYVISCAIP
      case SK_DEV_SONYVISCAIP:
        SONYVISCAIP[deviceMap[a]].serialOutput(debugLevel);
        break;
#endif
      }
    }
  }
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
#endif
        break;
      case SK_DEV_HYPERDECK:
#if SK_DEVICES_HYPERDECK
        Serial << F(": HYPERDECK") << HyperDeck_initIdx;
        deviceMap[a] = HyperDeck_initIdx++;
        HyperDeck[deviceMap[a]].begin(deviceIP[a]);

        // This is known to cause problems with 70+ clips,
        // But is necessary for next/previous clip features
        HyperDeck[deviceMap[a]].askForClips(true);
// HyperDeck[deviceMap[a]].askForClipNames(true);
#endif
        break;
      case SK_DEV_VIDEOHUB:
#if SK_DEVICES_VIDEOHUB
        Serial << F(": VIDEOHUB") << VideoHub_initIdx;
        deviceMap[a] = VideoHub_initIdx++;
        VideoHub[deviceMap[a]].begin(deviceIP[a]);
#endif
        break;
      case SK_DEV_SMARTSCOPE:
#if SK_DEVICES_SMARTSCOPE
        Serial << F(": SMARTSCOPE") << SmartView_initIdx;
        deviceMap[a] = SmartView_initIdx++;
        SmartView[deviceMap[a]].begin(deviceIP[a]);
#endif
        break;
      case SK_DEV_BMDCAMCTRL:
#if SK_DEVICES_BMDCAMCTRL
        Serial << F(": BMDCAMCTRL") << BMDCamCtrl_initIdx;
        deviceMap[a] = BMDCamCtrl_initIdx++;
        BMDCamCtrl[deviceMap[a]].begin(0x6E); // TODO doesn't make sense
#endif
        break;
      case SK_DEV_SONYRCP:
#if SK_DEVICES_SONYRCP
        Serial << F(": SONYRCP") << SonyRCP_initIdx;
        deviceMap[a] = SonyRCP_initIdx++;
        SonyRCP[deviceMap[a]].begin(deviceIP[a]);
#endif
        break;
      case SK_DEV_VMIX:
#if SK_DEVICES_VMIX
        Serial << F(": VMIX") << VMIX_initIdx;
        deviceMap[a] = VMIX_initIdx++;
        VMIX[deviceMap[a]].begin(deviceIP[a]);
#endif
        break;
      case SK_DEV_ROLANDVR50:
#if SK_DEVICES_ROLANDVR50
        Serial << F(": ROLANDVR50") << ROLANDVR50_initIdx;
        deviceMap[a] = ROLANDVR50_initIdx++;
        ROLANDVR50[deviceMap[a]].begin(deviceIP[a]);
#endif
        break;
      case SK_DEV_PANAAWHEX:
#if SK_DEVICES_PANAAWHEX
        Serial << F(": PANAAWHEX") << PANAAWHEX_initIdx;
        deviceMap[a] = PANAAWHEX_initIdx++;
        PANAAWHEX[deviceMap[a]].begin(deviceIP[a]);
#endif
        break;
      case SK_DEV_MATROXMONARCH:
#if SK_DEVICES_MATROXMONARCH
        Serial << F(": MATROXMONARCH") << MATROXMONARCH_initIdx;
        deviceMap[a] = MATROXMONARCH_initIdx++;
        MATROXMONARCH[deviceMap[a]].begin(deviceIP[a]);
#endif
        break;
      case SK_DEV_H264REC:
#if SK_DEVICES_H264REC
        Serial << F(": H264REC") << H264REC_initIdx;
        deviceMap[a] = H264REC_initIdx++;
// H264REC[deviceMap[a]].begin(deviceIP[a]);
#endif
        break;
      case SK_DEV_SONYVISCAIP:
#if SK_DEVICES_SONYVISCAIP
        Serial << F(": SONYVISCAIP") << SONYVISCAIP_initIdx;
        deviceMap[a] = SONYVISCAIP_initIdx++;
        SONYVISCAIP[deviceMap[a]].begin(deviceIP[a]);
#endif
        break;
      }
      Serial << F(", IP=") << deviceIP[a] << F("\n");
    }
  }

  deviceDebugLevel(debugMode);
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
      case SK_DEV_BMDCAMCTRL:
#if SK_DEVICES_BMDCAMCTRL
        // Todo: Check we have coms with shield...
        deviceReady[a] = BMDCamCtrl[deviceMap[a]].hasInitialized();
#endif
        break;
      case SK_DEV_SONYRCP:
#if SK_DEVICES_SONYRCP
        deviceReady[a] = SonyRCP[deviceMap[a]].hasInitialized();
#endif
        break;
      case SK_DEV_VMIX:
#if SK_DEVICES_VMIX
        VMIX[deviceMap[a]].runLoop();
        deviceReady[a] = VMIX[deviceMap[a]].hasInitialized();
#endif
        break;
      case SK_DEV_ROLANDVR50:
#if SK_DEVICES_ROLANDVR50
        ROLANDVR50[deviceMap[a]].runLoop();
        deviceReady[a] = ROLANDVR50[deviceMap[a]].hasInitialized();
#endif
        break;
      case SK_DEV_PANAAWHEX:
#if SK_DEVICES_PANAAWHEX
        PANAAWHEX[deviceMap[a]].runLoop();
//    deviceReady[a] = PANAAWHEX[deviceMap[a]].???
#endif
        break;
      case SK_DEV_MATROXMONARCH:
#if SK_DEVICES_MATROXMONARCH
        MATROXMONARCH[deviceMap[a]].runLoop();
        deviceReady[a] = MATROXMONARCH[deviceMap[a]].hasInitialized();
#endif
        break;
      case SK_DEV_H264REC:
#if SK_DEVICES_H264REC
        H264REC[deviceMap[a]].runLoop();
        deviceReady[a] = H264REC[deviceMap[a]].hasInitialized();
#endif
        break;
      case SK_DEV_SONYVISCAIP:
#if SK_DEVICES_SONYVISCAIP
        SONYVISCAIP[deviceMap[a]].runLoop();
        // This is probably not the right way, to just select camera 1
        deviceReady[a] = SONYVISCAIP[deviceMap[a]].hasInitialized(1);
#endif
        break;
      }
    }
  }
}

/************************************
 *
 * Modular
 *
 ************************************/

#if SK_MODCOUNT > 0

uint8_t modular_readByte(uint8_t addr) {
  uint8_t rdata = 0xFF;
  Wire.beginTransmission(0x56);
  Wire.write(0);    // MSB
  Wire.write(addr); // LSB
  Wire.endTransmission();
  Wire.requestFrom(0x56, 1);
  if (Wire.available()) {
    rdata = Wire.read();
  }
  return rdata;
}
uint8_t checkID(uint8_t id, uint8_t csc, uint8_t csc2) { return ((SK_MODULAR_CSCSEED ^ id) == csc && ((((SK_MODULAR_CSCSEED ^ id) << 1) ^ id) & 0xFF) == csc2) ? id : 255; }

void modular_clock() {
  digitalWrite(SK_MODULAR_CLKPIN, HIGH);
  digitalWrite(SK_MODULAR_CLKPIN, LOW);
}
void modular_ResetChain() {
  digitalWrite(SK_MODULAR_ENPIN, HIGH); // Disabled

  // Remove any active modules first
  for (uint8_t a = 0; a < (SK_MODCOUNT + 1); a++) {
    modular_clock();
  }

  // Push select state to chain (selects first module = main controller)
  digitalWrite(SK_MODULAR_ENPIN, LOW);
  modular_clock();
  digitalWrite(SK_MODULAR_ENPIN, HIGH);
}
uint8_t modular_enableModule(uint8_t modId) {
  for (uint8_t a = 0; a < SK_MODCOUNT; a++) {
    if (moduleIdOrder[a] == modId && MODdis[a] != 0) {
      MODdis[a] = 0; // Enable it.
      return a;
    }
  }
  return 255;
}
uint8_t modular_readID() { return checkID(modular_readByte(0), modular_readByte(1), modular_readByte(2)); }

#endif

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

#if SK_MODEL == SK_RCP || SK_MODEL == SK_MICROMONITOR || SK_MODEL == SK_REFERENCE
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
#elif SK_MODEL == SK_REFERENCE
  infoDisplay << "REFERENCE";
#endif
  infoDisplay.display(B1); // Write to all
  statusLED(QUICKBLANK);
#endif
#if (SK_HWEN_SLIDER)
  Serial << F("Init Slider\n");
  uint16_t *cal1 = getAnalogComponentCalibration(1);
  slider.uniDirectionalSlider_init(cal1[2], cal1[0], cal1[1], A0);
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
#if SK_MODEL == SK_REFERENCE
  menuDisplay.begin(1, 0, 3); // DOGM163
  menuEncoders.begin(1);
#else
  menuDisplay.begin(4, 0, 3); // DOGM163
  menuEncoders.begin(5);
#endif
  menuDisplay.cursor(false);
  menuDisplay.print("SKAARHOJ");

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
#elif(SK_MODEL == SK_REFERENCE)
  SSWmenu.begin(3);
  SSWmenuEnc.begin(3);
  SSWmenuChip.begin(3);
#else
  SSWmenu.begin(0);
  SSWmenuEnc.begin(0);
  SSWmenuChip.begin(0);
#endif
  // SSWmenuEnc.serialOutput(SK_SERIAL_OUTPUT);
  SSWmenuEnc.setStateCheckDelay(250);
  SSWmenuEnc.setReverseMode(true);

  SSWmenuChip.inputOutputMask((B11111111 << 8) | B11101111);      // Set up input and output pins [GPA7-GPA0,GPB7-GPB0]
  SSWmenuChip.inputPolarityMask((B00010000 << 8) | B00000000);    // Reverse polarity for inputs.
  SSWmenuChip.setGPInterruptEnable((B00000001 << 8) | B00000001); // Set up which pins triggers interrupt (GPINTEN)

#if (SK_MODEL == SK_MICROSMARTE || SK_MODEL == SK_E201M16)
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
#elif(SK_MODEL == SK_REFERENCE)
  SSWbuttons.begin(4);
#else
  SSWbuttons.begin(1); // SETUP: Board address
#endif
#if (SK_MODEL == SK_MICROSMARTV)
  SSWbuttons.setRotation(2);
#else
  SSWbuttons.setRotation(0);
#endif
  SSWbuttons.setButtonBrightness(7, B1111);
  SSWbuttons.setButtonBrightness(7, B1111);
  SSWbuttons.setButtonColor(0, 2, 3, B1111);
  for (uint8_t i = 0; i < 64; i++) {
#if (SK_MODEL == SK_MICROSMARTV || SK_MODEL == SK_C15)
    SSWbuttons.clearDisplay();
    SSWbuttons.drawBitmap(64 - i - 1, 0, welcomeGraphics[0], 64, 32, 1, true);
    SSWbuttons.display(B10); // Write
    SSWbuttons.clearDisplay();
    SSWbuttons.drawBitmap(-(64 - i - 1), 0, welcomeGraphics[1], 64, 32, 1, true);
    SSWbuttons.display(B01); // Write
#elif SK_MODEL == SK_REFERENCE
    SSWbuttons.clearDisplay();
    SSWbuttons.drawBitmap(64 - i - 1, 0, welcomeGraphics[0], 64, 32, 1, true);
    SSWbuttons.display(B01); // Write
    SSWbuttons.clearDisplay();
    SSWbuttons.drawBitmap(-(64 - i - 1), 0, welcomeGraphics[1], 64, 32, 1, true);
    SSWbuttons.display(B10); // Write
    SSWbuttons.clearDisplay();
    SSWbuttons.drawBitmap(64 - i - 1, 0, welcomeGraphics[2], 64, 32, 1, true);
    SSWbuttons.display(B100); // Write
    SSWbuttons.clearDisplay();
    SSWbuttons.drawBitmap(-(64 - i - 1), 0, welcomeGraphics[3], 64, 32, 1, true);
    SSWbuttons.display(B1000); // Write
#else
    SSWbuttons.clearDisplay();
    SSWbuttons.drawBitmap(64 - i - 1, 0, welcomeGraphics[0], 64, 32, 1, true);
    SSWbuttons.display(B01); // Write
    SSWbuttons.clearDisplay();
    SSWbuttons.drawBitmap(-(64 - i - 1), 0, welcomeGraphics[1], 64, 32, 1, true);
    SSWbuttons.display(B10); // Write
#endif
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

  uint16_t *cal = getAnalogComponentCalibration(2);
  AudioMasterPot.uniDirectionalSlider_init(cal[2], cal[0], cal[1], 0, 0);
  AudioMasterPot.uniDirectionalSlider_disableUnidirectionality(true);

  if (getConfigMode()) {
    Serial << F("Test: AudioMasterControl\n");
    for (int16_t j = 0; j < 5; j++) {
      AudioMasterControl.setButtonLight(j + 1, true);
      delay(100);
    }
    for (int16_t j = 0; j < 5; j++) {
      AudioMasterControl.setButtonLight(j + 1, false);
      delay(100);
    }
    for (int16_t i = 1; i <= 2; i++) {
      for (int16_t j = 0; j < 4; j++) {
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
#if (SK_MODEL == SK_C90SM || SK_MODEL == SK_REFERENCE || SK_MODEL == SK_C90D || SK_MODEL == SK_C90MII)
  GPIOboard.begin(7);
#else
  GPIOboard.begin(0);
#endif

  statusLED(QUICKBLANK);
#endif

// Modular controllers:
#if SK_MODCOUNT > 0
  modular_ResetChain();
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
    int16_t SSWmenuEncValue = SSWmenuEnc.state(0, 1000);
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
      int16_t encValue = menuEncoders.state(a, 1000);
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
    GPIOboard.setOutputAll(GPIOboard.inputIsActiveAll());
#endif

#if (SK_HWEN_ACM)
    if (AudioMasterPot.uniDirectionalSlider_hasMoved()) {
      Serial << AudioMasterPot.uniDirectionalSlider_position() << F("\n");

      AudioMasterControl.VUmeter(AudioMasterPot.uniDirectionalSlider_position() * 17, (1000 - AudioMasterPot.uniDirectionalSlider_position()) * 17);
    }

    AudioMasterControl.setChannelIndicatorLight(1, (millis() >> 10) & B11);

    // Buttons:
    for (int16_t a = 0; a < 5; a++) {
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
  int16_t bDown;
  if (getNumOfActions(HWc) > 0) {
    static uint8_t SSWMenuItemPtr = 0;
    SSWmenuEnc.runLoop();
    if (SSWmenu.buttonDown(5)) {
      SSWMenuItemPtr = (SSWMenuItemPtr + 1) % getNumOfActions(HWc);
    }

    if (SSWMenuItemPtr + 1 > getNumOfActions(HWc)) {
      SSWMenuItemPtr = 0;
    }

    static bool voidVar = SSWmenuEnc.reset(0);

    int16_t clicks = 0;
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

    actionDispatch(HWc, actDown, actDown, (clicks << 1) | _systemHWcActionFineFlag[HWc - 1], BINARY_EVENT, SSWMenuItemPtr + 1);
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
  int16_t bDown;
  if (getNumOfActions(HWc) > 0) {
    static uint8_t menuItemPtr = 0;
    switch (menuEncoders.state(1, 1000)) {
    case 1:
    case -1:
      menuItemPtr = (menuItemPtr + getNumOfActions(HWc) + menuEncoders.lastCount(1, 2)) % getNumOfActions(HWc);
      Serial << menuItemPtr;
      break;
    }

    int16_t clicks = 0;
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
    actionDispatch(HWc, actDown, actDown, (clicks << 1) | _systemHWcActionFineFlag[HWc - 1], BINARY_EVENT, menuItemPtr + 1);

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

  bool disp_written[2] = {false, false};
  bool second_run = false;

  for (uint8_t a = 0; a < 3; a++) {
    extRetValIsWanted(true);
    retVal = actionDispatch(a + HWc);

    if (a == 0) {
      display_written = false;
    }

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
          disp_written[a - 1] = true;
        }
      }

      if (debugMode)
        Serial << F("Write info disp! ") << a << F(" HWc ") << HWc << F("\n");
    }

    // Make sure that all segments are written
    if (!second_run && (disp_written[0] || disp_written[1])) {
      for (uint8_t i = 0; i < 2; i++) {
        if (!disp_written[i]) {
          display_prevHash[i + 1]++;
          a = 0; // Recheck display segments
          second_run = true;
        }
      }
    }
  }

  if (display_update) {
    display.display(B1);
  }
}
#endif

#if SK_HWEN_SLIDER
void HWrunLoop_slider(const uint8_t HWc) {
  // Slider:
  bool hasMoved = false;
  static unsigned long timer = 0;
  if (sTools.hasTimedOut(timer, 40)) {
    hasMoved = slider.uniDirectionalSlider_hasMoved();
    if (hasMoved) {
      timer = millis();
      alarmLED();
    }
  }
  actionDispatch(HWc, hasMoved, hasMoved && slider.uniDirectionalSlider_isAtEnd(), 0, slider.uniDirectionalSlider_position());
}
#endif

#if SK_HWEN_GPIO || SK_MODEL == SK_E21CMB6M
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
void HWrunLoop_SSWbuttons(const uint8_t *HWcMap, uint8_t theSize) {
  if(theSize > 4) return;
  
  uint16_t bUp, bDown;
  bUp = SSWbuttons.buttonUpAll();
  bDown = SSWbuttons.buttonDownAll();
  static uint16_t prevHash[4];
  static uint8_t prevColor[4];
  for (uint8_t a = 0; a < theSize; a++) {
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
    switch (retVal) {
    case 0:
      break;
    case 1:
    case 4:
    case 5:
      retVal = 3;
      break;
    case 2:
      retVal = 1;
      break;
    case 3:
      retVal = 2;
      break;
    }
    control.setChannelIndicatorLight(1, retVal);
  }

  if (HWcMap[2]) { // VU
    uint16_t retVal = actionDispatch(HWcMap[2]);
    control.VUmeterDB((int)highByte(retVal) - 60, (int)lowByte(retVal) - 60);
  }

  // Buttons:
  uint16_t bUp = control.buttonUpAll();
  uint16_t bDown = control.buttonDownAll();
  for (int16_t a = 0; a < 5; a++) {
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

  for (int16_t a = 0; a < 2; a++) {
    if (HWcMap[2 + a]) { // Channel Indicator light
      uint16_t retVal = actionDispatch(HWcMap[2 + a]);
      switch (retVal) {
      case 0:
        break;
      case 1:
      case 4:
      case 5:
        retVal = 3;
        break;
      case 2:
        retVal = 1;
        break;
      case 3:
        retVal = 2;
        break;
      }
      control.setChannelIndicatorLight(1 + a, retVal);
    }
  }

  // Buttons:
  uint16_t bUp = control.buttonUpAll();
  uint16_t bDown = control.buttonDownAll();
  for (int16_t a = 0; a < 4; a++) {
    if (HWcMap[4 + a]) {
      uint16_t color = actionDispatch(HWcMap[4 + a], bDown & (B1 << a), bUp & (B1 << a));
      control.setButtonLight(a + 1, (color & 0xF) > 0 ? ((!(color & 0x10) || (millis() & 512) > 0) && ((color & 0xF) != 5) ? 1 : 0) : 0);
    }
  }
}
#endif

void HWrunLoop_BI8(SkaarhojBI8 &buttons, const uint8_t *HWcMap, uint8_t theSize) {
  uint16_t bUp, bDown;

  // Get all button up/down states at once, prevents inconsistencies
  uint32_t allButtons = buttons.buttonAll();
  bUp = allButtons >> 16;
  bDown = allButtons & 0xFFFF;

  for (uint8_t a = 0; a < theSize; a++) {
    extRetValPrefersLabel(HWcMap[a]);
    uint8_t color = actionDispatch(HWcMap[a], bDown & (B1 << a), bUp & (B1 << a));
    buttons.setButtonColor(a + 1, color & 0xF);
  }
}

void HWrunLoop_encoders(SkaarhojEncoders &encoders, const uint8_t *encMap, uint8_t theSize, bool reverseFirst = false) {
  int16_t bDown;
  for (uint8_t a = 0; a < theSize; a++) {
    if (encMap[a]) {
      int16_t clicks = 0;
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

int32_t pulsesHelper(int32_t inValue, const int32_t lower, const int32_t higher, const bool cycle, const int16_t pulses, const int16_t scaleFine, const int16_t scaleNormal) {
  int16_t scale = !(pulses & B1) ? scaleFine : scaleNormal; // Inverted when we use fine and coarse...
  inValue += (pulses >> 1) * scale;
  if (cycle) {
    if (inValue < lower) {
      inValue = higher - (lower - inValue - 1);
    } else if (inValue > higher) {
      inValue = lower + (inValue - higher - 1);
    }
  }

  // Serial << "In: " << inValue << " Lower: " << lower << " Upper: " << higher << " Result: " << constrain(inValue, lower, higher) << "\n";
  return constrain(inValue, lower, higher);
}
void storeMemory(uint8_t memPtr) {
  EEPROM.write(16 + memPtr, _systemMem[memPtr]);
  EEPROM.write(20, 193 ^ EEPROM.read(16) ^ EEPROM.read(17) ^ EEPROM.read(18) ^ EEPROM.read(19));
  if (debugMode)
    Serial << F("Memory ") << char(65 + memPtr) << F(" saved\n");
}
uint8_t cycleMemHelper(uint8_t actionPtr, uint8_t idx = 255) {
  int16_t values;
  values += (globalConfigMem[actionPtr + 3] != 0 || abs((int16_t)globalConfigMem[actionPtr + 2] - (int16_t)globalConfigMem[actionPtr + 3])) ? abs((int16_t)globalConfigMem[actionPtr + 2] - (int16_t)globalConfigMem[actionPtr + 3]) + 1 : 0;
  if (idx < values) {
    return (globalConfigMem[actionPtr + 2] < globalConfigMem[actionPtr + 3] ? globalConfigMem[actionPtr + 3] - (values - idx - 1) : globalConfigMem[actionPtr + 3] + (values - idx - 1));
  }
  values += (globalConfigMem[actionPtr + 5] != 0 || abs((int16_t)globalConfigMem[actionPtr + 4] - (int16_t)globalConfigMem[actionPtr + 5])) ? abs((int16_t)globalConfigMem[actionPtr + 4] - (int16_t)globalConfigMem[actionPtr + 5]) + 1 : 0;
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
uint16_t evaluateAction_system(const uint16_t actionPtr, const uint8_t HWc, const uint8_t actIdx, bool actDown, bool actUp, int16_t pulses, int16_t value) {
  if (debugMode && (actDown || actUp)) {
    Serial << F("System action ") << globalConfigMem[actionPtr] << F("\n");
  }
  uint8_t temp;
  uint16_t retVal = 0;
  uint8_t tempShift;

  switch (globalConfigMem[actionPtr]) {
  case 0:                                                                                                                                                              // Set Shift
    tempShift = (globalConfigMem[actionPtr + 3] > 0 && globalConfigMem[actionPtr + 3] <= 4) ? _systemShiftRegister[globalConfigMem[actionPtr + 3] - 1] : _systemShift; // Get shift register value

    if (globalConfigMem[actionPtr + 2] == 5) { // previous return value
      if ((_retValue & 0x20 ? 1 : 0) != _systemHWcActionCacheFlag[HWc][actIdx]) {
        _systemHWcActionCacheFlag[HWc][actIdx] = (_retValue & 0x20 ? 1 : 0);
        tempShift = constrain(_systemHWcActionCacheFlag[HWc][actIdx] ? globalConfigMem[actionPtr + 1] : 0, (uint8_t)0, (uint8_t)(SK_MAXACTIONS - 1));
        // Serial << "SERIAL RETURN VALUE: " << _systemHWcActionCacheFlag[HWc][actIdx] << "=>" << tempShift << "\n";
      }
    } else {
      if (actDown) {
        if (value != BINARY_EVENT) { // Value input
          tempShift = constrain(constrain(map(value, 0, 1000, 0, constrain(globalConfigMem[actionPtr + 1], (uint8_t)0, (uint8_t)(SK_MAXACTIONS - 1)) + 1), 0, constrain(globalConfigMem[actionPtr + 1], (uint8_t)0, (uint8_t)(SK_MAXACTIONS - 1))), (uint16_t)0, (uint16_t)(SK_MAXACTIONS - 1));
        } else {
          if (globalConfigMem[actionPtr + 2] == 3 || globalConfigMem[actionPtr + 2] == 4) { // Cycle up/down
            _systemHWcActionCacheFlag[HWc][actIdx] = true;                                  // Used to show button is highlighted here
            pulses = (globalConfigMem[actionPtr + 2] == 3 ? 1 : -1) << 1;
          } else if (globalConfigMem[actionPtr + 2] != 2 || !_systemHWcActionCacheFlag[HWc][actIdx]) {
            _systemHWcActionCacheFlag[HWc][actIdx] = true; // Used for toggle feature
            _systemHWcActionCache[HWc][actIdx] = tempShift;
            tempShift = constrain(globalConfigMem[actionPtr + 1], (uint8_t)0, (uint8_t)(SK_MAXACTIONS - 1));
          } else {
            tempShift = constrain(_systemHWcActionCache[HWc][actIdx], (uint16_t)0, (uint16_t)(SK_MAXACTIONS - 1));
            _systemHWcActionCacheFlag[HWc][actIdx] = false;
          }
        }
      }
      if (actUp && globalConfigMem[actionPtr + 2] == 1) { // "Hold Down"
        tempShift = constrain(_systemHWcActionCache[HWc][actIdx], (uint16_t)0, (uint16_t)(SK_MAXACTIONS - 1));
      }
      if (actUp && (globalConfigMem[actionPtr + 2] == 3 || globalConfigMem[actionPtr + 2] == 4)) { // "Cycle"
        _systemHWcActionCacheFlag[HWc][actIdx] = false;
      }
      if (pulses & 0xFFFE) {
        tempShift = pulsesHelper(tempShift, 0, constrain(globalConfigMem[actionPtr + 1], (uint8_t)0, (uint8_t)(SK_MAXACTIONS - 1)), true, pulses, 1, 1);
      }
      if (debugMode && (actDown || (pulses & 0xFFFE))) {
        Serial << F("SHIFT: ") << tempShift << F("\n");
      }
      retVal = (globalConfigMem[actionPtr + 2] == 3 || globalConfigMem[actionPtr + 2] == 4) ? (_systemHWcActionCacheFlag[HWc][actIdx] ? (4 | 0x20) : 5) : (tempShift == globalConfigMem[actionPtr + 1] ? (4 | 0x20) : 5);

      if (extRetValIsWanted()) {
        temp = (_systemHWcActionPrefersLabel[HWc] && !(globalConfigMem[actionPtr + 2] == 3 || globalConfigMem[actionPtr + 2] == 4)) ? globalConfigMem[actionPtr + 1] : tempShift;

        extRetVal(temp, temp == 0 || (temp == 1 && globalConfigMem[actionPtr + 1] < 2) ? 7 : 0); // , pulses&B1 - not using this because it has no significance for this type of action.
        extRetValShortLabel(PSTR("Shift"));                                                      // TODO: Put in reg A-D in label if applicable
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
    }

    if (globalConfigMem[actionPtr + 3] > 0 && globalConfigMem[actionPtr + 3] <= 4) {
      _systemShiftRegister[globalConfigMem[actionPtr + 3] - 1] = tempShift;
    } else {
      _systemShift = tempShift;
    }

    return retVal;
    break;
  case 1: // Set State
    if (actDown) {
      if (value != BINARY_EVENT) { // Value input
        _systemState = constrain(map(value, 0, 1000, 0, constrain(globalConfigMem[actionPtr + 1], (uint8_t)0, (uint8_t)(SK_MAXSTATES - 1)) + 1), 0, constrain(globalConfigMem[actionPtr + 1], (uint8_t)0, (uint8_t)(SK_MAXSTATES - 1)));
      } else {
        if (globalConfigMem[actionPtr + 2] == 3 || globalConfigMem[actionPtr + 2] == 4) { // Cycle up/down
          _systemHWcActionCacheFlag[HWc][actIdx] = true;                                  // Used to show button is highlighted here
          pulses = (globalConfigMem[actionPtr + 2] == 3 ? 1 : -1) << 1;
        } else if (globalConfigMem[actionPtr + 2] != 2 || !_systemHWcActionCacheFlag[HWc][actIdx]) {
          _systemHWcActionCacheFlag[HWc][actIdx] = true; // Used for toggle feature
          _systemPrevState = _systemState;
          _systemState = constrain(globalConfigMem[actionPtr + 1], (uint8_t)0, (uint8_t)(SK_MAXSTATES - 1));
        } else {
          _systemState = constrain(_systemPrevState, (uint16_t)0, (uint16_t)(SK_MAXSTATES - 1));
          _systemHWcActionCacheFlag[HWc][actIdx] = false;
        }
      }
    }
    if (actUp && globalConfigMem[actionPtr + 2] == 1) { // "Hold Down"
      _systemState = constrain(_systemPrevState, (uint16_t)0, (uint16_t)(SK_MAXSTATES - 1));
    }
    if (actUp && (globalConfigMem[actionPtr + 2] == 3 || globalConfigMem[actionPtr + 2] == 4)) { // "Cycle"
      _systemHWcActionCacheFlag[HWc][actIdx] = false;
    }
    if (pulses & 0xFFFE) {
      _systemState = pulsesHelper(_systemState, 0, constrain(globalConfigMem[actionPtr + 1], (uint8_t)0, (uint8_t)(SK_MAXSTATES - 1)), true, pulses, 1, 1);
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
        if (value != BINARY_EVENT) { // Value input
          _systemMem[globalConfigMem[actionPtr + 1]] = constrain(map(value, 0, 1000, 0, globalConfigMem[actionPtr + 2] + 1), 0, globalConfigMem[actionPtr + 2]);
          // Serial << "SYSTEM MEM: " << value << "," << map(value, 0, 1000, 0, globalConfigMem[actionPtr + 2] + 1) << "," << constrain(map(value, 0, 1000, 0, globalConfigMem[actionPtr + 2] + 1), 0, globalConfigMem[actionPtr + 2]) << "\n";
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
        if (value == BINARY_EVENT) { // Binary
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
          setSystemBit(globalConfigMem[actionPtr + 1], (!globalConfigMem[actionPtr + 3]) ^ ((value == BINARY_EVENT) ? false : (value < 500)));
          //   Serial << "BIT A: " << ((!globalConfigMem[actionPtr + 3]) ^ ((value == BINARY_EVENT) ? false : (value < 500))) << "\n";
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
      lDelay(constrain(globalConfigMem[actionPtr + 1], (uint8_t)0, (uint8_t)250) * 100);
    break;
  case 9: // Custom function
    switch (globalConfigMem[actionPtr + 1]) {
    case 1:
      return customActionHandlerA(actionPtr, HWc, actIdx, actDown, actUp, pulses, value);
      break;
    case 2:
      return customActionHandlerB(actionPtr, HWc, actIdx, actDown, actUp, pulses, value);
      break;
    case 3:
      return customActionHandlerC(actionPtr, HWc, actIdx, actDown, actUp, pulses, value);
      break;
    case 4:
      return customActionHandlerD(actionPtr, HWc, actIdx, actDown, actUp, pulses, value);
      break;
    default:
      return customActionHandlerNative(actionPtr, HWc, actIdx, actDown, actUp, pulses, value);
      break;
    }
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
  case 12: // Panel brightness
    break;
  case 13: // Range Limiter
    if (globalConfigMem[actionPtr + 1] < 4) {
      if (actDown) {
        _systemRangeLower[globalConfigMem[actionPtr + 1]] = 0;
        _systemRangeUpper[globalConfigMem[actionPtr + 1]] = 255;
      }
      if (pulses & 0xFFFE) { // Encoder:
        if (!(pulses & B1)) {
          _systemRangeUpper[globalConfigMem[actionPtr + 1]] = pulsesHelper(_systemRangeUpper[globalConfigMem[actionPtr + 1]], 0, 255, false, pulses, 2, 2);
          if (_systemRangeUpper[globalConfigMem[actionPtr + 1]] < _systemRangeLower[globalConfigMem[actionPtr + 1]]) {
            _systemRangeUpper[globalConfigMem[actionPtr + 1]] = _systemRangeLower[globalConfigMem[actionPtr + 1]];
          }
        } else {
          _systemRangeLower[globalConfigMem[actionPtr + 1]] = pulsesHelper(_systemRangeLower[globalConfigMem[actionPtr + 1]], 0, 255, false, pulses, 2, 2);
          if (_systemRangeLower[globalConfigMem[actionPtr + 1]] > _systemRangeUpper[globalConfigMem[actionPtr + 1]]) {
            _systemRangeLower[globalConfigMem[actionPtr + 1]] = _systemRangeUpper[globalConfigMem[actionPtr + 1]];
          }
        }
      }
    }
    break;
  case 14: // Value Scaler
    if (globalConfigMem[actionPtr + 1] < 4) {
      if (actDown) {

        if (globalConfigMem[actionPtr + 3] != 2 || !_systemHWcActionCacheFlag[HWc][actIdx]) {
          _systemHWcActionCacheFlag[HWc][actIdx] = true; // Used for toggle feature
          _systemHWcActionCache[HWc][actIdx] = _systemScaler[globalConfigMem[actionPtr + 1]];
          _systemScaler[globalConfigMem[actionPtr + 1]] = globalConfigMem[actionPtr + 2];
        } else {
          _systemScaler[globalConfigMem[actionPtr + 1]] = _systemHWcActionCache[HWc][actIdx];
          _systemHWcActionCacheFlag[HWc][actIdx] = false;
        }
      }
      if (actUp && globalConfigMem[actionPtr + 3] == 1) { // "Hold Down"
        _systemScaler[globalConfigMem[actionPtr + 1]] = _systemHWcActionCache[HWc][actIdx];
      }
      if (pulses & 0xFFFE) {
        _systemScaler[globalConfigMem[actionPtr + 1]] = pulsesHelper(_systemScaler[globalConfigMem[actionPtr + 1]], 0, 3, true, pulses, 1, 1);
      }

      retVal = _systemScaler[globalConfigMem[actionPtr + 1]] == globalConfigMem[actionPtr + 2] ? (4 | 0x20) : 5;

      if (extRetValIsWanted()) {
        temp = _systemHWcActionPrefersLabel[HWc] ? globalConfigMem[actionPtr + 2] : _systemMem[globalConfigMem[actionPtr + 1]];

        extRetVal(temp, 0, pulses & B1);
        extRetValShortLabel(PSTR("Scaler "), globalConfigMem[actionPtr + 1]);
        extRetValLongLabel(PSTR("Scaler "), globalConfigMem[actionPtr + 1]);
        _extRetShort[7] = _extRetLong[7] = char(globalConfigMem[actionPtr + 1] + 65);

        if (_systemHWcActionPrefersLabel[HWc]) {
          extRetValColor(retVal & 0x20 ? B010111 : B101010);
          extRetValSetLabel(true);
        } else {
          extRetValColor(B010111);
        }
      }
    }
    return retVal;
    break;
  case 15: // Local Shift Level Register
           // No implementation here, see inside actionDispatch where it is hardcoded.
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
uint16_t actionDispatch(uint8_t HWcNum, bool actDown, bool actUp, int16_t pulses, int16_t value, const uint8_t specificAction) {
  uint8_t actionMirrorC = 0;
  actionMirror = 0;
  _inactivePanel_actDown = actDown;
  if (_inactivePanel) {
    actDown = false;
    actUp = false;
    pulses = pulses & B1;
    value = BINARY_EVENT;
  }
  if (specificAction > getNumOfActions(HWcNum)) {
    Serial << "Unreachable specificAction=" << specificAction << " from HWc " << HWcNum << ". Breaking...\n";
    return 0;
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
    _retValue = 0;                                                            // Returns zero by default (inactive element)
    uint16_t retValueT = 0;                                                   // Preliminary return value
    if (stateBehaviourPtr > 0) {                                              // If there is some configuration to relate to....
      uint8_t stateLen = globalConfigMem[stateBehaviourPtr - 1];
      bool shiftLevelMatch = false;
      uint8_t matchShiftValue = _systemShift;
      bool firstPass = true;
      // Traverse actions in state behaviour
      while (!shiftLevelMatch) {
        uint8_t actIdx = 0;
        uint8_t sShift = 0;
        uint8_t lptr = 0;
        while (lptr < stateLen) {
          if (actIdx >= SK_MAXACTIONS)
            break; // actIdx at or over SK_MAXACTIONS would result in memory leaks in various evaluation functions which would trust HWc and actIdx to not exceed the bounds of the _systemHWcActionCache array
          if (lptr > 0 && (globalConfigMem[stateBehaviourPtr + lptr] & 16) > 0)
            sShift++; // If a shift divider is found (cannot be the first element)

          uint8_t devIdx = globalConfigMem[stateBehaviourPtr + lptr] & 15;
          if ((specificAction == 0 && matchShiftValue == sShift) || (specificAction == actIdx + 1)) {
            shiftLevelMatch = true;
            // Traverse actions in shift level:
            if (devIdx > 0 && devIdx < sizeof(deviceArray) && deviceEn[devIdx]) {
              if (deviceEn[devIdx] && deviceReady[devIdx]) {
                switch (pgm_read_byte_near(deviceArray + devIdx)) {
                case SK_DEV_ATEM:
#if SK_DEVICES_ATEM
                  retValueT = evaluateAction_ATEM(deviceMap[devIdx], stateBehaviourPtr + lptr + 1, HWcNum - 1, actIdx, actDown, actUp, pulses, value);
                  if (_retValue == 0)
                    _retValue = retValueT; // Use first ever return value in case of multiple actions.
#endif
                  break;
                case SK_DEV_HYPERDECK:
#if SK_DEVICES_HYPERDECK
                  retValueT = evaluateAction_HYPERDECK(deviceMap[devIdx], stateBehaviourPtr + lptr + 1, HWcNum - 1, actIdx, actDown, actUp, pulses, value);
                  if (_retValue == 0)
                    _retValue = retValueT; // Use first ever return value in case of multiple actions.
#endif
                  break;
                case SK_DEV_VIDEOHUB:
#if SK_DEVICES_VIDEOHUB
                  retValueT = evaluateAction_VIDEOHUB(deviceMap[devIdx], stateBehaviourPtr + lptr + 1, HWcNum - 1, actIdx, actDown, actUp, pulses, value);
                  if (_retValue == 0)
                    _retValue = retValueT; // Use first ever return value in case of multiple actions.
#endif
                  break;
                case SK_DEV_SMARTSCOPE:
#if SK_DEVICES_SMARTSCOPE
                  retValueT = evaluateAction_SMARTSCOPE(deviceMap[devIdx], stateBehaviourPtr + lptr + 1, HWcNum - 1, actIdx, actDown, actUp, pulses, value);
                  if (_retValue == 0)
                    _retValue = retValueT; // Use first ever return value in case of multiple actions.
#endif
                  break;
                case SK_DEV_BMDCAMCTRL:
#if SK_DEVICES_BMDCAMCTRL
                  retValueT = evaluateAction_BMDCAMCTRL(deviceMap[devIdx], stateBehaviourPtr + lptr + 1, HWcNum - 1, actIdx, actDown, actUp, pulses, value);
                  if (_retValue == 0)
                    _retValue = retValueT; // Use first ever return value in case of multiple actions.
#endif
                  break;
                case SK_DEV_SONYRCP:
#if SK_DEVICES_SONYRCP
                  retValueT = evaluateAction_SONYRCP(deviceMap[devIdx], stateBehaviourPtr + lptr + 1, HWcNum - 1, actIdx, actDown, actUp, pulses, value);
                  if (_retValue == 0)
                    _retValue = retValueT; // Use first ever return value in case of multiple actions.
#endif
                  break;
                case SK_DEV_VMIX:
#if SK_DEVICES_VMIX
                  retValueT = evaluateAction_VMIX(deviceMap[devIdx], stateBehaviourPtr + lptr + 1, HWcNum - 1, actIdx, actDown, actUp, pulses, value);
                  if (_retValue == 0)
                    _retValue = retValueT; // Use first ever return value in case of multiple actions.
#endif
                  break;
                case SK_DEV_ROLANDVR50:
#if SK_DEVICES_ROLANDVR50
                  retValueT = evaluateAction_ROLANDVR50(deviceMap[devIdx], stateBehaviourPtr + lptr + 1, HWcNum - 1, actIdx, actDown, actUp, pulses, value);
                  if (_retValue == 0)
                    _retValue = retValueT; // Use first ever return value in case of multiple actions.
#endif
                  break;
                case SK_DEV_PANAAWHEX:
#if SK_DEVICES_PANAAWHEX
                  retValueT = evaluateAction_PANAAWHEX(deviceMap[devIdx], stateBehaviourPtr + lptr + 1, HWcNum - 1, actIdx, actDown, actUp, pulses, value);
                  if (_retValue == 0)
                    _retValue = retValueT; // Use first ever return value in case of multiple actions.
#endif
                  break;
                case SK_DEV_MATROXMONARCH:
#if SK_DEVICES_MATROXMONARCH
                  retValueT = evaluateAction_MATROXMONARCH(deviceMap[devIdx], stateBehaviourPtr + lptr + 1, HWcNum - 1, actIdx, actDown, actUp, pulses, value);
                  if (_retValue == 0)
                    _retValue = retValueT; // Use first ever return value in case of multiple actions.
#endif
                  break;
                case SK_DEV_H264REC:
#if SK_DEVICES_H264REC
                  retValueT = evaluateAction_H264REC(deviceMap[devIdx], stateBehaviourPtr + lptr + 1, HWcNum - 1, actIdx, actDown, actUp, pulses, value);
                  if (_retValue == 0)
                    _retValue = retValueT; // Use first ever return value in case of multiple actions.
#endif
                  break;
                case SK_DEV_SONYVISCAIP:
#if SK_DEVICES_SONYVISCAIP
                  retValueT = evaluateAction_SONYVISCAIP(deviceMap[devIdx], stateBehaviourPtr + lptr + 1, HWcNum - 1, actIdx, actDown, actUp, pulses, value);
                  if (_retValue == 0)
                    _retValue = retValueT; // Use first ever return value in case of multiple actions.
#endif
                  break;
                }
              } else {
                // Serial << "Device disabled!\n";
              }
            } else if (devIdx == 15) {
              retValueT = evaluateAction_system(stateBehaviourPtr + lptr + 1, HWcNum - 1, actIdx, actDown, actUp, pulses, value);
              if (_retValue == 0)
                _retValue = retValueT; // Use first ever return value in case of multiple actions.
            }
          } else if ((specificAction == 0 && matchShiftValue < sShift) || (specificAction > 0 && specificAction < actIdx + 1)) {

            break;
          }
          if (firstPass && sShift == 0 && devIdx == 15 && globalConfigMem[stateBehaviourPtr + lptr + 1] == 15) { // Local Shift Level Register setting (must be in shift=0 part of code, should also be first item, otherwise preceding items will be called.)
            if (globalConfigMem[stateBehaviourPtr + lptr + 2] < 4) {
              //    Serial << "Local SLR: " << globalConfigMem[stateBehaviourPtr + lptr + 2] << "=" << _systemShiftRegister[globalConfigMem[stateBehaviourPtr + lptr + 2]] << "\n";
              matchShiftValue = _systemShiftRegister[globalConfigMem[stateBehaviourPtr + lptr + 2]];
            }
          }

          lptr += (globalConfigMem[stateBehaviourPtr + lptr] >> 5) + 2; // Add length
          actIdx++;
        }
        firstPass = false;
        if (!shiftLevelMatch) {
          matchShiftValue = 0;
        }
      }
    }

    deviceRunLoop();

    if (actionMirror == 0) { // If no mirror was specified, just return value
      return _retValue;
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

// Delay to allow initial output to be displayed in serial console
#ifdef ARDUINO_SKAARDUINO_DUE
  delay(1700); // Enough time to let serial port appear and open it to receive the first message:
#endif

  delay(200); // This also prevents alarm-LED delay of 200 ms from messing with the blinking intro.
  Serial << F("\n\n*****************************\nSKAARHOJ Controller Booting \n*****************************\n");

#if SK_MODCOUNT > 0
  pinMode(SK_MODULAR_CLKPIN, OUTPUT);    // select clk (pin 9 on flatcable)
  pinMode(SK_MODULAR_ENPIN, OUTPUT);     // enable (pin 10 on flat cable)
  digitalWrite(SK_MODULAR_CLKPIN, HIGH); // Disabled
  digitalWrite(SK_MODULAR_ENPIN, HIGH);  // Disabled
#endif

  Serial << F("SK_VERSION: ") << SK_VERSION << "\n";

// Setup Config:
#if SK_ETHMEGA
  pinMode(A1, INPUT_PULLUP); // CFG input on ethermega MMBOS
#elif defined(ARDUINO_SKAARDUINO_DUE)
  pinMode(23, INPUT);         // CFG input (active low)  - if you set it to INPUT_PULLUP, the resistor on the Bottom part will not be strong enough to pull it down!!
#else
  pinMode(18, INPUT); // CFG input (active low)  - if you set it to INPUT_PULLUP, the resistor on the Bottom part will not be strong enough to pull it down!!
#endif

// Setup LED pins:
#if SK_ETHMEGA
  pinMode(3, OUTPUT); // Red Status LED, active high
  pinMode(2, OUTPUT); // Green Status LED, active high
#else
  statusLED(LED_OFF);         // To prevent brief light in LEDs upon boot
  pinMode(PIN_RED, OUTPUT);   // Red Status LED, active low
  pinMode(PIN_GREEN, OUTPUT); // Green Status LED, active low
  pinMode(PIN_BLUE, OUTPUT);  // Blue Status LED, active low
#endif

  HWcfgDisplay();

  statusLED(LED_PURPLE); // Normal mode, so far...

  // I2C setup:
  Wire.begin();
  statusLED(QUICKBLANK);

  // Check Config Button press or EEPROM setting to start it:
  if (isConfigButtonPushed()) {
    configMode = EEPROM.read(0) == 2 ? 2 : 1;
    if (EEPROM.read(0) != 0) {
      EEPROM.write(0, 0);
    }
    statusLED(LED_BLUE); // In config mode 1, LED will be blue
    unsigned long cfgButtonPressTime = millis();
    Serial << F("Config Mode=1\n");

    while (configMode == 2 || isConfigButtonPushed()) {
      if (configMode == 2 || (millis() - cfgButtonPressTime > 2000)) {
        statusLED(LED_WHITE); // In config mode 2, LED will be white and blink
        Serial << F("Config Mode=2\n");
        configMode = 2;

        while (isConfigButtonPushed()) {
          if (millis() - cfgButtonPressTime > 10000) {
            statusLED(LED_RED);
            Serial << F("Clearing presets, revert to normal boot\n");
            clearPresets();
            delay(1000);
            statusLED(LED_PURPLE); // Normal mode
            configMode = 0;
            break;
          }
        }
        break;
      }
    }
  } else {
    statusLED(LED_PURPLE); // Normal mode
  }

  // Initializes the actual hardware components / modules on the controller
  uint8_t buttonPressUponBoot = HWsetup();
  if (!configMode && buttonPressUponBoot) {
    switch (buttonPressUponBoot) {
    case 2:
      configMode = 2;
      Serial << F("Config Mode=2 (via HWc)\n");
      break;
    default:
      configMode = 1;
      Serial << F("Config Mode=1 (via HWc)\n");
      break;
    }
  }
  statusLED(QUICKBLANK);

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

  IPAddress configSubnet(255, 255, 255, 0);
  // Setting Subnet
  if (configMode != 2) {
    configSubnet[0] = globalConfigMem[getConfigMemIPIndex() + 4];
    configSubnet[1] = globalConfigMem[getConfigMemIPIndex() + 5];
    configSubnet[2] = globalConfigMem[getConfigMemIPIndex() + 6];
    configSubnet[3] = globalConfigMem[getConfigMemIPIndex() + 7];
  }

  if ((uint32_t)configSubnet != 0) {
    subnet = configSubnet;
    Serial << F("Subnet mask: ");
  } else {
    Serial << F("Using default subnet: ");
  }

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

  IPAddress gateway(0, 0, 0, 0);
  IPAddress dnsIP(0, 0, 0, 0);
  Ethernet.begin(mac, ip, dnsIP, gateway, subnet);
  statusLED(QUICKBLANK);
  delay(500);

  // Check if debug mode is on, if so reset it for next reset:
  if (EEPROM.read(1) != 0) {
    Serial << F("Debug Mode=1\n");
    // debugMode = true;
    debugMode = 0x80;
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