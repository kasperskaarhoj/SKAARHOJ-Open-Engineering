/*****************
 * Calibration of touch screen.
 * Follow instructions printed to the Serial monitor.
 *
 * - kasper
 */


#include "SkaarhojUtils.h"
SkaarhojUtils utils;

#include <EEPROM.h>      // For storing IP numbers

#include <Streaming.h>

// Collects 16 samples, buffer:
int roundRobinX[16];
int roundRobinY[16];

// Stores the detected coordinates:
int calibrationCoordinatesX[5];
int calibrationCoordinatesY[5];

uint8_t checksumByte;

void setup() {
  Serial.begin(115200);
  Serial.println(F("Welcome to the SKAARHOJ utils-library touchscreen calibration process!"));
  Serial.println(F("Please make sure your screen shows the SKAARHOJ monitor calibration chart edge to edge."));
  Serial.println(F("This chart is found in the same folder on your hard drive along with this example file (Monitor Calibration Chart.png)."));
  Serial.println(F("Then, follow instructions below:"));
  Serial.println("");

  utils.touch_init();


  for (int i=1; i<=4; i++)  {
    // Read point i:
    do {
      Serial.print(F("Press and hold crosshair point "));
      Serial.print(i);
      Serial.println(F(" until a confirmation appears."));
      while (!utils.touch_isTouched())  {
      }
    } 
    while (!readPoint(i));
    Serial.println("");
  }
  
  utils.touch_calibrationPointRawCoordinates(
    calibrationCoordinatesX[1], calibrationCoordinatesY[1], 
    calibrationCoordinatesX[2], calibrationCoordinatesY[2], 
    calibrationCoordinatesX[3], calibrationCoordinatesY[3], 
    calibrationCoordinatesX[4], calibrationCoordinatesY[4]);

    Serial.println(F("Screen is calibrated!"));
    Serial.println(F("In your arduino sketch you need to insert this line (copy/paste it) right after 'utils.touch_init();':"));
    Serial.println("");

    Serial.print(F("       utils.touch_calibrationPointRawCoordinates("));
    Serial.print(calibrationCoordinatesX[1]);    
    Serial.print(",");
    Serial.print(calibrationCoordinatesY[1]);    
    Serial.print(",");    
    Serial.print(calibrationCoordinatesX[2]);    
    Serial.print(",");    
    Serial.print(calibrationCoordinatesY[2]);    
    Serial.print(",");    
    Serial.print(calibrationCoordinatesX[3]);    
    Serial.print(",");    
    Serial.print(calibrationCoordinatesY[3]);    
    Serial.print(",");    
    Serial.print(calibrationCoordinatesX[4]);    
    Serial.print(",");    
    Serial.print(calibrationCoordinatesY[4]);    
    Serial.println(");");
    Serial.println("");
    Serial.println(F("Before you do so, please touch the screen and verify the coordinates matches the calibration picture."));

    Serial.println("");
    Serial.println(F("This configuration has also be written to EEPROM memory now and some recent sketches will use the data from there."));
    Serial.println("");
    
    EEPROM.write(50,(int)calibrationCoordinatesX[1]/256);
    EEPROM.write(51,(int)calibrationCoordinatesX[1]%256);
    EEPROM.write(52,(int)calibrationCoordinatesY[1]/256);
    EEPROM.write(53,(int)calibrationCoordinatesY[1]%256);
    EEPROM.write(54,(int)calibrationCoordinatesX[2]/256);
    EEPROM.write(55,(int)calibrationCoordinatesX[2]%256);
    EEPROM.write(56,(int)calibrationCoordinatesY[2]/256);
    EEPROM.write(57,(int)calibrationCoordinatesY[2]%256);
    EEPROM.write(58,(int)calibrationCoordinatesX[3]/256);
    EEPROM.write(59,(int)calibrationCoordinatesX[3]%256);
    EEPROM.write(60,(int)calibrationCoordinatesY[3]/256);
    EEPROM.write(61,(int)calibrationCoordinatesY[3]%256);
    EEPROM.write(62,(int)calibrationCoordinatesX[4]/256);
    EEPROM.write(63,(int)calibrationCoordinatesX[4]%256);
    EEPROM.write(64,(int)calibrationCoordinatesY[4]/256);
    EEPROM.write(65,(int)calibrationCoordinatesY[4]%256);
    EEPROM.write(66,
      ((int)(calibrationCoordinatesX[1]
      + calibrationCoordinatesY[1]
      + calibrationCoordinatesX[2]
      + calibrationCoordinatesY[2]
      + calibrationCoordinatesX[3]
      + calibrationCoordinatesY[3]
      + calibrationCoordinatesX[4]
      + calibrationCoordinatesY[4]))  & 0xFF);  // checksum
      
}


