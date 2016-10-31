/*********************************************************************
This library is inspired by the Adafruit_SSD1306 library from Adafruit
written by Limor Fried/Ladyada  for Adafruit Industries. The original 
library is BSD licensed.

Just like SKAARHOJ, Adafruit also invests time and resources providing 
open source code, so SKAARHOJ encourage you to support Adafruit and 
open-source hardware by purchasing products from Adafruit as well!
*********************************************************************/

#ifndef SkaarhojOLEDRocker_h
#define SkaarhojOLEDRocker_h

#include "Arduino.h"
//  #include "SkaarhojPgmspace.h"  - 23/2 2014
#include <Adafruit_GFX.h>
#include <Wire.h>


#if defined(ARDUINO_SKAARDUINO_V1) || defined(ARDUINO_SKAARDUINO_DUE)  
	#include <SPI.h>
#endif

#define BLACK 0
#define WHITE 1

#define SKAARHOJOLEDRocker_LCDWIDTH                  96
#define SKAARHOJOLEDRocker_LCDHEIGHT                 64


class SkaarhojOLEDRocker : public Adafruit_GFX {
 public:
  void drawPixel(int16_t x, int16_t y, uint16_t color);

  SkaarhojOLEDRocker();
  void begin(uint8_t address);

  void sendCommand(uint8_t c);
  void sendData(uint8_t c);

  void clearDisplay(void);
  void display(uint8_t cs);

  void testProgramme(uint8_t buttonMask);


	bool buttonUp(uint8_t buttonNumber, uint8_t buttonIndex);
	bool buttonDown(uint8_t buttonNumber, uint8_t buttonIndex);
	bool buttonIsPressed(uint8_t buttonNumber, uint8_t buttonIndex);
	bool buttonIsHeldFor(uint8_t buttonNumber, uint8_t buttonIndex, uint16_t timeout);
	bool buttonIsReleasedAgo(uint8_t buttonNumber, uint8_t buttonIndex, uint16_t timeout);
	uint8_t buttonUpAll(uint8_t buttonNumber);
	uint8_t buttonDownAll(uint8_t buttonNumber);
	uint8_t buttonIsPressedAll(uint8_t buttonNumber);
	bool isButtonIndexIn(uint8_t buttonIndex, uint8_t allButtonsState);



 private:
	uint8_t _boardAddress, _dataPin, _clockPin, _cs, _readButtonPressNum;
	bool _dc, _rst, _vcc;
	
	uint8_t _buttonStatus[4];
	uint8_t _buttonStatusLastUp[4];
	uint8_t _buttonStatusLastDown[4];

	uint16_t _lastButtonStatus[4];
	uint16_t _lastButtonReleased[4];
	unsigned long _inputChangeTime;
	bool _inputChangeTimeActive;
	
	
	
  void fastSPIwrite(uint8_t c);

  	void chipSelect(uint8_t cs);
	void setDC(bool dc);
	void setVCC(bool dc);
	void writeControlPins();
	void _readButtonStatus(uint8_t buttonNum);
	uint8_t readButton(uint8_t buttonNum);
	bool _validButtonNumber(uint8_t buttonNumber);
	
  volatile uint8_t *mosiport, *clkport;
  uint8_t mosipinmask, clkpinmask;
};

#endif
