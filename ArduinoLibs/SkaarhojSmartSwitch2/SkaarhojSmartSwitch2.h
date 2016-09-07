/*  SkaarhojSmartSwitch2 library for Arduino
	Copyright (C) 2012 Kasper Skårhøj    <kasperskaarhoj@gmail.com> 
	Copyright (C) 2012 Filip Sandborg-Olsen   <filipsandborg@me.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef SkaarhojSmartSwitch2_H
#define SkaarhojSmartSwitch2_H

#include "Arduino.h"
//  #include "SkaarhojPgmspace.h"  - 23/2 2014
#include <Adafruit_GFX.h>
#include <Wire.h>
#include "MCP23017.h"

#if defined(ARDUINO_SKAARDUINO_V1) || defined(ARDUINO_SKAARDUINO_DUE)  
	#include <SPI.h>
#endif

#define BLACK 0
#define WHITE 1

#define SKAARHOJSMARTSWITCH_LCDWIDTH                  64
#define SKAARHOJSMARTSWITCH_LCDHEIGHT                 32

/**
  Version 1.0.0
	(Semantic Versioning)
**/

class SkaarhojSmartSwitch2: public Adafruit_GFX {
  private:
	uint8_t _boardAddress;

	MCP23017 _buttonMux;
	uint8_t _buttonStatus;
	uint8_t _buttonStatusLastUp;
	uint8_t _buttonStatusLastDown;

	uint16_t _lastButtonStatus;
	uint16_t _lastButtonReleased;
	unsigned long _inputChangeTime;
	bool _inputChangeTimeActive;

	int _clockPin;
	int _dataPin;

  public:
	void drawPixel(int16_t x, int16_t y, uint16_t color);
	bool getPixel(int16_t x, int16_t y);

	SkaarhojSmartSwitch2();
	void begin(uint8_t address);

	void setButtonColor(uint8_t red, uint8_t green, uint8_t blue, uint8_t cs);
	void setButtonBrightness(uint8_t brightness, uint8_t cs);
	
	bool buttonUp(uint8_t buttonNumber);
	bool buttonDown(uint8_t buttonNumber);
	bool buttonIsPressed(uint8_t buttonNumber);
	bool buttonIsHeldFor(uint8_t buttonNumber, uint16_t timeout);
	bool buttonIsReleasedAgo(uint8_t buttonNumber, uint16_t timeout);
	uint8_t buttonUpAll();
	uint8_t buttonDownAll();
	uint8_t buttonIsPressedAll();
	bool isButtonIn(uint8_t buttonNumber, uint8_t allButtonsState);

	uint8_t getButtonModes();
	void setButtonModes(uint8_t modes);


    void clearDisplay(void);
    void display(uint8_t cs);
	
	void testProgramme(uint8_t buttonMask);
	
  private:
	void _readButtonStatus();
	bool _validButtonNumber(uint8_t buttonNumber);
	int _writeCommand(int address, int value, uint8_t cs);
	void _chipSelect(uint8_t cs);
};
#endif 
