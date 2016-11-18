/**
   Universal Sketch for generic SKAARHOJ devices

   By configuration you can compile software for various SKAARHOJ devices
   as well as target broadcast devices
*/

// Define model (according to list further down):
#define SK_MODEL SK_RCP







#define SK_E21CMB6M_OPTION_VSLIDER true     // AUTOGEN: true,false
#define SK_E21TVS_OPTION_GPIO false     // AUTOGEN: true,false
#define SK_E21GPIO_OPTION_GPIO2 false     // AUTOGEN: true,false
#define SK_RCP_OPTION_ENCODER true     // AUTOGEN: true,false

#define SK_C90D_OPTION_DISABLEMASK 0b00000000000000000000000000000000    // Must have 32 digits. Disables buttons #1-#32 from left. AUTOGEN: 0
#define SK_C90D_OPTION_VARIANT 4    // AUTOGEN: 0,1,2,3,4
#define SK_C90D_OPTION_GPIO true    // AUTOGEN: true,false

#define SK_C90MII_OPTION_GPIO true    // AUTOGEN: true,false








// ****************************
// NO USER CHANGE BELOW!
// ****************************
#define SK_SERIAL_OUTPUT 1
#ifndef SK_ETHMEGA
#define SK_ETHMEGA 0
#endif

// Definition of the various controller models this sketch supports:
#define SK_E21TVS 1
#define SK_E21KP01 2
#define SK_E21SLD 3
#define SK_E21SSW 4
#define SK_E21CMB6 5
#define SK_E21CMB6M 6
#define SK_E21GPIO 9
#define SK_E201L16 10
#define SK_E201L2 11
#define SK_E201M16 14
#define SK_E201S16 17
#define SK_MICROAUDIO 20
#define SK_MICROBI16 21
#define SK_MICROGPIO 22
#define SK_MICROKP01 23
#define SK_MICROLEVELS 24
#define SK_MICROMONITOR 25
#define SK_MICROSMARTE 26
#define SK_MICROSMARTH 27
#define SK_MICROSMARTV 28
#define SK_MICROTALLY 29
#define SK_C90 40
#define SK_C90A 41
#define SK_C90SM 42
#define SK_C31 50
#define SK_C10 51
#define SK_C15 52
#define SK_RCP 60
#define SK_WINGMAN 61
#define SK_WINGMANTOUCH 62
#define SK_CCUX 63
#define SK_TALLY 70
#define SK_C90MII 71
#define SK_C90D 72

#define SK_CUSTOMHW 1000 // Custom HARDWARE option - where the SK_CTRL_ file is also custom (located in sketch folder then). That file is normally a generic file.
#define SK_DUMMY 1001
#define SK_REFERENCE 1002

// Definition of the hardware devices we support:
#define SK_DEV_ATEM 1
#define SK_DEV_HYPERDECK 2
#define SK_DEV_VIDEOHUB 3
#define SK_DEV_SMARTSCOPE 4
#define SK_DEV_TERANEX 5
#define SK_DEV_BMDCAMCTRL 6
#define SK_DEV_VMIX 7
#define SK_DEV_ROLANDVR50 8
#define SK_DEV_MATROXMONARCH 9
#define SK_DEV_H264REC 10
#define SK_DEV_SONYDECK 11
#define SK_DEV_PANAAWHEX 12
#define SK_DEV_SONYVISCAIP 13

// DISABLED! Left here to avoid removing code from Skaarhojbase.h
#define SK_DEV_SONYRCP 255

// Defines to enable code for generic items:
#define SK_HWEN_STDOLEDDISPLAY 0
#define SK_HWEN_SLIDER 0
#define SK_HWEN_JOYSTICK 0
#define SK_HWEN_MENU 0
#define SK_HWEN_SSWMENU 0
#define SK_HWEN_SSWBUTTONS 0
#define SK_HWEN_ACM 0
#define SK_HWEN_GPIO 0

#define SK_MODCOUNT 0   // Number of modules in chain

// Customization file, located in sketch folder:
#include "customization.h"

/****************************************

   INCLUDING HARDWARE RELATED LIBRARIES, I
   Libraries with controller hardware configuration
   and physical hardware libraries

 ****************************************/
#include "SPI.h"
#include "Wire.h"
#include "Ethernet.h"
#include "utility/w5100.h"
#include "Streaming.h"
#ifdef __arm__ /* Arduino DUE */
#include "SkaarhojDueEEPROM.h"
SkaarhojDueEEPROM EEPROM;
#else
#include "EEPROM.h"
#endif
#include "SkaarhojPgmspace.h"
#include "MemoryFree.h"
#include "SkaarhojTools.h"
SkaarhojTools sTools(0);
#define WEBDUINO_FAVICON_DATA ""
#include "WebServer.h"

