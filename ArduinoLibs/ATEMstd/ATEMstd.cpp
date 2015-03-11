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



#include "ATEMstd.h"


/**
 * Constructor (using arguments is deprecated! Use begin() instead)
 */
ATEMstd::ATEMstd(){}



void ATEMstd::delay(const unsigned int delayTimeMillis)	{	// Responsible delay function which keeps the ATEM run loop up! DO NOT USE INSIDE THIS CLASS! Recursion could happen...
	runLoop(delayTimeMillis);
}




/********************************
 *
 * ATEM Switcher state methods
 * Returns the most recent information we've 
 * got about the switchers state
 *
 ********************************/

uint16_t ATEMstd::getProgramInput() {
	return getProgramInputVideoSource(0);
}
uint16_t ATEMstd::getPreviewInput() {
	return getPreviewInputVideoSource(0);
}
boolean ATEMstd::getProgramTally(uint8_t inputNumber) {
	return (getTallyByIndexTallyFlags(inputNumber-1) & 1) >0 ? true : false;
}
boolean ATEMstd::getPreviewTally(uint8_t inputNumber) {
	return (getTallyByIndexTallyFlags(inputNumber-1) & 2) >0 ? true : false;
}
boolean ATEMstd::getUpstreamKeyerStatus(uint8_t inputNumber) {
	return getKeyerOnAirEnabled(0,inputNumber-1);
}
boolean ATEMstd::getUpstreamKeyerOnNextTransitionStatus(uint8_t inputNumber) {	// input 0 = background
	return (getTransitionNextTransition(0) & (0x01 << inputNumber)) ? true : false;
}
boolean ATEMstd::getDownstreamKeyerStatus(uint8_t inputNumber) {
	return getDownstreamKeyerOnAir(inputNumber-1);
}
uint16_t ATEMstd::getTransitionPosition() {
	return getTransitionPosition(0);
}
bool ATEMstd::getTransitionPreview()	{
	return getTransitionPreviewEnabled(0);
}
uint8_t ATEMstd::getTransitionType()	{
	return getTransitionStyle(0);
}
uint8_t ATEMstd::getTransitionMixTime() {
	return getTransitionMixRate(0);		// Transition time for Mix Transitions
}
boolean ATEMstd::getFadeToBlackState() {
	return getFadeToBlackStateFullyBlack(0);    // Active state of Fade-to-black
}
uint8_t ATEMstd::getFadeToBlackFrameCount() {
	return getFadeToBlackStateFramesRemaining(0);    // Returns current frame in the FTB
}
uint8_t ATEMstd::getFadeToBlackTime() {
	return getFadeToBlackRate(0);		// Transition time for Fade-to-black
}
bool ATEMstd::getDownstreamKeyTie(uint8_t keyer)	{
	return getDownstreamKeyerTie(keyer-1);
}
uint16_t ATEMstd::getAuxState(uint8_t auxOutput)  {
	return getAuxSourceInput(auxOutput-1);
}	
uint8_t ATEMstd::getMediaPlayerType(uint8_t mediaPlayer)  {
	return getMediaPlayerSourceType(mediaPlayer-1);
}
uint8_t ATEMstd::getMediaPlayerStill(uint8_t mediaPlayer)  {
	return getMediaPlayerSourceStillIndex(mediaPlayer-1)+1;
}
uint8_t ATEMstd::getMediaPlayerClip(uint8_t mediaPlayer)  {
	return getMediaPlayerSourceClipIndex(mediaPlayer-1)+1;
}
uint16_t ATEMstd::getAudioLevels(uint8_t channel)	{
	if (channel>0)	{
		return atemAudioMixerLevelsSourceRight;
	} else {
		return atemAudioMixerLevelsSourceLeft;
	}
}
uint8_t ATEMstd::getAudioChannelMode(uint16_t channelNumber)	{
	return getAudioMixerInputMixOption(channelNumber);
}


/********************************
 *
 * ATEM Switcher Change methods
 * Asks the switcher to changes something
 *
 ********************************/



void ATEMstd::changeProgramInput(uint16_t inputNumber)  {
	setProgramInputVideoSource(0, inputNumber);
}
void ATEMstd::changePreviewInput(uint16_t inputNumber)  {
	setPreviewInputVideoSource(0, inputNumber);
}
void ATEMstd::doCut()	{
	performCutME(0);
}
void ATEMstd::doAuto()	{
	performAutoME(0);
}
void ATEMstd::doAuto(uint8_t me)	{
	performAutoME(me);
}
void ATEMstd::fadeToBlackActivate()	{
	performFadeToBlackME(0);
}
void ATEMstd::changeTransitionPosition(word value)	{
	setTransitionPosition(0,value);
}
void ATEMstd::changeTransitionPositionDone()	{	// When the last value of the transition is sent (1000), send this one too (we are done, change tally lights and preview bus!)
	setTransitionPosition(0,0);
}
void ATEMstd::changeTransitionPreview(bool state)	{
	setTransitionPreviewEnabled(0, state);
}
void ATEMstd::changeTransitionType(uint8_t type)	{
	setTransitionStyle(0, type);
}
void ATEMstd::changeTransitionMixTime(uint8_t frames)	{
	setTransitionMixRate(0, frames);
}
void ATEMstd::changeFadeToBlackTime(uint8_t frames)	{
	setFadeToBlackRate(0, frames);
}
void ATEMstd::changeUpstreamKeyOn(uint8_t keyer, bool state)	{
	setKeyerOnAirEnabled(0, keyer-1, state);
}
void ATEMstd::changeUpstreamKeyNextTransition(uint8_t keyer, bool state)	{	// Supporting "Background" by "0"
	if (keyer>=0 && keyer<=4)	{	// Todo: Should match available keyers depending on model?
		uint8_t stateValue = getTransitionNextTransition(0);
		if (state)	{
			stateValue = stateValue | (B1 << keyer);
		} else {
			stateValue = stateValue & (~(B1 << keyer));
		}
		setTransitionNextTransition(0, stateValue);
	}
}
void ATEMstd::changeDownstreamKeyOn(uint8_t keyer, bool state)	{
	setDownstreamKeyerOnAir(keyer-1, state);
}
void ATEMstd::changeDownstreamKeyTie(uint8_t keyer, bool state)	{
	setDownstreamKeyerTie(keyer-1, state);
}
void ATEMstd::doAutoDownstreamKeyer(uint8_t keyer)	{
	performDownstreamKeyerAutoKeyer(keyer-1);
}
void ATEMstd::changeAuxState(uint8_t auxOutput, uint16_t inputNumber)  {
	setAuxSourceInput(auxOutput-1, inputNumber);
}
void ATEMstd::settingsMemorySave()	{
	_prepareCommandPacket(PSTR("SRsv"),4);
	_finishCommandPacket();
}
void ATEMstd::settingsMemoryClear()	{
	_prepareCommandPacket(PSTR("SRcl"),4);
	_finishCommandPacket();
}
void ATEMstd::changeColorValue(uint8_t colorGenerator, uint16_t hue, uint16_t saturation, uint16_t lightness)  {
	commandBundleStart();
	setColorGeneratorHue(colorGenerator-1, hue);
	setColorGeneratorSaturation(colorGenerator-1, saturation);
	setColorGeneratorLuma(colorGenerator-1, lightness);
	commandBundleEnd();
}
void ATEMstd::mediaPlayerSelectSource(uint8_t mediaPlayer, boolean movieclip, uint8_t sourceIndex)  {
	if (movieclip)	{
		commandBundleStart();
		setMediaPlayerSourceType(mediaPlayer-1, 2);
		setMediaPlayerSourceClipIndex(mediaPlayer-1,sourceIndex-1);
		commandBundleEnd();
	} else {
		commandBundleStart();
		setMediaPlayerSourceType(mediaPlayer-1, 1);
		setMediaPlayerSourceStillIndex(mediaPlayer-1,sourceIndex-1);
		commandBundleEnd();
	}
}

void ATEMstd::mediaPlayerClipStart(uint8_t mediaPlayer)  {
	setClipPlayerPlaying(mediaPlayer-1,true);
}

void ATEMstd::changeSwitcherVideoFormat(uint8_t format)	{
	setVideoModeFormat(format);
}



void ATEMstd::changeDVESettingsTemp(unsigned long Xpos,unsigned long Ypos,unsigned long Xsize,unsigned long Ysize)	{	// TEMP
	commandBundleStart();
	setKeyDVEPositionX(0, 0, Xpos);
	setKeyDVEPositionY(0, 0, Ypos);
	setKeyDVESizeX(0, 0, Xsize);
	setKeyDVESizeY(0, 0, Ysize);
	commandBundleEnd();
}
void ATEMstd::changeDVEMaskTemp(unsigned long top,unsigned long bottom,unsigned long left,unsigned long right)	{	// TEMP
	// N/A
}
void ATEMstd::changeDVEBorder(bool enableBorder)	{	// TEMP
	setKeyDVEBorderEnabled(0, 0, enableBorder);
}

void ATEMstd::changeDVESettingsTemp_Rate(uint8_t rateFrames)	{	// TEMP
	setKeyDVERate(0,0,rateFrames);
}
void ATEMstd::changeDVESettingsTemp_RunKeyFrame(uint8_t runType)	{	// runType: 1=A, 2=B, 3=Full, 4=on of the others (with an extra paramter:)
	setRunFlyingKeyRuntoInfiniteindex(0,0, runType);	
}
void ATEMstd::changeKeyerMask(uint16_t topMask, uint16_t bottomMask, uint16_t leftMask, uint16_t rightMask) {
	changeKeyerMask(0, topMask, bottomMask, leftMask, rightMask);
}
void ATEMstd::changeKeyerMask(uint8_t keyer, uint16_t topMask, uint16_t bottomMask, uint16_t leftMask, uint16_t rightMask)	{
	commandBundleStart();
	setKeyerTop(0, keyer, topMask);
	setKeyerBottom(0, keyer, bottomMask);
	setKeyerLeft(0, keyer, leftMask);
	setKeyerRight(0, keyer, rightMask);
	commandBundleEnd();
}

void ATEMstd::changeDownstreamKeyMask(uint8_t keyer, uint16_t topMask, uint16_t bottomMask, uint16_t leftMask, uint16_t rightMask)	{
		if (keyer>=1 && keyer<=2)	{
			commandBundleStart();
			setDownstreamKeyerTop(keyer, topMask);
			setDownstreamKeyerBottom(keyer, bottomMask);
			setDownstreamKeyerLeft(keyer, leftMask);
			setDownstreamKeyerRight(keyer, rightMask);
			commandBundleEnd();
		}
}



void ATEMstd::changeUpstreamKeyFillSource(uint8_t keyer, uint16_t inputNumber)	{
	if (keyer>=1 && keyer<=4)	{	// Todo: Should match available keyers depending on model?
		setKeyerFillSource(0, keyer, inputNumber);
	}
}

// TODO: ONLY clip works right now! there is a bug...
void ATEMstd::changeUpstreamKeyBlending(uint8_t keyer, bool preMultipliedAlpha, uint16_t clip, uint16_t gain, bool invKey)	{
	if (keyer>=1 && keyer<=4)	{	// Todo: Should match available keyers depending on model?
		commandBundleStart();
		setKeyLumaPreMultiplied(0, keyer, preMultipliedAlpha);
		setKeyLumaClip(0, keyer, clip);
		setKeyLumaGain(0, keyer, gain);
		setKeyLumaInvertKey(0, keyer, invKey);
		commandBundleEnd();
	}
}

// TODO: ONLY clip works right now! there is a bug...
void ATEMstd::changeDownstreamKeyBlending(uint8_t keyer, bool preMultipliedAlpha, uint16_t clip, uint16_t gain, bool invKey)	{
	if (keyer>=1 && keyer<=2)	{	// Todo: Should match available keyers depending on model?
		commandBundleStart();
		setDownstreamKeyerPreMultiplied(keyer, preMultipliedAlpha);
		setDownstreamKeyerClip(keyer, clip);
		setDownstreamKeyerGain(keyer, gain);
		setDownstreamKeyerInvertKey(keyer, invKey);
		commandBundleEnd();
	}
}

// Statuskode retur: DskB, data byte 2 derefter er fill source, data byte 3 er key source, data byte 1 er keyer 1-2 (0-1)
// Key source command er : CDsC - og ellers ens med...
void ATEMstd::changeDownstreamKeyFillSource(uint8_t keyer, uint16_t inputNumber)	{
	if (keyer>=1 && keyer<=2)	{	// Todo: Should match available keyers depending on model?
	  	setDownstreamKeyerFillSource(keyer, inputNumber);	
	}
}

void ATEMstd::changeDownstreamKeyKeySource(uint8_t keyer, uint16_t inputNumber)	{
	if (keyer>=1 && keyer<=2)	{	// Todo: Should match available keyers depending on model?
		setDownstreamKeyerKeySource(keyer, inputNumber);
	}
}




void ATEMstd::changeAudioChannelMode(uint16_t channelNumber, uint8_t mode)	{	// Mode: 0=Off, 1=On, 2=AFV
	setAudioMixerInputMixOption(channelNumber, mode);
}
void ATEMstd::changeAudioChannelVolume(uint16_t channelNumber, uint16_t volume)	{
	setAudioMixerInputVolume(channelNumber, volume);
	/*
	Based on data from the ATEM switcher, this is an approximation to the integer value vs. the dB value:
	dB	+60 added	Number from protocol		Interpolated
	6	66	65381		65381
	3	63	46286		46301,04
	0	60	32768		32789,13
	-3	57	23198		23220,37
	-6	54	16423		16444,03
	-9	51	11627		11645,22
	-20	40	3377		3285,93
	-30	30	1036		1040,21
	-40	20	328		329,3
	-50	10	104		104,24
	-60	0	33		33

	for (int i=-60; i<=6; i=i+3)  {
	   Serial.print(i);
	   Serial.print(" dB = ");
	   Serial.print(33*pow(1.121898585, i+60));
	   Serial.println();
	}


	*/
}

void ATEMstd::changeAudioMasterVolume(uint16_t volume)	{
	setAudioMixerMasterVolume(volume);
}
void ATEMstd::sendAudioLevelNumbers(bool enable)	{
	setAudioLevelsEnable(enable);
}

// IMCOMPATIBLE with the similar function in old ATEM library  - here you enter the official number of the audio source instead!
void ATEMstd::setAudioLevelReadoutChannel(uint16_t AMLv)	{
	_ATEM_AMLv_channel = AMLv;	// Should check that it's in range 0-12
}



void ATEMstd::setWipeReverseDirection(bool reverse) {
	setTransitionWipeReverse(0,reverse);
}



// SPECIAL AUDIO:

/**
 * Get Audio Mixer Levels; Master Left
 */
long ATEMstd::getAudioMixerLevelsMasterLeft() {
	return atemAudioMixerLevelsMasterLeft;
}

/**
 * Get Audio Mixer Levels; Master Right
 */
long ATEMstd::getAudioMixerLevelsMasterRight() {
	return atemAudioMixerLevelsMasterRight;
}

/**
 * Get Audio Mixer Levels; Monitor
 */
long ATEMstd::getAudioMixerLevelsMonitor() {
	return atemAudioMixerLevelsMonitor;
}

/**
 * Get Audio Mixer Levels; Source Left
 */
long ATEMstd::getAudioMixerLevelsSourceLeft() {
	return atemAudioMixerLevelsSourceLeft;
}

/**
 * Get Audio Mixer Levels; Source Right
 */
long ATEMstd::getAudioMixerLevelsSourceRight() {
	return atemAudioMixerLevelsSourceRight;
}	
	



























// *********************************
// **
// ** Implementations in ATEMstd.c:
// **
// *********************************

