/*****************
 * ...
 * - kasper
 *
 * This example code is in the public domain.
 */


#include "SkaarhojUtils.h"
SkaarhojUtils utils;

void setup() {
  Serial.begin(115200);
  Serial.println("Serial started");
  Serial.println("Coordinate system origo should be lower left corner of screen");
  
  utils.touch_init();
  utils.touch_setExtended();
  
    // The line below is calibration numbers for a specific monitor. 
    // Substitute this with calibration for YOUR monitor (see example "Touch_Calibrate")
  utils.touch_calibrationPointRawCoordinates(326,705,764,715,763,357,326,342);  
}

void loop() {
  Serial.print(utils.touch_isTouched());
  Serial.print(" - Raw reading (x,y): ");
  Serial.print(utils.touch_getRawXVal());
  Serial.print(",");
  Serial.print(utils.touch_getRawYVal());

  Serial.print(" - Calibrated (x,y): ");
  Serial.print(utils.touch_coordX(utils.touch_getRawXVal()));
  Serial.print(",");
  Serial.print(utils.touch_coordY(utils.touch_getRawYVal()));

  Serial.println();
}
