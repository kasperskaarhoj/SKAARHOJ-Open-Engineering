#include "ClientBMDCamCtrl.h"

ClientBMDCamCtrl::ClientBMDCamCtrl() { _hasInitialized = false; }

void ClientBMDCamCtrl::begin(uint8_t address) {

  for (int i = 0; i < ClientBMDCamCtrl_Cams; i++) {
    cameraIrisValue[i] = 0.00;
    cameraContrastValue[i][0] = 0.5;
    cameraContrastValue[i][1] = 1.0;
    cameraWBValue[i] = 3200;
    cameraExposureValue[i] = 10000;
    cameraSensorGainValue[i] = 2; // Apparently this is the lowest value

    for (int j = 0; j < 4; j++) {
      cameraGainValue[i][j] = 1.00;
    }

    cameraColourAdjustValue[i][0] = 0.0;
    cameraColourAdjustValue[i][1] = 1.0;
  }

  _cameraControl.begin(address);
  _tallyControl.begin(address);

  _cameraControl.setOverride(true);

  _hasInitialized = true;
}

char* ClientBMDCamCtrl::getOutputBuffer() {
  return _cameraControl.outputBuffer;
}

uint8_t ClientBMDCamCtrl::getOutputLength() {
  return _cameraControl.outputLength;
}

void ClientBMDCamCtrl::serialOutput(uint8_t level) { _serialOutput = level; }

bool ClientBMDCamCtrl::hasInitialized() { return _hasInitialized; }

