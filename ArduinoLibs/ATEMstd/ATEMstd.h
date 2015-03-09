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




#ifndef ATEMstd_h
#define ATEMstd_h

#include "ATEMbase.h"



class ATEMstd : public ATEMbase
{
private:
	
		// Special audio:
	uint16_t _ATEM_AMLv_channel;
	uint16_t atemAudioMixerLevelsMasterLeft;
	uint16_t atemAudioMixerLevelsMasterRight;
	uint16_t atemAudioMixerLevelsMonitor;
	uint16_t atemAudioMixerLevelsSourceLeft;
	uint16_t atemAudioMixerLevelsSourceRight;
	
	
	
	
	
	
  public:
	ATEMstd();  
	  

	void delay(const unsigned int delayTimeMillis);


	/********************************
	* ATEM Switcher state methods
	* Returns the most recent information we've 
	* got about the switchers state
	 ********************************/
		uint16_t getProgramInput();
		uint16_t getPreviewInput();
		boolean getProgramTally(uint8_t inputNumber);
		boolean getPreviewTally(uint8_t inputNumber);
		boolean getUpstreamKeyerStatus(uint8_t inputNumber);
		boolean getUpstreamKeyerOnNextTransitionStatus(uint8_t inputNumber);
		boolean getDownstreamKeyerStatus(uint8_t inputNumber);
		uint16_t getTransitionPosition();
		bool getTransitionPreview();
		uint8_t getTransitionType();
		uint8_t getTransitionMixTime();
	    boolean getFadeToBlackState();
		uint8_t getFadeToBlackFrameCount();
		uint8_t getFadeToBlackTime();
		bool getDownstreamKeyTie(uint8_t keyer);
		uint16_t getAuxState(uint8_t auxOutput);
		uint8_t getMediaPlayerType(uint8_t mediaPlayer);
		uint8_t getMediaPlayerStill(uint8_t mediaPlayer);
		uint8_t getMediaPlayerClip(uint8_t mediaPlayer);
		uint16_t getAudioLevels(uint8_t channel);
		uint8_t getAudioChannelMode(uint16_t channelNumber);

	/********************************
	 * ATEM Switcher Change methods
	 * Asks the switcher to changes something
	 ********************************/
		void changeProgramInput(uint16_t inputNumber);
		void changePreviewInput(uint16_t inputNumber);
		void doCut();
		void doAuto();
		void doAuto(uint8_t me);
		void fadeToBlackActivate();
		void changeTransitionPosition(word value);
		void changeTransitionPositionDone();
		void changeTransitionPreview(bool state);
		void changeTransitionType(uint8_t type);
		void changeTransitionMixTime(uint8_t frames);
		void changeFadeToBlackTime(uint8_t frames);
		void changeUpstreamKeyOn(uint8_t keyer, bool state);
		void changeUpstreamKeyNextTransition(uint8_t keyer, bool state);
		void changeDownstreamKeyOn(uint8_t keyer, bool state);
		void changeDownstreamKeyTie(uint8_t keyer, bool state);	
		void doAutoDownstreamKeyer(uint8_t keyer);
		void changeAuxState(uint8_t auxOutput, uint16_t inputNumber);
		void settingsMemorySave();
		void settingsMemoryClear();
		void changeColorValue(uint8_t colorGenerator, uint16_t hue, uint16_t saturation, uint16_t lightness);
		void mediaPlayerSelectSource(uint8_t mediaPlayer, boolean movieclip, uint8_t sourceIndex);
		void mediaPlayerClipStart(uint8_t mediaPlayer);

		void changeSwitcherVideoFormat(uint8_t format);
		
