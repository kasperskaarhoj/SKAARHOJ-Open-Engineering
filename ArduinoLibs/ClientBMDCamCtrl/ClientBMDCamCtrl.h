#ifndef ClientBMDCamCtrl_h
#define ClientBMDCamCtrl_h

#define ClientBMDCamCtrl_Cams 10

#include "BMDSDIControl.h"
#include <Arduino.h>

class ClientBMDCamCtrl
{
private:
	BMD_SDICameraControl_I2C _cameraControl;
	BMD_SDITallyControl_I2C _tallyControl;
	bool _hasInitialized;

	float cameraIrisValue[ClientBMDCamCtrl_Cams];

	byte _cameraOutput[255];
	int _cameraOutPos;

	byte a[519];
	int pos;

public:
	ClientBMDCamCtrl();
	uint8_t _serialOutput;

	void serialOutput(uint8_t level);
	bool hasInitialized();

	void begin(uint8_t address);

	//void begin(BMD_SDICameraControl_I2C* cameraControl, BMD_SDITallyControl_I2C *tallyControl);
	void sendCameraPacket(byte* data, int len);
	void appendCameraPacket(byte* data, int len);
	void printPacket(byte *data, int len);
	void runLoop();

	// Lens commands
	void setFocus(uint8_t camera, float focus, bool offset = false); // Range: 0.0 - 1.0
	void setAutoFocus(uint8_t camera);
	void setIrisf(uint8_t camera, float iris, bool offset = false); // Range: -1.0 - 16.0
	void setIris(uint8_t camera, float iris, bool offset = false); // Range: 0.0- 1.0
	float getIris(uint8_t camera);
	void setAutoIris(uint8_t camera);
	void setOIS(uint8_t camera, bool state, bool offset = false);
	void setZoom(uint8_t camera, uint16_t zoom, bool offset = false);
	void setNormZoom(uint8_t camera, float zoom, bool offset = false);
	void setContinuousZoom(uint8_t camera, float rate, bool offset = false); // -1.0 wide, 0.0 stop, 1.0 tele

	// Video controls
	void setVideoMode(uint8_t camera, int8_t (&mode)[5]);
	void setSensorGain(uint8_t camera, int8_t gain, bool offset = false);
	void setWhiteBalance(uint8_t camera, int16_t wb, bool offset = false); // 3200 - 7500
	void setExposure(uint8_t camera, int32_t exposure, bool offset = false); // 1 - 32000 µs
	void setDynamicRangeMode(uint8_t camera, int8_t mode); // 0: film, 1: Video
	void setVideoSharpening(uint8_t camera, int8_t mode); // 0: off, 1: low, 2: medium, 3: high

	// Audio controls
	void setMicLevel(uint8_t camera, float level, bool offset = false); // 0.0 - 1.0
	void setHeadphoneLevel(uint8_t camera, float level, bool offset = false); // 0.0 - 1.0
	void setHeadphoneProgramMix(uint8_t camera, float level, bool offset = false); // 0.0 - 1.0
	void setSpeakerLevel(uint8_t camera, float level, bool offset = false); // 0.0 - 1.0
	void setInputType(uint8_t camera, int8_t type); // 0: Internal mic, 1: Line level, 2: Low mic level, 3: High mic level
	void setInputLevels(uint8_t camera, float (&level)[2], bool offset = false); // Range 0.0 - 1.0, [0]: ch0, [1]: ch1
	void setPhantomPower(uint8_t camera, bool state, bool offset = false); // True: powered

	// Output controls
	void setOverlays(uint8_t camera, uint16_t mask);

	// Display controls
	void setDisplayBrightness(uint8_t camera, float level, bool offset = false);// 0.0 - 1.0
	void setDisplayOverlays(uint8_t camera, uint8_t mask); // 0x4: Zebra, 0x8 peaking
	void setDisplayZebraLevels(uint8_t camera, float level, bool offset = false); // 0.0 - 1.0
	void setDisplayPeakingLevel(uint8_t camera, float level, bool offset = false);
	void setDisplayColorBarsTime(uint8_t camera, int8_t secs, bool offset = false); // 0: disable, 1-30: time to display bars

	// Tally controls
	void setTallyBrightness(uint8_t camera, float level, bool offset = false); // 0.0 - 1.0
	void setTallyFrontBrightness(uint8_t camera, float level, bool offset = false); // 0.0 - 1.0
	void setTallyRearBrightness(uint8_t camera, float level, bool offset = false); // 0.0 - 1.0

	// Reference controls
	void setReferenceSource(uint8_t camera, int8_t value); // 0: internal, 1: program, 2: external
	void setReferenceOffset(uint8_t camera, int32_t offset, bool offs = false);

	// Configuration controls
	void setClock(uint8_t camera, int32_t (&value)[2]); // 0: time, 1: date

	// Colour correction controls
	void setCameraLift(uint8_t camera, float (&value)[4], bool offset = false); // 0: red, 1: green, 2: blue, 3: luma (-2.0 - 2.0)
	void setCameraGamma(uint8_t camera, float (&value)[4], bool offset = false); // 0: red, 1: green, 2: blue, 3: luma (-4.0 - 4.0)
	void setCameraGain(uint8_t camera, float (&value)[4], bool offset = false); // 0: red, 1: green, 2: blue, 3: luma (0.0 - 16.0)
	void setCameraOffset(uint8_t camera, float (&value)[4], bool offset = false); // 0: red, 1: green, 2: blue, 3: luma (-8.0 - 8.0)
	void setCameraContrast(uint8_t camera, float (&value)[2], bool offset = false); // 0: pivot (0.0-1.0), 1: adjust (0.0 - 2.0)
	void setCameraLumaMix(uint8_t camera, float value, bool offset = false);  // 0.0 - 1.0
	void setCameraColourAdjust(uint8_t camera, float (&value)[2], bool offset = false); // 0: hue (-1.0-1.0), 1: saturation (0.0-2.0)
	void setCameraCorrectionReset(uint8_t camera);

};

#endif
