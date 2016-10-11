/*****************
 * This example prints the binary status of the 4 pins used for the SKAARHOJ encoder boards rotary function.
 * It will also print, if/when the interrupts were fired.
 * The output - properly understood - will show in which order pins are set and released by the encoders
 * It's used for a very basic diagnostics of the encoder boards, in particular it should confirm that only
 * one (or two) interrupts are triggered per "click" of the encoder 
 * 
 * - kasper
 *
 * This example code is under GNU GPL license
 */



#include <Streaming.h>


// Set up the pins:
int triggerPin1 = 3;
int directionPin1 = 5;
int triggerPin2 = 2;
int directionPin2 = 8;


void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(115200);
  Serial << F("\n- - - - - - - -\nSerial Started\n");

  // make the pushbutton's pin an input:
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(5, INPUT);
  pinMode(8, INPUT);
  
    // Here we attach hardware interrups to functions that call the encoder object (see below). Trigger pins must be soldered to D2 and D3:
  attachInterrupt(0, _enc1active, RISING);  // Lower encoder (1), pin 2
  attachInterrupt(1, _enc0active, RISING);  // Higher encoder (0), pin 3  
}

unsigned long lastTime=0;

  // The following two functions are called for hardware interrupt 0 and 1. They in turn call the encoder object to detect the direction pin state.
void _enc0active()  {
    if (lastTime+10 < (unsigned long)millis())  {
        Serial << F("==========\n");
    }
    lastTime = millis();
  Serial << F("E0\n"); 
}
void _enc1active()  {
    if (lastTime+10 < (unsigned long)millis())  {
        Serial << F("==========\n");
    }
    lastTime = millis();
  Serial << F("E1\n"); 
}

// the loop routine runs over and over again forever:
uint8_t memory = 255;
void loop() {
  bool pin2 = digitalRead(2);
  bool pin3 = digitalRead(3);
  bool pin5 = digitalRead(5);
  bool pin8 = digitalRead(8);
  uint8_t fingerPrint = (pin2?B1<<3:B0) | (pin8?B1<<2:B0) | (pin3?B1<<1:B0) | (pin5?B1:B0);
  
  // Only print, if there was a change:
  if (fingerPrint>0)  {
    if (lastTime+10 < (unsigned long)millis())  {  // Print a divider if it's more than 100 ms before last event.
        Serial << F("==========\n");
    }
    lastTime = millis();
  }
  if (fingerPrint!=memory)  {
    Serial << pin2  << pin8  << pin3  << pin5 << "\n";
  }
  
  memory = fingerPrint;
  delay(1);        // delay in between reads for stability
}

