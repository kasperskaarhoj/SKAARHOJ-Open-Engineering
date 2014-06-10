#include "Wire.h"
#include "SkaarhojEADOGMDisplay.h"
#include "Streaming.h"




SkaarhojEADOGMDisplay Disp163;
SkaarhojEADOGMDisplay Disp81;

void setup() {                
  Serial.begin(9600); // set up serial
  Serial.println("Start");

  Wire.begin();
  Disp163.begin(0,0,3);  // DOGM163
  Disp81.begin(0,1,1);  // DOGM81


  Disp163.cursor(true);
  Disp163 << F("millis(): ") << millis();
  Disp163.print("AbCDE\nY");
  Disp81.cursor(true);
  Disp81 << _HEX(12345);
  delay(3000);
  
  Disp163.cursor(false);
  Disp81.cursor(false);
  Disp163.clearDisplay();
}

void loop()  {
  Disp163 << F("Time:") << _DECPADL(millis(), 7, " ") << F("ms");  // No need to add New Line because we set position directly for the next line

  Disp163.gotoRowCol(1,4);
  Disp163 << F("0x") << _HEXPADL(millis(), 6, "0") << F("ms\n");  // Adding newline to go to next line because we don't fill out the line completely.

  Disp163 << _BINPADL(millis() >> 10, 16, "0");  // No need to add New Line because the line is 16 chars long through padding. It will wrap around.
}