#include "MCP23017.h"
#include "PCA9685.h"
#include "SkaarhojBI8.h"
#include "SkaarhojBI16.h"
#include "SkaarhojEncoders.h"

// Includes per model:
#if (SK_MODEL == SK_E21TVS)
#include "SK_CFGDEF_E21TVS.h"
#elif(SK_MODEL == SK_E21KP01)
#include "SK_CFGDEF_E21KP01.h"
#elif(SK_MODEL == SK_E21SLD)
#include "SK_CFGDEF_E21SLD.h"
#elif(SK_MODEL == SK_E21SSW)
#include "SK_CFGDEF_E21SSW.h"
#elif(SK_MODEL == SK_E21CMB6)
#include "SK_CFGDEF_E21CMB6.h"
#elif(SK_MODEL == SK_E21CMB6M)
#include "SkaarhojGPIO2x8.h"
#include "Adafruit_GFX.h"
#include "SkaarhojDisplayArray.h"
#include "SK_CFGDEF_E21CMB6M.h"
#elif(SK_MODEL == SK_E21GPIO)
#include "SkaarhojGPIO2x8.h"
#include "SK_CFGDEF_E21GPIO.h"
#elif(SK_MODEL == SK_E201M16)
#include "SK_CFGDEF_E201M16.h"
#elif(SK_MODEL == SK_E201L2)
#include "SK_CFGDEF_E201L2.h"
#define MCU_ETM 1
#elif(SK_MODEL == SK_E201S16)
#include "SK_CFGDEF_E201S16.h"
#elif(SK_MODEL == SK_MICROAUDIO)
#include "SkaarhojAudioControl.h"
#include "SkaarhojAnalog.h"
#include "ADS7828.h"
#include "SK_CFGDEF_MICROAUDIO.h"
#elif(SK_MODEL == SK_MICROBI16)
#include "SK_CFGDEF_MICROBI16.h"
#elif(SK_MODEL == SK_MICROGPIO)
#include "SK_CFGDEF_MICROGPIO.h"
#elif(SK_MODEL == SK_MICROKP01)
#include "SK_CFGDEF_MICROKP01.h"
#elif(SK_MODEL == SK_MICROLEVELS)
#include "SkaarhojAudioControl.h"
#include "SK_CFGDEF_MICROLEVELS.h"
#elif(SK_MODEL == SK_MICROMONITOR)
#include "SK_CFGDEF_MICROMONITOR.h"
#elif(SK_MODEL == SK_MICROSMARTE)
#include "SK_CFGDEF_MICROSMARTE.h"
#elif(SK_MODEL == SK_MICROSMARTH)
#include "SK_CFGDEF_MICROSMARTH.h"
#elif(SK_MODEL == SK_MICROSMARTV)
#include "SK_CFGDEF_MICROSMARTV.h"
#elif(SK_MODEL == SK_MICROTALLY)
#include "SkaarhojGPIO2x8.h"
#include "SK_CFGDEF_MICROTALLY.h"
#elif(SK_MODEL == SK_C90)
#include "SK_CFGDEF_C90.h"
#elif(SK_MODEL == SK_C90A)
#include "SkaarhojAudioControl2.h"
#include "SkaarhojAnalog.h"
#include "ADS7828.h"
#include "SK_CFGDEF_C90A.h"
#elif(SK_MODEL == SK_C90SM)
#include "SK_CFGDEF_C90SM.h"
#elif(SK_MODEL == SK_C31)
#include "SK_CFGDEF_C31.h"
#elif(SK_MODEL == SK_C10)
#include "SK_CFGDEF_C10.h"
#elif(SK_MODEL == SK_C15)
#include "SK_CFGDEF_C15.h"
#elif(SK_MODEL == SK_RCP)
#include "SkaarhojUHB.h"
#include "SkaarhojEADOGMDisplay.h"
#include "SkaarhojAnalog.h"
#include "ADS7828.h"
#include "Adafruit_GFX.h"
#include "SkaarhojOLED64x256.h"
#include "SK_CFGDEF_RCP.h"
#elif(SK_MODEL == SK_WINGMAN)
#include "Adafruit_GFX.h"
#include "SkaarhojOLED64x256.h"
#include "SkaarhojDome.h"
#include "SK_CFGDEF_WINGMAN.h"
#elif(SK_MODEL == SK_WINGMANTOUCH)
#include "Adafruit_GFX.h"
#include "SkaarhojOLED64x256.h"
#include "SkaarhojDisplayArray.h"
#include "SkaarhojDome.h"
#include "Adafruit_STMPE610.h"
#include "SK_CFGDEF_WINGMANTOUCH.h"
#elif(SK_MODEL == SK_CCUX)
#include "SkaarhojDisplayArray.h"
#include "SkaarhojAnalog.h"
#include "ADS7828.h"
#include "Adafruit_GFX.h"
#include "SkaarhojOLEDRocker.h"
#include "SkaarhojSmartSwitch2.h"
#include "SK_CFGDEF_CCU.h" // TODO: Change to CCU-X when ready....
#elif(SK_MODEL == SK_TALLY)
#include "SkaarhojGPIO1x16.h"
#include "SK_CFGDEF_TALLY.h"
#elif(SK_MODEL == SK_CUSTOMHW)
#include "SK_CFGDEF_CUSTOMHW.h"
#elif(SK_MODEL == SK_DUMMY)
#include "SK_CFGDEF_DUMMY.h"
#elif(SK_MODEL == SK_REFERENCE)
#include "SkaarhojAnalog.h"
//#include "ADS7828.h"
#include "SkaarhojDisplayArray.h"
#include "Adafruit_GFX.h"
#include "SkaarhojOLED64x256.h"
#include "SK_CFGDEF_REFERENCE.h"
#elif (SK_MODEL == SK_C90MII) 
#include "SK_CFGDEF_C90MII.h"
#elif (SK_MODEL == SK_C90D)
#include "SK_CFGDEF_C90D.h"
#endif

