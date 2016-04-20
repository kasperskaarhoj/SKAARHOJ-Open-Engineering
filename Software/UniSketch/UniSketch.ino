/**
 * Universal Sketch for generic SKAARHOJ devices
 *
 * By configuration you can compile software for various SKAARHOJ devices
 * as well as target broadcast devices
 */

// Define model (according to list):
#define SK_MODEL SK_E21TVS

// Disable (-1) / Enable (1) HW features beyond the default included with each model:
#define SK_HWCONF_STDOLEDDISPLAY 0
#define SK_HWCONF_SLIDER 0
#define SK_HWCONF_JOYSTICK 0
#define SK_HWCONF_MENU 0
#define SK_HWCONF_SSWMENU 0
#define SK_HWCONF_SSWBUTTONS 0
#define SK_HWCONF_ACM 0
#define SK_HWCONF_GPIO 1
#define SK_MODELVAR 0












// ****************************
// NO USER CHANGE BELOW!
// ****************************
#define SK_SERIAL_OUTPUT 1

#define SK_HWEN_STDOLEDDISPLAY 0
#define SK_HWEN_SLIDER 0
#define SK_HWEN_JOYSTICK 0
#define SK_HWEN_MENU 0
#define SK_HWEN_SSWMENU 0
#define SK_HWEN_SSWBUTTONS 0
#define SK_HWEN_ACM 0
#define SK_HWEN_GPIO 0

// Definition of the various controller models this sketch supports:
#define SK_E21TVS 1
#define SK_E21KP01 2
#define SK_E21SLD 3
#define SK_E21SSW 4
#define SK_E21CMB6 5
#define SK_E21CMB6X 6
#define SK_E201L16 10
#define SK_E201M16 14
#define SK_E201S16 17
#define SK_MICROAUDIO 20
#define SK_MICROBI16  21
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

// Definition of the hardware devices we support
#define SK_DEV_ATEM 1
#define SK_DEV_HYPERDECK 2
#define SK_DEV_VIDEOHUB 3
#define SK_DEV_SMARTSCOPE 4


#define SK_VERSION "0.9.1"    // Version of UniSketch

#include "customization.h"

/****************************************
 *
 * INCLUDING HARDWARE RELATED LIBRARIES, I
 * Libraries with controller hardware configuration
 * and physical hardware libraries
 *
 ****************************************/
#include "SPI.h"
#include "Wire.h"
#include "Ethernet.h"
#include "utility/w5100.h"
#include "Streaming.h"
#include "EEPROM.h"
#include "SkaarhojPgmspace.h"
#include "MemoryFree.h"
#include "SkaarhojTools.h"
SkaarhojTools sTools(0);
#define WEBDUINO_FAVICON_DATA ""
#include "WebServer.h"


// Always include these, it's almost inconceivable we are not using them...
#include "MCP23017.h"
#include "PCA9685.h"




// Includes per model:
#if (SK_MODEL == SK_E21TVS)
#include "SK_CFGDEF_E21TVS.h"
#if SK_HWCONF_SLIDER!=-1
#define SK_HWEN_SLIDER 1
#endif
#if SK_HWCONF_GPIO==1
#define SK_HWEN_GPIO 1
#endif
#include "SkaarhojBI16.h"
SkaarhojBI16 buttons;
#endif

#if (SK_MODEL == SK_MICROBI16)
#include "SkaarhojBI16.h"
SkaarhojBI16 buttons;
#endif

#if (SK_MODEL == SK_E21CMB6)
#if SK_HWCONF_STDOLEDDISPLAY!=-1
#define SK_HWEN_STDOLEDDISPLAY 1
#endif
#if SK_HWCONF_SLIDER!=-1
#define SK_HWEN_SLIDER 1
#endif
#if SK_HWCONF_SSWMENU!=-1
#define SK_HWEN_SSWMENU 1
#endif
#include "SkaarhojBI8.h"
SkaarhojBI8 buttons;
SkaarhojBI8 buttons2;
#endif

#if (SK_MODEL == SK_C15)
#include "SkaarhojBI8.h"
SkaarhojBI8 buttons;
#if SK_HWCONF_JOYSTICK!=-1
#define SK_HWEN_JOYSTICK 1
#endif
#if SK_HWCONF_SSWBUTTONS!=-1
#define SK_HWEN_SSWBUTTONS 1
#endif
#endif