		void changeDVESettingsTemp(unsigned long Xpos,unsigned long Ypos,unsigned long Xsize,unsigned long Ysize);
		void changeDVEMaskTemp(unsigned long top,unsigned long bottom,unsigned long left,unsigned long right);
		void changeDVEBorder(bool enableBorder);
		void changeDVESettingsTemp_RunKeyFrame(uint8_t runType);
		void changeDVESettingsTemp_Rate(uint8_t rateFrames);
		void changeKeyerMask(uint16_t topMask, uint16_t bottomMask, uint16_t leftMask, uint16_t rightMask);
		void changeKeyerMask(uint8_t keyer, uint16_t topMask, uint16_t bottomMask, uint16_t leftMask, uint16_t rightMask);
		void changeDownstreamKeyMask(uint8_t keyer, uint16_t topMask, uint16_t bottomMask, uint16_t leftMask, uint16_t rightMask);
		void changeUpstreamKeyFillSource(uint8_t keyer, uint16_t inputNumber);
		void changeUpstreamKeyBlending(uint8_t keyer, bool preMultipliedAlpha, uint16_t clip, uint16_t gain, bool invKey);	
		void changeDownstreamKeyBlending(uint8_t keyer, bool preMultipliedAlpha, uint16_t clip, uint16_t gain, bool invKey);
		void changeDownstreamKeyFillSource(uint8_t keyer, uint16_t inputNumber);
		void changeDownstreamKeyKeySource(uint8_t keyer, uint16_t inputNumber);
		
	
		void changeAudioChannelMode(uint16_t channelNumber, uint8_t mode);
		void changeAudioChannelVolume(uint16_t channelNumber, uint16_t volume);
		void changeAudioMasterVolume(uint16_t volume);
		void sendAudioLevelNumbers(bool enable);
		void setAudioLevelReadoutChannel(uint16_t AMLv);

		void setWipeReverseDirection(bool reverse);


		// Special Audio:
		long getAudioMixerLevelsMasterLeft();
		long getAudioMixerLevelsMasterRight();
		long getAudioMixerLevelsMonitor();
		long getAudioMixerLevelsSourceLeft();
		long getAudioMixerLevelsSourceRight();














	// *********************************
	// **
	// ** Implementations in ATEMstd.h:
	// **
	// *********************************

	private:
		void _parseGetCommands(const char *cmdStr);
	
				// Private Variables in ATEM.h:
	
				uint16_t atemProtocolVersionMajor;
				uint16_t atemProtocolVersionMinor;
				uint8_t atemVideoModeFormat;
				uint16_t atemProgramInputVideoSource[2];
				uint16_t atemPreviewInputVideoSource[2];
				uint8_t atemTransitionStyle[2];
				uint8_t atemTransitionNextTransition[2];
				bool atemTransitionPreviewEnabled[2];
				bool atemTransitionInTransition[2];
				uint8_t atemTransitionFramesRemaining[2];
				uint16_t atemTransitionPosition[2];
				uint8_t atemTransitionMixRate[2];
				bool atemKeyerOnAirEnabled[2][4];
				bool atemDownstreamKeyerTie[2];
				uint8_t atemDownstreamKeyerRate[2];
				bool atemDownstreamKeyerPreMultiplied[2];
				uint16_t atemDownstreamKeyerClip[2];
				uint16_t atemDownstreamKeyerGain[2];
				bool atemDownstreamKeyerInvertKey[2];
				bool atemDownstreamKeyerMasked[2];
				int atemDownstreamKeyerTop[2];
				int atemDownstreamKeyerBottom[2];
				int atemDownstreamKeyerLeft[2];
				int atemDownstreamKeyerRight[2];
				bool atemDownstreamKeyerOnAir[2];
				bool atemDownstreamKeyerInTransition[2];
				bool atemDownstreamKeyerIsAutoTransitioning[2];
				uint8_t atemDownstreamKeyerFramesRemaining[2];
				uint8_t atemFadeToBlackRate[2];
				bool atemFadeToBlackStateFullyBlack[2];
				bool atemFadeToBlackStateInTransition[2];
				uint8_t atemFadeToBlackStateFramesRemaining[2];
				uint16_t atemAuxSourceInput[6];
				uint8_t atemMediaPlayerSourceType[2];
				uint8_t atemMediaPlayerSourceStillIndex[2];
				uint8_t atemMediaPlayerSourceClipIndex[2];
				uint8_t atemMacroRunStatusState;
				bool atemMacroRunStatusIsLooping;
				uint16_t atemMacroRunStatusIndex;
				bool atemMacroPropertiesIsUsed[10];
				bool atemMacroRecordingStatusIsRecording;
				uint16_t atemMacroRecordingStatusIndex;
				uint8_t atemAudioMixerInputMixOption[25];
				uint16_t atemAudioMixerInputVolume[25];
				int atemAudioMixerInputBalance[25];
				uint16_t atemTallyByIndexSources;
				uint8_t atemTallyByIndexTallyFlags[21];
	