#if SK_HWEN_STDOLEDDISPLAY || SK_HWEN_SSWMENU || SK_HWEN_SSWBUTTONS || SK_MODEL == SK_RCP || SK_MODEL == SK_CCUX || SK_MODEL == SK_WINGMAN || SK_MODEL == SK_WINGMANTOUCH || SK_MODEL == REFERENCE
#define SK_HWEN_GRAPHICS 1
#else
#define SK_HWEN_GRAPHICS 0
#endif

// Includes per Generic HW feature
#if SK_HWEN_STDOLEDDISPLAY
#include "Adafruit_GFX.h"
#include "SkaarhojDisplayArray.h"
SkaarhojDisplayArray infoDisplay;
#endif

#if SK_HWEN_SLIDER
#include "SkaarhojUtils.h"
SkaarhojUtils slider;
#endif

#if SK_HWEN_JOYSTICK
#include "ADS7828.h"
#include "SkaarhojAnalog.h"
SkaarhojAnalog joystick;
#endif

#if SK_HWEN_MENU
SkaarhojEncoders menuEncoders;
#include "SkaarhojEADOGMDisplay.h"
SkaarhojEADOGMDisplay menuDisplay;
#endif

#if SK_HWEN_SSWMENU
#include "Adafruit_GFX.h"
#include "SkaarhojSmartSwitch2.h"
SkaarhojSmartSwitch2 SSWmenu;
SkaarhojEncoders SSWmenuEnc;
MCP23017 SSWmenuChip;
#endif

#if SK_HWEN_SSWBUTTONS
#include "Adafruit_GFX.h"
#include "SkaarhojSmartSwitch2.h"
SkaarhojSmartSwitch2 SSWbuttons;
#endif

#if SK_HWEN_ACM
#include "SkaarhojAudioControl2.h"
#include "SkaarhojAnalog.h"
#include "ADS7828.h"
SkaarhojAudioControl2 AudioMasterControl;
SkaarhojAnalog AudioMasterPot;
#endif

#if SK_HWEN_GPIO
#include "SkaarhojGPIO2x8.h"
SkaarhojGPIO2x8 GPIOboard;
#endif

/****************************************

   GLOBAL VARIABLES and FUNCTION DEFINITIONS

 ****************************************/
