/*****************
 * This example is an integrated functional test of the components that make up a SKAARHOJ C10x, C20x, C3xx series device.
 * It does NOT connect to any peripherals (such as an ATEM switcher)
 * It allows you to exercise multiple interconnected components. These include:
 * - Writing to the serial display (2x16 chars)
 * - Navigate an example menu structure with the encoders (no actions though).
 * - Shows status of the config switch (Used on some SKAARHOJ models).
 * - Reacts to button presses (colors are cycled, button numbers revealed in display)
 * - Shows slider position on display
 * - Tests 2 smartswitches
 * 
 * The menu structure is a real world example taken from a C100 sketch. 
 * The data it displays is dummy data from internal memory.
 *
 * - kasper
 *
 * This example code is under GNU GPL license
 */






/****************************
 * Including libraries: 
 ***************************/
// Library for menu navigation. Must (for some reason) be included early! Otherwise compilation chokes.
#include "MenuBackend.h"  

// All related to library "SkaarhojBI8":
#include "Wire.h"
#include "MCP23017.h"
#include "PCA9685.h"
#include "SkaarhojBI8.h"

// Libraries for slider/T-bar and encoder:
#include "SkaarhojUtils.h"
SkaarhojUtils utils;

// Library for SmartSwitches:
#include "SkaarhojSmartSwitch.h"
SkaarhojSmartSwitch SmartSwitch;


// Animated logo
static uint8_t movielogo[] PROGMEM = {
  32, // width
  20, // height

  /* page 0 (lines 0-7) */
  0xfc,0xfe,0xf3,0xf3,0xff,0xff,0xf3,0xf3,0xff,0xff,0xf3,0xf3,0xff,0xff,0xf7,0xf3,
  0xff,0xff,0xf7,0xf3,0xf7,0xff,0xff,0xf3,0xf3,0xfe,0xfc,0x80,0x80,0xc0,0xc0,0xc0,
  
  /* page 1 (lines 8-15) */
  0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
  0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x1f,0x1f,0x3f,0x3f,0x7f,
  
  /* page 2 (lines 16-23) */
  0x3,0x7,0xc,0xc,0xf,0xf,0xc,0xc,0xf,0xf,0xc,0xc,0xf,0xf,0xe,0xc,
  0xf,0xf,0xe,0xc,0xe,0xf,0xf,0xc,0xe,0x7,0x3,0x0,0x0,0x0,0x0,0x0,
  
};

//#include <MemoryFree.h>

// Setting up four boards, addresses 0-3 (onboard dip switch):
SkaarhojBI8 boardArray[] = {
  SkaarhojBI8(), SkaarhojBI8(), SkaarhojBI8(), SkaarhojBI8()};
bool boardArrayEnabled[4];


/* MEMORY USAGE:
 * This can be used to track free memory. 
 * Include "MemoryFree.h" and use the following line
 *     Serial << F("freeMemory()=") << freeMemory() << "\n";  
 * in your code to see available memory.
 */
//#include <MemoryFree.h>