	public:
				// Public Methods in ATEM.h:
	
				uint16_t getProtocolVersionMajor();
				uint16_t getProtocolVersionMinor();
				uint8_t getVideoModeFormat();
				void setVideoModeFormat(uint8_t format);
				uint16_t getProgramInputVideoSource(uint8_t mE);
				void setProgramInputVideoSource(uint8_t mE, uint16_t videoSource);
				uint16_t getPreviewInputVideoSource(uint8_t mE);
				void setPreviewInputVideoSource(uint8_t mE, uint16_t videoSource);
				void performCutME(uint8_t mE);
				void performAutoME(uint8_t mE);
				uint8_t getTransitionStyle(uint8_t mE);
				uint8_t getTransitionNextTransition(uint8_t mE);
				void setTransitionStyle(uint8_t mE, uint8_t style);
				void setTransitionNextTransition(uint8_t mE, uint8_t nextTransition);
				bool getTransitionPreviewEnabled(uint8_t mE);
				void setTransitionPreviewEnabled(uint8_t mE, bool enabled);
				bool getTransitionInTransition(uint8_t mE);
				uint8_t getTransitionFramesRemaining(uint8_t mE);
				uint16_t getTransitionPosition(uint8_t mE);
				void setTransitionPosition(uint8_t mE, uint16_t position);
				uint8_t getTransitionMixRate(uint8_t mE);
				void setTransitionMixRate(uint8_t mE, uint8_t rate);
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
				bool getKeyerOnAirEnabled(uint8_t mE, uint8_t keyer);
				void setKeyerOnAirEnabled(uint8_t mE, uint8_t keyer, bool enabled);
				void setKeyerMasked(uint8_t mE, uint8_t keyer, bool masked);
				void setKeyerTop(uint8_t mE, uint8_t keyer, int top);
				void setKeyerBottom(uint8_t mE, uint8_t keyer, int bottom);
				void setKeyerLeft(uint8_t mE, uint8_t keyer, int left);
				void setKeyerRight(uint8_t mE, uint8_t keyer, int right);
				void setKeyerFillSource(uint8_t mE, uint8_t keyer, uint16_t fillSource);
				void setKeyLumaPreMultiplied(uint8_t mE, uint8_t keyer, bool preMultiplied);
				void setKeyLumaClip(uint8_t mE, uint8_t keyer, uint16_t clip);
				void setKeyLumaGain(uint8_t mE, uint8_t keyer, uint16_t gain);
				void setKeyLumaInvertKey(uint8_t mE, uint8_t keyer, bool invertKey);
				void setKeyDVESizeX(uint8_t mE, uint8_t keyer, long sizeX);
				void setKeyDVESizeY(uint8_t mE, uint8_t keyer, long sizeY);
				void setKeyDVEPositionX(uint8_t mE, uint8_t keyer, long positionX);
				void setKeyDVEPositionY(uint8_t mE, uint8_t keyer, long positionY);
				void setKeyDVERotation(uint8_t mE, uint8_t keyer, long rotation);
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
				void setKeyDVETop(uint8_t mE, uint8_t keyer, int top);
				void setKeyDVEBottom(uint8_t mE, uint8_t keyer, int bottom);
				void setKeyDVELeft(uint8_t mE, uint8_t keyer, int left);
				void setKeyDVERight(uint8_t mE, uint8_t keyer, int right);
				void setKeyDVERate(uint8_t mE, uint8_t keyer, uint8_t rate);
				void setRunFlyingKeyKeyFrame(uint8_t mE, uint8_t keyer, uint8_t keyFrame);
				void setRunFlyingKeyRuntoInfiniteindex(uint8_t mE, uint8_t keyer, uint8_t runtoInfiniteindex);
				void setDownstreamKeyerFillSource(uint8_t keyer, uint16_t fillSource);
				void setDownstreamKeyerKeySource(uint8_t keyer, uint16_t keySource);
				bool getDownstreamKeyerTie(uint8_t keyer);
				uint8_t getDownstreamKeyerRate(uint8_t keyer);
				bool getDownstreamKeyerPreMultiplied(uint8_t keyer);
				uint16_t getDownstreamKeyerClip(uint8_t keyer);
				uint16_t getDownstreamKeyerGain(uint8_t keyer);
				bool getDownstreamKeyerInvertKey(uint8_t keyer);
				bool getDownstreamKeyerMasked(uint8_t keyer);
				int getDownstreamKeyerTop(uint8_t keyer);
				int getDownstreamKeyerBottom(uint8_t keyer);
				int getDownstreamKeyerLeft(uint8_t keyer);
				int getDownstreamKeyerRight(uint8_t keyer);
				void setDownstreamKeyerTie(uint8_t keyer, bool tie);
				void setDownstreamKeyerPreMultiplied(uint8_t keyer, bool preMultiplied);
				void setDownstreamKeyerClip(uint8_t keyer, uint16_t clip);
				void setDownstreamKeyerGain(uint8_t keyer, uint16_t gain);
				void setDownstreamKeyerInvertKey(uint8_t keyer, bool invertKey);
				void setDownstreamKeyerMasked(uint8_t keyer, bool masked);
				void setDownstreamKeyerTop(uint8_t keyer, int top);
				void setDownstreamKeyerBottom(uint8_t keyer, int bottom);
				void setDownstreamKeyerLeft(uint8_t keyer, int left);
				void setDownstreamKeyerRight(uint8_t keyer, int right);
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
				void setColorGeneratorHue(uint8_t colorGenerator, uint16_t hue);
				void setColorGeneratorSaturation(uint8_t colorGenerator, uint16_t saturation);
				void setColorGeneratorLuma(uint8_t colorGenerator, uint16_t luma);
				uint16_t getAuxSourceInput(uint8_t aUXChannel);
				void setAuxSourceInput(uint8_t aUXChannel, uint16_t input);
				void setClipPlayerPlaying(uint8_t mediaPlayer, bool playing);
				void setClipPlayerLoop(uint8_t mediaPlayer, bool loop);
				void setClipPlayerAtBeginning(uint8_t mediaPlayer, bool atBeginning);
				void setClipPlayerClipFrame(uint8_t mediaPlayer, uint16_t clipFrame);
				uint8_t getMediaPlayerSourceType(uint8_t mediaPlayer);
				uint8_t getMediaPlayerSourceStillIndex(uint8_t mediaPlayer);
				uint8_t getMediaPlayerSourceClipIndex(uint8_t mediaPlayer);
				void setMediaPlayerSourceType(uint8_t mediaPlayer, uint8_t type);
				void setMediaPlayerSourceStillIndex(uint8_t mediaPlayer, uint8_t stillIndex);
				void setMediaPlayerSourceClipIndex(uint8_t mediaPlayer, uint8_t clipIndex);
				uint8_t getMacroRunStatusState();
				bool getMacroRunStatusIsLooping();
				uint16_t getMacroRunStatusIndex();
				void setMacroAction(uint16_t index, uint8_t action);
				bool getMacroPropertiesIsUsed(uint8_t macroIndex);
				void setMacroAddPauseFrames(uint16_t frames);
				bool getMacroRecordingStatusIsRecording();
				uint16_t getMacroRecordingStatusIndex();
				uint8_t getAudioMixerInputMixOption(uint16_t audioSource);
				uint16_t getAudioMixerInputVolume(uint16_t audioSource);
				int getAudioMixerInputBalance(uint16_t audioSource);
				void setAudioMixerInputMixOption(uint16_t audioSource, uint8_t mixOption);
				void setAudioMixerInputVolume(uint16_t audioSource, uint16_t volume);
				void setAudioMixerInputBalance(uint16_t audioSource, int balance);
				void setAudioMixerMasterVolume(uint16_t volume);
				void setAudioLevelsEnable(bool enable);
				uint16_t getTallyByIndexSources();
				uint8_t getTallyByIndexTallyFlags(uint16_t sources);

	  
};

#endif