void ATEMstd::_parseGetCommands(const char *cmdStr)	{
	uint8_t mE,keyer,colorGenerator,aUXChannel,mediaPlayer,macroIndex;
	uint16_t index,audioSource,sources;
	long temp;
	uint8_t readBytesForTlSr;
	
	if (!strcmp_P(cmdStr, PSTR("AMLv")))	{
		_readToPacketBuffer(36);
	} else if (!strcmp_P(cmdStr, PSTR("TlSr")))	{
		readBytesForTlSr = ((ATEM_packetBufferLength-2)/3)*3+2;
		_readToPacketBuffer(readBytesForTlSr);
	} else {
		_readToPacketBuffer();	// Default
	}
	
	
	if (!strcmp_P(cmdStr, PSTR("_pin")))	{
		if (_packetBuffer[5]=='T')	{
				_ATEMmodel = 0;
		} else
		if (_packetBuffer[5]=='1')	{
				_ATEMmodel = _packetBuffer[29]=='4' ? 4 : 1;
		} else
		if (_packetBuffer[5]=='2')	{
			_ATEMmodel = _packetBuffer[29]=='4' ? 5 : 2;
		} else
		if (_packetBuffer[5]=='P')	{
				_ATEMmodel = 3;
		} 
		
		#if ATEM_debug
		if (_serialOutput>0)	{
			Serial.print(F("Switcher type: "));
			Serial.print(_ATEMmodel);
			switch(_ATEMmodel)	{
				case 0:
					Serial.println(F(" - TeleVision Studio"));
				break;
				case 1:
					Serial.println(F(" - ATEM 1 M/E"));
				break;
				case 2:
					Serial.println(F(" - ATEM 2 M/E"));
				break;
				case 3:
					Serial.println(F(" - ATEM Production Studio 4K"));
				break;
				case 4:
					Serial.println(F(" - ATEM 1 M/E 4K"));
				break;
				case 5:
					Serial.println(F(" - ATEM 2 M/E 4K"));
				break;
			}
		}
		#endif
	}

	
	if(!strcmp_P(cmdStr, PSTR("AMLv"))) {
		sources = word(_packetBuffer[0],_packetBuffer[1]);
		if (sources<=24) {
				atemAudioMixerLevelsMasterLeft = (uint16_t)_packetBuffer[5]<<8 | _packetBuffer[6];
				atemAudioMixerLevelsMasterRight = (uint16_t)_packetBuffer[9]<<8 | _packetBuffer[10];
				atemAudioMixerLevelsMonitor = (uint16_t)_packetBuffer[21]<<8 | _packetBuffer[22];

				_readToPacketBuffer(sources*2);
				for(uint8_t a=0;a<sources;a++)	{
					if (_ATEM_AMLv_channel == word(_packetBuffer[a<<1], _packetBuffer[(a<<1)+1]))	{
				
						if (sources&B1)	{	// We must read 4-byte chunks, so compensate if sources was an odd number
							_readToPacketBuffer(2);
						}

						for(uint8_t b=0;b<sources;b++)	{
							_readToPacketBuffer(16);

							if(b==a)	{
								atemAudioMixerLevelsSourceLeft = (uint16_t)_packetBuffer[1]<<8 | _packetBuffer[2];
								atemAudioMixerLevelsSourceRight = (uint16_t)_packetBuffer[5]<<8 | _packetBuffer[6];
								break;
							}
						}
						break;
					}
				}
		}
	}			
	



	
	if(!strcmp_P(cmdStr, PSTR("_ver"))) {
		
			#if ATEM_debug
			temp = atemProtocolVersionMajor;
			#endif					
			atemProtocolVersionMajor = word(_packetBuffer[0], _packetBuffer[1]);
			#if ATEM_debug
			if ((_serialOutput==0x80 && atemProtocolVersionMajor!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
				Serial.print(F("atemProtocolVersionMajor = "));
				Serial.println(atemProtocolVersionMajor);
			}
			#endif
			
			#if ATEM_debug
			temp = atemProtocolVersionMinor;
			#endif					
			atemProtocolVersionMinor = word(_packetBuffer[2], _packetBuffer[3]);
			#if ATEM_debug
			if ((_serialOutput==0x80 && atemProtocolVersionMinor!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
				Serial.print(F("atemProtocolVersionMinor = "));
				Serial.println(atemProtocolVersionMinor);
			}
			#endif
			
	} else 
	if(!strcmp_P(cmdStr, PSTR("VidM"))) {
		
			#if ATEM_debug
			temp = atemVideoModeFormat;
			#endif					
			atemVideoModeFormat = _packetBuffer[0];
			#if ATEM_debug
			if ((_serialOutput==0x80 && atemVideoModeFormat!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
				Serial.print(F("atemVideoModeFormat = "));
				Serial.println(atemVideoModeFormat);
			}
			#endif
			
	} else 
	if(!strcmp_P(cmdStr, PSTR("PrgI"))) {
		
		mE = _packetBuffer[0];
		if (mE<=1) {
			#if ATEM_debug
			temp = atemProgramInputVideoSource[mE];
			#endif					
			atemProgramInputVideoSource[mE] = word(_packetBuffer[2], _packetBuffer[3]);
			#if ATEM_debug
			if ((_serialOutput==0x80 && atemProgramInputVideoSource[mE]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
				Serial.print(F("atemProgramInputVideoSource[mE=")); Serial.print(mE); Serial.print(F("] = "));
				Serial.println(atemProgramInputVideoSource[mE]);
			}
			#endif
			
		}
	} else 
	if(!strcmp_P(cmdStr, PSTR("PrvI"))) {
		
		mE = _packetBuffer[0];
		if (mE<=1) {
			#if ATEM_debug
			temp = atemPreviewInputVideoSource[mE];
			#endif					
			atemPreviewInputVideoSource[mE] = word(_packetBuffer[2], _packetBuffer[3]);
			#if ATEM_debug
			if ((_serialOutput==0x80 && atemPreviewInputVideoSource[mE]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
				Serial.print(F("atemPreviewInputVideoSource[mE=")); Serial.print(mE); Serial.print(F("] = "));
				Serial.println(atemPreviewInputVideoSource[mE]);
			}
			#endif
			
		}
	} else 
	if(!strcmp_P(cmdStr, PSTR("TrSS"))) {
		
		mE = _packetBuffer[0];
		if (mE<=1) {
			#if ATEM_debug
			temp = atemTransitionStyle[mE];
			#endif					
			atemTransitionStyle[mE] = _packetBuffer[1];
			#if ATEM_debug
			if ((_serialOutput==0x80 && atemTransitionStyle[mE]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
				Serial.print(F("atemTransitionStyle[mE=")); Serial.print(mE); Serial.print(F("] = "));
				Serial.println(atemTransitionStyle[mE]);
			}
			#endif
			
			#if ATEM_debug
			temp = atemTransitionNextTransition[mE];
			#endif					
			atemTransitionNextTransition[mE] = _packetBuffer[2];
			#if ATEM_debug
			if ((_serialOutput==0x80 && atemTransitionNextTransition[mE]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
				Serial.print(F("atemTransitionNextTransition[mE=")); Serial.print(mE); Serial.print(F("] = "));
				Serial.println(atemTransitionNextTransition[mE]);
			}
			#endif
			
		}
	} else 
	if(!strcmp_P(cmdStr, PSTR("TrPr"))) {
		
		mE = _packetBuffer[0];
		if (mE<=1) {
			#if ATEM_debug
			temp = atemTransitionPreviewEnabled[mE];
			#endif					
			atemTransitionPreviewEnabled[mE] = _packetBuffer[1];
			#if ATEM_debug
			if ((_serialOutput==0x80 && atemTransitionPreviewEnabled[mE]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
				Serial.print(F("atemTransitionPreviewEnabled[mE=")); Serial.print(mE); Serial.print(F("] = "));
				Serial.println(atemTransitionPreviewEnabled[mE]);
			}
			#endif
			
		}
	} else 
	if(!strcmp_P(cmdStr, PSTR("TrPs"))) {
		
		mE = _packetBuffer[0];
		if (mE<=1) {
			#if ATEM_debug
			temp = atemTransitionInTransition[mE];
			#endif					
			atemTransitionInTransition[mE] = _packetBuffer[1];
			#if ATEM_debug
			if ((_serialOutput==0x80 && atemTransitionInTransition[mE]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
				Serial.print(F("atemTransitionInTransition[mE=")); Serial.print(mE); Serial.print(F("] = "));
				Serial.println(atemTransitionInTransition[mE]);
			}
			#endif
			
			#if ATEM_debug
			temp = atemTransitionFramesRemaining[mE];
			#endif					
			atemTransitionFramesRemaining[mE] = _packetBuffer[2];
			#if ATEM_debug
			if ((_serialOutput==0x80 && atemTransitionFramesRemaining[mE]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
				Serial.print(F("atemTransitionFramesRemaining[mE=")); Serial.print(mE); Serial.print(F("] = "));
				Serial.println(atemTransitionFramesRemaining[mE]);
			}
			#endif
			
			#if ATEM_debug
			temp = atemTransitionPosition[mE];
			#endif					
			atemTransitionPosition[mE] = word(_packetBuffer[4], _packetBuffer[5]);
			#if ATEM_debug
			if ((_serialOutput==0x80 && atemTransitionPosition[mE]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
				Serial.print(F("atemTransitionPosition[mE=")); Serial.print(mE); Serial.print(F("] = "));
				Serial.println(atemTransitionPosition[mE]);
			}
			#endif
			
		}
	} else 
	if(!strcmp_P(cmdStr, PSTR("TMxP"))) {
		
		mE = _packetBuffer[0];
		if (mE<=1) {
			#if ATEM_debug
			temp = atemTransitionMixRate[mE];
			#endif					
			atemTransitionMixRate[mE] = _packetBuffer[1];
			#if ATEM_debug
			if ((_serialOutput==0x80 && atemTransitionMixRate[mE]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
				Serial.print(F("atemTransitionMixRate[mE=")); Serial.print(mE); Serial.print(F("] = "));
				Serial.println(atemTransitionMixRate[mE]);
			}
			#endif
			
		}
	} else 
	if(!strcmp_P(cmdStr, PSTR("KeOn"))) {
		
		mE = _packetBuffer[0];
		keyer = _packetBuffer[1];
		if (mE<=1 && keyer<=3) {
			#if ATEM_debug
			temp = atemKeyerOnAirEnabled[mE][keyer];
			#endif					
			atemKeyerOnAirEnabled[mE][keyer] = _packetBuffer[2];
			#if ATEM_debug
			if ((_serialOutput==0x80 && atemKeyerOnAirEnabled[mE][keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
				Serial.print(F("atemKeyerOnAirEnabled[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("] = "));
				Serial.println(atemKeyerOnAirEnabled[mE][keyer]);
			}
			#endif
			
		}
	} else 
	if(!strcmp_P(cmdStr, PSTR("DskP"))) {
		
		keyer = _packetBuffer[0];
		if (keyer<=1) {
			#if ATEM_debug
			temp = atemDownstreamKeyerTie[keyer];
			#endif					
			atemDownstreamKeyerTie[keyer] = _packetBuffer[1];
			#if ATEM_debug
			if ((_serialOutput==0x80 && atemDownstreamKeyerTie[keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
				Serial.print(F("atemDownstreamKeyerTie[keyer=")); Serial.print(keyer); Serial.print(F("] = "));
				Serial.println(atemDownstreamKeyerTie[keyer]);
			}
			#endif
			
			#if ATEM_debug
			temp = atemDownstreamKeyerRate[keyer];
			#endif					
			atemDownstreamKeyerRate[keyer] = _packetBuffer[2];
			#if ATEM_debug
			if ((_serialOutput==0x80 && atemDownstreamKeyerRate[keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
				Serial.print(F("atemDownstreamKeyerRate[keyer=")); Serial.print(keyer); Serial.print(F("] = "));
				Serial.println(atemDownstreamKeyerRate[keyer]);
			}
			#endif
			
			#if ATEM_debug
			temp = atemDownstreamKeyerPreMultiplied[keyer];
			#endif					
			atemDownstreamKeyerPreMultiplied[keyer] = _packetBuffer[3];
			#if ATEM_debug
			if ((_serialOutput==0x80 && atemDownstreamKeyerPreMultiplied[keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
				Serial.print(F("atemDownstreamKeyerPreMultiplied[keyer=")); Serial.print(keyer); Serial.print(F("] = "));
				Serial.println(atemDownstreamKeyerPreMultiplied[keyer]);
			}
			#endif
			
			#if ATEM_debug
			temp = atemDownstreamKeyerClip[keyer];
			#endif					
			atemDownstreamKeyerClip[keyer] = word(_packetBuffer[4], _packetBuffer[5]);
			#if ATEM_debug
			if ((_serialOutput==0x80 && atemDownstreamKeyerClip[keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
				Serial.print(F("atemDownstreamKeyerClip[keyer=")); Serial.print(keyer); Serial.print(F("] = "));
				Serial.println(atemDownstreamKeyerClip[keyer]);
			}
			#endif
			
			#if ATEM_debug
			temp = atemDownstreamKeyerGain[keyer];
			#endif					
			atemDownstreamKeyerGain[keyer] = word(_packetBuffer[6], _packetBuffer[7]);
			#if ATEM_debug
			if ((_serialOutput==0x80 && atemDownstreamKeyerGain[keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
				Serial.print(F("atemDownstreamKeyerGain[keyer=")); Serial.print(keyer); Serial.print(F("] = "));
				Serial.println(atemDownstreamKeyerGain[keyer]);
			}
			#endif
			
			#if ATEM_debug
			temp = atemDownstreamKeyerInvertKey[keyer];
			#endif					
			atemDownstreamKeyerInvertKey[keyer] = _packetBuffer[8];
			#if ATEM_debug
			if ((_serialOutput==0x80 && atemDownstreamKeyerInvertKey[keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
				Serial.print(F("atemDownstreamKeyerInvertKey[keyer=")); Serial.print(keyer); Serial.print(F("] = "));
				Serial.println(atemDownstreamKeyerInvertKey[keyer]);
			}
			#endif
			
			#if ATEM_debug
			temp = atemDownstreamKeyerMasked[keyer];
			#endif					
			atemDownstreamKeyerMasked[keyer] = _packetBuffer[9];
			#if ATEM_debug
			if ((_serialOutput==0x80 && atemDownstreamKeyerMasked[keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
				Serial.print(F("atemDownstreamKeyerMasked[keyer=")); Serial.print(keyer); Serial.print(F("] = "));
				Serial.println(atemDownstreamKeyerMasked[keyer]);
			}
			#endif
			
			#if ATEM_debug
			temp = atemDownstreamKeyerTop[keyer];
			#endif					
			atemDownstreamKeyerTop[keyer] = (int16_t) word(_packetBuffer[10], _packetBuffer[11]);
			#if ATEM_debug
			if ((_serialOutput==0x80 && atemDownstreamKeyerTop[keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
				Serial.print(F("atemDownstreamKeyerTop[keyer=")); Serial.print(keyer); Serial.print(F("] = "));
				Serial.println(atemDownstreamKeyerTop[keyer]);
			}
			#endif
			
			#if ATEM_debug
			temp = atemDownstreamKeyerBottom[keyer];
			#endif					
			atemDownstreamKeyerBottom[keyer] = (int16_t) word(_packetBuffer[12], _packetBuffer[13]);
			#if ATEM_debug
			if ((_serialOutput==0x80 && atemDownstreamKeyerBottom[keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
				Serial.print(F("atemDownstreamKeyerBottom[keyer=")); Serial.print(keyer); Serial.print(F("] = "));
				Serial.println(atemDownstreamKeyerBottom[keyer]);
			}
			#endif
			
			#if ATEM_debug
			temp = atemDownstreamKeyerLeft[keyer];
			#endif					
			atemDownstreamKeyerLeft[keyer] = (int16_t) word(_packetBuffer[14], _packetBuffer[15]);
			#if ATEM_debug
			if ((_serialOutput==0x80 && atemDownstreamKeyerLeft[keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
				Serial.print(F("atemDownstreamKeyerLeft[keyer=")); Serial.print(keyer); Serial.print(F("] = "));
				Serial.println(atemDownstreamKeyerLeft[keyer]);
			}
			#endif
			
			#if ATEM_debug
			temp = atemDownstreamKeyerRight[keyer];
			#endif					
			atemDownstreamKeyerRight[keyer] = (int16_t) word(_packetBuffer[16], _packetBuffer[17]);
			#if ATEM_debug
			if ((_serialOutput==0x80 && atemDownstreamKeyerRight[keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
				Serial.print(F("atemDownstreamKeyerRight[keyer=")); Serial.print(keyer); Serial.print(F("] = "));
				Serial.println(atemDownstreamKeyerRight[keyer]);
			}
			#endif
			
		}
	} else 
	if(!strcmp_P(cmdStr, PSTR("DskS"))) {
		
		keyer = _packetBuffer[0];
		if (keyer<=1) {
			#if ATEM_debug
			temp = atemDownstreamKeyerOnAir[keyer];
			#endif					
			atemDownstreamKeyerOnAir[keyer] = _packetBuffer[1];
			#if ATEM_debug
			if ((_serialOutput==0x80 && atemDownstreamKeyerOnAir[keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
				Serial.print(F("atemDownstreamKeyerOnAir[keyer=")); Serial.print(keyer); Serial.print(F("] = "));
				Serial.println(atemDownstreamKeyerOnAir[keyer]);
			}
			#endif
			
			#if ATEM_debug
			temp = atemDownstreamKeyerInTransition[keyer];
			#endif					
			atemDownstreamKeyerInTransition[keyer] = _packetBuffer[2];
			#if ATEM_debug
			if ((_serialOutput==0x80 && atemDownstreamKeyerInTransition[keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
				Serial.print(F("atemDownstreamKeyerInTransition[keyer=")); Serial.print(keyer); Serial.print(F("] = "));
				Serial.println(atemDownstreamKeyerInTransition[keyer]);
			}
			#endif
			
			#if ATEM_debug
			temp = atemDownstreamKeyerIsAutoTransitioning[keyer];
			#endif					
			atemDownstreamKeyerIsAutoTransitioning[keyer] = _packetBuffer[3];
			#if ATEM_debug
			if ((_serialOutput==0x80 && atemDownstreamKeyerIsAutoTransitioning[keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
				Serial.print(F("atemDownstreamKeyerIsAutoTransitioning[keyer=")); Serial.print(keyer); Serial.print(F("] = "));
				Serial.println(atemDownstreamKeyerIsAutoTransitioning[keyer]);
			}
			#endif
			
			#if ATEM_debug
			temp = atemDownstreamKeyerFramesRemaining[keyer];
			#endif					
			atemDownstreamKeyerFramesRemaining[keyer] = _packetBuffer[4];
			#if ATEM_debug
			if ((_serialOutput==0x80 && atemDownstreamKeyerFramesRemaining[keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
				Serial.print(F("atemDownstreamKeyerFramesRemaining[keyer=")); Serial.print(keyer); Serial.print(F("] = "));
				Serial.println(atemDownstreamKeyerFramesRemaining[keyer]);
			}
			#endif
			
		}
	} else 
	if(!strcmp_P(cmdStr, PSTR("FtbP"))) {
		
		mE = _packetBuffer[0];
		if (mE<=1) {
			#if ATEM_debug
			temp = atemFadeToBlackRate[mE];
			#endif					
			atemFadeToBlackRate[mE] = _packetBuffer[1];
			#if ATEM_debug
			if ((_serialOutput==0x80 && atemFadeToBlackRate[mE]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
				Serial.print(F("atemFadeToBlackRate[mE=")); Serial.print(mE); Serial.print(F("] = "));
				Serial.println(atemFadeToBlackRate[mE]);
			}
			#endif
			
		}
	} else 
	if(!strcmp_P(cmdStr, PSTR("FtbS"))) {
		
		mE = _packetBuffer[0];
		if (mE<=1) {
			#if ATEM_debug
			temp = atemFadeToBlackStateFullyBlack[mE];
			#endif					
			atemFadeToBlackStateFullyBlack[mE] = _packetBuffer[1];
			#if ATEM_debug
			if ((_serialOutput==0x80 && atemFadeToBlackStateFullyBlack[mE]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
				Serial.print(F("atemFadeToBlackStateFullyBlack[mE=")); Serial.print(mE); Serial.print(F("] = "));
				Serial.println(atemFadeToBlackStateFullyBlack[mE]);
			}
			#endif
			
			#if ATEM_debug
			temp = atemFadeToBlackStateInTransition[mE];
			#endif					
			atemFadeToBlackStateInTransition[mE] = _packetBuffer[2];
			#if ATEM_debug
			if ((_serialOutput==0x80 && atemFadeToBlackStateInTransition[mE]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
				Serial.print(F("atemFadeToBlackStateInTransition[mE=")); Serial.print(mE); Serial.print(F("] = "));
				Serial.println(atemFadeToBlackStateInTransition[mE]);
			}
			#endif
			
			#if ATEM_debug
			temp = atemFadeToBlackStateFramesRemaining[mE];
			#endif					
			atemFadeToBlackStateFramesRemaining[mE] = _packetBuffer[3];
			#if ATEM_debug
			if ((_serialOutput==0x80 && atemFadeToBlackStateFramesRemaining[mE]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
				Serial.print(F("atemFadeToBlackStateFramesRemaining[mE=")); Serial.print(mE); Serial.print(F("] = "));
				Serial.println(atemFadeToBlackStateFramesRemaining[mE]);
			}
			#endif
			
		}
	} else 
	if(!strcmp_P(cmdStr, PSTR("AuxS"))) {
		
		aUXChannel = _packetBuffer[0];
		if (aUXChannel<=5) {
			#if ATEM_debug
			temp = atemAuxSourceInput[aUXChannel];
			#endif					
			atemAuxSourceInput[aUXChannel] = word(_packetBuffer[2], _packetBuffer[3]);
			#if ATEM_debug
			if ((_serialOutput==0x80 && atemAuxSourceInput[aUXChannel]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
				Serial.print(F("atemAuxSourceInput[aUXChannel=")); Serial.print(aUXChannel); Serial.print(F("] = "));
				Serial.println(atemAuxSourceInput[aUXChannel]);
			}
			#endif
			
		}
	} else 
	if(!strcmp_P(cmdStr, PSTR("MPCE"))) {
		
		mediaPlayer = _packetBuffer[0];
		if (mediaPlayer<=1) {
			#if ATEM_debug
			temp = atemMediaPlayerSourceType[mediaPlayer];
			#endif					
			atemMediaPlayerSourceType[mediaPlayer] = _packetBuffer[1];
			#if ATEM_debug
			if ((_serialOutput==0x80 && atemMediaPlayerSourceType[mediaPlayer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
				Serial.print(F("atemMediaPlayerSourceType[mediaPlayer=")); Serial.print(mediaPlayer); Serial.print(F("] = "));
				Serial.println(atemMediaPlayerSourceType[mediaPlayer]);
			}
			#endif
			
			#if ATEM_debug
			temp = atemMediaPlayerSourceStillIndex[mediaPlayer];
			#endif					
			atemMediaPlayerSourceStillIndex[mediaPlayer] = _packetBuffer[2];
			#if ATEM_debug
			if ((_serialOutput==0x80 && atemMediaPlayerSourceStillIndex[mediaPlayer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
				Serial.print(F("atemMediaPlayerSourceStillIndex[mediaPlayer=")); Serial.print(mediaPlayer); Serial.print(F("] = "));
				Serial.println(atemMediaPlayerSourceStillIndex[mediaPlayer]);
			}
			#endif
			
			#if ATEM_debug
			temp = atemMediaPlayerSourceClipIndex[mediaPlayer];
			#endif					
			atemMediaPlayerSourceClipIndex[mediaPlayer] = _packetBuffer[3];
			#if ATEM_debug
			if ((_serialOutput==0x80 && atemMediaPlayerSourceClipIndex[mediaPlayer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
				Serial.print(F("atemMediaPlayerSourceClipIndex[mediaPlayer=")); Serial.print(mediaPlayer); Serial.print(F("] = "));
				Serial.println(atemMediaPlayerSourceClipIndex[mediaPlayer]);
			}
			#endif
			
		}
	} else 
	if(!strcmp_P(cmdStr, PSTR("MRPr"))) {
		
			#if ATEM_debug
			temp = atemMacroRunStatusState;
			#endif					
			atemMacroRunStatusState = _packetBuffer[0];
			#if ATEM_debug
			if ((_serialOutput==0x80 && atemMacroRunStatusState!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
				Serial.print(F("atemMacroRunStatusState = "));
				Serial.println(atemMacroRunStatusState);
			}
			#endif
			
			#if ATEM_debug
			temp = atemMacroRunStatusIsLooping;
			#endif					
			atemMacroRunStatusIsLooping = _packetBuffer[1];
			#if ATEM_debug
			if ((_serialOutput==0x80 && atemMacroRunStatusIsLooping!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
				Serial.print(F("atemMacroRunStatusIsLooping = "));
				Serial.println(atemMacroRunStatusIsLooping);
			}
			#endif
			
			#if ATEM_debug
			temp = atemMacroRunStatusIndex;
			#endif					
			atemMacroRunStatusIndex = word(_packetBuffer[2], _packetBuffer[3]);
			#if ATEM_debug
			if ((_serialOutput==0x80 && atemMacroRunStatusIndex!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
				Serial.print(F("atemMacroRunStatusIndex = "));
				Serial.println(atemMacroRunStatusIndex);
			}
			#endif
			
	} else 
	if(!strcmp_P(cmdStr, PSTR("MPrp"))) {
		
		macroIndex = _packetBuffer[1];
		if (macroIndex<=9) {
			#if ATEM_debug
			temp = atemMacroPropertiesIsUsed[macroIndex];
			#endif					
			atemMacroPropertiesIsUsed[macroIndex] = _packetBuffer[2];
			#if ATEM_debug
			if ((_serialOutput==0x80 && atemMacroPropertiesIsUsed[macroIndex]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
				Serial.print(F("atemMacroPropertiesIsUsed[macroIndex=")); Serial.print(macroIndex); Serial.print(F("] = "));
				Serial.println(atemMacroPropertiesIsUsed[macroIndex]);
			}
			#endif
			
		}
	} else 
	if(!strcmp_P(cmdStr, PSTR("MRcS"))) {
		
			#if ATEM_debug
			temp = atemMacroRecordingStatusIsRecording;
			#endif					
			atemMacroRecordingStatusIsRecording = _packetBuffer[0];
			#if ATEM_debug
			if ((_serialOutput==0x80 && atemMacroRecordingStatusIsRecording!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
				Serial.print(F("atemMacroRecordingStatusIsRecording = "));
				Serial.println(atemMacroRecordingStatusIsRecording);
			}
			#endif
			
			#if ATEM_debug
			temp = atemMacroRecordingStatusIndex;
			#endif					
			atemMacroRecordingStatusIndex = word(_packetBuffer[2], _packetBuffer[3]);
			#if ATEM_debug
			if ((_serialOutput==0x80 && atemMacroRecordingStatusIndex!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
				Serial.print(F("atemMacroRecordingStatusIndex = "));
				Serial.println(atemMacroRecordingStatusIndex);
			}
			#endif
			
	} else 
	if(!strcmp_P(cmdStr, PSTR("AMIP"))) {
		
		audioSource = word(_packetBuffer[0],_packetBuffer[1]);
		if (getAudioSrcIndex(audioSource)<=24) {
			#if ATEM_debug
			temp = atemAudioMixerInputMixOption[getAudioSrcIndex(audioSource)];
			#endif					
			atemAudioMixerInputMixOption[getAudioSrcIndex(audioSource)] = _packetBuffer[8];
			#if ATEM_debug
			if ((_serialOutput==0x80 && atemAudioMixerInputMixOption[getAudioSrcIndex(audioSource)]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
				Serial.print(F("atemAudioMixerInputMixOption[getAudioSrcIndex(audioSource)=")); Serial.print(getAudioSrcIndex(audioSource)); Serial.print(F("] = "));
				Serial.println(atemAudioMixerInputMixOption[getAudioSrcIndex(audioSource)]);
			}
			#endif
			
			#if ATEM_debug
			temp = atemAudioMixerInputVolume[getAudioSrcIndex(audioSource)];
			#endif					
			atemAudioMixerInputVolume[getAudioSrcIndex(audioSource)] = word(_packetBuffer[10], _packetBuffer[11]);
			#if ATEM_debug
			if ((_serialOutput==0x80 && atemAudioMixerInputVolume[getAudioSrcIndex(audioSource)]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
				Serial.print(F("atemAudioMixerInputVolume[getAudioSrcIndex(audioSource)=")); Serial.print(getAudioSrcIndex(audioSource)); Serial.print(F("] = "));
				Serial.println(atemAudioMixerInputVolume[getAudioSrcIndex(audioSource)]);
			}
			#endif
			
			#if ATEM_debug
			temp = atemAudioMixerInputBalance[getAudioSrcIndex(audioSource)];
			#endif					
			atemAudioMixerInputBalance[getAudioSrcIndex(audioSource)] = (int16_t) word(_packetBuffer[12], _packetBuffer[13]);
			#if ATEM_debug
			if ((_serialOutput==0x80 && atemAudioMixerInputBalance[getAudioSrcIndex(audioSource)]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
				Serial.print(F("atemAudioMixerInputBalance[getAudioSrcIndex(audioSource)=")); Serial.print(getAudioSrcIndex(audioSource)); Serial.print(F("] = "));
				Serial.println(atemAudioMixerInputBalance[getAudioSrcIndex(audioSource)]);
			}
			#endif
			
		}
	} else 
	if(!strcmp_P(cmdStr, PSTR("TlIn"))) {
		
		sources = word(_packetBuffer[0],_packetBuffer[1]);
		if (sources<=20) {
			#if ATEM_debug
			temp = atemTallyByIndexSources;
			#endif					
			atemTallyByIndexSources = word(_packetBuffer[0], _packetBuffer[1]);
			#if ATEM_debug
			if ((_serialOutput==0x80 && atemTallyByIndexSources!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
				Serial.print(F("atemTallyByIndexSources = "));
				Serial.println(atemTallyByIndexSources);
			}
			#endif

			for(uint8_t a=0;a<sources;a++)	{
				#if ATEM_debug
				temp = atemTallyByIndexTallyFlags[a];
				#endif					
				atemTallyByIndexTallyFlags[a] = _packetBuffer[2+a];
				#if ATEM_debug
				if ((_serialOutput==0x80 && atemTallyByIndexTallyFlags[a]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
					Serial.print(F("atemTallyByIndexTallyFlags[a=")); Serial.print(a); Serial.print(F("] = "));
					Serial.println(atemTallyByIndexTallyFlags[a]);
				}
				#endif
			}			

		}
	} else 
	{}
}





	/**
	 * Get Protocol Version; Major
	 */
	uint16_t ATEMstd::getProtocolVersionMajor() {
		return atemProtocolVersionMajor;
	}
	
	/**
	 * Get Protocol Version; Minor
	 */
	uint16_t ATEMstd::getProtocolVersionMinor() {
		return atemProtocolVersionMinor;
	}
	
	/**
	 * Get Video Mode; Format
	 */
	uint8_t ATEMstd::getVideoModeFormat() {
		return atemVideoModeFormat;
	}
	
	/**
	 * Set Video Mode; Format
	 * format 	0: 525i59.94 NTSC, 1: 625i 50 PAL, 2: 525i59.94 NTSC 16:9, 3: 625i 50 PAL 16:9, 4: 720p50, 5: 720p59.94, 6: 1080i50, 7: 1080i59.94, 8: 1080p23.98, 9: 1080p24, 10: 1080p25, 11: 1080p29.97, 12: 1080p50, 13: 1080p59.94, 14: 2160p23.98, 15: 2160p24, 16: 2160p25, 17: 2160p29.97
	 */
	void ATEMstd::setVideoModeFormat(uint8_t format) {
	
  		_prepareCommandPacket(PSTR("CVdM"),4);

		_packetBuffer[12+_cBBO+4+4+0] = format;
		
   		_finishCommandPacket();

	}
	
	/**
	 * Get Program Input; Video Source
	 * mE 	0: ME1, 1: ME2
	 */
	uint16_t ATEMstd::getProgramInputVideoSource(uint8_t mE) {
		return atemProgramInputVideoSource[mE];
	}
	
	/**
	 * Set Program Input; Video Source
	 * mE 	0: ME1, 1: ME2
	 * videoSource 	(See video source list)
	 */
	void ATEMstd::setProgramInputVideoSource(uint8_t mE, uint16_t videoSource) {
	
  		_prepareCommandPacket(PSTR("CPgI"),4,(_packetBuffer[12+_cBBO+4+4+0]==mE));

		_packetBuffer[12+_cBBO+4+4+0] = mE;
		
		_packetBuffer[12+_cBBO+4+4+2] = highByte(videoSource);
		_packetBuffer[12+_cBBO+4+4+3] = lowByte(videoSource);
		
   		_finishCommandPacket();

	}
	
	/**
	 * Get Preview Input; Video Source
	 * mE 	0: ME1, 1: ME2
	 */
	uint16_t ATEMstd::getPreviewInputVideoSource(uint8_t mE) {
		return atemPreviewInputVideoSource[mE];
	}
	
	/**
	 * Set Preview Input; Video Source
	 * mE 	0: ME1, 1: ME2
	 * videoSource 	(See video source list)
	 */
	void ATEMstd::setPreviewInputVideoSource(uint8_t mE, uint16_t videoSource) {
	
  		_prepareCommandPacket(PSTR("CPvI"),4,(_packetBuffer[12+_cBBO+4+4+0]==mE));

		_packetBuffer[12+_cBBO+4+4+0] = mE;
		
		_packetBuffer[12+_cBBO+4+4+2] = highByte(videoSource);
		_packetBuffer[12+_cBBO+4+4+3] = lowByte(videoSource);
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Cut; M/E
	 * mE 	0: ME1, 1: ME2
	 */
	void ATEMstd::performCutME(uint8_t mE) {
	
  		_prepareCommandPacket(PSTR("DCut"),4);

		_packetBuffer[12+_cBBO+4+4+0] = mE;
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Auto; M/E
	 * mE 	0: ME1, 1: ME2
	 */
	void ATEMstd::performAutoME(uint8_t mE) {
	
  		_prepareCommandPacket(PSTR("DAut"),4);

		_packetBuffer[12+_cBBO+4+4+0] = mE;
		
   		_finishCommandPacket();

	}
	
	/**
	 * Get Transition; Style
	 * mE 	0: ME1, 1: ME2
	 */
	uint8_t ATEMstd::getTransitionStyle(uint8_t mE) {
		return atemTransitionStyle[mE];
	}
	
	/**
	 * Get Transition; Next Transition
	 * mE 	0: ME1, 1: ME2
	 */
	uint8_t ATEMstd::getTransitionNextTransition(uint8_t mE) {
		return atemTransitionNextTransition[mE];
	}
	
	/**
	 * Set Transition; Style
	 * mE 	0: ME1, 1: ME2
	 * style 	0: Mix, 1: Dip, 2: Wipe, 3: DVE, 4: Sting
	 */
	void ATEMstd::setTransitionStyle(uint8_t mE, uint8_t style) {
	
  		_prepareCommandPacket(PSTR("CTTp"),4,(_packetBuffer[12+_cBBO+4+4+1]==mE));

			// Set Mask: 1
		_packetBuffer[12+_cBBO+4+4+0] |= 1;
				
		_packetBuffer[12+_cBBO+4+4+1] = mE;
		
		_packetBuffer[12+_cBBO+4+4+2] = style;
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Transition; Next Transition
	 * mE 	0: ME1, 1: ME2
	 * nextTransition 	Bit 0: Background=On/Off, Bit 1: Key 1=On/Off, Bit 2: Key 2=On/Off, Bit 3: Key 3=On/Off, Bit 4: Key 4=On/Off
	 */
	void ATEMstd::setTransitionNextTransition(uint8_t mE, uint8_t nextTransition) {
	
  		_prepareCommandPacket(PSTR("CTTp"),4,(_packetBuffer[12+_cBBO+4+4+1]==mE));

			// Set Mask: 2
		_packetBuffer[12+_cBBO+4+4+0] |= 2;
				
		_packetBuffer[12+_cBBO+4+4+1] = mE;
		
		_packetBuffer[12+_cBBO+4+4+3] = nextTransition;
		
   		_finishCommandPacket();

	}
	
	/**
	 * Get Transition Preview; Enabled
	 * mE 	0: ME1, 1: ME2
	 */
	bool ATEMstd::getTransitionPreviewEnabled(uint8_t mE) {
		return atemTransitionPreviewEnabled[mE];
	}
	
	/**
	 * Set Transition Preview; Enabled
	 * mE 	0: ME1, 1: ME2
	 * enabled 	Bit 0: On/Off
	 */
	void ATEMstd::setTransitionPreviewEnabled(uint8_t mE, bool enabled) {
	
  		_prepareCommandPacket(PSTR("CTPr"),4,(_packetBuffer[12+_cBBO+4+4+0]==mE));

		_packetBuffer[12+_cBBO+4+4+0] = mE;
		
		_packetBuffer[12+_cBBO+4+4+1] = enabled;
		
   		_finishCommandPacket();

	}
	
	/**
	 * Get Transition Position; In Transition
	 * mE 	0: ME1, 1: ME2
	 */
	bool ATEMstd::getTransitionInTransition(uint8_t mE) {
		return atemTransitionInTransition[mE];
	}
	
	/**
	 * Get Transition Position; Frames Remaining
	 * mE 	0: ME1, 1: ME2
	 */
	uint8_t ATEMstd::getTransitionFramesRemaining(uint8_t mE) {
		return atemTransitionFramesRemaining[mE];
	}
	
	/**
	 * Get Transition Position; Position
	 * mE 	0: ME1, 1: ME2
	 */
	uint16_t ATEMstd::getTransitionPosition(uint8_t mE) {
		return atemTransitionPosition[mE];
	}
	
	/**
	 * Set Transition Position; Position
	 * mE 	0: ME1, 1: ME2
	 * position 	0-9999
	 */
	void ATEMstd::setTransitionPosition(uint8_t mE, uint16_t position) {
	
  		_prepareCommandPacket(PSTR("CTPs"),4,(_packetBuffer[12+_cBBO+4+4+0]==mE));

		_packetBuffer[12+_cBBO+4+4+0] = mE;
		
		_packetBuffer[12+_cBBO+4+4+2] = highByte(position);
		_packetBuffer[12+_cBBO+4+4+3] = lowByte(position);
		
   		_finishCommandPacket();

	}
	
	/**
	 * Get Transition Mix; Rate
	 * mE 	0: ME1, 1: ME2
	 */
	uint8_t ATEMstd::getTransitionMixRate(uint8_t mE) {
		return atemTransitionMixRate[mE];
	}
	
	/**
	 * Set Transition Mix; Rate
	 * mE 	0: ME1, 1: ME2
	 * rate 	1-250: Frames
	 */
	void ATEMstd::setTransitionMixRate(uint8_t mE, uint8_t rate) {
	
  		_prepareCommandPacket(PSTR("CTMx"),4,(_packetBuffer[12+_cBBO+4+4+0]==mE));

		_packetBuffer[12+_cBBO+4+4+0] = mE;
		
		_packetBuffer[12+_cBBO+4+4+1] = rate;
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Transition Wipe; Rate
	 * mE 	0: ME1, 1: ME2
	 * rate 	1-250: Frames
	 */
	void ATEMstd::setTransitionWipeRate(uint8_t mE, uint8_t rate) {
	
  		_prepareCommandPacket(PSTR("CTWp"),20,(_packetBuffer[12+_cBBO+4+4+2]==mE));

			// Set Mask: 1
		_packetBuffer[12+_cBBO+4+4+1] |= 1;
				
		_packetBuffer[12+_cBBO+4+4+2] = mE;
		
		_packetBuffer[12+_cBBO+4+4+3] = rate;
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Transition Wipe; Pattern
	 * mE 	0: ME1, 1: ME2
	 * pattern 	0-17: Pattern Styles
	 */
	void ATEMstd::setTransitionWipePattern(uint8_t mE, uint8_t pattern) {
	
  		_prepareCommandPacket(PSTR("CTWp"),20,(_packetBuffer[12+_cBBO+4+4+2]==mE));

			// Set Mask: 2
		_packetBuffer[12+_cBBO+4+4+1] |= 2;
				
		_packetBuffer[12+_cBBO+4+4+2] = mE;
		
		_packetBuffer[12+_cBBO+4+4+4] = pattern;
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Transition Wipe; Width
	 * mE 	0: ME1, 1: ME2
	 * width 	0-10000: 0-100%
	 */
	void ATEMstd::setTransitionWipeWidth(uint8_t mE, uint16_t width) {
	
  		_prepareCommandPacket(PSTR("CTWp"),20,(_packetBuffer[12+_cBBO+4+4+2]==mE));

			// Set Mask: 4
		_packetBuffer[12+_cBBO+4+4+1] |= 4;
				
		_packetBuffer[12+_cBBO+4+4+2] = mE;
		
		_packetBuffer[12+_cBBO+4+4+6] = highByte(width);
		_packetBuffer[12+_cBBO+4+4+7] = lowByte(width);
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Transition Wipe; Fill Source
	 * mE 	0: ME1, 1: ME2
	 * fillSource 	(See video source list)
	 */
	void ATEMstd::setTransitionWipeFillSource(uint8_t mE, uint16_t fillSource) {
	
  		_prepareCommandPacket(PSTR("CTWp"),20,(_packetBuffer[12+_cBBO+4+4+2]==mE));

			// Set Mask: 8
		_packetBuffer[12+_cBBO+4+4+1] |= 8;
				
		_packetBuffer[12+_cBBO+4+4+2] = mE;
		
		_packetBuffer[12+_cBBO+4+4+8] = highByte(fillSource);
		_packetBuffer[12+_cBBO+4+4+9] = lowByte(fillSource);
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Transition Wipe; Symmetry
	 * mE 	0: ME1, 1: ME2
	 * symmetry 	0-10000: 0-100%
	 */
	void ATEMstd::setTransitionWipeSymmetry(uint8_t mE, uint16_t symmetry) {
	
  		_prepareCommandPacket(PSTR("CTWp"),20,(_packetBuffer[12+_cBBO+4+4+2]==mE));

			// Set Mask: 16
		_packetBuffer[12+_cBBO+4+4+1] |= 16;
				
		_packetBuffer[12+_cBBO+4+4+2] = mE;
		
		_packetBuffer[12+_cBBO+4+4+10] = highByte(symmetry);
		_packetBuffer[12+_cBBO+4+4+11] = lowByte(symmetry);
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Transition Wipe; Softness
	 * mE 	0: ME1, 1: ME2
	 * softness 	0-10000: 0-100%
	 */
	void ATEMstd::setTransitionWipeSoftness(uint8_t mE, uint16_t softness) {
	
  		_prepareCommandPacket(PSTR("CTWp"),20,(_packetBuffer[12+_cBBO+4+4+2]==mE));

			// Set Mask: 32
		_packetBuffer[12+_cBBO+4+4+1] |= 32;
				
		_packetBuffer[12+_cBBO+4+4+2] = mE;
		
		_packetBuffer[12+_cBBO+4+4+12] = highByte(softness);
		_packetBuffer[12+_cBBO+4+4+13] = lowByte(softness);
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Transition Wipe; Position X
	 * mE 	0: ME1, 1: ME2
	 * positionX 	0-10000: 0.0-1.0
	 */
	void ATEMstd::setTransitionWipePositionX(uint8_t mE, uint16_t positionX) {
	
  		_prepareCommandPacket(PSTR("CTWp"),20,(_packetBuffer[12+_cBBO+4+4+2]==mE));

			// Set Mask: 64
		_packetBuffer[12+_cBBO+4+4+1] |= 64;
				
		_packetBuffer[12+_cBBO+4+4+2] = mE;
		
		_packetBuffer[12+_cBBO+4+4+14] = highByte(positionX);
		_packetBuffer[12+_cBBO+4+4+15] = lowByte(positionX);
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Transition Wipe; Position Y
	 * mE 	0: ME1, 1: ME2
	 * positionY 	0-10000: 0.0-1.0
	 */
	void ATEMstd::setTransitionWipePositionY(uint8_t mE, uint16_t positionY) {
	
  		_prepareCommandPacket(PSTR("CTWp"),20,(_packetBuffer[12+_cBBO+4+4+2]==mE));

			// Set Mask: 128
		_packetBuffer[12+_cBBO+4+4+1] |= 128;
				
		_packetBuffer[12+_cBBO+4+4+2] = mE;
		
		_packetBuffer[12+_cBBO+4+4+16] = highByte(positionY);
		_packetBuffer[12+_cBBO+4+4+17] = lowByte(positionY);
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Transition Wipe; Reverse
	 * mE 	0: ME1, 1: ME2
	 * reverse 	Bit 0: On/Off
	 */
	void ATEMstd::setTransitionWipeReverse(uint8_t mE, bool reverse) {
	
  		_prepareCommandPacket(PSTR("CTWp"),20,(_packetBuffer[12+_cBBO+4+4+2]==mE));

			// Set Mask: 256
		_packetBuffer[12+_cBBO+4+4+0] |= 1;
				
		_packetBuffer[12+_cBBO+4+4+2] = mE;
		
		_packetBuffer[12+_cBBO+4+4+18] = reverse;
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Transition Wipe; FlipFlop
	 * mE 	0: ME1, 1: ME2
	 * flipFlop 	Bit 0: On/Off
	 */
	void ATEMstd::setTransitionWipeFlipFlop(uint8_t mE, bool flipFlop) {
	
  		_prepareCommandPacket(PSTR("CTWp"),20,(_packetBuffer[12+_cBBO+4+4+2]==mE));

			// Set Mask: 512
		_packetBuffer[12+_cBBO+4+4+0] |= 2;
				
		_packetBuffer[12+_cBBO+4+4+2] = mE;
		
		_packetBuffer[12+_cBBO+4+4+19] = flipFlop;
		
   		_finishCommandPacket();

	}
	
	/**
	 * Get Keyer On Air; Enabled
	 * mE 	0: ME1, 1: ME2
	 * keyer 	0-3: Keyer 1-4
	 */
	bool ATEMstd::getKeyerOnAirEnabled(uint8_t mE, uint8_t keyer) {
		return atemKeyerOnAirEnabled[mE][keyer];
	}
	
	/**
	 * Set Keyer On Air; Enabled
	 * mE 	0: ME1, 1: ME2
	 * keyer 	0-3: Keyer 1-4
	 * enabled 	Bit 0: On/Off
	 */
	void ATEMstd::setKeyerOnAirEnabled(uint8_t mE, uint8_t keyer, bool enabled) {
	
  		_prepareCommandPacket(PSTR("CKOn"),4,(_packetBuffer[12+_cBBO+4+4+0]==mE) && (_packetBuffer[12+_cBBO+4+4+1]==keyer));

		_packetBuffer[12+_cBBO+4+4+0] = mE;
		
		_packetBuffer[12+_cBBO+4+4+1] = keyer;
		
		_packetBuffer[12+_cBBO+4+4+2] = enabled;
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Key Mask; Masked
	 * mE 	0: ME1, 1: ME2
	 * keyer 	0-3: Keyer 1-4
	 * masked 	Bit 0: On/Off
	 */
	void ATEMstd::setKeyerMasked(uint8_t mE, uint8_t keyer, bool masked) {
	
  		_prepareCommandPacket(PSTR("CKMs"),12,(_packetBuffer[12+_cBBO+4+4+1]==mE) && (_packetBuffer[12+_cBBO+4+4+2]==keyer));

			// Set Mask: 1
		_packetBuffer[12+_cBBO+4+4+0] |= 1;
				
		_packetBuffer[12+_cBBO+4+4+1] = mE;
		
		_packetBuffer[12+_cBBO+4+4+2] = keyer;
		
		_packetBuffer[12+_cBBO+4+4+3] = masked;
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Key Mask; Top
	 * mE 	0: ME1, 1: ME2
	 * keyer 	0-3: Keyer 1-4
	 * top 	-9000-9000: -9.00-9.00
	 */
	void ATEMstd::setKeyerTop(uint8_t mE, uint8_t keyer, int top) {
	
  		_prepareCommandPacket(PSTR("CKMs"),12,(_packetBuffer[12+_cBBO+4+4+1]==mE) && (_packetBuffer[12+_cBBO+4+4+2]==keyer));

			// Set Mask: 2
		_packetBuffer[12+_cBBO+4+4+0] |= 2;
				
		_packetBuffer[12+_cBBO+4+4+1] = mE;
		
		_packetBuffer[12+_cBBO+4+4+2] = keyer;
		
		_packetBuffer[12+_cBBO+4+4+4] = highByte(top);
		_packetBuffer[12+_cBBO+4+4+5] = lowByte(top);
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Key Mask; Bottom
	 * mE 	0: ME1, 1: ME2
	 * keyer 	0-3: Keyer 1-4
	 * bottom 	-9000-9000: -9.00-9.00
	 */
	void ATEMstd::setKeyerBottom(uint8_t mE, uint8_t keyer, int bottom) {
	
  		_prepareCommandPacket(PSTR("CKMs"),12,(_packetBuffer[12+_cBBO+4+4+1]==mE) && (_packetBuffer[12+_cBBO+4+4+2]==keyer));

			// Set Mask: 4
		_packetBuffer[12+_cBBO+4+4+0] |= 4;
				
		_packetBuffer[12+_cBBO+4+4+1] = mE;
		
		_packetBuffer[12+_cBBO+4+4+2] = keyer;
		
		_packetBuffer[12+_cBBO+4+4+6] = highByte(bottom);
		_packetBuffer[12+_cBBO+4+4+7] = lowByte(bottom);
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Key Mask; Left
	 * mE 	0: ME1, 1: ME2
	 * keyer 	0-3: Keyer 1-4
	 * left 	-16000-16000: -9.00-9.00
	 */
	void ATEMstd::setKeyerLeft(uint8_t mE, uint8_t keyer, int left) {
	
  		_prepareCommandPacket(PSTR("CKMs"),12,(_packetBuffer[12+_cBBO+4+4+1]==mE) && (_packetBuffer[12+_cBBO+4+4+2]==keyer));

			// Set Mask: 8
		_packetBuffer[12+_cBBO+4+4+0] |= 8;
				
		_packetBuffer[12+_cBBO+4+4+1] = mE;
		
		_packetBuffer[12+_cBBO+4+4+2] = keyer;
		
		_packetBuffer[12+_cBBO+4+4+8] = highByte(left);
		_packetBuffer[12+_cBBO+4+4+9] = lowByte(left);
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Key Mask; Right
	 * mE 	0: ME1, 1: ME2
	 * keyer 	0-3: Keyer 1-4
	 * right 	-16000-16000: -9.00-9.00
	 */
	void ATEMstd::setKeyerRight(uint8_t mE, uint8_t keyer, int right) {
	
  		_prepareCommandPacket(PSTR("CKMs"),12,(_packetBuffer[12+_cBBO+4+4+1]==mE) && (_packetBuffer[12+_cBBO+4+4+2]==keyer));

			// Set Mask: 16
		_packetBuffer[12+_cBBO+4+4+0] |= 16;
				
		_packetBuffer[12+_cBBO+4+4+1] = mE;
		
		_packetBuffer[12+_cBBO+4+4+2] = keyer;
		
		_packetBuffer[12+_cBBO+4+4+10] = highByte(right);
		_packetBuffer[12+_cBBO+4+4+11] = lowByte(right);
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Key Fill; Fill Source
	 * mE 	0: ME1, 1: ME2
	 * keyer 	0-3: Keyer 1-4
	 * fillSource 	(See video source list)
	 */
	void ATEMstd::setKeyerFillSource(uint8_t mE, uint8_t keyer, uint16_t fillSource) {
	
  		_prepareCommandPacket(PSTR("CKeF"),4,(_packetBuffer[12+_cBBO+4+4+0]==mE) && (_packetBuffer[12+_cBBO+4+4+1]==keyer));

		_packetBuffer[12+_cBBO+4+4+0] = mE;
		
		_packetBuffer[12+_cBBO+4+4+1] = keyer;
		
		_packetBuffer[12+_cBBO+4+4+2] = highByte(fillSource);
		_packetBuffer[12+_cBBO+4+4+3] = lowByte(fillSource);
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Key Luma; Pre Multiplied
	 * mE 	0: ME1, 1: ME2
	 * keyer 	0-3: Keyer 1-4
	 * preMultiplied 	Bit 0: On/Off
	 */
	void ATEMstd::setKeyLumaPreMultiplied(uint8_t mE, uint8_t keyer, bool preMultiplied) {
	
  		_prepareCommandPacket(PSTR("CKLm"),12,(_packetBuffer[12+_cBBO+4+4+1]==mE) && (_packetBuffer[12+_cBBO+4+4+2]==keyer));

			// Set Mask: 1
		_packetBuffer[12+_cBBO+4+4+0] |= 1;
				
		_packetBuffer[12+_cBBO+4+4+1] = mE;
		
		_packetBuffer[12+_cBBO+4+4+2] = keyer;
		
		_packetBuffer[12+_cBBO+4+4+3] = preMultiplied;
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Key Luma; Clip
	 * mE 	0: ME1, 1: ME2
	 * keyer 	0-3: Keyer 1-4
	 * clip 	0-1000: 0-100%
	 */
	void ATEMstd::setKeyLumaClip(uint8_t mE, uint8_t keyer, uint16_t clip) {
	
  		_prepareCommandPacket(PSTR("CKLm"),12,(_packetBuffer[12+_cBBO+4+4+1]==mE) && (_packetBuffer[12+_cBBO+4+4+2]==keyer));

			// Set Mask: 2
		_packetBuffer[12+_cBBO+4+4+0] |= 2;
				
		_packetBuffer[12+_cBBO+4+4+1] = mE;
		
		_packetBuffer[12+_cBBO+4+4+2] = keyer;
		
		_packetBuffer[12+_cBBO+4+4+4] = highByte(clip);
		_packetBuffer[12+_cBBO+4+4+5] = lowByte(clip);
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Key Luma; Gain
	 * mE 	0: ME1, 1: ME2
	 * keyer 	0-3: Keyer 1-4
	 * gain 	0-1000: 0-100%
	 */
	void ATEMstd::setKeyLumaGain(uint8_t mE, uint8_t keyer, uint16_t gain) {
	
  		_prepareCommandPacket(PSTR("CKLm"),12,(_packetBuffer[12+_cBBO+4+4+1]==mE) && (_packetBuffer[12+_cBBO+4+4+2]==keyer));

			// Set Mask: 4
		_packetBuffer[12+_cBBO+4+4+0] |= 4;
				
		_packetBuffer[12+_cBBO+4+4+1] = mE;
		
		_packetBuffer[12+_cBBO+4+4+2] = keyer;
		
		_packetBuffer[12+_cBBO+4+4+6] = highByte(gain);
		_packetBuffer[12+_cBBO+4+4+7] = lowByte(gain);
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Key Luma; Invert Key
	 * mE 	0: ME1, 1: ME2
	 * keyer 	0-3: Keyer 1-4
	 * invertKey 	Bit 0: On/Off
	 */
	void ATEMstd::setKeyLumaInvertKey(uint8_t mE, uint8_t keyer, bool invertKey) {
	
  		_prepareCommandPacket(PSTR("CKLm"),12,(_packetBuffer[12+_cBBO+4+4+1]==mE) && (_packetBuffer[12+_cBBO+4+4+2]==keyer));

			// Set Mask: 8
		_packetBuffer[12+_cBBO+4+4+0] |= 8;
				
		_packetBuffer[12+_cBBO+4+4+1] = mE;
		
		_packetBuffer[12+_cBBO+4+4+2] = keyer;
		
		_packetBuffer[12+_cBBO+4+4+8] = invertKey;
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Key DVE; Size X
	 * mE 	0: ME1, 1: ME2
	 * keyer 	0-3: Keyer 1-4
	 * sizeX 	Example: 1000: 1.000
	 */
	void ATEMstd::setKeyDVESizeX(uint8_t mE, uint8_t keyer, long sizeX) {
	
  		_prepareCommandPacket(PSTR("CKDV"),64,(_packetBuffer[12+_cBBO+4+4+4]==mE) && (_packetBuffer[12+_cBBO+4+4+5]==keyer));

			// Set Mask: 1
		_packetBuffer[12+_cBBO+4+4+3] |= 1;
				
		_packetBuffer[12+_cBBO+4+4+4] = mE;
		
		_packetBuffer[12+_cBBO+4+4+5] = keyer;
		
		_packetBuffer[12+_cBBO+4+4+8] = (long)((sizeX>>24) & 0xFF);
		_packetBuffer[12+_cBBO+4+4+9] = (long)((sizeX>>16) & 0xFF);
		_packetBuffer[12+_cBBO+4+4+10] = (long)((sizeX>>8) & 0xFF);
		_packetBuffer[12+_cBBO+4+4+11] = (long)(sizeX & 0xFF);
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Key DVE; Size Y
	 * mE 	0: ME1, 1: ME2
	 * keyer 	0-3: Keyer 1-4
	 * sizeY 	Example: 2000: 2.000
	 */
	void ATEMstd::setKeyDVESizeY(uint8_t mE, uint8_t keyer, long sizeY) {
	
  		_prepareCommandPacket(PSTR("CKDV"),64,(_packetBuffer[12+_cBBO+4+4+4]==mE) && (_packetBuffer[12+_cBBO+4+4+5]==keyer));

			// Set Mask: 2
		_packetBuffer[12+_cBBO+4+4+3] |= 2;
				
		_packetBuffer[12+_cBBO+4+4+4] = mE;
		
		_packetBuffer[12+_cBBO+4+4+5] = keyer;
		
		_packetBuffer[12+_cBBO+4+4+12] = (long)((sizeY>>24) & 0xFF);
		_packetBuffer[12+_cBBO+4+4+13] = (long)((sizeY>>16) & 0xFF);
		_packetBuffer[12+_cBBO+4+4+14] = (long)((sizeY>>8) & 0xFF);
		_packetBuffer[12+_cBBO+4+4+15] = (long)(sizeY & 0xFF);
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Key DVE; Position X
	 * mE 	0: ME1, 1: ME2
	 * keyer 	0-3: Keyer 1-4
	 * positionX 	Example: 1000: 1.000
	 */
	void ATEMstd::setKeyDVEPositionX(uint8_t mE, uint8_t keyer, long positionX) {
	
  		_prepareCommandPacket(PSTR("CKDV"),64,(_packetBuffer[12+_cBBO+4+4+4]==mE) && (_packetBuffer[12+_cBBO+4+4+5]==keyer));

			// Set Mask: 4
		_packetBuffer[12+_cBBO+4+4+3] |= 4;
				
		_packetBuffer[12+_cBBO+4+4+4] = mE;
		
		_packetBuffer[12+_cBBO+4+4+5] = keyer;
		
		_packetBuffer[12+_cBBO+4+4+16] = (long)((positionX>>24) & 0xFF);
		_packetBuffer[12+_cBBO+4+4+17] = (long)((positionX>>16) & 0xFF);
		_packetBuffer[12+_cBBO+4+4+18] = (long)((positionX>>8) & 0xFF);
		_packetBuffer[12+_cBBO+4+4+19] = (long)(positionX & 0xFF);
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Key DVE; Position Y
	 * mE 	0: ME1, 1: ME2
	 * keyer 	0-3: Keyer 1-4
	 * positionY 	Example: -1000: -1.000
	 */
	void ATEMstd::setKeyDVEPositionY(uint8_t mE, uint8_t keyer, long positionY) {
	
  		_prepareCommandPacket(PSTR("CKDV"),64,(_packetBuffer[12+_cBBO+4+4+4]==mE) && (_packetBuffer[12+_cBBO+4+4+5]==keyer));

			// Set Mask: 8
		_packetBuffer[12+_cBBO+4+4+3] |= 8;
				
		_packetBuffer[12+_cBBO+4+4+4] = mE;
		
		_packetBuffer[12+_cBBO+4+4+5] = keyer;
		
		_packetBuffer[12+_cBBO+4+4+20] = (long)((positionY>>24) & 0xFF);
		_packetBuffer[12+_cBBO+4+4+21] = (long)((positionY>>16) & 0xFF);
		_packetBuffer[12+_cBBO+4+4+22] = (long)((positionY>>8) & 0xFF);
		_packetBuffer[12+_cBBO+4+4+23] = (long)(positionY & 0xFF);
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Key DVE; Rotation
	 * mE 	0: ME1, 1: ME2
	 * keyer 	0-3: Keyer 1-4
	 * rotation 	Example: 3670: 1 rotation+7 degress
	 */
	void ATEMstd::setKeyDVERotation(uint8_t mE, uint8_t keyer, long rotation) {
	
  		_prepareCommandPacket(PSTR("CKDV"),64,(_packetBuffer[12+_cBBO+4+4+4]==mE) && (_packetBuffer[12+_cBBO+4+4+5]==keyer));

			// Set Mask: 16
		_packetBuffer[12+_cBBO+4+4+3] |= 16;
				
		_packetBuffer[12+_cBBO+4+4+4] = mE;
		
		_packetBuffer[12+_cBBO+4+4+5] = keyer;
		
		_packetBuffer[12+_cBBO+4+4+24] = (long)((rotation>>24) & 0xFF);
		_packetBuffer[12+_cBBO+4+4+25] = (long)((rotation>>16) & 0xFF);
		_packetBuffer[12+_cBBO+4+4+26] = (long)((rotation>>8) & 0xFF);
		_packetBuffer[12+_cBBO+4+4+27] = (long)(rotation & 0xFF);
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Key DVE; Border Enabled
	 * mE 	0: ME1, 1: ME2
	 * keyer 	0-3: Keyer 1-4
	 * borderEnabled 	Bit 0: On/Off
	 */
	void ATEMstd::setKeyDVEBorderEnabled(uint8_t mE, uint8_t keyer, bool borderEnabled) {
	
  		_prepareCommandPacket(PSTR("CKDV"),64,(_packetBuffer[12+_cBBO+4+4+4]==mE) && (_packetBuffer[12+_cBBO+4+4+5]==keyer));

			// Set Mask: 32
		_packetBuffer[12+_cBBO+4+4+3] |= 32;
				
		_packetBuffer[12+_cBBO+4+4+4] = mE;
		
		_packetBuffer[12+_cBBO+4+4+5] = keyer;
		
		_packetBuffer[12+_cBBO+4+4+28] = borderEnabled;
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Key DVE; Shadow
	 * mE 	0: ME1, 1: ME2
	 * keyer 	0-3: Keyer 1-4
	 * shadow 	Bit 0: On/Off
	 */
	void ATEMstd::setKeyDVEShadow(uint8_t mE, uint8_t keyer, bool shadow) {
	
  		_prepareCommandPacket(PSTR("CKDV"),64,(_packetBuffer[12+_cBBO+4+4+4]==mE) && (_packetBuffer[12+_cBBO+4+4+5]==keyer));

			// Set Mask: 64
		_packetBuffer[12+_cBBO+4+4+3] |= 64;
				
		_packetBuffer[12+_cBBO+4+4+4] = mE;
		
		_packetBuffer[12+_cBBO+4+4+5] = keyer;
		
		_packetBuffer[12+_cBBO+4+4+29] = shadow;
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Key DVE; Border Bevel
	 * mE 	0: ME1, 1: ME2
	 * keyer 	0-3: Keyer 1-4
	 * borderBevel 	0: No, 1: In/Out, 2: In, 3: Out
	 */
	void ATEMstd::setKeyDVEBorderBevel(uint8_t mE, uint8_t keyer, uint8_t borderBevel) {
	
  		_prepareCommandPacket(PSTR("CKDV"),64,(_packetBuffer[12+_cBBO+4+4+4]==mE) && (_packetBuffer[12+_cBBO+4+4+5]==keyer));

			// Set Mask: 128
		_packetBuffer[12+_cBBO+4+4+3] |= 128;
				
		_packetBuffer[12+_cBBO+4+4+4] = mE;
		
		_packetBuffer[12+_cBBO+4+4+5] = keyer;
		
		_packetBuffer[12+_cBBO+4+4+30] = borderBevel;
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Key DVE; Border Outer Width
	 * mE 	0: ME1, 1: ME2
	 * keyer 	0-3: Keyer 1-4
	 * borderOuterWidth 	0-1600: 0-16.00
	 */
	void ATEMstd::setKeyDVEBorderOuterWidth(uint8_t mE, uint8_t keyer, uint16_t borderOuterWidth) {
	
  		_prepareCommandPacket(PSTR("CKDV"),64,(_packetBuffer[12+_cBBO+4+4+4]==mE) && (_packetBuffer[12+_cBBO+4+4+5]==keyer));

			// Set Mask: 256
		_packetBuffer[12+_cBBO+4+4+2] |= 1;
				
		_packetBuffer[12+_cBBO+4+4+4] = mE;
		
		_packetBuffer[12+_cBBO+4+4+5] = keyer;
		
		_packetBuffer[12+_cBBO+4+4+32] = highByte(borderOuterWidth);
		_packetBuffer[12+_cBBO+4+4+33] = lowByte(borderOuterWidth);
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Key DVE; Border Inner Width
	 * mE 	0: ME1, 1: ME2
	 * keyer 	0-3: Keyer 1-4
	 * borderInnerWidth 	0-1600: 0-16.00
	 */
	void ATEMstd::setKeyDVEBorderInnerWidth(uint8_t mE, uint8_t keyer, uint16_t borderInnerWidth) {
	
  		_prepareCommandPacket(PSTR("CKDV"),64,(_packetBuffer[12+_cBBO+4+4+4]==mE) && (_packetBuffer[12+_cBBO+4+4+5]==keyer));

			// Set Mask: 512
		_packetBuffer[12+_cBBO+4+4+2] |= 2;
				
		_packetBuffer[12+_cBBO+4+4+4] = mE;
		
		_packetBuffer[12+_cBBO+4+4+5] = keyer;
		
		_packetBuffer[12+_cBBO+4+4+34] = highByte(borderInnerWidth);
		_packetBuffer[12+_cBBO+4+4+35] = lowByte(borderInnerWidth);
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Key DVE; Border Outer Softness
	 * mE 	0: ME1, 1: ME2
	 * keyer 	0-3: Keyer 1-4
	 * borderOuterSoftness 	0-100: 0-100%
	 */
	void ATEMstd::setKeyDVEBorderOuterSoftness(uint8_t mE, uint8_t keyer, uint8_t borderOuterSoftness) {
	
  		_prepareCommandPacket(PSTR("CKDV"),64,(_packetBuffer[12+_cBBO+4+4+4]==mE) && (_packetBuffer[12+_cBBO+4+4+5]==keyer));

			// Set Mask: 1024
		_packetBuffer[12+_cBBO+4+4+2] |= 4;
				
		_packetBuffer[12+_cBBO+4+4+4] = mE;
		
		_packetBuffer[12+_cBBO+4+4+5] = keyer;
		
		_packetBuffer[12+_cBBO+4+4+36] = borderOuterSoftness;
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Key DVE; Border Inner Softness
	 * mE 	0: ME1, 1: ME2
	 * keyer 	0-3: Keyer 1-4
	 * borderInnerSoftness 	0-100: 0-100%
	 */
	void ATEMstd::setKeyDVEBorderInnerSoftness(uint8_t mE, uint8_t keyer, uint8_t borderInnerSoftness) {
	
  		_prepareCommandPacket(PSTR("CKDV"),64,(_packetBuffer[12+_cBBO+4+4+4]==mE) && (_packetBuffer[12+_cBBO+4+4+5]==keyer));

			// Set Mask: 2048
		_packetBuffer[12+_cBBO+4+4+2] |= 8;
				
		_packetBuffer[12+_cBBO+4+4+4] = mE;
		
		_packetBuffer[12+_cBBO+4+4+5] = keyer;
		
		_packetBuffer[12+_cBBO+4+4+37] = borderInnerSoftness;
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Key DVE; Border Bevel Softness
	 * mE 	0: ME1, 1: ME2
	 * keyer 	0-3: Keyer 1-4
	 * borderBevelSoftness 	0-100: 0.0-1.0
	 */
	void ATEMstd::setKeyDVEBorderBevelSoftness(uint8_t mE, uint8_t keyer, uint8_t borderBevelSoftness) {
	
  		_prepareCommandPacket(PSTR("CKDV"),64,(_packetBuffer[12+_cBBO+4+4+4]==mE) && (_packetBuffer[12+_cBBO+4+4+5]==keyer));

			// Set Mask: 4096
		_packetBuffer[12+_cBBO+4+4+2] |= 16;
				
		_packetBuffer[12+_cBBO+4+4+4] = mE;
		
		_packetBuffer[12+_cBBO+4+4+5] = keyer;
		
		_packetBuffer[12+_cBBO+4+4+38] = borderBevelSoftness;
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Key DVE; Border Bevel Position
	 * mE 	0: ME1, 1: ME2
	 * keyer 	0-3: Keyer 1-4
	 * borderBevelPosition 	0-100: 0.0-1.0
	 */
	void ATEMstd::setKeyDVEBorderBevelPosition(uint8_t mE, uint8_t keyer, uint8_t borderBevelPosition) {
	
  		_prepareCommandPacket(PSTR("CKDV"),64,(_packetBuffer[12+_cBBO+4+4+4]==mE) && (_packetBuffer[12+_cBBO+4+4+5]==keyer));

			// Set Mask: 8192
		_packetBuffer[12+_cBBO+4+4+2] |= 32;
				
		_packetBuffer[12+_cBBO+4+4+4] = mE;
		
		_packetBuffer[12+_cBBO+4+4+5] = keyer;
		
		_packetBuffer[12+_cBBO+4+4+39] = borderBevelPosition;
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Key DVE; Border Opacity
	 * mE 	0: ME1, 1: ME2
	 * keyer 	0-3: Keyer 1-4
	 * borderOpacity 	0-100: 0-100%
	 */
	void ATEMstd::setKeyDVEBorderOpacity(uint8_t mE, uint8_t keyer, uint8_t borderOpacity) {
	
  		_prepareCommandPacket(PSTR("CKDV"),64,(_packetBuffer[12+_cBBO+4+4+4]==mE) && (_packetBuffer[12+_cBBO+4+4+5]==keyer));

			// Set Mask: 16384
		_packetBuffer[12+_cBBO+4+4+2] |= 64;
				
		_packetBuffer[12+_cBBO+4+4+4] = mE;
		
		_packetBuffer[12+_cBBO+4+4+5] = keyer;
		
		_packetBuffer[12+_cBBO+4+4+40] = borderOpacity;
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Key DVE; Border Hue
	 * mE 	0: ME1, 1: ME2
	 * keyer 	0-3: Keyer 1-4
	 * borderHue 	0-3599: 0-359.9 Degrees
	 */
	void ATEMstd::setKeyDVEBorderHue(uint8_t mE, uint8_t keyer, uint16_t borderHue) {
	
  		_prepareCommandPacket(PSTR("CKDV"),64,(_packetBuffer[12+_cBBO+4+4+4]==mE) && (_packetBuffer[12+_cBBO+4+4+5]==keyer));

			// Set Mask: 32768
		_packetBuffer[12+_cBBO+4+4+2] |= 128;
				
		_packetBuffer[12+_cBBO+4+4+4] = mE;
		
		_packetBuffer[12+_cBBO+4+4+5] = keyer;
		
		_packetBuffer[12+_cBBO+4+4+42] = highByte(borderHue);
		_packetBuffer[12+_cBBO+4+4+43] = lowByte(borderHue);
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Key DVE; Border Saturation
	 * mE 	0: ME1, 1: ME2
	 * keyer 	0-3: Keyer 1-4
	 * borderSaturation 	0-1000: 0-100%
	 */
	void ATEMstd::setKeyDVEBorderSaturation(uint8_t mE, uint8_t keyer, uint16_t borderSaturation) {
	
  		_prepareCommandPacket(PSTR("CKDV"),64,(_packetBuffer[12+_cBBO+4+4+4]==mE) && (_packetBuffer[12+_cBBO+4+4+5]==keyer));

			// Set Mask: 65536
		_packetBuffer[12+_cBBO+4+4+1] |= 1;
				
		_packetBuffer[12+_cBBO+4+4+4] = mE;
		
		_packetBuffer[12+_cBBO+4+4+5] = keyer;
		
		_packetBuffer[12+_cBBO+4+4+44] = highByte(borderSaturation);
		_packetBuffer[12+_cBBO+4+4+45] = lowByte(borderSaturation);
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Key DVE; Border Luma
	 * mE 	0: ME1, 1: ME2
	 * keyer 	0-3: Keyer 1-4
	 * borderLuma 	0-1000: 0-100%
	 */
	void ATEMstd::setKeyDVEBorderLuma(uint8_t mE, uint8_t keyer, uint16_t borderLuma) {
	
  		_prepareCommandPacket(PSTR("CKDV"),64,(_packetBuffer[12+_cBBO+4+4+4]==mE) && (_packetBuffer[12+_cBBO+4+4+5]==keyer));

			// Set Mask: 131072
		_packetBuffer[12+_cBBO+4+4+1] |= 2;
				
		_packetBuffer[12+_cBBO+4+4+4] = mE;
		
		_packetBuffer[12+_cBBO+4+4+5] = keyer;
		
		_packetBuffer[12+_cBBO+4+4+46] = highByte(borderLuma);
		_packetBuffer[12+_cBBO+4+4+47] = lowByte(borderLuma);
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Key DVE; Light Source Direction
	 * mE 	0: ME1, 1: ME2
	 * keyer 	0-3: Keyer 1-4
	 * lightSourceDirection 	0-3590: 0-359 Degrees
	 */
	void ATEMstd::setKeyDVELightSourceDirection(uint8_t mE, uint8_t keyer, uint16_t lightSourceDirection) {
	
  		_prepareCommandPacket(PSTR("CKDV"),64,(_packetBuffer[12+_cBBO+4+4+4]==mE) && (_packetBuffer[12+_cBBO+4+4+5]==keyer));

			// Set Mask: 262144
		_packetBuffer[12+_cBBO+4+4+1] |= 4;
				
		_packetBuffer[12+_cBBO+4+4+4] = mE;
		
		_packetBuffer[12+_cBBO+4+4+5] = keyer;
		
		_packetBuffer[12+_cBBO+4+4+48] = highByte(lightSourceDirection);
		_packetBuffer[12+_cBBO+4+4+49] = lowByte(lightSourceDirection);
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Key DVE; Light Source Altitude
	 * mE 	0: ME1, 1: ME2
	 * keyer 	0-3: Keyer 1-4
	 * lightSourceAltitude 	10-100: 10-100
	 */
	void ATEMstd::setKeyDVELightSourceAltitude(uint8_t mE, uint8_t keyer, uint8_t lightSourceAltitude) {
	
  		_prepareCommandPacket(PSTR("CKDV"),64,(_packetBuffer[12+_cBBO+4+4+4]==mE) && (_packetBuffer[12+_cBBO+4+4+5]==keyer));

			// Set Mask: 524288
		_packetBuffer[12+_cBBO+4+4+1] |= 8;
				
		_packetBuffer[12+_cBBO+4+4+4] = mE;
		
		_packetBuffer[12+_cBBO+4+4+5] = keyer;
		
		_packetBuffer[12+_cBBO+4+4+50] = lightSourceAltitude;
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Key DVE; Masked
	 * mE 	0: ME1, 1: ME2
	 * keyer 	0-3: Keyer 1-4
	 * masked 	Bit 0: On/Off
	 */
	void ATEMstd::setKeyDVEMasked(uint8_t mE, uint8_t keyer, bool masked) {
	
  		_prepareCommandPacket(PSTR("CKDV"),64,(_packetBuffer[12+_cBBO+4+4+4]==mE) && (_packetBuffer[12+_cBBO+4+4+5]==keyer));

			// Set Mask: 1048576
		_packetBuffer[12+_cBBO+4+4+1] |= 16;
				
		_packetBuffer[12+_cBBO+4+4+4] = mE;
		
		_packetBuffer[12+_cBBO+4+4+5] = keyer;
		
		_packetBuffer[12+_cBBO+4+4+51] = masked;
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Key DVE; Top
	 * mE 	0: ME1, 1: ME2
	 * keyer 	0-3: Keyer 1-4
	 * top 	-9000-9000: -9.00-9.00
	 */
	void ATEMstd::setKeyDVETop(uint8_t mE, uint8_t keyer, int top) {
	
  		_prepareCommandPacket(PSTR("CKDV"),64,(_packetBuffer[12+_cBBO+4+4+4]==mE) && (_packetBuffer[12+_cBBO+4+4+5]==keyer));

			// Set Mask: 2097152
		_packetBuffer[12+_cBBO+4+4+1] |= 32;
				
		_packetBuffer[12+_cBBO+4+4+4] = mE;
		
		_packetBuffer[12+_cBBO+4+4+5] = keyer;
		
		_packetBuffer[12+_cBBO+4+4+52] = highByte(top);
		_packetBuffer[12+_cBBO+4+4+53] = lowByte(top);
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Key DVE; Bottom
	 * mE 	0: ME1, 1: ME2
	 * keyer 	0-3: Keyer 1-4
	 * bottom 	-9000-9000: -9.00-9.00
	 */
	void ATEMstd::setKeyDVEBottom(uint8_t mE, uint8_t keyer, int bottom) {
	
  		_prepareCommandPacket(PSTR("CKDV"),64,(_packetBuffer[12+_cBBO+4+4+4]==mE) && (_packetBuffer[12+_cBBO+4+4+5]==keyer));

			// Set Mask: 4194304
		_packetBuffer[12+_cBBO+4+4+1] |= 64;
				
		_packetBuffer[12+_cBBO+4+4+4] = mE;
		
		_packetBuffer[12+_cBBO+4+4+5] = keyer;
		
		_packetBuffer[12+_cBBO+4+4+54] = highByte(bottom);
		_packetBuffer[12+_cBBO+4+4+55] = lowByte(bottom);
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Key DVE; Left
	 * mE 	0: ME1, 1: ME2
	 * keyer 	0-3: Keyer 1-4
	 * left 	-16000-16000: -9.00-9.00
	 */
	void ATEMstd::setKeyDVELeft(uint8_t mE, uint8_t keyer, int left) {
	
  		_prepareCommandPacket(PSTR("CKDV"),64,(_packetBuffer[12+_cBBO+4+4+4]==mE) && (_packetBuffer[12+_cBBO+4+4+5]==keyer));

			// Set Mask: 8388608
		_packetBuffer[12+_cBBO+4+4+1] |= 128;
				
		_packetBuffer[12+_cBBO+4+4+4] = mE;
		
		_packetBuffer[12+_cBBO+4+4+5] = keyer;
		
		_packetBuffer[12+_cBBO+4+4+56] = highByte(left);
		_packetBuffer[12+_cBBO+4+4+57] = lowByte(left);
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Key DVE; Right
	 * mE 	0: ME1, 1: ME2
	 * keyer 	0-3: Keyer 1-4
	 * right 	-16000-16000: -9.00-9.00
	 */
	void ATEMstd::setKeyDVERight(uint8_t mE, uint8_t keyer, int right) {
	
  		_prepareCommandPacket(PSTR("CKDV"),64,(_packetBuffer[12+_cBBO+4+4+4]==mE) && (_packetBuffer[12+_cBBO+4+4+5]==keyer));

			// Set Mask: 16777216
		_packetBuffer[12+_cBBO+4+4+0] |= 1;
				
		_packetBuffer[12+_cBBO+4+4+4] = mE;
		
		_packetBuffer[12+_cBBO+4+4+5] = keyer;
		
		_packetBuffer[12+_cBBO+4+4+58] = highByte(right);
		_packetBuffer[12+_cBBO+4+4+59] = lowByte(right);
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Key DVE; Rate
	 * mE 	0: ME1, 1: ME2
	 * keyer 	0-3: Keyer 1-4
	 * rate 	1-250: Frames
	 */
	void ATEMstd::setKeyDVERate(uint8_t mE, uint8_t keyer, uint8_t rate) {
	
  		_prepareCommandPacket(PSTR("CKDV"),64,(_packetBuffer[12+_cBBO+4+4+4]==mE) && (_packetBuffer[12+_cBBO+4+4+5]==keyer));

			// Set Mask: 33554432
		_packetBuffer[12+_cBBO+4+4+0] |= 2;
				
		_packetBuffer[12+_cBBO+4+4+4] = mE;
		
		_packetBuffer[12+_cBBO+4+4+5] = keyer;
		
		_packetBuffer[12+_cBBO+4+4+60] = rate;
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Run Flying Key; Key Frame
	 * mE 	0: ME1, 1: ME2
	 * keyer 	0-3: Keyer 1-4
	 * keyFrame 	1: A, 2: B, 3: Full, 4: Run-To-Infinite
	 */
	void ATEMstd::setRunFlyingKeyKeyFrame(uint8_t mE, uint8_t keyer, uint8_t keyFrame) {
	
  		_prepareCommandPacket(PSTR("RFlK"),8,(_packetBuffer[12+_cBBO+4+4+1]==mE) && (_packetBuffer[12+_cBBO+4+4+2]==keyer));

		_packetBuffer[12+_cBBO+4+4+1] = mE;
		
		_packetBuffer[12+_cBBO+4+4+2] = keyer;
		
		_packetBuffer[12+_cBBO+4+4+4] = keyFrame;
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Run Flying Key; Run-to-Infinite-index
	 * mE 	0: ME1, 1: ME2
	 * keyer 	0-3: Keyer 1-4
	 * runtoInfiniteindex 	
	 */
	void ATEMstd::setRunFlyingKeyRuntoInfiniteindex(uint8_t mE, uint8_t keyer, uint8_t runtoInfiniteindex) {
	
  		_prepareCommandPacket(PSTR("RFlK"),8,(_packetBuffer[12+_cBBO+4+4+1]==mE) && (_packetBuffer[12+_cBBO+4+4+2]==keyer));

			// Set Mask: 2
		_packetBuffer[12+_cBBO+4+4+0] |= 2;
				
		_packetBuffer[12+_cBBO+4+4+1] = mE;
		
		_packetBuffer[12+_cBBO+4+4+2] = keyer;
		
		_packetBuffer[12+_cBBO+4+4+5] = runtoInfiniteindex;
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Downstream Keyer; Fill Source
	 * keyer 	0-3: Keyer 1-4
	 * fillSource 	(See video source list)
	 */
	void ATEMstd::setDownstreamKeyerFillSource(uint8_t keyer, uint16_t fillSource) {
	
  		_prepareCommandPacket(PSTR("CDsF"),4,(_packetBuffer[12+_cBBO+4+4+0]==keyer));

		_packetBuffer[12+_cBBO+4+4+0] = keyer;
		
		_packetBuffer[12+_cBBO+4+4+2] = highByte(fillSource);
		_packetBuffer[12+_cBBO+4+4+3] = lowByte(fillSource);
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Downstream Keyer; Key Source
	 * keyer 	0-3: Keyer 1-4
	 * keySource 	(See video source list)
	 */
	void ATEMstd::setDownstreamKeyerKeySource(uint8_t keyer, uint16_t keySource) {
	
  		_prepareCommandPacket(PSTR("CDsC"),4,(_packetBuffer[12+_cBBO+4+4+0]==keyer));

		_packetBuffer[12+_cBBO+4+4+0] = keyer;
		
		_packetBuffer[12+_cBBO+4+4+2] = highByte(keySource);
		_packetBuffer[12+_cBBO+4+4+3] = lowByte(keySource);
		
   		_finishCommandPacket();

	}
	
	/**
	 * Get Downstream Keyer; Tie
	 * keyer 	0: DSK1, 1: DSK2
	 */
	bool ATEMstd::getDownstreamKeyerTie(uint8_t keyer) {
		return atemDownstreamKeyerTie[keyer];
	}
	
	/**
	 * Get Downstream Keyer; Rate
	 * keyer 	0: DSK1, 1: DSK2
	 */
	uint8_t ATEMstd::getDownstreamKeyerRate(uint8_t keyer) {
		return atemDownstreamKeyerRate[keyer];
	}
	
	/**
	 * Get Downstream Keyer; Pre Multiplied
	 * keyer 	0: DSK1, 1: DSK2
	 */
	bool ATEMstd::getDownstreamKeyerPreMultiplied(uint8_t keyer) {
		return atemDownstreamKeyerPreMultiplied[keyer];
	}
	
	/**
	 * Get Downstream Keyer; Clip
	 * keyer 	0: DSK1, 1: DSK2
	 */
	uint16_t ATEMstd::getDownstreamKeyerClip(uint8_t keyer) {
		return atemDownstreamKeyerClip[keyer];
	}
	
	/**
	 * Get Downstream Keyer; Gain
	 * keyer 	0: DSK1, 1: DSK2
	 */
	uint16_t ATEMstd::getDownstreamKeyerGain(uint8_t keyer) {
		return atemDownstreamKeyerGain[keyer];
	}
	
	/**
	 * Get Downstream Keyer; Invert Key
	 * keyer 	0: DSK1, 1: DSK2
	 */
	bool ATEMstd::getDownstreamKeyerInvertKey(uint8_t keyer) {
		return atemDownstreamKeyerInvertKey[keyer];
	}
	
	/**
	 * Get Downstream Keyer; Masked
	 * keyer 	0: DSK1, 1: DSK2
	 */
	bool ATEMstd::getDownstreamKeyerMasked(uint8_t keyer) {
		return atemDownstreamKeyerMasked[keyer];
	}
	
	/**
	 * Get Downstream Keyer; Top
	 * keyer 	0: DSK1, 1: DSK2
	 */
	int ATEMstd::getDownstreamKeyerTop(uint8_t keyer) {
		return atemDownstreamKeyerTop[keyer];
	}
	
	/**
	 * Get Downstream Keyer; Bottom
	 * keyer 	0: DSK1, 1: DSK2
	 */
	int ATEMstd::getDownstreamKeyerBottom(uint8_t keyer) {
		return atemDownstreamKeyerBottom[keyer];
	}
	
	/**
	 * Get Downstream Keyer; Left
	 * keyer 	0: DSK1, 1: DSK2
	 */
	int ATEMstd::getDownstreamKeyerLeft(uint8_t keyer) {
		return atemDownstreamKeyerLeft[keyer];
	}
	
	/**
	 * Get Downstream Keyer; Right
	 * keyer 	0: DSK1, 1: DSK2
	 */
	int ATEMstd::getDownstreamKeyerRight(uint8_t keyer) {
		return atemDownstreamKeyerRight[keyer];
	}
	
	/**
	 * Set Downstream Keyer; Tie
	 * keyer 	0: DSK1, 1: DSK2
	 * tie 	Bit 0: On/Off
	 */
	void ATEMstd::setDownstreamKeyerTie(uint8_t keyer, bool tie) {
	
  		_prepareCommandPacket(PSTR("CDsT"),4,(_packetBuffer[12+_cBBO+4+4+0]==keyer));

		_packetBuffer[12+_cBBO+4+4+0] = keyer;
		
		_packetBuffer[12+_cBBO+4+4+1] = tie;
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Downstream Keyer; Pre Multiplied
	 * keyer 	0-3: Keyer 1-4
	 * preMultiplied 	Bit 0: On/Off
	 */
	void ATEMstd::setDownstreamKeyerPreMultiplied(uint8_t keyer, bool preMultiplied) {
	
  		_prepareCommandPacket(PSTR("CDsG"),12,(_packetBuffer[12+_cBBO+4+4+1]==keyer));

			// Set Mask: 1
		_packetBuffer[12+_cBBO+4+4+0] |= 1;
				
		_packetBuffer[12+_cBBO+4+4+1] = keyer;
		
		_packetBuffer[12+_cBBO+4+4+2] = preMultiplied;
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Downstream Keyer; Clip
	 * keyer 	0-3: Keyer 1-4
	 * clip 	0-1000: 0-100%
	 */
	void ATEMstd::setDownstreamKeyerClip(uint8_t keyer, uint16_t clip) {
	
  		_prepareCommandPacket(PSTR("CDsG"),12,(_packetBuffer[12+_cBBO+4+4+1]==keyer));

			// Set Mask: 2
		_packetBuffer[12+_cBBO+4+4+0] |= 2;
				
		_packetBuffer[12+_cBBO+4+4+1] = keyer;
		
		_packetBuffer[12+_cBBO+4+4+4] = highByte(clip);
		_packetBuffer[12+_cBBO+4+4+5] = lowByte(clip);
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Downstream Keyer; Gain
	 * keyer 	0-3: Keyer 1-4
	 * gain 	0-1000: 0-100%
	 */
	void ATEMstd::setDownstreamKeyerGain(uint8_t keyer, uint16_t gain) {
	
  		_prepareCommandPacket(PSTR("CDsG"),12,(_packetBuffer[12+_cBBO+4+4+1]==keyer));

			// Set Mask: 4
		_packetBuffer[12+_cBBO+4+4+0] |= 4;
				
		_packetBuffer[12+_cBBO+4+4+1] = keyer;
		
		_packetBuffer[12+_cBBO+4+4+6] = highByte(gain);
		_packetBuffer[12+_cBBO+4+4+7] = lowByte(gain);
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Downstream Keyer; Invert Key(??)
	 * keyer 	0-3: Keyer 1-4
	 * invertKey 	Bit 0: On/Off
	 */
	void ATEMstd::setDownstreamKeyerInvertKey(uint8_t keyer, bool invertKey) {
	
  		_prepareCommandPacket(PSTR("CDsG"),12,(_packetBuffer[12+_cBBO+4+4+1]==keyer));

			// Set Mask: 8
		_packetBuffer[12+_cBBO+4+4+0] |= 8;
				
		_packetBuffer[12+_cBBO+4+4+1] = keyer;
		
		_packetBuffer[12+_cBBO+4+4+8] = invertKey;
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Downstream Keyer; Masked
	 * keyer 	0-3: Keyer 1-4
	 * masked 	Bit 0: On/Off
	 */
	void ATEMstd::setDownstreamKeyerMasked(uint8_t keyer, bool masked) {
	
  		_prepareCommandPacket(PSTR("CDsM"),12,(_packetBuffer[12+_cBBO+4+4+1]==keyer));

			// Set Mask: 1
		_packetBuffer[12+_cBBO+4+4+0] |= 1;
				
		_packetBuffer[12+_cBBO+4+4+1] = keyer;
		
		_packetBuffer[12+_cBBO+4+4+2] = masked;
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Downstream Keyer; Top
	 * keyer 	0-3: Keyer 1-4
	 * top 	-9000-9000: -9.00-9.00
	 */
	void ATEMstd::setDownstreamKeyerTop(uint8_t keyer, int top) {
	
  		_prepareCommandPacket(PSTR("CDsM"),12,(_packetBuffer[12+_cBBO+4+4+1]==keyer));

			// Set Mask: 2
		_packetBuffer[12+_cBBO+4+4+0] |= 2;
				
		_packetBuffer[12+_cBBO+4+4+1] = keyer;
		
		_packetBuffer[12+_cBBO+4+4+4] = highByte(top);
		_packetBuffer[12+_cBBO+4+4+5] = lowByte(top);
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Downstream Keyer; Bottom
	 * keyer 	0-3: Keyer 1-4
	 * bottom 	-9000-9000: -9.00-9.00
	 */
	void ATEMstd::setDownstreamKeyerBottom(uint8_t keyer, int bottom) {
	
  		_prepareCommandPacket(PSTR("CDsM"),12,(_packetBuffer[12+_cBBO+4+4+1]==keyer));

			// Set Mask: 4
		_packetBuffer[12+_cBBO+4+4+0] |= 4;
				
		_packetBuffer[12+_cBBO+4+4+1] = keyer;
		
		_packetBuffer[12+_cBBO+4+4+6] = highByte(bottom);
		_packetBuffer[12+_cBBO+4+4+7] = lowByte(bottom);
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Downstream Keyer; Left
	 * keyer 	0-3: Keyer 1-4
	 * left 	-16000-16000: -9.00-9.00
	 */
	void ATEMstd::setDownstreamKeyerLeft(uint8_t keyer, int left) {
	
  		_prepareCommandPacket(PSTR("CDsM"),12,(_packetBuffer[12+_cBBO+4+4+1]==keyer));

			// Set Mask: 8
		_packetBuffer[12+_cBBO+4+4+0] |= 8;
				
		_packetBuffer[12+_cBBO+4+4+1] = keyer;
		
		_packetBuffer[12+_cBBO+4+4+8] = highByte(left);
		_packetBuffer[12+_cBBO+4+4+9] = lowByte(left);
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Downstream Keyer; Right
	 * keyer 	0-3: Keyer 1-4
	 * right 	-16000-16000: -9.00-9.00
	 */
	void ATEMstd::setDownstreamKeyerRight(uint8_t keyer, int right) {
	
  		_prepareCommandPacket(PSTR("CDsM"),12,(_packetBuffer[12+_cBBO+4+4+1]==keyer));

			// Set Mask: 16
		_packetBuffer[12+_cBBO+4+4+0] |= 16;
				
		_packetBuffer[12+_cBBO+4+4+1] = keyer;
		
		_packetBuffer[12+_cBBO+4+4+10] = highByte(right);
		_packetBuffer[12+_cBBO+4+4+11] = lowByte(right);
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Downstream Keyer Auto; Keyer
	 * keyer 	0: DSK1, 1: DSK2
	 */
	void ATEMstd::performDownstreamKeyerAutoKeyer(uint8_t keyer) {
	
  		_prepareCommandPacket(PSTR("DDsA"),4);

		_packetBuffer[12+_cBBO+4+4+0] = keyer;
		
   		_finishCommandPacket();

	}
	
	/**
	 * Get Downstream Keyer; On Air
	 * keyer 	0: DSK1, 1: DSK2
	 */
	bool ATEMstd::getDownstreamKeyerOnAir(uint8_t keyer) {
		return atemDownstreamKeyerOnAir[keyer];
	}
	
	/**
	 * Get Downstream Keyer; In Transition
	 * keyer 	0: DSK1, 1: DSK2
	 */
	bool ATEMstd::getDownstreamKeyerInTransition(uint8_t keyer) {
		return atemDownstreamKeyerInTransition[keyer];
	}
	
	/**
	 * Get Downstream Keyer; Is Auto Transitioning
	 * keyer 	0: DSK1, 1: DSK2
	 */
	bool ATEMstd::getDownstreamKeyerIsAutoTransitioning(uint8_t keyer) {
		return atemDownstreamKeyerIsAutoTransitioning[keyer];
	}
	
	/**
	 * Get Downstream Keyer; Frames Remaining
	 * keyer 	0: DSK1, 1: DSK2
	 */
	uint8_t ATEMstd::getDownstreamKeyerFramesRemaining(uint8_t keyer) {
		return atemDownstreamKeyerFramesRemaining[keyer];
	}
	
	/**
	 * Set Downstream Keyer; On Air
	 * keyer 	0: DSK1, 1: DSK2
	 * onAir 	Bit 0: On/Off
	 */
	void ATEMstd::setDownstreamKeyerOnAir(uint8_t keyer, bool onAir) {
	
  		_prepareCommandPacket(PSTR("CDsL"),4,(_packetBuffer[12+_cBBO+4+4+0]==keyer));

		_packetBuffer[12+_cBBO+4+4+0] = keyer;
		
		_packetBuffer[12+_cBBO+4+4+1] = onAir;
		
   		_finishCommandPacket();

	}
	
	/**
	 * Get Fade-To-Black; Rate
	 * mE 	0: ME1, 1: ME2
	 */
	uint8_t ATEMstd::getFadeToBlackRate(uint8_t mE) {
		return atemFadeToBlackRate[mE];
	}
	
	/**
	 * Set Fade-To-Black; Rate
	 * mE 	0: ME1, 1: ME2
	 * rate 	1-250: Frames
	 */
	void ATEMstd::setFadeToBlackRate(uint8_t mE, uint8_t rate) {
	
  		_prepareCommandPacket(PSTR("FtbC"),4,(_packetBuffer[12+_cBBO+4+4+1]==mE));

			// Set Mask: 1
		_packetBuffer[12+_cBBO+4+4+0] |= 1;
				
		_packetBuffer[12+_cBBO+4+4+1] = mE;
		
		_packetBuffer[12+_cBBO+4+4+2] = rate;
		
   		_finishCommandPacket();

	}
	
	/**
	 * Get Fade-To-Black State; Fully Black
	 * mE 	0: ME1, 1: ME2
	 */
	bool ATEMstd::getFadeToBlackStateFullyBlack(uint8_t mE) {
		return atemFadeToBlackStateFullyBlack[mE];
	}
	
	/**
	 * Get Fade-To-Black State; In Transition
	 * mE 	0: ME1, 1: ME2
	 */
	bool ATEMstd::getFadeToBlackStateInTransition(uint8_t mE) {
		return atemFadeToBlackStateInTransition[mE];
	}
	
	/**
	 * Get Fade-To-Black State; Frames Remaining
	 * mE 	0: ME1, 1: ME2
	 */
	uint8_t ATEMstd::getFadeToBlackStateFramesRemaining(uint8_t mE) {
		return atemFadeToBlackStateFramesRemaining[mE];
	}
	

	
		/**
		 * Set Fade-To-Black; M/E
		 * mE 	0: ME1, 1: ME2
		 */
		void ATEMstd::performFadeToBlackME(uint8_t mE) {
	
  	  		_prepareCommandPacket(PSTR("FtbA"),4);

			_packetBuffer[12+_cBBO+4+4+0] = mE;
			_packetBuffer[12+_cBBO+4+4+1] = 0x02;
		
			_finishCommandPacket();

		}
	
	/**
	 * Set Color Generator; Hue
	 * colorGenerator 	0: Color Generator 1, 1: Color Generator 2
	 * hue 	0-3599: 0-359.9 Degrees
	 */
	void ATEMstd::setColorGeneratorHue(uint8_t colorGenerator, uint16_t hue) {
	
  		_prepareCommandPacket(PSTR("CClV"),8,(_packetBuffer[12+_cBBO+4+4+1]==colorGenerator));

			// Set Mask: 1
		_packetBuffer[12+_cBBO+4+4+0] |= 1;
				
		_packetBuffer[12+_cBBO+4+4+1] = colorGenerator;
		
		_packetBuffer[12+_cBBO+4+4+2] = highByte(hue);
		_packetBuffer[12+_cBBO+4+4+3] = lowByte(hue);
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Color Generator; Saturation
	 * colorGenerator 	0: Color Generator 1, 1: Color Generator 2
	 * saturation 	0-1000: 0-100.0%
	 */
	void ATEMstd::setColorGeneratorSaturation(uint8_t colorGenerator, uint16_t saturation) {
	
  		_prepareCommandPacket(PSTR("CClV"),8,(_packetBuffer[12+_cBBO+4+4+1]==colorGenerator));

			// Set Mask: 2
		_packetBuffer[12+_cBBO+4+4+0] |= 2;
				
		_packetBuffer[12+_cBBO+4+4+1] = colorGenerator;
		
		_packetBuffer[12+_cBBO+4+4+4] = highByte(saturation);
		_packetBuffer[12+_cBBO+4+4+5] = lowByte(saturation);
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Color Generator; Luma
	 * colorGenerator 	0: Color Generator 1, 1: Color Generator 2
	 * luma 	0-1000: 0-100.0%
	 */
	void ATEMstd::setColorGeneratorLuma(uint8_t colorGenerator, uint16_t luma) {
	
  		_prepareCommandPacket(PSTR("CClV"),8,(_packetBuffer[12+_cBBO+4+4+1]==colorGenerator));

			// Set Mask: 4
		_packetBuffer[12+_cBBO+4+4+0] |= 4;
				
		_packetBuffer[12+_cBBO+4+4+1] = colorGenerator;
		
		_packetBuffer[12+_cBBO+4+4+6] = highByte(luma);
		_packetBuffer[12+_cBBO+4+4+7] = lowByte(luma);
		
   		_finishCommandPacket();

	}
	
	/**
	 * Get Aux Source; Input
	 * aUXChannel 	0-5: Aux 1-6
	 */
	uint16_t ATEMstd::getAuxSourceInput(uint8_t aUXChannel) {
		return atemAuxSourceInput[aUXChannel];
	}
	
	/**
	 * Set Aux Source; Input
	 * aUXChannel 	0-5: Aux 1-6
	 * input 	(See video source list)
	 */
	void ATEMstd::setAuxSourceInput(uint8_t aUXChannel, uint16_t input) {
	
  		_prepareCommandPacket(PSTR("CAuS"),4,(_packetBuffer[12+_cBBO+4+4+1]==aUXChannel));

			// Set Mask: 1
		_packetBuffer[12+_cBBO+4+4+0] |= 1;
				
		_packetBuffer[12+_cBBO+4+4+1] = aUXChannel;
		
		_packetBuffer[12+_cBBO+4+4+2] = highByte(input);
		_packetBuffer[12+_cBBO+4+4+3] = lowByte(input);
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Clip Player; Playing
	 * mediaPlayer 	0: Media Player 1, 1: Media Player 2
	 * playing 	Bit 0: On/Off
	 */
	void ATEMstd::setClipPlayerPlaying(uint8_t mediaPlayer, bool playing) {
	
  		_prepareCommandPacket(PSTR("SCPS"),8,(_packetBuffer[12+_cBBO+4+4+1]==mediaPlayer));

			// Set Mask: 1
		_packetBuffer[12+_cBBO+4+4+0] |= 1;
				
		_packetBuffer[12+_cBBO+4+4+1] = mediaPlayer;
		
		_packetBuffer[12+_cBBO+4+4+2] = playing;
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Clip Player; Loop
	 * mediaPlayer 	0: Media Player 1, 1: Media Player 2
	 * loop 	Bit 0: On/Off
	 */
	void ATEMstd::setClipPlayerLoop(uint8_t mediaPlayer, bool loop) {
	
  		_prepareCommandPacket(PSTR("SCPS"),8,(_packetBuffer[12+_cBBO+4+4+1]==mediaPlayer));

			// Set Mask: 2
		_packetBuffer[12+_cBBO+4+4+0] |= 2;
				
		_packetBuffer[12+_cBBO+4+4+1] = mediaPlayer;
		
		_packetBuffer[12+_cBBO+4+4+3] = loop;
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Clip Player; At Beginning
	 * mediaPlayer 	0: Media Player 1, 1: Media Player 2
	 * atBeginning 	Bit 0: On/Off
	 */
	void ATEMstd::setClipPlayerAtBeginning(uint8_t mediaPlayer, bool atBeginning) {
	
  		_prepareCommandPacket(PSTR("SCPS"),8,(_packetBuffer[12+_cBBO+4+4+1]==mediaPlayer));

			// Set Mask: 4
		_packetBuffer[12+_cBBO+4+4+0] |= 4;
				
		_packetBuffer[12+_cBBO+4+4+1] = mediaPlayer;
		
		_packetBuffer[12+_cBBO+4+4+4] = atBeginning;
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Clip Player; Clip Frame
	 * mediaPlayer 	0: Media Player 1, 1: Media Player 2
	 * clipFrame 	
	 */
	void ATEMstd::setClipPlayerClipFrame(uint8_t mediaPlayer, uint16_t clipFrame) {
	
  		_prepareCommandPacket(PSTR("SCPS"),8,(_packetBuffer[12+_cBBO+4+4+1]==mediaPlayer));

			// Set Mask: 8
		_packetBuffer[12+_cBBO+4+4+0] |= 8;
				
		_packetBuffer[12+_cBBO+4+4+1] = mediaPlayer;
		
		_packetBuffer[12+_cBBO+4+4+6] = highByte(clipFrame);
		_packetBuffer[12+_cBBO+4+4+7] = lowByte(clipFrame);
		
   		_finishCommandPacket();

	}
	
	/**
	 * Get Media Player Source; Type
	 * mediaPlayer 	0: Media Player 1, 1: Media Player 2
	 */
	uint8_t ATEMstd::getMediaPlayerSourceType(uint8_t mediaPlayer) {
		return atemMediaPlayerSourceType[mediaPlayer];
	}
	
	/**
	 * Get Media Player Source; Still Index
	 * mediaPlayer 	0: Media Player 1, 1: Media Player 2
	 */
	uint8_t ATEMstd::getMediaPlayerSourceStillIndex(uint8_t mediaPlayer) {
		return atemMediaPlayerSourceStillIndex[mediaPlayer];
	}
	
	/**
	 * Get Media Player Source; Clip Index
	 * mediaPlayer 	0: Media Player 1, 1: Media Player 2
	 */
	uint8_t ATEMstd::getMediaPlayerSourceClipIndex(uint8_t mediaPlayer) {
		return atemMediaPlayerSourceClipIndex[mediaPlayer];
	}
	
	/**
	 * Set Media Player Source; Type
	 * mediaPlayer 	0: Media Player 1, 1: Media Player 2
	 * type 	1: Still, 2: Clip
	 */
	void ATEMstd::setMediaPlayerSourceType(uint8_t mediaPlayer, uint8_t type) {
	
  		_prepareCommandPacket(PSTR("MPSS"),8,(_packetBuffer[12+_cBBO+4+4+1]==mediaPlayer));

			// Set Mask: 1
		_packetBuffer[12+_cBBO+4+4+0] |= 1;
				
		_packetBuffer[12+_cBBO+4+4+1] = mediaPlayer;
		
		_packetBuffer[12+_cBBO+4+4+2] = type;
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Media Player Source; Still Index
	 * mediaPlayer 	0: Media Player 1, 1: Media Player 2
	 * stillIndex 	0-x: Still 1-x
	 */
	void ATEMstd::setMediaPlayerSourceStillIndex(uint8_t mediaPlayer, uint8_t stillIndex) {
	
  		_prepareCommandPacket(PSTR("MPSS"),8,(_packetBuffer[12+_cBBO+4+4+1]==mediaPlayer));

			// Set Mask: 2
		_packetBuffer[12+_cBBO+4+4+0] |= 2;
				
		_packetBuffer[12+_cBBO+4+4+1] = mediaPlayer;
		
		_packetBuffer[12+_cBBO+4+4+3] = stillIndex;
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Media Player Source; Clip Index
	 * mediaPlayer 	0: Media Player 1, 1: Media Player 2
	 * clipIndex 	0-x: Clip 1-x
	 */
	void ATEMstd::setMediaPlayerSourceClipIndex(uint8_t mediaPlayer, uint8_t clipIndex) {
	
  		_prepareCommandPacket(PSTR("MPSS"),8,(_packetBuffer[12+_cBBO+4+4+1]==mediaPlayer));

			// Set Mask: 4
		_packetBuffer[12+_cBBO+4+4+0] |= 4;
				
		_packetBuffer[12+_cBBO+4+4+1] = mediaPlayer;
		
		_packetBuffer[12+_cBBO+4+4+4] = clipIndex;
		
   		_finishCommandPacket();

	}
	
	/**
	 * Get Macro Run Status; State
	 */
	uint8_t ATEMstd::getMacroRunStatusState() {
		return atemMacroRunStatusState;
	}
	
	/**
	 * Get Macro Run Status; Is Looping
	 */
	bool ATEMstd::getMacroRunStatusIsLooping() {
		return atemMacroRunStatusIsLooping;
	}
	
	/**
	 * Get Macro Run Status; Index
	 */
	uint16_t ATEMstd::getMacroRunStatusIndex() {
		return atemMacroRunStatusIndex;
	}
	
	/**
	 * Set Macro Action; Action
	 * index 	0-99: Macro Index Number. 0xFFFF: stop
	 * action 	0: Run Macro, 1: Stop (w/Index 0xFFFF), 2: Stop Recording (w/Index 0xFFFF), 3: Insert Wait for User (w/Index 0xFFFF), 4: Continue (w/Index 0xFFFF), 5: Delete Macro
	 */
	void ATEMstd::setMacroAction(uint16_t index, uint8_t action) {
	
  		_prepareCommandPacket(PSTR("MAct"),4,(_packetBuffer[12+_cBBO+4+4+0]==highByte(index)) && (_packetBuffer[12+_cBBO+4+4+1]==lowByte(index)));

		_packetBuffer[12+_cBBO+4+4+0] = highByte(index);
		_packetBuffer[12+_cBBO+4+4+1] = lowByte(index);
		
		_packetBuffer[12+_cBBO+4+4+2] = action;
		
   		_finishCommandPacket();

	}
	
	/**
	 * Get Macro Properties; Is Used
	 * macroIndex 	0-9: Macro Index Number
	 */
	bool ATEMstd::getMacroPropertiesIsUsed(uint8_t macroIndex) {
		return atemMacroPropertiesIsUsed[macroIndex];
	}
	
	/**
	 * Set Macro Add Pause; Frames
	 * frames 	Number of
	 */
	void ATEMstd::setMacroAddPauseFrames(uint16_t frames) {
	
  		_prepareCommandPacket(PSTR("MSlp"),4);

		_packetBuffer[12+_cBBO+4+4+2] = highByte(frames);
		_packetBuffer[12+_cBBO+4+4+3] = lowByte(frames);
		
   		_finishCommandPacket();

	}
	
	/**
	 * Get Macro Recording Status; Is Recording
	 */
	bool ATEMstd::getMacroRecordingStatusIsRecording() {
		return atemMacroRecordingStatusIsRecording;
	}
	
	/**
	 * Get Macro Recording Status; Index
	 */
	uint16_t ATEMstd::getMacroRecordingStatusIndex() {
		return atemMacroRecordingStatusIndex;
	}
	
	/**
	 * Get Audio Mixer Input; Mix Option
	 * audioSource 	(See audio source list)
	 */
	uint8_t ATEMstd::getAudioMixerInputMixOption(uint16_t audioSource) {
		return atemAudioMixerInputMixOption[getAudioSrcIndex(audioSource)];
	}
	
	/**
	 * Get Audio Mixer Input; Volume
	 * audioSource 	(See audio source list)
	 */
	uint16_t ATEMstd::getAudioMixerInputVolume(uint16_t audioSource) {
		return atemAudioMixerInputVolume[getAudioSrcIndex(audioSource)];
	}
	
	/**
	 * Get Audio Mixer Input; Balance
	 * audioSource 	(See audio source list)
	 */
	int ATEMstd::getAudioMixerInputBalance(uint16_t audioSource) {
		return atemAudioMixerInputBalance[getAudioSrcIndex(audioSource)];
	}
	
	/**
	 * Set Audio Mixer Input; Mix Option
	 * audioSource 	(See audio source list)
	 * mixOption 	0: Off, 1: On, 2: AFV
	 */
	void ATEMstd::setAudioMixerInputMixOption(uint16_t audioSource, uint8_t mixOption) {
	
  		_prepareCommandPacket(PSTR("CAMI"),12,(_packetBuffer[12+_cBBO+4+4+2]==highByte(audioSource)) && (_packetBuffer[12+_cBBO+4+4+3]==lowByte(audioSource)));

			// Set Mask: 1
		_packetBuffer[12+_cBBO+4+4+0] |= 1;
				
		_packetBuffer[12+_cBBO+4+4+2] = highByte(audioSource);
		_packetBuffer[12+_cBBO+4+4+3] = lowByte(audioSource);
		
		_packetBuffer[12+_cBBO+4+4+4] = mixOption;
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Audio Mixer Input; Volume
	 * audioSource 	(See audio source list)
	 * volume 	0-65381: (DB)
	 */
	void ATEMstd::setAudioMixerInputVolume(uint16_t audioSource, uint16_t volume) {
	
  		_prepareCommandPacket(PSTR("CAMI"),12,(_packetBuffer[12+_cBBO+4+4+2]==highByte(audioSource)) && (_packetBuffer[12+_cBBO+4+4+3]==lowByte(audioSource)));

			// Set Mask: 2
		_packetBuffer[12+_cBBO+4+4+0] |= 2;
				
		_packetBuffer[12+_cBBO+4+4+2] = highByte(audioSource);
		_packetBuffer[12+_cBBO+4+4+3] = lowByte(audioSource);
		
		_packetBuffer[12+_cBBO+4+4+6] = highByte(volume);
		_packetBuffer[12+_cBBO+4+4+7] = lowByte(volume);
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Audio Mixer Input; Balance
	 * audioSource 	(See audio source list)
	 * balance 	-10000-10000: Left/Right Extremes
	 */
	void ATEMstd::setAudioMixerInputBalance(uint16_t audioSource, int balance) {
	
  		_prepareCommandPacket(PSTR("CAMI"),12,(_packetBuffer[12+_cBBO+4+4+2]==highByte(audioSource)) && (_packetBuffer[12+_cBBO+4+4+3]==lowByte(audioSource)));

			// Set Mask: 4
		_packetBuffer[12+_cBBO+4+4+0] |= 4;
				
		_packetBuffer[12+_cBBO+4+4+2] = highByte(audioSource);
		_packetBuffer[12+_cBBO+4+4+3] = lowByte(audioSource);
		
		_packetBuffer[12+_cBBO+4+4+8] = highByte(balance);
		_packetBuffer[12+_cBBO+4+4+9] = lowByte(balance);
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Audio Mixer Master; Volume
	 * volume 	0-65381: (DB)
	 */
	void ATEMstd::setAudioMixerMasterVolume(uint16_t volume) {
	
  		_prepareCommandPacket(PSTR("CAMM"),8);

			// Set Mask: 1
		_packetBuffer[12+_cBBO+4+4+0] |= 1;
				
		_packetBuffer[12+_cBBO+4+4+2] = highByte(volume);
		_packetBuffer[12+_cBBO+4+4+3] = lowByte(volume);
		
   		_finishCommandPacket();

	}
	
	/**
	 * Set Audio Levels; Enable
	 * enable 	Bit 0: On/Off
	 */
	void ATEMstd::setAudioLevelsEnable(bool enable) {
	
  		_prepareCommandPacket(PSTR("SALN"),4);

		_packetBuffer[12+_cBBO+4+4+0] = enable;
		
   		_finishCommandPacket();

	}
	
	/**
	 * Get Tally By Index; Sources
	 */
	uint16_t ATEMstd::getTallyByIndexSources() {
		return atemTallyByIndexSources;
	}
	
	/**
	 * Get Tally By Index; Tally Flags
	 * sources 	0-20: Number of
	 */
	uint8_t ATEMstd::getTallyByIndexTallyFlags(uint16_t sources) {
		return atemTallyByIndexTallyFlags[sources];
	}
	




