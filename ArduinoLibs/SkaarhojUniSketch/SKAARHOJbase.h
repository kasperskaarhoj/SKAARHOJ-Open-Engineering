/*
Copyright 2016 Kasper Skårhøj, SKAARHOJ, kasper@skaarhoj.com

This file is part of the Skaarhoj Unisketch for Arduino

The Skaarhoj Unisketch library is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published by the 
Free Software Foundation, either version 3 of the License, or (at your 
option) any later version.

The Skaarhoj Unisketch library is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
or FITNESS FOR A PARTICULAR PURPOSE. 
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along 
with the Skaarhoj Unisketch library. If not, see http://www.gnu.org/licenses/.


IMPORTANT: If you want to use this library in your own projects and/or products,
please play a fair game and heed the license rules! See our web page for a Q&A so
you can keep a clear conscience: http://skaarhoj.com/about/licenses/

*/


/************************************
 *
 * GRAPHICS
 *
 ************************************/
#if SK_HWEN_SSWMENU || SK_HWEN_SSWBUTTONS
static const uint8_t welcomeGraphics[4][32 * 8] PROGMEM = {
  {
    B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111,
    B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111,
    B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111,
    B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111,
    B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111,
    B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111,
    B11111100, B00000000, B00000000, B00000000, B11100000, B00011111, B11111100, B00000000,
    B11100000, B00000000, B00000000, B00000000, B11100000, B00011111, B11111000, B00000001,
    B11000000, B00000000, B00000000, B00000000, B11100000, B00011111, B11100000, B00000011,
    B10000000, B00000000, B00000000, B00000000, B11100000, B00011111, B11000000, B00001111,
    B00000000, B00011111, B11111111, B11111111, B11100000, B00011111, B10000000, B00011111,
    B00000000, B00111111, B11111111, B11111111, B11100000, B00011110, B00000000, B01111111,
    B00000000, B00011111, B11111111, B11111111, B11100000, B00011100, B00000000, B11111111,
    B10000000, B00000000, B00000000, B00111111, B11100000, B00011000, B00000011, B11111111,
    B10000000, B00000000, B00000000, B00000011, B11100000, B00000000, B00000111, B11111111,
    B11100000, B00000000, B00000000, B00000000, B11100000, B00000000, B00011111, B11111111,
    B11111000, B00000000, B00000000, B00000000, B01100000, B00010000, B00000111, B11111111,
    B11111111, B11111111, B11111100, B00000000, B00100000, B00011000, B00000011, B11111111,
    B11111111, B11111111, B11111111, B00000000, B00100000, B00011100, B00000001, B11111111,
    B11111111, B11111111, B11111111, B10000000, B00100000, B00011110, B00000000, B01111111,
    B11111111, B11111111, B11111111, B00000000, B00100000, B00011111, B10000000, B00111111,
    B11000000, B00000000, B00000000, B00000000, B00100000, B00011111, B11000000, B00001111,
    B11000000, B00000000, B00000000, B00000000, B01100000, B00011111, B11100000, B00000111,
    B11000000, B00000000, B00000000, B00000001, B11100000, B00011111, B11110000, B00000001,
    B11000000, B00000000, B00000000, B00000111, B11100000, B00011111, B11111100, B00000000,
    B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111,
    B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111,
    B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111,
    B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111,
    B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111,
    B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111,
    B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111,
  }
  ,
  {
    B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111,
    B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111,
    B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111,
    B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111,
    B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111,
    B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111,
    B11111111, B11110000, B00000111, B11111111, B11111111, B11100000, B00001111, B11111111,
    B11111111, B11100000, B00000111, B11111111, B11111111, B11000000, B00000111, B11111111,
    B11111111, B11100000, B00000011, B11111111, B11111111, B11000000, B00000111, B11111111,
    B11111111, B11000000, B00000001, B11111111, B11111111, B10000000, B00000011, B11111111,
    B11111111, B10000000, B00000000, B11111111, B11111111, B00000000, B00000001, B11111111,
    B11111111, B10000000, B00000000, B11111111, B11111111, B00000000, B00000001, B11111111,
    B11111111, B00000000, B10000000, B01111111, B11111110, B00000001, B00000000, B11111111,
    B11111110, B00000001, B11000000, B00111111, B11111100, B00000011, B10000000, B01111111,
    B11111110, B00000001, B11100000, B00011111, B11111100, B00000011, B11000000, B00111111,
    B11111100, B00000011, B11100000, B00011111, B11111000, B00000111, B11000000, B00111111,
    B11111000, B00000111, B11110000, B00001111, B11110000, B00001111, B11100000, B00011111,
    B11110000, B00000000, B00000000, B00000111, B11110000, B00000000, B00000000, B00001111,
    B11110000, B00000000, B00000000, B00000111, B11100000, B00000000, B00000000, B00001111,
    B11100000, B00000000, B00000000, B00000011, B11000000, B00000000, B00000000, B00000111,
    B11000000, B00000000, B00000000, B00000001, B11000000, B00000000, B00000000, B00000011,
    B11000000, B00111111, B11111110, B00000000, B10000000, B01111111, B11111100, B00000001,
    B10000000, B01111111, B11111111, B00000000, B00000000, B01111111, B11111110, B00000001,
    B00000000, B01111111, B11111111, B10000000, B00000000, B11111111, B11111111, B00000000,
    B00000000, B11111111, B11111111, B10000000, B00000001, B11111111, B11111111, B00000000,
    B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111,
    B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111,
    B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111,
    B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111,
    B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111,
    B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111,
    B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111,
  }
  ,
  {
    B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111,
    B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111,
    B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111,
    B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111,
    B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111,
    B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111,
    B00000000, B00000000, B00000000, B00011111, B11000000, B00111111, B11111111, B00000000,
    B00000000, B00000000, B00000000, B00000111, B11000000, B00111111, B11111111, B00000000,
    B00000000, B00000000, B00000000, B00000011, B11000000, B00111111, B11111111, B00000000,
    B00000000, B00000000, B00000000, B00000001, B11000000, B00111111, B11111111, B00000000,
    B00000000, B11111111, B11111100, B00000000, B11000000, B00111111, B11111111, B00000000,
    B00000000, B11111111, B11111110, B00000000, B11000000, B00111111, B11111111, B00000000,
    B00000000, B11111111, B11111110, B00000000, B11000000, B00111111, B11111111, B00000000,
    B00000000, B11111111, B11111100, B00000000, B11000000, B00111111, B11111111, B00000000,
    B00000000, B00000000, B00000000, B00000001, B11000000, B00000000, B00000000, B00000000,
    B00000000, B00000000, B00000000, B00000011, B11000000, B00000000, B00000000, B00000000,
    B00000000, B00000000, B00000000, B00000111, B11000000, B00000000, B00000000, B00000000,
    B00000000, B00000000, B00000000, B01111111, B11000000, B00000000, B00000000, B00000000,
    B00000000, B11111111, B10000000, B00111111, B11000000, B00111111, B11111111, B00000000,
    B00000000, B11111111, B11000000, B00011111, B11000000, B00111111, B11111111, B00000000,
    B00000000, B11111111, B11100000, B00001111, B11000000, B00111111, B11111111, B00000000,
    B00000000, B11111111, B11110000, B00000011, B11000000, B00111111, B11111111, B00000000,
    B00000000, B11111111, B11111000, B00000001, B11000000, B00111111, B11111111, B00000000,
    B00000000, B11111111, B11111100, B00000000, B11000000, B00111111, B11111111, B00000000,
    B00000000, B11111111, B11111110, B00000000, B01000000, B00111111, B11111111, B00000000,
    B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111,
    B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111,
    B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111,
    B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111,
    B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111,
    B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111,
    B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111,

  }
  ,
  {
    B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111,
    B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111,
    B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111,
    B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111,
    B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111,
    B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111,
    B11111111, B11100000, B00000000, B00000111, B11111111, B11111111, B11111000, B00000011,
    B11111111, B00000000, B00000000, B00000000, B11111111, B11111111, B11111000, B00000011,
    B11111100, B00000000, B00000000, B00000000, B00111111, B11111111, B11111000, B00000011,
    B11111000, B00000000, B00000000, B00000000, B00011111, B11111111, B11111000, B00000011,
    B11110000, B00000011, B11111111, B10000000, B00001111, B11111111, B11111000, B00000011,
    B11100000, B00001111, B11111111, B11100000, B00001111, B11111111, B11111000, B00000011,
    B11100000, B00001111, B11111111, B11110000, B00000111, B11111111, B11111000, B00000011,
    B11000000, B00011111, B11111111, B11110000, B00000111, B11111111, B11111000, B00000011,
    B11000000, B00011111, B11111111, B11110000, B00000111, B11111111, B11111000, B00000011,
    B11000000, B00011111, B11111111, B11110000, B00000111, B11111111, B11111000, B00000011,
    B11000000, B00011111, B11111111, B11110000, B00000111, B11111111, B11111000, B00000011,
    B11000000, B00011111, B11111111, B11110000, B00000111, B11111111, B11111000, B00000011,
    B11100000, B00011111, B11111111, B11110000, B00000111, B11111111, B11111000, B00000011,
    B11100000, B00001111, B11111111, B11100000, B00001111, B11111111, B11111000, B00000011,
    B11110000, B00000011, B11111111, B11000000, B00001111, B11111111, B11100000, B00000111,
    B11110000, B00000000, B00000000, B00000000, B00010000, B00000000, B00000000, B00000111,
    B11111100, B00000000, B00000000, B00000000, B00100000, B00000000, B00000000, B00001111,
    B11111110, B00000000, B00000000, B00000000, B11100000, B00000000, B00000000, B00111111,
    B11111111, B11000000, B00000000, B00000111, B11100000, B00000000, B00000001, B11111111,
    B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111,
    B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111,
    B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111,
    B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111,
    B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111,
    B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111,
    B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111,
  }
};
#endif

