/*****************
 * Basis control for the SKAARHOJ C20x series devices
 * This example is programmed for ATEM 1M/E versions
 *
 * This example also uses several custom libraries which you must install first. 
 * Search for "#include" in this file to find the libraries. Then download the libraries from http://skaarhoj.com/wiki/index.php/Libraries_for_Arduino
 *
 * Works with ethernet-enabled arduino devices (Arduino Ethernet or a model with Ethernet shield)
 * Run the example sketch "ConfigEthernetAddresses" to set up Ethernet before installing this sketch.
 * 
 * - kasper
 */





 
 
// Including libraries: 
#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet.h>
#include "WebServer.h"  // For web interface
#include <EEPROM.h>      // For storing IP numbers
#include <MenuBackend.h>  // Library for menu navigation. Must (for some reason) be included early! Otherwise compilation chokes.


static uint8_t default_ip[] = {     // IP for Configuration Mode (192.168.10.99)
  192, 168, 10, 99 };
uint8_t ip[4];        // Will hold the C200 IP address
uint8_t atem_ip[4];  // Will hold the ATEM IP address
uint8_t mac[6];    // Will hold the Arduino Ethernet shield/board MAC address (loaded from EEPROM memory, set with ConfigEthernetAddresses example sketch)


/* MEMORY USAGE:
 * This can be used to track free memory. 
 * Include "MemoryFree.h" and use the following line
 *     Serial << F("freeMemory()=") << freeMemory() << "\n";  
 * in your code to see available memory.
 */
//#include <MemoryFree.h>



// No-cost stream operator as described at 
// http://arduiniana.org/libraries/streaming/
template<class T>
inline Print &operator <<(Print &obj, T arg)
{  
  obj.print(arg); 
  return obj; 
}


// Include ATEM library and make an instance:
#include <ATEM.h>
ATEM AtemSwitcher;

// All related to library "SkaarhojBI8", which controls the buttons:
#include "Wire.h"
#include "MCP23017.h"
#include "PCA9685.h"
#include "SkaarhojBI8.h"
SkaarhojBI8 previewSelect;
SkaarhojBI8 programSelect;
SkaarhojBI8 cmdSelect;
SkaarhojBI8 extraButtons;

// All related to library "SkaarhojUtils". Used for rotary encoder and "T-bar" potentiometer:
#include "SkaarhojUtils.h"
SkaarhojUtils utils;


































/*************************************************************
 *
 *
 *                     SMARTSWITCH FUNCTIONS
 *
 *
 **********************************************************/


#include <SkaarhojSmartSwitch.h>
SkaarhojSmartSwitch SmartSwitch;

// AUX graphic:
static uint8_t SSS_AUXgfx[] PROGMEM = {
  64, // width
  15, // height

     /* page 0 (lines 0-7) */
  0x0,0x0,0x80,0xc0,0xf0,0xfc,0xff,0xff,0x7f,0xf,0x1f,0xff,0xff,0xff,0xfc,0xf0,
  0xc0,0x0,0x0,0x0,0x0,0x0,0xff,0xff,0xff,0xff,0xff,0xff,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0xff,0xff,0xff,0xff,0xff,0xff,0x0,0x0,0x0,0x0,0x1,0x7,0xf,
  0x1f,0x3f,0xff,0xff,0xfc,0xf8,0xf0,0xf0,0xf8,0xfe,0xff,0x7f,0x3f,0xf,0x7,0x3,
  
  /* page 1 (lines 8-15) */
  0x38,0x7e,0x7f,0x7f,0x7f,0x3f,0x1f,0x1f,0x1e,0x1e,0x1e,0x1e,0x1f,0x1f,0x3f,0x7f,
  0x7f,0x7f,0x7c,0x70,0x60,0x0,0xf,0x1f,0x1f,0x3f,0x3f,0x3f,0x78,0x70,0x70,0x70,
  0x70,0x70,0x78,0x7f,0x3f,0x3f,0x3f,0x1f,0xf,0x0,0x0,0x0,0x0,0x60,0x70,0x78,
  0x7c,0x7e,0x7f,0x3f,0x1f,0xf,0x7,0x7,0x7,0xf,0x1f,0x3f,0x7f,0x7e,0x78,0x70,
  
};

// PGM graphic:
static uint8_t SSS_PGMgfx[] PROGMEM = {
    64, // width
  32, // height

  /* page 0 (lines 0-7) */
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  
  /* page 1 (lines 8-15) */
  0xff,0xff,0xff,0xff,0xff,0xff,0xc7,0xc7,0xc7,0xc7,0xff,0xff,0xff,0xfe,0xfe,0x7c,
  0x0,0x0,0x0,0xf0,0xf8,0xfc,0xfe,0xfe,0xfe,0x1f,0xf,0x7,0x7,0x87,0x87,0x8f,
  0x9f,0x9f,0x8e,0x8e,0x8c,0x88,0x0,0x0,0x0,0x0,0xff,0xff,0xff,0xff,0xff,0x3f,
  0xff,0xff,0xfc,0xc0,0x0,0x0,0xf0,0xfe,0xff,0x7f,0x1f,0xff,0xff,0xff,0xff,0xff,
  
  /* page 2 (lines 16-23) */
  0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x0,0x0,0x0,
  0x0,0x0,0x0,0x7,0xf,0x1f,0x3f,0x3f,0x3f,0x7c,0x78,0x70,0x70,0x73,0x73,0x73,
  0x73,0x3f,0x3f,0x3f,0x3f,0x1f,0x0,0x0,0x0,0x0,0x7f,0x7f,0x7f,0x7f,0x7f,0x0,
  0x1,0xf,0x7f,0x7f,0x7e,0x7f,0x7f,0x3f,0x3,0x0,0x0,0x7f,0x7f,0x7f,0x7f,0x7f,
  
  /* page 3 (lines 24-31) */
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  
};


// PIP_L graphic:
static uint8_t SSS_PIPLgfx[] PROGMEM = {
  64, // width
  32, // height

  /* page 0 (lines 0-7) */
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xff,0xff,0xff,0xff,0xff,0xff,0xc7,
  0xc7,0xc7,0xc7,0xff,0xff,0xff,0xfe,0xfe,0x7c,0x0,0x0,0x0,0x0,0xff,0xff,0xff,
  0xff,0xff,0xff,0x0,0x0,0x0,0x0,0xff,0xff,0xff,0xff,0xff,0xff,0xc7,0xc7,0xc7,
  0xc7,0xff,0xff,0xff,0xfe,0xfe,0x7c,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  
  /* page 1 (lines 8-15) */
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x1,
  0x1,0x1,0x1,0x1,0x1,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x7f,0x7f,0x7f,
  0x7f,0x7f,0x7f,0x0,0x0,0x0,0x0,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x1,0x1,0x1,
  0x1,0x1,0x1,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  
  /* page 2 (lines 16-23) */
  0x0,0x0,0x0,0x0,0xfc,0xfc,0xc,0xc,0xc,0xc,0xc,0xc,0xc,0xc,0xc,0xc,
  0xc,0xc,0xc,0xc,0xc,0xc,0xc,0xc,0xc,0xc,0xc,0xc,0xc,0xfc,0xfc,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  
  /* page 3 (lines 24-31) */
  0x0,0x0,0x0,0x0,0x3f,0x3f,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,
  0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x3f,0x3f,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,  
};

// PIP_R graphic:
static uint8_t SSS_PIPRgfx[] PROGMEM = {
  64, // width
  32, // height

  /* page 0 (lines 0-7) */
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xff,0xff,0xff,0xff,0xff,0xff,0xc7,
  0xc7,0xc7,0xc7,0xff,0xff,0xff,0xfe,0xfe,0x7c,0x0,0x0,0x0,0x0,0xff,0xff,0xff,
  0xff,0xff,0xff,0x0,0x0,0x0,0x0,0xff,0xff,0xff,0xff,0xff,0xff,0xc7,0xc7,0xc7,
  0xc7,0xff,0xff,0xff,0xfe,0xfe,0x7c,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  
  /* page 1 (lines 8-15) */
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x1,
  0x1,0x1,0x1,0x1,0x1,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x7f,0x7f,0x7f,
  0x7f,0x7f,0x7f,0x0,0x0,0x0,0x0,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x1,0x1,0x1,
  0x1,0x1,0x1,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  
  /* page 2 (lines 16-23) */
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0xfc,0xfc,0xc,0xc,0xc,0xc,0xc,0xc,0xc,0xc,0xc,0xc,0xc,0xc,0xc,
  0xc,0xc,0xc,0xc,0xc,0xc,0xc,0xc,0xc,0xc,0xfc,0xfc,0x0,0x0,0x0,0x0,
  
  /* page 3 (lines 24-31) */
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  0x0,0x3f,0x3f,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,
  0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x3f,0x3f,0x0,0x0,0x0,0x0,
  
};





























/*************************************************************
 *
 *
 *                     LCD PANEL FUNCTIONS
 *
 *
 **********************************************************/




#include <SoftwareSerial.h>
#define txPin 7

SoftwareSerial LCD = SoftwareSerial(0, txPin);
// Since the LCD does not send data back to the Arduino, we should only define the txPin
const int LCDdelay=1;  // 10 is conservative, 2 actually works

// wbp: goto with row & column
void lcdPosition(int row, int col) {
  LCD.write(0xFE);   //command flag
  LCD.write((col + row*64 + 128));    //position 
  AtemSwitcher.delay(LCDdelay);
}
void clearLCD(){
  LCD.write(0xFE);   //command flag
  LCD.write(0x01);   //clear command.
  AtemSwitcher.delay(LCDdelay);
}
void backlightOn() {  //turns on the backlight
  LCD.write(0x7C);   //command flag for backlight stuff
  LCD.write(157);    //light level.
  AtemSwitcher.delay(LCDdelay);
}
void backlightOff(){  //turns off the backlight
  LCD.write(0x7C);   //command flag for backlight stuff
  LCD.write(128);     //light level for off.
  AtemSwitcher.delay(LCDdelay);
}
void serCommand(){   //a general function to call the command flag for issuing all other commands   
  LCD.write(0xFE);
}
void lcdPrintValue(int number, uint8_t padding)  {
  LCD.print(number);
  for(int i=String(number).length(); i<padding; i++)  {
    LCD.print(" ");
  }
}



































/*************************************************************
 *
 *
 *                     MENU SYSTEM
 *
 *
 **********************************************************/



uint8_t userButtonMode = 0;  // 0-3
uint8_t setMenuValues = 0;  //
uint8_t BUSselect = 0;  // Preview/Program by default

