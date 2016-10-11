/*****************
 * 
 * 
 * - kasper
 */

#include <Wire.h>
#include <MCP23017.h>
#include <PCA9685.h>
#include <Streaming.h>

MCP23017 GPIOchipArray[] = {
  MCP23017(), MCP23017(), MCP23017(), MCP23017(), MCP23017(), MCP23017(), MCP23017(), MCP23017()}; 
word MCP23017_states[8];

PCA9685 ledDriver; 
bool PCA9685_states[64];


void setup()
{
  Serial.begin(115200); // set up serial
  Serial << F("\n- - - - - - - -\nSerial Started\n");
  delay(500);

  Wire.begin();           // Wire must be started!
  delay(500);
  // Set up each MCP23017 chip:
  for(uint8_t i=0;i<=7;i++)  {  
    GPIOchipArray[i].begin(i);
    GPIOchipArray[i].init();
    GPIOchipArray[i].internalPullupMask(65535);
    GPIOchipArray[i].inputOutputMask(65535);  // All inputs
  }
  delay(500);
}

bool firstTime = true;
unsigned long counter = 0;
bool deviation = false;
int theSpeed = 10;

void loop()
{
  if (firstTime)  Serial << "----\nMCP23017::\n";
  for(uint8_t i=0;i<=7;i++)  {  
    word buttonStatus = GPIOchipArray[i].digitalWordRead();
    if (firstTime)  {
      Serial << "Board #" << i << ": ";
      Serial.println(buttonStatus, BIN);
      MCP23017_states[i] = buttonStatus;
    } 
    else {
      if (MCP23017_states[i] != buttonStatus)  {
        Serial << "Iter." << counter << ": MCP23017, Board #" << i << ": ";
        Serial.println(buttonStatus, BIN);
        deviation = true;
        theSpeed = 3;
      }
    }
    delay(theSpeed);
  }



  if (firstTime)  Serial << "\n\nPCA9685::\n";
  for(uint8_t i=0;i<=63;i++)  {  
    if (i!=48)  {  // Don't address the broadcast address.
      // Set up each board:
      ledDriver.begin(i);  // Address pins A5-A0 set to B111000

      if (firstTime)  {
        if (ledDriver.init())  {
          Serial << "\nBoard #" << i << " found\n";
          PCA9685_states[i] = true;
        } 
        else {
          Serial << ".";
          PCA9685_states[i] = false;
        }
      }
      else{
        bool ledDriverState = ledDriver.init();
        if (ledDriverState!=PCA9685_states[i])  {
          Serial << "Iter." << counter << ": PCA9685, Board #" << i << " " << (ledDriver.init()?"found":"missing");
          Serial << "\n";
          deviation = true;
          theSpeed = 3;
        } 
        if (ledDriverState)  {
          if (deviation)  {  // Blinks if there has been a deviation from expected:
            for (uint8_t ii=0; ii<16; ii++)  {
              ledDriver.setLEDDimmed(ii,counter%2 ? 100 : 0);
            } 
          } 
          else {
            if (counter%20 < 3)  {   // 100% color: (3 seconds)
              for (uint8_t ii=0; ii<16; ii++)  {
                ledDriver.setLEDDimmed(ii,(counter%20)*50);
              } 
            }
            else if (counter%20 < 15)  {  // The random color programme:
              for (uint8_t ii=0; ii<16; ii++)  {
                ledDriver.setLEDDimmed(ii,random(0,6)*20);
              } 
            } 
            else {  // Same intensity for all LEDs:
              int randColor1 = random(0,3)*50;
              int randColor2 = random(0,3)*50;
              for (uint8_t ii=0; ii<16; ii++)  {
                ledDriver.setLEDDimmed(ii,ii%2 ? randColor1 : randColor2);
              } 
            }
          }
        }
      }
    }
    delay(theSpeed);
  }  
  if (counter%20 < 3)  {
    delay(3000); 
  }


  if (firstTime)  Serial << "\n\n----\nWaiting for Deviations:";
  if (counter%(30*60)==0)  Serial << "\n" << counter << " ";
  if (counter%30==0)  Serial << ".";

  counter++;
  firstTime = false;
} 