#if SK_HWEN_STDOLEDDISPLAY
// width x height = 128,13
static const uint8_t SKAARHOJLogo[] PROGMEM = {
  B00011111, B11111111, B00111000, B00011111, B00000011, B11000000, B00000011, B11000000, B01111111, B11111100, B00111100, B00000111, B10000001, B11111110, B00000000, B00001111,
  B01111111, B11111111, B00111000, B00111110, B00000011, B11100000, B00000011, B11100000, B01111111, B11111111, B00111100, B00000111, B10000111, B11111111, B11000000, B00001111,
  B11111111, B11111111, B00111000, B01111100, B00000111, B11110000, B00000111, B11100000, B01111111, B11111111, B10111100, B00000111, B10011111, B11001111, B11100000, B00001111,
  B11111000, B00000000, B00111000, B11111000, B00000111, B11110000, B00001111, B11110000, B01111000, B00001111, B10111100, B00000111, B10011110, B00000011, B11100000, B00001111,
  B11111000, B00000000, B00111001, B11110000, B00001111, B01111000, B00001111, B01110000, B01111000, B00000111, B10111100, B00000111, B10111110, B00000001, B11100000, B00001111,
  B11111111, B11111100, B00111011, B11100000, B00001111, B01111000, B00011110, B01111000, B01111000, B00011111, B10111111, B11111111, B10111100, B00000001, B11110000, B00001111,
  B01111111, B11111111, B00111111, B11000000, B00011110, B00111100, B00011110, B00111100, B01111111, B11111111, B00111111, B11111111, B10111100, B00000001, B11110000, B00001111,
  B00011111, B11111111, B10111011, B11100000, B00011110, B00111100, B00111110, B00111100, B01111111, B11111110, B00111111, B11111111, B10111100, B00000001, B11110000, B00001111,
  B00000000, B00001111, B10111001, B11110000, B00111111, B11111110, B00111111, B11111110, B01111000, B01111100, B00111100, B00000111, B10111110, B00000001, B11100000, B00001111,
  B00000000, B00001111, B10111000, B11111000, B00111111, B11111110, B01111111, B11111110, B01111000, B00111110, B00111100, B00000111, B10011110, B00000001, B11100000, B00011111,
  B01111111, B11111111, B10111000, B01111100, B01111000, B00001111, B01111000, B00001111, B01111000, B00011111, B00111100, B00000111, B10011111, B10000111, B11000000, B00111110,
  B01111111, B11111111, B00111000, B00111110, B01111000, B00001111, B11110000, B00001111, B01111000, B00001111, B10111100, B00000111, B10001111, B11111111, B10011111, B11111110,
  B01111111, B11111100, B00111000, B00011111, B11110000, B00000111, B11110000, B00000111, B11111000, B00000111, B10111100, B00000111, B10000001, B11111110, B00011111, B11110000,
};
#endif



/************************************
 *
 * STANDARD FUNCTIONS and VARIABLES
 *
 ************************************/

void(* resetFunc) (void) = 0; //declare reset function @ address 0

// MAC address and IP address for this *particular* SKAARDUINO
byte mac[6] = {};   // Loaded from EEPROM
IPAddress ip(192, 168, 10, 99);  // This is the default IP address in case deep-configuration mode is entered
IPAddress subnet(255, 255, 255, 0); // This is the default subnet address

#define RED 1
#define GREEN 2
#define BLUE 3
#define WHITE 4
#define PURPLE 7
#define OFF 0
#define QUICKBLANK 254
#define DONTCHANGE 255


/**
 * Reading serial buffer for commands
 */
#define SER_BUFFER_SIZE 30
char serialBuffer[SER_BUFFER_SIZE];
uint8_t serialBufferPointer = 255;
bool loadSerialCommandToCRNL()  {  // Call in loop() to check for commands
  if (serialBufferPointer == 255)  { // Initialization
    serialBuffer[0] = 0; // Null-terminate empty buffer
    serialBufferPointer = 0;
  }

  while (Serial.available())  {
    char c = Serial.read();
    if (c == 10 || c == 13) {
      serialBufferPointer = 0; // so, we can start over again filling the buffer
      return true;
    } else {
      if (serialBufferPointer < SER_BUFFER_SIZE - 1)    {   // one byte for null termination reserved
        serialBuffer[serialBufferPointer] = c;
        serialBuffer[serialBufferPointer + 1] = 0;
        serialBufferPointer++;
      }
      else {
        // Serial.println(F("ERROR: Buffer overflow."));
      }
    }
  }

  return false;
}

/**
 * Check incoming serial commands
 */
bool checkIncomingSerial()  {
  if (loadSerialCommandToCRNL())  {
      if (!strncmp(serialBuffer, "IP=", 3))  {
        char *tmp;
        int i = 0;
        tmp = strtok(serialBuffer + 3, ".");
        while (tmp && i < 4) {
          EEPROM.write(i + 2, atoi(tmp));
          i++;
          tmp = strtok(NULL, ".");
        }
        Serial << F("IP address updated to: ");
        for (uint8_t i = 0; i < 4; i++) {
          Serial << EEPROM.read(i + 2) << (i != 3 ? F(".") : F("\n"));
        }
    } else if (!strncmp(serialBuffer, "Subnet=", 7))  {
      char *tmp;
      int i = 0;
      tmp = strtok(serialBuffer + 7, ".");
      while (tmp && i < 4) {
        EEPROM.write(i + 6, atoi(tmp));
        i++;
        tmp = strtok(NULL, ".");
      }
      Serial << F("Subnet mask updated to: ");
      for (uint8_t i = 0; i < 4; i++) {
        Serial << EEPROM.read(i + 6) << (i != 3 ? F(".") : F("\n"));
      }
    } else if (!strncmp(serialBuffer, "newmac", 6))  {
		EEPROM.write(10, 0x90);
		EEPROM.write(11, 0xA1);
		EEPROM.write(12, 0xDA);
		for(uint8_t i=0; i<3;i++)	{
			EEPROM.write(13+i, random(0, 256));
		}
      Serial << F("MAC address updated to: ");
      for (uint8_t i = 0; i < 6; i++) {
        Serial << _HEXPADL(EEPROM.read(i + 10), 2, "0") << (i != 5 ? F(":") : F("\n"));
      }
    } else if (!strncmp(serialBuffer, "debug", 5))  {
      EEPROM.write(1, true);
      Serial << F("Resetting in debug mode...\n");
      delay(1000);
      resetFunc();
    } else if (!strncmp(serialBuffer, "configd", 7))  {
      EEPROM.write(0, 2);
      Serial << F("Resetting in config default mode...\n");
      delay(1000);
      resetFunc();
    } else if (!strncmp(serialBuffer, "config", 6))  {
      EEPROM.write(0, 1);
      Serial << F("Resetting in config mode...\n");
      delay(1000);
      resetFunc();
    } else if (!strncmp(serialBuffer, "reset", 5))  {
      Serial << F("Resetting...\n");
      delay(1000);
      resetFunc();
    } else {
      Serial << F("NAK\n");
    }
  }

  return false;
}