// Configuration of the menu items and hierarchi plus call-back functions:
MenuBackend menu = MenuBackend(menuUseEvent,menuChangeEvent);
  // Beneath is list of menu items needed to build the menu
  MenuItem menu_UP1       = MenuItem(menu, "< Exit", 1);
  MenuItem menu_mediab1   = MenuItem(menu, "Media Bank 1", 1);    // On Change: Show selected item/Value (2nd encoder rotates). On Use: N/A
  MenuItem menu_mediab2   = MenuItem(menu, "Media Bank 2", 1);    // (As Media Bank 1)
  MenuItem menu_userbut   = MenuItem(menu, "User Buttons", 1);    // On Change: N/A. On Use: Show active configuration
    MenuItem menu_usrcfg1 = MenuItem(menu, "DSK1     VGA+PIPAUTO         PIP", 2);  // On Change: Use it (and se as default)! On Use: Exit
    MenuItem menu_usrcfg2 = MenuItem(menu, "DSK1        DSK2AUTO         PIP", 2);  // (As above)
    MenuItem menu_usrcfg3 = MenuItem(menu, "KEY1        KEY2KEY3        KEY4", 2);  // (As above)
    MenuItem menu_usrcfg4 = MenuItem(menu, "COLOR1    COLOR2BLACK       BARS", 2);  // (As above)
    MenuItem menu_usrcfg5 = MenuItem(menu, "AUX1        AUX2AUX3     PROGRAM", 2);  // (As above)
  MenuItem menu_trans     = MenuItem(menu, "Transitions", 1);    // (As User Buttons)
    MenuItem menu_trtype  = MenuItem(menu, "Type", 2);  // (As Media Bank 1)
    MenuItem menu_trtime  = MenuItem(menu, "Trans. Time", 2);  // (As Media Bank 1)
  MenuItem menu_ftb       = MenuItem(menu, "Fade To Black", 1);
    MenuItem menu_ftbtime = MenuItem(menu, "FTB Time", 2);
    MenuItem menu_ftbexec = MenuItem(menu, "Do Fade to Black", 2);
  MenuItem menu_aux1      = MenuItem(menu, "AUX 1", 1);    // (As Media Bank 1)
  MenuItem menu_aux2      = MenuItem(menu, "AUX 2", 1);    // (As Media Bank 1)
  MenuItem menu_aux3      = MenuItem(menu, "AUX 3", 1);    // (As Media Bank 1)
  MenuItem menu_network   = MenuItem(menu, "Network", 1);
    MenuItem menu_ownIP   = MenuItem(menu, "Own IP", 2);
    MenuItem menu_AtemIP  = MenuItem(menu, "ATEM IP", 2);
  MenuItem menu_UP2       = MenuItem(menu, "< Exit", 1);

// This function builds the menu and connects the correct items together
void menuSetup()
{
  Serial << F("Setting up menu...");

  // Add first to the menu root:
  menu.getRoot().add(menu_mediab1); 

    // Setup the rest of menu items on level 1:
    menu_mediab1.addBefore(menu_UP1);
    menu_mediab1.addAfter(menu_mediab2);
    menu_mediab2.addAfter(menu_userbut);
    menu_userbut.addAfter(menu_trans);
    menu_trans.addAfter(menu_ftb);
    menu_ftb.addAfter(menu_aux1);
    menu_aux1.addAfter(menu_aux2);
    menu_aux2.addAfter(menu_aux3);
    menu_aux3.addAfter(menu_UP2);

      // Set up user button menu:
    menu_usrcfg1.addAfter(menu_usrcfg2);  // Chain subitems...
    menu_usrcfg2.addAfter(menu_usrcfg3);
    menu_usrcfg3.addAfter(menu_usrcfg4);
    menu_usrcfg4.addAfter(menu_usrcfg5);
    menu_usrcfg2.addLeft(menu_userbut);  // Add parent item - starting with number 2...
    menu_usrcfg3.addLeft(menu_userbut);
    menu_usrcfg4.addLeft(menu_userbut);
    menu_usrcfg5.addLeft(menu_userbut);
    menu_userbut.addRight(menu_usrcfg1);     // Add the submenu to the parent - this will also see "left" for "menu_usercfg1"

      // Set up transition menu:
    menu_trtype.addAfter(menu_trtime);    // Chain subitems...
    menu_trtime.addLeft(menu_trans);      // Add parent item
    menu_trans.addRight(menu_trtype);     // Add the submenu to the parent - this will also see "left" for "menu_trtype"

      // Set up fade-to-black menu:
    menu_ftbtime.addAfter(menu_ftbexec);    // Chain subitems...
    menu_ftbexec.addLeft(menu_ftb);      // Add parent item
    menu_ftb.addRight(menu_ftbtime);     // Add the submenu to the parent
}

/*
  Here all use events are handled. Mainly these are used to navigate in to and out of menu items with the encoder button.
*/
void menuUseEvent(MenuUseEvent used)
{
  setMenuValues=0;

  if (used.item.getName()=="MenuRoot")  {
     menu.moveDown(); 
  }
  
    // Exit in upper level:
  if (used.item.isEqual(menu_UP1) || used.item.isEqual(menu_UP2))  {
    menu.toRoot(); 
  }

    // This will set the selected element as default when entering the menu again.
    // PS: I don't know why I needed to put the "*" before "used.item.getLeft()" It was a lucky guess, or...?
  if (used.item.getLeft())  {
    used.item.addLeft(*used.item.getLeft());
  }
  
    // Using an element moves left or right depending on where there are elements.
    // This works fine for a two level menu like this one.
  if((bool)used.item.getRight())  {
    menu.moveRight();
  } else {
    menu.moveLeft();
  }
}

/*
  Here we get a notification whenever the user changes the menu
  That is, when the menu is navigated
*/
void menuChangeEvent(MenuChangeEvent changed)
{
  setMenuValues=0;
  
  if (changed.to.getName()=="MenuRoot")  {
      // Show default text.... status whatever....
    clearLCD();
    lcdPosition(0,0);
    LCD.print(F("    SKAARHOJ    "));    
    lcdPosition(1,0);
    LCD.print(F("      C200      "));    
    setMenuValues=0;
  } else {
      // Show the item name in upper line:
    lcdPosition(0,0);
    LCD.print(changed.to.getName());
    for(int i=strlen(changed.to.getName()); i<16; i++)  {
      LCD.print(F(" "));
    }
    if (strlen(changed.to.getName())>16)  {
      lcdPosition(1,0);
      for(int i=16; i<strlen(changed.to.getName()); i++)  {
        LCD.print(changed.to.getName()[i]);
      }
    }
    
      // If there are no menu items to the right, we assume its a value change:
    if (!(bool)changed.to.getRight())  {
        if (!menu_userbut.isEqual(*changed.to.getLeft()))  {  // If it is not the special "User Button" selection, show the value and set a flag for Encoder 1 to operate (see ....)
          lcdPosition(1,0);
          LCD.print(F("                "));  // Clear the line...
        }
        
          // Make settings as a consequence of menu selection:
        if (changed.to.getName() == menu_usrcfg1.getName())  { userButtonMode=0; }
        if (changed.to.getName() == menu_usrcfg2.getName())  { userButtonMode=1; }
        if (changed.to.getName() == menu_usrcfg3.getName())  { userButtonMode=2; }
        if (changed.to.getName() == menu_usrcfg4.getName())  { userButtonMode=3; }
        if (changed.to.getName() == menu_usrcfg5.getName())  { userButtonMode=4; }
        if (changed.to.getName() == menu_mediab1.getName())  { setMenuValues=1;  }
        if (changed.to.getName() == menu_mediab2.getName())  { setMenuValues=2;  }
        if (changed.to.getName() == menu_aux1.getName())  { setMenuValues=3;  }
        if (changed.to.getName() == menu_aux2.getName())  { setMenuValues=4;  }
        if (changed.to.getName() == menu_aux3.getName())  { setMenuValues=5;  }
        if (changed.to.getName() == menu_trtype.getName())  { setMenuValues=10;  }
        if (changed.to.getName() == menu_trtime.getName())  { setMenuValues=11;  }
        if (changed.to.getName() == menu_ftbtime.getName())  { setMenuValues=20;  }
        if (changed.to.getName() == menu_ftbexec.getName())  { setMenuValues=21;  }
          // TODO: I HAVE to find another way to match the items here because two items with the same name will choke!!
        
        
    } else {  // Just clear the displays second line if there are items to the right in the menu:
      lcdPosition(0,15);
      LCD.print(F(">"));  // Arrow + Clear the line...
      lcdPosition(1,0);
      LCD.print(F("                "));  // Arrow + Clear the line...
    }
  }
}



// Interrupt functions for encoders:
void _enc0active()  {
  utils.encoders_interrupt(0);
}
void _enc1active()  {
  utils.encoders_interrupt(1);
}
















































/*************************************************************
 *
 *
 *                     Webserver
 *
 *
 **********************************************************/




#define PREFIX ""
WebServer webserver(PREFIX, 80);