#define SK_CONFIG_MEMORY_SIZE 1500
#define SK_MAXSTATES 10 // 0-9
uint8_t globalConfigMem[SK_CONFIG_MEMORY_SIZE];
uint8_t _systemState = 0;
uint8_t _systemShift = 0;
uint8_t _systemShiftRegister[4] = {0, 0, 0, 0};              // Shift registers (in addition to the default)
uint8_t _systemMem[4] = {0, 0, 0, 0};              // Mem A-D
uint8_t _systemRangeUpper[4] = {255, 255, 255, 255};              // Upper range A-D
uint8_t _systemRangeLower[4] = {0, 0, 0, 0};              // Lower range A-D
uint8_t _systemScaler[4] = {0, 0, 0, 0};              // Scaler function for A-D
uint8_t _systemBits[8] = {0, 0, 0, 0, 0, 0, 0, 0}; // 64 system bits
uint16_t _systemHWcActionCache[SK_HWCCOUNT][SK_MAXACTIONS];
uint8_t _systemHWcActionCacheFlag[SK_HWCCOUNT][SK_MAXACTIONS];
bool _systemHWcActionFineFlag[SK_HWCCOUNT];
bool _systemHWcActionPrefersLabel[SK_HWCCOUNT];
uint8_t HWdis[SK_HWCCOUNT];
uint8_t MODdis[SK_MODCOUNT];
uint16_t _systemPrevState = 0;
uint8_t actionMirror = 0;
bool _inactivePanel = false;
bool _inactivePanel_actDown = false;
bool _calibrateMode = false;
uint8_t debugMode = SK_SERIAL_OUTPUT;
uint16_t _retValue = 0;                                                    // Return value in actionDispatch. In global memory so it can be accessed.

// Pre-declaring. Implemented in "SKAARHOJbase.h":
int32_t pulsesHelper(int32_t inValue, const int32_t lower, const int32_t higher, const bool cycle, const int16_t pulses, const int16_t scaleFine = 1, const int16_t scaleNormal = 1);
uint16_t customActionHandlerNative(const uint16_t actionPtr, const uint8_t HWc, const uint8_t actIdx, const bool actDown = false, const bool actUp = false, const uint8_t pulses = 0, const uint16_t value = 0);
uint16_t actionDispatch(const uint8_t HWcNum, const bool actDown = false, const bool actUp = false, const int16_t pulses = 0, const int16_t value = 0x8000, const uint8_t specificAction = 0);
uint16_t getNumOfActions(const uint8_t HWcNum);
void lDelay(uint16_t delayVal);
uint8_t HWsetupL();
void HWtestL();
void HWcfgDisplay();
void storePreset(uint8_t index, uint8_t type, uint8_t *buffer);
bool recallPreset(uint8_t index, uint8_t type, uint8_t* buffer);
bool presetChecksumMatches(uint8_t index);
bool presetExists(uint8_t index, uint8_t type);

// Pre-declaring functions defined in the individual hardware files
uint8_t HWnumOfAnalogComponents();
int16_t HWAnalogComponentValue(uint8_t num);
uint16_t *HWMinCalibrationValues(uint8_t num);
void HWanalogComponentName(uint8_t num, char* buffer, uint8_t len);

/****************************************

   EXTENDED RETURN VALUE (global functions and variables)
   Related to "extended return values", or what is basically driving complex outputs like displays.

 ****************************************/
int16_t _extRetValue[2] = {0, 0};
bool _extRetValIsWanted = false;
bool _extRetValIsLabel = false;
uint8_t _extRetPair = 0;
uint8_t _extRetFormat = 0;
uint8_t _extRetScaleType = 0;
int16_t _extRetRangeLow = 0;
int16_t _extRetRangeHigh = 0;
int16_t _extRetLimitLow = 0;
int16_t _extRetLimitHigh = 0;
char _extRetLong[17];
char _extRetShort[11];
uint8_t _extRetLongPtr = 0;
uint8_t _extRetShortPtr = 0;
char _extRetTxtShort[6];
char _extRetTxt[2][17];
uint8_t _extRetColor = B010101;
char _strCache[22]; // Used for number to string conversions

void extRetValPrefersLabel(uint8_t HWc) {
  if (HWc > 0 && HWc <= SK_HWCCOUNT)
    _systemHWcActionPrefersLabel[HWc - 1] = true;
}

/**
   Sets the extended return value (long) and initializes everything else.
   ALWAYS call first!
   Format: 0=integer XX, 1=float from 10^3 (X.XX), 2=XX%, 3=XXdb, 4=XXf, 5=1/XX, 6=XXK, 7=Blank (not printed)
   Fine: Fine-flag
*/
void extRetVal(int16_t value, uint8_t format = 0, bool fine = 0) {
  _extRetValue[0] = value;
  _extRetFormat = (format & B1111) | (fine ? B10000 : 0);
  // Resetting:
  _extRetPair = 0;
  _extRetValue[1] = 0;
  _extRetScaleType = 0;
  _extRetRangeLow = 0;
  _extRetRangeHigh = 0;
  _extRetLimitLow = 0;
  _extRetLimitHigh = 0;
  memset(_extRetLong, 0, 17);
  memset(_extRetShort, 0, 11);
  _extRetLongPtr = 0;
  _extRetShortPtr = 0;
  memset(_extRetTxtShort, 0, 6);
  memset(_extRetTxt[0], 0, 17);
  memset(_extRetTxt[1], 0, 17);
  _extRetColor = B010101;
  _extRetValIsLabel = false;
}

