/*
Copyright 2012-2014 Kasper Skårhøj, SKAARHOJ K/S, kasper@skaarhoj.com

This file is part of the Blackmagic Design ATEM Client library for Arduino

The ATEM library is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published by the 
Free Software Foundation, either version 3 of the License, or (at your 
option) any later version.

The ATEM library is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
or FITNESS FOR A PARTICULAR PURPOSE. 
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along 
with the ATEM library. If not, see http://www.gnu.org/licenses/.


IMPORTANT: If you want to use this library in your own projects and/or products,
please play a fair game and heed the license rules! See our web page for a Q&A so
you can keep a clear conscience: http://skaarhoj.com/about/licenses/


*/




#ifndef ATEMuni_h
#define ATEMuni_h

#include "Arduino.h"
#include "ATEMbase.h"
#include "EthernetUdp.h"


class ATEMuni : public ATEMbase
{
  public:
	ATEMuni();  
	  
	  
	uint8_t getTallyFlags(uint16_t videoSource);
	uint8_t getAudioTallyFlags(uint16_t audioSource);

	void setCameraControlVideomode(uint8_t input, uint8_t fps, uint8_t resolution, uint8_t interlaced, uint8_t mrate = 0);
	void setCameraControlLift(uint8_t input, int liftR, int liftG, int liftB, int liftY);
	void setCameraControlGamma(uint8_t input, int gammaR, int gammaG, int gammaB, int gammaY);
	void setCameraControlGain(uint8_t input, int gainR, int gainG, int gainB, int gainY);
	void setCameraControlHueSaturation(uint8_t input, int hue, int saturation);





// *********************************
// **
// ** Implementations in ATEMuni.h:
// **
// *********************************






// *********************************
// **
// ** Implementations in ATEMuni.h:
// **
// *********************************



private:
	void _parseGetCommands(const char *cmdStr);

			// Private Variables in ATEM.h:
	