/**
 * StatusLED function. Call it without parameters to just update the LED flashing. Call it with parameters to set a new value.
 */
void statusLED(uint8_t incolor = 255, uint8_t inblnk = 255)    {
  static uint8_t color = 0;
  static uint8_t blnk = 0;

  if (incolor < 254) {
    color = incolor;
    blnk = 0;
  }
  if (inblnk != 255) {
    blnk = inblnk;
  }

  for (uint8_t i = 0; i < (incolor == QUICKBLANK ? 2 : 1); i++)   {
    if ((blnk == 0 || ((millis() & (2048 >> blnk)) > 0)) && !(incolor == QUICKBLANK && i == 0))    {
      switch (color)   {
        case 1: //  red
          digitalWrite(13, 0);   // Red
          digitalWrite(15, 1);   // Green
          digitalWrite(14, 1);   // Blue
          break;
        case 2: //  green
          digitalWrite(13, 1);   // Red
          digitalWrite(15, 0);   // Green
          digitalWrite(14, 1);   // Blue
          break;
        case 3: //  blue
          digitalWrite(13, 1);   // Red
          digitalWrite(15, 1);   // Green
          digitalWrite(14, 0);   // Blue
          break;
        case 4: //  white
          digitalWrite(13, 0);   // Red
          digitalWrite(15, 0);   // Green
          digitalWrite(14, 0);   // Blue
          break;
        case 5: //  yellow
          digitalWrite(13, 0);   // Red
          digitalWrite(15, 0);   // Green
          digitalWrite(14, 1);   // Blue
          break;
        case 6: //  cyan
          digitalWrite(13, 1);   // Red
          digitalWrite(15, 0);   // Green
          digitalWrite(14, 0);   // Blue
          break;
        case 7: //  purple
          digitalWrite(13, 0);   // Red
          digitalWrite(15, 1);   // Green
          digitalWrite(14, 0);   // Blue
          break;
        default: //  off
          digitalWrite(13, 1);   // Red
          digitalWrite(15, 1);   // Green
          digitalWrite(14, 1);   // Blue
          break;
      }
    } else {
      digitalWrite(13, 1);   // Red
      digitalWrite(15, 1);   // Green
      digitalWrite(14, 1);   // Blue
    }
    if (incolor == QUICKBLANK)    {
      delay(200);
    }
  }
}

/**
 * Use this to make delays (because it will keep important communication running as you wait)
 */
void statusSerial() {
  static unsigned long timer = millis();
  static uint8_t printsSinceLastLinebreak = 0;
  if (sTools.hasTimedOut(timer, 1000, true))  {
    Serial << ".";
    printsSinceLastLinebreak++;
    if (printsSinceLastLinebreak >= 60)   { // TODO?: We could consider to rather make a line break whenever a minute has passed. If then less than 60 dots is printed, we know the loop() function has been stalled for longer than a second at some point.
      printsSinceLastLinebreak = 0;
      Serial << F("\n");
    }
  }
}

/**
 * Set up devices
 */
uint8_t deviceMap[sizeof(deviceArray)];	// Maps indexes of the individual device arrays
uint8_t deviceEn[sizeof(deviceArray)] = {0,1,0,0,0,0,0,0,0};	// Sets the enabled-flag for the devices.
uint8_t deviceReady[sizeof(deviceArray)];	// Flag that lets the system know if a device is connected and ready for commands.

void deviceSetup()	{	// Call ONLY once - or we will overflow memory...
	
    //W5100.setRetransmissionTime(200);  // Milli seconds
    //W5100.setRetransmissionCount(2);
	IPAddress switcherIp(192, 168, 10, 240);

    for (uint8_t a = 1; a < sizeof(deviceArray); a++)   {
	  deviceReady[a] = 0;
	  Serial << "Setup HW#" << a << "\n";
      switch (pgm_read_byte_near(deviceArray + a)) {
        case SK_DEV_ATEM:
  #if SK_DEVICES_ATEM
		// TODO: Load "enabled" + "IP" from config
 	   if (deviceEn[a]) {
		   Serial << ": ATEM" << AtemSwitcher_initIdx << "\n";
		  deviceMap[a] = AtemSwitcher_initIdx++;
          AtemSwitcher[deviceMap[a]].begin(switcherIp);
		  AtemSwitcher[deviceMap[a]].serialOutput(SK_SERIAL_OUTPUT);
	   } else {
		   Serial << ": ATEM Skipped\n";
	   }
  #endif
          break;
        case SK_DEV_HYPERDECK:
  #if SK_DEVICES_HYPERDECK
		// TODO: Load "enabled" + "IP" from config
	   if (deviceEn[a]) {
		  deviceMap[a] = HyperDeck_initIdx++;
          HyperDeck[deviceMap[a]].begin(switcherIp);
		  HyperDeck[deviceMap[a]].serialOutput(SK_SERIAL_OUTPUT);
	   }
  #endif
          break;
        case SK_DEV_VIDEOHUB:
  #if SK_DEVICES_VIDEOHUB
		// TODO: Load "enabled" + "IP" from config
 	   if (deviceEn[a]) {
		  deviceMap[a] = VideoHub_initIdx++;
          VideoHub[deviceMap[a]].begin(switcherIp);
		  VideoHub[deviceMap[a]].serialOutput(SK_SERIAL_OUTPUT);
	   }
  #endif
          break;
        case SK_DEV_SMARTSCOPE:
  #if SK_DEVICES_SMARTSCOPE
		// TODO: Load "enabled" + "IP" from config
 	   if (deviceEn[a]) {
		  deviceMap[a] = SmartView_initIdx++;
          SmartView[deviceMap[a]].begin(switcherIp);
		  SmartView[deviceMap[a]].serialOutput(SK_SERIAL_OUTPUT);
	   }
  #endif
          break;
      }
    }	
}

/**
 * Device run loop
 */
void deviceRunLoop()	{
    // Keeping up the connections:
    for (uint8_t a = 1; a < sizeof(deviceArray); a++)   {
      if (deviceEn[a])    {
        switch (pgm_read_byte_near(deviceArray + a)) {
          case SK_DEV_ATEM:
#if SK_DEVICES_ATEM
            AtemSwitcher[deviceMap[a]].runLoop();
			deviceReady[a] = AtemSwitcher[deviceMap[a]].hasInitialized();
			
#endif
            break;
          case SK_DEV_HYPERDECK:
#if SK_DEVICES_HYPERDECK
            HyperDeck[deviceMap[a]].runLoop();
#endif
            break;
          case SK_DEV_VIDEOHUB:
#if SK_DEVICES_VIDEOHUB
            VideoHub[deviceMap[a]].runLoop();
#endif
            break;
          case SK_DEV_SMARTSCOPE:
#if SK_DEVICES_SMARTSCOPE
            SmartView[deviceMap[a]].runLoop();
#endif
            break;
        }
      }
    }
}

/**
 * Use this to make delays (because it will keep important communication running as you wait)
 */
void lDelay(uint16_t delayVal)  {
  unsigned long timer = millis();
  do {
    statusLED();
    statusSerial();
	deviceRunLoop();
  }
  while (delayVal > 0 && !sTools.hasTimedOut(timer, delayVal));
}

