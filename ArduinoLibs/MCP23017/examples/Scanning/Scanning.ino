/*****************


 * - kasper
 */
 
#include <Wire.h>
#include <MCP23017.h>


#include <Streaming.h>

MCP23017 GPIOchipArray[] = {MCP23017(), MCP23017(), MCP23017(), MCP23017(), MCP23017(), MCP23017(), MCP23017(), MCP23017()}; 



void setup()
{
  Serial.begin(115200); // set up serial
  Serial << F("\n- - - - - - - -\nSerial Started\n");

  Wire.begin();           // Wire must be started!

  // Set up each board:
  for(uint8_t i=0;i<=7;i++)  {  
    GPIOchipArray[i].begin(i);
    GPIOchipArray[i].init();
    GPIOchipArray[i].internalPullupMask(65535);
    GPIOchipArray[i].inputOutputMask(65535);  // All inputs
  }
}

void loop()
{
    Serial << "----\n";
  for(uint8_t i=0;i<=7;i++)  {  
    word buttonStatus = GPIOchipArray[i].digitalWordRead();
    Serial << "Board " << i << ": ";
    Serial.println(buttonStatus, BIN);
    delay(100);
  }
} 




