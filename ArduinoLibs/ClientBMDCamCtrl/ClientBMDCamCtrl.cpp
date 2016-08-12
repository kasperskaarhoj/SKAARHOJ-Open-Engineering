#include "ClientBMDCamCtrl.h"

void ClientBMDCamCtrl::begin(uint8_t address) {
  _cameraControl.begin(address);
  _tallyControl.begin(address);

  _cameraControl.setOverride(true);
}

// void ClientBMDCamCtrl::begin(BMD_SDICameraControl_I2C *cameraControl, BMD_SDITallyControl_I2C *tallyControl) {
//   _cameraControl = cameraControl;
//   _tallyControl = tallyControl;
//
//   _cameraControl.begin();
//   _tallyControl->begin();
//
//   _serialOutput = 0;
//   _cameraOutPos = 0;
// }

void ClientBMDCamCtrl::serialOutput(uint8_t level) {
	_serialOutput = level;
}

void ClientBMDCamCtrl::runLoop() {
  unsigned long startTime = millis();
  if(_cameraControl.available()){
    int availableLength = _cameraControl.read(a + pos, 519 - pos) + pos;

    if(availableLength > pos) {
      int i = 0;

      while(1) {
        int len = 0;

        if(i+1 <= availableLength) {
          len = a[i+1];
        }

        int totalPacket = (4 + len + 3) & ~0x03;

        if(totalPacket + i >= availableLength) {
          memmove(a, a + i, availableLength - i);
          pos = availableLength - i + 1;
          break;
        } else {
          printPacket(a + i, 4 + len);
          // if(*(a+i) == 0x01 && *(a+i+4) == 0x08 && *(a+i+5) == 0x04) {
          //   appendCameraPacket(a+i, 4+len);
          //   uint16_t c = *(a+i+11) << 8 | *(a+i+10);
          //   float contrast = _cameraControl.fromFixed16(c);
          //   Serial.print("Contrast is set to: ");
          //   Serial.println(contrast);
          // }
          i += totalPacket;
        }
      }
    } else if(availableLength == -1) {
        Serial.println("FATAL ERROR: Received package did not fit into buffer! Deadlock.");
        while(1);
    }
    if(_cameraOutPos > 0) {
      /*Serial.print("Writing ");
      Serial.print(_cameraOutPos);
      Serial.println(" bytes to output.");

      Serial.print("Parsing: ");
      Serial.println(millis() - startTime);*/
      sendCameraPacket(_cameraOutput, _cameraOutPos);
      _cameraOutPos = 0;
      //Serial.print(" Parsing and sending: ");
      //Serial.println(millis() - startTime);
    }
  }
}

void ClientBMDCamCtrl::sendCameraPacket(byte* data, int len) {
  _cameraControl.write(data, len);
  //printPacket(data, len);
}

void ClientBMDCamCtrl::appendCameraPacket(byte* data, int len) {
  if(_cameraOutPos + len > 255) {
    sendCameraPacket(_cameraOutput, _cameraOutPos);
    _cameraOutPos = 0;
  }

  memcpy(_cameraOutput + _cameraOutPos, data, len);
  _cameraOutPos += len;
}

void ClientBMDCamCtrl::printPacket(byte *data, int len) {
    for(int i = 0; i < len; i++) {
      if(data[i] < 16) {
        Serial.print("0");
      }
      Serial.print(data[i], HEX);
      Serial.print(":");
    }
    Serial.println("");
}

template <typename T>
inline void clampValue(T *value, T const & min, T const & max) {
  if(*value < min) *value = min;
  if(*value > max) *value = max;
}

