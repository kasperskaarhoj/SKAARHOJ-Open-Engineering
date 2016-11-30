#ifndef ClientBMDCamCtrl_h
#define ClientBMDCamCtrl_h

#define ClientBMDCamCtrl_Cams 8

#include "BMDSDIControl.h"
#include <Arduino.h>

class ClientBMDCamCtrl {
private:
  void initColourCorrection(uint8_t cam);
  bool validCamera(uint8_t cam);

  BMD_SDICameraControl_I2C _cameraControl;
  BMD_SDITallyControl_I2C _tallyControl;
  bool _hasInitialized;

  uint16_t _previewTally;
  uint16_t _programTally;

  float cameraIrisValue[ClientBMDCamCtrl_Cams];
  float cameraFocusValue[ClientBMDCamCtrl_Cams];
  float cameraZoomValue[ClientBMDCamCtrl_Cams];
  float cameraContinuousZoomValue[ClientBMDCamCtrl_Cams];

  int8_t cameraSensorGainValue[ClientBMDCamCtrl_Cams];
  int16_t cameraWBValue[ClientBMDCamCtrl_Cams];
  int32_t cameraExposureValue[ClientBMDCamCtrl_Cams];

  float cameraMicLevel[ClientBMDCamCtrl_Cams];
  float cameraHeadphoneValue[ClientBMDCamCtrl_Cams];
  float cameraHeadphoneProgramLevelValue[ClientBMDCamCtrl_Cams];
  float cameraSpeakerLevel[ClientBMDCamCtrl_Cams];
  int8_t cameraInputType[ClientBMDCamCtrl_Cams];
  float cameraInputLevels[ClientBMDCamCtrl_Cams][2];
  bool cameraPhantomPower[ClientBMDCamCtrl_Cams];

  uint16_t cameraOverlaysValue[ClientBMDCamCtrl_Cams];

  float cameraDisplayBrightnessValue[ClientBMDCamCtrl_Cams];
  uint8_t cameraDisplayOverlaysValue[ClientBMDCamCtrl_Cams];
  float cameraDisplayZebraValue[ClientBMDCamCtrl_Cams];
  float cameraDisplayPeakingValue[ClientBMDCamCtrl_Cams];
  int8_t cameraDisplayColorbarsValue[ClientBMDCamCtrl_Cams];
  uint8_t cameraSharpeningLevel[ClientBMDCamCtrl_Cams];

  float cameraTallyBrightnessValue[ClientBMDCamCtrl_Cams];
  float cameraTallyRearBrightnessValue[ClientBMDCamCtrl_Cams];
  float cameraTallyFrontBrightnessValue[ClientBMDCamCtrl_Cams];

  int8_t cameraReferenceSourceValue[ClientBMDCamCtrl_Cams];
  int32_t cameraReferenceOffsetValue[ClientBMDCamCtrl_Cams];

  int32_t cameraClockValue[ClientBMDCamCtrl_Cams][2];

  float cameraLiftValue[ClientBMDCamCtrl_Cams][4];
  float cameraGammaValue[ClientBMDCamCtrl_Cams][4];
  float cameraGainValue[ClientBMDCamCtrl_Cams][4];
  float cameraOffsetValue[ClientBMDCamCtrl_Cams][4];
  float cameraContrastValue[ClientBMDCamCtrl_Cams][2];
  float cameraLumaMixValue[ClientBMDCamCtrl_Cams];
  float cameraColourAdjustValue[ClientBMDCamCtrl_Cams][2];

  byte _cameraOutput[255];
  int _cameraOutPos;

  byte a[519];
  int pos;

public:
  ClientBMDCamCtrl();
  uint8_t _serialOutput;

  char* getOutputBuffer();
  uint8_t getOutputLength();

  void serialOutput(uint8_t level);
  bool hasInitialized();

  void begin(uint8_t address);

  void cameraOverride(bool override);
  void tallyOverride(bool override);

  // Tally commands
  void setTally(uint8_t cam, bool programTally, bool previewTally);
  void getTally(uint8_t cam, bool &programTally, bool &previewTally);
  void getInternalTally(uint8_t cam, bool &programTally, bool &previewTally);
  
  // void begin(BMD_SDICameraControl_I2C* cameraControl, BMD_SDITallyControl_I2C *tallyControl);
  void sendCameraPacket(byte *data, int len);
  void appendCameraPacket(byte *data, int len);
  void printPacket(byte *data, int len);
  void runLoop();

  // Lens commands
  void setFocus(uint8_t camera, float focus, bool offset = false); // Range: 0.0 - 1.0
  float getFocus(uint8_t camera);
  void setAutoFocus(uint8_t camera);
  void setIrisf(uint8_t camera, float iris, bool offset = false); // Range: -1.0 - 16.0
  void setIris(uint8_t camera, float iris, bool offset = false);  // Range: 0.0- 1.0
  float getIris(uint8_t camera);
  void setAutoIris(uint8_t camera);
  void setOIS(uint8_t camera, bool state, bool offset = false);
  void setZoom(uint8_t camera, uint16_t zoom, bool offset = false);
  void setNormZoom(uint8_t camera, float zoom, bool offset = false);
  float getNormZoom(uint8_t camera);
  void setContinuousZoom(uint8_t camera, float rate, bool offset = false); // -1.0 wide, 0.0 stop, 1.0 tele
  float getContinuousZoom(uint8_t camera);

