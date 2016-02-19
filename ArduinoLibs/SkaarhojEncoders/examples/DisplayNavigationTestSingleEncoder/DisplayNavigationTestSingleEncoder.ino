// Including libraries: 
#include <MenuBackend.h>  // Library for menu navigation. Must (for some reason) be included early! Otherwise compilation chokes.


#include "Wire.h"
#include "MCP23017.h"
#include "Streaming.h"

#include "SkaarhojEncoders.h"
SkaarhojEncoders encoders;

#include "SkaarhojEADOGMDisplay.h"
SkaarhojEADOGMDisplay Disp163;










/*************************************************************
 *
 *
 *                     MAIN PROGRAM CODE AHEAD
 *
 *
 **********************************************************/

void setup() { 
  Serial.begin(115200);
  Serial << F("\n- - - - - - - -\nSerial Started\n");


  Wire.begin();
  Disp163.begin(6,0,3);  // DOGM163

    Disp163.cursor(true);
  Disp163 << F("millis(): ") << millis();
  Disp163.print("AbCDE\nY");
  delay(3000);

  Disp163.cursor(false);
  Disp163.clearDisplay();


  Wire.begin();
  encoders.begin(6);
  encoders.serialOutput(0);


  menuSetup();
  displayHomeScreen();
}






/*************************************************************
 * Loop function (runtime loop)
 *************************************************************/

// The loop function:
void loop() {
  encoders.runLoop();
  menuNavigation();
}























/*************************************************************
 *
 *
 *                     MENU SYSTEM
 *
 *
 **********************************************************/

uint8_t menuValueSetID = 0;  // The value of this variable corresponds to a menu item for which we can set a value on the second display line.
bool menuSetValueMode = false;  // If set, we are changing a value with the encoder rather than navigating the menu

// Configuration of the menu items and hierarchi plus call-back functions:
MenuBackend menu = MenuBackend(menuUseEvent,menuChangeEvent);
// Beneath is list of menu items needed to build the menu
// First argument: the "menu" object (created above), second argument: The text string, third argument: Menu level
MenuItem menu_L1_EXIT1     = MenuItem(menu, "< Exit", 1);
MenuItem menu_L1_presets = MenuItem(menu, "Presets", 1);
MenuItem menu_L2_Recall = MenuItem(menu, "Recall", 2);
MenuItem menu_L2_Store = MenuItem(menu, "Store", 2);
MenuItem menu_L2_Delete = MenuItem(menu, "Delete", 2);
MenuItem menu_L2_BACK1 = MenuItem(menu, "< Back", 2);

MenuItem menu_L1_monitors   = MenuItem(menu, "Monitors", 1);
MenuItem menu_L2_rightScreen = MenuItem(menu, "Right Screen", 2);
MenuItem menu_L2_leftScreen = MenuItem(menu, "Left Screen", 2);
MenuItem menu_L2_scope1 = MenuItem(menu, "Scope 1", 2);
MenuItem menu_L2_scope2 = MenuItem(menu, "Scope 2", 2);
MenuItem menu_L2_scope3 = MenuItem(menu, "Scope 3", 2);
MenuItem menu_L2_scope4 = MenuItem(menu, "Scope 4", 2);
MenuItem menu_L3_brightness = MenuItem(menu, "Brightness", 3);
MenuItem menu_L3_contrast = MenuItem(menu, "Contrast", 3);
MenuItem menu_L3_saturation = MenuItem(menu, "Saturation", 3);
MenuItem menu_L3_type = MenuItem(menu, "Type", 3);
MenuItem menu_L3_BACK1 = MenuItem(menu, "< Back", 3);
MenuItem menu_L2_reset = MenuItem(menu, "Reset", 2);
MenuItem menu_L2_BACK2 = MenuItem(menu, "< Back", 2);

MenuItem menu_L1_dateTime     = MenuItem(menu, "Date and Time", 1);
MenuItem menu_L2_date  = MenuItem(menu, "Date", 2);
MenuItem menu_L2_time  = MenuItem(menu, "Time", 2);
MenuItem menu_L2_BACK3 = MenuItem(menu, "< Back", 2);

MenuItem menu_L1_framing     = MenuItem(menu, "Framing", 1);

MenuItem menu_L1_service     = MenuItem(menu, "Service", 1);

MenuItem menu_L1_EXIT2       = MenuItem(menu, "< Exit", 1);



// This function builds the menu and connects the correct items together
void menuSetup()
{
  Serial << F("Setting up menu...");

  // Add first item to the menu root:
  menu.getRoot().add(menu_L1_presets); 

  // Setup the rest of menu items on level 1:
  menu_L1_presets.addBefore(menu_L1_EXIT1);
  menu_L1_presets.addAfter(menu_L1_monitors);
  menu_L1_monitors.addAfter(menu_L1_dateTime);
  menu_L1_dateTime.addAfter(menu_L1_framing);
  menu_L1_framing.addAfter(menu_L1_service);
  menu_L1_service.addAfter(menu_L1_EXIT2);

  // Set up level 2:
  menu_L2_Recall.addAfter(menu_L2_Store);  // Chain subitems...
  menu_L2_Store.addAfter(menu_L2_Delete);
  menu_L2_Delete.addAfter(menu_L2_BACK1);
  menu_L2_BACK1.addLeft(menu_L1_presets);
  menu_L1_presets.addRight(menu_L2_Recall);     // Add the submenu to the parent
}