// Lens commands
void ClientBMDCamCtrl::setFocus(uint8_t camera, float focus, bool offset) { // Range: 0.0 - 1.0
  clampValue(&focus, 0.0f, 1.0f);
  _cameraControl.writeCommandFixed16(camera, 0, 0, (offset?1:0), focus);
}
void ClientBMDCamCtrl::setAutoFocus(uint8_t camera) {
  _cameraControl.writeCommandVoid(camera, 0, 1);
}
void ClientBMDCamCtrl::setIrisf(uint8_t camera, float iris, bool offset) { // Range: -1.0 - 16.0
  clampValue(&iris, -1.0f, 16.0f);
  _cameraControl.writeCommandFixed16(camera, 0, 2, (offset?1:0), iris);
}
void ClientBMDCamCtrl::setIris(uint8_t camera, float iris, bool offset) { // Range: 0.0- 1.0
  clampValue(&iris, 0.0f, 1.0f);
  _cameraControl.writeCommandFixed16(camera, 0, 3, (offset?1:0), iris);
}
void ClientBMDCamCtrl::setAutoIris(uint8_t camera) {
  _cameraControl.writeCommandVoid(camera, 0, 5);
}
void ClientBMDCamCtrl::setOIS(uint8_t camera, bool state, bool offset) {
  _cameraControl.writeCommandBool(camera, 0, 6, (offset?1:0), state);
}
void ClientBMDCamCtrl::setZoom(uint8_t camera, uint16_t zoom, bool offset) {
  _cameraControl.writeCommandInt16(camera, 0, 7, (offset?1:0), zoom);
}

void ClientBMDCamCtrl::setNormZoom(uint8_t camera, float zoom, bool offset) {
  clampValue(&zoom, 0.0f, 1.0f);

  _cameraControl.writeCommandInt16(camera, 0, 8, (offset?1:0), zoom);
}

void ClientBMDCamCtrl::setContinuousZoom(uint8_t camera, float rate, bool offset) { // -1.0 wide, 0.0 stop, 1.0 tele
  clampValue(&rate, -1.0f, 1.0f);
  _cameraControl.writeCommandFixed16(camera, 0, 9, (offset?1:0), rate);
}

// Video controls
void ClientBMDCamCtrl::setVideoMode(uint8_t camera, int8_t (&mode)[5]) {
  _cameraControl.writeCommandInt8(camera, 1, 0, 0, mode);
}
void ClientBMDCamCtrl::setSensorGain(uint8_t camera, int8_t gain, bool offset) {
  clampValue(&gain, (int8_t)1, (int8_t)16);
  _cameraControl.writeCommandInt8(camera, 1, 1, (offset?1:0), gain);
}
void ClientBMDCamCtrl::setWhiteBalance(uint8_t camera, int16_t wb, bool offset) { // 3200 - 7500
  clampValue(&wb, 3200, 7500);
  _cameraControl.writeCommandInt16(camera, 1, 2, (offset?1:0), wb);
}
void ClientBMDCamCtrl::setExposure(uint8_t camera, int32_t exposure, bool offset) { // 1 - 32000 µs
  clampValue(&exposure, (int32_t)1, (int32_t)42000);
  _cameraControl.writeCommandInt32(camera, 1, 5, (offset?1:0), exposure);
}
void ClientBMDCamCtrl::setDynamicRangeMode(uint8_t camera, int8_t mode) { // 0: film, 1: Video
  if(mode == 0 || mode == 1) {
    _cameraControl.writeCommandInt8(camera, 1, 7, 0, mode);
  }
}
void ClientBMDCamCtrl::setVideoSharpening(uint8_t camera, int8_t mode) { // 0: off, 1: low, 2: medium, 3: high
  clampValue(&mode, (int8_t) 0, (int8_t) 3);
  _cameraControl.writeCommandInt8(camera, 1, 8, 0, mode);
}

// Audio controls
void ClientBMDCamCtrl::setMicLevel(uint8_t camera, float level, bool offset) { // 0.0 - 1.0
  clampValue(&level, 0.0f, 1.0f);
  _cameraControl.writeCommandFixed16(camera, 2, 0, (offset?1:0), level);
}
void ClientBMDCamCtrl::setHeadphoneLevel(uint8_t camera, float level, bool offset) { // 0.0 - 1.0
  clampValue(&level, 0.0f, 1.0f);
  _cameraControl.writeCommandFixed16(camera, 2, 1, (offset?1:0), level);
}
void ClientBMDCamCtrl::setHeadphoneProgramMix(uint8_t camera, float level, bool offset) { // 0.0 - 1.0
  clampValue(&level, 0.0f, 1.0f);
  _cameraControl.writeCommandFixed16(camera, 2, 2, (offset?1:0), level);
}
void ClientBMDCamCtrl::setSpeakerLevel(uint8_t camera, float level, bool offset) { // 0.0 - 1.0
  clampValue(&level, 0.0f, 1.0f);
  _cameraControl.writeCommandFixed16(camera, 2, 3, (offset?1:0), level);
}