void ClientBMDCamCtrl::runLoop() {
  unsigned long startTime = millis();
  if (_cameraControl.available()) {
    int availableLength = _cameraControl.read(a + pos, 519 - pos) + pos;

    if (availableLength > pos) {
      int i = 0;

      while (1) {
        int len = 0;

        if (i + 1 <= availableLength) {
          len = a[i + 1];
        }

        int totalPacket = (4 + len + 3) & ~0x03;

        if (totalPacket + i >= availableLength) {
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
    } else if (availableLength == -1) {
      Serial.println("FATAL ERROR: Received package did not fit into buffer! Deadlock.");
      while (1)
        ;
    }
    if (_cameraOutPos > 0) {
      /*Serial.print("Writing ");
      Serial.print(_cameraOutPos);
      Serial.println(" bytes to output.");

      Serial.print("Parsing: ");
      Serial.println(millis() - startTime);*/
      sendCameraPacket(_cameraOutput, _cameraOutPos);
      _cameraOutPos = 0;
      // Serial.print(" Parsing and sending: ");
      // Serial.println(millis() - startTime);
    }
  }
}

void ClientBMDCamCtrl::sendCameraPacket(byte *data, int len) {
  _cameraControl.write(data, len);
  // printPacket(data, len);
}

void ClientBMDCamCtrl::appendCameraPacket(byte *data, int len) {
  if (_cameraOutPos + len > 255) {
    sendCameraPacket(_cameraOutput, _cameraOutPos);
    _cameraOutPos = 0;
  }

  memcpy(_cameraOutput + _cameraOutPos, data, len);
  _cameraOutPos += len;
}

void ClientBMDCamCtrl::printPacket(byte *data, int len) {
  for (int i = 0; i < len; i++) {
    if (data[i] < 16) {
      Serial.print("0");
    }
    Serial.print(data[i], HEX);
    Serial.print(":");
  }
  Serial.println("");
}

template <typename T> inline void clampValue(T *value, T const &min, T const &max) {
  if (*value < min)
    *value = min;
  if (*value > max)
    *value = max;
}

// Lens commands
void ClientBMDCamCtrl::setFocus(uint8_t camera, float focus, bool offset) { // Range: 0.0 - 1.0
  clampValue(&focus, 0.0f, 1.0f);
  cameraFocusValue[camera - 1] = focus;
  _cameraControl.writeCommandFixed16(camera, 0, 0, (offset ? 1 : 0), focus);
}
void ClientBMDCamCtrl::setAutoFocus(uint8_t camera) { _cameraControl.writeCommandVoid(camera, 0, 1); }
void ClientBMDCamCtrl::setIrisf(uint8_t camera, float iris, bool offset) { // Range: -1.0 - 16.0
  clampValue(&iris, -1.0f, 16.0f);
  _cameraControl.writeCommandFixed16(camera, 0, 2, (offset ? 1 : 0), iris);
}
void ClientBMDCamCtrl::setIris(uint8_t camera, float iris, bool offset) { // Range: 0.0- 1.0
  clampValue(&iris, 0.0f, 1.0f);
  cameraIrisValue[camera - 1] = iris;
  _cameraControl.writeCommandFixed16(camera, 0, 3, (offset ? 1 : 0), iris);
}

void ClientBMDCamCtrl::setAutoIris(uint8_t camera) { _cameraControl.writeCommandVoid(camera, 0, 5); }
void ClientBMDCamCtrl::setOIS(uint8_t camera, bool state, bool offset) { _cameraControl.writeCommandBool(camera, 0, 6, (offset ? 1 : 0), state); }
void ClientBMDCamCtrl::setZoom(uint8_t camera, uint16_t zoom, bool offset) { _cameraControl.writeCommandInt16(camera, 0, 7, (offset ? 1 : 0), zoom); }

void ClientBMDCamCtrl::setNormZoom(uint8_t camera, float zoom, bool offset) {
  clampValue(&zoom, 0.0f, 1.0f);
  cameraZoomValue[camera - 1] = zoom;
  _cameraControl.writeCommandInt16(camera, 0, 8, (offset ? 1 : 0), zoom);
}

void ClientBMDCamCtrl::setContinuousZoom(uint8_t camera, float rate, bool offset) { // -1.0 wide, 0.0 stop, 1.0 tele
  clampValue(&rate, -1.0f, 1.0f);
  _cameraControl.writeCommandFixed16(camera, 0, 9, (offset ? 1 : 0), rate);
}

// Video controls
void ClientBMDCamCtrl::setVideoMode(uint8_t camera, int8_t (&mode)[5]) { _cameraControl.writeCommandInt8(camera, 1, 0, 0, mode); }
void ClientBMDCamCtrl::setSensorGain(uint8_t camera, int8_t gain, bool offset) {
  clampValue(&gain, (int8_t)1, (int8_t)16);
  cameraSensorGainValue[camera - 1] = gain;
  _cameraControl.writeCommandInt8(camera, 1, 1, (offset ? 1 : 0), gain);
}
void ClientBMDCamCtrl::setWhiteBalance(uint8_t camera, int16_t wb, bool offset) { // 3200 - 7500
  clampValue(&wb, (int16_t)3200, (int16_t)7500);
  cameraWBValue[camera - 1] = wb;
  _cameraControl.writeCommandInt16(camera, 1, 2, (offset ? 1 : 0), wb);
}
void ClientBMDCamCtrl::setExposure(uint8_t camera, int32_t exposure, bool offset) { // 1 - 32000 µs
  clampValue(&exposure, (int32_t)1, (int32_t)42000);
  cameraExposureValue[camera - 1] = exposure;
  _cameraControl.writeCommandInt32(camera, 1, 5, (offset ? 1 : 0), exposure);
}
void ClientBMDCamCtrl::setDynamicRangeMode(uint8_t camera, int8_t mode) { // 0: film, 1: Video
  if (mode == 0 || mode == 1) {
    _cameraControl.writeCommandInt8(camera, 1, 7, 0, mode);
  }
}
void ClientBMDCamCtrl::setVideoSharpening(uint8_t camera, int8_t mode) { // 0: off, 1: low, 2: medium, 3: high
  clampValue(&mode, (int8_t)0, (int8_t)3);
  _cameraControl.writeCommandInt8(camera, 1, 8, 0, mode);
}

// Audio controls
void ClientBMDCamCtrl::setMicLevel(uint8_t camera, float level, bool offset) { // 0.0 - 1.0
  clampValue(&level, 0.0f, 1.0f);
  cameraMicLevel[camera - 1] = level;
  _cameraControl.writeCommandFixed16(camera, 2, 0, (offset ? 1 : 0), level);
}
void ClientBMDCamCtrl::setHeadphoneLevel(uint8_t camera, float level, bool offset) { // 0.0 - 1.0
  clampValue(&level, 0.0f, 1.0f);
  cameraHeadphoneValue[camera - 1] = level;
  _cameraControl.writeCommandFixed16(camera, 2, 1, (offset ? 1 : 0), level);
}
void ClientBMDCamCtrl::setHeadphoneProgramMix(uint8_t camera, float level, bool offset) { // 0.0 - 1.0
  clampValue(&level, 0.0f, 1.0f);
  cameraHeadphoneProgramLevelValue[camera - 1] = level;
  _cameraControl.writeCommandFixed16(camera, 2, 2, (offset ? 1 : 0), level);
}
void ClientBMDCamCtrl::setSpeakerLevel(uint8_t camera, float level, bool offset) { // 0.0 - 1.0
  clampValue(&level, 0.0f, 1.0f);
  cameraSpeakerLevel[camera - 1] = level;
  _cameraControl.writeCommandFixed16(camera, 2, 3, (offset ? 1 : 0), level);
}

void ClientBMDCamCtrl::setInputType(uint8_t camera, int8_t type) { // 0: Internal mic, 1: Line level, 2: Low mic level, 3: High mic level
  clampValue(&type, (int8_t)0, (int8_t)3);
  cameraInputType[camera - 1] = type;
  _cameraControl.writeCommandInt8(camera, 2, 4, 0, type);
}

void ClientBMDCamCtrl::setInputLevels(uint8_t camera, float (&level)[2], bool offset) { // Range 0.0 - 1.0, [0]: ch0, [1]: ch1
  clampValue(&level[0], 0.0f, 1.0f);
  clampValue(&level[1], 0.0f, 1.0f);

  memcpy(cameraInputLevels[camera - 1], level, sizeof(level));
  _cameraControl.writeCommandFixed16(camera, 2, 5, (offset ? 1 : 0), level);
}
void ClientBMDCamCtrl::setPhantomPower(uint8_t camera, bool state, bool offset) { // True: powered
  cameraPhantomPower[camera - 1] = state;
  _cameraControl.writeCommandBool(camera, 2, 6, (offset ? 1 : 0), state);
}

// Output controls
void ClientBMDCamCtrl::setOverlays(uint8_t camera, uint16_t mask) {
  cameraOverlaysValue[camera - 1] = mask;
  _cameraControl.writeCommandInt16(camera, 3, 0, 0, mask);
}

// Display controls
void ClientBMDCamCtrl::setDisplayBrightness(uint8_t camera, float level, bool offset) { // 0.0 - 1.0
  clampValue(&level, 0.0f, 1.0f);
  cameraDisplayBrightnessValue[camera - 1] = level;
  _cameraControl.writeCommandFixed16(camera, 4, 0, (offset ? 1 : 0), level);
}
void ClientBMDCamCtrl::setDisplayOverlays(uint8_t camera, uint8_t mask) { // 0x4: Zebra, 0x8 peaking
  cameraDisplayOverlaysValue[camera - 1] = mask;
  _cameraControl.writeCommandInt16(camera, 4, 1, 0, mask);
}
void ClientBMDCamCtrl::setDisplayZebraLevels(uint8_t camera, float level, bool offset) { // 0.0 - 1.0
  clampValue(&level, 0.0f, 1.0f);
  cameraDisplayZebraValue[camera - 1] = level;
  _cameraControl.writeCommandFixed16(camera, 4, 2, (offset ? 1 : 0), level);
}
void ClientBMDCamCtrl::setDisplayPeakingLevel(uint8_t camera, float level, bool offset) {
  clampValue(&level, 0.0f, 1.0f);
  cameraDisplayPeakingValue[camera - 1] = level;
  _cameraControl.writeCommandFixed16(camera, 4, 3, (offset ? 1 : 0), level);
}
void ClientBMDCamCtrl::setDisplayColorBarsTime(uint8_t camera, int8_t secs, bool offset) { // 0: disable, 1-30: time to display bars
  clampValue(&secs, (int8_t)0, (int8_t)30);
  cameraDisplayColorbarsValue[camera - 1] = secs;
  _cameraControl.writeCommandInt8(camera, 4, 4, (offset ? 1 : 0), secs);
}

// Tally controls
void ClientBMDCamCtrl::setTallyBrightness(uint8_t camera, float level, bool offset) { // 0.0 - 1.0
  clampValue(&level, 0.0f, 1.0f);
  cameraTallyBrightnessValue[camera - 1] = level;
  cameraTallyRearBrightnessValue[camera - 1] = level;
  cameraTallyFrontBrightnessValue[camera - 1] = level;

  _cameraControl.writeCommandFixed16(camera, 5, 0, (offset ? 1 : 0), level);
}
void ClientBMDCamCtrl::setTallyFrontBrightness(uint8_t camera, float level, bool offset) { // 0.0 - 1.0
  clampValue(&level, 0.0f, 1.0f);
  cameraTallyFrontBrightnessValue[camera - 1] = level;
  _cameraControl.writeCommandFixed16(camera, 5, 1, (offset ? 1 : 0), level);
}
void ClientBMDCamCtrl::setTallyRearBrightness(uint8_t camera, float level, bool offset) { // 0.0 - 1.0
  clampValue(&level, 0.0f, 1.0f);
  cameraTallyRearBrightnessValue[camera - 1] = level;
  _cameraControl.writeCommandFixed16(camera, 5, 2, (offset ? 1 : 0), level);
}

// Reference controls
void ClientBMDCamCtrl::setReferenceSource(uint8_t camera, int8_t value) { // 0: internal, 1: program, 2: external
  if (value == 0 || value == 1 || value == 2) {
    cameraReferenceSourceValue[camera - 1] = value;
    _cameraControl.writeCommandInt8(camera, 6, 0, 0, value);
  }
}
void ClientBMDCamCtrl::setReferenceOffset(uint8_t camera, int32_t offset, bool offs) {
  cameraReferenceOffsetValue[camera - 1] = offset;
  _cameraControl.writeCommandInt32(camera, 6, 1, (offs ? 1 : 0), offset);
}

// Configuration controls
void ClientBMDCamCtrl::setClock(uint8_t camera, int32_t (&value)[2]) { // 0: time, 1: date
  memcpy(cameraClockValue[camera - 1], value, sizeof(value));
  _cameraControl.writeCommandInt32(camera, 7, 0, 0, value);
}

// Colour correction controls
void ClientBMDCamCtrl::setCameraLift(uint8_t camera, float (&value)[4], bool offset) { // 0: red, 1: green, 2: blue, 3: luma (-2.0 - 2.0)
  float(&existing)[4] = getCameraLift(camera);
  existing[0] = isnan(value[0]) ? existing[0] : value[0];
  existing[1] = isnan(value[1]) ? existing[1] : value[1];
  existing[2] = isnan(value[2]) ? existing[2] : value[2];
  existing[3] = isnan(value[3]) ? existing[3] : value[3];

  clampValue(&existing[0], -2.0f, 2.0f);
  clampValue(&existing[1], -2.0f, 2.0f);
  clampValue(&existing[2], -2.0f, 2.0f);
  clampValue(&existing[3], -2.0f, 2.0f);

  _cameraControl.writeCommandFixed16(camera, 8, 0, (offset ? 1 : 0), existing);
}
void ClientBMDCamCtrl::setCameraGamma(uint8_t camera, float (&value)[4], bool offset) { // 0: red, 1: green, 2: blue, 3: luma (-4.0 - 4.0)
  float(&existing)[4] = getCameraGamma(camera);
  existing[0] = isnan(value[0]) ? existing[0] : value[0];
  existing[1] = isnan(value[1]) ? existing[1] : value[1];
  existing[2] = isnan(value[2]) ? existing[2] : value[2];
  existing[3] = isnan(value[3]) ? existing[3] : value[3];

  clampValue(&existing[0], -4.0f, 4.0f);
  clampValue(&existing[1], -4.0f, 4.0f);
  clampValue(&existing[2], -4.0f, 4.0f);
  clampValue(&existing[3], -4.0f, 4.0f);

  _cameraControl.writeCommandFixed16(camera, 8, 1, (offset ? 1 : 0), existing);
}
void ClientBMDCamCtrl::setCameraGain(uint8_t camera, float (&value)[4], bool offset) { // 0: red, 1: green, 2: blue, 3: luma (0.0 - 16.0)
  float(&existing)[4] = getCameraGain(camera);
  existing[0] = isnan(value[0]) ? existing[0] : value[0];
  existing[1] = isnan(value[1]) ? existing[1] : value[1];
  existing[2] = isnan(value[2]) ? existing[2] : value[2];
  existing[3] = isnan(value[3]) ? existing[3] : value[3];

  clampValue(&existing[0], 0.0f, 16.0f);
  clampValue(&existing[1], 0.0f, 16.0f);
  clampValue(&existing[2], 0.0f, 16.0f);
  clampValue(&existing[3], 0.0f, 16.0f);

  _cameraControl.writeCommandFixed16(camera, 8, 2, (offset ? 1 : 0), existing);
}
void ClientBMDCamCtrl::setCameraOffset(uint8_t camera, float (&value)[4], bool offset) { // 0: red, 1: green, 2: blue, 3: luma (-8.0 - 8.0)
  float(&existing)[4] = getCameraOffset(camera);
  existing[0] = isnan(value[0]) ? existing[0] : value[0];
  existing[1] = isnan(value[1]) ? existing[1] : value[1];
  existing[2] = isnan(value[2]) ? existing[2] : value[2];
  existing[3] = isnan(value[3]) ? existing[3] : value[3];

  clampValue(&existing[0], -8.0f, 8.0f);
  clampValue(&existing[1], -8.0f, 8.0f);
  clampValue(&existing[2], -8.0f, 8.0f);
  clampValue(&existing[3], -8.0f, 8.0f);

  _cameraControl.writeCommandFixed16(camera, 8, 3, (offset ? 1 : 0), existing);
}
void ClientBMDCamCtrl::setCameraContrast(uint8_t camera, float (&value)[2], bool offset) { // 0: pivot (0.0-1.0), 1: adjust (0.0 - 2.0)
  float(&existing)[2] = getCameraContrast(camera);
  existing[0] = isnan(value[0]) ? existing[0] : value[0];
  existing[1] = isnan(value[1]) ? existing[1] : value[1];

  clampValue(&existing[0], 0.0f, 1.0f);
  clampValue(&existing[1], 0.0f, 2.0f);

  _cameraControl.writeCommandFixed16(camera, 8, 4, (offset ? 1 : 0), existing);
}
void ClientBMDCamCtrl::setCameraLumaMix(uint8_t camera, float value, bool offset) { // 0.0 - 1.0
  clampValue(&value, 0.0f, 1.0f);

  cameraLumaMixValue[camera - 1] = value;
  _cameraControl.writeCommandFixed16(camera, 8, 5, (offset ? 1 : 0), value);
}
void ClientBMDCamCtrl::setCameraColourAdjust(uint8_t camera, float (&value)[2], bool offset) { // 0: hue (-1.0-1.0), 1: saturation (0.0-2.0)
  float(&existing)[2] = getCameraColourAdjust(camera);
  existing[0] = isnan(value[0]) ? existing[0] : value[0];
  existing[1] = isnan(value[1]) ? existing[1] : value[1];

  clampValue(&existing[0], -1.0f, 1.0f);
  clampValue(&existing[1], 0.0f, 2.0f);

  _cameraControl.writeCommandFixed16(camera, 8, 6, (offset ? 1 : 0), existing);
}
void ClientBMDCamCtrl::setCameraCorrectionReset(uint8_t camera) { _cameraControl.writeCommandVoid(camera, 8, 7); }

// ------------------------
// Getter commands
// ------------------------

// Lens commands

float ClientBMDCamCtrl::getIris(uint8_t camera) { return cameraIrisValue[camera - 1]; }
float ClientBMDCamCtrl::getFocus(uint8_t camera) { return cameraFocusValue[camera - 1]; }
float ClientBMDCamCtrl::getNormZoom(uint8_t camera) { return cameraZoomValue[camera - 1]; }

// Video controls
int8_t ClientBMDCamCtrl::getSensorGain(uint8_t camera) { return cameraSensorGainValue[camera - 1]; }
int16_t ClientBMDCamCtrl::getWhiteBalance(uint8_t camera) { return cameraWBValue[camera - 1]; }
int32_t ClientBMDCamCtrl::getExposure(uint8_t camera) { return cameraExposureValue[camera - 1]; }

// Audio controls
float ClientBMDCamCtrl::getMicLevel(uint8_t camera) { return cameraMicLevel[camera - 1]; }
float ClientBMDCamCtrl::getHeadphoneLevel(uint8_t camera) { return cameraHeadphoneValue[camera - 1]; }
float ClientBMDCamCtrl::getHeadphoneProgramMix(uint8_t camera) { return cameraHeadphoneProgramLevelValue[camera - 1]; }
float ClientBMDCamCtrl::getSpeakerLevel(uint8_t camera) { return cameraSpeakerLevel[camera - 1]; }
int8_t ClientBMDCamCtrl::getInputType(uint8_t camera) { return cameraInputType[camera - 1]; }
float (&ClientBMDCamCtrl::getInputLevels(uint8_t camera))[2] { return cameraInputLevels[camera - 1]; }
bool ClientBMDCamCtrl::getPhantomPower(uint8_t camera) { return cameraPhantomPower[camera - 1]; }

// Output controls

// Display controls
float ClientBMDCamCtrl::getDisplayBrightness(uint8_t camera) { return cameraDisplayBrightnessValue[camera - 1]; }
uint8_t ClientBMDCamCtrl::getDisplayOverlays(uint8_t camera) { return cameraDisplayOverlaysValue[camera - 1]; }
float ClientBMDCamCtrl::getDisplayZebraLevels(uint8_t camera) { return cameraDisplayZebraValue[camera - 1]; }
float ClientBMDCamCtrl::getDisplayPeakingLevel(uint8_t camera) { return cameraDisplayPeakingValue[camera - 1]; }
int8_t ClientBMDCamCtrl::getDisplayColorBarsTime(uint8_t camera) { return cameraDisplayColorbarsValue[camera - 1]; }

// Tally controls
float ClientBMDCamCtrl::getTallyBrightness(uint8_t camera) { return cameraTallyBrightnessValue[camera - 1]; }
float ClientBMDCamCtrl::getTallyFrontBrightness(uint8_t camera) { return cameraTallyFrontBrightnessValue[camera - 1]; }
float ClientBMDCamCtrl::getTallyRearBrightness(uint8_t camera) { return cameraTallyRearBrightnessValue[camera - 1]; }

// Reference controls
int8_t ClientBMDCamCtrl::getReferenceSource(uint8_t camera) { return cameraReferenceSourceValue[camera - 1]; }

// Configuration controls
int32_t (&ClientBMDCamCtrl::getClock(uint8_t camera))[2] { return cameraClockValue[camera - 1]; }

// Colour correction controls
float (&ClientBMDCamCtrl::getCameraLift(uint8_t camera))[4] { return cameraLiftValue[camera - 1]; }
float (&ClientBMDCamCtrl::getCameraGamma(uint8_t camera))[4] { return cameraGammaValue[camera - 1]; }
float (&ClientBMDCamCtrl::getCameraGain(uint8_t camera))[4] { return cameraGainValue[camera - 1]; }
float (&ClientBMDCamCtrl::getCameraContrast(uint8_t camera))[2] { return cameraContrastValue[camera - 1]; }
float ClientBMDCamCtrl::getCameraLumaMix(uint8_t camera) { return cameraLumaMixValue[camera - 1]; }
float (&ClientBMDCamCtrl::getCameraColourAdjust(uint8_t camera))[2] { return cameraColourAdjustValue[camera - 1]; }
