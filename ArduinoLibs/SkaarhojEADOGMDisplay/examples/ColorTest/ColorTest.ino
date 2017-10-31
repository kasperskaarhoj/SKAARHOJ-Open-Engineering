#include "Wire.h"
#include "SkaarhojEADOGMDisplay.h"
#include "Streaming.h"




//SkaarhojEADOGMDisplay Disp163;
//SkaarhojEADOGMDisplay Disp162;
SkaarhojEADOGMDisplay Disp81;

void setup() {
  Serial.begin(115200); // set up serial
  Serial.println("Start");

  Wire.begin();
  //  Disp163.begin(0,0,3);  // DOGM163
  Disp81.begin(1, 0, 1); // DOGM81
  Disp81.contrast(5);
  Disp81.cursor(false);
  //  Disp162.begin(4,0,2);  // DOGM81
  Disp81.setBacklight(1, 0, 0);
  delay(500);
  Disp81.setBacklight(0, 1, 0);
  delay(500);
  Disp81.setBacklight(1, 1, 1);


  /*
    Disp163.cursor(true);
    Disp163 << F("millis(): ") << millis();
    Disp163.print("AbCDE\nY");

    Disp162.cursor(true);
    Disp162 << F("millis():");
    Disp162.gotoRowCol(1,0);
    Disp162 << F("ABC");

    //  Disp162.print("AbCDE\nY");
  */
  Disp81.cursor(true);
  Disp81 << _HEX(12345);
  delay(3000);

  for (uint8_t a = 0; a < 16; a++)  {
    Disp81.contrast(a);
    Disp81.clearDisplay();
    Disp81 << _HEX(a);
    delay(500);
  }
  Disp81.contrast(0x4);
  Disp81.contrast(5);
  Disp81.cursor(false);
  /*
    for (uint8_t a=0; a<16; a++)  {
    Disp163.contrast(a);
    Disp163.clearDisplay();
    Disp163 << _HEX(a);
    delay(500);
    }
    Disp163.contrast(0xC);

    for (uint8_t a=0; a<16; a++)  {
    Disp162.contrast(a);
    Disp162.clearDisplay();
    Disp162 << _HEX(a);
    delay(500);
    }
    Disp162.contrast(0xA);

    Disp163.cursor(false);
    Disp81.cursor(false);
    Disp163.clearDisplay();
  */
}

void loop()  {
  Disp81.clearDisplay();
  Disp81.setBacklight(1, 0, 0);
  Disp81 << "RED";
  delay(1000);
  Disp81.clearDisplay();
  Disp81.setBacklight(0, 1, 0);
  Disp81 << "GREEN";
  delay(1000);
  Disp81.clearDisplay();
  Disp81.setBacklight(0, 0, 1);
  Disp81 << "BLUE";
  delay(1000);
  Disp81.clearDisplay();
  Disp81.setBacklight(1, 1, 0);
  Disp81 << "YELLOW";
  delay(1000);
  Disp81.clearDisplay();
  Disp81.setBacklight(1, 0, 1);
  Disp81 << "MAGENTA";
  delay(1000);
  Disp81.clearDisplay();
  Disp81.setBacklight(0, 1, 1);
  Disp81 << "CYAN";
  delay(1000);
  Disp81.clearDisplay();
  Disp81.setBacklight(1, 1, 1);
  Disp81 << "WHITE";
  delay(1000);
  /*
    Disp163 << F("Time:") << _DECPADL(millis(), 7, " ") << F("ms");  // No need to add New Line because we set position directly for the next line

    Disp163.gotoRowCol(1,4);
    Disp163 << F("0x") << _HEXPADL(millis(), 6, "0") << F("ms\n");  // Adding newline to go to next line because we don't fill out the line completely.

    Disp163 << _BINPADL(millis() >> 10, 16, "0");  // No need to add New Line because the line is 16 chars long through padding. It will wrap around.
  */

  // Disp162.gotoRowCol(0,0);
  // Disp162 << F("Time");
  // Disp162.gotoRowCol(0,6);
  // Disp162 << _DECPADL(millis(), 7, " ") << F("\n");  // No need to add New Line because we set position directly for the next line
  //  Disp162.gotoRowCol(1,0);

  //  for(uint8_t a=0; a<10;a++)  {
  //      Disp162 << a;
  //  //    delay(100);
  //  }
  //  Disp162.gotoRowCol(0,0);
  //  Disp162 << F("0x") << _HEXPADL(millis(), 6, "0");
  //  delay(100);
  //  Disp162.gotoRowCol(1,10);
  //  Disp162 << F("12345");  // Adding newline to go to next line because we don't fill out the line completely.
}
