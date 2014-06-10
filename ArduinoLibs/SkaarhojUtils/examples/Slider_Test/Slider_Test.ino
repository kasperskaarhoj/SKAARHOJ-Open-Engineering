/*****************
 * This example returns state of a slider or T-bar from a SKAARHOJ Slider board/T-bar
 * Notice that the output is always relative to which end the slider starts. This is how the T-bar on an ATEM
 * console expect the data to be passed. The output of uniDirectionalSlider_position() can be passed directly
 * to the ATEM library.
 * 
 * - kasper
 */


#include "SkaarhojUtils.h"
SkaarhojUtils utils;

//#include <MemoryFree.h>

// no-cost stream operator as described at 
// http://arduiniana.org/libraries/streaming/
template<class T>
inline Print &operator <<(Print &obj, T arg)
{  
  obj.print(arg); 
  return obj; 
}



void setup() { 

  // Start the Serial (debugging) and UDP:
  Serial.begin(9600);  
  Serial << F("\n- - - - - - - -\nSerial Started\n");

  // Initializing the slider:
  utils.uniDirectionalSlider_init(10, 35, 35, A0);  // These 4 arguments are the default arguments given, if you call the function without any. See the library SkaarhojUtils for more info on what they are for.
  
    // Shows free memory:  
//  Serial << F("freeMemory()=") << freeMemory() << "\n";
}

void loop() {

  // Slidertest:
  if (utils.uniDirectionalSlider_hasMoved())  {
    Serial << F("New position ") << utils.uniDirectionalSlider_position() << "\n";
    if (utils.uniDirectionalSlider_isAtEnd())  {
      Serial << F("Slider at end\n");
    }
  }
}


