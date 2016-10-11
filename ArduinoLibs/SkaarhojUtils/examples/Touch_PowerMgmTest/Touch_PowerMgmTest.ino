/*****************
 * Touch Power Management Test
 * 
 * - kasper
 */

/*
  // Pins for Arduino Breakout Shield 2:
int greenLEDPin = 2;
int redLEDPin = 3;
int powerMgmPin = 4;
int powerStatusDetectPin = A5;
*/

  // Pins for Arduino Mega Extended Breakout Shield:
int greenLEDPin = 22;
int redLEDPin = 23;
int powerMgmPin = 24;
int powerStatusDetectPin = A10;

#include <Streaming.h>

void setup() { 

    
  // Start the Ethernet, Serial (debugging) and UDP:
  Serial.begin(115200);
  Serial << F("\n- - - - - - - -\nSerial Started\n");  
  
  pinMode(powerMgmPin, INPUT);
  delay(1000);
  
  // Sets the Bi-color LED to off = "no connection"
  digitalWrite(redLEDPin,false);
  digitalWrite(greenLEDPin,false);
  pinMode(redLEDPin, OUTPUT);
  pinMode(greenLEDPin, OUTPUT);
}


void loop() {
  powerOnScreen();

  Serial << "RED\n";
  digitalWrite(greenLEDPin,false);
  digitalWrite(redLEDPin,true);
  delay(1000);

  Serial << "GREEN\n";
  digitalWrite(greenLEDPin,true);
  digitalWrite(redLEDPin,false);
  delay(1000);

  Serial << "ORANGE\n";
  digitalWrite(greenLEDPin,true);
  digitalWrite(redLEDPin,true);
  delay(1000);

  Serial << "Off\n";
  digitalWrite(greenLEDPin,false);
  digitalWrite(redLEDPin,false);
  delay(1000);
}


void powerOnScreen() {
    // Touch Screen power-up. Should not affect touch screen solution where it is not wired up.
    // Uses D4 to trigger the "on" signal (through a 15K res), first set it in Tri-state mode.
    // Uses Analog pin A5 to detect from the LED voltage levels whether it is already on or not
  int analogInputValue = analogRead(powerStatusDetectPin);
  Serial << "Analog Read of PWRLED2: " << analogInputValue;
/*
Samples on various monitors:
(off) 830-670 (on)
(off) 860-700 (on)
(off) 870-710 (on)
(off) 830-670 (on)
=> 770 is a reasonable threshold.
*/
  if (analogInputValue>770)  {
    Serial << " => Screen off, turning it on..." << "\n";
    // Now, trying to turn the screen on:
    digitalWrite(powerMgmPin,0);  // First, set the next output of D4 to low
    pinMode(powerMgmPin, OUTPUT);  // Change D4 from an high impedance input to an output, effectively triggering a button press
    delay(500);  // Wait a while.
    pinMode(powerMgmPin, INPUT);  // Change D4 back to high impedance input.
  } else {
    Serial << " => Screen already turned on" << "\n";
  }  
}