			char atemInputLongName[47][21];
			char atemInputShortName[47][5];
			uint8_t atemInputAvailability[47];
			uint8_t atemInputMEAvailability[47];
			uint16_t atemMultiViewerInputVideoSource[2][10];
			uint16_t atemProgramInputVideoSource[2];
			uint16_t atemPreviewInputVideoSource[2];
			uint8_t atemTransitionStyle[2];
			uint8_t atemTransitionNextTransition[2];
			uint8_t atemTransitionStyleNext[2];
			uint8_t atemTransitionNextTransitionNext[2];
			bool atemTransitionInTransition[2];
			uint8_t atemTransitionFramesRemaining[2];
			uint16_t atemTransitionPosition[2];
			uint8_t atemTransitionMixRate[2];
			uint8_t atemTransitionDipRate[2];
			uint16_t atemTransitionDipInput[2];
			uint8_t atemTransitionWipeRate[2];
			uint8_t atemTransitionWipePattern[2];
			uint16_t atemTransitionWipeWidth[2];
			uint16_t atemTransitionWipeFillSource[2];
			uint16_t atemTransitionWipeSymmetry[2];
			uint16_t atemTransitionWipeSoftness[2];
			uint16_t atemTransitionWipePositionX[2];
			uint16_t atemTransitionWipePositionY[2];
			bool atemTransitionWipeReverse[2];
			bool atemTransitionWipeFlipFlop[2];
			uint8_t atemTransitionDVERate[2];
			uint8_t atemTransitionDVEStyle[2];
			uint16_t atemTransitionDVEFillSource[2];
			uint16_t atemTransitionDVEKeySource[2];
			bool atemTransitionDVEEnableKey[2];
			bool atemTransitionDVEPreMultiplied[2];
			uint16_t atemTransitionDVEClip[2];
			uint16_t atemTransitionDVEGain[2];
			bool atemTransitionDVEInvertKey[2];
			bool atemTransitionDVEReverse[2];
			bool atemTransitionDVEFlipFlop[2];
			bool atemKeyerOnAirEnabled[2][4];
			uint8_t atemKeyerType[2][4];
			bool atemKeyerFlyEnabled[2][4];
			uint16_t atemKeyerFillSource[2][4];
			uint16_t atemKeyerKeySource[2][4];
			bool atemKeyerMasked[2][4];
			int16_t atemKeyerTop[2][4];
			int16_t atemKeyerBottom[2][4];
			int16_t atemKeyerLeft[2][4];
			int16_t atemKeyerRight[2][4];
			int32_t atemKeyDVESizeX[2][4];
			int32_t atemKeyDVESizeY[2][4];
			int32_t atemKeyDVEPositionX[2][4];
			int32_t atemKeyDVEPositionY[2][4];
			int32_t atemKeyDVERotation[2][4];
			bool atemKeyDVEBorderEnabled[2][4];
			bool atemKeyDVEShadow[2][4];
			uint8_t atemKeyDVEBorderBevel[2][4];
			uint16_t atemKeyDVEBorderOuterWidth[2][4];
			uint16_t atemKeyDVEBorderInnerWidth[2][4];
			uint8_t atemKeyDVEBorderOuterSoftness[2][4];
			uint8_t atemKeyDVEBorderInnerSoftness[2][4];
			uint8_t atemKeyDVEBorderBevelSoftness[2][4];
			uint8_t atemKeyDVEBorderBevelPosition[2][4];
			uint8_t atemKeyDVEBorderOpacity[2][4];
			uint16_t atemKeyDVEBorderHue[2][4];
			uint16_t atemKeyDVEBorderSaturation[2][4];
			uint16_t atemKeyDVEBorderLuma[2][4];
			uint16_t atemKeyDVELightSourceDirection[2][4];
			uint8_t atemKeyDVELightSourceAltitude[2][4];
			bool atemKeyDVEMasked[2][4];
			int16_t atemKeyDVETop[2][4];
			int16_t atemKeyDVEBottom[2][4];
			int16_t atemKeyDVELeft[2][4];
			int16_t atemKeyDVERight[2][4];
			uint8_t atemKeyDVERate[2][4];
			uint16_t atemDownstreamKeyerFillSource[2];
			uint16_t atemDownstreamKeyerKeySource[2];
			bool atemDownstreamKeyerTie[2];
			uint8_t atemDownstreamKeyerRate[2];
			bool atemDownstreamKeyerPreMultiplied[2];
			uint16_t atemDownstreamKeyerClip[2];
			uint16_t atemDownstreamKeyerGain[2];
			bool atemDownstreamKeyerInvertKey[2];
			bool atemDownstreamKeyerMasked[2];
			int16_t atemDownstreamKeyerTop[2];
			int16_t atemDownstreamKeyerBottom[2];
			int16_t atemDownstreamKeyerLeft[2];
			int16_t atemDownstreamKeyerRight[2];
			bool atemDownstreamKeyerOnAir[2];
			bool atemDownstreamKeyerInTransition[2];
			bool atemDownstreamKeyerIsAutoTransitioning[2];
			uint8_t atemDownstreamKeyerFramesRemaining[2];
			uint8_t atemFadeToBlackRate[2];
			bool atemFadeToBlackStateFullyBlack[2];
			bool atemFadeToBlackStateInTransition[2];
			uint8_t atemFadeToBlackStateFramesRemaining[2];
			uint16_t atemAuxSourceInput[6];
			int16_t atemCameraControlIris[21];
			int16_t atemCameraControlFocus[21];
			int16_t atemCameraControlGain[21];
			int16_t atemCameraControlWhiteBalance[21];
			int16_t atemCameraControlSharpeningLevel[21];
			int16_t atemCameraControlZoomNormalized[21];
			int16_t atemCameraControlZoomSpeed[21];
			int16_t atemCameraControlColorbars[21];
			int16_t atemCameraControlLiftR[21];
			int16_t atemCameraControlGammaR[21];
			int16_t atemCameraControlGainR[21];
			int16_t atemCameraControlLumMix[21];
			int16_t atemCameraControlHue[21];
			int16_t atemCameraControlShutter[21];
			int16_t atemCameraControlLiftG[21];
			int16_t atemCameraControlGammaG[21];
			int16_t atemCameraControlGainG[21];
			int16_t atemCameraControlContrast[21];
			int16_t atemCameraControlSaturation[21];
			int16_t atemCameraControlLiftB[21];
			int16_t atemCameraControlGammaB[21];
			int16_t atemCameraControlGainB[21];
			int16_t atemCameraControlLiftY[21];
			int16_t atemCameraControlGammaY[21];
			int16_t atemCameraControlGainY[21];
			uint8_t atemMediaPlayerSourceType[2];
			uint8_t atemMediaPlayerSourceStillIndex[2];
			uint8_t atemMediaPlayerSourceClipIndex[2];
			bool atemMediaPlayerStillFilesIsUsed[32];
			char atemMediaPlayerStillFilesFileName[32][17];
			uint8_t atemMacroRunStatusState;
			bool atemMacroRunStatusIsLooping;
			uint16_t atemMacroRunStatusIndex;
			bool atemMacroPropertiesIsUsed[10];
			char atemMacroPropertiesName[10][11];
			uint16_t atemSuperSourceFillSource;
			uint16_t atemSuperSourceKeySource;
			bool atemSuperSourceForeground;
			bool atemSuperSourcePreMultiplied;
			uint16_t atemSuperSourceClip;
			uint16_t atemSuperSourceGain;
			bool atemSuperSourceInvertKey;
			bool atemSuperSourceBorderEnabled;
			uint8_t atemSuperSourceBorderBevel;
			uint16_t atemSuperSourceBorderOuterWidth;
			uint16_t atemSuperSourceBorderInnerWidth;
			uint8_t atemSuperSourceBorderOuterSoftness;
			uint8_t atemSuperSourceBorderInnerSoftness;
			uint8_t atemSuperSourceBorderBevelSoftness;
			uint8_t atemSuperSourceBorderBevelPosition;
			uint16_t atemSuperSourceBorderHue;
			uint16_t atemSuperSourceBorderSaturation;
			uint16_t atemSuperSourceBorderLuma;
			uint16_t atemSuperSourceLightSourceDirection;
			uint8_t atemSuperSourceLightSourceAltitude;
			bool atemSuperSourceBoxParametersEnabled[4];
			uint16_t atemSuperSourceBoxParametersInputSource[4];
			int16_t atemSuperSourceBoxParametersPositionX[4];
			int16_t atemSuperSourceBoxParametersPositionY[4];
			uint16_t atemSuperSourceBoxParametersSize[4];
			bool atemSuperSourceBoxParametersCropped[4];
			uint16_t atemSuperSourceBoxParametersCropTop[4];
			uint16_t atemSuperSourceBoxParametersCropBottom[4];
			uint16_t atemSuperSourceBoxParametersCropLeft[4];
			uint16_t atemSuperSourceBoxParametersCropRight[4];
			uint8_t atemAudioMixerInputMixOption[25];
			uint16_t atemAudioMixerInputVolume[25];
			int16_t atemAudioMixerInputBalance[25];
			uint16_t atemAudioMixerMasterVolume;
			bool atemAudioMixerMonitorMonitorAudio;
			uint16_t atemAudioMixerMonitorVolume;
			bool atemAudioMixerMonitorMute;
			bool atemAudioMixerMonitorSolo;
			uint16_t atemAudioMixerMonitorSoloInput;
			bool atemAudioMixerMonitorDim;