/**
 * Check if config/reset button is pressed
 */
bool isConfigButtonPushed() {
  return (!digitalRead(18)) || (EEPROM.read(0) != 0);
}

/**
 * Returns config mode
 */
uint8_t configMode = 0;
uint8_t getConfigMode() {
  return configMode;
}

/**
 * Returns default color for given run mode
 */
uint8_t defaultStatusColor()    {
  return getConfigMode() == 1 ? BLUE : (getConfigMode() == 2 ? WHITE : GREEN);
}

/**
 * Returns index to position in configuration array where a given Hardware Component state behaviour is described.
 */
uint16_t getConfigMemIndex(uint8_t HWcIdx, uint8_t stateIdx = 0)   {

  uint16_t ptr = 2;
  int HWcIndex = -1;

  while (ptr < SK_CONFIG_MEMORY_SIZE && globalConfigMem[ptr] != 255)    {   // Traverses HW components
    uint8_t HWcSegmentLength = globalConfigMem[ptr];
    uint16_t HWcSegmentStartPtr = ptr;
    HWcIndex++;
    int stateIndex = -1;

    if (HWcIdx == HWcIndex) {   // Found it...
      ptr++;
      while (ptr < HWcSegmentStartPtr + HWcSegmentLength + 1)   {   // Traverses state index
        uint8_t stateSegmentLength = globalConfigMem[ptr];
        uint16_t stateSegmentStartPtr = ptr;
        stateIndex++;

        if (stateIdx == stateIndex)   {   // If state index matches, return either state behaviour configuration - or the default (Normal) behaviour
          return stateSegmentLength > 0 ? stateSegmentStartPtr + 1 : HWcSegmentStartPtr + 2;
        } else {
          ptr += stateSegmentLength+1;
        }
      }
      
      return HWcSegmentStartPtr + 2;
    } else {
      ptr += HWcSegmentLength+1;
    }
  }
  return 0;
}

/**
 * Returns index to position in configuration array where the controller IP and subnet mask is stored
 */
uint16_t getConfigMemIPIndex()   {
	uint16_t ptr = globalConfigMem[0];
	ptr = (ptr << 8) | globalConfigMem[1];
	return ptr + 2;
}

/**
 * Returns index to position in configuration array where a given device is configured
 */
uint16_t getConfigMemDevIndex(uint8_t devNum)	{
	uint16_t ptr = globalConfigMem[0];
	return ((ptr << 8) | globalConfigMem[1])+2+8+1+devNum*5;
}

/**
 * Returns index to position in configuration array where number of states is stored (and subsequently labels are stored)
 */
uint16_t getConfigMemStateIndex()	{
	uint16_t ptr = globalConfigMem[0];
	return ((ptr << 8) | globalConfigMem[1])+2+8+1+(SK_DEVICES*5)+1;
}

/**
 * Hardware Setup
 */
bool HWsetup()  {
	bool retVal = false;
  // ++++++++++++++++++++++
  // HW setup: E21-TVS
  // ++++++++++++++++++++++
#if SK_MODEL==SK_E21TVS || SK_MODEL==SK_MICROBI16
  Serial << F("Init BI16 board\n");
#if SK_MODEL==SK_MICROBI16
  buttons.begin(0);
#else
  buttons.begin(1);
#endif
  buttons.setDefaultColor(3);  // Dimmed by default
  buttons.setButtonColorsToDefault();
  if (getConfigMode())    {
    Serial << F("Test: BI16 board color sequence\n");
    buttons.testSequence();
  }
  if (buttons.buttonIsPressedAll()>0)	retVal=true;
  statusLED(QUICKBLANK);
#endif

  // ++++++++++++++++++++++
  // HW setup: E21-CMB6
  // ++++++++++++++++++++++
#if (SK_MODEL == SK_E21CMB6)
  Serial << F("Init BI8 boards\n");
  buttons.begin(1, false, true);
  buttons.setDefaultColor(0);  // Off by default
  buttons2.begin(2, false, true);
  buttons2.setDefaultColor(0);  // Off by default
  if (getConfigMode())    {
    Serial << F("Test: BI8 Boards\n");
    buttons.testSequence(10);
    buttons2.testSequence(10);
  } else delay(500);
  buttons.setButtonColorsToDefault();
  buttons2.setButtonColorsToDefault();
  statusLED(QUICKBLANK);
#endif

  // ++++++++++++++++++++++
  // HW setup: C15
  // ++++++++++++++++++++++
#if (SK_MODEL == SK_C15)
  Serial << F("Init BI8 boards\n");
  buttons.begin(0, false, false);
  buttons.setDefaultColor(0);  // Off by default
  if (getConfigMode())    {
    Serial << F("Test: BI8 Board\n");
    buttons.testSequence(10);
  } else delay(500);
  buttons.setButtonColorsToDefault();
  statusLED(QUICKBLANK);
#endif

  // ++++++++++++++++++++++
  // HW setup: C90, C90A, C90SM
  // ++++++++++++++++++++++
#if (SK_MODEL == SK_C90 || SK_MODEL == SK_C90A || SK_MODEL == SK_C90SM)
  Serial << F("Init BI8 boards\n");
  buttons.begin(0, false, true);
  buttons.setDefaultColor(0);  // Off by default
  buttons2.begin(1, false, true);
  buttons2.setDefaultColor(0);  // Off by default
  if (getConfigMode())    {
    Serial << F("Test: BI8 Boards\n");
    buttons.testSequence(10);
    buttons2.testSequence(10);
  } else delay(500);
  buttons.setButtonColorsToDefault();
  buttons2.setButtonColorsToDefault();
  statusLED(QUICKBLANK);
#endif



  // ++++++++++++++++++++++
  // Generic HW feature
  // ++++++++++++++++++++++
#if (SK_HWEN_STDOLEDDISPLAY)
  Serial << F("Init Info OLED Display\n");
  infoDisplay.begin(0, 1);
  for (uint8_t i = 0; i < 8; i++) {
    infoDisplay.clearDisplay();
    infoDisplay.drawBitmap(0, -7 + i,  SKAARHOJLogo, 128, 13, 1, false);
    infoDisplay.display(B1);  // Write to all
  }
  infoDisplay.setTextSize(1);
  infoDisplay.setTextColor(1, 0);
  infoDisplay.setCursor(40, 24);
  infoDisplay << "E21-CMB6";
  infoDisplay.display(B1);  // Write to all
  statusLED(QUICKBLANK);
#endif
#if (SK_HWEN_SLIDER)
  Serial << F("Init slider\n");
  slider.uniDirectionalSlider_init();
  slider.uniDirectionalSlider_hasMoved();
  statusLED(QUICKBLANK);
#endif
#if (SK_HWEN_JOYSTICK)
  Serial << F("Init Joystick\n");
  joystick.joystick_init(10, 0);
  statusLED(QUICKBLANK);
#endif
#if (SK_HWEN_MENU)
  Serial << F("Init Encoder Menu\n");
  menuDisplay.begin(4, 0, 3); // DOGM163
  menuDisplay.cursor(false);
  menuDisplay.print("SKAARHOJ");
  menuEncoders.begin(5);
  menuEncoders.serialOutput(0);
  statusLED(QUICKBLANK);
#endif
#if (SK_HWEN_SSWMENU)
  // TODO: SSW/Encoder library used when made!!
  Serial << F("Init SmartSwitch Menu\n");
#if (SK_MODEL == SK_C90SM)
  SSWmenu.begin(2);
#else
  SSWmenu.begin(0);
#endif
  SSWmenu.setRotation(2);
  SSWmenu.setButtonBrightness(7, B00010000);
  SSWmenu.setButtonBrightness(7, B00010000);
  SSWmenu.setButtonColor(0, 2, 3, B00010000);
  for (uint8_t i = 0; i < 64; i++) {
    SSWmenu.clearDisplay();
    SSWmenu.drawBitmap(64 - i - 1, 0,  welcomeGraphics[0], 64, 32, 1, true);
    SSWmenu.display(B00010000);  // Write
  }
  statusLED(QUICKBLANK);
#endif
#if (SK_HWEN_SSWBUTTONS)
  Serial << F("Init SmartSwitch Buttons\n");
  SSWbuttons.begin(1);  // SETUP: Board address
  SSWbuttons.setRotation(0);
  SSWbuttons.setButtonBrightness(7, B11);
  SSWbuttons.setButtonBrightness(7, B11);
  SSWbuttons.setButtonColor(0, 2, 3, B11);
  for (uint8_t i = 0; i < 64; i++) {
    SSWbuttons.clearDisplay();
    SSWbuttons.drawBitmap(64 - i - 1, 0,  welcomeGraphics[0], 64, 32, 1, true);
    SSWbuttons.display(B10);  // Write
    SSWbuttons.clearDisplay();
    SSWbuttons.drawBitmap(-(64 - i - 1), 0,  welcomeGraphics[1], 64, 32, 1, true);
    SSWbuttons.display(B01);  // Write
  }
  statusLED(QUICKBLANK);
#endif
#if (SK_HWEN_ACM)
  Serial << F("Init Audio Master Control\n");
  AudioMasterControl.begin(3, 0);
  AudioMasterControl.setIsMasterBoard();

  if (getConfigMode())    {
    Serial << F("Test: AudioMasterControl\n");
    for (int j = 0; j < 5; j++) {
      AudioMasterControl.setButtonLight(j + 1, true);
      delay(100);
    }
    for (int j = 0; j < 5; j++) {
      AudioMasterControl.setButtonLight(j + 1, false);
      delay(100);
    }
    for (int i = 1; i <= 2; i++) {
      for (int j = 0; j < 4; j++) {
        AudioMasterControl.setChannelIndicatorLight(i, j);
        delay(100);
      }
    }
    AudioMasterControl.setChannelIndicatorLight(1, 0);
    AudioMasterControl.setChannelIndicatorLight(2, 0);
    for (uint16_t i = 0; i <= 16445; i += 100) {
      AudioMasterControl.VUmeter(16445 - i, i);
    }
    AudioMasterControl.VUmeter(16445, 16445);
    delay(500);
    AudioMasterControl.VUmeter(0, 0);
  } else delay(500);
  statusLED(QUICKBLANK);
#endif
#if (SK_HWEN_GPIO)
  Serial << F("Init GPIO board\n");
  GPIOboard.begin(0);

  if (getConfigMode())    {
    Serial << F("Test: GPIO relays\n");
    // Set:
    for (int i = 1; i <= 8; i++)  {
      GPIOboard.setOutput(i, HIGH);
      delay(100);
    }
    // Clear:
    for (int i = 1; i <= 8; i++)  {
      GPIOboard.setOutput(i, LOW);
      delay(100);
    }
  }
  statusLED(QUICKBLANK);
#endif


  return retVal;
}