/**
   Returns isWanted flag (but sets it first to input value)
*/
bool extRetValIsWanted(bool set = false) {
  bool v = _extRetValIsWanted;
  _extRetValIsWanted = set;
  if (set)
    extRetVal(0, 7); // init/clear display
  return v;
}

/**
   Returns isWanted flag (but sets it first to input value)
*/
void extRetValSetLabel(bool flag) {
  _extRetValIsLabel = flag;
}

/**
   Sets the extended return value (long) and initializes everything else.
   scaleType: 0=no scale, 1=fill, 2=center
*/
void extRetValScale(const uint8_t scaleType, const int16_t rangeLow, const int16_t rangeHigh, const int16_t limitLow, const int16_t limitHigh) {
  _extRetScaleType = scaleType;
  _extRetRangeLow = rangeLow;
  _extRetRangeHigh = rangeHigh;
  _extRetLimitLow = limitLow;
  _extRetLimitHigh = limitHigh;
}

/**
    Sets secondary value. Values are printed in smaller size.
    Pair: 1=two values shown, 2=box around first value, 3=box around second value, 4=box around both values
*/
void extRetVal2(int16_t value2, uint8_t pair = 1) {
  _extRetValue[1] = value2;
  _extRetPair = pair;
}

/**
    Sets long label
*/
void extRetValLongLabel(const char *longLabel, const int16_t number = 0x8000) {
  if (17 - 1 - _extRetLongPtr > 0) {
    strncpy_P(_extRetLong + _extRetLongPtr, longLabel, 17 - 1 - _extRetLongPtr);
    _extRetLongPtr += strlen_P(longLabel);
  }
  if (number != (int16_t)0x8000) {
    itoa(number, _strCache, 10);
    if (17 - 1 - _extRetLongPtr > 0) {
      strncpy(_extRetLong + _extRetLongPtr, _strCache, 17 - 1 - _extRetLongPtr);
      _extRetLongPtr += strlen(_strCache);
    }
  }
}

/**
    Sets short label
*/
void extRetValShortLabel(const char *shortLabel, const int16_t number = 0) {
  if (11 - 1 - _extRetShortPtr > 0) {
    strncpy_P(_extRetShort + _extRetShortPtr, shortLabel, 11 - 1 - _extRetShortPtr);
    _extRetShortPtr += strlen_P(shortLabel);
  }
  if (number != 0) {
    itoa(number, _strCache, 10);
    if (11 - 1 - _extRetShortPtr > 0) {
      strncpy(_extRetShort + _extRetShortPtr, _strCache, 11 - 1 - _extRetShortPtr);
      _extRetShortPtr += strlen(_strCache);
    }
  }
}

/**
    Alternative Prefix labels for values in case of 64 pixels wide displays
*/
void extRetValTxtShort(const char *txt) {
  strncpy(_extRetTxtShort, txt, 5);
}

/**
    Prefix labels for values
*/
void extRetValTxt(const char *txt, uint8_t i) {
  strncpy(_extRetTxt[i], txt, 17 - 1);
  extRetValTxtShort(txt);
}

/**
    Alternative Prefix labels for values in case of 64 pixels wide displays
*/
void extRetValTxtShort_P(const char *txt) {
  strncpy_P(_extRetTxtShort, txt, 5);
}

/**
    Prefix labels, PROGMEM input
*/
void extRetValTxt_P(const char *txt, uint8_t i) {
  strncpy_P(_extRetTxt[i], txt, 17 - 1);
  extRetValTxtShort_P(txt);
}

/**
    Button color (if RGB SmartSwitch)
    xxRRGGBB
*/
void extRetValColor(uint8_t col) {
  _extRetColor = col;
}

