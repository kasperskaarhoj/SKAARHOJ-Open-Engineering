/*  SkaarhojAnalog Arduino library with various utilities for products from SKAARHOJ.com
    Copyright (C) 2015 Kasper Skårhøj    <kasperskaarhoj@gmail.com>

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
#include "SkaarhojAnalog.h"

SkaarhojAnalog::SkaarhojAnalog() { _debugMode = false; } // Empty constructor.
void SkaarhojAnalog::debugMode() { _debugMode = true; }

/**
 * Joystick
 * i2cAddress: 0-3
 */
void SkaarhojAnalog::joystick_init(int16_t tolerance, uint8_t i2cAddress, uint8_t index) {

  ADS7828 analogConv; // Address
  _analogConv = analogConv;
  _analogConv.init(i2cAddress);

  _joystick_index = index;
  _joystick_tolerance = tolerance;

  // Internal variables during operation:
  _joystick_previousPosition[0] = -1000;
  _joystick_previousPosition[1] = -1000;
  _joystick_previousPosition[2] = -1000;
  _joystick_previousValue[0] = -1000;
  _joystick_previousValue[1] = -1000;
  _joystick_previousValue[2] = -1000;
  _joystick_sentCenterPos = 0;
  _joystick_range = 100;

  // Reset:
  _joystick_centerValue[0] = joystick_AnalogRead(0);
  _joystick_centerValue[1] = joystick_AnalogRead(1);
  _joystick_centerValue[2] = joystick_AnalogRead(2);
  joystick_hasMoved(0);
  joystick_hasMoved(1);
  joystick_hasMoved(2);

  Serial.print("Center values: ");
  Serial.print(_joystick_centerValue[0]);
  Serial.print(",");
  Serial.print(_joystick_centerValue[1]);
  Serial.print(",");
  Serial.print(_joystick_centerValue[2]);
  Serial.println();
}

void SkaarhojAnalog::joystick_extendedRange(bool state) {
  _joystick_range = state ? 500 : 100;
}

bool SkaarhojAnalog::joystick_hasMoved(uint8_t index) { // Index is 0 or 1 or 2
  if (index <= 2) {
    int potValue = joystick_AnalogRead(index) - _joystick_centerValue[index];

    if (potValue >= _joystick_previousValue[index] + _joystick_tolerance || potValue <= _joystick_previousValue[index] - _joystick_tolerance) {

      _joystick_previousValue[index] = potValue;

      int joystickPosition = potValue;
      if (potValue < 0) {
        joystickPosition = (long)_joystick_range * (long)joystickPosition / (long)_joystick_centerValue[index];
      } else {
        joystickPosition = (long)_joystick_range * (long)joystickPosition / (long)(1023 - _joystick_centerValue[index]);
      }
      if (joystickPosition > (_joystick_range*100)/97)
        joystickPosition = _joystick_range;
      if (joystickPosition < (-_joystick_range*100)/97)
        joystickPosition = -_joystick_range;
      if (joystickPosition < _joystick_range/30 && joystickPosition > -_joystick_range/30)
        joystickPosition = 0;

      if (_joystick_previousPosition[index] != joystickPosition) {
        _joystick_previousPosition[index] = joystickPosition;
        return true;
      }
    }
    return false;
  }
}

bool SkaarhojAnalog::joystick_isAtCenter(uint8_t index) {
  return abs(_joystick_previousPosition[index]) < _joystick_range/30;
}

int16_t SkaarhojAnalog::joystick_position(uint8_t index) { return _joystick_previousPosition[index]; }
bool SkaarhojAnalog::joystick_buttonUp() {
  bool buttonChange = (_joystick_buttonStatusLastUp ^ joystick_buttonIsPressed());
  _joystick_buttonStatusLastUp ^= buttonChange;

  return (buttonChange & ~_joystick_buttonStatus) ? true : false;
}
bool SkaarhojAnalog::joystick_buttonDown() {
  bool buttonChange = (_joystick_buttonStatusLastDown ^ joystick_buttonIsPressed());
  _joystick_buttonStatusLastDown ^= buttonChange;

  return (buttonChange & _joystick_buttonStatus) ? true : false;
}
bool SkaarhojAnalog::joystick_buttonIsPressed() {
  _joystick_buttonStatus = joystick_AnalogRead(3) < 100;
  return _joystick_buttonStatus;
}
int SkaarhojAnalog::joystick_AnalogRead(uint8_t index) {
  switch (index) {
  case 0:
    return _analogConv.analogRead(2 + (_joystick_index ? 4 : 0)) >> 2;
    break;
  case 1:
    return _analogConv.analogRead(3 + (_joystick_index ? 4 : 0)) >> 2;
    break;
  case 2:
    return _analogConv.analogRead(4 + (_joystick_index ? 4 : 0)) >> 2;
    break;
  case 3:
    return _analogConv.analogRead(0 + (_joystick_index ? 4 : 0)) >> 2;
    break;
  default:
    return -1;
    break;
  }
}

/**
 * Slider functions:
 */