				uint8_t atemAudioMixerLevelsSources;
				uint16_t atemAudioMixerLevelsMasterLeft;
				uint16_t atemAudioMixerLevelsMasterRight;
				uint16_t atemAudioMixerLevelsMasterPeakLeft;
				uint16_t atemAudioMixerLevelsMasterPeakRight;
				uint16_t atemAudioMixerLevelsMonitor;
				uint16_t atemAudioMixerLevelsSourceOrder[24];
				uint16_t atemAudioMixerLevelsSourceLeft[24];
				uint16_t atemAudioMixerLevelsSourceRight[24];
				uint16_t atemAudioMixerLevelsSourcePeakLeft[24];
				uint16_t atemAudioMixerLevelsSourcePeakRight[24];
				
			uint16_t atemAudioMixerTallySources;
			uint16_t atemAudioMixerTallyAudioSource[25];
			bool atemAudioMixerTallyIsMixedIn[25];
			uint16_t atemTallyBySourceSources;
			uint16_t atemTallyBySourceVideoSource[47];
			uint8_t atemTallyBySourceTallyFlags[47];

public:
			// Public Methods in ATEM.h:
	
			char *  getInputLongName(uint16_t videoSource);
			char *  getInputShortName(uint16_t videoSource);
			uint8_t getInputAvailability(uint16_t videoSource);
			uint8_t getInputMEAvailability(uint16_t videoSource);
			uint16_t getMultiViewerInputVideoSource(uint8_t multiViewer, uint8_t windowIndex);
			uint16_t getProgramInputVideoSource(uint8_t mE);
			void setProgramInputVideoSource(uint8_t mE, uint16_t videoSource);
			uint16_t getPreviewInputVideoSource(uint8_t mE);
			void setPreviewInputVideoSource(uint8_t mE, uint16_t videoSource);
			void performCutME(uint8_t mE);
			void performAutoME(uint8_t mE);
			uint8_t getTransitionStyle(uint8_t mE);
			uint8_t getTransitionNextTransition(uint8_t mE);
			uint8_t getTransitionStyleNext(uint8_t mE);
			uint8_t getTransitionNextTransitionNext(uint8_t mE);
			void setTransitionStyle(uint8_t mE, uint8_t style);
			void setTransitionNextTransition(uint8_t mE, uint8_t nextTransition);
			bool getTransitionInTransition(uint8_t mE);
			uint8_t getTransitionFramesRemaining(uint8_t mE);
			uint16_t getTransitionPosition(uint8_t mE);
			void setTransitionPosition(uint8_t mE, uint16_t position);
			uint8_t getTransitionMixRate(uint8_t mE);
			void setTransitionMixRate(uint8_t mE, uint8_t rate);
			uint8_t getTransitionDipRate(uint8_t mE);
			uint16_t getTransitionDipInput(uint8_t mE);
			void setTransitionDipRate(uint8_t mE, uint8_t rate);
			void setTransitionDipInput(uint8_t mE, uint16_t input);
			uint8_t getTransitionWipeRate(uint8_t mE);
			uint8_t getTransitionWipePattern(uint8_t mE);
			uint16_t getTransitionWipeWidth(uint8_t mE);
			uint16_t getTransitionWipeFillSource(uint8_t mE);
			uint16_t getTransitionWipeSymmetry(uint8_t mE);
			uint16_t getTransitionWipeSoftness(uint8_t mE);
			uint16_t getTransitionWipePositionX(uint8_t mE);
			uint16_t getTransitionWipePositionY(uint8_t mE);
			bool getTransitionWipeReverse(uint8_t mE);
			bool getTransitionWipeFlipFlop(uint8_t mE);
			void setTransitionWipeRate(uint8_t mE, uint8_t rate);
			void setTransitionWipePattern(uint8_t mE, uint8_t pattern);
			void setTransitionWipeWidth(uint8_t mE, uint16_t width);
			void setTransitionWipeFillSource(uint8_t mE, uint16_t fillSource);
			void setTransitionWipeSymmetry(uint8_t mE, uint16_t symmetry);
			void setTransitionWipeSoftness(uint8_t mE, uint16_t softness);
			void setTransitionWipePositionX(uint8_t mE, uint16_t positionX);
			void setTransitionWipePositionY(uint8_t mE, uint16_t positionY);
			void setTransitionWipeReverse(uint8_t mE, bool reverse);
			void setTransitionWipeFlipFlop(uint8_t mE, bool flipFlop);
			uint8_t getTransitionDVERate(uint8_t mE);
			uint8_t getTransitionDVEStyle(uint8_t mE);
			uint16_t getTransitionDVEFillSource(uint8_t mE);
			uint16_t getTransitionDVEKeySource(uint8_t mE);
			bool getTransitionDVEEnableKey(uint8_t mE);
			bool getTransitionDVEPreMultiplied(uint8_t mE);
			uint16_t getTransitionDVEClip(uint8_t mE);
			uint16_t getTransitionDVEGain(uint8_t mE);
			bool getTransitionDVEInvertKey(uint8_t mE);
			bool getTransitionDVEReverse(uint8_t mE);
			bool getTransitionDVEFlipFlop(uint8_t mE);
			void setTransitionDVERate(uint8_t mE, uint8_t rate);
			void setTransitionDVEStyle(uint8_t mE, uint8_t style);
			void setTransitionDVEFillSource(uint8_t mE, uint16_t fillSource);
			void setTransitionDVEKeySource(uint8_t mE, uint16_t keySource);
			void setTransitionDVEEnableKey(uint8_t mE, bool enableKey);
			void setTransitionDVEPreMultiplied(uint8_t mE, bool preMultiplied);
			void setTransitionDVEClip(uint8_t mE, uint16_t clip);
			void setTransitionDVEGain(uint8_t mE, uint16_t gain);
			void setTransitionDVEInvertKey(uint8_t mE, bool invertKey);
			void setTransitionDVEReverse(uint8_t mE, bool reverse);
			void setTransitionDVEFlipFlop(uint8_t mE, bool flipFlop);
			bool getKeyerOnAirEnabled(uint8_t mE, uint8_t keyer);
			void setKeyerOnAirEnabled(uint8_t mE, uint8_t keyer, bool enabled);
			uint8_t getKeyerType(uint8_t mE, uint8_t keyer);
			bool getKeyerFlyEnabled(uint8_t mE, uint8_t keyer);
			uint16_t getKeyerFillSource(uint8_t mE, uint8_t keyer);
			uint16_t getKeyerKeySource(uint8_t mE, uint8_t keyer);
			bool getKeyerMasked(uint8_t mE, uint8_t keyer);
			int16_t getKeyerTop(uint8_t mE, uint8_t keyer);
			int16_t getKeyerBottom(uint8_t mE, uint8_t keyer);
			int16_t getKeyerLeft(uint8_t mE, uint8_t keyer);
			int16_t getKeyerRight(uint8_t mE, uint8_t keyer);
			void setKeyerFillSource(uint8_t mE, uint8_t keyer, uint16_t fillSource);
			void setKeyerKeySource(uint8_t mE, uint8_t keyer, uint16_t keySource);
			int32_t getKeyDVESizeX(uint8_t mE, uint8_t keyer);
			int32_t getKeyDVESizeY(uint8_t mE, uint8_t keyer);
			int32_t getKeyDVEPositionX(uint8_t mE, uint8_t keyer);
			int32_t getKeyDVEPositionY(uint8_t mE, uint8_t keyer);
			int32_t getKeyDVERotation(uint8_t mE, uint8_t keyer);
			bool getKeyDVEBorderEnabled(uint8_t mE, uint8_t keyer);
			bool getKeyDVEShadow(uint8_t mE, uint8_t keyer);
			uint8_t getKeyDVEBorderBevel(uint8_t mE, uint8_t keyer);
			uint16_t getKeyDVEBorderOuterWidth(uint8_t mE, uint8_t keyer);
			uint16_t getKeyDVEBorderInnerWidth(uint8_t mE, uint8_t keyer);
			uint8_t getKeyDVEBorderOuterSoftness(uint8_t mE, uint8_t keyer);
			uint8_t getKeyDVEBorderInnerSoftness(uint8_t mE, uint8_t keyer);
			uint8_t getKeyDVEBorderBevelSoftness(uint8_t mE, uint8_t keyer);
			uint8_t getKeyDVEBorderBevelPosition(uint8_t mE, uint8_t keyer);
			uint8_t getKeyDVEBorderOpacity(uint8_t mE, uint8_t keyer);
			uint16_t getKeyDVEBorderHue(uint8_t mE, uint8_t keyer);
			uint16_t getKeyDVEBorderSaturation(uint8_t mE, uint8_t keyer);
			uint16_t getKeyDVEBorderLuma(uint8_t mE, uint8_t keyer);
			uint16_t getKeyDVELightSourceDirection(uint8_t mE, uint8_t keyer);
			uint8_t getKeyDVELightSourceAltitude(uint8_t mE, uint8_t keyer);
			bool getKeyDVEMasked(uint8_t mE, uint8_t keyer);
			int16_t getKeyDVETop(uint8_t mE, uint8_t keyer);
			int16_t getKeyDVEBottom(uint8_t mE, uint8_t keyer);
			int16_t getKeyDVELeft(uint8_t mE, uint8_t keyer);
			int16_t getKeyDVERight(uint8_t mE, uint8_t keyer);
			uint8_t getKeyDVERate(uint8_t mE, uint8_t keyer);
			void setKeyDVESizeX(uint8_t mE, uint8_t keyer, int32_t sizeX);
			void setKeyDVESizeY(uint8_t mE, uint8_t keyer, int32_t sizeY);
			void setKeyDVEPositionX(uint8_t mE, uint8_t keyer, int32_t positionX);
			void setKeyDVEPositionY(uint8_t mE, uint8_t keyer, int32_t positionY);
			void setKeyDVERotation(uint8_t mE, uint8_t keyer, int32_t rotation);
			void setKeyDVEBorderEnabled(uint8_t mE, uint8_t keyer, bool borderEnabled);
			void setKeyDVEShadow(uint8_t mE, uint8_t keyer, bool shadow);
			void setKeyDVEBorderBevel(uint8_t mE, uint8_t keyer, uint8_t borderBevel);
			void setKeyDVEBorderOuterWidth(uint8_t mE, uint8_t keyer, uint16_t borderOuterWidth);
			void setKeyDVEBorderInnerWidth(uint8_t mE, uint8_t keyer, uint16_t borderInnerWidth);
			void setKeyDVEBorderOuterSoftness(uint8_t mE, uint8_t keyer, uint8_t borderOuterSoftness);
			void setKeyDVEBorderInnerSoftness(uint8_t mE, uint8_t keyer, uint8_t borderInnerSoftness);
			void setKeyDVEBorderBevelSoftness(uint8_t mE, uint8_t keyer, uint8_t borderBevelSoftness);
			void setKeyDVEBorderBevelPosition(uint8_t mE, uint8_t keyer, uint8_t borderBevelPosition);
			void setKeyDVEBorderOpacity(uint8_t mE, uint8_t keyer, uint8_t borderOpacity);
			void setKeyDVEBorderHue(uint8_t mE, uint8_t keyer, uint16_t borderHue);
			void setKeyDVEBorderSaturation(uint8_t mE, uint8_t keyer, uint16_t borderSaturation);
			void setKeyDVEBorderLuma(uint8_t mE, uint8_t keyer, uint16_t borderLuma);
			void setKeyDVELightSourceDirection(uint8_t mE, uint8_t keyer, uint16_t lightSourceDirection);
			void setKeyDVELightSourceAltitude(uint8_t mE, uint8_t keyer, uint8_t lightSourceAltitude);
			void setKeyDVEMasked(uint8_t mE, uint8_t keyer, bool masked);
			void setKeyDVETop(uint8_t mE, uint8_t keyer, int16_t top);
			void setKeyDVEBottom(uint8_t mE, uint8_t keyer, int16_t bottom);
			void setKeyDVELeft(uint8_t mE, uint8_t keyer, int16_t left);
			void setKeyDVERight(uint8_t mE, uint8_t keyer, int16_t right);
			void setKeyDVERate(uint8_t mE, uint8_t keyer, uint8_t rate);
			uint16_t getDownstreamKeyerFillSource(uint8_t keyer);
			uint16_t getDownstreamKeyerKeySource(uint8_t keyer);
			void setDownstreamKeyerFillSource(uint8_t keyer, uint16_t fillSource);
			void setDownstreamKeyerKeySource(uint8_t keyer, uint16_t keySource);
			bool getDownstreamKeyerTie(uint8_t keyer);
			uint8_t getDownstreamKeyerRate(uint8_t keyer);
			bool getDownstreamKeyerPreMultiplied(uint8_t keyer);
			uint16_t getDownstreamKeyerClip(uint8_t keyer);
			uint16_t getDownstreamKeyerGain(uint8_t keyer);
			bool getDownstreamKeyerInvertKey(uint8_t keyer);
			bool getDownstreamKeyerMasked(uint8_t keyer);
			int16_t getDownstreamKeyerTop(uint8_t keyer);
			int16_t getDownstreamKeyerBottom(uint8_t keyer);
			int16_t getDownstreamKeyerLeft(uint8_t keyer);
			int16_t getDownstreamKeyerRight(uint8_t keyer);
			void setDownstreamKeyerRate(uint8_t keyer, uint8_t rate);
			void performDownstreamKeyerAutoKeyer(uint8_t keyer);
			bool getDownstreamKeyerOnAir(uint8_t keyer);
			bool getDownstreamKeyerInTransition(uint8_t keyer);
			bool getDownstreamKeyerIsAutoTransitioning(uint8_t keyer);
			uint8_t getDownstreamKeyerFramesRemaining(uint8_t keyer);
			void setDownstreamKeyerOnAir(uint8_t keyer, bool onAir);
			uint8_t getFadeToBlackRate(uint8_t mE);
			void setFadeToBlackRate(uint8_t mE, uint8_t rate);
			bool getFadeToBlackStateFullyBlack(uint8_t mE);
			bool getFadeToBlackStateInTransition(uint8_t mE);
			uint8_t getFadeToBlackStateFramesRemaining(uint8_t mE);
			void performFadeToBlackME(uint8_t mE);
			uint16_t getAuxSourceInput(uint8_t aUXChannel);
			void setAuxSourceInput(uint8_t aUXChannel, uint16_t input);
			int16_t getCameraControlIris(uint8_t input);
			int16_t getCameraControlFocus(uint8_t input);
			int16_t getCameraControlGain(uint8_t input);
			int16_t getCameraControlWhiteBalance(uint8_t input);
			int16_t getCameraControlSharpeningLevel(uint8_t input);
			int16_t getCameraControlZoomNormalized(uint8_t input);
			int16_t getCameraControlZoomSpeed(uint8_t input);
			int16_t getCameraControlColorbars(uint8_t input);
			int16_t getCameraControlLiftR(uint8_t input);
			int16_t getCameraControlGammaR(uint8_t input);
			int16_t getCameraControlGainR(uint8_t input);
			int16_t getCameraControlLumMix(uint8_t input);
			int16_t getCameraControlHue(uint8_t input);
			int16_t getCameraControlShutter(uint8_t input);
			int16_t getCameraControlLiftG(uint8_t input);
			int16_t getCameraControlGammaG(uint8_t input);
			int16_t getCameraControlGainG(uint8_t input);
			int16_t getCameraControlContrast(uint8_t input);
			int16_t getCameraControlSaturation(uint8_t input);
			int16_t getCameraControlLiftB(uint8_t input);
			int16_t getCameraControlGammaB(uint8_t input);
			int16_t getCameraControlGainB(uint8_t input);
			int16_t getCameraControlLiftY(uint8_t input);
			int16_t getCameraControlGammaY(uint8_t input);
			int16_t getCameraControlGainY(uint8_t input);
			void setCameraControlIris(uint8_t input, int16_t iris);
			void setCameraControlFocus(uint8_t input, int16_t focus);
			void setCameraControlAutoFocus(uint8_t input, int16_t autoFocus);
			void setCameraControlAutoIris(uint8_t input, int16_t autoIris);
			void setCameraControlGain(uint8_t input, int16_t gain);
			void setCameraControlWhiteBalance(uint8_t input, int16_t whiteBalance);
			void setCameraControlSharpeningLevel(uint8_t input, int16_t sharpeningLevel);
			void setCameraControlZoomNormalized(uint8_t input, int16_t zoomNormalized);
			void setCameraControlZoomSpeed(uint8_t input, int16_t zoomSpeed);
			void setCameraControlColorbars(uint8_t input, int16_t colorbars);
			void setCameraControlLiftR(uint8_t input, int16_t liftR);
			void setCameraControlGammaR(uint8_t input, int16_t gammaR);
			void setCameraControlGainR(uint8_t input, int16_t gainR);
			void setCameraControlLumMix(uint8_t input, int16_t lumMix);
			void setCameraControlHue(uint8_t input, int16_t hue);
			void setCameraControlResetAll(uint8_t input, int16_t resetAll);
			void setCameraControlShutter(uint8_t input, int16_t shutter);
			void setCameraControlLiftG(uint8_t input, int16_t liftG);
			void setCameraControlGammaG(uint8_t input, int16_t gammaG);
			void setCameraControlGainG(uint8_t input, int16_t gainG);
			void setCameraControlContrast(uint8_t input, int16_t contrast);
			void setCameraControlSaturation(uint8_t input, int16_t saturation);
			void setCameraControlLiftB(uint8_t input, int16_t liftB);
			void setCameraControlGammaB(uint8_t input, int16_t gammaB);
			void setCameraControlGainB(uint8_t input, int16_t gainB);
			void setCameraControlLiftY(uint8_t input, int16_t liftY);
			void setCameraControlGammaY(uint8_t input, int16_t gammaY);
			void setCameraControlGainY(uint8_t input, int16_t gainY);
			uint8_t getMediaPlayerSourceType(uint8_t mediaPlayer);
			uint8_t getMediaPlayerSourceStillIndex(uint8_t mediaPlayer);
			uint8_t getMediaPlayerSourceClipIndex(uint8_t mediaPlayer);
			void setMediaPlayerSourceType(uint8_t mediaPlayer, uint8_t type);
			void setMediaPlayerSourceStillIndex(uint8_t mediaPlayer, uint8_t stillIndex);
			void setMediaPlayerSourceClipIndex(uint8_t mediaPlayer, uint8_t clipIndex);
			bool getMediaPlayerStillFilesIsUsed(uint8_t stillBank);
			char *  getMediaPlayerStillFilesFileName(uint8_t stillBank);
			uint8_t getMacroRunStatusState();
			bool getMacroRunStatusIsLooping();
			uint16_t getMacroRunStatusIndex();
			void setMacroAction(uint16_t index, uint8_t action);
			void setMacroRunChangePropertiesLooping(bool looping);
			bool getMacroPropertiesIsUsed(uint8_t macroIndex);
			char *  getMacroPropertiesName(uint8_t macroIndex);
			uint16_t getSuperSourceFillSource();
			uint16_t getSuperSourceKeySource();
			bool getSuperSourceForeground();
			bool getSuperSourcePreMultiplied();
			uint16_t getSuperSourceClip();
			uint16_t getSuperSourceGain();
			bool getSuperSourceInvertKey();
			bool getSuperSourceBorderEnabled();
			uint8_t getSuperSourceBorderBevel();
			uint16_t getSuperSourceBorderOuterWidth();
			uint16_t getSuperSourceBorderInnerWidth();
			uint8_t getSuperSourceBorderOuterSoftness();
			uint8_t getSuperSourceBorderInnerSoftness();
			uint8_t getSuperSourceBorderBevelSoftness();
			uint8_t getSuperSourceBorderBevelPosition();
			uint16_t getSuperSourceBorderHue();
			uint16_t getSuperSourceBorderSaturation();
			uint16_t getSuperSourceBorderLuma();
			uint16_t getSuperSourceLightSourceDirection();
			uint8_t getSuperSourceLightSourceAltitude();
			void setSuperSourceFillSource(uint16_t fillSource);
			void setSuperSourceKeySource(uint16_t keySource);
			void setSuperSourceForeground(bool foreground);
			void setSuperSourcePreMultiplied(bool preMultiplied);
			void setSuperSourceClip(uint16_t clip);
			void setSuperSourceGain(uint16_t gain);
			void setSuperSourceInvertKey(bool invertKey);
			void setSuperSourceBorderEnabled(bool borderEnabled);
			void setSuperSourceBorderBevel(uint8_t borderBevel);
			void setSuperSourceBorderOuterWidth(uint16_t borderOuterWidth);
			void setSuperSourceBorderInnerWidth(uint16_t borderInnerWidth);
			void setSuperSourceBorderOuterSoftness(uint8_t borderOuterSoftness);
			void setSuperSourceBorderInnerSoftness(uint8_t borderInnerSoftness);
			void setSuperSourceBorderBevelSoftness(uint8_t borderBevelSoftness);
			void setSuperSourceBorderBevelPosition(uint8_t borderBevelPosition);
			void setSuperSourceBorderHue(uint16_t borderHue);
			void setSuperSourceBorderSaturation(uint16_t borderSaturation);
			void setSuperSourceBorderLuma(uint16_t borderLuma);
			void setSuperSourceLightSourceDirection(uint16_t lightSourceDirection);
			void setSuperSourceLightSourceAltitude(uint8_t lightSourceAltitude);
			bool getSuperSourceBoxParametersEnabled(uint8_t box);
			uint16_t getSuperSourceBoxParametersInputSource(uint8_t box);
			int16_t getSuperSourceBoxParametersPositionX(uint8_t box);
			int16_t getSuperSourceBoxParametersPositionY(uint8_t box);
			uint16_t getSuperSourceBoxParametersSize(uint8_t box);
			bool getSuperSourceBoxParametersCropped(uint8_t box);
			uint16_t getSuperSourceBoxParametersCropTop(uint8_t box);
			uint16_t getSuperSourceBoxParametersCropBottom(uint8_t box);
			uint16_t getSuperSourceBoxParametersCropLeft(uint8_t box);
			uint16_t getSuperSourceBoxParametersCropRight(uint8_t box);
			void setSuperSourceBoxParametersEnabled(uint8_t box, bool enabled);
			void setSuperSourceBoxParametersInputSource(uint8_t box, uint16_t inputSource);
			void setSuperSourceBoxParametersPositionX(uint8_t box, int16_t positionX);
			void setSuperSourceBoxParametersPositionY(uint8_t box, int16_t positionY);
			void setSuperSourceBoxParametersSize(uint8_t box, uint16_t size);
			void setSuperSourceBoxParametersCropped(uint8_t box, bool cropped);
			void setSuperSourceBoxParametersCropTop(uint8_t box, uint16_t cropTop);
			void setSuperSourceBoxParametersCropBottom(uint8_t box, uint16_t cropBottom);
			void setSuperSourceBoxParametersCropLeft(uint8_t box, uint16_t cropLeft);
			void setSuperSourceBoxParametersCropRight(uint8_t box, uint16_t cropRight);
			uint8_t getAudioMixerInputMixOption(uint16_t audioSource);
			uint16_t getAudioMixerInputVolume(uint16_t audioSource);
			int16_t getAudioMixerInputBalance(uint16_t audioSource);
			void setAudioMixerInputMixOption(uint16_t audioSource, uint8_t mixOption);
			void setAudioMixerInputVolume(uint16_t audioSource, uint16_t volume);
			void setAudioMixerInputBalance(uint16_t audioSource, int16_t balance);
			uint16_t getAudioMixerMasterVolume();
			void setAudioMixerMasterVolume(uint16_t volume);
			bool getAudioMixerMonitorMonitorAudio();
			uint16_t getAudioMixerMonitorVolume();
			bool getAudioMixerMonitorMute();
			bool getAudioMixerMonitorSolo();
			uint16_t getAudioMixerMonitorSoloInput();
			bool getAudioMixerMonitorDim();
			void setAudioMixerMonitorMonitorAudio(bool monitorAudio);
			void setAudioMixerMonitorVolume(uint16_t volume);
			void setAudioMixerMonitorMute(bool mute);
			void setAudioMixerMonitorSolo(bool solo);
			void setAudioMixerMonitorSoloInput(uint16_t soloInput);
			void setAudioMixerMonitorDim(bool dim);
			void setAudioLevelsEnable(bool enable);
			uint16_t getAudioMixerLevelsSources();
			int32_t getAudioMixerLevelsMasterLeft();
			int32_t getAudioMixerLevelsMasterRight();
			int32_t getAudioMixerLevelsMasterPeakLeft();
			int32_t getAudioMixerLevelsMasterPeakRight();
			int32_t getAudioMixerLevelsMonitor();
			uint16_t getAudioMixerLevelsSourceOrder(uint16_t sources);
			int32_t getAudioMixerLevelsSourceLeft(uint16_t sources);
			int32_t getAudioMixerLevelsSourceRight(uint16_t sources);
			int32_t getAudioMixerLevelsSourcePeakLeft(uint16_t sources);
			int32_t getAudioMixerLevelsSourcePeakRight(uint16_t sources);
			uint16_t getAudioMixerTallySources();
			uint16_t getAudioMixerTallyAudioSource(uint16_t sources);
			bool getAudioMixerTallyIsMixedIn(uint16_t sources);
			uint16_t getTallyBySourceSources();
			uint16_t getTallyBySourceVideoSource(uint16_t sources);
			uint8_t getTallyBySourceTallyFlags(uint16_t sources);
};

#endif