#include <Streaming.h>





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
  delay(LCDdelay); 
}
void clearLCD(){
  LCD.write(0xFE);   //command flag
  LCD.write(0x01);   //clear command.
  delay(LCDdelay);
}
void backlightOn() {  //turns on the backlight
  LCD.write(0x7C);   //command flag for backlight stuff
  LCD.write(157);    //light level.
  delay(LCDdelay);
}
void backlightOff(){  //turns off the backlight
  LCD.write(0x7C);   //command flag for backlight stuff
  LCD.write(128);     //light level for off.
  delay(LCDdelay);
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
uint8_t setMenuValues = 0;  // The value of this variable determines what the function "menuValues()" prints to the displays second line.

// Configuration of the menu items and hierarchi plus call-back functions:
MenuBackend menu = MenuBackend(menuUseEvent,menuChangeEvent);
// Beneath is list of menu items needed to build the menu
// First argument: the "menu" object (created above), second argument: The text string, third argument: Menu level
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
MenuItem menu_UP2       = MenuItem(menu, "< Exit", 1);

// This function builds the menu and connects the correct items together
void menuSetup()
{
  Serial << F("Setting up menu...");

  // Add first item to the menu root:
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

  // Set up user button menu (level 2):
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
  } 
  else {
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
    LCD.print(F("  HELLO WORLD!  "));    
    lcdPosition(1,0);
    LCD.print(F("(Back to normal)"));    
    setMenuValues=0;
  } 
  else {
    // Show the item name in upper line:
    lcdPosition(0,0);
    LCD.print(changed.to.getName());
    for(int i=strlen(changed.to.getName()); i<16; i++)  {
      LCD.print(" ");
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
        LCD.print("                ");  // Clear the line...
      }

      // Make settings as a consequence of menu selection:
      if (changed.to.getName() == menu_usrcfg1.getName())  { 
        userButtonMode=0; 
      }
      if (changed.to.getName() == menu_usrcfg2.getName())  { 
        userButtonMode=1; 
      }
      if (changed.to.getName() == menu_usrcfg3.getName())  { 
        userButtonMode=2; 
      }
      if (changed.to.getName() == menu_usrcfg4.getName())  { 
        userButtonMode=3; 
      }
      if (changed.to.getName() == menu_usrcfg5.getName())  { 
        userButtonMode=4; 
      }
      if (changed.to.getName() == menu_mediab1.getName())  { 
        setMenuValues=1;  
      }
      if (changed.to.getName() == menu_mediab2.getName())  { 
        setMenuValues=2;  
      }
      if (changed.to.getName() == menu_aux1.getName())  { 
        setMenuValues=3;  
      }
      if (changed.to.getName() == menu_aux2.getName())  { 
        setMenuValues=4;  
      }
      if (changed.to.getName() == menu_aux3.getName())  { 
        setMenuValues=5;  
      }
      if (changed.to.getName() == menu_trtype.getName())  { 
        setMenuValues=10;  
      }
      if (changed.to.getName() == menu_trtime.getName())  { 
        setMenuValues=11;  
      }
      if (changed.to.getName() == menu_ftbtime.getName())  { 
        setMenuValues=20;  
      }
      if (changed.to.getName() == menu_ftbexec.getName())  { 
        setMenuValues=21;  
      }
      // TODO: I HAVE to find another way to match the items here because two items with the same name will choke!!


    } 
    else {  // Just clear the displays second line if there are items to the right in the menu:
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
//  Serial << F("E0\n");   // Watch out!: Printing to Serial from within an interrupt can crash the arduino! If that happens, try to remove this line.
}
void _enc1active()  {
  utils.encoders_interrupt(1);
//  Serial << F("E1\n");   // Watch out!: Printing to Serial from within an interrupt can crash the arduino! If that happens, try to remove this line.
}









































































/*************************************************************
 *
 *
 *                     MAIN PROGRAM CODE AHEAD
 *
 *
 **********************************************************/

bool isConfigMode;

void setup() { 
  Serial.begin(115200);
  Serial << F("\n- - - - - - - -\nSerial Started\n");



  // *********************************
  // Mode of Operation (Normal / Configuration)
  // *********************************
  pinMode(A1,INPUT_PULLUP);
  delay(100);
  isConfigMode = (analogRead(A1) > 100) ? true : false;



  // ********************
  // Start up LCD (has to be after Ethernet has been started!)
  // ********************
  pinMode(txPin, OUTPUT);
  LCD.begin(9600);
  delay(500);
  clearLCD();
  lcdPosition(0,0);
  LCD.print(F("Config Switch:  "));
  LCD.print(isConfigMode ? "On" : "Off");
  backlightOn();


  // Always initialize Wire before setting up the SkaarhojBI8 class!
  Wire.begin();

  // Set up each board, test if it exists (looking for MCP23017 chip).
  for(uint8_t i=0;i<=3;i++)  {  
    boardArray[i].debugMode();
    boardArrayEnabled[i] = boardArray[i].begin(i);
    Serial << F("Board ") << i << " is " << (boardArrayEnabled[i]?"present":"NOT present") << "\n";
    boardArray[i].testSequence(10);  // Runs LED test sequence

    boardArray[i].setDefaultColor(1);
  }



  clearLCD();
  lcdPosition(0,0);
  LCD.print(F("  HELLO WORLD!  "));    
  lcdPosition(1,0);
  LCD.print(F("(Disp+Nav..test)"));    

  // Switches on address 4 + SDI on pins 48+49:  
  SmartSwitch.begin(4,48,49);
  SmartSwitch.setButtonBrightness(7, BUTTON_ALL);
  SmartSwitch.setButtonColor(3,3,3,BUTTON_ALL);
  SmartSwitch.clearScreen(BUTTON_ALL);
  delay(500);
  SmartSwitch.drawImage(BUTTON1, 0, 0, IMAGE_CENTER, movielogo);  // This line crashes the Arduino because of "-2". If "0" in the Y-coordinate, it doesn't. Probably there is a memory problem here!
  SmartSwitch.updateScreen(BUTTON1);  // Writes the buffered pixmap to the button.
  delay(1000);
  SmartSwitch.writeText(BUTTON1, "TESTING", 3, TEXT_CENTER | TEXT_BACKGROUND | TEXT_REVERSE);
  SmartSwitch.updateScreen(BUTTON1);  // Writes the buffered pixmap to the button.

  // *********************************
  // Final Setup
  // *********************************
  utils.debugMode();
  
  // Initializing menu control:
  utils.encoders_init();
  attachInterrupt(0, _enc1active, RISING);
  attachInterrupt(1, _enc0active, RISING);

  menuSetup();

  // Initializing the slider:
  utils.uniDirectionalSlider_init(10, 35, 35, A0);  // These 4 arguments are the default arguments given, if you call the function without any. See the library SkaarhojUtils for more info on what they are for.
  utils.uniDirectionalSlider_hasMoved();
}






/*************************************************************
 * Loop function (runtime loop)
 *************************************************************/

// The loop function:
unsigned long lastActionTime = 0;
void loop() {
  menuNavigation();
  menuValues();
  checkConfigMode();
  checkSlider();
  checkButtons();
  smartSwitchButtons();

  // Run the LED test sequence for each board:
  if ((unsigned long)(lastActionTime+60000) < (unsigned long)millis())  {
    uint8_t bDown = 0;
    clearLCD();
    lcdPosition(0,0);
    LCD.print(F("Cycling btn cols"));    
    lcdPosition(1,0);
    LCD.print(F("Press to stop"));    
    
    delay(2000);
    for(uint8_t i=0;i<=3;i++)  {  
      if (boardArrayEnabled[i]) {
        bDown = boardArray[i].testSequence(50);
      }
      if (bDown)  {
        clearLCD();
        setLastActionTime();
        break;
      }
    }
  } else if ((unsigned long)(lastActionTime+5000) < (unsigned long)millis())  {
    long hours;
    long mins;
    long secs;
    char buffer[18];
    secs = (unsigned long)(millis())/1000;
    mins = secs/60;
    hours = mins/60;
    secs = secs-(mins*60);
    mins = mins-(hours*60);
    sprintf(buffer, "Upt: %3d:%02d:%02d   ", (int)hours, (int)mins, (int)secs);
    lcdPosition(1,0);
    LCD << buffer;
  }

  // Shows free memory:  
  // Serial << F("freeMemory()=") << freeMemory() << "\n";  
}
void setLastActionTime()  {
  lastActionTime = millis();
}






/**********************************
 * Config switch checked here:
 ***********************************/
void checkConfigMode() {
  if (((analogRead(A1) > 100) ? true : false) != isConfigMode)  {
    isConfigMode=!isConfigMode;
    lcdPosition(1,0);
    LCD.print(F("Cfg.Switch:  "));
    LCD.print(isConfigMode ? "On " : "Off");
    setLastActionTime();
  }
}






/**********************************
 * Slider handled here:
 ***********************************/
void checkSlider()  {
  // Slidertest:
  if (utils.uniDirectionalSlider_hasMoved())  {
    setLastActionTime();
    lcdPosition(1,0);
    LCD << F("Slider: ");
    lcdPrintValue(utils.uniDirectionalSlider_position(),4);
    LCD << "    ";
    if (utils.uniDirectionalSlider_isAtEnd())  {
      lcdPosition(1,0);
      LCD << F("Slider is at end");
    }
  }
}







/**********************************
 * Button presses handled here:
 ***********************************/
uint8_t buttonColorTracking[] = {
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
void checkButtons()  {
  // Run the LED test sequence for each board:
  for(uint8_t i=0;i<=3;i++)  {  
    if (boardArrayEnabled[i]) {
      for(int j=1;j<=8;j++)  {
        if (boardArray[i].buttonDown(j))  {
          lcdPosition(1,0);
          LCD << F("BI8#") << i << F(" btn#") << j << " Down";
          setLastActionTime();

          boardArray[i].setButtonColor(j, nextButtonColor(i*8+j));
        }  
        if (boardArray[i].buttonUp(j))  {
          lcdPosition(1,0);
          LCD << F("BI8#") << i << F(" btn#") << j << " Up  ";
          setLastActionTime();
        }  
      }  

    }
  }
}

uint8_t nextButtonColor(int i) {
  buttonColorTracking[i-1]++;
  if (buttonColorTracking[i-1]>5)  {
    buttonColorTracking[i-1]=0;
  }
  return buttonColorTracking[i-1];
}





/**********************************
 * Navigation for the main menu:
 ***********************************/
void menuNavigation() {
  // Read upper encoder value
  int encValue = utils.encoders_state(0,1000);
  if (encValue)  {
     setLastActionTime();
     Serial << F("Encoder 0: ") << encValue << "\n"; 
  }  
  // Decide the action to take (navigation wise):
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
      } 
      else {
        menu.toRoot();
      }
    }
    break;
  } 
}
// These variables hold the dummy values we set with the menu items:
uint8_t mediaPlayerStill[] = {
  7,23};