/**
    Calculating change hash
    Using pgm_read_word_near() to read "random" but fixed stuff from memory to spice up the hash.
*/
uint16_t extRetValHash() {
  uint16_t cs = 0;

  cs ^= pgm_read_word_near(cs) ^ lowByte(_extRetValue[0]);
  cs ^= pgm_read_word_near(cs) ^ highByte(_extRetValue[0]);

  cs ^= pgm_read_word_near(cs) ^ lowByte(_extRetValue[1]);
  cs ^= pgm_read_word_near(cs) ^ highByte(_extRetValue[1]);

  cs += _extRetPair;
  cs += _extRetFormat;
  cs += _extRetScaleType;
  cs += _extRetRangeLow;
  cs += _extRetRangeHigh;
  cs += _extRetLimitLow;
  cs += _extRetLimitHigh;

  for (uint8_t a = 0; a < 16; a++) {
    cs ^= pgm_read_word_near(cs) ^ _extRetLong[a];
    cs ^= pgm_read_word_near(cs) ^ _extRetTxt[0][a];
    cs ^= pgm_read_word_near(cs) ^ _extRetTxt[1][a];
  }
  for (uint8_t a = 0; a < 10; a++) {
    cs ^= pgm_read_word_near(cs) ^ _extRetShort[a];
  }
  for (uint8_t a = 0; a < 5; a++) {
    cs ^= pgm_read_word_near(cs) ^ _extRetTxtShort[a];
  }
  cs += _extRetValIsLabel << 4;

  /*  cs += lowByte(~_extRetValue[0]);
    cs += highByte(~_extRetValue[0]);

    cs += lowByte(_extRetValue[1]);
    cs += highByte(_extRetValue[1]);

    cs += _extRetPair;
    cs += _extRetFormat;
    cs += _extRetScaleType;
    cs += _extRetRangeLow;
    cs += _extRetRangeHigh;
    cs += _extRetLimitLow;
    cs += _extRetLimitHigh;

    for (uint8_t a = 0; a < 16; a++)   {
      cs += _extRetLong[a] ^ pgm_read_word_near(123+a);
      cs += _extRetTxt[0][a] ^ pgm_read_word_near(2345+a);
      cs += _extRetTxt[1][a] ^ pgm_read_word_near(4564+a);
    }
    for (uint8_t a = 0; a < 10; a++)   {
      cs += _extRetShort[a]  ^ pgm_read_word_near(986+a);
    }
    for (uint8_t a = 0; a < 5; a++)   {
      cs += _extRetTxtShort[a]  ^ pgm_read_word_near(7925+a);
    }
    cs += _extRetValIsLabel<<4;
  */

  return cs;
}

/**
   EEPROM location pointers required for device libraries
*/

#define EEPROM_FILEBANK_START 4095-6*48
#define EEPROM_FILEBANK_NUM 6

// Preset types
#define PRESET_CCU 1
#define PRESET_VIDEOHUB 2

#define BINARY_EVENT INT16_MIN

/****************************************

   INCLUDING DEVICE RELATED LIBRARIES
   Libraries for the devices we want to control,
   like ATEM switchers etc.

 ****************************************/
#if SK_DEVICES_ATEM
#include "ATEMbase.h"
#include "ATEMuni.h"
ATEMuni AtemSwitcher[SK_DEVICES_ATEM];
uint8_t AtemSwitcher_initIdx = 0;
#include "SK_DEV_ATEM.h"
#endif

#if SK_DEVICES_HYPERDECK
#include "SkaarhojBufferTools.h"
#include "SkaarhojASCIIClient.h"
#include "SkaarhojTCPClient.h"
#include "ClientBMDHyperdeckStudio.h"
ClientBMDHyperdeckStudio HyperDeck[SK_DEVICES_HYPERDECK];
uint8_t HyperDeck_initIdx = 0;
#include "SK_DEV_HYPERDECK.h";
#endif

#if SK_DEVICES_VIDEOHUB
#include "SkaarhojBufferTools.h"
#include "SkaarhojASCIIClient.h"
#include "SkaarhojTCPClient.h"
#include "ClientBMDVideohubTCP.h"
ClientBMDVideohubTCP VideoHub[SK_DEVICES_VIDEOHUB];
uint8_t VideoHub_initIdx = 0;
#include "SK_DEV_VIDEOHUB.h";
#endif

#if SK_DEVICES_SMARTSCOPE
#include "SkaarhojBufferTools.h"
#include "SkaarhojASCIIClient.h"
#include "SkaarhojTCPClient.h"
#include "ClientBMDSmartView.h"
ClientBMDSmartView SmartView[SK_DEVICES_SMARTSCOPE];
uint8_t SmartView_initIdx = 0;
#include "SK_DEV_SMARTSCOPE.h";
#endif

#if SK_DEVICES_BMDCAMCTRL
#include "BMDSDIControl.h"
#include "ClientBMDCamCtrl.h"
ClientBMDCamCtrl BMDCamCtrl[SK_DEVICES_BMDCAMCTRL];
uint8_t BMDCamCtrl_initIdx = 0;
#include "SK_DEV_BMDCAMCTRL.h";
#endif