#if (SK_MODEL == SK_C90 || SK_MODEL == SK_C90A || SK_MODEL == SK_C90SM)
#include "SkaarhojBI8.h"
SkaarhojBI8 buttons;
SkaarhojBI8 buttons2;
#if SK_HWCONF_SLIDER!=-1
#define SK_HWEN_SLIDER 1
#endif
#endif
#if (SK_MODEL == SK_C90SM || SK_MODEL == SK_C90A)
#if SK_HWCONF_ACM!=-1
#define SK_HWEN_ACM 1
#endif
#endif
#if (SK_MODEL == SK_C90SM)
#if SK_HWCONF_SSWMENU!=-1
#define SK_HWEN_SSWMENU 1
#endif
#if SK_HWCONF_MENU!=-1
#define SK_HWEN_MENU 1
#endif
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
#include "SkaarhojEncoders.h"
SkaarhojEncoders menuEncoders;
#include "SkaarhojEADOGMDisplay.h"
SkaarhojEADOGMDisplay menuDisplay;
#endif

#if SK_HWEN_SSWMENU
#include "Adafruit_GFX.h"
#include "SkaarhojSmartSwitch2.h"
SkaarhojSmartSwitch2 SSWmenu;
#endif

#if SK_HWEN_SSWBUTTONS
#include "Adafruit_GFX.h"
#include "SkaarhojSmartSwitch2.h"
SkaarhojSmartSwitch2 SSWbuttons;
#endif

#if SK_HWEN_ACM
#include "SkaarhojAudioControl2.h"
SkaarhojAudioControl2 AudioMasterControl;
#endif

#if SK_HWEN_GPIO
#include "SkaarhojGPIO2x8.h"
SkaarhojGPIO2x8 GPIOboard;
#endif







/****************************************
 *
 * INCLUDING DEVICE RELATED LIBRARIES
 * Libraries for the devices we want to control,
 * like ATEM switchers etc.
 *
 ****************************************/
#define SK_CONFIG_MEMORY_SIZE 1500
uint8_t globalConfigMem[SK_CONFIG_MEMORY_SIZE];
uint8_t _systemState = 0;
uint8_t _systemShift = 0;
uint8_t _systemMem[4] = {0, 0, 0, 0}; // Mem A-D
uint16_t _systemHWcActionCache[SK_HWCCOUNT][SK_MAXACTIONS];
bool _systemHWcActionCacheFlag[SK_HWCCOUNT][SK_MAXACTIONS];
uint16_t _systemPrevState = 0;



#if SK_DEVICES_ATEM
#include "ATEMext.h";
#include "ATEMbase.h";
ATEMext AtemSwitcher[SK_DEVICES_ATEM];
uint8_t AtemSwitcher_initIdx = 0;
#include "SK_DEV_ATEM.h";
#endif

#if SK_DEVICES_HYPERDECK
#include "SkaarhojBufferTools.h"
#include "SkaarhojASCIIClient.h"
#include "SkaarhojTCPClient.h"
#include "ClientBMDHyperdeckStudio.h"
ClientBMDHyperdeckStudio HyperDeck[SK_DEVICES_HYPERDECK];
uint8_t HyperDeck_initIdx = 0;
#endif

#if SK_DEVICES_VIDEOHUB
#include "SkaarhojBufferTools.h"
#include "SkaarhojASCIIClient.h"
#include "SkaarhojTCPClient.h"
#include "ClientBMDVideohubTCP.h"
ClientBMDVideohubTCP VideoHub[SK_DEVICES_VIDEOHUB];
uint8_t VideoHub_initIdx = 0;
#endif

#if SK_DEVICES_SMARTSCOPE
#include "SkaarhojBufferTools.h"
#include "SkaarhojASCIIClient.h"
#include "SkaarhojTCPClient.h"
#include "ClientBMDSmartView.h"
ClientBMDSmartView SmartView[SK_DEVICES_SMARTSCOPE];
uint8_t SmartView_initIdx = 0;
#endif



















/****************************************
 *
 * INCLUDING BASIC FUNCTIONS
 *
 ****************************************/
#include "SKAARHOJbase.h"
#include "SKAARHOJwebserver.h"



/****************************************
 *
 * INCLUDING HARDWARE RELATED LIBRARIES, II
 * Include file with controller specific functions
 * that bind to the hardware actions
 *
 ****************************************/
#if (SK_MODEL == SK_E21TVS)
#include "SK_CTRL_E21TVS.h"
#endif



void setup() {
  initController(); // Initializes Serial, Hardware, Config mode, Ethernet

  Serial << F("setup() Done\n-----------------------------\n");

  if (getConfigMode())  {
    webserver.begin();
    webserver.setDefaultCommand(&defaultCmd);
    webserver.addCommand("script.js", &scriptCmd);
  } else {
    deviceSetup();  // Sets up hardware devices (those enabled) we communicate to. No initialization though, this must happen automatically in the runloop of each device
  }
}
void loop() {
  checkIncomingSerial();
  statusLED(defaultStatusColor(), 1);
  statusSerial();

  if (getConfigMode())  {
    HWtest();
    webserver.processConnection();
  } else {

    deviceRunLoop();    // Keeping in touch with enabled hardware devices and updating the deviceReady booleans so we know which devices can be safely talked to.
    HWrunLoop();
  }
}