/**
 * Hardware Test
 */
void HWtest()   {
  bool displayWritten = false;
  if (millis() > 120000 || getConfigMode() == 2) {
#if (SK_MODEL == SK_E21TVS)
    buttons.testProgramme(0xFFFF);
#endif
#if (SK_MODEL == SK_E21CMB6)
    buttons.testProgramme(0x9F5F);
    buttons2.testProgramme(0xFFEF);
#endif
#if (SK_MODEL == SK_C90 || SK_MODEL == SK_C90A || SK_MODEL == SK_C90SM)
    buttons.testProgramme(0xFF);
    buttons2.testProgramme(B11001101);
#endif
#if (SK_MODEL == SK_C15)
#if (SK_MODELVAR == 1)  //C15 with 2+8 buttons
    buttons.testProgramme(0x3FF);
#else
    buttons.testProgramme(B11001111);
#endif
#endif

#if (SK_HWEN_STDOLEDDISPLAY)
    if ((millis() & 0x1800) == 0x0800)  {
      infoDisplay.testProgramme(B1);
      displayWritten = true;
    }
#endif
#if (SK_HWEN_SLIDER)
    if (slider.uniDirectionalSlider_hasMoved())  {
      Serial << F("New position ") << slider.uniDirectionalSlider_position() << "\n";
      if (slider.uniDirectionalSlider_isAtEnd())  {
        Serial << F("Slider at end\n");
      }
    }
#endif
#if (SK_HWEN_JOYSTICK)
    if (joystick.joystick_hasMoved(0) || joystick.joystick_hasMoved(1))  {
      Serial << F("New joystick position:") << joystick.joystick_position(0) << "," << joystick.joystick_position(1) << "\n";
    }
    if (joystick.joystick_buttonUp())  {
      Serial << F("Joystick Button Up") << "\n";
    }
    if (joystick.joystick_buttonDown())  {
      Serial << F("Joystick Button Down") << "\n";
    }
    if (joystick.joystick_buttonIsPressed())  {
      Serial << F("Joystick Button Pressed") << "\n";
    }
#endif
#if (SK_HWEN_SSWMENU)
    SSWmenu.testProgramme(B00010000);
#endif
#if (SK_HWEN_SSWBUTTONS)
    SSWbuttons.testProgramme(B11111111);    // TODO: Fetch from ....
#endif
    delay(40);
  }

#if (SK_HWEN_STDOLEDDISPLAY)
  if (!displayWritten)    {
    infoDisplay.clearDisplay();
    infoDisplay.fillRoundRect(0, 0, 128, 11, 1, 1);
    infoDisplay.setTextSize(1);
    infoDisplay.setTextColor(0, 1);
    infoDisplay.setCursor(10, 2);
    infoDisplay << F("Configuration Mode");
    infoDisplay.setTextColor(1, 0);
    infoDisplay.setCursor(0, 14);
    infoDisplay << "http://" << ip;
    infoDisplay.setCursor(95, 24);
    infoDisplay << _DECPADL((millis() / 1000) / 60, 2, "0") << (((millis() / 500) % 2) == 1 ? F(" ") : F(":")) << _DECPADL((millis() / 1000) % 60, 2, "0");
    infoDisplay.display(B1);  // Write to all
  }
#endif
}




















void initActionCache()  {
  memset(_systemHWcActionCache, 0, SK_HWCCOUNT * SK_MAXACTIONS);    // 16 bit - does it clear it all???
  memset(_systemHWcActionCacheFlag, 0, SK_HWCCOUNT * SK_MAXACTIONS);
}