#if SK_DEVICES_SONYRCP
#include "ClientSonyRCP.h"
ClientSonyRCP SonyRCP[SK_DEVICES_SONYRCP];
uint8_t SonyRCP_initIdx = 0;
#include "SK_DEV_SONYRCP.h";
#endif

#if SK_DEVICES_VMIX
#include "ClientVMixUDP.h"
ClientVMixUDP VMIX[SK_DEVICES_VMIX];
uint8_t VMIX_initIdx = 0;
#include "SK_DEV_VMIX.h";
#endif

#if SK_DEVICES_ROLANDVR50
#include "ClientRolandVR50IP.h"
ClientRolandVR50IP ROLANDVR50[SK_DEVICES_ROLANDVR50];
uint8_t ROLANDVR50_initIdx = 0;
#include "SK_DEV_ROLANDVR50.h";
#endif

#if SK_DEVICES_PANAAWHEX
#include "ClientPanaAWHExTCP.h"
ClientPanaAWHExTCP PANAAWHEX[SK_DEVICES_PANAAWHEX];
uint8_t PANAAWHEX_initIdx = 0;
#include "SK_DEV_PANAAWHEX.h";
#endif

#if SK_DEVICES_MATROXMONARCH
#include "ClientMatroxMonarch.h"
ClientMatroxMonarch MATROXMONARCH[SK_DEVICES_MATROXMONARCH];
uint8_t MATROXMONARCH_initIdx = 0;
#include "SK_DEV_MATROXMONARCH.h";
#endif

#if SK_DEVICES_H264REC
#include "GenericHTTPClient.h"
#include "ClientLLabH264Recorder.h"
ClientLLabH264Recorder H264REC[SK_DEVICES_H264REC];
uint8_t H264REC_initIdx = 0;
#include "SK_DEV_H264REC.h";
#endif

#if SK_DEVICES_SONYVISCAIP
#include "ClientSonyVISCAoverIP.h"
ClientSonyVISCAoverIP SONYVISCAIP[SK_DEVICES_SONYVISCAIP];
uint8_t SONYVISCAIP_initIdx = 0;
#include "SK_DEV_SONYVISCAIP.h";
#endif





/****************************************

   INCLUDING BASIC FUNCTIONS

 ****************************************/

#include "SKAARHOJbase.h"
#include "SKAARHOJwebserver.h"

/****************************************

   INCLUDING HARDWARE RELATED LIBRARIES, II
   Include file with controller specific functions
   that bind to the hardware actions

 ****************************************/

// Includes per model:
#if (SK_MODEL == SK_E21TVS)
#include "SK_CTRL_E21TVS.h"
#elif(SK_MODEL == SK_E21KP01)
#include "SK_CTRL_E21KP01.h"
#elif(SK_MODEL == SK_E21SLD)
#include "SK_CTRL_E21SLD.h"
#elif(SK_MODEL == SK_E21SSW)
#include "SK_CTRL_E21SSW.h"
#elif(SK_MODEL == SK_E21CMB6)
#include "SK_CTRL_E21CMB6.h"
#elif(SK_MODEL == SK_E21CMB6M)
#include "SK_CTRL_E21CMB6M.h"
#elif(SK_MODEL == SK_E21GPIO)
#include "SK_CTRL_E21GPIO.h"
#elif(SK_MODEL == SK_E201M16)
#include "SK_CTRL_E201M16.h"
#elif(SK_MODEL == SK_E201L2)
#include "SK_CTRL_E201L2.h"
#elif(SK_MODEL == SK_E201S16)
#include "SK_CTRL_E201S16.h"
#elif(SK_MODEL == SK_MICROAUDIO)
#include "SK_CTRL_MICROAUDIO.h"
#elif(SK_MODEL == SK_MICROBI16)
#include "SK_CTRL_MICROBI16.h"
#elif(SK_MODEL == SK_MICROGPIO)
#include "SK_CTRL_MICROGPIO.h"
#elif(SK_MODEL == SK_MICROKP01)
#include "SK_CTRL_MICROKP01.h"
#elif(SK_MODEL == SK_MICROLEVELS)
#include "SK_CTRL_MICROLEVELS.h"
#elif(SK_MODEL == SK_MICROMONITOR)
#include "SK_CTRL_MICROMONITOR.h"
#elif(SK_MODEL == SK_MICROSMARTE)
#include "SK_CTRL_MICROSMARTE.h"
#elif(SK_MODEL == SK_MICROSMARTH)
#include "SK_CTRL_MICROSMARTH.h"
#elif(SK_MODEL == SK_MICROSMARTV)
#include "SK_CTRL_MICROSMARTV.h"
#elif(SK_MODEL == SK_MICROTALLY)
#include "SK_CTRL_MICROTALLY.h"
#elif(SK_MODEL == SK_C90)
#include "SK_CTRL_C90.h"
#elif(SK_MODEL == SK_C90A)
#include "SK_CTRL_C90A.h"
#elif(SK_MODEL == SK_C90SM)
#include "SK_CTRL_C90SM.h"
#elif(SK_MODEL == SK_C31)
#include "SK_CTRL_C31.h"
#elif(SK_MODEL == SK_C10)
#include "SK_CTRL_C10.h"
#elif(SK_MODEL == SK_C15)
#include "SK_CTRL_C15.h"
#elif(SK_MODEL == SK_RCP)
#include "SK_CTRL_RCP.h"
#elif(SK_MODEL == SK_WINGMAN)
#include "SK_CTRL_WINGMAN.h"
#elif (SK_MODEL == SK_WINGMANTOUCH)
#include "SK_CTRL_WINGMANTOUCH.h"
#elif(SK_MODEL == SK_CCUX)
#include "SK_CTRL_CCUX.h"
#elif(SK_MODEL == SK_TALLY)
#include "SK_CTRL_TALLY.h"
#elif(SK_MODEL == SK_CUSTOMHW)
#include "SK_CTRL_CUSTOMHW.h"
#elif(SK_MODEL == SK_DUMMY)
#include "SK_CTRL_DUMMY.h"
#elif(SK_MODEL == SK_REFERENCE)
#include "SK_CTRL_REFERENCE.h"
#elif(SK_MODEL == SK_C90MII)
#include "SK_CTRL_C90MII.h"
#elif(SK_MODEL == SK_C90D)
#include "SK_CTRL_C90D.h"
#endif