void logoCmd(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{
  /* this data was taken from a PNG file that was converted to a C data structure
   * by running it through the directfb-csource application. 
   * (Alternatively by PHPSH script "HeaderGraphicsWebInterfaceInUnitsPNG8.phpsh")
   */
  P(logoData) = {
	0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 
	0x00, 0x0d, 0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x02, 0xa2, 
	0x00, 0x00, 0x00, 0x2b, 0x08, 0x03, 0x00, 0x00, 0x00, 0x94, 
	0xff, 0x1a, 0xf8, 0x00, 0x00, 0x00, 0x19, 0x74, 0x45, 0x58, 
	0x74, 0x53, 0x6f, 0x66, 0x74, 0x77, 0x61, 0x72, 0x65, 0x00, 
	0x41, 0x64, 0x6f, 0x62, 0x65, 0x20, 0x49, 0x6d, 0x61, 0x67, 
	0x65, 0x52, 0x65, 0x61, 0x64, 0x79, 0x71, 0xc9, 0x65, 0x3c, 
	0x00, 0x00, 0x00, 0x30, 0x50, 0x4c, 0x54, 0x45, 0xe6, 0xe6, 
	0xe6, 0x0d, 0x5c, 0xaf, 0x0c, 0x61, 0xbd, 0x57, 0x89, 0xcc, 
	0xd7, 0xe2, 0xf2, 0xfa, 0xfa, 0xfa, 0x85, 0xa8, 0xd9, 0xab, 
	0xc2, 0xe3, 0xc7, 0xd6, 0xec, 0x0b, 0x51, 0x9d, 0x36, 0x76, 
	0xc4, 0x23, 0x68, 0xb9, 0x0a, 0x65, 0xc5, 0x0a, 0x65, 0xc6, 
	0x0d, 0x5e, 0xb5, 0xff, 0xff, 0xff, 0xe6, 0xb3, 0x8a, 0x8c, 
	0x00, 0x00, 0x07, 0x1e, 0x49, 0x44, 0x41, 0x54, 0x78, 0xda, 
	0xec, 0x5d, 0x0d, 0x7b, 0xa3, 0x20, 0x0c, 0x56, 0xa8, 0xc0, 
	0xd4, 0xe2, 0xff, 0xff, 0xb7, 0x67, 0x05, 0x42, 0x12, 0x82, 
	0xda, 0xae, 0xf7, 0xcc, 0x6e, 0xc4, 0x75, 0x53, 0xf2, 0x8d, 
	0xaf, 0xaf, 0xca, 0xed, 0xee, 0xba, 0x25, 0xca, 0xe0, 0xc6, 
	0xf9, 0xde, 0xa4, 0xc9, 0x45, 0x44, 0x8d, 0x6e, 0x08, 0xc8, 
	0xec, 0x00, 0xa1, 0xaa, 0x4d, 0x4b, 0x93, 0x2b, 0xc9, 0x1c, 
	0x31, 0x1a, 0x21, 0x3a, 0x0c, 0x63, 0x9b, 0x93, 0x26, 0x17, 
	0xe3, 0x51, 0x06, 0xd1, 0x46, 0xa2, 0x4d, 0xae, 0x86, 0xd1, 
	0x61, 0xc0, 0x37, 0xfa, 0xae, 0xcd, 0x48, 0x93, 0xab, 0x49, 
	0x87, 0x21, 0x3a, 0x34, 0x88, 0x36, 0xb9, 0x9c, 0x10, 0x16, 
	0x1d, 0x86, 0x36, 0x21, 0x4d, 0x2e, 0xce, 0xa2, 0xf3, 0xbc, 
	0xbe, 0x43, 0xa5, 0x77, 0xa9, 0xf8, 0xb9, 0xa3, 0x65, 0xa8, 
	0xa0, 0x9f, 0x83, 0x6a, 0xfd, 0x31, 0x0b, 0x23, 0xd9, 0x75, 
	0x4e, 0xdb, 0x9c, 0x62, 0x04, 0xbb, 0xcd, 0x67, 0xfb, 0x1e, 
	0x13, 0x91, 0x48, 0x31, 0x54, 0xb0, 0x03, 0x1b, 0xe4, 0x99, 
	0x7d, 0xe6, 0x6c, 0x1f, 0x7d, 0x90, 0xd1, 0x7c, 0xa7, 0xda, 
	0x58, 0xfd, 0x7c, 0xc7, 0x3d, 0xcd, 0x2c, 0x4b, 0x2a, 0x1c, 
	0x77, 0x9e, 0x5b, 0x40, 0xe3, 0x78, 0x52, 0x50, 0x40, 0xc8, 
	0x87, 0xcd, 0x52, 0xc5, 0x77, 0x16, 0x36, 0x55, 0x19, 0x1d, 
	0x67, 0xe8, 0xfb, 0x8e, 0x6b, 0x7c, 0x29, 0xda, 0xcb, 0x15, 
	0x3c, 0xe9, 0xfa, 0xe6, 0x66, 0xd1, 0x11, 0xc0, 0xa3, 0x23, 
	0x8b, 0x4e, 0xdd, 0xfc, 0xa7, 0xe4, 0x7e, 0x46, 0x73, 0xff, 
	0xd9, 0x4a, 0x7e, 0x7e, 0x2e, 0x7e, 0x3a, 0x15, 0x7f, 0x16, 
	0x9d, 0x9b, 0x34, 0xb9, 0x96, 0x9c, 0x64, 0x51, 0x63, 0x6e, 
	0xa5, 0x98, 0xf5, 0x82, 0xd0, 0xdb, 0x9e, 0x4e, 0x76, 0x3a, 
	0xab, 0xc2, 0x05, 0x33, 0xd2, 0xe3, 0x2c, 0xa5, 0x42, 0xdf, 
	0xa4, 0x1c, 0x63, 0x56, 0x59, 0xe4, 0xee, 0x68, 0xde, 0xd5, 
	0x24, 0x97, 0x68, 0x8c, 0x11, 0x2f, 0x5e, 0x53, 0x44, 0x69, 
	0xf2, 0x01, 0xb7, 0xba, 0x53, 0x2c, 0x2a, 0xe1, 0xf3, 0x01, 
	0x90, 0x7b, 0x02, 0x4f, 0x80, 0x8a, 0x02, 0x94, 0xe9, 0x3b, 
	0x77, 0x1d, 0xe7, 0x68, 0x52, 0x51, 0xd4, 0x20, 0x1a, 0x40, 
	0x7c, 0x04, 0xd1, 0x89, 0x55, 0xe8, 0x6e, 0x46, 0xe3, 0x06, 
	0x14, 0x4a, 0xf9, 0x46, 0x88, 0xaa, 0xff, 0x7e, 0x7e, 0xd4, 
	0x5f, 0x02, 0xa3, 0x3a, 0xcb, 0xa2, 0x8a, 0x6d, 0xb3, 0xb2, 
	0xb7, 0x0a, 0x78, 0x14, 0x86, 0x28, 0x1c, 0xdc, 0x9c, 0x86, 
	0x30, 0x23, 0x02, 0x9a, 0x0a, 0x35, 0x94, 0x8a, 0x38, 0xa4, 
	0xeb, 0x69, 0x12, 0x44, 0x73, 0x55, 0x09, 0xa2, 0xdb, 0x91, 
	0xa9, 0xb8, 0xa9, 0x14, 0x3a, 0xfc, 0xb4, 0x45, 0x94, 0xdc, 
	0x63, 0x65, 0xdb, 0xd3, 0xed, 0xaa, 0x9e, 0x51, 0x9e, 0xb1, 
	0x7b, 0x25, 0xd5, 0x37, 0x2a, 0x78, 0x36, 0xdd, 0x7b, 0x9b, 
	0x25, 0x87, 0xfc, 0x59, 0x34, 0xe9, 0xb1, 0x69, 0x82, 0x8e, 
	0x1d, 0x85, 0x18, 0x09, 0xa2, 0x0a, 0xf6, 0x6f, 0x6e, 0xcc, 
	0xc9, 0x32, 0x78, 0x98, 0xb3, 0xa0, 0xe0, 0x38, 0x34, 0x16, 
	0x2c, 0xf6, 0x21, 0x6a, 0xc2, 0x81, 0x33, 0x5b, 0x28, 0x70, 
	0x33, 0x38, 0x5f, 0x2e, 0xc6, 0xd2, 0xee, 0x4e, 0x6c, 0x6f, 
	0x52, 0x3d, 0x99, 0xf5, 0xfb, 0xd1, 0x5e, 0xaf, 0xa0, 0x3c, 
	0xf8, 0x6f, 0xa9, 0x8e, 0x37, 0xce, 0xa2, 0x21, 0x3e, 0xc9, 
	0xe4, 0x6e, 0xe9, 0x06, 0x29, 0x14, 0xa2, 0x33, 0x54, 0x34, 
	0x45, 0x32, 0xe1, 0xca, 0x88, 0x2f, 0xa8, 0x9d, 0x2b, 0x72, 
	0x28, 0x8b, 0x2e, 0x01, 0x9b, 0x2c, 0x34, 0xb5, 0x5c, 0x37, 
	0x04, 0x51, 0x93, 0x58, 0x13, 0xae, 0xa9, 0x58, 0xb1, 0x56, 
	0xa4, 0x97, 0x04, 0x51, 0x85, 0x67, 0x90, 0xce, 0x66, 0x7d, 
	0x9e, 0x77, 0xed, 0xe6, 0xea, 0x29, 0x55, 0x95, 0x93, 0xac, 
	0x76, 0xd0, 0x50, 0x1f, 0xdf, 0x8f, 0x26, 0x17, 0x7f, 0x5c, 
	0x41, 0xe9, 0x52, 0xda, 0xef, 0x17, 0xff, 0xce, 0x66, 0x8b, 
	0xf9, 0xbe, 0x0b, 0x2c, 0xaa, 0x48, 0x5c, 0x80, 0xa8, 0x51, 
	0x4a, 0x38, 0x07, 0x3a, 0x81, 0x01, 0x10, 0x3a, 0xdd, 0x55, 
	0x4c, 0xa0, 0x22, 0x7a, 0x0c, 0x05, 0x8c, 0x02, 0x58, 0x61, 
	0x05, 0x62, 0x51, 0x45, 0x79, 0xf5, 0xe6, 0x54, 0x46, 0x6f, 
	0xca, 0x1d, 0x21, 0xba, 0xee, 0x4d, 0x0e, 0x41, 0x3b, 0x7c, 
	0x8d, 0x37, 0x14, 0x6a, 0xcb, 0x58, 0xb0, 0x28, 0x9a, 0x09, 
	0x05, 0x7e, 0xe4, 0x4b, 0x65, 0x0d, 0x0a, 0x2d, 0x1c, 0xce, 
	0x6c, 0xbe, 0x58, 0xcc, 0x72, 0xe2, 0x71, 0xdc, 0xe2, 0x33, 
	0x2b, 0xa9, 0x92, 0x33, 0xd1, 0xd2, 0x2e, 0xde, 0x29, 0xf0, 
	0x26, 0x55, 0xc0, 0xf7, 0x59, 0x6f, 0x47, 0xc5, 0x3f, 0x93, 
	0x6a, 0xbf, 0xd9, 0xb9, 0xec, 0xb4, 0x64, 0x51, 0x55, 0x4a, 
	0xe6, 0x32, 0x41, 0x99, 0x21, 0x9a, 0x10, 0xaa, 0x90, 0xd9, 
	0x18, 0x41, 0xeb, 0x22, 0x7b, 0xed, 0x2b, 0x12, 0x0e, 0xb9, 
	0xd9, 0x7a, 0xa7, 0x2f, 0x55, 0x0e, 0xd0, 0x1d, 0x80, 0xe7, 
	0xa6, 0x0c, 0x2a, 0x78, 0xbe, 0x98, 0x68, 0xad, 0xb6, 0x88, 
	0xd2, 0xe4, 0xfa, 0x52, 0x3e, 0x8b, 0x96, 0x62, 0xd0, 0xb3, 
	0x68, 0x15, 0xa2, 0x3a, 0x53, 0x2d, 0xf7, 0xd4, 0x33, 0xb0, 
	0xe5, 0xae, 0xa2, 0x0e, 0xd1, 0x69, 0x07, 0xa2, 0x11, 0x78, 
	0x86, 0xd4, 0x16, 0x1d, 0xf5, 0x2c, 0x94, 0xd3, 0x20, 0xfa, 
	0x61, 0x72, 0xcc, 0xa2, 0x4a, 0x7c, 0xa3, 0xb7, 0x66, 0xc4, 
	0x10, 0x15, 0x11, 0x3a, 0x26, 0x2e, 0x1b, 0xe3, 0xfd, 0x7a, 
	0x5f, 0x51, 0xe2, 0xd0, 0xf2, 0x1b, 0xbd, 0x04, 0x51, 0x27, 
	0x64, 0x16, 0x07, 0xe7, 0x06, 0xd1, 0xdf, 0xca, 0xa2, 0x4a, 
	0x5e, 0xd4, 0x59, 0xef, 0xd3, 0x80, 0x2b, 0x73, 0xe3, 0x3c, 
	0x99, 0xdd, 0x0c, 0x60, 0x0d, 0x48, 0x4d, 0x56, 0x70, 0x1c, 
	0xc6, 0x17, 0xf5, 0x87, 0xba, 0xb6, 0x1e, 0x95, 0x21, 0xca, 
	0x08, 0x33, 0x42, 0xd4, 0xd3, 0x7a, 0xba, 0x06, 0xd1, 0x5f, 
	0xca, 0xa2, 0x4a, 0x4d, 0x46, 0x64, 0xd2, 0xcc, 0xa2, 0xae, 
	0x4e, 0xa2, 0x2b, 0xdb, 0x7a, 0x4d, 0x68, 0xb4, 0xa2, 0xa8, 
	0xaf, 0x8b, 0xaa, 0x63, 0x88, 0x4a, 0x2c, 0xea, 0x0c, 0x6b, 
	0xa2, 0xb1, 0xe8, 0x47, 0x42, 0x94, 0xb2, 0x68, 0xcd, 0xcc, 
	0x63, 0x3e, 0xd2, 0xb0, 0xf2, 0xe8, 0x13, 0x44, 0x47, 0x01, 
	0x28, 0x18, 0x11, 0x0e, 0xb3, 0x6c, 0xa9, 0x58, 0x03, 0xf9, 
	0x0a, 0x0e, 0xdd, 0x83, 0x0a, 0xab, 0x10, 0xf5, 0x40, 0xc4, 
	0xd2, 0x8d, 0x9e, 0xb3, 0x7a, 0x63, 0xd1, 0x5f, 0xc0, 0xa2, 
	0xeb, 0x29, 0xf7, 0xb0, 0xa5, 0x9d, 0x00, 0x4f, 0xaf, 0x60, 
	0xc8, 0xa7, 0x97, 0xfc, 0x44, 0x83, 0xda, 0x8f, 0x69, 0xe9, 
	0xc7, 0xab, 0x68, 0x34, 0x02, 0x8a, 0x32, 0xc6, 0xa6, 0x87, 
	0x6e, 0x04, 0x06, 0x04, 0xc5, 0xe3, 0x7d, 0xdc, 0x4b, 0x38, 
	0x34, 0xe6, 0x91, 0xdd, 0xc3, 0x33, 0x00, 0x14, 0x01, 0x18, 
	0x8c, 0xb5, 0x18, 0x52, 0xf4, 0x94, 0x52, 0xa4, 0x6a, 0xb6, 
	0x26, 0xe2, 0xa5, 0x81, 0x1b, 0x83, 0x7d, 0xd6, 0x72, 0x3e, 
	0x0c, 0x3f, 0x49, 0xf7, 0x58, 0x81, 0xac, 0xb1, 0x0b, 0x8e, 
	0x21, 0xc4, 0x22, 0xf1, 0x7d, 0x51, 0x42, 0x9c, 0x74, 0xea, 
	0x57, 0x89, 0xe6, 0x85, 0x13, 0xf6, 0x8a, 0x4f, 0x32, 0x78, 
	0xb2, 0xf8, 0x77, 0x36, 0xeb, 0xc5, 0xea, 0x4a, 0x16, 0x65, 
	0x73, 0x5e, 0x04, 0xc5, 0x88, 0xbb, 0xc1, 0x8d, 0x1e, 0x80, 
	0xb7, 0x62, 0xd4, 0x13, 0x18, 0x53, 0xc4, 0x3d, 0x74, 0xb2, 
	0x22, 0x3f, 0x8b, 0x6e, 0x19, 0x52, 0xb8, 0x7e, 0xab, 0x00, 
	0xab, 0xc2, 0x16, 0x21, 0xba, 0xea, 0xe2, 0xa2, 0x13, 0x29, 
	0x3c, 0x8e, 0x85, 0x03, 0x68, 0xde, 0x42, 0x85, 0xb9, 0x2b, 
	0x61, 0x47, 0xda, 0x3c, 0x36, 0x42, 0xc7, 0x82, 0xe5, 0xe9, 
	0x61, 0x79, 0x68, 0xb7, 0x8c, 0xe7, 0xa2, 0x7d, 0xc7, 0xe7, 
	0x5b, 0xe9, 0xde, 0xd4, 0x6c, 0xfc, 0xc1, 0x9f, 0x45, 0xb7, 
	0xe9, 0xf7, 0xf9, 0xcc, 0x86, 0x4f, 0x1c, 0x4d, 0x43, 0x89, 
	0xf0, 0x9c, 0xef, 0xd3, 0xa2, 0x93, 0x4f, 0xac, 0xb9, 0xbe, 
	0x44, 0x6d, 0x36, 0xe2, 0xbd, 0xd9, 0x4d, 0x87, 0x0a, 0x1b, 
	0x61, 0x90, 0xd6, 0x59, 0x7d, 0x3e, 0xb0, 0x3e, 0x57, 0xe7, 
	0x20, 0x6f, 0x58, 0x5c, 0xed, 0x34, 0x14, 0xab, 0x7c, 0x9f, 
	0x1e, 0x1f, 0x08, 0xa4, 0x26, 0x3c, 0x8a, 0x9a, 0x42, 0x3b, 
	0xb9, 0x59, 0x55, 0x34, 0x5e, 0xda, 0xe0, 0x19, 0x52, 0x85, 
	0x92, 0x84, 0x57, 0x2c, 0xa2, 0xaa, 0x79, 0xa8, 0x32, 0xdc, 
	0xd9, 0x68, 0xd8, 0xee, 0xa4, 0x8f, 0x92, 0x2b, 0xc0, 0xf5, 
	0x1d, 0x16, 0xff, 0x6a, 0xb3, 0xd2, 0xd4, 0xd1, 0x70, 0x71, 
	0x97, 0xb3, 0x28, 0x3b, 0x13, 0x54, 0x60, 0xc8, 0x00, 0x7e, 
	0x22, 0x44, 0x7b, 0x84, 0xdb, 0x69, 0x33, 0x91, 0x7f, 0xf7, 
	0xc4, 0xec, 0x28, 0x00, 0x87, 0x41, 0x60, 0x1d, 0xcb, 0x73, 
	0xd5, 0x63, 0xc4, 0xe5, 0xbc, 0x69, 0x39, 0x21, 0x6b, 0x6d, 
	0xfc, 0x5d, 0x01, 0x5a, 0xbd, 0x05, 0x3b, 0xd2, 0xa5, 0x92, 
	0xda, 0x15, 0x31, 0xcc, 0xa7, 0xba, 0x98, 0x1d, 0x55, 0xce, 
	0x94, 0x92, 0x6c, 0x4b, 0x00, 0x4a, 0x57, 0x81, 0x90, 0x6d, 
	0x37, 0x9a, 0x70, 0xb1, 0x1d, 0xfb, 0x88, 0xa5, 0x3c, 0x59, 
	0xfc, 0x6b, 0xcd, 0x2a, 0x5f, 0xe3, 0x09, 0x36, 0x05, 0xec, 
	0x6f, 0x80, 0xfa, 0x33, 0x32, 0x1a, 0x78, 0x5b, 0x49, 0x2c, 
	0x8a, 0x41, 0x65, 0xc7, 0xbc, 0x9f, 0x51, 0x32, 0x92, 0x3f, 
	0x28, 0x12, 0x14, 0x3d, 0x83, 0xe8, 0xa4, 0x01, 0xbc, 0x5c, 
	0x95, 0x21, 0x8a, 0xb0, 0xd7, 0x59, 0x33, 0x86, 0xea, 0x1c, 
	0xc5, 0x6c, 0xbc, 0xaa, 0xec, 0xad, 0x08, 0xd2, 0xe4, 0x13, 
	0xa4, 0xce, 0xa2, 0x59, 0xaa, 0xab, 0x41, 0x9e, 0xb0, 0x28, 
	0xf0, 0x99, 0x1b, 0x7b, 0x78, 0x89, 0x41, 0x18, 0x09, 0x23, 
	0x55, 0x85, 0x2f, 0x70, 0x98, 0x7e, 0x3f, 0xa4, 0xf7, 0xfb, 
	0x10, 0xf5, 0xd2, 0xba, 0xad, 0xd3, 0x22, 0x95, 0xdb, 0xa9, 
	0x9d, 0xf3, 0x4f, 0x83, 0xe8, 0x72, 0xcc, 0xa2, 0xba, 0xb6, 
	0xe2, 0x93, 0x21, 0xba, 0xd9, 0xf5, 0x3d, 0x60, 0x54, 0xe3, 
	0x7b, 0x7e, 0x64, 0x45, 0x47, 0x1f, 0x3d, 0xb9, 0x62, 0x2a, 
	0x71, 0x98, 0x30, 0x7a, 0x04, 0x51, 0xaf, 0x0b, 0x2c, 0x9a, 
	0x49, 0x7a, 0xda, 0x70, 0xa6, 0x9d, 0xf1, 0x3f, 0xc3, 0xa2, 
	0x36, 0x60, 0x80, 0xb2, 0xa8, 0x07, 0x8c, 0x8a, 0xb7, 0x5a, 
	0x86, 0x6e, 0xa6, 0xb0, 0x25, 0x0e, 0x01, 0xa3, 0x63, 0x0d, 
	0xa2, 0x29, 0xef, 0x7a, 0x75, 0xe0, 0xbf, 0x18, 0xa2, 0xd7, 
	0x81, 0x02, 0xa2, 0xeb, 0xb0, 0xef, 0xdb, 0x19, 0xff, 0x74, 
	0x16, 0x95, 0x4e, 0x61, 0xef, 0xe5, 0xd1, 0x88, 0x8d, 0xed, 
	0x3b, 0x19, 0xed, 0x45, 0x97, 0x68, 0x24, 0xa8, 0x82, 0xa6, 
	0xef, 0x45, 0xfc, 0xf4, 0x0f, 0x45, 0xe9, 0xd4, 0x7b, 0x92, 
	0xd7, 0x07, 0x9b, 0x14, 0x48, 0x46, 0x62, 0xc3, 0xe7, 0x27, 
	0x0a, 0x7f, 0xa3, 0x8f, 0x10, 0xea, 0x85, 0x8d, 0xed, 0x14, 
	0x56, 0xdc, 0xb3, 0x74, 0x49, 0x9e, 0xa5, 0x42, 0x74, 0xad, 
	0x55, 0x71, 0x30, 0x5a, 0xef, 0x60, 0x57, 0xc1, 0x5b, 0x94, 
	0xac, 0x4b, 0xed, 0x5e, 0xd3, 0x72, 0x80, 0x9a, 0x07, 0x9f, 
	0xa5, 0xe3, 0xea, 0x05, 0x8f, 0xe3, 0x86, 0x4f, 0x55, 0xf0, 
	0x62, 0xba, 0x37, 0x35, 0xcb, 0x5d, 0xf8, 0xb3, 0x68, 0xa0, 
	0xac, 0x00, 0xa4, 0xf8, 0x05, 0xc0, 0xea, 0x81, 0xd1, 0x7a, 
	0x34, 0xee, 0xc1, 0x10, 0x9b, 0x64, 0x53, 0x50, 0x79, 0x7a, 
	0x48, 0xdd, 0x7a, 0xec, 0xe9, 0x79, 0x78, 0x48, 0xeb, 0x69, 
	0x9c, 0x22, 0x37, 0xfb, 0x06, 0xd5, 0xa2, 0xa2, 0x41, 0x23, 
	0x54, 0x81, 0x83, 0x7a, 0xa2, 0xf5, 0x3d, 0x6d, 0xcd, 0xe7, 
	0xc0, 0x24, 0x7b, 0x4e, 0x83, 0x2d, 0x89, 0x0f, 0x9f, 0x87, 
	0x5c, 0x10, 0xcb, 0xeb, 0x9f, 0x8d, 0x26, 0xd4, 0xf2, 0x5a, 
	0x05, 0xcf, 0xa4, 0x7b, 0x77, 0xb3, 0x6c, 0xaa, 0x8b, 0x67, 
	0x51, 0x00, 0x44, 0x93, 0x26, 0x17, 0x11, 0xb6, 0x2e, 0xda, 
	0x26, 0xa4, 0xc9, 0xd5, 0xa4, 0x60, 0xd1, 0x26, 0x4d, 0x2e, 
	0x06, 0x51, 0xfa, 0x4f, 0xe0, 0x7e, 0xb5, 0x19, 0x69, 0x72, 
	0x6d, 0x16, 0x9d, 0xda, 0x8c, 0x34, 0xb9, 0x96, 0x4c, 0x8c, 
	0x45, 0xed, 0x57, 0xe3, 0xd1, 0x26, 0x17, 0x92, 0xaf, 0x2f, 
	0x3b, 0xd0, 0xff, 0x31, 0xa4, 0xb3, 0xd3, 0x57, 0x93, 0x26, 
	0x97, 0x91, 0xc9, 0x76, 0xf4, 0x8d, 0x7e, 0x18, 0xba, 0x26, 
	0x4d, 0x2e, 0x25, 0x03, 0x65, 0xd1, 0x15, 0xa3, 0x0d, 0xa4, 
	0x4d, 0x2e, 0x05, 0x50, 0xf6, 0x5f, 0x83, 0x3d, 0x40, 0xba, 
	0xe1, 0xf4, 0xa1, 0xc9, 0x9f, 0x25, 0x1c, 0x2f, 0x69, 0x7c, 
	0x49, 0x7a, 0xf4, 0x73, 0xc1, 0x7b, 0x5c, 0x03, 0x9f, 0x34, 
	0xc2, 0x83, 0x0f, 0x52, 0xf0, 0xec, 0x20, 0x8e, 0x0c, 0xb2, 
	0x0d, 0x3e, 0x58, 0xb0, 0xeb, 0x52, 0xf5, 0x5c, 0x68, 0x61, 
	0xa1, 0x8a, 0x9d, 0x6e, 0x68, 0xc1, 0x03, 0xe9, 0x45, 0x98, 
	0x9e, 0x45, 0xc8, 0xb9, 0x94, 0xbd, 0x2c, 0xbc, 0xf6, 0x85, 
	0x54, 0xf4, 0x4c, 0xb4, 0xd7, 0x2a, 0x58, 0x4e, 0xba, 0x2e, 
	0xc5, 0xdc, 0xbd, 0xad, 0xd9, 0x81, 0xfa, 0x2c, 0x09, 0xa0, 
	0xcb, 0xf2, 0x4f, 0x80, 0x01, 0x00, 0x3d, 0x86, 0x58, 0xcf, 
	0x39, 0xc2, 0xf5, 0xc6, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 
	0x4e, 0x44, 0xae, 0x42, 0x60, 0x82  };

  if (type == WebServer::POST)
  {
    // ignore POST data
    server.httpFail();
    return;
  }

  /* for a GET or HEAD, send the standard "it's all OK headers" but identify our data as a PNG file */
  server.httpSuccess("image/png");

  /* we don't output the body for a HEAD request */
  if (type == WebServer::GET)
  {
    server.writeP(logoData, sizeof(logoData));
  }
}

// commands are functions that get called by the webserver framework
// they can read any posted data from client, and they output to server
void webDefaultView(WebServer &server, WebServer::ConnectionType type)
{
  P(htmlHead) =
    "<html>"
    "<head>"
    "<title>SKAARHOJ Device Configuration</title>"
    "<style type=\"text/css\">"
    "BODY { font-family: sans-serif }"
    "H1 { font-size: 14pt; }"
    "P  { font-size: 10pt; }"
    "</style>"
    "</head>"
    "<body>"
    "<img src='logo.png'><br/>";

  int i;
  server.httpSuccess();
  server.printP(htmlHead);

  server << "<div style='width:660px; margin-left:10px;'><form action='" PREFIX "/form' method='post'>";

  // Panel IP:
  server << "<h1>SKAARHOJ Device IP Address:</h1><p>";
  for (i = 0; i <= 3; ++i)
  {
    server << "<input type='text' name='IP" << i << "' value='" << EEPROM.read(i+2) << "' id='IP" << i << "' size='4'>";
    if (i<3)  server << '.';
  }
  server << "<hr/>";

  // ATEM Switcher Panel IP:
  server << "<h1>ATEM Switcher IP Address:</h1><p>";
  for (i = 0; i <= 3; ++i)
  {
    server << "<input type='text' name='ATEM_IP" << i << "' value='" << EEPROM.read(i+2+4) << "' id='ATEM_IP" << i << "' size='4'>";
    if (i<3)  server << '.';
  }
  server << "<hr/>";

  // End form and page:
  server << "<input type='submit' value='Submit'/></form></div>";
  server << "<br><i>(Reset / Pull the power after submitting the form successfully)</i>";
  server << "</body></html>";
}

void formCmd(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{
  if (type == WebServer::POST)
  {
    bool repeat;
    char name[16], value[16];
    do
    {
      repeat = server.readPOSTparam(name, 16, value, 16);
      String Name = String(name);

      // C200 Panel IP:
      if (Name.startsWith("IP"))  {
        int addr = strtoul(name + 2, NULL, 10);
        int val = strtoul(value, NULL, 10);
        if (addr>=0 && addr <=3)  {
          EEPROM.write(addr+2, val);  // IP address stored in bytes 0-3
        }
      }

      // ATEM Switcher Panel IP:
      if (Name.startsWith("ATEM_IP"))  {
        int addr = strtoul(name + 7, NULL, 10);
        int val = strtoul(value, NULL, 10);
        if (addr>=0 && addr <=3)  {
          EEPROM.write(addr+2+4, val);  // IP address stored in bytes 4-7
        }
      }

    } 
    while (repeat);

    server.httpSeeOther(PREFIX "/form");
  }
  else
    webDefaultView(server, type);
}
void defaultCmd(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{
  webDefaultView(server, type);  
}





























/*************************************************************
 *
 *
 *                     MAIN PROGRAM CODE AHEAD
 *
 *
 **********************************************************/



bool isConfigMode;  // If set, the system will run the Web Configurator, not the normal program

void setup() { 
  Serial.begin(9600);
  Serial << F("\n- - - - - - - -\nSerial Started\n");
  

  // *********************************
  // Start up BI8 boards and I2C bus:
  // *********************************
    // Always initialize Wire before setting up the SkaarhojBI8 class!
  Wire.begin(); 
  
    // Set up the SkaarhojBI8 boards:
  previewSelect.begin(0,false);
  programSelect.begin(1,false);
  cmdSelect.begin(2,false);
  extraButtons.begin(3,false);


  
  // *********************************
  // Mode of Operation (Normal / Configuration)
  // *********************************
     // Determine web config mode
     // This is done by:
     // -  either flipping a switch connecting A1 to GND
     // -  Holding the CUT button during start up.
  pinMode(A1,INPUT_PULLUP);
  delay(100);
  isConfigMode = cmdSelect.buttonIsPressed(1) || (analogRead(A1) > 100) ? true : false;
  



  // *********************************
  // INITIALIZE EEPROM memory:
  // *********************************
  // Check if EEPROM has ever been initialized, if not, install default IP
  if (EEPROM.read(0) != 12 ||  EEPROM.read(1) != 232)  {  // Just randomly selected values which should be unlikely to be in EEPROM by default.
    // Set these random values so this initialization is only run once!
    EEPROM.write(0,12);
    EEPROM.write(1,232);

    // Set default IP address for Arduino/C100 panel (192.168.10.99)
    EEPROM.write(2,192);
    EEPROM.write(3,168);
    EEPROM.write(4,10);
    EEPROM.write(5,99);  // Just some value I chose, probably below DHCP range?

    // Set default IP address for ATEM Switcher (192.168.10.240):
    EEPROM.write(6,192);
    EEPROM.write(7,168);
    EEPROM.write(8,10);
    EEPROM.write(9,240);
  }
  
 
  // *********************************
  // Setting up IP addresses, starting Ethernet
  // *********************************
  if (isConfigMode)  {
    // Setting the default ip address for configuration mode:
    ip[0] = default_ip[0];
    ip[1] = default_ip[1];
    ip[2] = default_ip[2];
    ip[3] = default_ip[3];
  } else {
    ip[0] = EEPROM.read(0+2);
    ip[1] = EEPROM.read(1+2);
    ip[2] = EEPROM.read(2+2);
    ip[3] = EEPROM.read(3+2);
  }

  // Setting the ATEM IP address:
  atem_ip[0] = EEPROM.read(0+2+4);
  atem_ip[1] = EEPROM.read(1+2+4);
  atem_ip[2] = EEPROM.read(2+2+4);
  atem_ip[3] = EEPROM.read(3+2+4);
  
  Serial << F("SKAARHOJ Device IP Address: ") << ip[0] << "." << ip[1] << "." << ip[2] << "." << ip[3] << "\n";
  Serial << F("ATEM Switcher IP Address: ") << atem_ip[0] << "." << atem_ip[1] << "." << atem_ip[2] << "." << atem_ip[3] << "\n";
  
  // Setting MAC address:
  mac[0] = EEPROM.read(10);
  mac[1] = EEPROM.read(11);
  mac[2] = EEPROM.read(12);
  mac[3] = EEPROM.read(13);
  mac[4] = EEPROM.read(14);
  mac[5] = EEPROM.read(15);
  char buffer[18];
  sprintf(buffer, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  Serial << F("SKAARHOJ Device MAC address: ") << buffer << F(" - Checksum: ")
        << ((mac[0]+mac[1]+mac[2]+mac[3]+mac[4]+mac[5]) & 0xFF);
  if ((uint8_t)EEPROM.read(16)!=((mac[0]+mac[1]+mac[2]+mac[3]+mac[4]+mac[5]) & 0xFF))  {
    Serial << F("MAC address not found in EEPROM memory!\n") <<
      F("Please load example sketch ConfigEthernetAddresses to set it.\n") <<
      F("The MAC address is found on the backside of your Ethernet Shield/Board\n (STOP)");
    while(true);
  }

  Ethernet.begin(mac, ip);


  // ********************
  // Start up LCD (has to be after Ethernet has been started!)
  // ********************
  pinMode(txPin, OUTPUT);
  LCD.begin(9600);
  clearLCD();
  lcdPosition(0,0);
  LCD.print(F("    SKAARHOJ    "));
  lcdPosition(1,0);
  LCD.print(F("      C200      "));
  backlightOn();
  delay(2000);


  // *********************************
  // Final Setup based on mode
  // *********************************
  if (isConfigMode)  {
    
      // LCD IP info:
    clearLCD();
    lcdPosition(0,0);
    LCD.print(F("CONFIG MODE, IP:"));
    lcdPosition(1,0);
    LCD.print(ip[0]);
    LCD.print('.');
    LCD.print(ip[1]);
    LCD.print('.');
    LCD.print(ip[2]);
    LCD.print('.');
    LCD.print(ip[3]);

       // Red by default:
    previewSelect.setDefaultColor(2);
    programSelect.setDefaultColor(2);
    cmdSelect.setDefaultColor(2); 
    extraButtons.setDefaultColor(2);

    previewSelect.setButtonColorsToDefault();
    programSelect.setButtonColorsToDefault();
    cmdSelect.setButtonColorsToDefault();
    extraButtons.setButtonColorsToDefault();

    webserver.begin();
    webserver.setDefaultCommand(&defaultCmd);
    webserver.addCommand("form", &formCmd);
    webserver.addCommand("logo.png", &logoCmd);
  } else {

      // LCD IP info:
    clearLCD();
    lcdPosition(0,0);
    LCD.print(F("IP Address:"));
    lcdPosition(1,0);
    LCD.print(ip[0]);
    LCD.print('.');
    LCD.print(ip[1]);
    LCD.print('.');
    LCD.print(ip[2]);
    LCD.print('.');
    LCD.print(ip[3]);
    
    delay(1000);

      // Colors of buttons:
    previewSelect.setDefaultColor(0);  // Off by default
    programSelect.setDefaultColor(0);  // Off by default
    cmdSelect.setDefaultColor(0);  // Off by default
    extraButtons.setDefaultColor(0);  // Off by default

    previewSelect.testSequence(10);
    programSelect.testSequence(10);
    cmdSelect.testSequence(10);
    extraButtons.testSequence(10);
  
    // Initializing the slider:
    utils.uniDirectionalSlider_init();
    utils.uniDirectionalSlider_hasMoved();
    
    // Initializing menu control:
    utils.encoders_init();
    attachInterrupt(0, _enc1active, RISING);
    attachInterrupt(1, _enc0active, RISING);
    
    menuSetup();
    
    
        // Switches on address 4 + SDI on pins 48+49:  
    SmartSwitch.begin(4,48,49);
    SmartSwitch.setButtonBrightness(7, BUTTON_ALL);
    SmartSwitch.setButtonColor(2,2,2,BUTTON_ALL);
    SmartSwitch.clearScreen(BUTTON_ALL);
    
  
    // Connect to an ATEM switcher on this address and using this local port:
    // The port number is chosen randomly among high numbers.
    clearLCD();
    lcdPosition(0,0);
    LCD.print(F("Connecting to:"));
    lcdPosition(1,0);
    LCD.print(atem_ip[0]);
    LCD.print('.');
    LCD.print(atem_ip[1]);
    LCD.print('.');
    LCD.print(atem_ip[2]);
    LCD.print('.');
    LCD.print(atem_ip[3]);
    
    AtemSwitcher.begin(IPAddress(atem_ip[0],atem_ip[1],atem_ip[2],atem_ip[3]), 56417);
   // AtemSwitcher.serialOutput(true);
    AtemSwitcher.connect();
  }
}






/*************************************************************
 * Loop function (runtime loop)
 *************************************************************/

// These variables are used to track state, for instance when the VGA+PIP button has been pushed.
bool preVGA_active = false;
bool preVGA_UpstreamkeyerStatus = false;
int preVGA_programInput = 0;

// AtemOnline is true, if there is a connection to the ATEM switcher
bool AtemOnline = false;

// The loop function:
void loop() {
  
  if (isConfigMode)  {
    webserver.processConnection();
  } else {
  
    // Check for packets, respond to them etc. Keeping the connection alive!
    AtemSwitcher.runLoop();
    menuNavigation();
    menuValues();
    
    // If connection is gone, try to reconnect:
    if (AtemSwitcher.isConnectionTimedOut())  {
        if (AtemOnline)  {
          AtemOnline = false;
  
         clearLCD();
         lcdPosition(0,0);
         LCD.print(F("Connection Lost!"));
         lcdPosition(1,0);
         LCD.print(F("Reconnecting... "));
  
          previewSelect.setDefaultColor(0);  // Off by default
          programSelect.setDefaultColor(0);  // Off by default
          cmdSelect.setDefaultColor(0);  // Off by default
          extraButtons.setDefaultColor(0);  // Off by default

          previewSelect.setButtonColorsToDefault();
          programSelect.setButtonColorsToDefault();
          cmdSelect.setButtonColorsToDefault();
          extraButtons.setButtonColorsToDefault();
        }
       
       AtemSwitcher.connect();
    }
  
      // If the switcher has been initialized, check for button presses as reflect status of switcher in button lights:
    if (AtemSwitcher.hasInitialized())  {
      if (!AtemOnline)  {
        AtemOnline = true;
  
        clearLCD();
        lcdPosition(0,0);
        LCD.print(F("Connected"));
        lcdPosition(1,0);
        LCD.print(AtemSwitcher._ATEM_pin);
        lcdPosition(1,11);
        LCD.print(F(" v ."));
        lcdPosition(1,13);
        LCD.print(AtemSwitcher._ATEM_ver_m);
        lcdPosition(1,15);
        LCD.print(AtemSwitcher._ATEM_ver_l);
  
        previewSelect.setDefaultColor(5);  // Dimmed by default
        programSelect.setDefaultColor(5);  // Dimmed by default
        cmdSelect.setDefaultColor(5);  // Dimmed by default
        extraButtons.setDefaultColor(5);  // Dimmed by default

        previewSelect.setButtonColorsToDefault();
        programSelect.setButtonColorsToDefault();
        cmdSelect.setButtonColorsToDefault();
        extraButtons.setButtonColorsToDefault();
      }
      
      
      setButtonColors();
      AtemSwitcher.runLoop();  // Call here and there...
      
      readingButtonsAndSendingCommands();
      AtemSwitcher.runLoop();  // Call here and there...
      
      extraButtonsCommands();
      AtemSwitcher.runLoop();  // Call here and there...
      
      smartSwitches();
    }
  }
}








/**********************************
 * Navigation for the main menu:
 ***********************************/
void menuNavigation() {
  int encValue = utils.encoders_state(0,1000);
  switch(encValue)  {
     case 1:
       menu.moveDown();
     break; 
     case -1:
       menu.moveUp();
     break; 
     default:
       if (encValue>2)  {
         if (encValue<1000)  {
           menu.use();
         } else {
           menu.toRoot();
         }
       }
     break;
  } 
}

uint8_t prev_setMenuValues = 0;
void menuValues()  {
      int sVal;
      
      int encValue = utils.encoders_state(1,1000);
      int lastCount =  utils.encoders_lastCount(1);
      
      switch(setMenuValues)  {
        case 1:  // Media bank 1 selector:
        case 2:  // Media bank 2 selector:
          sVal = AtemSwitcher.getMediaPlayerStill(setMenuValues);

          if (encValue==1 || encValue==-1)  {
            sVal+=lastCount;
            if (sVal>32) sVal=1;
            if (sVal<1) sVal=32;
            AtemSwitcher.mediaPlayerSelectSource(setMenuValues, false, sVal) ;
          }
          lcdPosition(1,0);
          LCD << F("Still ");
          menuValues_printValue(sVal,6,3);
        break;
        case 3:  // AUX 1
        case 4:  // AUX 2
        case 5:  // AUX 3
          sVal = AtemSwitcher.getAuxState(setMenuValues-2);
          if (encValue==1 || encValue==-1)  {
            sVal+=lastCount;
            if (sVal>19) sVal=0;
            if (sVal<0) sVal=19;
            AtemSwitcher.changeAuxState(setMenuValues-2, sVal);
            menuValues_clearValueLine();
          }
          lcdPosition(1,0);
          menuValues_printSource(sVal);
        break;
        
        case 10:  // Transition: Type
          sVal = AtemSwitcher.getTransitionType();
          if (encValue==1 || encValue==-1)  {
            sVal+=encValue;
            if (sVal>4) sVal=0;
            if (sVal<0) sVal=4;
            AtemSwitcher.changeTransitionType(sVal);
            menuValues_clearValueLine();
          }
          lcdPosition(1,0);
          menuValues_printTrType(sVal);
        break;
        case 11:  // Transition: Time
          sVal = AtemSwitcher.getTransitionMixTime();
          if (encValue==1 || encValue==-1)  {
            sVal+=(int)encValue+(abs(lastCount)-1)*5*encValue;
            if (sVal>0xFA) sVal=1;
            if (sVal<1) sVal=0xFA;
            AtemSwitcher.changeTransitionMixTime(sVal);
            menuValues_clearValueLine();
          }
          lcdPosition(1,0);
          LCD << F("Frames: ");
          menuValues_printValue(sVal,8,3);
        break;
        case 20:  // Fade-to-black: Time
          sVal = AtemSwitcher.getFadeToBlackTime();
          if (encValue==1 || encValue==-1)  {
            sVal+=(int)encValue+(abs(lastCount)-1)*5*encValue;
            if (sVal>0xFA) sVal=1;
            if (sVal<1) sVal=0xFA;
            AtemSwitcher.changeFadeToBlackTime(sVal);
            menuValues_clearValueLine();
          }
          lcdPosition(1,0);
          LCD << F("Frames: ");
          menuValues_printValue(sVal,8,3);
        break;
        case 21:  // Fade-to-black: Execute
          lcdPosition(1,0);
          LCD << F("Press to execute");
          if (encValue>2 && encValue <1000)  {
             AtemSwitcher.fadeToBlackActivate(); 
          }
        break;
        default:
        break;        
      }
}
void menuValues_clearValueLine()  {
          lcdPosition(1,0);
          LCD.print("                ");
}
void menuValues_printValue(int number, uint8_t pos, uint8_t padding)  {
          lcdPosition(1,pos);
          LCD.print(number);
          for(int i=String(number).length(); i<padding; i++)  {
            LCD.print(" ");
          }
}
void menuValues_printSource(int sVal)  {
   switch(sVal)  {
       case 0: LCD.print("Black"); break;
       case 1: LCD.print("Camera 1"); break;
       case 2: LCD.print("Camera 2"); break;
       case 3: LCD.print("Camera 3"); break;
       case 4: LCD.print("Camera 4"); break;
       case 5: LCD.print("Camera 5"); break;
       case 6: LCD.print("Camera 6"); break;
       case 7: LCD.print("Camera 7"); break;
       case 8: LCD.print("Camera 8"); break;
       case 9: LCD.print("Color Bars"); break;
       case 10: LCD.print("Color 1"); break;
       case 11: LCD.print("Color 2"); break;
       case 12: LCD.print("Media Player 1"); break;
       case 13: LCD.print("Media Play.1 key"); break;
       case 14: LCD.print("Media Player 2"); break;
       case 15: LCD.print("Media Play.2 key"); break;
       case 16: LCD.print("Program"); break;
       case 17: LCD.print("Preview"); break;
       case 18: LCD.print("Clean Feed 1"); break;
       case 19: LCD.print("Clean Feed 2"); break;
       default: LCD.print("N/A"); break;
   } 
}
void menuValues_printTrType(int sVal)  {
   switch(sVal)  {
       case 0: LCD.print("Mix"); break;
       case 1: LCD.print("Dip"); break;
       case 2: LCD.print("Wipe"); break;
       case 3: LCD.print("DVE"); break;
       case 4: LCD.print("Sting"); break;
       default: LCD.print("N/A"); break;
   } 
}






/**********************************
 * Button State Colors set:
 ***********************************/
void setButtonColors()  {

    // Setting colors of PREVIEW input select buttons:
    for (uint8_t i=1;i<=8;i++)  {
      if (AtemSwitcher.getPreviewTally(i))  {
        previewSelect.setButtonColor(i, 3);
      } else {
        previewSelect.setButtonColor(i, 5);   
      }
    }
    
    // Setting colors of PROGRAM input select buttons:
    for (uint8_t i=1;i<=8;i++)  {
      if (BUSselect==0)  {  // Normal: PROGRAM
        if (AtemSwitcher.getProgramTally(i))  {
          programSelect.setButtonColor(i, 2);
        } else {
          programSelect.setButtonColor(i, 5);   
        }
      } else if (BUSselect<=3)  {  // 1-3: AUX bus:
        if (i==8?(AtemSwitcher.getAuxState(BUSselect)==16):(AtemSwitcher.getAuxState(BUSselect)==i))  {
          programSelect.setButtonColor(i, 4);
        } else {
          programSelect.setButtonColor(i, 5);   
        }
      } else if (BUSselect==10 || BUSselect==11)  {
        if (AtemSwitcher.getMediaPlayerStill(BUSselect-9)==i)  {
          programSelect.setButtonColor(i, 4);
        } else {
          programSelect.setButtonColor(i, 5);   
        }
      }       
    }
    
      // The user button mode tells us, how the four user buttons should be programmed. This sets the colors to match the function:
    switch(userButtonMode)  {
       case 1:
          // Setting colors of the command buttons:
          cmdSelect.setButtonColor(5, AtemSwitcher.getDownstreamKeyerStatus(1) ? 4 : 5);    // DSK1 button
          cmdSelect.setButtonColor(6, AtemSwitcher.getDownstreamKeyerStatus(2) ? 4 : 5);    // DSK2 button
          cmdSelect.setButtonColor(3, AtemSwitcher.getTransitionPosition()>0 ? 4 : 5);     // Auto button
          cmdSelect.setButtonColor(8, AtemSwitcher.getUpstreamKeyerStatus(1) ? 4 : 5);     // PIP button
      break; 
       case 2:
          // Setting colors of the command buttons:
          cmdSelect.setButtonColor(5, AtemSwitcher.getUpstreamKeyerStatus(1) ? 4 : 5);     // Key1
          cmdSelect.setButtonColor(6, AtemSwitcher.getUpstreamKeyerStatus(2) ? 4 : 5);     // Key2
          cmdSelect.setButtonColor(3, AtemSwitcher.getUpstreamKeyerStatus(3) ? 4 : 5);     // Key3
          cmdSelect.setButtonColor(8, AtemSwitcher.getUpstreamKeyerStatus(4) ? 4 : 5);     // Key4
      break; 
       case 3:
          // Setting colors of the command buttons:
          cmdSelect.setButtonColor(5, AtemSwitcher.getProgramInput()==10 ? 2 : (AtemSwitcher.getPreviewInput()==10 ? 3 : 5));     // Color1
          cmdSelect.setButtonColor(6, AtemSwitcher.getProgramInput()==11 ? 2 : (AtemSwitcher.getPreviewInput()==11 ? 3 : 5));     // Color2
          cmdSelect.setButtonColor(3, AtemSwitcher.getProgramInput()==0 ? 2 : (AtemSwitcher.getPreviewInput()==0 ? 3 : 5));     // Black
          cmdSelect.setButtonColor(8, AtemSwitcher.getProgramInput()==9 ? 2 : (AtemSwitcher.getPreviewInput()==9 ? 3 : 5));     // Bars
      break; 
       case 4:
          // Setting colors of the command buttons:
          cmdSelect.setButtonColor(5, BUSselect==1 ? 2 : 5);     // AUX1
          cmdSelect.setButtonColor(6, BUSselect==2 ? 2 : 5);     // AUX2
          cmdSelect.setButtonColor(3, BUSselect==3 ? 2 : 5);     // AUX3
          cmdSelect.setButtonColor(8, BUSselect==0 ? 2 : 5);     // Program
      break; 
      default:
          // Setting colors of the command buttons:
          cmdSelect.setButtonColor(5, AtemSwitcher.getDownstreamKeyerStatus(1) ? 4 : 5);    // DSK1 button
          cmdSelect.setButtonColor(6, preVGA_active ? 4 : 5);     // VGA+PIP button
          cmdSelect.setButtonColor(3, AtemSwitcher.getTransitionPosition()>0 ? 4 : 5);     // Auto button
          cmdSelect.setButtonColor(8, AtemSwitcher.getUpstreamKeyerStatus(1) ? 4 : 5);     // PIP button
      break;
    }

    if (!cmdSelect.buttonIsPressed(1))  {
      cmdSelect.setButtonColor(1, 5);   // de-highlight CUT button
    }
    cmdSelect.setButtonColor(2, AtemSwitcher.getTransitionPosition()>0 ? 4 : 5);     // Auto button
    cmdSelect.setButtonColor(7, AtemSwitcher.getDownstreamKeyerStatus(1) ? 4 : 5);    // DSK1 button
    cmdSelect.setButtonColor(4, AtemSwitcher.getDownstreamKeyerStatus(2) ? 4 : 5);    // DSK2 button
}









/**********************************
 * ATEM Commands
 ***********************************/
void readingButtonsAndSendingCommands() {

    // Sending commands for PREVIEW input selection:
    uint8_t busSelection = previewSelect.buttonDownAll();
    for (uint8_t i=1;i<=8;i++)  {
      if (previewSelect.isButtonIn(i, busSelection))  { AtemSwitcher.changePreviewInput(i); }  
    }

    // Sending commands for PROGRAM input selection:
    busSelection = programSelect.buttonDownAll();
    if (BUSselect==0)  {
      for (uint8_t i=1;i<=8;i++)  {
        if (programSelect.isButtonIn(i, busSelection))  { AtemSwitcher.changeProgramInput(i); }
      }
    } else if (BUSselect<=3)  {
      for (uint8_t i=1;i<=8;i++)  {
        if (programSelect.isButtonIn(i, busSelection))  { AtemSwitcher.changeAuxState(BUSselect, i==8?16:i); }  // When selecting AUX bus, input 8 is mapped to PROGRAM instead
      }
    } else if (BUSselect==10 || BUSselect==11)  {
      for (uint8_t i=1;i<=8;i++)  {
        if (programSelect.isButtonIn(i, busSelection))  { AtemSwitcher.mediaPlayerSelectSource(BUSselect-9, false, i); }
      }
    } 
  
      // "T-bar" slider:
    if (utils.uniDirectionalSlider_hasMoved())  {
      AtemSwitcher.changeTransitionPosition(utils.uniDirectionalSlider_position());
      AtemSwitcher.delay(20);
      if (utils.uniDirectionalSlider_isAtEnd())  {
	AtemSwitcher.changeTransitionPositionDone();
	AtemSwitcher.delay(5);  
      }
    }  
    
    // Cut button:
    uint8_t cmdSelection = cmdSelect.buttonDownAll();
    if (cmdSelection & (B1 << 0))  { 
      cmdSelect.setButtonColor(1, 4);    // Highlight CUT button
      AtemSwitcher.doCut(); 
      preVGA_active = false;
    }

    // Auto button:
    if (cmdSelection & (B1 << 1))  { 
      AtemSwitcher.doAuto(); 
      preVGA_active = false;
    }

    // DSK1 button:
    if (cmdSelection & (B1 << 6))  { 
      AtemSwitcher.changeDownstreamKeyOn(1, !AtemSwitcher.getDownstreamKeyerStatus(1));
    }

    // DSK2 button:
    if (cmdSelection & (B1 << 3))  { 
      AtemSwitcher.changeDownstreamKeyOn(2, !AtemSwitcher.getDownstreamKeyerStatus(2));
    }

    switch(userButtonMode)  {
       case 1:
          if (cmdSelection & (B1 << 4))  { AtemSwitcher.changeDownstreamKeyOn(1, !AtemSwitcher.getDownstreamKeyerStatus(1)); }  // DSK1
          if (cmdSelection & (B1 << 5))  { AtemSwitcher.changeDownstreamKeyOn(2, !AtemSwitcher.getDownstreamKeyerStatus(2)); }  // DSK1
          if (cmdSelection & (B1 << 2))  { AtemSwitcher.doAuto(); preVGA_active = false;}
          if (cmdSelection & (B1 << 7))  { cmd_pipToggle(); }  // PIP
       break;
       case 2:
          if (cmdSelection & (B1 << 4))  { AtemSwitcher.changeUpstreamKeyOn(1, !AtemSwitcher.getUpstreamKeyerStatus(1)); }  // Key1
          if (cmdSelection & (B1 << 5))  { AtemSwitcher.changeUpstreamKeyOn(2, !AtemSwitcher.getUpstreamKeyerStatus(2)); }  // Key2
          if (cmdSelection & (B1 << 2))  { AtemSwitcher.changeUpstreamKeyOn(3, !AtemSwitcher.getUpstreamKeyerStatus(3)); }  // Key3
          if (cmdSelection & (B1 << 7))  { AtemSwitcher.changeUpstreamKeyOn(4, !AtemSwitcher.getUpstreamKeyerStatus(4)); }  // Key4
       break;
       case 3:
          if (cmdSelection & (B1 << 4))  { AtemSwitcher.changePreviewInput(10); }  // Color1
          if (cmdSelection & (B1 << 5))  { AtemSwitcher.changePreviewInput(11); }  // Color2  
          if (cmdSelection & (B1 << 2))  { AtemSwitcher.changePreviewInput(0); }   // Black
          if (cmdSelection & (B1 << 7))  { AtemSwitcher.changePreviewInput(9); }  // Bars
       break;
       case 4:
          if (cmdSelection & (B1 << 4))  { BUSselect= BUSselect==1 ? 0 : 1;   }
          if (cmdSelection & (B1 << 5))  { BUSselect= BUSselect==2 ? 0 : 2;   }  
          if (cmdSelection & (B1 << 2))  { BUSselect= BUSselect==3 ? 0 : 3;   }   
          if (cmdSelection & (B1 << 7))  { BUSselect= 0;   }   
      break; 
       default:
          if (cmdSelection & (B1 << 4))  { AtemSwitcher.changeDownstreamKeyOn(1, !AtemSwitcher.getDownstreamKeyerStatus(1)); }  // DSK1
          if (cmdSelection & (B1 << 5))  { cmd_vgaToggle(); }    
          if (cmdSelection & (B1 << 2))  { AtemSwitcher.doAuto(); preVGA_active = false;}
          if (cmdSelection & (B1 << 7))  { cmd_pipToggle(); }  // PIP
       break;
    }
}
void cmd_vgaToggle()  {
         if (!preVGA_active)  {
          preVGA_active = true;
          preVGA_UpstreamkeyerStatus = AtemSwitcher.getUpstreamKeyerStatus(1);
          preVGA_programInput = AtemSwitcher.getProgramInput();
  
          AtemSwitcher.changeProgramInput(8);
          AtemSwitcher.changeUpstreamKeyOn(1, true); 
        } else {
          preVGA_active = false;
          AtemSwitcher.changeProgramInput(preVGA_programInput);
          AtemSwitcher.changeUpstreamKeyOn(1, preVGA_UpstreamkeyerStatus); 
        }
 
}
void cmd_pipToggle()  {
      // For Picture-in-picture, do an "auto" transition:
      unsigned long timeoutTime = millis()+5000;

        // First, store original preview input:
      uint8_t tempPreviewInput = AtemSwitcher.getPreviewInput();  
        
        // Then, set preview=program (so auto doesn't change input)
      AtemSwitcher.changePreviewInput(AtemSwitcher.getProgramInput());  
      while(AtemSwitcher.getProgramInput()!=AtemSwitcher.getPreviewInput())  {
           AtemSwitcher.runLoop();
           if (timeoutTime<millis()) {break;}
      }
        
        // Then set transition status:
      bool tempOnNextTransitionStatus = AtemSwitcher.getUpstreamKeyerOnNextTransitionStatus(1);
      AtemSwitcher.changeUpstreamKeyNextTransition(1, true);  // Set upstream key next transition
      while(!AtemSwitcher.getUpstreamKeyerOnNextTransitionStatus(1))  {  
           AtemSwitcher.runLoop();
           if (timeoutTime<millis()) {break;}
      }

        // Make Auto Transition:      
      AtemSwitcher.doAuto();
      while(AtemSwitcher.getTransitionPosition()==0)  {
           AtemSwitcher.runLoop();
           if (timeoutTime<millis()) {break;}
      }
      while(AtemSwitcher.getTransitionPosition()>0)  {
           AtemSwitcher.runLoop();
           if (timeoutTime<millis()) {break;}
      }

        // Then reset transition status:
      AtemSwitcher.changeUpstreamKeyNextTransition(1, tempOnNextTransitionStatus);
      while(tempOnNextTransitionStatus!=AtemSwitcher.getUpstreamKeyerOnNextTransitionStatus(1))  {  
           AtemSwitcher.runLoop();
           if (timeoutTime<millis()) {break;}
      }
        // Reset preview bus:
      AtemSwitcher.changePreviewInput(tempPreviewInput);  
      while(tempPreviewInput!=AtemSwitcher.getPreviewInput())  {
           AtemSwitcher.runLoop();
           if (timeoutTime<millis()) {break;}
      }
        // Finally, tell us how we did:
     if (timeoutTime<millis()) {
       Serial.println("Timed out during operation!");
     } else {
      Serial.println("DONE!");
     }  
}



uint8_t prevBUSselect = 255;
uint8_t pipMode = 0;
uint8_t prevPipMode = 0;
void smartSwitches()  {
  word buttons = SmartSwitch.buttonUpAll();
  
    // SmartSwitch 1 action:
  uint8_t j = 0;
  if(buttons & (1 << j)) {
    BUSselect++;
    if (BUSselect>3)  {
      BUSselect=0;
    }
  }
    // SmartSwitch 1 display content:
  if (prevBUSselect!=BUSselect)  {
     prevBUSselect =  BUSselect;
     switch(BUSselect)  {
       case 1:
        SmartSwitch.clearPixmap(1<<j);
        SmartSwitch.drawImage(BUTTON1, 0, 0, IMAGE_LEFT, SSS_AUXgfx);        
        SmartSwitch.writeTextXY(BUTTON1, "1", 32-10, 24, TEXT_BACKGROUND | TEXT_REVERSE);
        SmartSwitch.writeTextXY(BUTTON1, "2", 32, 24, TEXT_BACKGROUND);
        SmartSwitch.writeTextXY(BUTTON1, "3", 32+10, 24, TEXT_BACKGROUND);
        SmartSwitch.setButtonColor(2,0,0, 1<<j);
        SmartSwitch.updateScreen(1<<j);
       break;
       case 2:
        SmartSwitch.clearPixmap(1<<j);
        SmartSwitch.drawImage(BUTTON1, 0, 0, IMAGE_LEFT, SSS_AUXgfx);        
        SmartSwitch.writeTextXY(BUTTON1, "1", 32-10, 24, TEXT_BACKGROUND);
        SmartSwitch.writeTextXY(BUTTON1, "2", 32, 24, TEXT_BACKGROUND | TEXT_REVERSE);
        SmartSwitch.writeTextXY(BUTTON1, "3", 32+10, 24, TEXT_BACKGROUND);
        SmartSwitch.setButtonColor(2,0,0, 1<<j);
        SmartSwitch.updateScreen(1<<j);
       break;
       case 3:
        SmartSwitch.clearPixmap(1<<j);
        SmartSwitch.drawImage(BUTTON1, 0, 0, IMAGE_LEFT, SSS_AUXgfx);        
        SmartSwitch.writeTextXY(BUTTON1, "1", 32-10, 24, TEXT_BACKGROUND);
        SmartSwitch.writeTextXY(BUTTON1, "2", 32, 24, TEXT_BACKGROUND);
        SmartSwitch.writeTextXY(BUTTON1, "3", 32+10, 24, TEXT_BACKGROUND | TEXT_REVERSE);
        SmartSwitch.setButtonColor(2,0,0, 1<<j);
        SmartSwitch.updateScreen(1<<j);
       break;
       case 10:
        SmartSwitch.clearPixmap(1<<j);
        SmartSwitch.writeText(BUTTON1, "MEDIA 1", 3, TEXT_CENTER | TEXT_BACKGROUND | TEXT_REVERSE);
        SmartSwitch.setButtonColor(2,2,0, 1<<j);
        SmartSwitch.updateScreen(1<<j);
       break;
       case 11:
        SmartSwitch.clearPixmap(1<<j);
        SmartSwitch.writeText(BUTTON1, "MEDIA 2", 3, TEXT_CENTER | TEXT_BACKGROUND | TEXT_REVERSE);
        SmartSwitch.setButtonColor(2,2,0, 1<<j);
        SmartSwitch.updateScreen(1<<j);
       break;
       case 0:
        SmartSwitch.clearPixmap(1<<j);
        SmartSwitch.drawImage(BUTTON1, 0, -3, IMAGE_LEFT, SSS_PGMgfx);        
        SmartSwitch.writeText(BUTTON1, "PROGRAM", 3, TEXT_CENTER | TEXT_BACKGROUND | TEXT_REVERSE);
        SmartSwitch.setButtonColor(2,2,2, 1<<j);
        SmartSwitch.updateScreen(1<<j);
       break;
       default:
        SmartSwitch.clearPixmap(1<<j);
        SmartSwitch.writeText(BUTTON1, "N/A", 3, TEXT_CENTER | TEXT_BACKGROUND | TEXT_REVERSE);
        SmartSwitch.setButtonColor(0,0,0, 1<<j);
        SmartSwitch.updateScreen(1<<j);
       break;
     }
  }

    // SmartSwitch 1 action:
  j = 1;
  if(buttons & (1 << j)) {
    pipMode++;
    if (pipMode>2)  {
      pipMode=1;
    }
     switch(pipMode)  {
       case 1:
        AtemSwitcher.changeDVESettingsTemp(-8500,-4500,300,300);
       break;
       case 2:
        AtemSwitcher.changeDVESettingsTemp(8500,-4500,300,300);
       break;
     }
  }
    // SmartSwitch 1 display content:
  if (prevPipMode!=pipMode)  {
     prevPipMode =  pipMode;
     switch(pipMode)  {
       case 1:
        SmartSwitch.clearPixmap(1<<j);
        SmartSwitch.drawImage(BUTTON2, 0, 0, IMAGE_LEFT, SSS_PIPLgfx);        
        SmartSwitch.setButtonColor(1,1,2, 1<<j);
        SmartSwitch.updateScreen(1<<j);
       break;
       case 2:
        SmartSwitch.clearPixmap(1<<j);
        SmartSwitch.drawImage(BUTTON2, 0, 0, IMAGE_LEFT, SSS_PIPRgfx);        
        SmartSwitch.setButtonColor(1,1,2, 1<<j);
        SmartSwitch.updateScreen(1<<j);
       break;
       default:
        SmartSwitch.clearPixmap(1<<j);
        SmartSwitch.writeText(BUTTON2, "N/A", 3, TEXT_CENTER | TEXT_BACKGROUND | TEXT_REVERSE);
        SmartSwitch.updateScreen(1<<j);
        SmartSwitch.setButtonColor(0,0,0, 1<<j);
       break;
     }
  }
}