uint16_t evaluateAction_system(const uint16_t actionPtr, const uint8_t HWc, const uint8_t actIdx, const bool actDown = false, const bool actUp = false, const uint8_t pulses = 0, const uint16_t value = 0)    {
#if SK_SERIAL_OUTPUT
  if (actDown || actUp) {
    Serial << "System action " << globalConfigMem[actionPtr] << ", " << actDown << ", " << actUp << "\n";
  }
#endif
  switch (globalConfigMem[actionPtr])   {
    case 0: // Cycle States
      if (actDown)   {
        _systemState = (_systemState + 1) % (sTools.shapeInt(globalConfigMem[actionPtr + 1], 1, 9) + 1);
        initActionCache();
		Serial << F("State: ") << _systemState << F("\n");
      }
	  if (_systemState==0)	{
		  return 5;
	  } else {
		  if ((millis()&256)>0)	{
			  if (!_systemHWcActionCacheFlag[HWc][actIdx])	{
				  _systemHWcActionCache[HWc][actIdx] = (_systemHWcActionCache[HWc][actIdx]+1)%(_systemState+1);
			  }
			  _systemHWcActionCacheFlag[HWc][actIdx] = true;
			  return _systemHWcActionCache[HWc][actIdx]>0 ? 4 : 5;
		  } else {
		  	_systemHWcActionCacheFlag[HWc][actIdx] = false;
			return 5;
		  }
	  }
      break;
    case 1: // Set State
      if (actDown)    {
        _systemPrevState = _systemState;
        _systemState = sTools.shapeInt(globalConfigMem[actionPtr + 1], 0, 9);
        initActionCache();
		Serial << F("State: ") << _systemState << F("\n");
      }
      if (actUp && globalConfigMem[actionPtr + 2])    { // globalConfigMem[actionPtr + 1] == "Hold Down"
        _systemState = _systemPrevState;
		Serial << F("State: ") << _systemState << F("\n");
      }
	  return _systemState == globalConfigMem[actionPtr + 1] ? 4 : 5;
      break;
    case 2: // Cycle Shift
      if (actDown)    {
		  _systemShift = (_systemShift + 1) % (sTools.shapeInt(globalConfigMem[actionPtr + 1], 1, SK_MAXACTIONS - 1) + 1);
	  	Serial << F("Shift: ") << _systemShift << F("\n");
	  }
      break;
    case 3: // Set Shift
      if (actDown)    {
        _systemHWcActionCache[HWc][actIdx] = _systemShift;
        _systemShift = sTools.shapeInt(globalConfigMem[actionPtr + 1], 0, SK_MAXACTIONS - 1);
	  	Serial << F("Shift: ") << _systemShift << F("\n");
      }
      if (actUp && globalConfigMem[actionPtr + 2])    { // globalConfigMem[actionPtr + 1] == "Hold Down"
        _systemShift = _systemHWcActionCache[HWc][actIdx];
	  	Serial << F("Shift: ") << _systemShift << F("\n");
      }
	  return _systemShift == globalConfigMem[actionPtr + 1] ? 4 : 5;
      break;
    case 4: // Wait
      if (actDown)    lDelay(sTools.shapeInt(globalConfigMem[actionPtr + 1], 0, 20) * 100);
      break;
    case 5: // Set Memory
      if (actDown)    {
        if (globalConfigMem[actionPtr + 1] < 4) {
          _systemMem[globalConfigMem[actionPtr + 1]] = sTools.shapeInt(globalConfigMem[actionPtr + 2], 0, 100);
		  Serial << "Mem " << globalConfigMem[actionPtr + 1] << ": " << _systemMem[globalConfigMem[actionPtr + 1]] << "\n";
        }
		_systemHWcActionCacheFlag[HWc][actIdx] = true;
      }
      if (actUp)   {
  	    _systemHWcActionCacheFlag[HWc][actIdx] = false;
      }
  	  return _systemHWcActionCacheFlag[HWc][actIdx] ? 4 : 5;
      break;
    case 6: // Cycle Memory
      if (actDown)    {
        if (globalConfigMem[actionPtr + 1] < 4) {
          _systemMem[globalConfigMem[actionPtr + 1]]++;
          if (_systemMem[globalConfigMem[actionPtr + 1]] > globalConfigMem[actionPtr + 3])   _systemMem[globalConfigMem[actionPtr + 1]] = globalConfigMem[actionPtr + 2];
		  	Serial << "Mem " << globalConfigMem[actionPtr + 1] << ": " << _systemMem[globalConfigMem[actionPtr + 1]] << "\n";
        }
		_systemHWcActionCacheFlag[HWc][actIdx] = true;
      }
      if (actUp)   {
    	  _systemHWcActionCacheFlag[HWc][actIdx] = false;
      }
	  return _systemHWcActionCacheFlag[HWc][actIdx] ? 4 : 5;
      break;
  }
  return 0;
}





uint16_t actionDispatch(const uint8_t HWcNum, const bool actDown = false, const bool actUp = false, const uint8_t pulses = 0, const uint16_t value = 0)    {
  if (HWcNum > SK_HWCCOUNT || HWcNum == 0)    return 0;   // Invalid values check - would leak memory if not checked.
  
  uint16_t stateBehaviourPtr = getConfigMemIndex(HWcNum-1, _systemState);   // Fetching pointer to state behaviour 
  uint16_t retValue = 0; // Returns zero by default (inactive element)
  uint16_t retValueT = 0; // Preliminary return value
  if (stateBehaviourPtr > 0)  { // If there is some configuration to relate to....
    uint8_t stateLen = globalConfigMem[stateBehaviourPtr - 1];  
    bool shiftLevelMatch = false;
    uint8_t matchShiftValue = _systemShift;

    // Traverse actions in state behaviour
    while (!shiftLevelMatch)   {
      uint8_t sShift = 0;
      uint8_t lptr = 0;
      uint8_t actIdx = 0;
      while (lptr < stateLen)    {
        if (actIdx >= SK_MAXACTIONS)    break;  // actIdx over SK_MAXACTIONS would result in memory leaks in various evaluation functions which would trust HWc and actIdx to not exceed the bounds of the _systemHWcActionCache array
        if (lptr > 0 && (globalConfigMem[stateBehaviourPtr + lptr] & 16) > 0)  sShift++; // If a shift divider is found (cannot be the first element)
        if (matchShiftValue == sShift)   {
          shiftLevelMatch = true;
          // Traverse actions in shift level:

          uint8_t devIdx = globalConfigMem[stateBehaviourPtr + lptr] & 15;
          if (devIdx > 0 && devIdx < sizeof(deviceArray) && deviceEn[devIdx])   {
            if (deviceEn[devIdx] && deviceReady[devIdx])   {
              switch (pgm_read_byte_near(deviceArray + devIdx)) {
                case SK_DEV_ATEM:
#if SK_DEVICES_ATEM
                  //       Serial << "ATEM action " << globalConfigMem[stateBehaviourPtr + lptr + 1] << "\n";
                  retValueT = evaluateAction_ATEM(deviceMap[devIdx], stateBehaviourPtr + lptr + 1, HWcNum-1, actIdx, actDown, actUp, pulses, value);
                  if (retValue==0)  retValue = retValueT;   // Use first ever return value in case of multiple actions.
#endif
                  break;
                case SK_DEV_HYPERDECK:
#if SK_DEVICES_HYPERDECK
                  //       Serial << "HyperDeck action " << globalConfigMem[stateBehaviourPtr + lptr + 1] << "\n";
#endif
                  break;
                case SK_DEV_VIDEOHUB:
#if SK_DEVICES_VIDEOHUB
                  Serial << "VideoHub action " << globalConfigMem[stateBehaviourPtr + lptr + 1] << "\n";
#endif
                  break;
                case SK_DEV_SMARTSCOPE:
#if SK_DEVICES_SMARTSCOPE
                  Serial << "SmartScope action " << globalConfigMem[stateBehaviourPtr + lptr + 1] << "\n";
#endif
                  break;
              }
            } else {
              //Serial << "Device disabled!\n";
            }
          } else if (devIdx == 15)    {
            retValue = evaluateAction_system(stateBehaviourPtr + lptr + 1, HWcNum-1, actIdx, actDown, actUp, pulses, value);
          }
        } else if (matchShiftValue < sShift)   {
          break;
        }

        lptr += (globalConfigMem[stateBehaviourPtr + lptr] >> 5) + 2; // Add length
        actIdx++;
      }
      if (!shiftLevelMatch)   {
        matchShiftValue = 0;
      }
    }
  }

  deviceRunLoop();
  
  return retValue;
}