  // Video controls
  void setVideoMode(uint8_t camera, int8_t (&mode)[5]);
  void setSensorGain(uint8_t camera, int8_t gain, bool offset = false);
  int8_t getSensorGain(uint8_t camera);
  void setWhiteBalance(uint8_t camera, int16_t wb, bool offset = false); // 3200 - 7500
  int16_t getWhiteBalance(uint8_t camera);
  void setExposure(uint8_t camera, int32_t exposure, bool offset = false); // 1 - 32000 µs
  int32_t getExposure(uint8_t camera);
  void setDynamicRangeMode(uint8_t camera, int8_t mode); // 0: film, 1: Video
  void setVideoSharpening(uint8_t camera, int8_t mode);  // 0: off, 1: low, 2: medium, 3: high
  uint8_t getVideoSharpening(uint8_t camera);

  // Audio controls
  void setMicLevel(uint8_t camera, float level, bool offset = false); // 0.0 - 1.0
  float getMicLevel(uint8_t camera);
  void setHeadphoneLevel(uint8_t camera, float level, bool offset = false); // 0.0 - 1.0
  float getHeadphoneLevel(uint8_t camera);
  void setHeadphoneProgramMix(uint8_t camera, float level, bool offset = false); // 0.0 - 1.0
  float getHeadphoneProgramMix(uint8_t camera);
  void setSpeakerLevel(uint8_t camera, float level, bool offset = false); // 0.0 - 1.0
  float getSpeakerLevel(uint8_t camera);
  void setInputType(uint8_t camera, int8_t type); // 0: Internal mic, 1: Line level, 2: Low mic level, 3: High mic level
  int8_t getInputType(uint8_t camera);
  void setInputLevels(uint8_t camera, float (&level)[2], bool offset = false); // Range 0.0 - 1.0, [0]: ch0, [1]: ch1
  bool getInputLevels(uint8_t camera, float (&level)[2]);
  void setPhantomPower(uint8_t camera, bool state, bool offset = false); // True: powered
  bool getPhantomPower(uint8_t camera);

  // Output controls
  void setOverlays(uint8_t camera, uint16_t mask);

  // Display controls
  void setDisplayBrightness(uint8_t camera, float level, bool offset = false); // 0.0 - 1.0
  float getDisplayBrightness(uint8_t camera);
  void setDisplayOverlays(uint8_t camera, uint8_t mask); // 0x4: Zebra, 0x8 peaking
  uint8_t getDisplayOverlays(uint8_t camera);
  void setDisplayZebraLevels(uint8_t camera, float level, bool offset = false); // 0.0 - 1.0
  float getDisplayZebraLevels(uint8_t camera);
  void setDisplayPeakingLevel(uint8_t camera, float level, bool offset = false);
  float getDisplayPeakingLevel(uint8_t camera);
  void setDisplayColorBarsTime(uint8_t camera, int8_t secs, bool offset = false); // 0: disable, 1-30: time to display bars
  int8_t getDisplayColorBarsTime(uint8_t camera);

  // Tally controls
  void setTallyBrightness(uint8_t camera, float level, bool offset = false); // 0.0 - 1.0
  float getTallyBrightness(uint8_t camera);
  void setTallyFrontBrightness(uint8_t camera, float level, bool offset = false); // 0.0 - 1.0
  float getTallyFrontBrightness(uint8_t camera);
  void setTallyRearBrightness(uint8_t camera, float level, bool offset = false); // 0.0 - 1.0
  float getTallyRearBrightness(uint8_t camera);

  // Reference controls
  void setReferenceSource(uint8_t camera, int8_t value); // 0: internal, 1: program, 2: external
  int8_t getReferenceSource(uint8_t camera);
  void setReferenceOffset(uint8_t camera, int32_t offset, bool offs = false);
  int32_t getReferenceOffset(uint8_t camera);

  // Configuration controls
  void setClock(uint8_t camera, int32_t (&value)[2]); // 0: time, 1: date
  bool getClock(uint8_t camera, int32_t (&value)[2]);

  // Colour correction controls
  void setCameraLift(uint8_t camera, float (&lift)[4], bool offset = false); // 0: red, 1: green, 2: blue, 3: luma (-2.0 - 2.0)
  bool getCameraLift(uint8_t camera, float (&lift)[4]);
  void setCameraGamma(uint8_t camera, float (&gamma)[4], bool offset = false); // 0: red, 1: green, 2: blue, 3: luma (-4.0 - 4.0)
  bool getCameraGamma(uint8_t camera, float (&gamma)[4]);
  void setCameraGain(uint8_t camera, float (&gain)[4], bool offset = false); // 0: red, 1: green, 2: blue, 3: luma (0.0 - 16.0)
  bool getCameraGain(uint8_t camera, float (&gain)[4]);
  void setCameraOffset(uint8_t camera, float (&value)[4], bool offset = false); // 0: red, 1: green, 2: blue, 3: luma (-8.0 - 8.0)
  bool getCameraOffset(uint8_t camera, float (&offset)[4]);
  void setCameraContrast(uint8_t camera, float (&contrast)[2], bool offset = false); // 0: pivot (0.0-1.0), 1: adjust (0.0 - 2.0)
  bool getCameraContrast(uint8_t camera, float (&contrast)[2]);
  void setCameraLumaMix(uint8_t camera, float value, bool offset = false); // 0.0 - 1.0
  float getCameraLumaMix(uint8_t camera);
  void setCameraColourAdjust(uint8_t camera, float (&adjust)[2], bool offset = false); // 0: hue (-1.0-1.0), 1: saturation (0.0-2.0)
  bool getCameraColourAdjust(uint8_t camera, float (&adjust)[2]);
  void setCameraCorrectionReset(uint8_t camera);

  // Pan-tilt controls
  void setServoSpeed(uint8_t camera, uint8_t num, int16_t speed);
  void setServoPosition(uint8_t camera, uint8_t num, uint16_t position);
};

#endif