void menuOperationReset()  {
  menuValueSetID = 0;
  menuSetValueMode = false;
}

/*
  Here all use events are handled. Mainly these are used to navigate in to and out of menu items with the encoder button.
 */
void menuUseEvent(MenuUseEvent used)
{
  // Any change to the menu will reset these variables related to setting values in the menu:

  if (used.item.getName()=="MenuRoot")  {
    menu.moveDown(); 
    menuOperationReset();
    return;
  }

  // Exit in upper level:
  if (used.item.isEqual(menu_L1_EXIT1) || used.item.isEqual(menu_L1_EXIT2))  {
    menu.toRoot(); 
    menuOperationReset();
    return;
  }

  // Go back:
  if (used.item.isEqual(menu_L2_BACK1))  {
    menu.moveLeft(); 
    menuOperationReset();
    return;
  }

  // Move right, if there are sub-items, otherwise begin to set values:
  if((bool)used.item.getRight())  {
    menu.moveRight();
  } 
  else {
    menuSetValueMode = true;
    menuDisplayValue();
  }
}

/*
  Here we get a notification whenever the user changes the menu
 That is, when the menu is navigated
 */
void menuChangeEvent(MenuChangeEvent changed) {
  menuOperationReset();

  if (changed.to.getName()=="MenuRoot")  {
    displayHomeScreen();
  } 
  else {
    // Show the item name in upper line:
    Serial  << changed.to.getName() << F("\n");

    // Shows main menu item:
    Disp163.gotoRowCol(0,0);
    Disp163 << changed.to.getName();
    for(int i=strlen(changed.to.getName()); i<16; i++)  {
      Disp163 << F(" ");
    }

    displayClearValueLine();

    // If there are no menu items to the right, we assume its a value change:
    if (!(bool)changed.to.getRight())  {
      if (changed.to.getName() == menu_L1_monitors.getName())  { 
        menuValueSetID = 1;
      }
      if (changed.to.getName() == menu_L1_framing.getName())  { 
        menuValueSetID = 2;
      }
      menuDisplayValue();
    } 
    else {  // Just clear the displays second line if there are items to the right in the menu:
      Disp163.gotoRowCol(0,15);
      Disp163 << F(">");  // Arrow + Clear the line...
    }
  }
}


/**********************************
 * Navigation for the main menu:
 ***********************************/
uint8_t testValue = 88;

void menuNavigation() {

  // Read encoder state:
  int encValue = encoders.state(1,1000);
  int lastCount =  encoders.lastCount(1);
  if (encValue)  {
    Serial << F("Encoder 1: ") << encValue << " /  Count: " << lastCount << "\n";   // The longer this serial print, the more likely is a crash. I believe it's because I'm also serial-printing from within the interrupt functions and it can happen that the serial.print function is executed both places at the same time!
  }

  // NAVIGATION:
  if (!menuSetValueMode)  {

    // Decide the action to take (navigation wise):
    switch(encValue)  {
    case -1:
      Serial << F("nav up\n");
      menu.moveUp();
      break; 
    case 1:
      Serial << F("nav down\n");
      menu.moveDown();
      break; 
    default:
      if (encValue>2)  {  // Button push
        if (encValue<1000)  {
          menu.use();
        } 
        else {  // Push and holding...
          menu.toRoot();
        }
      }
      break;
    } 
  } 
  else {
    // VALUE CHANGE:
    switch(encValue)  {
    case 1:
    case -1:
      switch(menuValueSetID)  {
      case 1:
      case 2:
        testValue+=lastCount+1;
        Serial << F("value change\n") << testValue;
        menuDisplayValue();
        break;
      default:
        menuDisplayValue();
        break;
      }
      break; 
    default:
      if (encValue>2)  {  // Button push
        menuSetValueMode = false;
        menuDisplayValue();
        if (encValue>=1000)  {
          menu.toRoot();
        }
      }
      break;
    } 
  }
}

void menuDisplayValue()  {
  switch(menuValueSetID)  {
  case 1:
  case 2:
    Disp163.gotoRowCol(1,6);
    Disp163 << _DECPADL(testValue, 10,"_");
    break;
  default:
    Disp163.gotoRowCol(1,5);
    Disp163 << F("(No values)");
    break;
  }

  Disp163.gotoRowCol(1,0);
  Disp163 << (menuSetValueMode ? F(">") : F(" "));
}














void displayHomeScreen()  {
  // Show default text.... status whatever....
  Disp163.clearDisplay();
  Disp163.gotoRowCol(0,0);
  Disp163 << F("  HELLO WORLD!  ");
  Disp163.gotoRowCol(1,0);
  Disp163 << F("(Back to normal)");    
}

void displayClearValueLine() {
  Disp163.gotoRowCol(1,0);
  Disp163 << F("                ");  // Arrow + Clear the line...
}