// Preset memory: [NumberofPresets][CurrentPreset][PresetStoreLen-MSB][PresetStoreLen-LSB][CSC of previous 4 bytes + token][Preset1][Preset2][Preset...][PresetX], where [PresetX] = [Len-MSB][Len-LSB][Byte1][Byte2][Byte...][ByteX][CSC of bytes + token]
// On a global level, we can verify the preset store by checking if a traversal of the presets and lengths will match the global length
// On an individual level, presets validity is confirmed by the xor-checksum byte stored prior to the preset data
#define EEPROM_PRESET_START 100
#define EEPROM_PRESET_TOKEN 0xf3	// Just some random value that is used for a checksum offset. Change this and existing configuration will be invalidated and has to be rewritten.
void loadDefaultConfig()	{
    // Copy default controller configuration to the global Config memory.
    memcpy_P(globalConfigMem, defaultControllerConfig, sizeof(defaultControllerConfig));
}
void moveEEPROMMemoryBlock(uint16_t from, uint16_t to, int offset)	{	// From is inclusive, To is exclusive, To>From, offset>0 = move forward
//	Serial << "moveEEPROMMemoryBlock (" << from << "," << to << "," << offset << " )\n";
	if (offset>0)	{
		for(uint16_t a=to; a>from; a--)	{
			EEPROM.write(a-1+offset,EEPROM.read(a-1));
		}
	} else if (offset<0) {
		for(uint16_t a=from; a<to; a++)	{
			EEPROM.write(a+offset,EEPROM.read(a));
		}
	}
}
uint16_t getPresetOffsetAddress(uint8_t presetNum)	{
	uint16_t offset = 0;
	for(uint8_t a=1; a<presetNum; a++)	{
		offset+= ((uint16_t)EEPROM.read(EEPROM_PRESET_START+5+offset)<<8) | EEPROM.read(EEPROM_PRESET_START+6+offset) + 2;	// Length of stored preset + 2 for addressing
	}
	return offset;
}
void updatePresetChecksum()	{
	uint8_t csc = EEPROM_PRESET_TOKEN;
	for(uint8_t a=0; a<4; a++)	{
		csc^=EEPROM.read(EEPROM_PRESET_START+a);
	}
//	Serial << "Writing CSC:" << csc << "\n";
	EEPROM.write(EEPROM_PRESET_START+4, csc);
}
void setNumberOfPresets(uint8_t n)	{
	EEPROM.write(EEPROM_PRESET_START,n);	// Number of...
//	Serial << "setNumberOfPresets:" << n << "\n";
	updatePresetChecksum();
}
void setCurrentPreset(uint8_t n)	{
	EEPROM.write(EEPROM_PRESET_START+1,n);	// Current
//	Serial << "setCurrentPreset:" << n << "\n";
	updatePresetChecksum();
}
void setPresetStoreLength(uint16_t len)	{
	EEPROM.write(EEPROM_PRESET_START+2,highByte(len));
	EEPROM.write(EEPROM_PRESET_START+3,lowByte(len));
//	Serial << "setPresetStoreLength:" << len << "\n";
	updatePresetChecksum();
}
uint8_t getNumberOfPresets()	{
	uint8_t csc = EEPROM_PRESET_TOKEN;
	for(uint8_t a=0; a<5; a++)	{
		csc^=EEPROM.read(EEPROM_PRESET_START+a);
	}
	if (csc!=0)	{
//		Serial << "CSC mismatch:"<<csc<<"\n";
		setNumberOfPresets(0);
		setCurrentPreset(0);
		setPresetStoreLength(0);
	}

	return EEPROM.read(EEPROM_PRESET_START);
}
bool loadPreset(uint8_t presetNum=0)	{
//	Serial << "Load preset: " << presetNum << "\n";
	if (getNumberOfPresets()>0)	{	// If there are valid presets, consider to load one...
//		Serial << "There are valid presets, in fact "<< getNumberOfPresets() << " are found.\n";
		if (presetNum==0)	{	// Load current preset if nothing is given
			presetNum = EEPROM.read(EEPROM_PRESET_START+1);
//			Serial << "Current preset: "<< presetNum << "\n";
		}
//		Serial << "Preset: "<< presetNum << "\n";
		if (presetNum>0 && presetNum<=getNumberOfPresets())	{	// preset num must be equal to or less than the total number
			uint16_t presetOffset = getPresetOffsetAddress(presetNum);
//			Serial << "presetOffset: "<< presetOffset << "\n";
			uint16_t presetLen = ((uint16_t)EEPROM.read(EEPROM_PRESET_START+5+presetOffset)<<8) | EEPROM.read(EEPROM_PRESET_START+6+presetOffset);
//			Serial << "presetLen: "<< presetLen << "\n";
			uint8_t csc = EEPROM_PRESET_TOKEN;
			for(uint16_t a=0; a<presetLen; a++)	{
				if (a>=SK_CONFIG_MEMORY_SIZE)	break;	// ERROR!
				globalConfigMem[a] = EEPROM.read(EEPROM_PRESET_START+7+presetOffset+a);	// Loading memory with preset, byte by byte.
				csc^=globalConfigMem[a];
			}
			if (csc==0)	{
//				Serial << "All good...\n";
				// All is good, exit with the new config in memory!
				return true;
			}
		}
	}
//	Serial << "Load default config...";
	loadDefaultConfig();
	return false;
}
uint16_t getPresetStoreLength()	{
	return ((uint16_t)EEPROM.read(EEPROM_PRESET_START+2)<<8) | EEPROM.read(EEPROM_PRESET_START+3);
}
void savePreset(uint8_t presetNum, uint16_t len)	{	// Len is excluding CSC byte. presetNum=255 to make new preset.
//		Serial << "savePreset() \n";
	if (getNumberOfPresets()>0)	{
		if (presetNum==0)	{	// Load current preset if nothing is given
			presetNum = EEPROM.read(EEPROM_PRESET_START+1);
		}
	} else {
		presetNum = 255;	// New preset initiated
	}
	uint16_t presetOffset;
	if (presetNum > getNumberOfPresets())	{
		if (len>0)	{
			presetNum = getNumberOfPresets()+1;	// New preset
		//				Serial << "New preset: " << presetNum << "\n";
			setNumberOfPresets(presetNum);
			setCurrentPreset(presetNum);
			presetOffset = getPresetOffsetAddress(presetNum);
		//				Serial << "New presetOffset: " << presetOffset << "\n";

			setPresetStoreLength(presetOffset+len+1+2);
		//	Serial << "getPresetStoreLength(): " << getPresetStoreLength() << "\n";
		}
	} else {
		setCurrentPreset(presetNum);
//		Serial << "Existing preset: " << presetNum << "\n";
		presetOffset = getPresetOffsetAddress(presetNum);
//		Serial << "New presetOffset: " << presetOffset << "\n";
		uint16_t nextPresetOffset = getPresetOffsetAddress(presetNum+1);
//		Serial << "nextPresetOffset: " << nextPresetOffset << "\n";
		uint16_t presetLen = ((uint16_t)EEPROM.read(EEPROM_PRESET_START+5+presetOffset)<<8) | EEPROM.read(EEPROM_PRESET_START+6+presetOffset);	// Includes CSC byte
		if (len>0)	{
			moveEEPROMMemoryBlock(EEPROM_PRESET_START+5+nextPresetOffset, EEPROM_PRESET_START+5+getPresetStoreLength(), (len+1)-presetLen);
			setPresetStoreLength(getPresetStoreLength()+(len+1)-presetLen);
		} else {
//			Serial << "Deleting a preset \n";
			moveEEPROMMemoryBlock(EEPROM_PRESET_START+5+nextPresetOffset, EEPROM_PRESET_START+5+getPresetStoreLength(), -presetLen-2);
			setPresetStoreLength(getPresetStoreLength()-presetLen-2);
			setNumberOfPresets(getNumberOfPresets()-1);
			setCurrentPreset(presetNum-1);
		}
	}
	if (len>0)	{
		// Store memory:
		uint8_t csc = EEPROM_PRESET_TOKEN;
		for(uint16_t a=0; a<len; a++)	{
			EEPROM.write(EEPROM_PRESET_START+7+presetOffset+a,globalConfigMem[a]);	// Loading memory with preset, byte by byte.
			csc^=globalConfigMem[a];
		}
		EEPROM.write(EEPROM_PRESET_START+7+presetOffset+len,csc);	// Checksum byte
		EEPROM.write(EEPROM_PRESET_START+5+presetOffset,highByte(len+1));
		EEPROM.write(EEPROM_PRESET_START+6+presetOffset,lowByte(len+1));
	}
}
/*
void presetCheck()	{
	Serial << "Number of presets: " << EEPROM.read(EEPROM_PRESET_START+0) << "\n";
	Serial << "Current preset: " << EEPROM.read(EEPROM_PRESET_START+1) << "\n";
	uint16_t accumLength = 0;
	for(uint8_t a=0; a<EEPROM.read(EEPROM_PRESET_START+0); a++)	{
		Serial << "Preset: " << (a+1)<< "\n";
		uint16_t presetOffset = getPresetOffsetAddress(a+1);
		Serial << "Preset offset: " << presetOffset << "\n";
		uint16_t length = ((EEPROM.read(EEPROM_PRESET_START+5+presetOffset)<<8)|EEPROM.read(EEPROM_PRESET_START+6+presetOffset));
		Serial << "Length " << length << ":\n";
		accumLength+=length+2;
		
		uint8_t csc=EEPROM_PRESET_TOKEN;
		for(uint16_t b=0; b<length;b++)	{
			csc^=EEPROM.read(EEPROM_PRESET_START+7+presetOffset+b);
		}
		Serial << "csc: 0=" << csc << "\n";
	}
	Serial << "Pointer at: " << accumLength << "\n";
	Serial << "Total Length of preset store: " << getPresetStoreLength() << "\n";
}
*/