void SkaarhojAnalog::uniDirectionalSlider_init(int16_t sliderTolerance, int16_t sliderLowEndOffset, int16_t sliderHighEndOffset, uint8_t i2cAddress, uint8_t pinIndex) {

  ADS7828 analogConv; // Address
  _analogConv = analogConv;
  _analogConv.init(i2cAddress);

  // Configuration constants, should have setter-methods:
  _uniDirectionalSlider_pinIndex = pinIndex;
  _uniDirectionalSlider_sliderTolerance = sliderTolerance;         // >0. How much it should be moved before it counts as a change.
  _uniDirectionalSlider_sliderLowEndOffset = sliderLowEndOffset;   // >0. How far the slider is moved in the low end before we start registering the value range: The starting position.
  _uniDirectionalSlider_sliderHighEndOffset = sliderHighEndOffset; // >0. How far the slider is moved in the high end before we start registering the value range: The ending position.

  // Internal variables during operation:
  _uniDirectionalSlider_previousSliderValue = -1;
  _uniDirectionalSlider_previousTransitionPosition = -1;
  _uniDirectionalSlider_sliderDirectionUp = false;
  _uniDirectionalSlider_disableUnidirectionality = false;

  uniDirectionalSlider_hasMoved(); // Make sure the _uniDirectionalSlider_previousSliderValue is set correctly.
}

void SkaarhojAnalog::uniDirectionalSlider_disableUnidirectionality(bool disable) {
  _uniDirectionalSlider_disableUnidirectionality = disable;
  _uniDirectionalSlider_previousSliderValue = -1;
  _uniDirectionalSlider_previousTransitionPosition = -1;
  _uniDirectionalSlider_sliderDirectionUp = false;

  uniDirectionalSlider_hasMoved(); // Make sure the _uniDirectionalSlider_previousSliderValue is set correctly.
}

int16_t SkaarhojAnalog::uniDirectionalSlider_rawValue() { return _analogConv.analogRead(_uniDirectionalSlider_pinIndex) >> 2; }

bool SkaarhojAnalog::uniDirectionalSlider_hasMoved() {
  int sliderValue = _analogConv.analogRead(_uniDirectionalSlider_pinIndex) >> 2;
  bool closeToEnd = false;

  if (sliderValue - _uniDirectionalSlider_sliderLowEndOffset < _uniDirectionalSlider_sliderTolerance && sliderValue < _uniDirectionalSlider_previousSliderValue)
  closeToEnd = true;
  if (sliderValue + _uniDirectionalSlider_sliderHighEndOffset > 1023 - _uniDirectionalSlider_sliderTolerance && sliderValue > _uniDirectionalSlider_previousSliderValue)
  closeToEnd = true;

  if (abs(sliderValue - _uniDirectionalSlider_previousSliderValue) >= _uniDirectionalSlider_sliderTolerance || closeToEnd) {

    // Find direction:
    if (!_uniDirectionalSlider_disableUnidirectionality && sliderValue >= _uniDirectionalSlider_previousSliderValue + _uniDirectionalSlider_sliderTolerance && (_uniDirectionalSlider_previousSliderValue == -1 || _uniDirectionalSlider_previousSliderValue < _uniDirectionalSlider_sliderLowEndOffset)) {
      _uniDirectionalSlider_sliderDirectionUp = true;
    }
    if (!_uniDirectionalSlider_disableUnidirectionality && sliderValue <= _uniDirectionalSlider_previousSliderValue - _uniDirectionalSlider_sliderTolerance && (_uniDirectionalSlider_previousSliderValue == -1 || _uniDirectionalSlider_previousSliderValue > 1023 - _uniDirectionalSlider_sliderHighEndOffset)) {
      _uniDirectionalSlider_sliderDirectionUp = false;
    }

    _uniDirectionalSlider_previousSliderValue = sliderValue;

    int16_t transitionPosition = 1000L * (int32_t)(sliderValue - _uniDirectionalSlider_sliderLowEndOffset) / (int32_t)(1023 - _uniDirectionalSlider_sliderLowEndOffset - _uniDirectionalSlider_sliderHighEndOffset);
    transitionPosition = constrain(transitionPosition, 0, 1000);
    
    if (!_uniDirectionalSlider_sliderDirectionUp)
      transitionPosition = 1000 - transitionPosition;
    if (_uniDirectionalSlider_previousTransitionPosition != transitionPosition) {
      bool returnValue = true;
      if ((_uniDirectionalSlider_previousTransitionPosition == 0 || _uniDirectionalSlider_previousTransitionPosition == 1000) && (transitionPosition == 0 || transitionPosition == 1000)) {
        returnValue = false;
      }
      _uniDirectionalSlider_previousTransitionPosition = transitionPosition;
      return returnValue;
    }
  }
  return false;
}

int16_t SkaarhojAnalog::uniDirectionalSlider_position() { return _uniDirectionalSlider_previousTransitionPosition; }

bool SkaarhojAnalog::uniDirectionalSlider_isAtEnd() { return (_uniDirectionalSlider_previousTransitionPosition == 1000 || _uniDirectionalSlider_previousTransitionPosition == 0); }