void ClientBMDCamCtrl::setInputType(uint8_t camera, int8_t type) { // 0: Internal mic, 1: Line level, 2: Low mic level, 3: High mic level
  clampValue(&type, (int8_t) 0, (int8_t) 3);
  _cameraControl.writeCommandInt8(camera, 2, 4, 0, type);
}

void ClientBMDCamCtrl::setInputLevels(uint8_t camera, float (&level)[2], bool offset) { // Range 0.0 - 1.0, [0]: ch0, [1]: ch1
  clampValue(&level[0], 0.0f, 1.0f);
  clampValue(&level[1], 0.0f, 1.0f);
  _cameraControl.writeCommandFixed16(camera, 2, 5, (offset?1:0), level);
}
void ClientBMDCamCtrl::setPhantomPower(uint8_t camera, bool state, bool offset) { // True: powered
  _cameraControl.writeCommandBool(camera, 2, 6, (offset?1:0), state);
}

// Output controls
void ClientBMDCamCtrl::setOverlays(uint8_t camera, uint16_t mask) {
  _cameraControl.writeCommandInt16(camera, 3, 0, 0, mask);
}

// Display controls
void ClientBMDCamCtrl::setDisplayBrightness(uint8_t camera, float level, bool offset) { // 0.0 - 1.0
  clampValue(&level, 0.0f, 1.0f);
  _cameraControl.writeCommandFixed16(camera, 4, 0, (offset?1:0), level);
}
void ClientBMDCamCtrl::setDisplayOverlays(uint8_t camera, uint8_t mask) { // 0x4: Zebra, 0x8 peaking
  _cameraControl.writeCommandInt16(camera, 4, 1, 0, mask);
}
void ClientBMDCamCtrl::setDisplayZebraLevels(uint8_t camera, float level, bool offset) { // 0.0 - 1.0
  clampValue(&level, 0.0f, 1.0f);
  _cameraControl.writeCommandFixed16(camera, 4, 2, (offset?1:0), level);
}
void ClientBMDCamCtrl::setDisplayPeakingLevel(uint8_t camera, float level, bool offset) {
  clampValue(&level, 0.0f, 1.0f);
  _cameraControl.writeCommandFixed16(camera, 4, 3, (offset?1:0), level);
}
void ClientBMDCamCtrl::setDisplayColorBarsTime(uint8_t camera, int8_t secs, bool offset) { // 0: disable, 1-30: time to display bars
  clampValue(&secs, (int8_t)0, (int8_t) 30);
  _cameraControl.writeCommandInt8(camera, 4, 4, (offset?1:0), secs);
}

// Tally controls
void ClientBMDCamCtrl::setTallyBrightness(uint8_t camera, float level, bool offset) { // 0.0 - 1.0
  clampValue(&level, 0.0f, 1.0f);
  _cameraControl.writeCommandFixed16(camera, 5, 0, (offset?1:0), level);
}
void ClientBMDCamCtrl::setTallyFrontBrightness(uint8_t camera, float level, bool offset) { // 0.0 - 1.0
  clampValue(&level, 0.0f, 1.0f);
  _cameraControl.writeCommandFixed16(camera, 5, 1, (offset?1:0), level);
}
void ClientBMDCamCtrl::setTallyRearBrightness(uint8_t camera, float level, bool offset) { // 0.0 - 1.0
  clampValue(&level, 0.0f, 1.0f);
  _cameraControl.writeCommandFixed16(camera, 5, 2, (offset?1:0), level);
}