/**
 * First thing to call in setup() is the device initialization:
 */
bool debugMode = false;
void initController()  {

  // Random Seed:
  randomSeed(analogRead(0) + analogRead(1) + analogRead(5) + analogRead(6) + analogRead(7));

  // Initialize serial communication at 115200 bits per second:
  Serial.begin(115200);
  Serial << F("\n\n*****************************\nSKAARHOJ Controller Booting \n*****************************\n");

  // Setup Config:
  pinMode(18, INPUT);    // CFG input (active low)  - if you set it to INPUT_PULLUP, the resistor on the Bottom part will not be strong enough to pull it down!!

  // Setup LED pins:
  statusLED(OFF);   // To prevent brief light in LEDs upon boot
  pinMode(13, OUTPUT);    // Red Status LED, active low
  pinMode(15, OUTPUT);    // Green Status LED, active low
  pinMode(14, OUTPUT);    // Blue Status LED, active low

#if SK_MODEL==SK_E21TVS
  Serial << F("SK_MODEL=SK_E21TVS\n");
  Serial << F("SK_HWCONF_SLIDER=") << SK_HWCONF_SLIDER << F("\n");
  Serial << F("SK_HWCONF_GPIO=") << SK_HWCONF_GPIO << F("\n");
#elif SK_MODEL==SK_MICROBI16
  Serial << F("SK_MODEL: SK_MICROBI16\n");
#elif SK_MODEL==SK_E21CMB6
  Serial << F("SK_MODEL: SK_E21CMB6\n");
  Serial << F("SK_HWCONF_STDOLEDDISPLAY=") << SK_HWCONF_STDOLEDDISPLAY << F("\n");
  Serial << F("SK_HWCONF_SSWMENU=") << SK_HWCONF_SSWMENU << F("\n");
  Serial << F("SK_HWCONF_SLIDER=") << SK_HWCONF_SLIDER << F("\n");
#elif SK_MODEL==SK_C15
  Serial << F("SK_MODEL: SK_C15\n");
  Serial << F("SK_HWCONF_SSWBUTTONS=") << SK_HWCONF_SSWBUTTONS << F("\n");
  Serial << F("SK_HWCONF_JOYSTICK=") << SK_HWCONF_JOYSTICK << F("\n");
  Serial << F("SK_MODELVAR=") << SK_MODELVAR << F("\n");
#elif SK_MODEL==SK_C90
  Serial << F("SK_MODEL: SK_C90\n");
  Serial << F("SK_HWCONF_SLIDER=") << SK_HWCONF_SLIDER << F("\n");
#elif SK_MODEL==SK_C90A
  Serial << F("SK_MODEL: SK_C90A\n");
  Serial << F("SK_HWCONF_SLIDER=") << SK_HWCONF_SLIDER << F("\n");
  Serial << F("SK_HWCONF_ACM=") << SK_HWCONF_ACM << F("\n");
#elif SK_MODEL==SK_C90SM
  Serial << F("SK_MODEL: SK_C90SM\n");
  Serial << F("SK_HWCONF_SLIDER=") << SK_HWCONF_SLIDER << F("\n");
  Serial << F("SK_HWCONF_MENU=") << SK_HWCONF_MENU << F("\n");
  Serial << F("SK_HWCONF_SSWMENU=") << SK_HWCONF_SSWMENU << F("\n");
  Serial << F("SK_HWCONF_ACM=") << SK_HWCONF_ACM << F("\n");
  Serial << F("SK_HWCONF_GPIO=") << SK_HWCONF_GPIO << F("\n");
#else
  Serial << F("SK_MODEL: UNKNOWN, HALTING!!\n");
  while (true) {
    statusLED(RED, 5);
  };
#endif
  Serial << F("UniSketch Ver: ") << SK_VERSION << F("\n");
  Serial << F("Compile Date: ") << __DATE__ << F(" ") << __TIME__ << F("\n");
  
  
  statusLED(PURPLE);   // Normal mode, so far...

  // I2C setup:
  Wire.begin();
  statusLED(QUICKBLANK);
    
    // Initializes the actual hardware components / modules on the controller
  bool buttonPressUponBoom = HWsetup();
  statusLED(QUICKBLANK);

  // Check Config Button press (or hardware button press)
  delay(500);   // Let people have time to release the button in case they just want to reset
  if (isConfigButtonPushed() || buttonPressUponBoom) {

    configMode = EEPROM.read(0) == 2 ? 2 : 1; // Current IP address
    if (EEPROM.read(0) != 0)  EEPROM.write(0, 0);
    statusLED(BLUE);    // In config mode 1, LED will be blue
    unsigned long cfgButtonPressTime = millis();
    Serial << F("Config Mode=1\n");

    while (configMode == 2 || isConfigButtonPushed())   {
      if (configMode == 2 || (millis() - cfgButtonPressTime > 5000)) {
        statusLED(WHITE);    // In config mode 2, LED will be white and blink
        Serial << F("Config Mode=2\n");
        configMode = 2;
        break;
      }
    }
  } else {
    statusLED(PURPLE);   // Normal mode
  }



	loadPreset(0);	// Current preset
//  Serial << "sizeof: " << sizeof(defaultControllerConfig) << "\n";
//  presetCheck();
  
  // Setting IP:
  if (configMode != 2)  {
    ip[0] = globalConfigMem[getConfigMemIPIndex()];
    ip[1] = globalConfigMem[getConfigMemIPIndex()+1];
    ip[2] = globalConfigMem[getConfigMemIPIndex()+2];
    ip[3] = globalConfigMem[getConfigMemIPIndex()+3];
  }
  Serial << F("IP address: ");
  for (uint8_t i = 0; i < 4; i++) {
    Serial << ip[i] << (i != 3 ? F(".") : F("\n"));
  }

  // Setting Subnet
  if (configMode != 2)  {
    subnet[0] = globalConfigMem[getConfigMemIPIndex()+4];
    subnet[1] = globalConfigMem[getConfigMemIPIndex()+5];
    subnet[2] = globalConfigMem[getConfigMemIPIndex()+6];
    subnet[3] = globalConfigMem[getConfigMemIPIndex()+7];
  }
  Serial << F("Subnet mask: ");
  for (uint8_t i = 0; i < 4; i++) {
    Serial << subnet[i] << (i != 3 ? F(".") : F("\n"));
  }
  
  // Setting MAC:
  mac[0] = EEPROM.read(10);
  mac[1] = EEPROM.read(11);
  mac[2] = EEPROM.read(12);
  mac[3] = EEPROM.read(13);
  mac[4] = EEPROM.read(14);
  mac[5] = EEPROM.read(15);
//  if (getConfigMode())  {
    Serial << F("MAC address: ");
    for (uint8_t i = 0; i < 6; i++) {
      Serial << _HEXPADL(mac[i], 2, "0") << (i != 5 ? F(":") : F("\n"));
    }
//  }

  // TODO: Add subnet - but only if it's valid!!
  Ethernet.begin(mac, ip);
  statusLED(QUICKBLANK);
  delay(500);
  
  // Check if debug mode is on, if so reset it for next reset:
  if (EEPROM.read(1) != 0)  {
    Serial << F("Debug Mode=1\n");
    debugMode = true;
    EEPROM.write(1, 0);
  }
  
  initActionCache();
}