#ifndef SK_CUSTOM_HANDLER_NATIVE
uint16_t customActionHandlerNative(const uint16_t actionPtr, const uint8_t HWc, const uint8_t actIdx, const bool actDown, const bool actUp, const uint8_t pulses, const uint16_t value) {}
#endif

// Custom handlers
#include "customhandler.h"
#ifndef SK_CUSTOM_HANDLER_A
uint16_t customActionHandlerA(const uint16_t actionPtr, const uint8_t HWc, const uint8_t actIdx, const bool actDown, const bool actUp, const uint8_t pulses, const uint16_t value) {}
#endif
#ifndef SK_CUSTOM_HANDLER_B
uint16_t customActionHandlerB(const uint16_t actionPtr, const uint8_t HWc, const uint8_t actIdx, const bool actDown, const bool actUp, const uint8_t pulses, const uint16_t value) {}
#endif
#ifndef SK_CUSTOM_HANDLER_C
uint16_t customActionHandlerC(const uint16_t actionPtr, const uint8_t HWc, const uint8_t actIdx, const bool actDown, const bool actUp, const uint8_t pulses, const uint16_t value) {}
#endif
#ifndef SK_CUSTOM_HANDLER_D
uint16_t customActionHandlerD(const uint16_t actionPtr, const uint8_t HWc, const uint8_t actIdx, const bool actDown, const bool actUp, const uint8_t pulses, const uint16_t value) {}
#endif

/**
   Standard Arduino setup() function
*/
void setup() {

  initController(); // Initializes Serial, Hardware, Config mode, Ethernet
  Serial << F("Compiled: ") << __DATE__ << F(" ") << __TIME__ << F("\n");

  if (getConfigMode()) {
    webserver.begin();
    webserver.setDefaultCommand(&defaultCmd);
    webserver.addCommand("script.js", &scriptCmd);
  } else {
    deviceSetup(); // Sets up hardware devices (those enabled) we communicate to. No initialization though, this must happen automatically in the runloop of each device
  }

  //Wire.setClock(400000L);  // Set this after device init because wire.begin() may be called and reset this...

  Serial << F("setup() Done\n-----------------------------\n");
}

/**
  Standard Arduino loop() function
*/
void loop() {
  checkIncomingSerial();
  statusSerial();

  if (getConfigMode()) {
    statusLED(defaultStatusColor(), 1);
    HWtest();
    webserver.processConnection();
  } else {
    if (getDevUnconnected()) {
      statusLED(LED_YELLOW, 4);
    } else {
      statusLED(defaultStatusColor(), 1);
    }

    deviceRunLoop(); // Keeping in touch with enabled hardware devices and updating the deviceReady booleans so we know which devices can be safely talked to.
    HWrunLoop();
  }
}
