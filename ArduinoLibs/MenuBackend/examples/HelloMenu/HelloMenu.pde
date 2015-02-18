/**
 * HelloMenu
 * by BREVIG http://alexanderbrevig.com
 * 
 * This is the structure of the modelled menu
 * 
 * Settings
 *   Pin
 *   Debug
 * Options
 *   Delay (D)
 *     100 ms
 *     200 ms
 *     300 ms
 *     400 ms
 */

#include <MenuBackend.h>

//this controls the menu backend and the event generation
MenuBackend menu = MenuBackend(menuUseEvent,menuChangeEvent);
  //beneath is list of menu items needed to build the menu
  MenuItem settings   = MenuItem(menu, "Settings", 1);
    MenuItem pin      = MenuItem(menu, "Pin", 2);
    MenuItem debug    = MenuItem(menu, "Debug", 2);
  MenuItem options    = MenuItem(menu, "Options", 1);
    MenuItem setDelay = MenuItem(menu, "Delay",'D', 2);
      MenuItem d100   = MenuItem(menu, "100 ms", 3);
      MenuItem d200   = MenuItem(menu, "200 ms", 3);
      MenuItem d300   = MenuItem(menu, "300 ms", 3);
      MenuItem d400   = MenuItem(menu, "400 ms", 3);

//this function builds the menu and connects the correct items together
void menuSetup()
{
  Serial.println("Setting up menu...");
  //add the file menu to the menu root
  menu.getRoot().add(settings); 
    //setup the settings menu item
    settings.addAfter(options); 
    options.addAfter(settings); 
    
    settings.addLeft(settings); //loop back if left on settings
    settings.addRight(pin);     //chain settings to  pin on right
      debug.addLeft(settings);  //also go to settings left for debug
      //we want looping both up and down
      pin.addBefore(debug);
      pin.addAfter(debug);
      debug.addBefore(pin);
      debug.addAfter(pin);
    
    options.addLeft(options);   //loop back if left on settings
    options.addRight(setDelay); //chain options to delay on right
      setDelay.addRight(d100);
        d100.addBefore(d100);   //loop to d400 
        d100.addAfter(d200);
        d200.addAfter(d300);
        d300.addAfter(d400);
        d400.addAfter(d100);    //loop back to d100
        //we want left to always be bak to delay
        d100.addLeft(setDelay);
        d200.addLeft(setDelay);
        d300.addLeft(setDelay);
        d400.addLeft(setDelay);
}

/*
  This is an important function
  Here all use events are handled
  
  This is where you define a behaviour for a menu item
*/
void menuUseEvent(MenuUseEvent used)
{
  Serial.print("Menu use ");
  Serial.println(used.item.getName());
  
  if (used.item.isEqual(setDelay)) //comparison agains a known item
  {
    Serial.println("menuUseEvent found Delay (D)");
  }
}

/*
  This is an important function
  Here we get a notification whenever the user changes the menu
  That is, when the menu is navigated
*/
void menuChangeEvent(MenuChangeEvent changed)
{
  Serial.print("Menu change ");
  Serial.print(changed.from.getName());
  Serial.print(" ");
  Serial.println(changed.to.getName());
}

void setup()
{
  Serial.begin(9600);
  
  menuSetup();
  Serial.println("Starting navigation:\r\nUp: w   Down: s   Left: a   Right: d   Use: e");
  
  //fire Delay : menu.use('D');
  //bail back to Options by moving up one logical level after a D : menu.moveRelativeLevels(-1);
}

void loop()
{
  if (Serial.available()) 
  {
    byte read = Serial.read();
    switch (read) 
    {
      case 'w': menu.moveUp(); break;
      case 's': menu.moveDown(); break;
      case 'd': menu.moveRight(); break;
      case 'a': menu.moveLeft(); break;
      case 'e': menu.use(); break;
    }
  }
}