uint8_t auxState[] = {
  3,6,9};
uint8_t transitionType = 0;
uint8_t transitionMixTime = 25;
uint8_t fadeToBlackTime = 234;
void menuValues()  {
  int sVal;
  
  // Read lower encoder value
  int encValue = utils.encoders_state(1,1000);
  int lastCount =  utils.encoders_lastCount(1);
  if (encValue)  {
     setLastActionTime();
     Serial << F("Encoder 1: ") << encValue << " /  Count: " << lastCount << "\n";   // The longer this serial print, the more likely is a crash. I believe it's because I'm also serial-printing from within the interrupt functions and it can happen that the serial.print function is executed both places at the same time!
   }
  
  switch(setMenuValues)  {
  case 1:  // Media bank 1 selector:
  case 2:  // Media bank 2 selector:
    sVal = mediaPlayerStill[setMenuValues-1];

    if (encValue==1 || encValue==-1)  {
      sVal+=lastCount;
      if (sVal>32) sVal=1;
      if (sVal<1) sVal=32;
      mediaPlayerStill[setMenuValues-1] = sVal;
    }
    lcdPosition(1,0);
    LCD << F("Still ");
    menuValues_printValue(sVal,6,3);
    break;
  case 3:  // AUX 1
  case 4:  // AUX 2
  case 5:  // AUX 3
    sVal = auxState[setMenuValues-3];
    if (encValue==1 || encValue==-1)  {
      sVal+=lastCount;
      if (sVal>19) sVal=0;
      if (sVal<0) sVal=19;
      auxState[setMenuValues-3] = sVal;
      menuValues_clearValueLine();
    }
    lcdPosition(1,0);
    menuValues_printValueName(sVal);
    break;

  case 10:  // Transition: Type
    sVal = transitionType;
    if (encValue==1 || encValue==-1)  {
      sVal+=encValue;
      if (sVal>4) sVal=0;
      if (sVal<0) sVal=4;
      transitionType = sVal;
      menuValues_clearValueLine();
    }
    lcdPosition(1,0);
    menuValues_printTrType(sVal);
    break;
  case 11:  // Transition: Time
    sVal = transitionMixTime;
    if (encValue==1 || encValue==-1)  {
      sVal+=(int)encValue+(abs(lastCount)-1)*5*encValue;
      if (sVal>0xFA) sVal=1;
      if (sVal<1) sVal=0xFA;
      transitionMixTime = sVal;
      menuValues_clearValueLine();
    }
    lcdPosition(1,0);
    LCD << F("Frames: ");
    menuValues_printValue(sVal,8,3);
    break;
  case 20:  // Fade-to-black: Time
    sVal = fadeToBlackTime;
    if (encValue==1 || encValue==-1)  {
      sVal+=(int)encValue+(abs(lastCount)-1)*5*encValue;
      if (sVal>0xFA) sVal=1;
      if (sVal<1) sVal=0xFA;
      fadeToBlackTime = sVal;
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
      Serial << F("Sending FTB command... \n");
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
void menuValues_printValueName(int sVal)  {
  switch(sVal)  {
  case 0: 
    LCD.print("Black"); 
    break;
  case 1: 
    LCD.print("Camera 1"); 
    break;
  case 2: 
    LCD.print("Camera 2"); 
    break;
  case 3: 
    LCD.print("Camera 3"); 
    break;
  case 4: 
    LCD.print("Camera 4"); 
    break;
  case 5: 
    LCD.print("Camera 5"); 
    break;
  case 6: 
    LCD.print("Camera 6"); 
    break;
  case 7: 
    LCD.print("Camera 7"); 
    break;
  case 8: 
    LCD.print("Camera 8"); 
    break;
  case 9: 
    LCD.print("Color Bars"); 
    break;
  case 10: 
    LCD.print("Color 1"); 
    break;
  case 11: 
    LCD.print("Color 2"); 
    break;
  case 12: 
    LCD.print("Media Player 1"); 
    break;
  case 13: 
    LCD.print("Media Play.1 key"); 
    break;
  case 14: 
    LCD.print("Media Player 2"); 
    break;
  case 15: 
    LCD.print("Media Play.2 key"); 
    break;
  case 16: 
    LCD.print("Program"); 
    break;
  case 17: 
    LCD.print("Preview"); 
    break;
  case 18: 
    LCD.print("Clean Feed 1"); 
    break;
  case 19: 
    LCD.print("Clean Feed 2"); 
    break;
  default: 
    LCD.print("N/A"); 
    break;
  } 
}
void menuValues_printTrType(int sVal)  {
  switch(sVal)  {
  case 0: 
    LCD.print("Mix"); 
    break;
  case 1: 
    LCD.print("Dip"); 
    break;
  case 2: 
    LCD.print("Wipe"); 
    break;
  case 3: 
    LCD.print("DVE"); 
    break;
  case 4: 
    LCD.print("Sting"); 
    break;
  default: 
    LCD.print("N/A"); 
    break;
  } 
}


/**********************************
 * SmartSwitch buttons:
 ***********************************/
int state, modes;
void smartSwitchButtons() {
  word buttons = SmartSwitch.buttonUpAll();
  for(int j=0; j<4; j++) {
    if(buttons & (1 << j)) {
      modes = SmartSwitch.getButtonModes();
      setLastActionTime();
      Serial.print("Button ");
      Serial.print(j, DEC);
      Serial.println(" was pressed!");
      state = modes >> j*2 & B11;
      Serial.print("State: ");
      Serial.println(state, BIN);
      if(state == 0) {
        SmartSwitch.clearPixmap(1<<j);
        SmartSwitch.drawLine(0,0,63,31, 1<<j);
        SmartSwitch.drawLine(0,31,63,0, 1<<j);
        SmartSwitch.drawHorisontalLine(0, 1<<j);
        SmartSwitch.drawHorisontalLine(31, 1<<j);
        SmartSwitch.drawVerticalLine(0, 1<<j);
        SmartSwitch.drawVerticalLine(63, 1<<j);
        SmartSwitch.updateScreen(1<<j);
        SmartSwitch.setButtonModes(modes ^ 1 << 2*j);
        SmartSwitch.setButtonColor(3,0,0, 1<<j);
      } else if (state == 1) {
        SmartSwitch.clearPixmap(1<<j);
        SmartSwitch.drawCircle(31, 16, 21, 1<<j);
        SmartSwitch.updateScreen(1<<j);
        SmartSwitch.setButtonModes(modes ^ (state | 2) << 2*j);
        SmartSwitch.setButtonColor(0,3,0, 1<<j);
      } else {
        SmartSwitch.clearPixmap(1<<j);
        SmartSwitch.updateScreen(1<<j);
        SmartSwitch.setButtonModes(modes ^ state << 2*j);
        SmartSwitch.setButtonColor(0,0,3, 1<<j);
      }
      Serial.print("ButtonState: ");
      Serial.println(SmartSwitch.getButtonModes(), BIN);
    }
  }
}



