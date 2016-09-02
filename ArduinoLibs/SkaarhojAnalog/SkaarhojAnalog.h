/*  SkaarhojAnalog Arduino library with various utilities for products from SKAARHOJ.com
    Copyright (C) 2012 Kasper Skårhøj    <kasperskaarhoj@gmail.com>

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
#ifndef SkaarhojAnalog_H
#define SkaarhojAnalog_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "ADS7828.h"
#include "Wire.h"

/**
  Version 1.1.0
        (Semantic Versioning)
**/

class SkaarhojAnalog {
private:
  ADS7828 _analogConv;

  bool _debugMode;

  uint8_t _joystick_index;
  int _joystick_tolerance;
  int _joystick_previousPosition[3];
  int _joystick_previousValue[3];
  int _joystick_centerValue[3];
  bool _joystick_buttonStatus;
  bool _joystick_buttonStatusLastUp;
  bool _joystick_buttonStatusLastDown;

  uint8_t _uniDirectionalSlider_pinIndex;
  int _uniDirectionalSlider_sliderTolerance;
  int _uniDirectionalSlider_sliderLowEndOffset;
  int _uniDirectionalSlider_sliderHighEndOffset;
  int _uniDirectionalSlider_previousSliderValue;
  int _uniDirectionalSlider_previousTransitionPosition;
  bool _uniDirectionalSlider_sliderDirectionUp;
  bool _uniDirectionalSlider_disableUnidirectionality;

public:
  SkaarhojAnalog();
  void debugMode();

  // Joystick functions:
  void joystick_init(int tolerance, uint8_t i2cAddress, uint8_t index = 0);
  bool joystick_hasMoved(uint8_t index);
  int joystick_position(uint8_t index);
  bool joystick_buttonUp();
  bool joystick_buttonDown();
  bool joystick_buttonIsPressed();
  int joystick_AnalogRead(uint8_t index);

  // Slider functions:
  void uniDirectionalSlider_init(int sliderTolerance, int sliderLowEndOffset, int sliderHighEndOffset, uint8_t i2cAddress, uint8_t pinIndex);
  void uniDirectionalSlider_disableUnidirectionality(bool disable);
  bool uniDirectionalSlider_hasMoved();
  int uniDirectionalSlider_position();
  int16_t uniDirectionalSlider_rawValue();
  bool uniDirectionalSlider_isAtEnd();
};
#endif