void loop() {
  uint8_t touchState = utils.touch_state();
  switch(touchState)  {
    case 1:
      Serial.print("Single touch in: x=");
      Serial.print(utils.touch_getEndedXCoord());
      Serial.print("; y=");
      Serial.println(utils.touch_getEndedYCoord());
    break; 
    case 2:
      Serial.print("Double touch in: x=");
      Serial.print(utils.touch_getEndedXCoord());
      Serial.print("; y=");
      Serial.println(utils.touch_getEndedYCoord());
    break; 
    case 3:
      Serial.print("Triple touch in: x=");
      Serial.print(utils.touch_getEndedXCoord());
      Serial.print("; y=");
      Serial.println(utils.touch_getEndedYCoord());
    break; 
    case 9:
      Serial.print("Single touch - held down for a while: x=");
      Serial.print(utils.touch_getEndedXCoord());
      Serial.print("; y=");
      Serial.println(utils.touch_getEndedYCoord());
    break; 
    case 10:
      Serial.println("Swipe gesture Left -> Right");
    break; 
    case 11:
      Serial.println("Swipe gesture Right -> Left");
    break; 
    case 12:
      Serial.println("Swipe gesture Top -> Bottom");
    break; 
    case 13:
      Serial.println("Swipe gesture Bottom -> Top");
    break; 
    case 255:
      Serial.println("Some other type of touch. Bounding Box?");
          int xMax = utils.touch_coordX(utils.touch_getRawXValMax());
	  int xMin = utils.touch_coordX(utils.touch_getRawXValMin());
	  int yMax = utils.touch_coordY(utils.touch_getRawYValMax());
	  int yMin = utils.touch_coordY(utils.touch_getRawYValMin());

      Serial.print("From: x=");
      Serial.print(xMin);
      Serial.print("; y=");
      Serial.println(yMin);

      Serial.print("To: x=");
      Serial.print(xMax);
      Serial.print("; y=");
      Serial.println(yMax);
    break; 
  }
  delay(10);
}

// Reading the raw coordinates of a reference point:
bool readPoint(uint8_t pointNumber)  {
  Serial.println("Reading in progress...");

  // PointNumber between 1-4:
  int i=0;
  calibrationCoordinatesX[pointNumber] = 0;
  calibrationCoordinatesY[pointNumber] = 0;

  delay(1000);  // Wait for press to stabilize

  // Making 16 samples:
  while(utils.touch_isTouched() && i<16)  {
    roundRobinX[i] = utils.touch_getRawXVal();
    roundRobinY[i] = utils.touch_getRawYVal();
    i++;
    delay(50);
  }

  // Seeing if the pressure was released prematurely:
  if (i!=16)  {
    Serial.println("Error: You released your pressure too early, try again");
    return false; 
  } 
  else {
    // Ok reading, make calculations and return:
    Serial.print("Point reading OK: ");
    for(i=0; i<16;i++)  {
      calibrationCoordinatesX[pointNumber]+= roundRobinX[i];
      calibrationCoordinatesY[pointNumber]+= roundRobinY[i];
    }
    calibrationCoordinatesX[pointNumber]=calibrationCoordinatesX[pointNumber]>>4;
    calibrationCoordinatesY[pointNumber]=calibrationCoordinatesY[pointNumber]>>4;

    Serial.print("x=");
    Serial.print(calibrationCoordinatesX[pointNumber]);
    Serial.print("; y=");
    Serial.print(calibrationCoordinatesY[pointNumber]);

    // Waiting for touch to stop before we return:
    while (utils.touch_isTouched())  {
    }

    return true; 
  }
}


