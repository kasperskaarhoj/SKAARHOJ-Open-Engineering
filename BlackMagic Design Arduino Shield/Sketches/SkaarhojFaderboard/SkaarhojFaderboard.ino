#include "SkaarhojUtils.h"
#include "Streaming.h"
#include <BMDSDIControl.h>
#include "Wire.h"

const int shieldAddress = 0x6E;

//BMD_SDICameraControl_I2C cameraControl(shieldAddress);
BMD_SDICameraControl_I2C cameraControl;

SkaarhojUtils utils;
SkaarhojUtils utils2;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  
  cameraControl.begin(shieldAddress);
  //cameraControl.begin();
  Wire.setClock(400000L);
  // Init button pins, active -> low
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(2, INPUT);
  
  // Init LED pins
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  
  // Set SDI Override
  cameraControl.setOverride(true);
  
  // Initialize slider
  utils.uniDirectionalSlider_init(40, 35, 35, A0);
  utils.uniDirectionalSlider_disableUnidirectionality(true);
  utils.uniDirectionalSlider_hasMoved();

  // Initialize knob
  utils2.uniDirectionalSlider_init(40, 35, 35, A1);
  utils2.uniDirectionalSlider_disableUnidirectionality(true);
  utils2.uniDirectionalSlider_hasMoved();  
}

uint8_t buttonState[3];
unsigned long buttonStateChanged[3];

bool buttonHelper(uint8_t button) {
  uint8_t reading;
  switch(button) {
    case 0:
      reading = digitalRead(A2);
      break;
    case 1:
      reading = digitalRead(A3);
      break;
    case 2:
      reading = digitalRead(2);
      break;
  }
  
  if(millis() - buttonStateChanged[button] > 50) {
    if(buttonState[button] == 1) {
      buttonState[button] = 2;
      return true;
    } else if(buttonState[button] == 3) {
      buttonState[button] = 0;
    }
  }
  
  if(reading == LOW && buttonState[button] == 0) {
    buttonState[button] = 1;
    buttonStateChanged[button] = millis();
  }
  
  if(reading == HIGH && buttonState[button] == 2) {
    buttonState[button] = 3;
    buttonStateChanged[button] = millis();
  }
  
  return false;
}

void setLed(uint8_t num, bool red, bool green) {
  switch(num) {
    case 0:
      digitalWrite(4, green);
      digitalWrite(5, red);
      break;
    case 1:
      digitalWrite(6, green);
      digitalWrite(7, red);
      break;
    case 2:
      digitalWrite(8, green);
      digitalWrite(9, red);
      break;
  }
}

// 0: Knob controls master black , 1-3: Knob controls gamma R,G,B respectively
uint8_t currentState = 0;

float lift[4] = {0.0, 0.0, 0.0, 0.0};
void loop() {
  // Check if slider has moved
  if(utils.uniDirectionalSlider_hasMoved()) {
    Serial << F("New slider position") << utils.uniDirectionalSlider_position() << "\n";
    if(utils.uniDirectionalSlider_isAtEnd()) {
      Serial << F("Slider at end\n");
    }
    cameraControl.writeCommandFixed16(1, 0, 3, 0, 1.0 - (float)(utils.uniDirectionalSlider_position()/1000.0));
  }
  
  // Check if knob has moved
  if(utils2.uniDirectionalSlider_hasMoved()) {
    Serial << F("New knob position") << utils2.uniDirectionalSlider_position() << "\n";
    if(utils2.uniDirectionalSlider_isAtEnd()) {
      Serial << F("Knob at end\n");
    }
    lift[(currentState+3)%4] = (float)(1.0 - utils2.uniDirectionalSlider_position()/1000.0);
    cameraControl.writeCommandFixed16(1, 8, 0, 0, lift);
  }
 
  if(buttonHelper(0)) {
    if(currentState == 1) {
      currentState = 0;
      setLed(0, false, false);
    } else {
      Serial << F("Button 1 pressed\n");
      setLed(0, true, false);
      setLed(1, false, false);
      setLed(2, false, false);
      currentState = 1;
    }
  }
  
  if(buttonHelper(1)) {
    if(currentState == 2) {
      currentState = 0;
      setLed(1, false, false);
    } else {
      Serial << F("Button 2 pressed\n");
      setLed(0, false, false);
      setLed(1, true, false);
      setLed(2, false, false);
      currentState = 2;
    }
  }
  
  if(buttonHelper(2)) {
    if(currentState == 3) {
      currentState = 0;
      setLed(2, false, false);
    } else {
      Serial << F("Button 3 pressed\n");
      setLed(0, false, false);
      setLed(1, false, false);
      setLed(2, true, false);
      currentState = 3;
    }
  }
}