// Reference controls
void ClientBMDCamCtrl::setReferenceSource(uint8_t camera, int8_t value) { // 0: internal, 1: program, 2: external
  if(value == 0 || value == 1 || value == 2) {
    _cameraControl.writeCommandInt8(camera, 6, 0, 0, value);
  }
}
void ClientBMDCamCtrl::setReferenceOffset(uint8_t camera, int32_t offset, bool offs) {
  _cameraControl.writeCommandInt32(camera, 6, 1, (offs?1:0), offset);
}

// Configuration controls
void ClientBMDCamCtrl::setClock(uint8_t camera, int32_t (&value)[2]) { // 0: time, 1: date
  _cameraControl.writeCommandInt32(camera, 7, 0, 0, value);
}

// Colour correction controls
void ClientBMDCamCtrl::setCameraLift(uint8_t camera, float (&value)[4], bool offset) { // 0: red, 1: green, 2: blue, 3: luma (-2.0 - 2.0)
  clampValue(&value[0], -2.0f, 2.0f);
  clampValue(&value[1], -2.0f, 2.0f);
  clampValue(&value[2], -2.0f, 2.0f);
  clampValue(&value[3], -2.0f, 2.0f);

  _cameraControl.writeCommandFixed16(camera, 8, 0, (offset?1:0), value);
}
void ClientBMDCamCtrl::setCameraGamma(uint8_t camera, float (&value)[4], bool offset) { // 0: red, 1: green, 2: blue, 3: luma (-4.0 - 4.0)
  clampValue(&value[0], -4.0f, 4.0f);
  clampValue(&value[1], -4.0f, 4.0f);
  clampValue(&value[2], -4.0f, 4.0f);
  clampValue(&value[3], -4.0f, 4.0f);

  _cameraControl.writeCommandFixed16(camera, 8, 1, (offset?1:0), value);
}
void ClientBMDCamCtrl::setCameraGain(uint8_t camera, float (&value)[4], bool offset) { // 0: red, 1: green, 2: blue, 3: luma (0.0 - 16.0)
  clampValue(&value[0], 0.0f, 16.0f);
  clampValue(&value[1], 0.0f, 16.0f);
  clampValue(&value[2], 0.0f, 16.0f);
  clampValue(&value[3], 0.0f, 16.0f);

  _cameraControl.writeCommandFixed16(camera, 8, 2, (offset?1:0), value);
}
void ClientBMDCamCtrl::setCameraOffset(uint8_t camera, float (&value)[4], bool offset) { // 0: red, 1: green, 2: blue, 3: luma (-8.0 - 8.0)
  clampValue(&value[0], -8.0f, 8.0f);
  clampValue(&value[1], -8.0f, 8.0f);
  clampValue(&value[2], -8.0f, 8.0f);
  clampValue(&value[3], -8.0f, 8.0f);

  _cameraControl.writeCommandFixed16(camera, 8, 3, (offset?1:0), value);
}
void ClientBMDCamCtrl::setCameraContrast(uint8_t camera, float (&value)[2], bool offset) { // 0: pivot (0.0-1.0), 1: adjust (0.0 - 2.0)
  clampValue(&value[0], 0.0f, 1.0f);
  clampValue(&value[1], 0.0f, 2.0f);

  _cameraControl.writeCommandFixed16(camera, 8, 4, (offset?1:0), value);
}
void ClientBMDCamCtrl::setCameraLumaMix(uint8_t camera, float value, bool offset) { // 0.0 - 1.0
  clampValue(&value, 0.0f, 1.0f);

  _cameraControl.writeCommandFixed16(camera, 8, 5, (offset?1:0), value);
}
void ClientBMDCamCtrl::setCameraColourAdjust(uint8_t camera, float (&value)[2], bool offset) { // 0: hue (-1.0-1.0), 1: saturation (0.0-2.0)
  clampValue(&value[0], -1.0f, 1.0f);
  clampValue(&value[1], 0.0f, 2.0f);

  _cameraControl.writeCommandFixed16(camera, 8, 6, (offset?1:0), value);
}
void ClientBMDCamCtrl::setCameraCorrectionReset(uint8_t camera) {
  _cameraControl.writeCommandVoid(camera, 8, 7);
}
