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




#include "ATEMmax.h"


/**
 * Constructor (using arguments is deprecated! Use begin() instead)
 */
ATEMmax::ATEMmax(){}




uint8_t ATEMmax::getTallyFlags(uint16_t videoSource)  {
  for (uint8_t a = 0; a < getTallyBySourceSources(); a++)  {
    if (getTallyBySourceVideoSource(a) == videoSource)  {
    	return getTallyBySourceTallyFlags(a);
    }
  }
  return 0;
}
uint8_t ATEMmax::getAudioTallyFlags(uint16_t audioSource)  {
  for (uint8_t a = 0; a < getAudioMixerTallySources(); a++)  {
    if (getAudioMixerTallyAudioSource(a) == audioSource)  {
    	return getAudioMixerTallyIsMixedIn(a);
    }
  }
  return 0;
}


void ATEMmax::setCameraControlVideomode(uint8_t input, uint8_t fps, uint8_t resolution, uint8_t interlaced) {
		_prepareCommandPacket(PSTR("CCmd"), 24);

		_packetBuffer[12+_cBBO+4+4+0] = input;

		_packetBuffer[12+_cBBO+4+4+1] = 1;
		_packetBuffer[12+_cBBO+4+4+2] = 0;

		_packetBuffer[12+_cBBO+4+4+4] = 0x01; // Data type: int8

		_packetBuffer[12+_cBBO+4+4+7] = 0x05; // 5 Byte array

		//_packetBuffer[12+_cBBO+4+4+9] = 0x05; // 5 byte array

		_packetBuffer[12+_cBBO+4+4+16] = fps;
		_packetBuffer[12+_cBBO+4+4+17] = 0x00; // Regular M-rate
		_packetBuffer[12+_cBBO+4+4+18] = resolution;
		_packetBuffer[12+_cBBO+4+4+19] = interlaced;
		_packetBuffer[12+_cBBO+4+4+20] = 0x00; // YUV

		_finishCommandPacket();
}


	void ATEMmax::setCameraControlLift(uint8_t input, int liftR, int liftG, int liftB, int liftY) {
  		_prepareCommandPacket(PSTR("CCmd"),24);

			// Preset values:
		_packetBuffer[12+_cBBO+4+4+1] = 8;
		_packetBuffer[12+_cBBO+4+4+2] = 0;

		_packetBuffer[12+_cBBO+4+4+4] = 0x80;
		_packetBuffer[12+_cBBO+4+4+9] = 0x04;

		_packetBuffer[12+_cBBO+4+4+0] = input;

		_packetBuffer[12+_cBBO+4+4+16] = highByte(liftR);
		_packetBuffer[12+_cBBO+4+4+17] = lowByte(liftR);
		_packetBuffer[12+_cBBO+4+4+18] = highByte(liftG);
		_packetBuffer[12+_cBBO+4+4+19] = lowByte(liftG);
		_packetBuffer[12+_cBBO+4+4+20] = highByte(liftB);
		_packetBuffer[12+_cBBO+4+4+21] = lowByte(liftB);
		_packetBuffer[12+_cBBO+4+4+22] = highByte(liftY);
		_packetBuffer[12+_cBBO+4+4+23] = lowByte(liftY);

   		_finishCommandPacket();
	}

	void ATEMmax::setCameraControlGamma(uint8_t input, int gammaR, int gammaG, int gammaB, int gammaY) {
  		_prepareCommandPacket(PSTR("CCmd"),24);

			// Preset values:
		_packetBuffer[12+_cBBO+4+4+1] = 8;
		_packetBuffer[12+_cBBO+4+4+2] = 1;

		_packetBuffer[12+_cBBO+4+4+4] = 0x80;
		_packetBuffer[12+_cBBO+4+4+9] = 0x04;

		_packetBuffer[12+_cBBO+4+4+0] = input;

		_packetBuffer[12+_cBBO+4+4+16] = highByte(gammaR);
		_packetBuffer[12+_cBBO+4+4+17] = lowByte(gammaR);
		_packetBuffer[12+_cBBO+4+4+18] = highByte(gammaG);
		_packetBuffer[12+_cBBO+4+4+19] = lowByte(gammaG);
		_packetBuffer[12+_cBBO+4+4+20] = highByte(gammaB);
		_packetBuffer[12+_cBBO+4+4+21] = lowByte(gammaB);
		_packetBuffer[12+_cBBO+4+4+22] = highByte(gammaY);
		_packetBuffer[12+_cBBO+4+4+23] = lowByte(gammaY);


		_finishCommandPacket();
	}

	void ATEMmax::setCameraControlGain(uint8_t input, int gainR, int gainG, int gainB, int gainY) {
  		_prepareCommandPacket(PSTR("CCmd"),24);

			// Preset values:
		_packetBuffer[12+_cBBO+4+4+1] = 8;
		_packetBuffer[12+_cBBO+4+4+2] = 2;

		_packetBuffer[12+_cBBO+4+4+4] = 0x80;
		_packetBuffer[12+_cBBO+4+4+9] = 0x04;

		_packetBuffer[12+_cBBO+4+4+0] = input;

		_packetBuffer[12+_cBBO+4+4+16] = highByte(gainR);
		_packetBuffer[12+_cBBO+4+4+17] = lowByte(gainR);
		_packetBuffer[12+_cBBO+4+4+18] = highByte(gainG);
		_packetBuffer[12+_cBBO+4+4+19] = lowByte(gainG);
		_packetBuffer[12+_cBBO+4+4+20] = highByte(gainB);
		_packetBuffer[12+_cBBO+4+4+21] = lowByte(gainB);
		_packetBuffer[12+_cBBO+4+4+22] = highByte(gainY);
		_packetBuffer[12+_cBBO+4+4+23] = lowByte(gainY);

		_finishCommandPacket();
	}

	void ATEMmax::setCameraControlHueSaturation(uint8_t input, int hue, int saturation) {
  		_prepareCommandPacket(PSTR("CCmd"),24);

			// Preset values:
		_packetBuffer[12+_cBBO+4+4+1] = 8;
		_packetBuffer[12+_cBBO+4+4+2] = 6;

		_packetBuffer[12+_cBBO+4+4+4] = 0x80;
		_packetBuffer[12+_cBBO+4+4+9] = 0x02;

		_packetBuffer[12+_cBBO+4+4+0] = input;

		_packetBuffer[12+_cBBO+4+4+16] = highByte(hue);
		_packetBuffer[12+_cBBO+4+4+17] = lowByte(hue);

		_packetBuffer[12+_cBBO+4+4+18] = highByte(saturation);
		_packetBuffer[12+_cBBO+4+4+19] = lowByte(saturation);

		_finishCommandPacket();
	}




		// *********************************
		// **
		// ** Implementations in ATEMmax.c:
		// **
		// *********************************


		// *********************************
		// **
		// ** Implementations in ATEMmax.c:
		// **
		// *********************************

		void ATEMmax::_parseGetCommands(const char *cmdStr)	{
			uint8_t mE,multiViewer,windowIndex,keyer,keyFrame,colorGenerator,aUXChannel,input,mediaPlayer,clipBank,stillBank,macroIndex,box;
			uint16_t videoSource,index,audioSource,sources;
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
			if(!strcmp_P(cmdStr, PSTR("_pin"))) {
				
					memset(atemProductIdName,0,45);
					strncpy(atemProductIdName, (char *)(_packetBuffer+0), 44);
					#if ATEM_debug
					if ((_serialOutput==0x80 && hasInitialized()) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemProductIdName = "));
						Serial.println(atemProductIdName);
					}
					#endif
					
			} else 
			if(!strcmp_P(cmdStr, PSTR("Warn"))) {
				
					memset(atemWarningText,0,45);
					strncpy(atemWarningText, (char *)(_packetBuffer+0), 44);
					#if ATEM_debug
					if ((_serialOutput==0x80 && hasInitialized()) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemWarningText = "));
						Serial.println(atemWarningText);
					}
					#endif
					
			} else 
			if(!strcmp_P(cmdStr, PSTR("_top"))) {
				
					#if ATEM_debug
					temp = atemTopologyMEs;
					#endif
					atemTopologyMEs = _packetBuffer[0];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemTopologyMEs!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemTopologyMEs = "));
						Serial.println(atemTopologyMEs);
					}
					#endif
					
					#if ATEM_debug
					temp = atemTopologySources;
					#endif
					atemTopologySources = _packetBuffer[1];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemTopologySources!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemTopologySources = "));
						Serial.println(atemTopologySources);
					}
					#endif
					
					#if ATEM_debug
					temp = atemTopologyColorGenerators;
					#endif
					atemTopologyColorGenerators = _packetBuffer[2];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemTopologyColorGenerators!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemTopologyColorGenerators = "));
						Serial.println(atemTopologyColorGenerators);
					}
					#endif
					
					#if ATEM_debug
					temp = atemTopologyAUXbusses;
					#endif
					atemTopologyAUXbusses = _packetBuffer[3];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemTopologyAUXbusses!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemTopologyAUXbusses = "));
						Serial.println(atemTopologyAUXbusses);
					}
					#endif
					
					#if ATEM_debug
					temp = atemTopologyDownstreamKeyes;
					#endif
					atemTopologyDownstreamKeyes = _packetBuffer[4];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemTopologyDownstreamKeyes!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemTopologyDownstreamKeyes = "));
						Serial.println(atemTopologyDownstreamKeyes);
					}
					#endif
					
					#if ATEM_debug
					temp = atemTopologyStingers;
					#endif
					atemTopologyStingers = _packetBuffer[5];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemTopologyStingers!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemTopologyStingers = "));
						Serial.println(atemTopologyStingers);
					}
					#endif
					
					#if ATEM_debug
					temp = atemTopologyDVEs;
					#endif
					atemTopologyDVEs = _packetBuffer[6];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemTopologyDVEs!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemTopologyDVEs = "));
						Serial.println(atemTopologyDVEs);
					}
					#endif
					
					#if ATEM_debug
					temp = atemTopologySuperSources;
					#endif
					atemTopologySuperSources = _packetBuffer[7];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemTopologySuperSources!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemTopologySuperSources = "));
						Serial.println(atemTopologySuperSources);
					}
					#endif
					
					#if ATEM_debug
					temp = atemTopologyHasSDOutput;
					#endif
					atemTopologyHasSDOutput = _packetBuffer[9];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemTopologyHasSDOutput!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemTopologyHasSDOutput = "));
						Serial.println(atemTopologyHasSDOutput);
					}
					#endif
					
			} else 
			if(!strcmp_P(cmdStr, PSTR("_MeC"))) {
				
				mE = _packetBuffer[0];
				if (mE<=1) {
					#if ATEM_debug
					temp = atemMixEffectConfigKeyersOnME[mE];
					#endif
					atemMixEffectConfigKeyersOnME[mE] = _packetBuffer[1];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemMixEffectConfigKeyersOnME[mE]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemMixEffectConfigKeyersOnME[mE=")); Serial.print(mE); Serial.print(F("] = "));
						Serial.println(atemMixEffectConfigKeyersOnME[mE]);
					}
					#endif
					
				}
			} else 
			if(!strcmp_P(cmdStr, PSTR("_mpl"))) {
				
					#if ATEM_debug
					temp = atemMediaPlayersStillBanks;
					#endif
					atemMediaPlayersStillBanks = _packetBuffer[0];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemMediaPlayersStillBanks!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemMediaPlayersStillBanks = "));
						Serial.println(atemMediaPlayersStillBanks);
					}
					#endif
					
					#if ATEM_debug
					temp = atemMediaPlayersClipBanks;
					#endif
					atemMediaPlayersClipBanks = _packetBuffer[1];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemMediaPlayersClipBanks!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemMediaPlayersClipBanks = "));
						Serial.println(atemMediaPlayersClipBanks);
					}
					#endif
					
			} else 
			if(!strcmp_P(cmdStr, PSTR("_MvC"))) {
				
					#if ATEM_debug
					temp = atemMultiViewConfigMultiViewers;
					#endif
					atemMultiViewConfigMultiViewers = _packetBuffer[0];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemMultiViewConfigMultiViewers!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemMultiViewConfigMultiViewers = "));
						Serial.println(atemMultiViewConfigMultiViewers);
					}
					#endif
					
			} else 
			if(!strcmp_P(cmdStr, PSTR("_SSC"))) {
				
					#if ATEM_debug
					temp = atemSuperSourceConfigBoxes;
					#endif
					atemSuperSourceConfigBoxes = _packetBuffer[0];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemSuperSourceConfigBoxes!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemSuperSourceConfigBoxes = "));
						Serial.println(atemSuperSourceConfigBoxes);
					}
					#endif
					
			} else 
			if(!strcmp_P(cmdStr, PSTR("_TlC"))) {
				
					#if ATEM_debug
					temp = atemTallyChannelConfigTallyChannels;
					#endif
					atemTallyChannelConfigTallyChannels = _packetBuffer[4];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemTallyChannelConfigTallyChannels!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemTallyChannelConfigTallyChannels = "));
						Serial.println(atemTallyChannelConfigTallyChannels);
					}
					#endif
					
			} else 
			if(!strcmp_P(cmdStr, PSTR("_AMC"))) {
				
					#if ATEM_debug
					temp = atemAudioMixerConfigAudioChannels;
					#endif
					atemAudioMixerConfigAudioChannels = _packetBuffer[0];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemAudioMixerConfigAudioChannels!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemAudioMixerConfigAudioChannels = "));
						Serial.println(atemAudioMixerConfigAudioChannels);
					}
					#endif
					
					#if ATEM_debug
					temp = atemAudioMixerConfigHasMonitor;
					#endif
					atemAudioMixerConfigHasMonitor = _packetBuffer[1];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemAudioMixerConfigHasMonitor!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemAudioMixerConfigHasMonitor = "));
						Serial.println(atemAudioMixerConfigHasMonitor);
					}
					#endif
					
			} else 
			if(!strcmp_P(cmdStr, PSTR("_VMC"))) {
				
					#if ATEM_debug
					temp = atemVideoMixerConfigModes;
					#endif
					atemVideoMixerConfigModes = (uint32_t)_packetBuffer[1]<<16 | (uint32_t)_packetBuffer[2]<<8 | (uint32_t)_packetBuffer[3];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemVideoMixerConfigModes!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemVideoMixerConfigModes = "));
						Serial.println(atemVideoMixerConfigModes);
					}
					#endif
					
			} else 
			if(!strcmp_P(cmdStr, PSTR("_MAC"))) {
				
					#if ATEM_debug
					temp = atemMacroPoolBanks;
					#endif
					atemMacroPoolBanks = _packetBuffer[0];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemMacroPoolBanks!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemMacroPoolBanks = "));
						Serial.println(atemMacroPoolBanks);
					}
					#endif
					
			} else 
			if(!strcmp_P(cmdStr, PSTR("Powr"))) {
				
					#if ATEM_debug
					temp = atemPowerStatus;
					#endif
					atemPowerStatus = _packetBuffer[0];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemPowerStatus!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemPowerStatus = "));
						Serial.println(atemPowerStatus);
					}
					#endif
					
			} else 
			if(!strcmp_P(cmdStr, PSTR("DcOt"))) {
				
					#if ATEM_debug
					temp = atemDownConverterMode;
					#endif
					atemDownConverterMode = _packetBuffer[0];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemDownConverterMode!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemDownConverterMode = "));
						Serial.println(atemDownConverterMode);
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
			if(!strcmp_P(cmdStr, PSTR("InPr"))) {
				
				videoSource = word(_packetBuffer[0],_packetBuffer[1]);
				if (getVideoSrcIndex(videoSource)<=46) {
					memset(atemInputLongName[getVideoSrcIndex(videoSource)],0,21);
					strncpy(atemInputLongName[getVideoSrcIndex(videoSource)], (char *)(_packetBuffer+2), 20);
					#if ATEM_debug
					if ((_serialOutput==0x80 && hasInitialized()) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemInputLongName[getVideoSrcIndex(videoSource)=")); Serial.print(getVideoSrcIndex(videoSource)); Serial.print(F("] = "));
						Serial.println(atemInputLongName[getVideoSrcIndex(videoSource)]);
					}
					#endif
					
					memset(atemInputShortName[getVideoSrcIndex(videoSource)],0,5);
					strncpy(atemInputShortName[getVideoSrcIndex(videoSource)], (char *)(_packetBuffer+22), 4);
					#if ATEM_debug
					if ((_serialOutput==0x80 && hasInitialized()) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemInputShortName[getVideoSrcIndex(videoSource)=")); Serial.print(getVideoSrcIndex(videoSource)); Serial.print(F("] = "));
						Serial.println(atemInputShortName[getVideoSrcIndex(videoSource)]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemInputAvailableExternalPortTypes[getVideoSrcIndex(videoSource)];
					#endif
					atemInputAvailableExternalPortTypes[getVideoSrcIndex(videoSource)] = _packetBuffer[27];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemInputAvailableExternalPortTypes[getVideoSrcIndex(videoSource)]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemInputAvailableExternalPortTypes[getVideoSrcIndex(videoSource)=")); Serial.print(getVideoSrcIndex(videoSource)); Serial.print(F("] = "));
						Serial.println(atemInputAvailableExternalPortTypes[getVideoSrcIndex(videoSource)]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemInputExternalPortType[getVideoSrcIndex(videoSource)];
					#endif
					atemInputExternalPortType[getVideoSrcIndex(videoSource)] = _packetBuffer[29];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemInputExternalPortType[getVideoSrcIndex(videoSource)]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemInputExternalPortType[getVideoSrcIndex(videoSource)=")); Serial.print(getVideoSrcIndex(videoSource)); Serial.print(F("] = "));
						Serial.println(atemInputExternalPortType[getVideoSrcIndex(videoSource)]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemInputPortType[getVideoSrcIndex(videoSource)];
					#endif
					atemInputPortType[getVideoSrcIndex(videoSource)] = _packetBuffer[30];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemInputPortType[getVideoSrcIndex(videoSource)]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemInputPortType[getVideoSrcIndex(videoSource)=")); Serial.print(getVideoSrcIndex(videoSource)); Serial.print(F("] = "));
						Serial.println(atemInputPortType[getVideoSrcIndex(videoSource)]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemInputAvailability[getVideoSrcIndex(videoSource)];
					#endif
					atemInputAvailability[getVideoSrcIndex(videoSource)] = _packetBuffer[34];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemInputAvailability[getVideoSrcIndex(videoSource)]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemInputAvailability[getVideoSrcIndex(videoSource)=")); Serial.print(getVideoSrcIndex(videoSource)); Serial.print(F("] = "));
						Serial.println(atemInputAvailability[getVideoSrcIndex(videoSource)]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemInputMEAvailability[getVideoSrcIndex(videoSource)];
					#endif
					atemInputMEAvailability[getVideoSrcIndex(videoSource)] = _packetBuffer[35];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemInputMEAvailability[getVideoSrcIndex(videoSource)]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemInputMEAvailability[getVideoSrcIndex(videoSource)=")); Serial.print(getVideoSrcIndex(videoSource)); Serial.print(F("] = "));
						Serial.println(atemInputMEAvailability[getVideoSrcIndex(videoSource)]);
					}
					#endif
					
				}
			} else 
			if(!strcmp_P(cmdStr, PSTR("MvPr"))) {
				
				multiViewer = _packetBuffer[0];
				if (multiViewer<=1) {
					#if ATEM_debug
					temp = atemMultiViewerPropertiesLayout[multiViewer];
					#endif
					atemMultiViewerPropertiesLayout[multiViewer] = _packetBuffer[1];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemMultiViewerPropertiesLayout[multiViewer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemMultiViewerPropertiesLayout[multiViewer=")); Serial.print(multiViewer); Serial.print(F("] = "));
						Serial.println(atemMultiViewerPropertiesLayout[multiViewer]);
					}
					#endif
					
				}
			} else 
			if(!strcmp_P(cmdStr, PSTR("MvIn"))) {
				
				multiViewer = _packetBuffer[0];
				windowIndex = _packetBuffer[1];
				if (multiViewer<=1 && windowIndex<=9) {
					#if ATEM_debug
					temp = atemMultiViewerInputVideoSource[multiViewer][windowIndex];
					#endif
					atemMultiViewerInputVideoSource[multiViewer][windowIndex] = word(_packetBuffer[2], _packetBuffer[3]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemMultiViewerInputVideoSource[multiViewer][windowIndex]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemMultiViewerInputVideoSource[multiViewer=")); Serial.print(multiViewer); Serial.print(F("][windowIndex=")); Serial.print(windowIndex); Serial.print(F("] = "));
						Serial.println(atemMultiViewerInputVideoSource[multiViewer][windowIndex]);
					}
					#endif
					
				}
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
					
					#if ATEM_debug
					temp = atemTransitionStyleNext[mE];
					#endif
					atemTransitionStyleNext[mE] = _packetBuffer[3];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemTransitionStyleNext[mE]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemTransitionStyleNext[mE=")); Serial.print(mE); Serial.print(F("] = "));
						Serial.println(atemTransitionStyleNext[mE]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemTransitionNextTransitionNext[mE];
					#endif
					atemTransitionNextTransitionNext[mE] = _packetBuffer[4];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemTransitionNextTransitionNext[mE]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemTransitionNextTransitionNext[mE=")); Serial.print(mE); Serial.print(F("] = "));
						Serial.println(atemTransitionNextTransitionNext[mE]);
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
			if(!strcmp_P(cmdStr, PSTR("TDpP"))) {
				
				mE = _packetBuffer[0];
				if (mE<=1) {
					#if ATEM_debug
					temp = atemTransitionDipRate[mE];
					#endif
					atemTransitionDipRate[mE] = _packetBuffer[1];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemTransitionDipRate[mE]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemTransitionDipRate[mE=")); Serial.print(mE); Serial.print(F("] = "));
						Serial.println(atemTransitionDipRate[mE]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemTransitionDipInput[mE];
					#endif
					atemTransitionDipInput[mE] = word(_packetBuffer[2], _packetBuffer[3]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemTransitionDipInput[mE]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemTransitionDipInput[mE=")); Serial.print(mE); Serial.print(F("] = "));
						Serial.println(atemTransitionDipInput[mE]);
					}
					#endif
					
				}
			} else 
			if(!strcmp_P(cmdStr, PSTR("TWpP"))) {
				
				mE = _packetBuffer[0];
				if (mE<=1) {
					#if ATEM_debug
					temp = atemTransitionWipeRate[mE];
					#endif
					atemTransitionWipeRate[mE] = _packetBuffer[1];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemTransitionWipeRate[mE]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemTransitionWipeRate[mE=")); Serial.print(mE); Serial.print(F("] = "));
						Serial.println(atemTransitionWipeRate[mE]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemTransitionWipePattern[mE];
					#endif
					atemTransitionWipePattern[mE] = _packetBuffer[2];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemTransitionWipePattern[mE]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemTransitionWipePattern[mE=")); Serial.print(mE); Serial.print(F("] = "));
						Serial.println(atemTransitionWipePattern[mE]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemTransitionWipeWidth[mE];
					#endif
					atemTransitionWipeWidth[mE] = word(_packetBuffer[4], _packetBuffer[5]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemTransitionWipeWidth[mE]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemTransitionWipeWidth[mE=")); Serial.print(mE); Serial.print(F("] = "));
						Serial.println(atemTransitionWipeWidth[mE]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemTransitionWipeFillSource[mE];
					#endif
					atemTransitionWipeFillSource[mE] = word(_packetBuffer[6], _packetBuffer[7]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemTransitionWipeFillSource[mE]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemTransitionWipeFillSource[mE=")); Serial.print(mE); Serial.print(F("] = "));
						Serial.println(atemTransitionWipeFillSource[mE]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemTransitionWipeSymmetry[mE];
					#endif
					atemTransitionWipeSymmetry[mE] = word(_packetBuffer[8], _packetBuffer[9]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemTransitionWipeSymmetry[mE]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemTransitionWipeSymmetry[mE=")); Serial.print(mE); Serial.print(F("] = "));
						Serial.println(atemTransitionWipeSymmetry[mE]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemTransitionWipeSoftness[mE];
					#endif
					atemTransitionWipeSoftness[mE] = word(_packetBuffer[10], _packetBuffer[11]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemTransitionWipeSoftness[mE]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemTransitionWipeSoftness[mE=")); Serial.print(mE); Serial.print(F("] = "));
						Serial.println(atemTransitionWipeSoftness[mE]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemTransitionWipePositionX[mE];
					#endif
					atemTransitionWipePositionX[mE] = word(_packetBuffer[12], _packetBuffer[13]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemTransitionWipePositionX[mE]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemTransitionWipePositionX[mE=")); Serial.print(mE); Serial.print(F("] = "));
						Serial.println(atemTransitionWipePositionX[mE]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemTransitionWipePositionY[mE];
					#endif
					atemTransitionWipePositionY[mE] = word(_packetBuffer[14], _packetBuffer[15]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemTransitionWipePositionY[mE]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemTransitionWipePositionY[mE=")); Serial.print(mE); Serial.print(F("] = "));
						Serial.println(atemTransitionWipePositionY[mE]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemTransitionWipeReverse[mE];
					#endif
					atemTransitionWipeReverse[mE] = _packetBuffer[16];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemTransitionWipeReverse[mE]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemTransitionWipeReverse[mE=")); Serial.print(mE); Serial.print(F("] = "));
						Serial.println(atemTransitionWipeReverse[mE]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemTransitionWipeFlipFlop[mE];
					#endif
					atemTransitionWipeFlipFlop[mE] = _packetBuffer[17];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemTransitionWipeFlipFlop[mE]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemTransitionWipeFlipFlop[mE=")); Serial.print(mE); Serial.print(F("] = "));
						Serial.println(atemTransitionWipeFlipFlop[mE]);
					}
					#endif
					
				}
			} else 
			if(!strcmp_P(cmdStr, PSTR("TDvP"))) {
				
				mE = _packetBuffer[0];
				if (mE<=1) {
					#if ATEM_debug
					temp = atemTransitionDVERate[mE];
					#endif
					atemTransitionDVERate[mE] = _packetBuffer[1];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemTransitionDVERate[mE]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemTransitionDVERate[mE=")); Serial.print(mE); Serial.print(F("] = "));
						Serial.println(atemTransitionDVERate[mE]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemTransitionDVEStyle[mE];
					#endif
					atemTransitionDVEStyle[mE] = _packetBuffer[3];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemTransitionDVEStyle[mE]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemTransitionDVEStyle[mE=")); Serial.print(mE); Serial.print(F("] = "));
						Serial.println(atemTransitionDVEStyle[mE]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemTransitionDVEFillSource[mE];
					#endif
					atemTransitionDVEFillSource[mE] = word(_packetBuffer[4], _packetBuffer[5]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemTransitionDVEFillSource[mE]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemTransitionDVEFillSource[mE=")); Serial.print(mE); Serial.print(F("] = "));
						Serial.println(atemTransitionDVEFillSource[mE]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemTransitionDVEKeySource[mE];
					#endif
					atemTransitionDVEKeySource[mE] = word(_packetBuffer[6], _packetBuffer[7]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemTransitionDVEKeySource[mE]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemTransitionDVEKeySource[mE=")); Serial.print(mE); Serial.print(F("] = "));
						Serial.println(atemTransitionDVEKeySource[mE]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemTransitionDVEEnableKey[mE];
					#endif
					atemTransitionDVEEnableKey[mE] = _packetBuffer[8];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemTransitionDVEEnableKey[mE]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemTransitionDVEEnableKey[mE=")); Serial.print(mE); Serial.print(F("] = "));
						Serial.println(atemTransitionDVEEnableKey[mE]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemTransitionDVEPreMultiplied[mE];
					#endif
					atemTransitionDVEPreMultiplied[mE] = _packetBuffer[9];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemTransitionDVEPreMultiplied[mE]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemTransitionDVEPreMultiplied[mE=")); Serial.print(mE); Serial.print(F("] = "));
						Serial.println(atemTransitionDVEPreMultiplied[mE]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemTransitionDVEClip[mE];
					#endif
					atemTransitionDVEClip[mE] = word(_packetBuffer[10], _packetBuffer[11]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemTransitionDVEClip[mE]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemTransitionDVEClip[mE=")); Serial.print(mE); Serial.print(F("] = "));
						Serial.println(atemTransitionDVEClip[mE]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemTransitionDVEGain[mE];
					#endif
					atemTransitionDVEGain[mE] = word(_packetBuffer[12], _packetBuffer[13]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemTransitionDVEGain[mE]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemTransitionDVEGain[mE=")); Serial.print(mE); Serial.print(F("] = "));
						Serial.println(atemTransitionDVEGain[mE]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemTransitionDVEInvertKey[mE];
					#endif
					atemTransitionDVEInvertKey[mE] = _packetBuffer[14];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemTransitionDVEInvertKey[mE]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemTransitionDVEInvertKey[mE=")); Serial.print(mE); Serial.print(F("] = "));
						Serial.println(atemTransitionDVEInvertKey[mE]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemTransitionDVEReverse[mE];
					#endif
					atemTransitionDVEReverse[mE] = _packetBuffer[15];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemTransitionDVEReverse[mE]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemTransitionDVEReverse[mE=")); Serial.print(mE); Serial.print(F("] = "));
						Serial.println(atemTransitionDVEReverse[mE]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemTransitionDVEFlipFlop[mE];
					#endif
					atemTransitionDVEFlipFlop[mE] = _packetBuffer[16];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemTransitionDVEFlipFlop[mE]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemTransitionDVEFlipFlop[mE=")); Serial.print(mE); Serial.print(F("] = "));
						Serial.println(atemTransitionDVEFlipFlop[mE]);
					}
					#endif
					
				}
			} else 
			if(!strcmp_P(cmdStr, PSTR("TStP"))) {
				
				mE = _packetBuffer[0];
				if (mE<=1) {
					#if ATEM_debug
					temp = atemTransitionStingerSource[mE];
					#endif
					atemTransitionStingerSource[mE] = _packetBuffer[1];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemTransitionStingerSource[mE]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemTransitionStingerSource[mE=")); Serial.print(mE); Serial.print(F("] = "));
						Serial.println(atemTransitionStingerSource[mE]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemTransitionStingerPreMultiplied[mE];
					#endif
					atemTransitionStingerPreMultiplied[mE] = _packetBuffer[2];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemTransitionStingerPreMultiplied[mE]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemTransitionStingerPreMultiplied[mE=")); Serial.print(mE); Serial.print(F("] = "));
						Serial.println(atemTransitionStingerPreMultiplied[mE]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemTransitionStingerClip[mE];
					#endif
					atemTransitionStingerClip[mE] = word(_packetBuffer[4], _packetBuffer[5]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemTransitionStingerClip[mE]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemTransitionStingerClip[mE=")); Serial.print(mE); Serial.print(F("] = "));
						Serial.println(atemTransitionStingerClip[mE]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemTransitionStingerGain[mE];
					#endif
					atemTransitionStingerGain[mE] = word(_packetBuffer[6], _packetBuffer[7]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemTransitionStingerGain[mE]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemTransitionStingerGain[mE=")); Serial.print(mE); Serial.print(F("] = "));
						Serial.println(atemTransitionStingerGain[mE]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemTransitionStingerInvertKey[mE];
					#endif
					atemTransitionStingerInvertKey[mE] = _packetBuffer[8];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemTransitionStingerInvertKey[mE]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemTransitionStingerInvertKey[mE=")); Serial.print(mE); Serial.print(F("] = "));
						Serial.println(atemTransitionStingerInvertKey[mE]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemTransitionStingerPreRoll[mE];
					#endif
					atemTransitionStingerPreRoll[mE] = word(_packetBuffer[10], _packetBuffer[11]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemTransitionStingerPreRoll[mE]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemTransitionStingerPreRoll[mE=")); Serial.print(mE); Serial.print(F("] = "));
						Serial.println(atemTransitionStingerPreRoll[mE]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemTransitionStingerClipDuration[mE];
					#endif
					atemTransitionStingerClipDuration[mE] = word(_packetBuffer[12], _packetBuffer[13]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemTransitionStingerClipDuration[mE]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemTransitionStingerClipDuration[mE=")); Serial.print(mE); Serial.print(F("] = "));
						Serial.println(atemTransitionStingerClipDuration[mE]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemTransitionStingerTriggerPoint[mE];
					#endif
					atemTransitionStingerTriggerPoint[mE] = word(_packetBuffer[14], _packetBuffer[15]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemTransitionStingerTriggerPoint[mE]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemTransitionStingerTriggerPoint[mE=")); Serial.print(mE); Serial.print(F("] = "));
						Serial.println(atemTransitionStingerTriggerPoint[mE]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemTransitionStingerMixRate[mE];
					#endif
					atemTransitionStingerMixRate[mE] = word(_packetBuffer[16], _packetBuffer[17]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemTransitionStingerMixRate[mE]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemTransitionStingerMixRate[mE=")); Serial.print(mE); Serial.print(F("] = "));
						Serial.println(atemTransitionStingerMixRate[mE]);
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
			if(!strcmp_P(cmdStr, PSTR("KeBP"))) {
				
				mE = _packetBuffer[0];
				keyer = _packetBuffer[1];
				if (mE<=1 && keyer<=3) {
					#if ATEM_debug
					temp = atemKeyerType[mE][keyer];
					#endif
					atemKeyerType[mE][keyer] = _packetBuffer[2];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyerType[mE][keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyerType[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("] = "));
						Serial.println(atemKeyerType[mE][keyer]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyerFlyEnabled[mE][keyer];
					#endif
					atemKeyerFlyEnabled[mE][keyer] = _packetBuffer[5];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyerFlyEnabled[mE][keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyerFlyEnabled[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("] = "));
						Serial.println(atemKeyerFlyEnabled[mE][keyer]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyerFillSource[mE][keyer];
					#endif
					atemKeyerFillSource[mE][keyer] = word(_packetBuffer[6], _packetBuffer[7]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyerFillSource[mE][keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyerFillSource[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("] = "));
						Serial.println(atemKeyerFillSource[mE][keyer]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyerKeySource[mE][keyer];
					#endif
					atemKeyerKeySource[mE][keyer] = word(_packetBuffer[8], _packetBuffer[9]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyerKeySource[mE][keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyerKeySource[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("] = "));
						Serial.println(atemKeyerKeySource[mE][keyer]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyerMasked[mE][keyer];
					#endif
					atemKeyerMasked[mE][keyer] = _packetBuffer[10];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyerMasked[mE][keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyerMasked[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("] = "));
						Serial.println(atemKeyerMasked[mE][keyer]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyerTop[mE][keyer];
					#endif
					atemKeyerTop[mE][keyer] = (int16_t) word(_packetBuffer[12], _packetBuffer[13]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyerTop[mE][keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyerTop[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("] = "));
						Serial.println(atemKeyerTop[mE][keyer]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyerBottom[mE][keyer];
					#endif
					atemKeyerBottom[mE][keyer] = (int16_t) word(_packetBuffer[14], _packetBuffer[15]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyerBottom[mE][keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyerBottom[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("] = "));
						Serial.println(atemKeyerBottom[mE][keyer]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyerLeft[mE][keyer];
					#endif
					atemKeyerLeft[mE][keyer] = (int16_t) word(_packetBuffer[16], _packetBuffer[17]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyerLeft[mE][keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyerLeft[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("] = "));
						Serial.println(atemKeyerLeft[mE][keyer]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyerRight[mE][keyer];
					#endif
					atemKeyerRight[mE][keyer] = (int16_t) word(_packetBuffer[18], _packetBuffer[19]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyerRight[mE][keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyerRight[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("] = "));
						Serial.println(atemKeyerRight[mE][keyer]);
					}
					#endif
					
				}
			} else 
			if(!strcmp_P(cmdStr, PSTR("KeLm"))) {
				
				mE = _packetBuffer[0];
				keyer = _packetBuffer[1];
				if (mE<=1 && keyer<=3) {
					#if ATEM_debug
					temp = atemKeyLumaPreMultiplied[mE][keyer];
					#endif
					atemKeyLumaPreMultiplied[mE][keyer] = _packetBuffer[2];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyLumaPreMultiplied[mE][keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyLumaPreMultiplied[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("] = "));
						Serial.println(atemKeyLumaPreMultiplied[mE][keyer]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyLumaClip[mE][keyer];
					#endif
					atemKeyLumaClip[mE][keyer] = word(_packetBuffer[4], _packetBuffer[5]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyLumaClip[mE][keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyLumaClip[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("] = "));
						Serial.println(atemKeyLumaClip[mE][keyer]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyLumaGain[mE][keyer];
					#endif
					atemKeyLumaGain[mE][keyer] = word(_packetBuffer[6], _packetBuffer[7]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyLumaGain[mE][keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyLumaGain[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("] = "));
						Serial.println(atemKeyLumaGain[mE][keyer]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyLumaInvertKey[mE][keyer];
					#endif
					atemKeyLumaInvertKey[mE][keyer] = _packetBuffer[8];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyLumaInvertKey[mE][keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyLumaInvertKey[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("] = "));
						Serial.println(atemKeyLumaInvertKey[mE][keyer]);
					}
					#endif
					
				}
			} else 
			if(!strcmp_P(cmdStr, PSTR("KeCk"))) {
				
				mE = _packetBuffer[0];
				keyer = _packetBuffer[1];
				if (mE<=1 && keyer<=3) {
					#if ATEM_debug
					temp = atemKeyChromaHue[mE][keyer];
					#endif
					atemKeyChromaHue[mE][keyer] = word(_packetBuffer[2], _packetBuffer[3]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyChromaHue[mE][keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyChromaHue[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("] = "));
						Serial.println(atemKeyChromaHue[mE][keyer]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyChromaGain[mE][keyer];
					#endif
					atemKeyChromaGain[mE][keyer] = word(_packetBuffer[4], _packetBuffer[5]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyChromaGain[mE][keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyChromaGain[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("] = "));
						Serial.println(atemKeyChromaGain[mE][keyer]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyChromaYSuppress[mE][keyer];
					#endif
					atemKeyChromaYSuppress[mE][keyer] = word(_packetBuffer[6], _packetBuffer[7]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyChromaYSuppress[mE][keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyChromaYSuppress[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("] = "));
						Serial.println(atemKeyChromaYSuppress[mE][keyer]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyChromaLift[mE][keyer];
					#endif
					atemKeyChromaLift[mE][keyer] = word(_packetBuffer[8], _packetBuffer[9]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyChromaLift[mE][keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyChromaLift[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("] = "));
						Serial.println(atemKeyChromaLift[mE][keyer]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyChromaNarrow[mE][keyer];
					#endif
					atemKeyChromaNarrow[mE][keyer] = _packetBuffer[10];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyChromaNarrow[mE][keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyChromaNarrow[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("] = "));
						Serial.println(atemKeyChromaNarrow[mE][keyer]);
					}
					#endif
					
				}
			} else 
			if(!strcmp_P(cmdStr, PSTR("KePt"))) {
				
				mE = _packetBuffer[0];
				keyer = _packetBuffer[1];
				if (mE<=1 && keyer<=3) {
					#if ATEM_debug
					temp = atemKeyPatternPattern[mE][keyer];
					#endif
					atemKeyPatternPattern[mE][keyer] = _packetBuffer[2];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyPatternPattern[mE][keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyPatternPattern[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("] = "));
						Serial.println(atemKeyPatternPattern[mE][keyer]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyPatternSize[mE][keyer];
					#endif
					atemKeyPatternSize[mE][keyer] = word(_packetBuffer[4], _packetBuffer[5]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyPatternSize[mE][keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyPatternSize[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("] = "));
						Serial.println(atemKeyPatternSize[mE][keyer]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyPatternSymmetry[mE][keyer];
					#endif
					atemKeyPatternSymmetry[mE][keyer] = word(_packetBuffer[6], _packetBuffer[7]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyPatternSymmetry[mE][keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyPatternSymmetry[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("] = "));
						Serial.println(atemKeyPatternSymmetry[mE][keyer]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyPatternSoftness[mE][keyer];
					#endif
					atemKeyPatternSoftness[mE][keyer] = word(_packetBuffer[8], _packetBuffer[9]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyPatternSoftness[mE][keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyPatternSoftness[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("] = "));
						Serial.println(atemKeyPatternSoftness[mE][keyer]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyPatternPositionX[mE][keyer];
					#endif
					atemKeyPatternPositionX[mE][keyer] = word(_packetBuffer[10], _packetBuffer[11]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyPatternPositionX[mE][keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyPatternPositionX[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("] = "));
						Serial.println(atemKeyPatternPositionX[mE][keyer]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyPatternPositionY[mE][keyer];
					#endif
					atemKeyPatternPositionY[mE][keyer] = word(_packetBuffer[12], _packetBuffer[13]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyPatternPositionY[mE][keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyPatternPositionY[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("] = "));
						Serial.println(atemKeyPatternPositionY[mE][keyer]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyPatternInvertPattern[mE][keyer];
					#endif
					atemKeyPatternInvertPattern[mE][keyer] = _packetBuffer[14];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyPatternInvertPattern[mE][keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyPatternInvertPattern[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("] = "));
						Serial.println(atemKeyPatternInvertPattern[mE][keyer]);
					}
					#endif
					
				}
			} else 
			if(!strcmp_P(cmdStr, PSTR("KeDV"))) {
				
				mE = _packetBuffer[0];
				keyer = _packetBuffer[1];
				if (mE<=1 && keyer<=3) {
					#if ATEM_debug
					temp = atemKeyDVESizeX[mE][keyer];
					#endif
					atemKeyDVESizeX[mE][keyer] = (uint32_t)_packetBuffer[4]<<24 | (uint32_t)_packetBuffer[5]<<16 | (uint32_t)_packetBuffer[6]<<8 | (uint32_t)_packetBuffer[7];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyDVESizeX[mE][keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyDVESizeX[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("] = "));
						Serial.println(atemKeyDVESizeX[mE][keyer]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyDVESizeY[mE][keyer];
					#endif
					atemKeyDVESizeY[mE][keyer] = (uint32_t)_packetBuffer[8]<<24 | (uint32_t)_packetBuffer[9]<<16 | (uint32_t)_packetBuffer[10]<<8 | (uint32_t)_packetBuffer[11];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyDVESizeY[mE][keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyDVESizeY[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("] = "));
						Serial.println(atemKeyDVESizeY[mE][keyer]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyDVEPositionX[mE][keyer];
					#endif
					atemKeyDVEPositionX[mE][keyer] = (uint32_t)_packetBuffer[12]<<24 | (uint32_t)_packetBuffer[13]<<16 | (uint32_t)_packetBuffer[14]<<8 | (uint32_t)_packetBuffer[15];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyDVEPositionX[mE][keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyDVEPositionX[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("] = "));
						Serial.println(atemKeyDVEPositionX[mE][keyer]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyDVEPositionY[mE][keyer];
					#endif
					atemKeyDVEPositionY[mE][keyer] = (uint32_t)_packetBuffer[16]<<24 | (uint32_t)_packetBuffer[17]<<16 | (uint32_t)_packetBuffer[18]<<8 | (uint32_t)_packetBuffer[19];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyDVEPositionY[mE][keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyDVEPositionY[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("] = "));
						Serial.println(atemKeyDVEPositionY[mE][keyer]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyDVERotation[mE][keyer];
					#endif
					atemKeyDVERotation[mE][keyer] = (uint32_t)_packetBuffer[20]<<24 | (uint32_t)_packetBuffer[21]<<16 | (uint32_t)_packetBuffer[22]<<8 | (uint32_t)_packetBuffer[23];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyDVERotation[mE][keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyDVERotation[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("] = "));
						Serial.println(atemKeyDVERotation[mE][keyer]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyDVEBorderEnabled[mE][keyer];
					#endif
					atemKeyDVEBorderEnabled[mE][keyer] = _packetBuffer[24];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyDVEBorderEnabled[mE][keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyDVEBorderEnabled[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("] = "));
						Serial.println(atemKeyDVEBorderEnabled[mE][keyer]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyDVEShadow[mE][keyer];
					#endif
					atemKeyDVEShadow[mE][keyer] = _packetBuffer[25];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyDVEShadow[mE][keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyDVEShadow[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("] = "));
						Serial.println(atemKeyDVEShadow[mE][keyer]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyDVEBorderBevel[mE][keyer];
					#endif
					atemKeyDVEBorderBevel[mE][keyer] = _packetBuffer[26];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyDVEBorderBevel[mE][keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyDVEBorderBevel[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("] = "));
						Serial.println(atemKeyDVEBorderBevel[mE][keyer]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyDVEBorderOuterWidth[mE][keyer];
					#endif
					atemKeyDVEBorderOuterWidth[mE][keyer] = word(_packetBuffer[28], _packetBuffer[29]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyDVEBorderOuterWidth[mE][keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyDVEBorderOuterWidth[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("] = "));
						Serial.println(atemKeyDVEBorderOuterWidth[mE][keyer]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyDVEBorderInnerWidth[mE][keyer];
					#endif
					atemKeyDVEBorderInnerWidth[mE][keyer] = word(_packetBuffer[30], _packetBuffer[31]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyDVEBorderInnerWidth[mE][keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyDVEBorderInnerWidth[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("] = "));
						Serial.println(atemKeyDVEBorderInnerWidth[mE][keyer]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyDVEBorderOuterSoftness[mE][keyer];
					#endif
					atemKeyDVEBorderOuterSoftness[mE][keyer] = _packetBuffer[32];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyDVEBorderOuterSoftness[mE][keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyDVEBorderOuterSoftness[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("] = "));
						Serial.println(atemKeyDVEBorderOuterSoftness[mE][keyer]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyDVEBorderInnerSoftness[mE][keyer];
					#endif
					atemKeyDVEBorderInnerSoftness[mE][keyer] = _packetBuffer[33];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyDVEBorderInnerSoftness[mE][keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyDVEBorderInnerSoftness[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("] = "));
						Serial.println(atemKeyDVEBorderInnerSoftness[mE][keyer]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyDVEBorderBevelSoftness[mE][keyer];
					#endif
					atemKeyDVEBorderBevelSoftness[mE][keyer] = _packetBuffer[34];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyDVEBorderBevelSoftness[mE][keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyDVEBorderBevelSoftness[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("] = "));
						Serial.println(atemKeyDVEBorderBevelSoftness[mE][keyer]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyDVEBorderBevelPosition[mE][keyer];
					#endif
					atemKeyDVEBorderBevelPosition[mE][keyer] = _packetBuffer[35];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyDVEBorderBevelPosition[mE][keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyDVEBorderBevelPosition[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("] = "));
						Serial.println(atemKeyDVEBorderBevelPosition[mE][keyer]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyDVEBorderOpacity[mE][keyer];
					#endif
					atemKeyDVEBorderOpacity[mE][keyer] = _packetBuffer[36];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyDVEBorderOpacity[mE][keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyDVEBorderOpacity[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("] = "));
						Serial.println(atemKeyDVEBorderOpacity[mE][keyer]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyDVEBorderHue[mE][keyer];
					#endif
					atemKeyDVEBorderHue[mE][keyer] = word(_packetBuffer[38], _packetBuffer[39]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyDVEBorderHue[mE][keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyDVEBorderHue[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("] = "));
						Serial.println(atemKeyDVEBorderHue[mE][keyer]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyDVEBorderSaturation[mE][keyer];
					#endif
					atemKeyDVEBorderSaturation[mE][keyer] = word(_packetBuffer[40], _packetBuffer[41]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyDVEBorderSaturation[mE][keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyDVEBorderSaturation[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("] = "));
						Serial.println(atemKeyDVEBorderSaturation[mE][keyer]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyDVEBorderLuma[mE][keyer];
					#endif
					atemKeyDVEBorderLuma[mE][keyer] = word(_packetBuffer[42], _packetBuffer[43]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyDVEBorderLuma[mE][keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyDVEBorderLuma[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("] = "));
						Serial.println(atemKeyDVEBorderLuma[mE][keyer]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyDVELightSourceDirection[mE][keyer];
					#endif
					atemKeyDVELightSourceDirection[mE][keyer] = word(_packetBuffer[44], _packetBuffer[45]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyDVELightSourceDirection[mE][keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyDVELightSourceDirection[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("] = "));
						Serial.println(atemKeyDVELightSourceDirection[mE][keyer]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyDVELightSourceAltitude[mE][keyer];
					#endif
					atemKeyDVELightSourceAltitude[mE][keyer] = _packetBuffer[46];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyDVELightSourceAltitude[mE][keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyDVELightSourceAltitude[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("] = "));
						Serial.println(atemKeyDVELightSourceAltitude[mE][keyer]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyDVEMasked[mE][keyer];
					#endif
					atemKeyDVEMasked[mE][keyer] = _packetBuffer[47];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyDVEMasked[mE][keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyDVEMasked[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("] = "));
						Serial.println(atemKeyDVEMasked[mE][keyer]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyDVETop[mE][keyer];
					#endif
					atemKeyDVETop[mE][keyer] = (int16_t) word(_packetBuffer[48], _packetBuffer[49]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyDVETop[mE][keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyDVETop[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("] = "));
						Serial.println(atemKeyDVETop[mE][keyer]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyDVEBottom[mE][keyer];
					#endif
					atemKeyDVEBottom[mE][keyer] = (int16_t) word(_packetBuffer[50], _packetBuffer[51]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyDVEBottom[mE][keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyDVEBottom[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("] = "));
						Serial.println(atemKeyDVEBottom[mE][keyer]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyDVELeft[mE][keyer];
					#endif
					atemKeyDVELeft[mE][keyer] = (int16_t) word(_packetBuffer[52], _packetBuffer[53]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyDVELeft[mE][keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyDVELeft[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("] = "));
						Serial.println(atemKeyDVELeft[mE][keyer]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyDVERight[mE][keyer];
					#endif
					atemKeyDVERight[mE][keyer] = (int16_t) word(_packetBuffer[54], _packetBuffer[55]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyDVERight[mE][keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyDVERight[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("] = "));
						Serial.println(atemKeyDVERight[mE][keyer]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyDVERate[mE][keyer];
					#endif
					atemKeyDVERate[mE][keyer] = _packetBuffer[56];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyDVERate[mE][keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyDVERate[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("] = "));
						Serial.println(atemKeyDVERate[mE][keyer]);
					}
					#endif
					
				}
			} else 
			if(!strcmp_P(cmdStr, PSTR("KeFS"))) {
				
				mE = _packetBuffer[0];
				keyer = _packetBuffer[1];
				if (mE<=1 && keyer<=3) {
					#if ATEM_debug
					temp = atemKeyerFlyIsASet[mE][keyer];
					#endif
					atemKeyerFlyIsASet[mE][keyer] = _packetBuffer[2];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyerFlyIsASet[mE][keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyerFlyIsASet[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("] = "));
						Serial.println(atemKeyerFlyIsASet[mE][keyer]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyerFlyIsBSet[mE][keyer];
					#endif
					atemKeyerFlyIsBSet[mE][keyer] = _packetBuffer[3];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyerFlyIsBSet[mE][keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyerFlyIsBSet[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("] = "));
						Serial.println(atemKeyerFlyIsBSet[mE][keyer]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyerFlyIsAtKeyFrame[mE][keyer];
					#endif
					atemKeyerFlyIsAtKeyFrame[mE][keyer] = _packetBuffer[6];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyerFlyIsAtKeyFrame[mE][keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyerFlyIsAtKeyFrame[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("] = "));
						Serial.println(atemKeyerFlyIsAtKeyFrame[mE][keyer]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyerFlyRuntoInfiniteindex[mE][keyer];
					#endif
					atemKeyerFlyRuntoInfiniteindex[mE][keyer] = _packetBuffer[7];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyerFlyRuntoInfiniteindex[mE][keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyerFlyRuntoInfiniteindex[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("] = "));
						Serial.println(atemKeyerFlyRuntoInfiniteindex[mE][keyer]);
					}
					#endif
					
				}
			} else 
			if(!strcmp_P(cmdStr, PSTR("KKFP"))) {
				
				mE = _packetBuffer[0];
				keyer = _packetBuffer[1];
				keyFrame = _packetBuffer[2];
				if (mE<=1 && keyer<=3 && keyFrame<=2) {
					#if ATEM_debug
					temp = atemKeyerFlyKeyFrameSizeX[mE][keyer][keyFrame];
					#endif
					atemKeyerFlyKeyFrameSizeX[mE][keyer][keyFrame] = (uint32_t)_packetBuffer[4]<<24 | (uint32_t)_packetBuffer[5]<<16 | (uint32_t)_packetBuffer[6]<<8 | (uint32_t)_packetBuffer[7];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyerFlyKeyFrameSizeX[mE][keyer][keyFrame]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyerFlyKeyFrameSizeX[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("][keyFrame=")); Serial.print(keyFrame); Serial.print(F("] = "));
						Serial.println(atemKeyerFlyKeyFrameSizeX[mE][keyer][keyFrame]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyerFlyKeyFrameSizeY[mE][keyer][keyFrame];
					#endif
					atemKeyerFlyKeyFrameSizeY[mE][keyer][keyFrame] = (uint32_t)_packetBuffer[8]<<24 | (uint32_t)_packetBuffer[9]<<16 | (uint32_t)_packetBuffer[10]<<8 | (uint32_t)_packetBuffer[11];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyerFlyKeyFrameSizeY[mE][keyer][keyFrame]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyerFlyKeyFrameSizeY[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("][keyFrame=")); Serial.print(keyFrame); Serial.print(F("] = "));
						Serial.println(atemKeyerFlyKeyFrameSizeY[mE][keyer][keyFrame]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyerFlyKeyFramePositionX[mE][keyer][keyFrame];
					#endif
					atemKeyerFlyKeyFramePositionX[mE][keyer][keyFrame] = (uint32_t)_packetBuffer[12]<<24 | (uint32_t)_packetBuffer[13]<<16 | (uint32_t)_packetBuffer[14]<<8 | (uint32_t)_packetBuffer[15];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyerFlyKeyFramePositionX[mE][keyer][keyFrame]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyerFlyKeyFramePositionX[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("][keyFrame=")); Serial.print(keyFrame); Serial.print(F("] = "));
						Serial.println(atemKeyerFlyKeyFramePositionX[mE][keyer][keyFrame]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyerFlyKeyFramePositionY[mE][keyer][keyFrame];
					#endif
					atemKeyerFlyKeyFramePositionY[mE][keyer][keyFrame] = (uint32_t)_packetBuffer[16]<<24 | (uint32_t)_packetBuffer[17]<<16 | (uint32_t)_packetBuffer[18]<<8 | (uint32_t)_packetBuffer[19];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyerFlyKeyFramePositionY[mE][keyer][keyFrame]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyerFlyKeyFramePositionY[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("][keyFrame=")); Serial.print(keyFrame); Serial.print(F("] = "));
						Serial.println(atemKeyerFlyKeyFramePositionY[mE][keyer][keyFrame]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyerFlyKeyFrameRotation[mE][keyer][keyFrame];
					#endif
					atemKeyerFlyKeyFrameRotation[mE][keyer][keyFrame] = (uint32_t)_packetBuffer[20]<<24 | (uint32_t)_packetBuffer[21]<<16 | (uint32_t)_packetBuffer[22]<<8 | (uint32_t)_packetBuffer[23];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyerFlyKeyFrameRotation[mE][keyer][keyFrame]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyerFlyKeyFrameRotation[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("][keyFrame=")); Serial.print(keyFrame); Serial.print(F("] = "));
						Serial.println(atemKeyerFlyKeyFrameRotation[mE][keyer][keyFrame]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyerFlyKeyFrameBorderOuterWidth[mE][keyer][keyFrame];
					#endif
					atemKeyerFlyKeyFrameBorderOuterWidth[mE][keyer][keyFrame] = word(_packetBuffer[24], _packetBuffer[25]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyerFlyKeyFrameBorderOuterWidth[mE][keyer][keyFrame]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyerFlyKeyFrameBorderOuterWidth[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("][keyFrame=")); Serial.print(keyFrame); Serial.print(F("] = "));
						Serial.println(atemKeyerFlyKeyFrameBorderOuterWidth[mE][keyer][keyFrame]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyerFlyKeyFrameBorderInnerWidth[mE][keyer][keyFrame];
					#endif
					atemKeyerFlyKeyFrameBorderInnerWidth[mE][keyer][keyFrame] = word(_packetBuffer[26], _packetBuffer[27]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyerFlyKeyFrameBorderInnerWidth[mE][keyer][keyFrame]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyerFlyKeyFrameBorderInnerWidth[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("][keyFrame=")); Serial.print(keyFrame); Serial.print(F("] = "));
						Serial.println(atemKeyerFlyKeyFrameBorderInnerWidth[mE][keyer][keyFrame]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyerFlyKeyFrameBorderOuterSoftness[mE][keyer][keyFrame];
					#endif
					atemKeyerFlyKeyFrameBorderOuterSoftness[mE][keyer][keyFrame] = _packetBuffer[28];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyerFlyKeyFrameBorderOuterSoftness[mE][keyer][keyFrame]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyerFlyKeyFrameBorderOuterSoftness[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("][keyFrame=")); Serial.print(keyFrame); Serial.print(F("] = "));
						Serial.println(atemKeyerFlyKeyFrameBorderOuterSoftness[mE][keyer][keyFrame]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyerFlyKeyFrameBorderInnerSoftness[mE][keyer][keyFrame];
					#endif
					atemKeyerFlyKeyFrameBorderInnerSoftness[mE][keyer][keyFrame] = _packetBuffer[29];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyerFlyKeyFrameBorderInnerSoftness[mE][keyer][keyFrame]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyerFlyKeyFrameBorderInnerSoftness[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("][keyFrame=")); Serial.print(keyFrame); Serial.print(F("] = "));
						Serial.println(atemKeyerFlyKeyFrameBorderInnerSoftness[mE][keyer][keyFrame]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyerFlyKeyFrameBorderBevelSoftness[mE][keyer][keyFrame];
					#endif
					atemKeyerFlyKeyFrameBorderBevelSoftness[mE][keyer][keyFrame] = _packetBuffer[30];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyerFlyKeyFrameBorderBevelSoftness[mE][keyer][keyFrame]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyerFlyKeyFrameBorderBevelSoftness[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("][keyFrame=")); Serial.print(keyFrame); Serial.print(F("] = "));
						Serial.println(atemKeyerFlyKeyFrameBorderBevelSoftness[mE][keyer][keyFrame]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyerFlyKeyFrameBorderBevelPosition[mE][keyer][keyFrame];
					#endif
					atemKeyerFlyKeyFrameBorderBevelPosition[mE][keyer][keyFrame] = _packetBuffer[31];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyerFlyKeyFrameBorderBevelPosition[mE][keyer][keyFrame]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyerFlyKeyFrameBorderBevelPosition[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("][keyFrame=")); Serial.print(keyFrame); Serial.print(F("] = "));
						Serial.println(atemKeyerFlyKeyFrameBorderBevelPosition[mE][keyer][keyFrame]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyerFlyKeyFrameBorderOpacity[mE][keyer][keyFrame];
					#endif
					atemKeyerFlyKeyFrameBorderOpacity[mE][keyer][keyFrame] = _packetBuffer[32];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyerFlyKeyFrameBorderOpacity[mE][keyer][keyFrame]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyerFlyKeyFrameBorderOpacity[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("][keyFrame=")); Serial.print(keyFrame); Serial.print(F("] = "));
						Serial.println(atemKeyerFlyKeyFrameBorderOpacity[mE][keyer][keyFrame]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyerFlyKeyFrameBorderHue[mE][keyer][keyFrame];
					#endif
					atemKeyerFlyKeyFrameBorderHue[mE][keyer][keyFrame] = word(_packetBuffer[34], _packetBuffer[35]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyerFlyKeyFrameBorderHue[mE][keyer][keyFrame]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyerFlyKeyFrameBorderHue[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("][keyFrame=")); Serial.print(keyFrame); Serial.print(F("] = "));
						Serial.println(atemKeyerFlyKeyFrameBorderHue[mE][keyer][keyFrame]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyerFlyKeyFrameBorderSaturation[mE][keyer][keyFrame];
					#endif
					atemKeyerFlyKeyFrameBorderSaturation[mE][keyer][keyFrame] = word(_packetBuffer[36], _packetBuffer[37]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyerFlyKeyFrameBorderSaturation[mE][keyer][keyFrame]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyerFlyKeyFrameBorderSaturation[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("][keyFrame=")); Serial.print(keyFrame); Serial.print(F("] = "));
						Serial.println(atemKeyerFlyKeyFrameBorderSaturation[mE][keyer][keyFrame]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyerFlyKeyFrameBorderLuma[mE][keyer][keyFrame];
					#endif
					atemKeyerFlyKeyFrameBorderLuma[mE][keyer][keyFrame] = word(_packetBuffer[38], _packetBuffer[39]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyerFlyKeyFrameBorderLuma[mE][keyer][keyFrame]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyerFlyKeyFrameBorderLuma[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("][keyFrame=")); Serial.print(keyFrame); Serial.print(F("] = "));
						Serial.println(atemKeyerFlyKeyFrameBorderLuma[mE][keyer][keyFrame]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyerFlyKeyFrameLightSourceDirection[mE][keyer][keyFrame];
					#endif
					atemKeyerFlyKeyFrameLightSourceDirection[mE][keyer][keyFrame] = word(_packetBuffer[40], _packetBuffer[41]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyerFlyKeyFrameLightSourceDirection[mE][keyer][keyFrame]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyerFlyKeyFrameLightSourceDirection[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("][keyFrame=")); Serial.print(keyFrame); Serial.print(F("] = "));
						Serial.println(atemKeyerFlyKeyFrameLightSourceDirection[mE][keyer][keyFrame]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyerFlyKeyFrameLightSourceAltitude[mE][keyer][keyFrame];
					#endif
					atemKeyerFlyKeyFrameLightSourceAltitude[mE][keyer][keyFrame] = _packetBuffer[42];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyerFlyKeyFrameLightSourceAltitude[mE][keyer][keyFrame]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyerFlyKeyFrameLightSourceAltitude[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("][keyFrame=")); Serial.print(keyFrame); Serial.print(F("] = "));
						Serial.println(atemKeyerFlyKeyFrameLightSourceAltitude[mE][keyer][keyFrame]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyerFlyKeyFrameTop[mE][keyer][keyFrame];
					#endif
					atemKeyerFlyKeyFrameTop[mE][keyer][keyFrame] = (int16_t) word(_packetBuffer[44], _packetBuffer[45]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyerFlyKeyFrameTop[mE][keyer][keyFrame]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyerFlyKeyFrameTop[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("][keyFrame=")); Serial.print(keyFrame); Serial.print(F("] = "));
						Serial.println(atemKeyerFlyKeyFrameTop[mE][keyer][keyFrame]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyerFlyKeyFrameBottom[mE][keyer][keyFrame];
					#endif
					atemKeyerFlyKeyFrameBottom[mE][keyer][keyFrame] = (int16_t) word(_packetBuffer[46], _packetBuffer[47]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyerFlyKeyFrameBottom[mE][keyer][keyFrame]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyerFlyKeyFrameBottom[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("][keyFrame=")); Serial.print(keyFrame); Serial.print(F("] = "));
						Serial.println(atemKeyerFlyKeyFrameBottom[mE][keyer][keyFrame]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyerFlyKeyFrameLeft[mE][keyer][keyFrame];
					#endif
					atemKeyerFlyKeyFrameLeft[mE][keyer][keyFrame] = (int16_t) word(_packetBuffer[48], _packetBuffer[49]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyerFlyKeyFrameLeft[mE][keyer][keyFrame]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyerFlyKeyFrameLeft[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("][keyFrame=")); Serial.print(keyFrame); Serial.print(F("] = "));
						Serial.println(atemKeyerFlyKeyFrameLeft[mE][keyer][keyFrame]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyerFlyKeyFrameRight[mE][keyer][keyFrame];
					#endif
					atemKeyerFlyKeyFrameRight[mE][keyer][keyFrame] = (int16_t) word(_packetBuffer[50], _packetBuffer[51]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyerFlyKeyFrameRight[mE][keyer][keyFrame]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyerFlyKeyFrameRight[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("][keyFrame=")); Serial.print(keyFrame); Serial.print(F("] = "));
						Serial.println(atemKeyerFlyKeyFrameRight[mE][keyer][keyFrame]);
					}
					#endif
					
				}
			} else 
			if(!strcmp_P(cmdStr, PSTR("DskB"))) {
				
				keyer = _packetBuffer[0];
				if (keyer<=1) {
					#if ATEM_debug
					temp = atemDownstreamKeyerFillSource[keyer];
					#endif
					atemDownstreamKeyerFillSource[keyer] = word(_packetBuffer[2], _packetBuffer[3]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemDownstreamKeyerFillSource[keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemDownstreamKeyerFillSource[keyer=")); Serial.print(keyer); Serial.print(F("] = "));
						Serial.println(atemDownstreamKeyerFillSource[keyer]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemDownstreamKeyerKeySource[keyer];
					#endif
					atemDownstreamKeyerKeySource[keyer] = word(_packetBuffer[4], _packetBuffer[5]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemDownstreamKeyerKeySource[keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemDownstreamKeyerKeySource[keyer=")); Serial.print(keyer); Serial.print(F("] = "));
						Serial.println(atemDownstreamKeyerKeySource[keyer]);
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
			if(!strcmp_P(cmdStr, PSTR("ColV"))) {
				
				colorGenerator = _packetBuffer[0];
				if (colorGenerator<=1) {
					#if ATEM_debug
					temp = atemColorGeneratorHue[colorGenerator];
					#endif
					atemColorGeneratorHue[colorGenerator] = word(_packetBuffer[2], _packetBuffer[3]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemColorGeneratorHue[colorGenerator]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemColorGeneratorHue[colorGenerator=")); Serial.print(colorGenerator); Serial.print(F("] = "));
						Serial.println(atemColorGeneratorHue[colorGenerator]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemColorGeneratorSaturation[colorGenerator];
					#endif
					atemColorGeneratorSaturation[colorGenerator] = word(_packetBuffer[4], _packetBuffer[5]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemColorGeneratorSaturation[colorGenerator]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemColorGeneratorSaturation[colorGenerator=")); Serial.print(colorGenerator); Serial.print(F("] = "));
						Serial.println(atemColorGeneratorSaturation[colorGenerator]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemColorGeneratorLuma[colorGenerator];
					#endif
					atemColorGeneratorLuma[colorGenerator] = word(_packetBuffer[6], _packetBuffer[7]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemColorGeneratorLuma[colorGenerator]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemColorGeneratorLuma[colorGenerator=")); Serial.print(colorGenerator); Serial.print(F("] = "));
						Serial.println(atemColorGeneratorLuma[colorGenerator]);
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
			if(!strcmp_P(cmdStr, PSTR("CCdP"))) {
				
				input = _packetBuffer[0];
				if (input<=20) {
				if (_packetBuffer[1]==0 && _packetBuffer[2]==3)	{
					
					#if ATEM_debug
					temp = atemCameraControlIris[input];
					#endif
					atemCameraControlIris[input] = (int16_t) word(_packetBuffer[16], _packetBuffer[17]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemCameraControlIris[input]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemCameraControlIris[input=")); Serial.print(input); Serial.print(F("] = "));
						Serial.println(atemCameraControlIris[input]);
					}
					#endif
					
				}
				
				if (_packetBuffer[1]==0 && _packetBuffer[2]==0)	{
					
					#if ATEM_debug
					temp = atemCameraControlFocus[input];
					#endif
					atemCameraControlFocus[input] = (int16_t) word(_packetBuffer[16], _packetBuffer[17]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemCameraControlFocus[input]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemCameraControlFocus[input=")); Serial.print(input); Serial.print(F("] = "));
						Serial.println(atemCameraControlFocus[input]);
					}
					#endif
					
				}
				
				if (_packetBuffer[1]==1 && _packetBuffer[2]==1)	{
					
					#if ATEM_debug
					temp = atemCameraControlGain[input];
					#endif
					atemCameraControlGain[input] = (int16_t) word(_packetBuffer[16], _packetBuffer[17]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemCameraControlGain[input]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemCameraControlGain[input=")); Serial.print(input); Serial.print(F("] = "));
						Serial.println(atemCameraControlGain[input]);
					}
					#endif
					
				}
				
				if (_packetBuffer[1]==1 && _packetBuffer[2]==2)	{
					
					#if ATEM_debug
					temp = atemCameraControlWhiteBalance[input];
					#endif
					atemCameraControlWhiteBalance[input] = (int16_t) word(_packetBuffer[16], _packetBuffer[17]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemCameraControlWhiteBalance[input]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemCameraControlWhiteBalance[input=")); Serial.print(input); Serial.print(F("] = "));
						Serial.println(atemCameraControlWhiteBalance[input]);
					}
					#endif
					
				}
				
				if (_packetBuffer[1]==1 && _packetBuffer[2]==8)	{
					
					#if ATEM_debug
					temp = atemCameraControlSharpeningLevel[input];
					#endif
					atemCameraControlSharpeningLevel[input] = (int16_t) word(_packetBuffer[16], _packetBuffer[17]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemCameraControlSharpeningLevel[input]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemCameraControlSharpeningLevel[input=")); Serial.print(input); Serial.print(F("] = "));
						Serial.println(atemCameraControlSharpeningLevel[input]);
					}
					#endif
					
				}
				
				if (_packetBuffer[1]==0 && _packetBuffer[2]==8)	{
					
					#if ATEM_debug
					temp = atemCameraControlZoomNormalized[input];
					#endif
					atemCameraControlZoomNormalized[input] = (int16_t) word(_packetBuffer[16], _packetBuffer[17]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemCameraControlZoomNormalized[input]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemCameraControlZoomNormalized[input=")); Serial.print(input); Serial.print(F("] = "));
						Serial.println(atemCameraControlZoomNormalized[input]);
					}
					#endif
					
				}
				
				if (_packetBuffer[1]==0 && _packetBuffer[2]==9)	{
					
					#if ATEM_debug
					temp = atemCameraControlZoomSpeed[input];
					#endif
					atemCameraControlZoomSpeed[input] = (int16_t) word(_packetBuffer[16], _packetBuffer[17]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemCameraControlZoomSpeed[input]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemCameraControlZoomSpeed[input=")); Serial.print(input); Serial.print(F("] = "));
						Serial.println(atemCameraControlZoomSpeed[input]);
					}
					#endif
					
				}
				
				if (_packetBuffer[1]==4 && _packetBuffer[2]==4)	{
					
					#if ATEM_debug
					temp = atemCameraControlColorbars[input];
					#endif
					atemCameraControlColorbars[input] = (int16_t) word(_packetBuffer[16], _packetBuffer[17]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemCameraControlColorbars[input]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemCameraControlColorbars[input=")); Serial.print(input); Serial.print(F("] = "));
						Serial.println(atemCameraControlColorbars[input]);
					}
					#endif
					
				}
				
				if (_packetBuffer[1]==8 && _packetBuffer[2]==0)	{
					
					#if ATEM_debug
					temp = atemCameraControlLiftR[input];
					#endif
					atemCameraControlLiftR[input] = (int16_t) word(_packetBuffer[16], _packetBuffer[17]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemCameraControlLiftR[input]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemCameraControlLiftR[input=")); Serial.print(input); Serial.print(F("] = "));
						Serial.println(atemCameraControlLiftR[input]);
					}
					#endif
					
				}
				
				if (_packetBuffer[1]==8 && _packetBuffer[2]==1)	{
					
					#if ATEM_debug
					temp = atemCameraControlGammaR[input];
					#endif
					atemCameraControlGammaR[input] = (int16_t) word(_packetBuffer[16], _packetBuffer[17]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemCameraControlGammaR[input]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemCameraControlGammaR[input=")); Serial.print(input); Serial.print(F("] = "));
						Serial.println(atemCameraControlGammaR[input]);
					}
					#endif
					
				}
				
				if (_packetBuffer[1]==8 && _packetBuffer[2]==2)	{
					
					#if ATEM_debug
					temp = atemCameraControlGainR[input];
					#endif
					atemCameraControlGainR[input] = (int16_t) word(_packetBuffer[16], _packetBuffer[17]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemCameraControlGainR[input]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemCameraControlGainR[input=")); Serial.print(input); Serial.print(F("] = "));
						Serial.println(atemCameraControlGainR[input]);
					}
					#endif
					
				}
				
				if (_packetBuffer[1]==8 && _packetBuffer[2]==5)	{
					
					#if ATEM_debug
					temp = atemCameraControlLumMix[input];
					#endif
					atemCameraControlLumMix[input] = (int16_t) word(_packetBuffer[16], _packetBuffer[17]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemCameraControlLumMix[input]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemCameraControlLumMix[input=")); Serial.print(input); Serial.print(F("] = "));
						Serial.println(atemCameraControlLumMix[input]);
					}
					#endif
					
				}
				
				if (_packetBuffer[1]==8 && _packetBuffer[2]==6)	{
					
					#if ATEM_debug
					temp = atemCameraControlHue[input];
					#endif
					atemCameraControlHue[input] = (int16_t) word(_packetBuffer[16], _packetBuffer[17]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemCameraControlHue[input]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemCameraControlHue[input=")); Serial.print(input); Serial.print(F("] = "));
						Serial.println(atemCameraControlHue[input]);
					}
					#endif
					
				}
				
				if (_packetBuffer[1]==1 && _packetBuffer[2]==5)	{
					
					#if ATEM_debug
					temp = atemCameraControlShutter[input];
					#endif
					atemCameraControlShutter[input] = (int16_t) word(_packetBuffer[18], _packetBuffer[19]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemCameraControlShutter[input]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemCameraControlShutter[input=")); Serial.print(input); Serial.print(F("] = "));
						Serial.println(atemCameraControlShutter[input]);
					}
					#endif
					
				}
				
				if (_packetBuffer[1]==8 && _packetBuffer[2]==0)	{
					
					#if ATEM_debug
					temp = atemCameraControlLiftG[input];
					#endif
					atemCameraControlLiftG[input] = (int16_t) word(_packetBuffer[18], _packetBuffer[19]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemCameraControlLiftG[input]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemCameraControlLiftG[input=")); Serial.print(input); Serial.print(F("] = "));
						Serial.println(atemCameraControlLiftG[input]);
					}
					#endif
					
				}
				
				if (_packetBuffer[1]==8 && _packetBuffer[2]==1)	{
					
					#if ATEM_debug
					temp = atemCameraControlGammaG[input];
					#endif
					atemCameraControlGammaG[input] = (int16_t) word(_packetBuffer[18], _packetBuffer[19]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemCameraControlGammaG[input]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemCameraControlGammaG[input=")); Serial.print(input); Serial.print(F("] = "));
						Serial.println(atemCameraControlGammaG[input]);
					}
					#endif
					
				}
				
				if (_packetBuffer[1]==8 && _packetBuffer[2]==2)	{
					
					#if ATEM_debug
					temp = atemCameraControlGainG[input];
					#endif
					atemCameraControlGainG[input] = (int16_t) word(_packetBuffer[18], _packetBuffer[19]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemCameraControlGainG[input]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemCameraControlGainG[input=")); Serial.print(input); Serial.print(F("] = "));
						Serial.println(atemCameraControlGainG[input]);
					}
					#endif
					
				}
				
				if (_packetBuffer[1]==8 && _packetBuffer[2]==4)	{
					
					#if ATEM_debug
					temp = atemCameraControlContrast[input];
					#endif
					atemCameraControlContrast[input] = (int16_t) word(_packetBuffer[18], _packetBuffer[19]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemCameraControlContrast[input]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemCameraControlContrast[input=")); Serial.print(input); Serial.print(F("] = "));
						Serial.println(atemCameraControlContrast[input]);
					}
					#endif
					
				}
				
				if (_packetBuffer[1]==8 && _packetBuffer[2]==6)	{
					
					#if ATEM_debug
					temp = atemCameraControlSaturation[input];
					#endif
					atemCameraControlSaturation[input] = (int16_t) word(_packetBuffer[18], _packetBuffer[19]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemCameraControlSaturation[input]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemCameraControlSaturation[input=")); Serial.print(input); Serial.print(F("] = "));
						Serial.println(atemCameraControlSaturation[input]);
					}
					#endif
					
				}
				
				if (_packetBuffer[1]==8 && _packetBuffer[2]==0)	{
					
					#if ATEM_debug
					temp = atemCameraControlLiftB[input];
					#endif
					atemCameraControlLiftB[input] = (int16_t) word(_packetBuffer[20], _packetBuffer[21]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemCameraControlLiftB[input]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemCameraControlLiftB[input=")); Serial.print(input); Serial.print(F("] = "));
						Serial.println(atemCameraControlLiftB[input]);
					}
					#endif
					
				}
				
				if (_packetBuffer[1]==8 && _packetBuffer[2]==1)	{
					
					#if ATEM_debug
					temp = atemCameraControlGammaB[input];
					#endif
					atemCameraControlGammaB[input] = (int16_t) word(_packetBuffer[20], _packetBuffer[21]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemCameraControlGammaB[input]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemCameraControlGammaB[input=")); Serial.print(input); Serial.print(F("] = "));
						Serial.println(atemCameraControlGammaB[input]);
					}
					#endif
					
				}
				
				if (_packetBuffer[1]==8 && _packetBuffer[2]==2)	{
					
					#if ATEM_debug
					temp = atemCameraControlGainB[input];
					#endif
					atemCameraControlGainB[input] = (int16_t) word(_packetBuffer[20], _packetBuffer[21]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemCameraControlGainB[input]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemCameraControlGainB[input=")); Serial.print(input); Serial.print(F("] = "));
						Serial.println(atemCameraControlGainB[input]);
					}
					#endif
					
				}
				
				if (_packetBuffer[1]==8 && _packetBuffer[2]==0)	{
					
					#if ATEM_debug
					temp = atemCameraControlLiftY[input];
					#endif
					atemCameraControlLiftY[input] = (int16_t) word(_packetBuffer[22], _packetBuffer[23]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemCameraControlLiftY[input]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemCameraControlLiftY[input=")); Serial.print(input); Serial.print(F("] = "));
						Serial.println(atemCameraControlLiftY[input]);
					}
					#endif
					
				}
				
				if (_packetBuffer[1]==8 && _packetBuffer[2]==1)	{
					
					#if ATEM_debug
					temp = atemCameraControlGammaY[input];
					#endif
					atemCameraControlGammaY[input] = (int16_t) word(_packetBuffer[22], _packetBuffer[23]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemCameraControlGammaY[input]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemCameraControlGammaY[input=")); Serial.print(input); Serial.print(F("] = "));
						Serial.println(atemCameraControlGammaY[input]);
					}
					#endif
					
				}
				
				if (_packetBuffer[1]==8 && _packetBuffer[2]==2)	{
					
					#if ATEM_debug
					temp = atemCameraControlGainY[input];
					#endif
					atemCameraControlGainY[input] = (int16_t) word(_packetBuffer[22], _packetBuffer[23]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemCameraControlGainY[input]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemCameraControlGainY[input=")); Serial.print(input); Serial.print(F("] = "));
						Serial.println(atemCameraControlGainY[input]);
					}
					#endif
					
				}
				
				}
			} else 
			if(!strcmp_P(cmdStr, PSTR("RCPS"))) {
				
				mediaPlayer = _packetBuffer[0];
				if (mediaPlayer<=1) {
					#if ATEM_debug
					temp = atemClipPlayerPlaying[mediaPlayer];
					#endif
					atemClipPlayerPlaying[mediaPlayer] = _packetBuffer[1];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemClipPlayerPlaying[mediaPlayer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemClipPlayerPlaying[mediaPlayer=")); Serial.print(mediaPlayer); Serial.print(F("] = "));
						Serial.println(atemClipPlayerPlaying[mediaPlayer]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemClipPlayerLoop[mediaPlayer];
					#endif
					atemClipPlayerLoop[mediaPlayer] = _packetBuffer[2];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemClipPlayerLoop[mediaPlayer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemClipPlayerLoop[mediaPlayer=")); Serial.print(mediaPlayer); Serial.print(F("] = "));
						Serial.println(atemClipPlayerLoop[mediaPlayer]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemClipPlayerAtBeginning[mediaPlayer];
					#endif
					atemClipPlayerAtBeginning[mediaPlayer] = _packetBuffer[3];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemClipPlayerAtBeginning[mediaPlayer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemClipPlayerAtBeginning[mediaPlayer=")); Serial.print(mediaPlayer); Serial.print(F("] = "));
						Serial.println(atemClipPlayerAtBeginning[mediaPlayer]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemClipPlayerClipFrame[mediaPlayer];
					#endif
					atemClipPlayerClipFrame[mediaPlayer] = word(_packetBuffer[4], _packetBuffer[5]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemClipPlayerClipFrame[mediaPlayer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemClipPlayerClipFrame[mediaPlayer=")); Serial.print(mediaPlayer); Serial.print(F("] = "));
						Serial.println(atemClipPlayerClipFrame[mediaPlayer]);
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
			if(!strcmp_P(cmdStr, PSTR("MPSp"))) {
				
					#if ATEM_debug
					temp = atemMediaPoolStorageClip1MaxLength;
					#endif
					atemMediaPoolStorageClip1MaxLength = word(_packetBuffer[0], _packetBuffer[1]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemMediaPoolStorageClip1MaxLength!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemMediaPoolStorageClip1MaxLength = "));
						Serial.println(atemMediaPoolStorageClip1MaxLength);
					}
					#endif
					
					#if ATEM_debug
					temp = atemMediaPoolStorageClip2MaxLength;
					#endif
					atemMediaPoolStorageClip2MaxLength = word(_packetBuffer[2], _packetBuffer[3]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemMediaPoolStorageClip2MaxLength!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemMediaPoolStorageClip2MaxLength = "));
						Serial.println(atemMediaPoolStorageClip2MaxLength);
					}
					#endif
					
			} else 
			if(!strcmp_P(cmdStr, PSTR("MPCS"))) {
				
				clipBank = _packetBuffer[0];
				if (clipBank<=1) {
					#if ATEM_debug
					temp = atemMediaPlayerClipSourceIsUsed[clipBank];
					#endif
					atemMediaPlayerClipSourceIsUsed[clipBank] = _packetBuffer[1];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemMediaPlayerClipSourceIsUsed[clipBank]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemMediaPlayerClipSourceIsUsed[clipBank=")); Serial.print(clipBank); Serial.print(F("] = "));
						Serial.println(atemMediaPlayerClipSourceIsUsed[clipBank]);
					}
					#endif
					
					memset(atemMediaPlayerClipSourceFileName[clipBank],0,17);
					strncpy(atemMediaPlayerClipSourceFileName[clipBank], (char *)(_packetBuffer+2), 16);
					#if ATEM_debug
					if ((_serialOutput==0x80 && hasInitialized()) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemMediaPlayerClipSourceFileName[clipBank=")); Serial.print(clipBank); Serial.print(F("] = "));
						Serial.println(atemMediaPlayerClipSourceFileName[clipBank]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemMediaPlayerClipSourceFrames[clipBank];
					#endif
					atemMediaPlayerClipSourceFrames[clipBank] = word(_packetBuffer[66], _packetBuffer[67]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemMediaPlayerClipSourceFrames[clipBank]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemMediaPlayerClipSourceFrames[clipBank=")); Serial.print(clipBank); Serial.print(F("] = "));
						Serial.println(atemMediaPlayerClipSourceFrames[clipBank]);
					}
					#endif
					
				}
			} else 
			if(!strcmp_P(cmdStr, PSTR("MPAS"))) {
				
				clipBank = _packetBuffer[0];
				if (clipBank<=2) {
					#if ATEM_debug
					temp = atemMediaPlayerAudioSourceIsUsed[clipBank];
					#endif
					atemMediaPlayerAudioSourceIsUsed[clipBank] = _packetBuffer[1];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemMediaPlayerAudioSourceIsUsed[clipBank]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemMediaPlayerAudioSourceIsUsed[clipBank=")); Serial.print(clipBank); Serial.print(F("] = "));
						Serial.println(atemMediaPlayerAudioSourceIsUsed[clipBank]);
					}
					#endif
					
					memset(atemMediaPlayerAudioSourceFileName[clipBank],0,17);
					strncpy(atemMediaPlayerAudioSourceFileName[clipBank], (char *)(_packetBuffer+18), 16);
					#if ATEM_debug
					if ((_serialOutput==0x80 && hasInitialized()) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemMediaPlayerAudioSourceFileName[clipBank=")); Serial.print(clipBank); Serial.print(F("] = "));
						Serial.println(atemMediaPlayerAudioSourceFileName[clipBank]);
					}
					#endif
					
				}
			} else 
			if(!strcmp_P(cmdStr, PSTR("MPfe"))) {
				
				stillBank = _packetBuffer[3];
				if (stillBank<=31) {
					if (_packetBuffer[0]==0)	{
						#if ATEM_debug
						temp = atemMediaPlayerStillFilesIsUsed[stillBank];
						#endif
						atemMediaPlayerStillFilesIsUsed[stillBank] = _packetBuffer[4];
						#if ATEM_debug
						if ((_serialOutput==0x80 && atemMediaPlayerStillFilesIsUsed[stillBank]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
							Serial.print(F("atemMediaPlayerStillFilesIsUsed[stillBank=")); Serial.print(stillBank); Serial.print(F("] = "));
							Serial.println(atemMediaPlayerStillFilesIsUsed[stillBank]);
						}
						#endif

						memset(atemMediaPlayerStillFilesFileName[stillBank],0,17);
						if (_packetBuffer[23]>0)	{
							strncpy(atemMediaPlayerStillFilesFileName[stillBank], (char *)(_packetBuffer+24), _packetBuffer[23]<16?_packetBuffer[23]:16);
						}
						#if ATEM_debug
						if ((_serialOutput==0x80 && hasInitialized()) || (_serialOutput==0x81 && !hasInitialized()))	{
							Serial.print(F("atemMediaPlayerStillFilesFileName[stillBank=")); Serial.print(stillBank); Serial.print(F("] = "));
							Serial.println(atemMediaPlayerStillFilesFileName[stillBank]);
						}
						#endif
					}
			
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
					
					memset(atemMacroPropertiesName[macroIndex],0,11);
					strncpy(atemMacroPropertiesName[macroIndex], (char *)(_packetBuffer+8), _packetBuffer[5] > 10 ? 10 : _packetBuffer[5]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && hasInitialized()) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemMacroPropertiesName[macroIndex=")); Serial.print(macroIndex); Serial.print(F("] = "));
						Serial.println(atemMacroPropertiesName[macroIndex]);
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
			if(!strcmp_P(cmdStr, PSTR("SSrc"))) {
				
					#if ATEM_debug
					temp = atemSuperSourceFillSource;
					#endif
					atemSuperSourceFillSource = word(_packetBuffer[0], _packetBuffer[1]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemSuperSourceFillSource!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemSuperSourceFillSource = "));
						Serial.println(atemSuperSourceFillSource);
					}
					#endif
					
					#if ATEM_debug
					temp = atemSuperSourceKeySource;
					#endif
					atemSuperSourceKeySource = word(_packetBuffer[2], _packetBuffer[3]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemSuperSourceKeySource!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemSuperSourceKeySource = "));
						Serial.println(atemSuperSourceKeySource);
					}
					#endif
					
					#if ATEM_debug
					temp = atemSuperSourceForeground;
					#endif
					atemSuperSourceForeground = _packetBuffer[4];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemSuperSourceForeground!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemSuperSourceForeground = "));
						Serial.println(atemSuperSourceForeground);
					}
					#endif
					
					#if ATEM_debug
					temp = atemSuperSourcePreMultiplied;
					#endif
					atemSuperSourcePreMultiplied = _packetBuffer[5];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemSuperSourcePreMultiplied!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemSuperSourcePreMultiplied = "));
						Serial.println(atemSuperSourcePreMultiplied);
					}
					#endif
					
					#if ATEM_debug
					temp = atemSuperSourceClip;
					#endif
					atemSuperSourceClip = word(_packetBuffer[6], _packetBuffer[7]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemSuperSourceClip!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemSuperSourceClip = "));
						Serial.println(atemSuperSourceClip);
					}
					#endif
					
					#if ATEM_debug
					temp = atemSuperSourceGain;
					#endif
					atemSuperSourceGain = word(_packetBuffer[8], _packetBuffer[9]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemSuperSourceGain!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemSuperSourceGain = "));
						Serial.println(atemSuperSourceGain);
					}
					#endif
					
					#if ATEM_debug
					temp = atemSuperSourceInvertKey;
					#endif
					atemSuperSourceInvertKey = _packetBuffer[10];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemSuperSourceInvertKey!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemSuperSourceInvertKey = "));
						Serial.println(atemSuperSourceInvertKey);
					}
					#endif
					
					#if ATEM_debug
					temp = atemSuperSourceBorderEnabled;
					#endif
					atemSuperSourceBorderEnabled = _packetBuffer[11];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemSuperSourceBorderEnabled!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemSuperSourceBorderEnabled = "));
						Serial.println(atemSuperSourceBorderEnabled);
					}
					#endif
					
					#if ATEM_debug
					temp = atemSuperSourceBorderBevel;
					#endif
					atemSuperSourceBorderBevel = _packetBuffer[12];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemSuperSourceBorderBevel!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemSuperSourceBorderBevel = "));
						Serial.println(atemSuperSourceBorderBevel);
					}
					#endif
					
					#if ATEM_debug
					temp = atemSuperSourceBorderOuterWidth;
					#endif
					atemSuperSourceBorderOuterWidth = word(_packetBuffer[14], _packetBuffer[15]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemSuperSourceBorderOuterWidth!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemSuperSourceBorderOuterWidth = "));
						Serial.println(atemSuperSourceBorderOuterWidth);
					}
					#endif
					
					#if ATEM_debug
					temp = atemSuperSourceBorderInnerWidth;
					#endif
					atemSuperSourceBorderInnerWidth = word(_packetBuffer[16], _packetBuffer[17]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemSuperSourceBorderInnerWidth!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemSuperSourceBorderInnerWidth = "));
						Serial.println(atemSuperSourceBorderInnerWidth);
					}
					#endif
					
					#if ATEM_debug
					temp = atemSuperSourceBorderOuterSoftness;
					#endif
					atemSuperSourceBorderOuterSoftness = _packetBuffer[18];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemSuperSourceBorderOuterSoftness!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemSuperSourceBorderOuterSoftness = "));
						Serial.println(atemSuperSourceBorderOuterSoftness);
					}
					#endif
					
					#if ATEM_debug
					temp = atemSuperSourceBorderInnerSoftness;
					#endif
					atemSuperSourceBorderInnerSoftness = _packetBuffer[19];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemSuperSourceBorderInnerSoftness!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemSuperSourceBorderInnerSoftness = "));
						Serial.println(atemSuperSourceBorderInnerSoftness);
					}
					#endif
					
					#if ATEM_debug
					temp = atemSuperSourceBorderBevelSoftness;
					#endif
					atemSuperSourceBorderBevelSoftness = _packetBuffer[20];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemSuperSourceBorderBevelSoftness!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemSuperSourceBorderBevelSoftness = "));
						Serial.println(atemSuperSourceBorderBevelSoftness);
					}
					#endif
					
					#if ATEM_debug
					temp = atemSuperSourceBorderBevelPosition;
					#endif
					atemSuperSourceBorderBevelPosition = _packetBuffer[21];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemSuperSourceBorderBevelPosition!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemSuperSourceBorderBevelPosition = "));
						Serial.println(atemSuperSourceBorderBevelPosition);
					}
					#endif
					
					#if ATEM_debug
					temp = atemSuperSourceBorderHue;
					#endif
					atemSuperSourceBorderHue = word(_packetBuffer[22], _packetBuffer[23]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemSuperSourceBorderHue!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemSuperSourceBorderHue = "));
						Serial.println(atemSuperSourceBorderHue);
					}
					#endif
					
					#if ATEM_debug
					temp = atemSuperSourceBorderSaturation;
					#endif
					atemSuperSourceBorderSaturation = word(_packetBuffer[24], _packetBuffer[25]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemSuperSourceBorderSaturation!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemSuperSourceBorderSaturation = "));
						Serial.println(atemSuperSourceBorderSaturation);
					}
					#endif
					
					#if ATEM_debug
					temp = atemSuperSourceBorderLuma;
					#endif
					atemSuperSourceBorderLuma = word(_packetBuffer[26], _packetBuffer[27]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemSuperSourceBorderLuma!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemSuperSourceBorderLuma = "));
						Serial.println(atemSuperSourceBorderLuma);
					}
					#endif
					
					#if ATEM_debug
					temp = atemSuperSourceLightSourceDirection;
					#endif
					atemSuperSourceLightSourceDirection = word(_packetBuffer[28], _packetBuffer[29]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemSuperSourceLightSourceDirection!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemSuperSourceLightSourceDirection = "));
						Serial.println(atemSuperSourceLightSourceDirection);
					}
					#endif
					
					#if ATEM_debug
					temp = atemSuperSourceLightSourceAltitude;
					#endif
					atemSuperSourceLightSourceAltitude = _packetBuffer[30];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemSuperSourceLightSourceAltitude!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemSuperSourceLightSourceAltitude = "));
						Serial.println(atemSuperSourceLightSourceAltitude);
					}
					#endif
					
			} else 
			if(!strcmp_P(cmdStr, PSTR("SSBP"))) {
				
				box = _packetBuffer[0];
				if (box<=3) {
					#if ATEM_debug
					temp = atemSuperSourceBoxParametersEnabled[box];
					#endif
					atemSuperSourceBoxParametersEnabled[box] = _packetBuffer[1];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemSuperSourceBoxParametersEnabled[box]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemSuperSourceBoxParametersEnabled[box=")); Serial.print(box); Serial.print(F("] = "));
						Serial.println(atemSuperSourceBoxParametersEnabled[box]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemSuperSourceBoxParametersInputSource[box];
					#endif
					atemSuperSourceBoxParametersInputSource[box] = word(_packetBuffer[2], _packetBuffer[3]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemSuperSourceBoxParametersInputSource[box]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemSuperSourceBoxParametersInputSource[box=")); Serial.print(box); Serial.print(F("] = "));
						Serial.println(atemSuperSourceBoxParametersInputSource[box]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemSuperSourceBoxParametersPositionX[box];
					#endif
					atemSuperSourceBoxParametersPositionX[box] = (int16_t) word(_packetBuffer[4], _packetBuffer[5]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemSuperSourceBoxParametersPositionX[box]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemSuperSourceBoxParametersPositionX[box=")); Serial.print(box); Serial.print(F("] = "));
						Serial.println(atemSuperSourceBoxParametersPositionX[box]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemSuperSourceBoxParametersPositionY[box];
					#endif
					atemSuperSourceBoxParametersPositionY[box] = (int16_t) word(_packetBuffer[6], _packetBuffer[7]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemSuperSourceBoxParametersPositionY[box]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemSuperSourceBoxParametersPositionY[box=")); Serial.print(box); Serial.print(F("] = "));
						Serial.println(atemSuperSourceBoxParametersPositionY[box]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemSuperSourceBoxParametersSize[box];
					#endif
					atemSuperSourceBoxParametersSize[box] = word(_packetBuffer[8], _packetBuffer[9]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemSuperSourceBoxParametersSize[box]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemSuperSourceBoxParametersSize[box=")); Serial.print(box); Serial.print(F("] = "));
						Serial.println(atemSuperSourceBoxParametersSize[box]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemSuperSourceBoxParametersCropped[box];
					#endif
					atemSuperSourceBoxParametersCropped[box] = _packetBuffer[10];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemSuperSourceBoxParametersCropped[box]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemSuperSourceBoxParametersCropped[box=")); Serial.print(box); Serial.print(F("] = "));
						Serial.println(atemSuperSourceBoxParametersCropped[box]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemSuperSourceBoxParametersCropTop[box];
					#endif
					atemSuperSourceBoxParametersCropTop[box] = word(_packetBuffer[12], _packetBuffer[13]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemSuperSourceBoxParametersCropTop[box]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemSuperSourceBoxParametersCropTop[box=")); Serial.print(box); Serial.print(F("] = "));
						Serial.println(atemSuperSourceBoxParametersCropTop[box]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemSuperSourceBoxParametersCropBottom[box];
					#endif
					atemSuperSourceBoxParametersCropBottom[box] = word(_packetBuffer[14], _packetBuffer[15]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemSuperSourceBoxParametersCropBottom[box]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemSuperSourceBoxParametersCropBottom[box=")); Serial.print(box); Serial.print(F("] = "));
						Serial.println(atemSuperSourceBoxParametersCropBottom[box]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemSuperSourceBoxParametersCropLeft[box];
					#endif
					atemSuperSourceBoxParametersCropLeft[box] = word(_packetBuffer[16], _packetBuffer[17]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemSuperSourceBoxParametersCropLeft[box]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemSuperSourceBoxParametersCropLeft[box=")); Serial.print(box); Serial.print(F("] = "));
						Serial.println(atemSuperSourceBoxParametersCropLeft[box]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemSuperSourceBoxParametersCropRight[box];
					#endif
					atemSuperSourceBoxParametersCropRight[box] = word(_packetBuffer[18], _packetBuffer[19]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemSuperSourceBoxParametersCropRight[box]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemSuperSourceBoxParametersCropRight[box=")); Serial.print(box); Serial.print(F("] = "));
						Serial.println(atemSuperSourceBoxParametersCropRight[box]);
					}
					#endif
					
				}
			} else 
			if(!strcmp_P(cmdStr, PSTR("AMIP"))) {
				
				audioSource = word(_packetBuffer[0],_packetBuffer[1]);
				if (getAudioSrcIndex(audioSource)<=24) {
					#if ATEM_debug
					temp = atemAudioMixerInputType[getAudioSrcIndex(audioSource)];
					#endif
					atemAudioMixerInputType[getAudioSrcIndex(audioSource)] = _packetBuffer[2];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemAudioMixerInputType[getAudioSrcIndex(audioSource)]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemAudioMixerInputType[getAudioSrcIndex(audioSource)=")); Serial.print(getAudioSrcIndex(audioSource)); Serial.print(F("] = "));
						Serial.println(atemAudioMixerInputType[getAudioSrcIndex(audioSource)]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemAudioMixerInputFromMediaPlayer[getAudioSrcIndex(audioSource)];
					#endif
					atemAudioMixerInputFromMediaPlayer[getAudioSrcIndex(audioSource)] = _packetBuffer[6];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemAudioMixerInputFromMediaPlayer[getAudioSrcIndex(audioSource)]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemAudioMixerInputFromMediaPlayer[getAudioSrcIndex(audioSource)=")); Serial.print(getAudioSrcIndex(audioSource)); Serial.print(F("] = "));
						Serial.println(atemAudioMixerInputFromMediaPlayer[getAudioSrcIndex(audioSource)]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemAudioMixerInputPlugtype[getAudioSrcIndex(audioSource)];
					#endif
					atemAudioMixerInputPlugtype[getAudioSrcIndex(audioSource)] = _packetBuffer[7];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemAudioMixerInputPlugtype[getAudioSrcIndex(audioSource)]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemAudioMixerInputPlugtype[getAudioSrcIndex(audioSource)=")); Serial.print(getAudioSrcIndex(audioSource)); Serial.print(F("] = "));
						Serial.println(atemAudioMixerInputPlugtype[getAudioSrcIndex(audioSource)]);
					}
					#endif
					
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
			if(!strcmp_P(cmdStr, PSTR("AMMO"))) {
				
					#if ATEM_debug
					temp = atemAudioMixerMasterVolume;
					#endif
					atemAudioMixerMasterVolume = word(_packetBuffer[0], _packetBuffer[1]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemAudioMixerMasterVolume!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemAudioMixerMasterVolume = "));
						Serial.println(atemAudioMixerMasterVolume);
					}
					#endif
					
			} else 
			if(!strcmp_P(cmdStr, PSTR("AMmO"))) {
				
					#if ATEM_debug
					temp = atemAudioMixerMonitorMonitorAudio;
					#endif
					atemAudioMixerMonitorMonitorAudio = _packetBuffer[0];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemAudioMixerMonitorMonitorAudio!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemAudioMixerMonitorMonitorAudio = "));
						Serial.println(atemAudioMixerMonitorMonitorAudio);
					}
					#endif
					
					#if ATEM_debug
					temp = atemAudioMixerMonitorVolume;
					#endif
					atemAudioMixerMonitorVolume = word(_packetBuffer[2], _packetBuffer[3]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemAudioMixerMonitorVolume!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemAudioMixerMonitorVolume = "));
						Serial.println(atemAudioMixerMonitorVolume);
					}
					#endif
					
					#if ATEM_debug
					temp = atemAudioMixerMonitorMute;
					#endif
					atemAudioMixerMonitorMute = _packetBuffer[4];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemAudioMixerMonitorMute!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemAudioMixerMonitorMute = "));
						Serial.println(atemAudioMixerMonitorMute);
					}
					#endif
					
					#if ATEM_debug
					temp = atemAudioMixerMonitorSolo;
					#endif
					atemAudioMixerMonitorSolo = _packetBuffer[5];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemAudioMixerMonitorSolo!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemAudioMixerMonitorSolo = "));
						Serial.println(atemAudioMixerMonitorSolo);
					}
					#endif
					
					#if ATEM_debug
					temp = atemAudioMixerMonitorSoloInput;
					#endif
					atemAudioMixerMonitorSoloInput = word(_packetBuffer[6], _packetBuffer[7]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemAudioMixerMonitorSoloInput!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemAudioMixerMonitorSoloInput = "));
						Serial.println(atemAudioMixerMonitorSoloInput);
					}
					#endif
					
					#if ATEM_debug
					temp = atemAudioMixerMonitorDim;
					#endif
					atemAudioMixerMonitorDim = _packetBuffer[8];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemAudioMixerMonitorDim!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemAudioMixerMonitorDim = "));
						Serial.println(atemAudioMixerMonitorDim);
					}
					#endif
					
			} else 
			if(!strcmp_P(cmdStr, PSTR("AMLv"))) {
				
				sources = word(_packetBuffer[0],_packetBuffer[1]);
				if (sources<=24) {

						atemAudioMixerLevelsSources = word(_packetBuffer[0], _packetBuffer[1]);
							//Serial.print(F("atemAudioMixerLevelsSources = "));
							//Serial.println(atemAudioMixerLevelsSources);

						atemAudioMixerLevelsMasterLeft = (uint16_t)_packetBuffer[5]<<8 | _packetBuffer[6];
							//Serial.print(F("atemAudioMixerLevelsMasterLeft = "));
							//Serial.println(atemAudioMixerLevelsMasterLeft);

						atemAudioMixerLevelsMasterRight = (uint16_t)_packetBuffer[9]<<8 | _packetBuffer[10];
							//Serial.print(F("atemAudioMixerLevelsMasterRight = "));
							//Serial.println(atemAudioMixerLevelsMasterRight);

						atemAudioMixerLevelsMasterPeakLeft = (uint16_t)_packetBuffer[13]<<8 | _packetBuffer[14];
							//Serial.print(F("atemAudioMixerLevelsMasterPeakLeft = "));
							//Serial.println(atemAudioMixerLevelsMasterPeakLeft);

						atemAudioMixerLevelsMasterPeakRight = (uint16_t)_packetBuffer[17]<<8 | _packetBuffer[18];
							//Serial.print(F("atemAudioMixerLevelsMasterPeakRight = "));
							//Serial.println(atemAudioMixerLevelsMasterPeakRight);

						atemAudioMixerLevelsMonitor = (uint16_t)_packetBuffer[21]<<8 | _packetBuffer[22];
							//Serial.print(F("atemAudioMixerLevelsMonitor = "));
							//Serial.println(atemAudioMixerLevelsMonitor);

						_readToPacketBuffer(sources*2);
						for(uint8_t a=0;a<sources;a++)	{
							atemAudioMixerLevelsSourceOrder[a] = word(_packetBuffer[a<<1], _packetBuffer[(a<<1)+1]);
							//Serial.print(F("atemAudioMixerLevelsSourceOrder[a=")); Serial.print(a); Serial.print(F("] = "));
							//Serial.println(atemAudioMixerLevelsSourceOrder[a]);
						}
						if (sources&B1)	{	// We must read 4-byte chunks, so compensate if sources was an odd number
							_readToPacketBuffer(2);
						}

						for(uint8_t a=0;a<sources;a++)	{
							_readToPacketBuffer(16);

							atemAudioMixerLevelsSourceLeft[a] = (uint16_t)_packetBuffer[1]<<8 | _packetBuffer[2];
							//	Serial.print(F("atemAudioMixerLevelsSourceLeft[a=")); Serial.print(a); Serial.print(F("] = "));
							//	Serial.println(atemAudioMixerLevelsSourceLeft[a]);

							atemAudioMixerLevelsSourceRight[a] = (uint16_t)_packetBuffer[5]<<8 | _packetBuffer[6];
							//	Serial.print(F("atemAudioMixerLevelsSourceRight[a=")); Serial.print(a); Serial.print(F("] = "));
							//	Serial.println(atemAudioMixerLevelsSourceRight[a]);

							atemAudioMixerLevelsSourcePeakLeft[a] = (uint16_t)_packetBuffer[9]<<8 | _packetBuffer[10];
							//	Serial.print(F("atemAudioMixerLevelsSourcePeakLeft[a=")); Serial.print(a); Serial.print(F("] = "));
							//	Serial.println(atemAudioMixerLevelsSourcePeakLeft[a]);

							atemAudioMixerLevelsSourcePeakRight[a] = (uint16_t)_packetBuffer[13]<<8 | _packetBuffer[14];
							//	Serial.print(F("atemAudioMixerLevelsSourcePeakRight[a=")); Serial.print(a); Serial.print(F("] = "));
							//	Serial.println(atemAudioMixerLevelsSourcePeakRight[a]);
						}
			
				}
			} else 
			if(!strcmp_P(cmdStr, PSTR("AMTl"))) {
				
				sources = word(_packetBuffer[0],_packetBuffer[1]);
				if (sources<=24) {

					#if ATEM_debug
					temp = atemAudioMixerTallySources;
					#endif
					atemAudioMixerTallySources = word(_packetBuffer[0], _packetBuffer[1]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemAudioMixerTallySources!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemAudioMixerTallySources = "));
						Serial.println(atemAudioMixerTallySources);
					}
					#endif

					for(uint8_t a=0;a<sources;a++)	{
						#if ATEM_debug
						temp = atemAudioMixerTallyAudioSource[a];
						#endif
						atemAudioMixerTallyAudioSource[a] = word(_packetBuffer[2+3*a], _packetBuffer[2+(3*a)+1]);
						#if ATEM_debug
						if ((_serialOutput==0x80 && atemAudioMixerTallyAudioSource[a]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
							Serial.print(F("atemAudioMixerTallyAudioSource[a=")); Serial.print(a); Serial.print(F("] = "));
							Serial.println(atemAudioMixerTallyAudioSource[a]);
						}
						#endif

						#if ATEM_debug
						temp = atemAudioMixerTallyIsMixedIn[a];
						#endif
						atemAudioMixerTallyIsMixedIn[a] = _packetBuffer[2+(3*a)+2];
						#if ATEM_debug
						if ((_serialOutput==0x80 && atemAudioMixerTallyIsMixedIn[a]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
							Serial.print(F("atemAudioMixerTallyIsMixedIn[a=")); Serial.print(a); Serial.print(F("] = "));
							Serial.println(atemAudioMixerTallyIsMixedIn[a]);
						}
						#endif
					}
		
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
			if(!strcmp_P(cmdStr, PSTR("TlSr"))) {
				
				sources = word(_packetBuffer[0],_packetBuffer[1]);
				if (sources<=46) {
					#if ATEM_debug
					temp = atemTallyBySourceSources;
					#endif
					atemTallyBySourceSources = word(_packetBuffer[0], _packetBuffer[1]);
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemTallyBySourceSources!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemTallyBySourceSources = "));
						Serial.println(atemTallyBySourceSources);
					}
					#endif

					int readComp = 2;
					for(uint8_t a=0;a<sources;a++)	{
						if (2+(3*a) == readBytesForTlSr)	{
							readComp-=readBytesForTlSr;
							_readToPacketBuffer();
						}

						#if ATEM_debug
						temp = atemTallyBySourceVideoSource[a];
						#endif
						atemTallyBySourceVideoSource[a] = word(_packetBuffer[readComp+(3*a)], _packetBuffer[readComp+(3*a)+1]);
						#if ATEM_debug
						if ((_serialOutput==0x80 && atemTallyBySourceVideoSource[a]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
							Serial.print(F("atemTallyBySourceVideoSource[a=")); Serial.print(a); Serial.print(F("] = "));
							Serial.println(atemTallyBySourceVideoSource[a]);
						}
						#endif

						#if ATEM_debug
						temp = atemTallyBySourceTallyFlags[a];
						#endif
						atemTallyBySourceTallyFlags[a] = _packetBuffer[readComp+(3*a)+2];
						#if ATEM_debug
						if ((_serialOutput==0x80 && atemTallyBySourceTallyFlags[a]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
							Serial.print(F("atemTallyBySourceTallyFlags[a=")); Serial.print(a); Serial.print(F("] = "));
							Serial.println(atemTallyBySourceTallyFlags[a]);
						}
						#endif
					}

		
				}
			} else 
			if(!strcmp_P(cmdStr, PSTR("Time"))) {
				
					#if ATEM_debug
					temp = atemLastStateChangeTimeCodeHour;
					#endif
					atemLastStateChangeTimeCodeHour = _packetBuffer[0];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemLastStateChangeTimeCodeHour!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemLastStateChangeTimeCodeHour = "));
						Serial.println(atemLastStateChangeTimeCodeHour);
					}
					#endif
					
					#if ATEM_debug
					temp = atemLastStateChangeTimeCodeMinute;
					#endif
					atemLastStateChangeTimeCodeMinute = _packetBuffer[1];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemLastStateChangeTimeCodeMinute!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemLastStateChangeTimeCodeMinute = "));
						Serial.println(atemLastStateChangeTimeCodeMinute);
					}
					#endif
					
					#if ATEM_debug
					temp = atemLastStateChangeTimeCodeSecond;
					#endif
					atemLastStateChangeTimeCodeSecond = _packetBuffer[2];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemLastStateChangeTimeCodeSecond!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemLastStateChangeTimeCodeSecond = "));
						Serial.println(atemLastStateChangeTimeCodeSecond);
					}
					#endif
					
					#if ATEM_debug
					temp = atemLastStateChangeTimeCodeFrame;
					#endif
					atemLastStateChangeTimeCodeFrame = _packetBuffer[3];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemLastStateChangeTimeCodeFrame!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemLastStateChangeTimeCodeFrame = "));
						Serial.println(atemLastStateChangeTimeCodeFrame);
					}
					#endif
					
			} else 
			{}
		}





			/**
			 * Get Protocol Version; Major
			 */
			uint16_t ATEMmax::getProtocolVersionMajor() {
				return atemProtocolVersionMajor;
			}
			
			/**
			 * Get Protocol Version; Minor
			 */
			uint16_t ATEMmax::getProtocolVersionMinor() {
				return atemProtocolVersionMinor;
			}
			
			/**
			 * Get Product Id; Name
			 */
			char *  ATEMmax::getProductIdName() {
				return atemProductIdName;
			}
			
			/**
			 * Get Warning; Text
			 */
			char *  ATEMmax::getWarningText() {
				return atemWarningText;
			}
			
			/**
			 * Get Topology; MEs
			 */
			uint8_t ATEMmax::getTopologyMEs() {
				return atemTopologyMEs;
			}
			
			/**
			 * Get Topology; Sources
			 */
			uint8_t ATEMmax::getTopologySources() {
				return atemTopologySources;
			}
			
			/**
			 * Get Topology; Color Generators(?)
			 */
			uint8_t ATEMmax::getTopologyColorGenerators() {
				return atemTopologyColorGenerators;
			}
			
			/**
			 * Get Topology; AUX busses
			 */
			uint8_t ATEMmax::getTopologyAUXbusses() {
				return atemTopologyAUXbusses;
			}
			
			/**
			 * Get Topology; Downstream Keyes(?)
			 */
			uint8_t ATEMmax::getTopologyDownstreamKeyes() {
				return atemTopologyDownstreamKeyes;
			}
			
			/**
			 * Get Topology; Stingers(?)
			 */
			uint8_t ATEMmax::getTopologyStingers() {
				return atemTopologyStingers;
			}
			
			/**
			 * Get Topology; DVEs(?)
			 */
			uint8_t ATEMmax::getTopologyDVEs() {
				return atemTopologyDVEs;
			}
			
			/**
			 * Get Topology; SuperSources(?)
			 */
			uint8_t ATEMmax::getTopologySuperSources() {
				return atemTopologySuperSources;
			}
			
			/**
			 * Get Topology; Has SD Output
			 */
			bool ATEMmax::getTopologyHasSDOutput() {
				return atemTopologyHasSDOutput;
			}
			
			/**
			 * Get Mix Effect Config; Keyers On ME
			 * mE 	0: ME1, 1: ME2
			 */
			uint8_t ATEMmax::getMixEffectConfigKeyersOnME(uint8_t mE) {
				return atemMixEffectConfigKeyersOnME[mE];
			}
			
			/**
			 * Get Media Players; Still Banks
			 */
			uint8_t ATEMmax::getMediaPlayersStillBanks() {
				return atemMediaPlayersStillBanks;
			}
			
			/**
			 * Get Media Players; Clip Banks
			 */
			uint8_t ATEMmax::getMediaPlayersClipBanks() {
				return atemMediaPlayersClipBanks;
			}
			
			/**
			 * Get Multi View Config; Multi Viewers
			 */
			uint8_t ATEMmax::getMultiViewConfigMultiViewers() {
				return atemMultiViewConfigMultiViewers;
			}
			
			/**
			 * Get Super Source Config; Boxes
			 */
			uint8_t ATEMmax::getSuperSourceConfigBoxes() {
				return atemSuperSourceConfigBoxes;
			}
			
			/**
			 * Get Tally Channel Config; Tally Channels
			 */
			uint8_t ATEMmax::getTallyChannelConfigTallyChannels() {
				return atemTallyChannelConfigTallyChannels;
			}
			
			/**
			 * Get Audio Mixer Config; Audio Channels
			 */
			uint8_t ATEMmax::getAudioMixerConfigAudioChannels() {
				return atemAudioMixerConfigAudioChannels;
			}
			
			/**
			 * Get Audio Mixer Config; Has Monitor
			 */
			bool ATEMmax::getAudioMixerConfigHasMonitor() {
				return atemAudioMixerConfigHasMonitor;
			}
			
			/**
			 * Get Video Mixer Config; Modes
			 */
			uint32_t ATEMmax::getVideoMixerConfigModes() {
				return atemVideoMixerConfigModes;
			}
			
			/**
			 * Get Macro Pool; Banks
			 */
			uint8_t ATEMmax::getMacroPoolBanks() {
				return atemMacroPoolBanks;
			}
			
			/**
			 * Get Power; Status
			 */
			uint8_t ATEMmax::getPowerStatus() {
				return atemPowerStatus;
			}
			
			/**
			 * Get Down Converter; Mode
			 */
			uint8_t ATEMmax::getDownConverterMode() {
				return atemDownConverterMode;
			}
			
			/**
			 * Set Down Converter; Mode
			 * mode 	0: Center Cut, 1: Letterbox, 2: Anamorphic
			 */
			void ATEMmax::setDownConverterMode(uint8_t mode) {
			
	  	  		_prepareCommandPacket(PSTR("CDcO"),4);
		
				_packetBuffer[12+_cBBO+4+4+0] = mode;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Get Video Mode; Format
			 */
			uint8_t ATEMmax::getVideoModeFormat() {
				return atemVideoModeFormat;
			}
			
			/**
			 * Set Video Mode; Format
			 * format 	0: 525i59.94 NTSC, 1: 625i 50 PAL, 2: 525i59.94 NTSC 16:9, 3: 625i 50 PAL 16:9, 4: 720p50, 5: 720p59.94, 6: 1080i50, 7: 1080i59.94, 8: 1080p23.98, 9: 1080p24, 10: 1080p25, 11: 1080p29.97, 12: 1080p50, 13: 1080p59.94, 14: 2160p23.98, 15: 2160p24, 16: 2160p25, 17: 2160p29.97
			 */
			void ATEMmax::setVideoModeFormat(uint8_t format) {
			
	  	  		_prepareCommandPacket(PSTR("CVdM"),4);
		
				_packetBuffer[12+_cBBO+4+4+0] = format;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Get Input Properties; Long Name
			 * videoSource 	(See video source list)
			 */
			char *  ATEMmax::getInputLongName(uint16_t videoSource) {
				return atemInputLongName[getVideoSrcIndex(videoSource)];
			}
			
			/**
			 * Get Input Properties; Short Name
			 * videoSource 	(See video source list)
			 */
			char *  ATEMmax::getInputShortName(uint16_t videoSource) {
				return atemInputShortName[getVideoSrcIndex(videoSource)];
			}
			
			/**
			 * Get Input Properties; Available External Port Types
			 * videoSource 	(See video source list)
			 */
			uint8_t ATEMmax::getInputAvailableExternalPortTypes(uint16_t videoSource) {
				return atemInputAvailableExternalPortTypes[getVideoSrcIndex(videoSource)];
			}
			
			/**
			 * Get Input Properties; External Port Type
			 * videoSource 	(See video source list)
			 */
			uint8_t ATEMmax::getInputExternalPortType(uint16_t videoSource) {
				return atemInputExternalPortType[getVideoSrcIndex(videoSource)];
			}
			
			/**
			 * Get Input Properties; Port Type
			 * videoSource 	(See video source list)
			 */
			uint8_t ATEMmax::getInputPortType(uint16_t videoSource) {
				return atemInputPortType[getVideoSrcIndex(videoSource)];
			}
			
			/**
			 * Get Input Properties; Availability
			 * videoSource 	(See video source list)
			 */
			uint8_t ATEMmax::getInputAvailability(uint16_t videoSource) {
				return atemInputAvailability[getVideoSrcIndex(videoSource)];
			}
			
			/**
			 * Get Input Properties; ME Availability
			 * videoSource 	(See video source list)
			 */
			uint8_t ATEMmax::getInputMEAvailability(uint16_t videoSource) {
				return atemInputMEAvailability[getVideoSrcIndex(videoSource)];
			}
			
			/**
			 * Set Input Properties; Long Name
			 * videoSource 	(See video source list)
			 * longName 	
			 */
			void ATEMmax::setInputLongName(uint16_t videoSource, char *  longName) {
			
	  	  		_prepareCommandPacket(PSTR("CInL"),32,(_packetBuffer[12+_cBBO+4+4+2]==highByte(videoSource)) && (_packetBuffer[12+_cBBO+4+4+3]==lowByte(videoSource)));
		
					// Set Mask: 1
				_packetBuffer[12+_cBBO+4+4+0] |= 1;
						
				_packetBuffer[12+_cBBO+4+4+2] = highByte(videoSource);
				_packetBuffer[12+_cBBO+4+4+3] = lowByte(videoSource);
				
				strncpy((char *)(_packetBuffer+12+4+4+4), longName, 20);
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Input Properties; Short Name
			 * videoSource 	(See video source list)
			 * shortName 	
			 */
			void ATEMmax::setInputShortName(uint16_t videoSource, char *  shortName) {
			
	  	  		_prepareCommandPacket(PSTR("CInL"),32,(_packetBuffer[12+_cBBO+4+4+2]==highByte(videoSource)) && (_packetBuffer[12+_cBBO+4+4+3]==lowByte(videoSource)));
		
					// Set Mask: 2
				_packetBuffer[12+_cBBO+4+4+0] |= 2;
						
				_packetBuffer[12+_cBBO+4+4+2] = highByte(videoSource);
				_packetBuffer[12+_cBBO+4+4+3] = lowByte(videoSource);
				
				strncpy((char *)(_packetBuffer+12+4+4+24), shortName, 4);
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Input Properties; External Port Type
			 * videoSource 	(See video source list)
			 * externalPortType 	
			 */
			void ATEMmax::setInputExternalPortType(uint16_t videoSource, uint16_t externalPortType) {
			
	  	  		_prepareCommandPacket(PSTR("CInL"),32,(_packetBuffer[12+_cBBO+4+4+2]==highByte(videoSource)) && (_packetBuffer[12+_cBBO+4+4+3]==lowByte(videoSource)));
		
					// Set Mask: 4
				_packetBuffer[12+_cBBO+4+4+0] |= 4;
						
				_packetBuffer[12+_cBBO+4+4+2] = highByte(videoSource);
				_packetBuffer[12+_cBBO+4+4+3] = lowByte(videoSource);
				
				_packetBuffer[12+_cBBO+4+4+28] = highByte(externalPortType);
				_packetBuffer[12+_cBBO+4+4+29] = lowByte(externalPortType);
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Get Multi Viewer Properties; Layout
			 * multiViewer 	0: 1, 1: 2
			 */
			uint8_t ATEMmax::getMultiViewerPropertiesLayout(uint8_t multiViewer) {
				return atemMultiViewerPropertiesLayout[multiViewer];
			}
			
			/**
			 * Set Multi Viewer Properties; Layout
			 * multiViewer 	0: 1, 1: 2
			 * layout 	0: Top, 1: Bottom, 2: Left, 3: Right
			 */
			void ATEMmax::setMultiViewerPropertiesLayout(uint8_t multiViewer, uint8_t layout) {
			
	  	  		_prepareCommandPacket(PSTR("CMvP"),4,(_packetBuffer[12+_cBBO+4+4+1]==multiViewer));
		
					// Set Mask: 1
				_packetBuffer[12+_cBBO+4+4+0] |= 1;
						
				_packetBuffer[12+_cBBO+4+4+1] = multiViewer;
				
				_packetBuffer[12+_cBBO+4+4+2] = layout;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Get Multi Viewer Input; Video Source
			 * multiViewer 	0: 1, 1: 2
			 * windowIndex 	0-9
			 */
			uint16_t ATEMmax::getMultiViewerInputVideoSource(uint8_t multiViewer, uint8_t windowIndex) {
				return atemMultiViewerInputVideoSource[multiViewer][windowIndex];
			}
			
			/**
			 * Set Multi Viewer Input; Video Source
			 * multiViewer 	0: 1, 1: 2
			 * windowIndex 	2-9
			 * videoSource 	(See video source list)
			 */
			void ATEMmax::setMultiViewerInputVideoSource(uint8_t multiViewer, uint8_t windowIndex, uint16_t videoSource) {
			
	  	  		_prepareCommandPacket(PSTR("CMvI"),4,(_packetBuffer[12+_cBBO+4+4+0]==multiViewer) && (_packetBuffer[12+_cBBO+4+4+1]==windowIndex));
		
				_packetBuffer[12+_cBBO+4+4+0] = multiViewer;
				
				_packetBuffer[12+_cBBO+4+4+1] = windowIndex;
				
				_packetBuffer[12+_cBBO+4+4+2] = highByte(videoSource);
				_packetBuffer[12+_cBBO+4+4+3] = lowByte(videoSource);
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Get Program Input; Video Source
			 * mE 	0: ME1, 1: ME2
			 */
			uint16_t ATEMmax::getProgramInputVideoSource(uint8_t mE) {
				return atemProgramInputVideoSource[mE];
			}
			
			/**
			 * Set Program Input; Video Source
			 * mE 	0: ME1, 1: ME2
			 * videoSource 	(See video source list)
			 */
			void ATEMmax::setProgramInputVideoSource(uint8_t mE, uint16_t videoSource) {
			
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
			uint16_t ATEMmax::getPreviewInputVideoSource(uint8_t mE) {
				return atemPreviewInputVideoSource[mE];
			}
			
			/**
			 * Set Preview Input; Video Source
			 * mE 	0: ME1, 1: ME2
			 * videoSource 	(See video source list)
			 */
			void ATEMmax::setPreviewInputVideoSource(uint8_t mE, uint16_t videoSource) {
			
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
			void ATEMmax::performCutME(uint8_t mE) {
			
	  	  		_prepareCommandPacket(PSTR("DCut"),4);
		
				_packetBuffer[12+_cBBO+4+4+0] = mE;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Auto; M/E
			 * mE 	0: ME1, 1: ME2
			 */
			void ATEMmax::performAutoME(uint8_t mE) {
			
	  	  		_prepareCommandPacket(PSTR("DAut"),4);
		
				_packetBuffer[12+_cBBO+4+4+0] = mE;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Get Transition; Style
			 * mE 	0: ME1, 1: ME2
			 */
			uint8_t ATEMmax::getTransitionStyle(uint8_t mE) {
				return atemTransitionStyle[mE];
			}
			
			/**
			 * Get Transition; Next Transition
			 * mE 	0: ME1, 1: ME2
			 */
			uint8_t ATEMmax::getTransitionNextTransition(uint8_t mE) {
				return atemTransitionNextTransition[mE];
			}
			
			/**
			 * Get Transition; Style Next
			 * mE 	0: ME1, 1: ME2
			 */
			uint8_t ATEMmax::getTransitionStyleNext(uint8_t mE) {
				return atemTransitionStyleNext[mE];
			}
			
			/**
			 * Get Transition; Next Transition Next
			 * mE 	0: ME1, 1: ME2
			 */
			uint8_t ATEMmax::getTransitionNextTransitionNext(uint8_t mE) {
				return atemTransitionNextTransitionNext[mE];
			}
			
			/**
			 * Set Transition; Style
			 * mE 	0: ME1, 1: ME2
			 * style 	0: Mix, 1: Dip, 2: Wipe, 3: DVE, 4: Sting
			 */
			void ATEMmax::setTransitionStyle(uint8_t mE, uint8_t style) {
			
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
			void ATEMmax::setTransitionNextTransition(uint8_t mE, uint8_t nextTransition) {
			
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
			bool ATEMmax::getTransitionPreviewEnabled(uint8_t mE) {
				return atemTransitionPreviewEnabled[mE];
			}
			
			/**
			 * Set Transition Preview; Enabled
			 * mE 	0: ME1, 1: ME2
			 * enabled 	Bit 0: On/Off
			 */
			void ATEMmax::setTransitionPreviewEnabled(uint8_t mE, bool enabled) {
			
	  	  		_prepareCommandPacket(PSTR("CTPr"),4,(_packetBuffer[12+_cBBO+4+4+0]==mE));
		
				_packetBuffer[12+_cBBO+4+4+0] = mE;
				
				_packetBuffer[12+_cBBO+4+4+1] = enabled;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Get Transition Position; In Transition
			 * mE 	0: ME1, 1: ME2
			 */
			bool ATEMmax::getTransitionInTransition(uint8_t mE) {
				return atemTransitionInTransition[mE];
			}
			
			/**
			 * Get Transition Position; Frames Remaining
			 * mE 	0: ME1, 1: ME2
			 */
			uint8_t ATEMmax::getTransitionFramesRemaining(uint8_t mE) {
				return atemTransitionFramesRemaining[mE];
			}
			
			/**
			 * Get Transition Position; Position
			 * mE 	0: ME1, 1: ME2
			 */
			uint16_t ATEMmax::getTransitionPosition(uint8_t mE) {
				return atemTransitionPosition[mE];
			}
			
			/**
			 * Set Transition Position; Position
			 * mE 	0: ME1, 1: ME2
			 * position 	0-9999
			 */
			void ATEMmax::setTransitionPosition(uint8_t mE, uint16_t position) {
			
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
			uint8_t ATEMmax::getTransitionMixRate(uint8_t mE) {
				return atemTransitionMixRate[mE];
			}
			
			/**
			 * Set Transition Mix; Rate
			 * mE 	0: ME1, 1: ME2
			 * rate 	1-250: Frames
			 */
			void ATEMmax::setTransitionMixRate(uint8_t mE, uint8_t rate) {
			
	  	  		_prepareCommandPacket(PSTR("CTMx"),4,(_packetBuffer[12+_cBBO+4+4+0]==mE));
		
				_packetBuffer[12+_cBBO+4+4+0] = mE;
				
				_packetBuffer[12+_cBBO+4+4+1] = rate;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Get Transition Dip; Rate
			 * mE 	0: ME1, 1: ME2
			 */
			uint8_t ATEMmax::getTransitionDipRate(uint8_t mE) {
				return atemTransitionDipRate[mE];
			}
			
			/**
			 * Get Transition Dip; Input
			 * mE 	0: ME1, 1: ME2
			 */
			uint16_t ATEMmax::getTransitionDipInput(uint8_t mE) {
				return atemTransitionDipInput[mE];
			}
			
			/**
			 * Set Transition Dip; Rate
			 * mE 	0: ME1, 1: ME2
			 * rate 	1-250: Frames
			 */
			void ATEMmax::setTransitionDipRate(uint8_t mE, uint8_t rate) {
			
	  	  		_prepareCommandPacket(PSTR("CTDp"),8,(_packetBuffer[12+_cBBO+4+4+1]==mE));
		
					// Set Mask: 1
				_packetBuffer[12+_cBBO+4+4+0] |= 1;
						
				_packetBuffer[12+_cBBO+4+4+1] = mE;
				
				_packetBuffer[12+_cBBO+4+4+2] = rate;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Transition Dip; Input
			 * mE 	0: ME1, 1: ME2
			 * input 	(See video source list)
			 */
			void ATEMmax::setTransitionDipInput(uint8_t mE, uint16_t input) {
			
	  	  		_prepareCommandPacket(PSTR("CTDp"),8,(_packetBuffer[12+_cBBO+4+4+1]==mE));
		
					// Set Mask: 2
				_packetBuffer[12+_cBBO+4+4+0] |= 2;
						
				_packetBuffer[12+_cBBO+4+4+1] = mE;
				
				_packetBuffer[12+_cBBO+4+4+4] = highByte(input);
				_packetBuffer[12+_cBBO+4+4+5] = lowByte(input);
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Get Transition Wipe; Rate
			 * mE 	0: ME1, 1: ME2
			 */
			uint8_t ATEMmax::getTransitionWipeRate(uint8_t mE) {
				return atemTransitionWipeRate[mE];
			}
			
			/**
			 * Get Transition Wipe; Pattern
			 * mE 	0: ME1, 1: ME2
			 */
			uint8_t ATEMmax::getTransitionWipePattern(uint8_t mE) {
				return atemTransitionWipePattern[mE];
			}
			
			/**
			 * Get Transition Wipe; Width
			 * mE 	0: ME1, 1: ME2
			 */
			uint16_t ATEMmax::getTransitionWipeWidth(uint8_t mE) {
				return atemTransitionWipeWidth[mE];
			}
			
			/**
			 * Get Transition Wipe; Fill Source
			 * mE 	0: ME1, 1: ME2
			 */
			uint16_t ATEMmax::getTransitionWipeFillSource(uint8_t mE) {
				return atemTransitionWipeFillSource[mE];
			}
			
			/**
			 * Get Transition Wipe; Symmetry
			 * mE 	0: ME1, 1: ME2
			 */
			uint16_t ATEMmax::getTransitionWipeSymmetry(uint8_t mE) {
				return atemTransitionWipeSymmetry[mE];
			}
			
			/**
			 * Get Transition Wipe; Softness
			 * mE 	0: ME1, 1: ME2
			 */
			uint16_t ATEMmax::getTransitionWipeSoftness(uint8_t mE) {
				return atemTransitionWipeSoftness[mE];
			}
			
			/**
			 * Get Transition Wipe; Position X
			 * mE 	0: ME1, 1: ME2
			 */
			uint16_t ATEMmax::getTransitionWipePositionX(uint8_t mE) {
				return atemTransitionWipePositionX[mE];
			}
			
			/**
			 * Get Transition Wipe; Position Y
			 * mE 	0: ME1, 1: ME2
			 */
			uint16_t ATEMmax::getTransitionWipePositionY(uint8_t mE) {
				return atemTransitionWipePositionY[mE];
			}
			
			/**
			 * Get Transition Wipe; Reverse
			 * mE 	0: ME1, 1: ME2
			 */
			bool ATEMmax::getTransitionWipeReverse(uint8_t mE) {
				return atemTransitionWipeReverse[mE];
			}
			
			/**
			 * Get Transition Wipe; FlipFlop
			 * mE 	0: ME1, 1: ME2
			 */
			bool ATEMmax::getTransitionWipeFlipFlop(uint8_t mE) {
				return atemTransitionWipeFlipFlop[mE];
			}
			
			/**
			 * Set Transition Wipe; Rate
			 * mE 	0: ME1, 1: ME2
			 * rate 	1-250: Frames
			 */
			void ATEMmax::setTransitionWipeRate(uint8_t mE, uint8_t rate) {
			
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
			void ATEMmax::setTransitionWipePattern(uint8_t mE, uint8_t pattern) {
			
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
			void ATEMmax::setTransitionWipeWidth(uint8_t mE, uint16_t width) {
			
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
			void ATEMmax::setTransitionWipeFillSource(uint8_t mE, uint16_t fillSource) {
			
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
			void ATEMmax::setTransitionWipeSymmetry(uint8_t mE, uint16_t symmetry) {
			
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
			void ATEMmax::setTransitionWipeSoftness(uint8_t mE, uint16_t softness) {
			
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
			void ATEMmax::setTransitionWipePositionX(uint8_t mE, uint16_t positionX) {
			
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
			void ATEMmax::setTransitionWipePositionY(uint8_t mE, uint16_t positionY) {
			
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
			void ATEMmax::setTransitionWipeReverse(uint8_t mE, bool reverse) {
			
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
			void ATEMmax::setTransitionWipeFlipFlop(uint8_t mE, bool flipFlop) {
			
	  	  		_prepareCommandPacket(PSTR("CTWp"),20,(_packetBuffer[12+_cBBO+4+4+2]==mE));
		
					// Set Mask: 512
				_packetBuffer[12+_cBBO+4+4+0] |= 2;
						
				_packetBuffer[12+_cBBO+4+4+2] = mE;
				
				_packetBuffer[12+_cBBO+4+4+19] = flipFlop;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Get Transition DVE; Rate
			 * mE 	0: ME1, 1: ME2
			 */
			uint8_t ATEMmax::getTransitionDVERate(uint8_t mE) {
				return atemTransitionDVERate[mE];
			}
			
			/**
			 * Get Transition DVE; Style
			 * mE 	0: ME1, 1: ME2
			 */
			uint8_t ATEMmax::getTransitionDVEStyle(uint8_t mE) {
				return atemTransitionDVEStyle[mE];
			}
			
			/**
			 * Get Transition DVE; Fill Source
			 * mE 	0: ME1, 1: ME2
			 */
			uint16_t ATEMmax::getTransitionDVEFillSource(uint8_t mE) {
				return atemTransitionDVEFillSource[mE];
			}
			
			/**
			 * Get Transition DVE; Key Source
			 * mE 	0: ME1, 1: ME2
			 */
			uint16_t ATEMmax::getTransitionDVEKeySource(uint8_t mE) {
				return atemTransitionDVEKeySource[mE];
			}
			
			/**
			 * Get Transition DVE; Enable Key
			 * mE 	0: ME1, 1: ME2
			 */
			bool ATEMmax::getTransitionDVEEnableKey(uint8_t mE) {
				return atemTransitionDVEEnableKey[mE];
			}
			
			/**
			 * Get Transition DVE; Pre Multiplied
			 * mE 	0: ME1, 1: ME2
			 */
			bool ATEMmax::getTransitionDVEPreMultiplied(uint8_t mE) {
				return atemTransitionDVEPreMultiplied[mE];
			}
			
			/**
			 * Get Transition DVE; Clip
			 * mE 	0: ME1, 1: ME2
			 */
			uint16_t ATEMmax::getTransitionDVEClip(uint8_t mE) {
				return atemTransitionDVEClip[mE];
			}
			
			/**
			 * Get Transition DVE; Gain
			 * mE 	0: ME1, 1: ME2
			 */
			uint16_t ATEMmax::getTransitionDVEGain(uint8_t mE) {
				return atemTransitionDVEGain[mE];
			}
			
			/**
			 * Get Transition DVE; Invert Key
			 * mE 	0: ME1, 1: ME2
			 */
			bool ATEMmax::getTransitionDVEInvertKey(uint8_t mE) {
				return atemTransitionDVEInvertKey[mE];
			}
			
			/**
			 * Get Transition DVE; Reverse
			 * mE 	0: ME1, 1: ME2
			 */
			bool ATEMmax::getTransitionDVEReverse(uint8_t mE) {
				return atemTransitionDVEReverse[mE];
			}
			
			/**
			 * Get Transition DVE; FlipFlop
			 * mE 	0: ME1, 1: ME2
			 */
			bool ATEMmax::getTransitionDVEFlipFlop(uint8_t mE) {
				return atemTransitionDVEFlipFlop[mE];
			}
			
			/**
			 * Set Transition DVE; Rate
			 * mE 	0: ME1, 1: ME2
			 * rate 	1-250: Frames
			 */
			void ATEMmax::setTransitionDVERate(uint8_t mE, uint8_t rate) {
			
	  	  		_prepareCommandPacket(PSTR("CTDv"),20,(_packetBuffer[12+_cBBO+4+4+2]==mE));
		
					// Set Mask: 1
				_packetBuffer[12+_cBBO+4+4+1] |= 1;
						
				_packetBuffer[12+_cBBO+4+4+2] = mE;
				
				_packetBuffer[12+_cBBO+4+4+3] = rate;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Transition DVE; Style
			 * mE 	0: ME1, 1: ME2
			 * style 	0-34
			 */
			void ATEMmax::setTransitionDVEStyle(uint8_t mE, uint8_t style) {
			
	  	  		_prepareCommandPacket(PSTR("CTDv"),20,(_packetBuffer[12+_cBBO+4+4+2]==mE));
		
					// Set Mask: 4
				_packetBuffer[12+_cBBO+4+4+1] |= 4;
						
				_packetBuffer[12+_cBBO+4+4+2] = mE;
				
				_packetBuffer[12+_cBBO+4+4+5] = style;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Transition DVE; Fill Source
			 * mE 	0: ME1, 1: ME2
			 * fillSource 	(See video source list)
			 */
			void ATEMmax::setTransitionDVEFillSource(uint8_t mE, uint16_t fillSource) {
			
	  	  		_prepareCommandPacket(PSTR("CTDv"),20,(_packetBuffer[12+_cBBO+4+4+2]==mE));
		
					// Set Mask: 8
				_packetBuffer[12+_cBBO+4+4+1] |= 8;
						
				_packetBuffer[12+_cBBO+4+4+2] = mE;
				
				_packetBuffer[12+_cBBO+4+4+6] = highByte(fillSource);
				_packetBuffer[12+_cBBO+4+4+7] = lowByte(fillSource);
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Transition DVE; Key Source
			 * mE 	0: ME1, 1: ME2
			 * keySource 	(See video source list)
			 */
			void ATEMmax::setTransitionDVEKeySource(uint8_t mE, uint16_t keySource) {
			
	  	  		_prepareCommandPacket(PSTR("CTDv"),20,(_packetBuffer[12+_cBBO+4+4+2]==mE));
		
					// Set Mask: 16
				_packetBuffer[12+_cBBO+4+4+1] |= 16;
						
				_packetBuffer[12+_cBBO+4+4+2] = mE;
				
				_packetBuffer[12+_cBBO+4+4+8] = highByte(keySource);
				_packetBuffer[12+_cBBO+4+4+9] = lowByte(keySource);
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Transition DVE; Enable Key
			 * mE 	0: ME1, 1: ME2
			 * enableKey 	Bit 0: On/Off
			 */
			void ATEMmax::setTransitionDVEEnableKey(uint8_t mE, bool enableKey) {
			
	  	  		_prepareCommandPacket(PSTR("CTDv"),20,(_packetBuffer[12+_cBBO+4+4+2]==mE));
		
					// Set Mask: 32
				_packetBuffer[12+_cBBO+4+4+1] |= 32;
						
				_packetBuffer[12+_cBBO+4+4+2] = mE;
				
				_packetBuffer[12+_cBBO+4+4+10] = enableKey;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Transition DVE; Pre Multiplied
			 * mE 	0: ME1, 1: ME2
			 * preMultiplied 	Bit 0: On/Off
			 */
			void ATEMmax::setTransitionDVEPreMultiplied(uint8_t mE, bool preMultiplied) {
			
	  	  		_prepareCommandPacket(PSTR("CTDv"),20,(_packetBuffer[12+_cBBO+4+4+2]==mE));
		
					// Set Mask: 64
				_packetBuffer[12+_cBBO+4+4+1] |= 64;
						
				_packetBuffer[12+_cBBO+4+4+2] = mE;
				
				_packetBuffer[12+_cBBO+4+4+11] = preMultiplied;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Transition DVE; Clip
			 * mE 	0: ME1, 1: ME2
			 * clip 	0-1000: 0-100%
			 */
			void ATEMmax::setTransitionDVEClip(uint8_t mE, uint16_t clip) {
			
	  	  		_prepareCommandPacket(PSTR("CTDv"),20,(_packetBuffer[12+_cBBO+4+4+2]==mE));
		
					// Set Mask: 128
				_packetBuffer[12+_cBBO+4+4+1] |= 128;
						
				_packetBuffer[12+_cBBO+4+4+2] = mE;
				
				_packetBuffer[12+_cBBO+4+4+12] = highByte(clip);
				_packetBuffer[12+_cBBO+4+4+13] = lowByte(clip);
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Transition DVE; Gain
			 * mE 	0: ME1, 1: ME2
			 * gain 	0-1000: 0-100%
			 */
			void ATEMmax::setTransitionDVEGain(uint8_t mE, uint16_t gain) {
			
	  	  		_prepareCommandPacket(PSTR("CTDv"),20,(_packetBuffer[12+_cBBO+4+4+2]==mE));
		
					// Set Mask: 256
				_packetBuffer[12+_cBBO+4+4+0] |= 1;
						
				_packetBuffer[12+_cBBO+4+4+2] = mE;
				
				_packetBuffer[12+_cBBO+4+4+14] = highByte(gain);
				_packetBuffer[12+_cBBO+4+4+15] = lowByte(gain);
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Transition DVE; Invert Key
			 * mE 	0: ME1, 1: ME2
			 * invertKey 	Bit 0: On/Off
			 */
			void ATEMmax::setTransitionDVEInvertKey(uint8_t mE, bool invertKey) {
			
	  	  		_prepareCommandPacket(PSTR("CTDv"),20,(_packetBuffer[12+_cBBO+4+4+2]==mE));
		
					// Set Mask: 512
				_packetBuffer[12+_cBBO+4+4+0] |= 2;
						
				_packetBuffer[12+_cBBO+4+4+2] = mE;
				
				_packetBuffer[12+_cBBO+4+4+16] = invertKey;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Transition DVE; Reverse
			 * mE 	0: ME1, 1: ME2
			 * reverse 	Bit 0: On/Off
			 */
			void ATEMmax::setTransitionDVEReverse(uint8_t mE, bool reverse) {
			
	  	  		_prepareCommandPacket(PSTR("CTDv"),20,(_packetBuffer[12+_cBBO+4+4+2]==mE));
		
					// Set Mask: 1024
				_packetBuffer[12+_cBBO+4+4+0] |= 4;
						
				_packetBuffer[12+_cBBO+4+4+2] = mE;
				
				_packetBuffer[12+_cBBO+4+4+17] = reverse;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Transition DVE; FlipFlop
			 * mE 	0: ME1, 1: ME2
			 * flipFlop 	Bit 0: On/Off
			 */
			void ATEMmax::setTransitionDVEFlipFlop(uint8_t mE, bool flipFlop) {
			
	  	  		_prepareCommandPacket(PSTR("CTDv"),20,(_packetBuffer[12+_cBBO+4+4+2]==mE));
		
					// Set Mask: 2048
				_packetBuffer[12+_cBBO+4+4+0] |= 8;
						
				_packetBuffer[12+_cBBO+4+4+2] = mE;
				
				_packetBuffer[12+_cBBO+4+4+18] = flipFlop;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Get Transition Stinger; Source
			 * mE 	0: ME1, 1: ME2
			 */
			uint8_t ATEMmax::getTransitionStingerSource(uint8_t mE) {
				return atemTransitionStingerSource[mE];
			}
			
			/**
			 * Get Transition Stinger; Pre Multiplied
			 * mE 	0: ME1, 1: ME2
			 */
			bool ATEMmax::getTransitionStingerPreMultiplied(uint8_t mE) {
				return atemTransitionStingerPreMultiplied[mE];
			}
			
			/**
			 * Get Transition Stinger; Clip
			 * mE 	0: ME1, 1: ME2
			 */
			uint16_t ATEMmax::getTransitionStingerClip(uint8_t mE) {
				return atemTransitionStingerClip[mE];
			}
			
			/**
			 * Get Transition Stinger; Gain
			 * mE 	0: ME1, 1: ME2
			 */
			uint16_t ATEMmax::getTransitionStingerGain(uint8_t mE) {
				return atemTransitionStingerGain[mE];
			}
			
			/**
			 * Get Transition Stinger; Invert Key
			 * mE 	0: ME1, 1: ME2
			 */
			bool ATEMmax::getTransitionStingerInvertKey(uint8_t mE) {
				return atemTransitionStingerInvertKey[mE];
			}
			
			/**
			 * Get Transition Stinger; Pre Roll
			 * mE 	0: ME1, 1: ME2
			 */
			uint16_t ATEMmax::getTransitionStingerPreRoll(uint8_t mE) {
				return atemTransitionStingerPreRoll[mE];
			}
			
			/**
			 * Get Transition Stinger; Clip Duration
			 * mE 	0: ME1, 1: ME2
			 */
			uint16_t ATEMmax::getTransitionStingerClipDuration(uint8_t mE) {
				return atemTransitionStingerClipDuration[mE];
			}
			
			/**
			 * Get Transition Stinger; Trigger Point
			 * mE 	0: ME1, 1: ME2
			 */
			uint16_t ATEMmax::getTransitionStingerTriggerPoint(uint8_t mE) {
				return atemTransitionStingerTriggerPoint[mE];
			}
			
			/**
			 * Get Transition Stinger; Mix Rate
			 * mE 	0: ME1, 1: ME2
			 */
			uint16_t ATEMmax::getTransitionStingerMixRate(uint8_t mE) {
				return atemTransitionStingerMixRate[mE];
			}
			
			/**
			 * Set Transition Stinger; Source
			 * mE 	0: ME1, 1: ME2
			 * source 	1: Media Player 1, 2: Media Player 2
			 */
			void ATEMmax::setTransitionStingerSource(uint8_t mE, uint8_t source) {
			
	  	  		_prepareCommandPacket(PSTR("CTSt"),20,(_packetBuffer[12+_cBBO+4+4+2]==mE));
		
					// Set Mask: 1
				_packetBuffer[12+_cBBO+4+4+1] |= 1;
						
				_packetBuffer[12+_cBBO+4+4+2] = mE;
				
				_packetBuffer[12+_cBBO+4+4+3] = source;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Transition Stinger; Pre Multiplied
			 * mE 	0: ME1, 1: ME2
			 * preMultiplied 	Bit 0: On/Off
			 */
			void ATEMmax::setTransitionStingerPreMultiplied(uint8_t mE, bool preMultiplied) {
			
	  	  		_prepareCommandPacket(PSTR("CTSt"),20,(_packetBuffer[12+_cBBO+4+4+2]==mE));
		
					// Set Mask: 2
				_packetBuffer[12+_cBBO+4+4+1] |= 2;
						
				_packetBuffer[12+_cBBO+4+4+2] = mE;
				
				_packetBuffer[12+_cBBO+4+4+4] = preMultiplied;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Transition Stinger; Clip
			 * mE 	0: ME1, 1: ME2
			 * clip 	0-1000: 0-100%
			 */
			void ATEMmax::setTransitionStingerClip(uint8_t mE, uint16_t clip) {
			
	  	  		_prepareCommandPacket(PSTR("CTSt"),20,(_packetBuffer[12+_cBBO+4+4+2]==mE));
		
					// Set Mask: 4
				_packetBuffer[12+_cBBO+4+4+1] |= 4;
						
				_packetBuffer[12+_cBBO+4+4+2] = mE;
				
				_packetBuffer[12+_cBBO+4+4+6] = highByte(clip);
				_packetBuffer[12+_cBBO+4+4+7] = lowByte(clip);
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Transition Stinger; Gain
			 * mE 	0: ME1, 1: ME2
			 * gain 	0-1000: 0-100%
			 */
			void ATEMmax::setTransitionStingerGain(uint8_t mE, uint16_t gain) {
			
	  	  		_prepareCommandPacket(PSTR("CTSt"),20,(_packetBuffer[12+_cBBO+4+4+2]==mE));
		
					// Set Mask: 8
				_packetBuffer[12+_cBBO+4+4+1] |= 8;
						
				_packetBuffer[12+_cBBO+4+4+2] = mE;
				
				_packetBuffer[12+_cBBO+4+4+8] = highByte(gain);
				_packetBuffer[12+_cBBO+4+4+9] = lowByte(gain);
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Transition Stinger; Invert Key
			 * mE 	0: ME1, 1: ME2
			 * invertKey 	Bit 0: On/Off
			 */
			void ATEMmax::setTransitionStingerInvertKey(uint8_t mE, bool invertKey) {
			
	  	  		_prepareCommandPacket(PSTR("CTSt"),20,(_packetBuffer[12+_cBBO+4+4+2]==mE));
		
					// Set Mask: 16
				_packetBuffer[12+_cBBO+4+4+1] |= 16;
						
				_packetBuffer[12+_cBBO+4+4+2] = mE;
				
				_packetBuffer[12+_cBBO+4+4+10] = invertKey;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Transition Stinger; Pre Roll
			 * mE 	0: ME1, 1: ME2
			 * preRoll 	Frames
			 */
			void ATEMmax::setTransitionStingerPreRoll(uint8_t mE, uint16_t preRoll) {
			
	  	  		_prepareCommandPacket(PSTR("CTSt"),20,(_packetBuffer[12+_cBBO+4+4+2]==mE));
		
					// Set Mask: 32
				_packetBuffer[12+_cBBO+4+4+1] |= 32;
						
				_packetBuffer[12+_cBBO+4+4+2] = mE;
				
				_packetBuffer[12+_cBBO+4+4+12] = highByte(preRoll);
				_packetBuffer[12+_cBBO+4+4+13] = lowByte(preRoll);
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Transition Stinger; Clip Duration
			 * mE 	0: ME1, 1: ME2
			 * clipDuration 	Frames
			 */
			void ATEMmax::setTransitionStingerClipDuration(uint8_t mE, uint16_t clipDuration) {
			
	  	  		_prepareCommandPacket(PSTR("CTSt"),20,(_packetBuffer[12+_cBBO+4+4+2]==mE));
		
					// Set Mask: 64
				_packetBuffer[12+_cBBO+4+4+1] |= 64;
						
				_packetBuffer[12+_cBBO+4+4+2] = mE;
				
				_packetBuffer[12+_cBBO+4+4+14] = highByte(clipDuration);
				_packetBuffer[12+_cBBO+4+4+15] = lowByte(clipDuration);
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Transition Stinger; Trigger Point
			 * mE 	0: ME1, 1: ME2
			 * triggerPoint 	Frames
			 */
			void ATEMmax::setTransitionStingerTriggerPoint(uint8_t mE, uint16_t triggerPoint) {
			
	  	  		_prepareCommandPacket(PSTR("CTSt"),20,(_packetBuffer[12+_cBBO+4+4+2]==mE));
		
					// Set Mask: 128
				_packetBuffer[12+_cBBO+4+4+1] |= 128;
						
				_packetBuffer[12+_cBBO+4+4+2] = mE;
				
				_packetBuffer[12+_cBBO+4+4+16] = highByte(triggerPoint);
				_packetBuffer[12+_cBBO+4+4+17] = lowByte(triggerPoint);
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Transition Stinger; Mix Rate
			 * mE 	0: ME1, 1: ME2
			 * mixRate 	Frames
			 */
			void ATEMmax::setTransitionStingerMixRate(uint8_t mE, uint16_t mixRate) {
			
	  	  		_prepareCommandPacket(PSTR("CTSt"),20,(_packetBuffer[12+_cBBO+4+4+2]==mE));
		
					// Set Mask: 256
				_packetBuffer[12+_cBBO+4+4+0] |= 1;
						
				_packetBuffer[12+_cBBO+4+4+2] = mE;
				
				_packetBuffer[12+_cBBO+4+4+18] = highByte(mixRate);
				_packetBuffer[12+_cBBO+4+4+19] = lowByte(mixRate);
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Get Keyer On Air; Enabled
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			bool ATEMmax::getKeyerOnAirEnabled(uint8_t mE, uint8_t keyer) {
				return atemKeyerOnAirEnabled[mE][keyer];
			}
			
			/**
			 * Set Keyer On Air; Enabled
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 * enabled 	Bit 0: On/Off
			 */
			void ATEMmax::setKeyerOnAirEnabled(uint8_t mE, uint8_t keyer, bool enabled) {
			
	  	  		_prepareCommandPacket(PSTR("CKOn"),4,(_packetBuffer[12+_cBBO+4+4+0]==mE) && (_packetBuffer[12+_cBBO+4+4+1]==keyer));
		
				_packetBuffer[12+_cBBO+4+4+0] = mE;
				
				_packetBuffer[12+_cBBO+4+4+1] = keyer;
				
				_packetBuffer[12+_cBBO+4+4+2] = enabled;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Get Keyer Base; Type
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			uint8_t ATEMmax::getKeyerType(uint8_t mE, uint8_t keyer) {
				return atemKeyerType[mE][keyer];
			}
			
			/**
			 * Get Keyer Base; Fly Enabled
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			bool ATEMmax::getKeyerFlyEnabled(uint8_t mE, uint8_t keyer) {
				return atemKeyerFlyEnabled[mE][keyer];
			}
			
			/**
			 * Get Keyer Base; Fill Source
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			uint16_t ATEMmax::getKeyerFillSource(uint8_t mE, uint8_t keyer) {
				return atemKeyerFillSource[mE][keyer];
			}
			
			/**
			 * Get Keyer Base; Key Source
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			uint16_t ATEMmax::getKeyerKeySource(uint8_t mE, uint8_t keyer) {
				return atemKeyerKeySource[mE][keyer];
			}
			
			/**
			 * Get Keyer Base; Masked
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			bool ATEMmax::getKeyerMasked(uint8_t mE, uint8_t keyer) {
				return atemKeyerMasked[mE][keyer];
			}
			
			/**
			 * Get Keyer Base; Top
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			int16_t ATEMmax::getKeyerTop(uint8_t mE, uint8_t keyer) {
				return atemKeyerTop[mE][keyer];
			}
			
			/**
			 * Get Keyer Base; Bottom
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			int16_t ATEMmax::getKeyerBottom(uint8_t mE, uint8_t keyer) {
				return atemKeyerBottom[mE][keyer];
			}
			
			/**
			 * Get Keyer Base; Left
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			int16_t ATEMmax::getKeyerLeft(uint8_t mE, uint8_t keyer) {
				return atemKeyerLeft[mE][keyer];
			}
			
			/**
			 * Get Keyer Base; Right
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			int16_t ATEMmax::getKeyerRight(uint8_t mE, uint8_t keyer) {
				return atemKeyerRight[mE][keyer];
			}
			
			/**
			 * Set Key Type; Type
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 * type 	0: Luma, 1: Chroma, 2: Pattern, 3: DVE
			 */
			void ATEMmax::setKeyerType(uint8_t mE, uint8_t keyer, uint8_t type) {
			
	  	  		_prepareCommandPacket(PSTR("CKTp"),8,(_packetBuffer[12+_cBBO+4+4+1]==mE) && (_packetBuffer[12+_cBBO+4+4+2]==keyer));
		
					// Set Mask: 1
				_packetBuffer[12+_cBBO+4+4+0] |= 1;
						
				_packetBuffer[12+_cBBO+4+4+1] = mE;
				
				_packetBuffer[12+_cBBO+4+4+2] = keyer;
				
				_packetBuffer[12+_cBBO+4+4+3] = type;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Key Type; Fly Enabled
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 * flyEnabled 	Bit 0: On/Off
			 */
			void ATEMmax::setKeyerFlyEnabled(uint8_t mE, uint8_t keyer, bool flyEnabled) {
			
	  	  		_prepareCommandPacket(PSTR("CKTp"),8,(_packetBuffer[12+_cBBO+4+4+1]==mE) && (_packetBuffer[12+_cBBO+4+4+2]==keyer));
		
					// Set Mask: 2
				_packetBuffer[12+_cBBO+4+4+0] |= 2;
						
				_packetBuffer[12+_cBBO+4+4+1] = mE;
				
				_packetBuffer[12+_cBBO+4+4+2] = keyer;
				
				_packetBuffer[12+_cBBO+4+4+4] = flyEnabled;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Key Mask; Masked
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 * masked 	Bit 0: On/Off
			 */
			void ATEMmax::setKeyerMasked(uint8_t mE, uint8_t keyer, bool masked) {
			
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
			void ATEMmax::setKeyerTop(uint8_t mE, uint8_t keyer, int16_t top) {
			
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
			void ATEMmax::setKeyerBottom(uint8_t mE, uint8_t keyer, int16_t bottom) {
			
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
			void ATEMmax::setKeyerLeft(uint8_t mE, uint8_t keyer, int16_t left) {
			
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
			void ATEMmax::setKeyerRight(uint8_t mE, uint8_t keyer, int16_t right) {
			
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
			void ATEMmax::setKeyerFillSource(uint8_t mE, uint8_t keyer, uint16_t fillSource) {
			
	  	  		_prepareCommandPacket(PSTR("CKeF"),4,(_packetBuffer[12+_cBBO+4+4+0]==mE) && (_packetBuffer[12+_cBBO+4+4+1]==keyer));
		
				_packetBuffer[12+_cBBO+4+4+0] = mE;
				
				_packetBuffer[12+_cBBO+4+4+1] = keyer;
				
				_packetBuffer[12+_cBBO+4+4+2] = highByte(fillSource);
				_packetBuffer[12+_cBBO+4+4+3] = lowByte(fillSource);
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Key Cut; Key Source
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 * keySource 	(See video source list)
			 */
			void ATEMmax::setKeyerKeySource(uint8_t mE, uint8_t keyer, uint16_t keySource) {
			
	  	  		_prepareCommandPacket(PSTR("CKeC"),4,(_packetBuffer[12+_cBBO+4+4+0]==mE) && (_packetBuffer[12+_cBBO+4+4+1]==keyer));
		
				_packetBuffer[12+_cBBO+4+4+0] = mE;
				
				_packetBuffer[12+_cBBO+4+4+1] = keyer;
				
				_packetBuffer[12+_cBBO+4+4+2] = highByte(keySource);
				_packetBuffer[12+_cBBO+4+4+3] = lowByte(keySource);
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Get Key Luma; Pre Multiplied
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			bool ATEMmax::getKeyLumaPreMultiplied(uint8_t mE, uint8_t keyer) {
				return atemKeyLumaPreMultiplied[mE][keyer];
			}
			
			/**
			 * Get Key Luma; Clip
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			uint16_t ATEMmax::getKeyLumaClip(uint8_t mE, uint8_t keyer) {
				return atemKeyLumaClip[mE][keyer];
			}
			
			/**
			 * Get Key Luma; Gain
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			uint16_t ATEMmax::getKeyLumaGain(uint8_t mE, uint8_t keyer) {
				return atemKeyLumaGain[mE][keyer];
			}
			
			/**
			 * Get Key Luma; Invert Key
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			bool ATEMmax::getKeyLumaInvertKey(uint8_t mE, uint8_t keyer) {
				return atemKeyLumaInvertKey[mE][keyer];
			}
			
			/**
			 * Set Key Luma; Pre Multiplied
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 * preMultiplied 	Bit 0: On/Off
			 */
			void ATEMmax::setKeyLumaPreMultiplied(uint8_t mE, uint8_t keyer, bool preMultiplied) {
			
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
			void ATEMmax::setKeyLumaClip(uint8_t mE, uint8_t keyer, uint16_t clip) {
			
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
			void ATEMmax::setKeyLumaGain(uint8_t mE, uint8_t keyer, uint16_t gain) {
			
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
			void ATEMmax::setKeyLumaInvertKey(uint8_t mE, uint8_t keyer, bool invertKey) {
			
	  	  		_prepareCommandPacket(PSTR("CKLm"),12,(_packetBuffer[12+_cBBO+4+4+1]==mE) && (_packetBuffer[12+_cBBO+4+4+2]==keyer));
		
					// Set Mask: 8
				_packetBuffer[12+_cBBO+4+4+0] |= 8;
						
				_packetBuffer[12+_cBBO+4+4+1] = mE;
				
				_packetBuffer[12+_cBBO+4+4+2] = keyer;
				
				_packetBuffer[12+_cBBO+4+4+8] = invertKey;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Get Key Chroma; Hue
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			uint16_t ATEMmax::getKeyChromaHue(uint8_t mE, uint8_t keyer) {
				return atemKeyChromaHue[mE][keyer];
			}
			
			/**
			 * Get Key Chroma; Gain
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			uint16_t ATEMmax::getKeyChromaGain(uint8_t mE, uint8_t keyer) {
				return atemKeyChromaGain[mE][keyer];
			}
			
			/**
			 * Get Key Chroma; Y Suppress
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			uint16_t ATEMmax::getKeyChromaYSuppress(uint8_t mE, uint8_t keyer) {
				return atemKeyChromaYSuppress[mE][keyer];
			}
			
			/**
			 * Get Key Chroma; Lift
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			uint16_t ATEMmax::getKeyChromaLift(uint8_t mE, uint8_t keyer) {
				return atemKeyChromaLift[mE][keyer];
			}
			
			/**
			 * Get Key Chroma; Narrow
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			bool ATEMmax::getKeyChromaNarrow(uint8_t mE, uint8_t keyer) {
				return atemKeyChromaNarrow[mE][keyer];
			}
			
			/**
			 * Set Key Chroma; Hue
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 * hue 	0-3599: 0-359.9 Degrees
			 */
			void ATEMmax::setKeyChromaHue(uint8_t mE, uint8_t keyer, uint16_t hue) {
			
	  	  		_prepareCommandPacket(PSTR("CKCk"),16,(_packetBuffer[12+_cBBO+4+4+1]==mE) && (_packetBuffer[12+_cBBO+4+4+2]==keyer));
		
					// Set Mask: 1
				_packetBuffer[12+_cBBO+4+4+0] |= 1;
						
				_packetBuffer[12+_cBBO+4+4+1] = mE;
				
				_packetBuffer[12+_cBBO+4+4+2] = keyer;
				
				_packetBuffer[12+_cBBO+4+4+4] = highByte(hue);
				_packetBuffer[12+_cBBO+4+4+5] = lowByte(hue);
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Key Chroma; Gain
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 * gain 	0-1000: 0-100%
			 */
			void ATEMmax::setKeyChromaGain(uint8_t mE, uint8_t keyer, uint16_t gain) {
			
	  	  		_prepareCommandPacket(PSTR("CKCk"),16,(_packetBuffer[12+_cBBO+4+4+1]==mE) && (_packetBuffer[12+_cBBO+4+4+2]==keyer));
		
					// Set Mask: 2
				_packetBuffer[12+_cBBO+4+4+0] |= 2;
						
				_packetBuffer[12+_cBBO+4+4+1] = mE;
				
				_packetBuffer[12+_cBBO+4+4+2] = keyer;
				
				_packetBuffer[12+_cBBO+4+4+6] = highByte(gain);
				_packetBuffer[12+_cBBO+4+4+7] = lowByte(gain);
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Key Chroma; Y Suppress
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 * ySuppress 	0-1000: 0-100%
			 */
			void ATEMmax::setKeyChromaYSuppress(uint8_t mE, uint8_t keyer, uint16_t ySuppress) {
			
	  	  		_prepareCommandPacket(PSTR("CKCk"),16,(_packetBuffer[12+_cBBO+4+4+1]==mE) && (_packetBuffer[12+_cBBO+4+4+2]==keyer));
		
					// Set Mask: 4
				_packetBuffer[12+_cBBO+4+4+0] |= 4;
						
				_packetBuffer[12+_cBBO+4+4+1] = mE;
				
				_packetBuffer[12+_cBBO+4+4+2] = keyer;
				
				_packetBuffer[12+_cBBO+4+4+8] = highByte(ySuppress);
				_packetBuffer[12+_cBBO+4+4+9] = lowByte(ySuppress);
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Key Chroma; Lift
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 * lift 	0-1000: 0-100%
			 */
			void ATEMmax::setKeyChromaLift(uint8_t mE, uint8_t keyer, uint16_t lift) {
			
	  	  		_prepareCommandPacket(PSTR("CKCk"),16,(_packetBuffer[12+_cBBO+4+4+1]==mE) && (_packetBuffer[12+_cBBO+4+4+2]==keyer));
		
					// Set Mask: 8
				_packetBuffer[12+_cBBO+4+4+0] |= 8;
						
				_packetBuffer[12+_cBBO+4+4+1] = mE;
				
				_packetBuffer[12+_cBBO+4+4+2] = keyer;
				
				_packetBuffer[12+_cBBO+4+4+10] = highByte(lift);
				_packetBuffer[12+_cBBO+4+4+11] = lowByte(lift);
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Key Chroma; Narrow
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 * narrow 	Bit 0: On/Off
			 */
			void ATEMmax::setKeyChromaNarrow(uint8_t mE, uint8_t keyer, bool narrow) {
			
	  	  		_prepareCommandPacket(PSTR("CKCk"),16,(_packetBuffer[12+_cBBO+4+4+1]==mE) && (_packetBuffer[12+_cBBO+4+4+2]==keyer));
		
					// Set Mask: 16
				_packetBuffer[12+_cBBO+4+4+0] |= 16;
						
				_packetBuffer[12+_cBBO+4+4+1] = mE;
				
				_packetBuffer[12+_cBBO+4+4+2] = keyer;
				
				_packetBuffer[12+_cBBO+4+4+12] = narrow;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Get Key Pattern; Pattern
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			uint8_t ATEMmax::getKeyPatternPattern(uint8_t mE, uint8_t keyer) {
				return atemKeyPatternPattern[mE][keyer];
			}
			
			/**
			 * Get Key Pattern; Size
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			uint16_t ATEMmax::getKeyPatternSize(uint8_t mE, uint8_t keyer) {
				return atemKeyPatternSize[mE][keyer];
			}
			
			/**
			 * Get Key Pattern; Symmetry
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			uint16_t ATEMmax::getKeyPatternSymmetry(uint8_t mE, uint8_t keyer) {
				return atemKeyPatternSymmetry[mE][keyer];
			}
			
			/**
			 * Get Key Pattern; Softness
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			uint16_t ATEMmax::getKeyPatternSoftness(uint8_t mE, uint8_t keyer) {
				return atemKeyPatternSoftness[mE][keyer];
			}
			
			/**
			 * Get Key Pattern; Position X
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			uint16_t ATEMmax::getKeyPatternPositionX(uint8_t mE, uint8_t keyer) {
				return atemKeyPatternPositionX[mE][keyer];
			}
			
			/**
			 * Get Key Pattern; Position Y
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			uint16_t ATEMmax::getKeyPatternPositionY(uint8_t mE, uint8_t keyer) {
				return atemKeyPatternPositionY[mE][keyer];
			}
			
			/**
			 * Get Key Pattern; Invert Pattern
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			bool ATEMmax::getKeyPatternInvertPattern(uint8_t mE, uint8_t keyer) {
				return atemKeyPatternInvertPattern[mE][keyer];
			}
			
			/**
			 * Set Key Pattern; Pattern
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 * pattern 	0-17: Pattern Styles
			 */
			void ATEMmax::setKeyPatternPattern(uint8_t mE, uint8_t keyer, uint8_t pattern) {
			
	  	  		_prepareCommandPacket(PSTR("CKPt"),16,(_packetBuffer[12+_cBBO+4+4+1]==mE) && (_packetBuffer[12+_cBBO+4+4+2]==keyer));
		
					// Set Mask: 1
				_packetBuffer[12+_cBBO+4+4+0] |= 1;
						
				_packetBuffer[12+_cBBO+4+4+1] = mE;
				
				_packetBuffer[12+_cBBO+4+4+2] = keyer;
				
				_packetBuffer[12+_cBBO+4+4+3] = pattern;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Key Pattern; Size
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 * size 	0-10000: 0-100%
			 */
			void ATEMmax::setKeyPatternSize(uint8_t mE, uint8_t keyer, uint16_t size) {
			
	  	  		_prepareCommandPacket(PSTR("CKPt"),16,(_packetBuffer[12+_cBBO+4+4+1]==mE) && (_packetBuffer[12+_cBBO+4+4+2]==keyer));
		
					// Set Mask: 2
				_packetBuffer[12+_cBBO+4+4+0] |= 2;
						
				_packetBuffer[12+_cBBO+4+4+1] = mE;
				
				_packetBuffer[12+_cBBO+4+4+2] = keyer;
				
				_packetBuffer[12+_cBBO+4+4+4] = highByte(size);
				_packetBuffer[12+_cBBO+4+4+5] = lowByte(size);
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Key Pattern; Symmetry
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 * symmetry 	0-10000: 0-100%
			 */
			void ATEMmax::setKeyPatternSymmetry(uint8_t mE, uint8_t keyer, uint16_t symmetry) {
			
	  	  		_prepareCommandPacket(PSTR("CKPt"),16,(_packetBuffer[12+_cBBO+4+4+1]==mE) && (_packetBuffer[12+_cBBO+4+4+2]==keyer));
		
					// Set Mask: 4
				_packetBuffer[12+_cBBO+4+4+0] |= 4;
						
				_packetBuffer[12+_cBBO+4+4+1] = mE;
				
				_packetBuffer[12+_cBBO+4+4+2] = keyer;
				
				_packetBuffer[12+_cBBO+4+4+6] = highByte(symmetry);
				_packetBuffer[12+_cBBO+4+4+7] = lowByte(symmetry);
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Key Pattern; Softness
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 * softness 	0-10000: 0-100%
			 */
			void ATEMmax::setKeyPatternSoftness(uint8_t mE, uint8_t keyer, uint16_t softness) {
			
	  	  		_prepareCommandPacket(PSTR("CKPt"),16,(_packetBuffer[12+_cBBO+4+4+1]==mE) && (_packetBuffer[12+_cBBO+4+4+2]==keyer));
		
					// Set Mask: 8
				_packetBuffer[12+_cBBO+4+4+0] |= 8;
						
				_packetBuffer[12+_cBBO+4+4+1] = mE;
				
				_packetBuffer[12+_cBBO+4+4+2] = keyer;
				
				_packetBuffer[12+_cBBO+4+4+8] = highByte(softness);
				_packetBuffer[12+_cBBO+4+4+9] = lowByte(softness);
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Key Pattern; Position X
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 * positionX 	0-10000: 0.0-1.0
			 */
			void ATEMmax::setKeyPatternPositionX(uint8_t mE, uint8_t keyer, uint16_t positionX) {
			
	  	  		_prepareCommandPacket(PSTR("CKPt"),16,(_packetBuffer[12+_cBBO+4+4+1]==mE) && (_packetBuffer[12+_cBBO+4+4+2]==keyer));
		
					// Set Mask: 16
				_packetBuffer[12+_cBBO+4+4+0] |= 16;
						
				_packetBuffer[12+_cBBO+4+4+1] = mE;
				
				_packetBuffer[12+_cBBO+4+4+2] = keyer;
				
				_packetBuffer[12+_cBBO+4+4+10] = highByte(positionX);
				_packetBuffer[12+_cBBO+4+4+11] = lowByte(positionX);
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Key Pattern; Position Y
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 * positionY 	0-10000: 0.0-1.0
			 */
			void ATEMmax::setKeyPatternPositionY(uint8_t mE, uint8_t keyer, uint16_t positionY) {
			
	  	  		_prepareCommandPacket(PSTR("CKPt"),16,(_packetBuffer[12+_cBBO+4+4+1]==mE) && (_packetBuffer[12+_cBBO+4+4+2]==keyer));
		
					// Set Mask: 32
				_packetBuffer[12+_cBBO+4+4+0] |= 32;
						
				_packetBuffer[12+_cBBO+4+4+1] = mE;
				
				_packetBuffer[12+_cBBO+4+4+2] = keyer;
				
				_packetBuffer[12+_cBBO+4+4+12] = highByte(positionY);
				_packetBuffer[12+_cBBO+4+4+13] = lowByte(positionY);
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Key Pattern; Invert Pattern
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 * invertPattern 	Bit 0: On/Off
			 */
			void ATEMmax::setKeyPatternInvertPattern(uint8_t mE, uint8_t keyer, bool invertPattern) {
			
	  	  		_prepareCommandPacket(PSTR("CKPt"),16,(_packetBuffer[12+_cBBO+4+4+1]==mE) && (_packetBuffer[12+_cBBO+4+4+2]==keyer));
		
					// Set Mask: 64
				_packetBuffer[12+_cBBO+4+4+0] |= 64;
						
				_packetBuffer[12+_cBBO+4+4+1] = mE;
				
				_packetBuffer[12+_cBBO+4+4+2] = keyer;
				
				_packetBuffer[12+_cBBO+4+4+14] = invertPattern;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Get Key DVE; Size X
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			int32_t ATEMmax::getKeyDVESizeX(uint8_t mE, uint8_t keyer) {
				return atemKeyDVESizeX[mE][keyer];
			}
			
			/**
			 * Get Key DVE; Size Y
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			int32_t ATEMmax::getKeyDVESizeY(uint8_t mE, uint8_t keyer) {
				return atemKeyDVESizeY[mE][keyer];
			}
			
			/**
			 * Get Key DVE; Position X
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			int32_t ATEMmax::getKeyDVEPositionX(uint8_t mE, uint8_t keyer) {
				return atemKeyDVEPositionX[mE][keyer];
			}
			
			/**
			 * Get Key DVE; Position Y
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			int32_t ATEMmax::getKeyDVEPositionY(uint8_t mE, uint8_t keyer) {
				return atemKeyDVEPositionY[mE][keyer];
			}
			
			/**
			 * Get Key DVE; Rotation
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			int32_t ATEMmax::getKeyDVERotation(uint8_t mE, uint8_t keyer) {
				return atemKeyDVERotation[mE][keyer];
			}
			
			/**
			 * Get Key DVE; Border Enabled
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			bool ATEMmax::getKeyDVEBorderEnabled(uint8_t mE, uint8_t keyer) {
				return atemKeyDVEBorderEnabled[mE][keyer];
			}
			
			/**
			 * Get Key DVE; Shadow
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			bool ATEMmax::getKeyDVEShadow(uint8_t mE, uint8_t keyer) {
				return atemKeyDVEShadow[mE][keyer];
			}
			
			/**
			 * Get Key DVE; Border Bevel
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			uint8_t ATEMmax::getKeyDVEBorderBevel(uint8_t mE, uint8_t keyer) {
				return atemKeyDVEBorderBevel[mE][keyer];
			}
			
			/**
			 * Get Key DVE; Border Outer Width
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			uint16_t ATEMmax::getKeyDVEBorderOuterWidth(uint8_t mE, uint8_t keyer) {
				return atemKeyDVEBorderOuterWidth[mE][keyer];
			}
			
			/**
			 * Get Key DVE; Border Inner Width
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			uint16_t ATEMmax::getKeyDVEBorderInnerWidth(uint8_t mE, uint8_t keyer) {
				return atemKeyDVEBorderInnerWidth[mE][keyer];
			}
			
			/**
			 * Get Key DVE; Border Outer Softness
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			uint8_t ATEMmax::getKeyDVEBorderOuterSoftness(uint8_t mE, uint8_t keyer) {
				return atemKeyDVEBorderOuterSoftness[mE][keyer];
			}
			
			/**
			 * Get Key DVE; Border Inner Softness
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			uint8_t ATEMmax::getKeyDVEBorderInnerSoftness(uint8_t mE, uint8_t keyer) {
				return atemKeyDVEBorderInnerSoftness[mE][keyer];
			}
			
			/**
			 * Get Key DVE; Border Bevel Softness
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			uint8_t ATEMmax::getKeyDVEBorderBevelSoftness(uint8_t mE, uint8_t keyer) {
				return atemKeyDVEBorderBevelSoftness[mE][keyer];
			}
			
			/**
			 * Get Key DVE; Border Bevel Position
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			uint8_t ATEMmax::getKeyDVEBorderBevelPosition(uint8_t mE, uint8_t keyer) {
				return atemKeyDVEBorderBevelPosition[mE][keyer];
			}
			
			/**
			 * Get Key DVE; Border Opacity
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			uint8_t ATEMmax::getKeyDVEBorderOpacity(uint8_t mE, uint8_t keyer) {
				return atemKeyDVEBorderOpacity[mE][keyer];
			}
			
			/**
			 * Get Key DVE; Border Hue
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			uint16_t ATEMmax::getKeyDVEBorderHue(uint8_t mE, uint8_t keyer) {
				return atemKeyDVEBorderHue[mE][keyer];
			}
			
			/**
			 * Get Key DVE; Border Saturation
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			uint16_t ATEMmax::getKeyDVEBorderSaturation(uint8_t mE, uint8_t keyer) {
				return atemKeyDVEBorderSaturation[mE][keyer];
			}
			
			/**
			 * Get Key DVE; Border Luma
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			uint16_t ATEMmax::getKeyDVEBorderLuma(uint8_t mE, uint8_t keyer) {
				return atemKeyDVEBorderLuma[mE][keyer];
			}
			
			/**
			 * Get Key DVE; Light Source Direction
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			uint16_t ATEMmax::getKeyDVELightSourceDirection(uint8_t mE, uint8_t keyer) {
				return atemKeyDVELightSourceDirection[mE][keyer];
			}
			
			/**
			 * Get Key DVE; Light Source Altitude
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			uint8_t ATEMmax::getKeyDVELightSourceAltitude(uint8_t mE, uint8_t keyer) {
				return atemKeyDVELightSourceAltitude[mE][keyer];
			}
			
			/**
			 * Get Key DVE; Masked
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			bool ATEMmax::getKeyDVEMasked(uint8_t mE, uint8_t keyer) {
				return atemKeyDVEMasked[mE][keyer];
			}
			
			/**
			 * Get Key DVE; Top
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			int16_t ATEMmax::getKeyDVETop(uint8_t mE, uint8_t keyer) {
				return atemKeyDVETop[mE][keyer];
			}
			
			/**
			 * Get Key DVE; Bottom
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			int16_t ATEMmax::getKeyDVEBottom(uint8_t mE, uint8_t keyer) {
				return atemKeyDVEBottom[mE][keyer];
			}
			
			/**
			 * Get Key DVE; Left
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			int16_t ATEMmax::getKeyDVELeft(uint8_t mE, uint8_t keyer) {
				return atemKeyDVELeft[mE][keyer];
			}
			
			/**
			 * Get Key DVE; Right
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			int16_t ATEMmax::getKeyDVERight(uint8_t mE, uint8_t keyer) {
				return atemKeyDVERight[mE][keyer];
			}
			
			/**
			 * Get Key DVE; Rate
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			uint8_t ATEMmax::getKeyDVERate(uint8_t mE, uint8_t keyer) {
				return atemKeyDVERate[mE][keyer];
			}
			
			/**
			 * Set Key DVE; Size X
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 * sizeX 	Example: 1000: 1.000
			 */
			void ATEMmax::setKeyDVESizeX(uint8_t mE, uint8_t keyer, int32_t sizeX) {
			
	  	  		_prepareCommandPacket(PSTR("CKDV"),64,(_packetBuffer[12+_cBBO+4+4+4]==mE) && (_packetBuffer[12+_cBBO+4+4+5]==keyer));
		
					// Set Mask: 1
				_packetBuffer[12+_cBBO+4+4+3] |= 1;
						
				_packetBuffer[12+_cBBO+4+4+4] = mE;
				
				_packetBuffer[12+_cBBO+4+4+5] = keyer;
				
				_packetBuffer[12+_cBBO+4+4+8] = (int32_t)((sizeX>>24) & 0xFF);
				_packetBuffer[12+_cBBO+4+4+9] = (int32_t)((sizeX>>16) & 0xFF);
				_packetBuffer[12+_cBBO+4+4+10] = (int32_t)((sizeX>>8) & 0xFF);
				_packetBuffer[12+_cBBO+4+4+11] = (int32_t)(sizeX & 0xFF);
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Key DVE; Size Y
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 * sizeY 	Example: 2000: 2.000
			 */
			void ATEMmax::setKeyDVESizeY(uint8_t mE, uint8_t keyer, int32_t sizeY) {
			
	  	  		_prepareCommandPacket(PSTR("CKDV"),64,(_packetBuffer[12+_cBBO+4+4+4]==mE) && (_packetBuffer[12+_cBBO+4+4+5]==keyer));
		
					// Set Mask: 2
				_packetBuffer[12+_cBBO+4+4+3] |= 2;
						
				_packetBuffer[12+_cBBO+4+4+4] = mE;
				
				_packetBuffer[12+_cBBO+4+4+5] = keyer;
				
				_packetBuffer[12+_cBBO+4+4+12] = (int32_t)((sizeY>>24) & 0xFF);
				_packetBuffer[12+_cBBO+4+4+13] = (int32_t)((sizeY>>16) & 0xFF);
				_packetBuffer[12+_cBBO+4+4+14] = (int32_t)((sizeY>>8) & 0xFF);
				_packetBuffer[12+_cBBO+4+4+15] = (int32_t)(sizeY & 0xFF);
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Key DVE; Position X
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 * positionX 	Example: 1000: 1.000
			 */
			void ATEMmax::setKeyDVEPositionX(uint8_t mE, uint8_t keyer, int32_t positionX) {
			
	  	  		_prepareCommandPacket(PSTR("CKDV"),64,(_packetBuffer[12+_cBBO+4+4+4]==mE) && (_packetBuffer[12+_cBBO+4+4+5]==keyer));
		
					// Set Mask: 4
				_packetBuffer[12+_cBBO+4+4+3] |= 4;
						
				_packetBuffer[12+_cBBO+4+4+4] = mE;
				
				_packetBuffer[12+_cBBO+4+4+5] = keyer;
				
				_packetBuffer[12+_cBBO+4+4+16] = (int32_t)((positionX>>24) & 0xFF);
				_packetBuffer[12+_cBBO+4+4+17] = (int32_t)((positionX>>16) & 0xFF);
				_packetBuffer[12+_cBBO+4+4+18] = (int32_t)((positionX>>8) & 0xFF);
				_packetBuffer[12+_cBBO+4+4+19] = (int32_t)(positionX & 0xFF);
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Key DVE; Position Y
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 * positionY 	Example: -1000: -1.000
			 */
			void ATEMmax::setKeyDVEPositionY(uint8_t mE, uint8_t keyer, int32_t positionY) {
			
	  	  		_prepareCommandPacket(PSTR("CKDV"),64,(_packetBuffer[12+_cBBO+4+4+4]==mE) && (_packetBuffer[12+_cBBO+4+4+5]==keyer));
		
					// Set Mask: 8
				_packetBuffer[12+_cBBO+4+4+3] |= 8;
						
				_packetBuffer[12+_cBBO+4+4+4] = mE;
				
				_packetBuffer[12+_cBBO+4+4+5] = keyer;
				
				_packetBuffer[12+_cBBO+4+4+20] = (int32_t)((positionY>>24) & 0xFF);
				_packetBuffer[12+_cBBO+4+4+21] = (int32_t)((positionY>>16) & 0xFF);
				_packetBuffer[12+_cBBO+4+4+22] = (int32_t)((positionY>>8) & 0xFF);
				_packetBuffer[12+_cBBO+4+4+23] = (int32_t)(positionY & 0xFF);
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Key DVE; Rotation
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 * rotation 	Example: 3670: 1 rotation+7 degress
			 */
			void ATEMmax::setKeyDVERotation(uint8_t mE, uint8_t keyer, int32_t rotation) {
			
	  	  		_prepareCommandPacket(PSTR("CKDV"),64,(_packetBuffer[12+_cBBO+4+4+4]==mE) && (_packetBuffer[12+_cBBO+4+4+5]==keyer));
		
					// Set Mask: 16
				_packetBuffer[12+_cBBO+4+4+3] |= 16;
						
				_packetBuffer[12+_cBBO+4+4+4] = mE;
				
				_packetBuffer[12+_cBBO+4+4+5] = keyer;
				
				_packetBuffer[12+_cBBO+4+4+24] = (int32_t)((rotation>>24) & 0xFF);
				_packetBuffer[12+_cBBO+4+4+25] = (int32_t)((rotation>>16) & 0xFF);
				_packetBuffer[12+_cBBO+4+4+26] = (int32_t)((rotation>>8) & 0xFF);
				_packetBuffer[12+_cBBO+4+4+27] = (int32_t)(rotation & 0xFF);
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Key DVE; Border Enabled
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 * borderEnabled 	Bit 0: On/Off
			 */
			void ATEMmax::setKeyDVEBorderEnabled(uint8_t mE, uint8_t keyer, bool borderEnabled) {
			
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
			void ATEMmax::setKeyDVEShadow(uint8_t mE, uint8_t keyer, bool shadow) {
			
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
			void ATEMmax::setKeyDVEBorderBevel(uint8_t mE, uint8_t keyer, uint8_t borderBevel) {
			
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
			void ATEMmax::setKeyDVEBorderOuterWidth(uint8_t mE, uint8_t keyer, uint16_t borderOuterWidth) {
			
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
			void ATEMmax::setKeyDVEBorderInnerWidth(uint8_t mE, uint8_t keyer, uint16_t borderInnerWidth) {
			
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
			void ATEMmax::setKeyDVEBorderOuterSoftness(uint8_t mE, uint8_t keyer, uint8_t borderOuterSoftness) {
			
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
			void ATEMmax::setKeyDVEBorderInnerSoftness(uint8_t mE, uint8_t keyer, uint8_t borderInnerSoftness) {
			
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
			void ATEMmax::setKeyDVEBorderBevelSoftness(uint8_t mE, uint8_t keyer, uint8_t borderBevelSoftness) {
			
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
			void ATEMmax::setKeyDVEBorderBevelPosition(uint8_t mE, uint8_t keyer, uint8_t borderBevelPosition) {
			
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
			void ATEMmax::setKeyDVEBorderOpacity(uint8_t mE, uint8_t keyer, uint8_t borderOpacity) {
			
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
			void ATEMmax::setKeyDVEBorderHue(uint8_t mE, uint8_t keyer, uint16_t borderHue) {
			
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
			void ATEMmax::setKeyDVEBorderSaturation(uint8_t mE, uint8_t keyer, uint16_t borderSaturation) {
			
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
			void ATEMmax::setKeyDVEBorderLuma(uint8_t mE, uint8_t keyer, uint16_t borderLuma) {
			
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
			void ATEMmax::setKeyDVELightSourceDirection(uint8_t mE, uint8_t keyer, uint16_t lightSourceDirection) {
			
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
			void ATEMmax::setKeyDVELightSourceAltitude(uint8_t mE, uint8_t keyer, uint8_t lightSourceAltitude) {
			
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
			void ATEMmax::setKeyDVEMasked(uint8_t mE, uint8_t keyer, bool masked) {
			
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
			void ATEMmax::setKeyDVETop(uint8_t mE, uint8_t keyer, int16_t top) {
			
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
			void ATEMmax::setKeyDVEBottom(uint8_t mE, uint8_t keyer, int16_t bottom) {
			
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
			void ATEMmax::setKeyDVELeft(uint8_t mE, uint8_t keyer, int16_t left) {
			
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
			void ATEMmax::setKeyDVERight(uint8_t mE, uint8_t keyer, int16_t right) {
			
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
			void ATEMmax::setKeyDVERate(uint8_t mE, uint8_t keyer, uint8_t rate) {
			
	  	  		_prepareCommandPacket(PSTR("CKDV"),64,(_packetBuffer[12+_cBBO+4+4+4]==mE) && (_packetBuffer[12+_cBBO+4+4+5]==keyer));
		
					// Set Mask: 33554432
				_packetBuffer[12+_cBBO+4+4+0] |= 2;
						
				_packetBuffer[12+_cBBO+4+4+4] = mE;
				
				_packetBuffer[12+_cBBO+4+4+5] = keyer;
				
				_packetBuffer[12+_cBBO+4+4+60] = rate;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Get Keyer Fly; IsASet
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			bool ATEMmax::getKeyerFlyIsASet(uint8_t mE, uint8_t keyer) {
				return atemKeyerFlyIsASet[mE][keyer];
			}
			
			/**
			 * Get Keyer Fly; IsBSet
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			bool ATEMmax::getKeyerFlyIsBSet(uint8_t mE, uint8_t keyer) {
				return atemKeyerFlyIsBSet[mE][keyer];
			}
			
			/**
			 * Get Keyer Fly; Is At Key Frame
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			uint8_t ATEMmax::getKeyerFlyIsAtKeyFrame(uint8_t mE, uint8_t keyer) {
				return atemKeyerFlyIsAtKeyFrame[mE][keyer];
			}
			
			/**
			 * Get Keyer Fly; Run-to-Infinite-index
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			uint8_t ATEMmax::getKeyerFlyRuntoInfiniteindex(uint8_t mE, uint8_t keyer) {
				return atemKeyerFlyRuntoInfiniteindex[mE][keyer];
			}
			
			/**
			 * Set Keyer Fly; Key Frame
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 * keyFrame 	1: A, 2: B
			 */
			void ATEMmax::setKeyerFlyKeyFrame(uint8_t mE, uint8_t keyer, uint8_t keyFrame) {
			
	  	  		_prepareCommandPacket(PSTR("SFKF"),4,(_packetBuffer[12+_cBBO+4+4+0]==mE) && (_packetBuffer[12+_cBBO+4+4+1]==keyer));
		
				_packetBuffer[12+_cBBO+4+4+0] = mE;
				
				_packetBuffer[12+_cBBO+4+4+1] = keyer;
				
				_packetBuffer[12+_cBBO+4+4+2] = keyFrame;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Run Flying Key; Key Frame
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 * keyFrame 	1: A, 2: B, 3: Full, 4: Run-To-Infinite
			 */
			void ATEMmax::setRunFlyingKeyKeyFrame(uint8_t mE, uint8_t keyer, uint8_t keyFrame) {
			
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
			void ATEMmax::setRunFlyingKeyRuntoInfiniteindex(uint8_t mE, uint8_t keyer, uint8_t runtoInfiniteindex) {
			
	  	  		_prepareCommandPacket(PSTR("RFlK"),8,(_packetBuffer[12+_cBBO+4+4+1]==mE) && (_packetBuffer[12+_cBBO+4+4+2]==keyer));
		
					// Set Mask: 2
				_packetBuffer[12+_cBBO+4+4+0] |= 2;
						
				_packetBuffer[12+_cBBO+4+4+1] = mE;
				
				_packetBuffer[12+_cBBO+4+4+2] = keyer;
				
				_packetBuffer[12+_cBBO+4+4+5] = runtoInfiniteindex;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Get Keyer Fly Key Frame; Size X
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 * keyFrame 	1: A, 2: B
			 */
			int32_t ATEMmax::getKeyerFlyKeyFrameSizeX(uint8_t mE, uint8_t keyer, uint8_t keyFrame) {
				return atemKeyerFlyKeyFrameSizeX[mE][keyer][keyFrame];
			}
			
			/**
			 * Get Keyer Fly Key Frame; Size Y
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 * keyFrame 	1: A, 2: B
			 */
			int32_t ATEMmax::getKeyerFlyKeyFrameSizeY(uint8_t mE, uint8_t keyer, uint8_t keyFrame) {
				return atemKeyerFlyKeyFrameSizeY[mE][keyer][keyFrame];
			}
			
			/**
			 * Get Keyer Fly Key Frame; Position X
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 * keyFrame 	1: A, 2: B
			 */
			int32_t ATEMmax::getKeyerFlyKeyFramePositionX(uint8_t mE, uint8_t keyer, uint8_t keyFrame) {
				return atemKeyerFlyKeyFramePositionX[mE][keyer][keyFrame];
			}
			
			/**
			 * Get Keyer Fly Key Frame; Position Y
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 * keyFrame 	1: A, 2: B
			 */
			int32_t ATEMmax::getKeyerFlyKeyFramePositionY(uint8_t mE, uint8_t keyer, uint8_t keyFrame) {
				return atemKeyerFlyKeyFramePositionY[mE][keyer][keyFrame];
			}
			
			/**
			 * Get Keyer Fly Key Frame; Rotation
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 * keyFrame 	1: A, 2: B
			 */
			int32_t ATEMmax::getKeyerFlyKeyFrameRotation(uint8_t mE, uint8_t keyer, uint8_t keyFrame) {
				return atemKeyerFlyKeyFrameRotation[mE][keyer][keyFrame];
			}
			
			/**
			 * Get Keyer Fly Key Frame; Border Outer Width
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 * keyFrame 	1: A, 2: B
			 */
			uint16_t ATEMmax::getKeyerFlyKeyFrameBorderOuterWidth(uint8_t mE, uint8_t keyer, uint8_t keyFrame) {
				return atemKeyerFlyKeyFrameBorderOuterWidth[mE][keyer][keyFrame];
			}
			
			/**
			 * Get Keyer Fly Key Frame; Border Inner Width
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 * keyFrame 	1: A, 2: B
			 */
			uint16_t ATEMmax::getKeyerFlyKeyFrameBorderInnerWidth(uint8_t mE, uint8_t keyer, uint8_t keyFrame) {
				return atemKeyerFlyKeyFrameBorderInnerWidth[mE][keyer][keyFrame];
			}
			
			/**
			 * Get Keyer Fly Key Frame; Border Outer Softness
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 * keyFrame 	1: A, 2: B
			 */
			uint8_t ATEMmax::getKeyerFlyKeyFrameBorderOuterSoftness(uint8_t mE, uint8_t keyer, uint8_t keyFrame) {
				return atemKeyerFlyKeyFrameBorderOuterSoftness[mE][keyer][keyFrame];
			}
			
			/**
			 * Get Keyer Fly Key Frame; Border Inner Softness
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 * keyFrame 	1: A, 2: B
			 */
			uint8_t ATEMmax::getKeyerFlyKeyFrameBorderInnerSoftness(uint8_t mE, uint8_t keyer, uint8_t keyFrame) {
				return atemKeyerFlyKeyFrameBorderInnerSoftness[mE][keyer][keyFrame];
			}
			
			/**
			 * Get Keyer Fly Key Frame; Border Bevel Softness
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 * keyFrame 	1: A, 2: B
			 */
			uint8_t ATEMmax::getKeyerFlyKeyFrameBorderBevelSoftness(uint8_t mE, uint8_t keyer, uint8_t keyFrame) {
				return atemKeyerFlyKeyFrameBorderBevelSoftness[mE][keyer][keyFrame];
			}
			
			/**
			 * Get Keyer Fly Key Frame; Border Bevel Position
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 * keyFrame 	1: A, 2: B
			 */
			uint8_t ATEMmax::getKeyerFlyKeyFrameBorderBevelPosition(uint8_t mE, uint8_t keyer, uint8_t keyFrame) {
				return atemKeyerFlyKeyFrameBorderBevelPosition[mE][keyer][keyFrame];
			}
			
			/**
			 * Get Keyer Fly Key Frame; Border Opacity
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 * keyFrame 	1: A, 2: B
			 */
			uint8_t ATEMmax::getKeyerFlyKeyFrameBorderOpacity(uint8_t mE, uint8_t keyer, uint8_t keyFrame) {
				return atemKeyerFlyKeyFrameBorderOpacity[mE][keyer][keyFrame];
			}
			
			/**
			 * Get Keyer Fly Key Frame; Border Hue
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 * keyFrame 	1: A, 2: B
			 */
			uint16_t ATEMmax::getKeyerFlyKeyFrameBorderHue(uint8_t mE, uint8_t keyer, uint8_t keyFrame) {
				return atemKeyerFlyKeyFrameBorderHue[mE][keyer][keyFrame];
			}
			
			/**
			 * Get Keyer Fly Key Frame; Border Saturation
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 * keyFrame 	1: A, 2: B
			 */
			uint16_t ATEMmax::getKeyerFlyKeyFrameBorderSaturation(uint8_t mE, uint8_t keyer, uint8_t keyFrame) {
				return atemKeyerFlyKeyFrameBorderSaturation[mE][keyer][keyFrame];
			}
			
			/**
			 * Get Keyer Fly Key Frame; Border Luma
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 * keyFrame 	1: A, 2: B
			 */
			uint16_t ATEMmax::getKeyerFlyKeyFrameBorderLuma(uint8_t mE, uint8_t keyer, uint8_t keyFrame) {
				return atemKeyerFlyKeyFrameBorderLuma[mE][keyer][keyFrame];
			}
			
			/**
			 * Get Keyer Fly Key Frame; Light Source Direction
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 * keyFrame 	1: A, 2: B
			 */
			uint16_t ATEMmax::getKeyerFlyKeyFrameLightSourceDirection(uint8_t mE, uint8_t keyer, uint8_t keyFrame) {
				return atemKeyerFlyKeyFrameLightSourceDirection[mE][keyer][keyFrame];
			}
			
			/**
			 * Get Keyer Fly Key Frame; Light Source Altitude
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 * keyFrame 	1: A, 2: B
			 */
			uint8_t ATEMmax::getKeyerFlyKeyFrameLightSourceAltitude(uint8_t mE, uint8_t keyer, uint8_t keyFrame) {
				return atemKeyerFlyKeyFrameLightSourceAltitude[mE][keyer][keyFrame];
			}
			
			/**
			 * Get Keyer Fly Key Frame; Top
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 * keyFrame 	1: A, 2: B
			 */
			int16_t ATEMmax::getKeyerFlyKeyFrameTop(uint8_t mE, uint8_t keyer, uint8_t keyFrame) {
				return atemKeyerFlyKeyFrameTop[mE][keyer][keyFrame];
			}
			
			/**
			 * Get Keyer Fly Key Frame; Bottom
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 * keyFrame 	1: A, 2: B
			 */
			int16_t ATEMmax::getKeyerFlyKeyFrameBottom(uint8_t mE, uint8_t keyer, uint8_t keyFrame) {
				return atemKeyerFlyKeyFrameBottom[mE][keyer][keyFrame];
			}
			
			/**
			 * Get Keyer Fly Key Frame; Left
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 * keyFrame 	1: A, 2: B
			 */
			int16_t ATEMmax::getKeyerFlyKeyFrameLeft(uint8_t mE, uint8_t keyer, uint8_t keyFrame) {
				return atemKeyerFlyKeyFrameLeft[mE][keyer][keyFrame];
			}
			
			/**
			 * Get Keyer Fly Key Frame; Right
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 * keyFrame 	1: A, 2: B
			 */
			int16_t ATEMmax::getKeyerFlyKeyFrameRight(uint8_t mE, uint8_t keyer, uint8_t keyFrame) {
				return atemKeyerFlyKeyFrameRight[mE][keyer][keyFrame];
			}
			
			/**
			 * Get Downstream Keyer; Fill Source
			 * keyer 	0: DSK1, 1: DSK2
			 */
			uint16_t ATEMmax::getDownstreamKeyerFillSource(uint8_t keyer) {
				return atemDownstreamKeyerFillSource[keyer];
			}
			
			/**
			 * Get Downstream Keyer; Key Source
			 * keyer 	0: DSK1, 1: DSK2
			 */
			uint16_t ATEMmax::getDownstreamKeyerKeySource(uint8_t keyer) {
				return atemDownstreamKeyerKeySource[keyer];
			}
			
			/**
			 * Set Downstream Keyer; Fill Source
			 * keyer 	0-3: Keyer 1-4
			 * fillSource 	(See video source list)
			 */
			void ATEMmax::setDownstreamKeyerFillSource(uint8_t keyer, uint16_t fillSource) {
			
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
			void ATEMmax::setDownstreamKeyerKeySource(uint8_t keyer, uint16_t keySource) {
			
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
			bool ATEMmax::getDownstreamKeyerTie(uint8_t keyer) {
				return atemDownstreamKeyerTie[keyer];
			}
			
			/**
			 * Get Downstream Keyer; Rate
			 * keyer 	0: DSK1, 1: DSK2
			 */
			uint8_t ATEMmax::getDownstreamKeyerRate(uint8_t keyer) {
				return atemDownstreamKeyerRate[keyer];
			}
			
			/**
			 * Get Downstream Keyer; Pre Multiplied
			 * keyer 	0: DSK1, 1: DSK2
			 */
			bool ATEMmax::getDownstreamKeyerPreMultiplied(uint8_t keyer) {
				return atemDownstreamKeyerPreMultiplied[keyer];
			}
			
			/**
			 * Get Downstream Keyer; Clip
			 * keyer 	0: DSK1, 1: DSK2
			 */
			uint16_t ATEMmax::getDownstreamKeyerClip(uint8_t keyer) {
				return atemDownstreamKeyerClip[keyer];
			}
			
			/**
			 * Get Downstream Keyer; Gain
			 * keyer 	0: DSK1, 1: DSK2
			 */
			uint16_t ATEMmax::getDownstreamKeyerGain(uint8_t keyer) {
				return atemDownstreamKeyerGain[keyer];
			}
			
			/**
			 * Get Downstream Keyer; Invert Key
			 * keyer 	0: DSK1, 1: DSK2
			 */
			bool ATEMmax::getDownstreamKeyerInvertKey(uint8_t keyer) {
				return atemDownstreamKeyerInvertKey[keyer];
			}
			
			/**
			 * Get Downstream Keyer; Masked
			 * keyer 	0: DSK1, 1: DSK2
			 */
			bool ATEMmax::getDownstreamKeyerMasked(uint8_t keyer) {
				return atemDownstreamKeyerMasked[keyer];
			}
			
			/**
			 * Get Downstream Keyer; Top
			 * keyer 	0: DSK1, 1: DSK2
			 */
			int16_t ATEMmax::getDownstreamKeyerTop(uint8_t keyer) {
				return atemDownstreamKeyerTop[keyer];
			}
			
			/**
			 * Get Downstream Keyer; Bottom
			 * keyer 	0: DSK1, 1: DSK2
			 */
			int16_t ATEMmax::getDownstreamKeyerBottom(uint8_t keyer) {
				return atemDownstreamKeyerBottom[keyer];
			}
			
			/**
			 * Get Downstream Keyer; Left
			 * keyer 	0: DSK1, 1: DSK2
			 */
			int16_t ATEMmax::getDownstreamKeyerLeft(uint8_t keyer) {
				return atemDownstreamKeyerLeft[keyer];
			}
			
			/**
			 * Get Downstream Keyer; Right
			 * keyer 	0: DSK1, 1: DSK2
			 */
			int16_t ATEMmax::getDownstreamKeyerRight(uint8_t keyer) {
				return atemDownstreamKeyerRight[keyer];
			}
			
			/**
			 * Set Downstream Keyer; Tie
			 * keyer 	0: DSK1, 1: DSK2
			 * tie 	Bit 0: On/Off
			 */
			void ATEMmax::setDownstreamKeyerTie(uint8_t keyer, bool tie) {
			
	  	  		_prepareCommandPacket(PSTR("CDsT"),4,(_packetBuffer[12+_cBBO+4+4+0]==keyer));
		
				_packetBuffer[12+_cBBO+4+4+0] = keyer;
				
				_packetBuffer[12+_cBBO+4+4+1] = tie;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Downstream Keyer; Rate
			 * keyer 	0: DSK1, 1: DSK2
			 * rate 	1-250: Frames
			 */
			void ATEMmax::setDownstreamKeyerRate(uint8_t keyer, uint8_t rate) {
			
	  	  		_prepareCommandPacket(PSTR("CDsR"),4,(_packetBuffer[12+_cBBO+4+4+0]==keyer));
		
				_packetBuffer[12+_cBBO+4+4+0] = keyer;
				
				_packetBuffer[12+_cBBO+4+4+1] = rate;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Downstream Keyer; Pre Multiplied
			 * keyer 	0-3: Keyer 1-4
			 * preMultiplied 	Bit 0: On/Off
			 */
			void ATEMmax::setDownstreamKeyerPreMultiplied(uint8_t keyer, bool preMultiplied) {
			
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
			void ATEMmax::setDownstreamKeyerClip(uint8_t keyer, uint16_t clip) {
			
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
			void ATEMmax::setDownstreamKeyerGain(uint8_t keyer, uint16_t gain) {
			
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
			void ATEMmax::setDownstreamKeyerInvertKey(uint8_t keyer, bool invertKey) {
			
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
			void ATEMmax::setDownstreamKeyerMasked(uint8_t keyer, bool masked) {
			
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
			void ATEMmax::setDownstreamKeyerTop(uint8_t keyer, int16_t top) {
			
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
			void ATEMmax::setDownstreamKeyerBottom(uint8_t keyer, int16_t bottom) {
			
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
			void ATEMmax::setDownstreamKeyerLeft(uint8_t keyer, int16_t left) {
			
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
			void ATEMmax::setDownstreamKeyerRight(uint8_t keyer, int16_t right) {
			
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
			void ATEMmax::performDownstreamKeyerAutoKeyer(uint8_t keyer) {
			
	  	  		_prepareCommandPacket(PSTR("DDsA"),4);
		
				_packetBuffer[12+_cBBO+4+4+0] = keyer;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Get Downstream Keyer; On Air
			 * keyer 	0: DSK1, 1: DSK2
			 */
			bool ATEMmax::getDownstreamKeyerOnAir(uint8_t keyer) {
				return atemDownstreamKeyerOnAir[keyer];
			}
			
			/**
			 * Get Downstream Keyer; In Transition
			 * keyer 	0: DSK1, 1: DSK2
			 */
			bool ATEMmax::getDownstreamKeyerInTransition(uint8_t keyer) {
				return atemDownstreamKeyerInTransition[keyer];
			}
			
			/**
			 * Get Downstream Keyer; Is Auto Transitioning
			 * keyer 	0: DSK1, 1: DSK2
			 */
			bool ATEMmax::getDownstreamKeyerIsAutoTransitioning(uint8_t keyer) {
				return atemDownstreamKeyerIsAutoTransitioning[keyer];
			}
			
			/**
			 * Get Downstream Keyer; Frames Remaining
			 * keyer 	0: DSK1, 1: DSK2
			 */
			uint8_t ATEMmax::getDownstreamKeyerFramesRemaining(uint8_t keyer) {
				return atemDownstreamKeyerFramesRemaining[keyer];
			}
			
			/**
			 * Set Downstream Keyer; On Air
			 * keyer 	0: DSK1, 1: DSK2
			 * onAir 	Bit 0: On/Off
			 */
			void ATEMmax::setDownstreamKeyerOnAir(uint8_t keyer, bool onAir) {
			
	  	  		_prepareCommandPacket(PSTR("CDsL"),4,(_packetBuffer[12+_cBBO+4+4+0]==keyer));
		
				_packetBuffer[12+_cBBO+4+4+0] = keyer;
				
				_packetBuffer[12+_cBBO+4+4+1] = onAir;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Get Fade-To-Black; Rate
			 * mE 	0: ME1, 1: ME2
			 */
			uint8_t ATEMmax::getFadeToBlackRate(uint8_t mE) {
				return atemFadeToBlackRate[mE];
			}
			
			/**
			 * Set Fade-To-Black; Rate
			 * mE 	0: ME1, 1: ME2
			 * rate 	1-250: Frames
			 */
			void ATEMmax::setFadeToBlackRate(uint8_t mE, uint8_t rate) {
			
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
			bool ATEMmax::getFadeToBlackStateFullyBlack(uint8_t mE) {
				return atemFadeToBlackStateFullyBlack[mE];
			}
			
			/**
			 * Get Fade-To-Black State; In Transition
			 * mE 	0: ME1, 1: ME2
			 */
			bool ATEMmax::getFadeToBlackStateInTransition(uint8_t mE) {
				return atemFadeToBlackStateInTransition[mE];
			}
			
			/**
			 * Get Fade-To-Black State; Frames Remaining
			 * mE 	0: ME1, 1: ME2
			 */
			uint8_t ATEMmax::getFadeToBlackStateFramesRemaining(uint8_t mE) {
				return atemFadeToBlackStateFramesRemaining[mE];
			}
			


				/**
				 * Set Fade-To-Black; M/E
				 * mE 	0: ME1, 1: ME2
				 */
				void ATEMmax::performFadeToBlackME(uint8_t mE) {

		  	  		_prepareCommandPacket(PSTR("FtbA"),4);

					_packetBuffer[12+_cBBO+4+4+0] = mE;
					_packetBuffer[12+_cBBO+4+4+1] = 0x02;

					_finishCommandPacket();

				}
			
			/**
			 * Get Color Generator; Hue
			 * colorGenerator 	0: Color Generator 1, 1: Color Generator 2
			 */
			uint16_t ATEMmax::getColorGeneratorHue(uint8_t colorGenerator) {
				return atemColorGeneratorHue[colorGenerator];
			}
			
			/**
			 * Get Color Generator; Saturation
			 * colorGenerator 	0: Color Generator 1, 1: Color Generator 2
			 */
			uint16_t ATEMmax::getColorGeneratorSaturation(uint8_t colorGenerator) {
				return atemColorGeneratorSaturation[colorGenerator];
			}
			
			/**
			 * Get Color Generator; Luma
			 * colorGenerator 	0: Color Generator 1, 1: Color Generator 2
			 */
			uint16_t ATEMmax::getColorGeneratorLuma(uint8_t colorGenerator) {
				return atemColorGeneratorLuma[colorGenerator];
			}
			
			/**
			 * Set Color Generator; Hue
			 * colorGenerator 	0: Color Generator 1, 1: Color Generator 2
			 * hue 	0-3599: 0-359.9 Degrees
			 */
			void ATEMmax::setColorGeneratorHue(uint8_t colorGenerator, uint16_t hue) {
			
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
			void ATEMmax::setColorGeneratorSaturation(uint8_t colorGenerator, uint16_t saturation) {
			
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
			void ATEMmax::setColorGeneratorLuma(uint8_t colorGenerator, uint16_t luma) {
			
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
			uint16_t ATEMmax::getAuxSourceInput(uint8_t aUXChannel) {
				return atemAuxSourceInput[aUXChannel];
			}
			
			/**
			 * Set Aux Source; Input
			 * aUXChannel 	0-5: Aux 1-6
			 * input 	(See video source list)
			 */
			void ATEMmax::setAuxSourceInput(uint8_t aUXChannel, uint16_t input) {
			
	  	  		_prepareCommandPacket(PSTR("CAuS"),4,(_packetBuffer[12+_cBBO+4+4+1]==aUXChannel));
		
					// Set Mask: 1
				_packetBuffer[12+_cBBO+4+4+0] |= 1;
						
				_packetBuffer[12+_cBBO+4+4+1] = aUXChannel;
				
				_packetBuffer[12+_cBBO+4+4+2] = highByte(input);
				_packetBuffer[12+_cBBO+4+4+3] = lowByte(input);
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Get Camera Control; Iris
			 * input 	1-20: Camera
			 */
			int16_t ATEMmax::getCameraControlIris(uint8_t input) {
				return atemCameraControlIris[input];
			}
			
			/**
			 * Get Camera Control; Focus
			 * input 	1-20: Camera
			 */
			int16_t ATEMmax::getCameraControlFocus(uint8_t input) {
				return atemCameraControlFocus[input];
			}
			
			/**
			 * Get Camera Control; Gain
			 * input 	1-20: Camera
			 */
			int16_t ATEMmax::getCameraControlGain(uint8_t input) {
				return atemCameraControlGain[input];
			}
			
			/**
			 * Get Camera Control; White Balance
			 * input 	1-20: Camera
			 */
			int16_t ATEMmax::getCameraControlWhiteBalance(uint8_t input) {
				return atemCameraControlWhiteBalance[input];
			}
			
			/**
			 * Get Camera Control; Sharpening Level
			 * input 	1-20: Camera
			 */
			int16_t ATEMmax::getCameraControlSharpeningLevel(uint8_t input) {
				return atemCameraControlSharpeningLevel[input];
			}
			
			/**
			 * Get Camera Control; Zoom Normalized
			 * input 	1-20: Camera
			 */
			int16_t ATEMmax::getCameraControlZoomNormalized(uint8_t input) {
				return atemCameraControlZoomNormalized[input];
			}
			
			/**
			 * Get Camera Control; Zoom Speed
			 * input 	1-20: Camera
			 */
			int16_t ATEMmax::getCameraControlZoomSpeed(uint8_t input) {
				return atemCameraControlZoomSpeed[input];
			}
			
			/**
			 * Get Camera Control; Colorbars
			 * input 	1-20: Camera
			 */
			int16_t ATEMmax::getCameraControlColorbars(uint8_t input) {
				return atemCameraControlColorbars[input];
			}
			
			/**
			 * Get Camera Control; Lift R
			 * input 	1-20: Camera
			 */
			int16_t ATEMmax::getCameraControlLiftR(uint8_t input) {
				return atemCameraControlLiftR[input];
			}
			
			/**
			 * Get Camera Control; Gamma R
			 * input 	1-20: Camera
			 */
			int16_t ATEMmax::getCameraControlGammaR(uint8_t input) {
				return atemCameraControlGammaR[input];
			}
			
			/**
			 * Get Camera Control; Gain R
			 * input 	1-20: Camera
			 */
			int16_t ATEMmax::getCameraControlGainR(uint8_t input) {
				return atemCameraControlGainR[input];
			}
			
			/**
			 * Get Camera Control; Lum Mix
			 * input 	1-20: Camera
			 */
			int16_t ATEMmax::getCameraControlLumMix(uint8_t input) {
				return atemCameraControlLumMix[input];
			}
			
			/**
			 * Get Camera Control; Hue
			 * input 	1-20: Camera
			 */
			int16_t ATEMmax::getCameraControlHue(uint8_t input) {
				return atemCameraControlHue[input];
			}
			
			/**
			 * Get Camera Control; Shutter
			 * input 	1-20: Camera
			 */
			int16_t ATEMmax::getCameraControlShutter(uint8_t input) {
				return atemCameraControlShutter[input];
			}
			
			/**
			 * Get Camera Control; Lift G
			 * input 	1-20: Camera
			 */
			int16_t ATEMmax::getCameraControlLiftG(uint8_t input) {
				return atemCameraControlLiftG[input];
			}
			
			/**
			 * Get Camera Control; Gamma G
			 * input 	1-20: Camera
			 */
			int16_t ATEMmax::getCameraControlGammaG(uint8_t input) {
				return atemCameraControlGammaG[input];
			}
			
			/**
			 * Get Camera Control; Gain G
			 * input 	1-20: Camera
			 */
			int16_t ATEMmax::getCameraControlGainG(uint8_t input) {
				return atemCameraControlGainG[input];
			}
			
			/**
			 * Get Camera Control; Contrast
			 * input 	1-20: Camera
			 */
			int16_t ATEMmax::getCameraControlContrast(uint8_t input) {
				return atemCameraControlContrast[input];
			}
			
			/**
			 * Get Camera Control; Saturation
			 * input 	1-20: Camera
			 */
			int16_t ATEMmax::getCameraControlSaturation(uint8_t input) {
				return atemCameraControlSaturation[input];
			}
			
			/**
			 * Get Camera Control; Lift B
			 * input 	1-20: Camera
			 */
			int16_t ATEMmax::getCameraControlLiftB(uint8_t input) {
				return atemCameraControlLiftB[input];
			}
			
			/**
			 * Get Camera Control; Gamma B
			 * input 	1-20: Camera
			 */
			int16_t ATEMmax::getCameraControlGammaB(uint8_t input) {
				return atemCameraControlGammaB[input];
			}
			
			/**
			 * Get Camera Control; Gain B
			 * input 	1-20: Camera
			 */
			int16_t ATEMmax::getCameraControlGainB(uint8_t input) {
				return atemCameraControlGainB[input];
			}
			
			/**
			 * Get Camera Control; Lift Y
			 * input 	1-20: Camera
			 */
			int16_t ATEMmax::getCameraControlLiftY(uint8_t input) {
				return atemCameraControlLiftY[input];
			}
			
			/**
			 * Get Camera Control; Gamma Y
			 * input 	1-20: Camera
			 */
			int16_t ATEMmax::getCameraControlGammaY(uint8_t input) {
				return atemCameraControlGammaY[input];
			}
			
			/**
			 * Get Camera Control; Gain Y
			 * input 	1-20: Camera
			 */
			int16_t ATEMmax::getCameraControlGainY(uint8_t input) {
				return atemCameraControlGainY[input];
			}
			

			/**
				* Set Camera Control; Auto iris
				* Command takes no input
				*/

			void ATEMmax::setCameraControlAutoIris(uint8_t input, int16_t autoiris) {
					_prepareCommandPacket(PSTR("CCmd"), 24);

					_packetBuffer[12+_cBBO+4+4+0] = input;

					_packetBuffer[12+_cBBO+4+4+1] = 0;
					_packetBuffer[12+_cBBO+4+4+2] = 5;

					_packetBuffer[12+_cBBO+4+4+4] = 0x00; // Data type: void

					_finishCommandPacket();
			}


			/**
				* Set Camera Control; Detail level
				* 0: Off, 1: Low, 2: Medium, 3: High
				*/

			void ATEMmax::setCameraControlSharpeningLevel(uint8_t input, int16_t detail) {
					_prepareCommandPacket(PSTR("CCmd"), 20);

					_packetBuffer[12+_cBBO+4+4+0] = input;

					_packetBuffer[12+_cBBO+4+4+1] = 1;
					_packetBuffer[12+_cBBO+4+4+2] = 8;

					_packetBuffer[12+_cBBO+4+4+4] = 0x01; // Data type: int8
					
					// Reduncancy: Support for ATEM Switchers & ATEM Proxy
					_packetBuffer[12+_cBBO+4+4+7] = 0x01;
					_packetBuffer[12+_cBBO+4+4+9] = 0x01;

					_packetBuffer[12+_cBBO+4+4+16] = detail & 0xFF;

					_finishCommandPacket();
			}

			/**
				* Set Camera Control; Auto focus
				* Command takes no input
				*/

			void ATEMmax::setCameraControlAutoFocus(uint8_t input, int16_t autoiris) {
					_prepareCommandPacket(PSTR("CCmd"), 24);

					_packetBuffer[12+_cBBO+4+4+0] = input;

					_packetBuffer[12+_cBBO+4+4+1] = 0;
					_packetBuffer[12+_cBBO+4+4+2] = 1;

					_packetBuffer[12+_cBBO+4+4+4] = 0x00; // Data type: void

					_finishCommandPacket();
			}

			/**
				* Set Camera Control; Reset all
				* Command takes no input
				*/

			void ATEMmax::setCameraControlResetAll(uint8_t input, int16_t reset) {
					_prepareCommandPacket(PSTR("CCmd"), 24);

					_packetBuffer[12+_cBBO+4+4+0] = input;

					_packetBuffer[12+_cBBO+4+4+1] = 8;
					_packetBuffer[12+_cBBO+4+4+2] = 7;

					_packetBuffer[12+_cBBO+4+4+4] = 0x00; // Data type: void

					_finishCommandPacket();

					// Update local state variables to reflect reset values
					atemCameraControlGammaY[input] = 0;
					atemCameraControlGammaR[input] = 0;
					atemCameraControlGammaG[input] = 0;
					atemCameraControlGammaB[input] = 0;

					atemCameraControlLiftY[input] = 0;
					atemCameraControlLiftR[input] = 0;
					atemCameraControlLiftG[input] = 0;
					atemCameraControlLiftB[input] = 0;

					atemCameraControlGainY[input] = 2048;
					atemCameraControlGainR[input] = 2048;
					atemCameraControlGainG[input] = 2048;
					atemCameraControlGainB[input] = 2048;

					atemCameraControlContrast[input] = 2048;
					atemCameraControlHue[input] = 0;
					atemCameraControlSaturation[input] = 2048;
			}

			/**
			 * Set Camera Control; Iris
			 * input 	0-7: Camera
			 * iris 	0-2048
			 */
			void ATEMmax::setCameraControlIris(uint8_t input, int16_t iris) {

		  		_prepareCommandPacket(PSTR("CCmd"),24);

					// Preset values:
				_packetBuffer[12+_cBBO+4+4+1] = 0;
				_packetBuffer[12+_cBBO+4+4+2] = 3;

				_packetBuffer[12+_cBBO+4+4+4] = 0x80;	// Data type: 5.11 floating point
				_packetBuffer[12+_cBBO+4+4+9] = 0x01;	// One byte

				_packetBuffer[12+_cBBO+4+4+0] = input;

				_packetBuffer[12+_cBBO+4+4+16] = highByte(iris);
				_packetBuffer[12+_cBBO+4+4+17] = lowByte(iris);

				_finishCommandPacket();
			}

			/**
			 * Set Camera Control; Colorbars
			 * input 	0-7: Camera
			 * colorbars: duration in secs (0=disable)
			 */
			void ATEMmax::setCameraControlColorbars(uint8_t input, int16_t colorbars) {

		  		_prepareCommandPacket(PSTR("CCmd"), 20);

				_packetBuffer[12+_cBBO+4+4+0] = input;

					// Preset values:
				_packetBuffer[12+_cBBO+4+4+1] = 4;
				_packetBuffer[12+_cBBO+4+4+2] = 4;

				_packetBuffer[12+_cBBO+4+4+4] = 0x01;	// Data type: int8
				
				// Reduncancy: Support for ATEM Switchers & ATEM Proxy
				_packetBuffer[12+_cBBO+4+4+7] = 0x01;
				_packetBuffer[12+_cBBO+4+4+9] = 0x01;


				_packetBuffer[12+_cBBO+4+4+16] = (colorbars & 0xFF);

				_finishCommandPacket();

			}

			/**
			 * Set Camera Control; Focus
			 * input 	0-7: Camera
			 * focus 	0-65535
			 */
			void ATEMmax::setCameraControlFocus(uint8_t input, int16_t focus) {

		  		_prepareCommandPacket(PSTR("CCmd"),24);

					// Preset values:
				_packetBuffer[12+_cBBO+4+4+1] = 0;
				_packetBuffer[12+_cBBO+4+4+2] = 0;

				_packetBuffer[12+_cBBO+4+4+4] = 0x80;	// Data type: 5.11 floating point
				_packetBuffer[12+_cBBO+4+4+9] = 0x01;	// One byte

				_packetBuffer[12+_cBBO+4+4+0] = input;

				_packetBuffer[12+_cBBO+4+4+16] = highByte(focus);
				_packetBuffer[12+_cBBO+4+4+17] = lowByte(focus);

		   		_finishCommandPacket();

			}

			/**
			 * Set Camera Control; Gain
			 * input 	0-7: Camera
			 * gain 	512: 0db, 1024: 6db, 2048: 12db, 4096: 18db
			 */
			void ATEMmax::setCameraControlGain(uint8_t input, int16_t gain) {

		  		_prepareCommandPacket(PSTR("CCmd"),24);

					// Preset values:
				_packetBuffer[12+_cBBO+4+4+1] = 1;
				_packetBuffer[12+_cBBO+4+4+2] = 1;

				_packetBuffer[12+_cBBO+4+4+4] = 0x01;

				// Reduncancy: Support for ATEM Switchers & ATEM Proxy
				_packetBuffer[12+_cBBO+4+4+7] = 0x01;
				_packetBuffer[12+_cBBO+4+4+9] = 0x01;

				_packetBuffer[12+_cBBO+4+4+0] = input;

				_packetBuffer[12+_cBBO+4+4+16] = highByte(gain);
				_packetBuffer[12+_cBBO+4+4+17] = lowByte(gain);

		   		_finishCommandPacket();

			}

			/**
			 * Set Camera Control; White Balance
			 * input 	0-7: Camera
			 *
			 */
			void ATEMmax::setCameraControlWhiteBalance(uint8_t input, int16_t whiteBalance) {

		  		_prepareCommandPacket(PSTR("CCmd"),24);

					// Preset values:
				_packetBuffer[12+_cBBO+4+4+1] = 1;
				_packetBuffer[12+_cBBO+4+4+2] = 2;

				_packetBuffer[12+_cBBO+4+4+4] = 0x02;
				_packetBuffer[12+_cBBO+4+4+9] = 0x01;

				_packetBuffer[12+_cBBO+4+4+0] = input;

				_packetBuffer[12+_cBBO+4+4+16] = highByte(whiteBalance);
				_packetBuffer[12+_cBBO+4+4+17] = lowByte(whiteBalance);

		   		_finishCommandPacket();

			}

			/**
			 * Set Camera Control; Zoom Normalized
			 * input 	0-7: Camera
			 *
			 */
			void ATEMmax::setCameraControlZoomNormalized(uint8_t input, int16_t zoomNormalized) {

		  		_prepareCommandPacket(PSTR("CCmd"),24);

					// Preset values:
				_packetBuffer[12+_cBBO+4+4+1] = 0;
				_packetBuffer[12+_cBBO+4+4+2] = 8;

				_packetBuffer[12+_cBBO+4+4+4] = 0x80;
				_packetBuffer[12+_cBBO+4+4+9] = 0x01;

				_packetBuffer[12+_cBBO+4+4+0] = input;

				_packetBuffer[12+_cBBO+4+4+16] = highByte(zoomNormalized);
				_packetBuffer[12+_cBBO+4+4+17] = lowByte(zoomNormalized);

		   		_finishCommandPacket();

			}

			/**
			 * Set Camera Control; Zoom
			 * input 	0-7: Camera
			 *
			 */
			void ATEMmax::setCameraControlZoomSpeed(uint8_t input, int16_t zoomSpeed) {

		  		_prepareCommandPacket(PSTR("CCmd"),24);

					// Preset values:
				_packetBuffer[12+_cBBO+4+4+1] = 0;
				_packetBuffer[12+_cBBO+4+4+2] = 9;

				_packetBuffer[12+_cBBO+4+4+4] = 0x80;
				_packetBuffer[12+_cBBO+4+4+9] = 0x01;

				_packetBuffer[12+_cBBO+4+4+0] = input;

				_packetBuffer[12+_cBBO+4+4+16] = highByte(zoomSpeed);
				_packetBuffer[12+_cBBO+4+4+17] = lowByte(zoomSpeed);

		   		_finishCommandPacket();

			}

			/**
			 * Set Camera Control; Lift R
			 * input 	0-7: Camera
			 * liftR 	-4096-4096: -1.00-1.00
			 */
			void ATEMmax::setCameraControlLiftR(uint8_t input, int16_t liftR) {

		  		_prepareCommandPacket(PSTR("CCmd"),24);

					// Preset values:
				_packetBuffer[12+_cBBO+4+4+1] = 8;
				_packetBuffer[12+_cBBO+4+4+2] = 0;

				_packetBuffer[12+_cBBO+4+4+4] = 0x80;
				_packetBuffer[12+_cBBO+4+4+9] = 0x04;

				_packetBuffer[12+_cBBO+4+4+0] = input;

				_packetBuffer[12+_cBBO+4+4+16] = highByte(liftR);
				_packetBuffer[12+_cBBO+4+4+17] = lowByte(liftR);

				_packetBuffer[12+_cBBO+4+4+18] = highByte(getCameraControlLiftG(input));
				_packetBuffer[12+_cBBO+4+4+19] = lowByte(getCameraControlLiftG(input));
				_packetBuffer[12+_cBBO+4+4+20] = highByte(getCameraControlLiftB(input));
				_packetBuffer[12+_cBBO+4+4+21] = lowByte(getCameraControlLiftB(input));
				_packetBuffer[12+_cBBO+4+4+22] = highByte(getCameraControlLiftY(input));
				_packetBuffer[12+_cBBO+4+4+23] = lowByte(getCameraControlLiftY(input));

		   		_finishCommandPacket();

			}

			/**
			 * Set Camera Control; Gamma R
			 * input 	0-7: Camera
			 * gammaR 	-4096-4096: -1.00-1.00
			 */
			void ATEMmax::setCameraControlGammaR(uint8_t input, int16_t gammaR) {

		  		_prepareCommandPacket(PSTR("CCmd"),24);

					// Preset values:
				_packetBuffer[12+_cBBO+4+4+1] = 8;
				_packetBuffer[12+_cBBO+4+4+2] = 1;

				_packetBuffer[12+_cBBO+4+4+4] = 0x80;
				_packetBuffer[12+_cBBO+4+4+9] = 0x04;

				_packetBuffer[12+_cBBO+4+4+0] = input;

				_packetBuffer[12+_cBBO+4+4+16] = highByte(gammaR);
				_packetBuffer[12+_cBBO+4+4+17] = lowByte(gammaR);

				_packetBuffer[12+_cBBO+4+4+18] = highByte(getCameraControlGammaG(input));
				_packetBuffer[12+_cBBO+4+4+19] = lowByte(getCameraControlGammaG(input));
				_packetBuffer[12+_cBBO+4+4+20] = highByte(getCameraControlGammaB(input));
				_packetBuffer[12+_cBBO+4+4+21] = lowByte(getCameraControlGammaB(input));
				_packetBuffer[12+_cBBO+4+4+22] = highByte(getCameraControlGammaY(input));
				_packetBuffer[12+_cBBO+4+4+23] = lowByte(getCameraControlGammaY(input));


				_finishCommandPacket();
			}

			/**
			 * Set Camera Control; Gain R
			 * input 	0-7: Camera
			 * gainR 	-4096-4096: -1.00-1.00
			 */
			void ATEMmax::setCameraControlGainR(uint8_t input, int16_t gainR) {

		  		_prepareCommandPacket(PSTR("CCmd"),24);

					// Preset values:
				_packetBuffer[12+_cBBO+4+4+1] = 8;
				_packetBuffer[12+_cBBO+4+4+2] = 2;

				_packetBuffer[12+_cBBO+4+4+4] = 0x80;
				_packetBuffer[12+_cBBO+4+4+9] = 0x04;

				_packetBuffer[12+_cBBO+4+4+0] = input;

				_packetBuffer[12+_cBBO+4+4+16] = highByte(gainR);
				_packetBuffer[12+_cBBO+4+4+17] = lowByte(gainR);

				_packetBuffer[12+_cBBO+4+4+18] = highByte(getCameraControlGainG(input));
				_packetBuffer[12+_cBBO+4+4+19] = lowByte(getCameraControlGainG(input));
				_packetBuffer[12+_cBBO+4+4+20] = highByte(getCameraControlGainB(input));
				_packetBuffer[12+_cBBO+4+4+21] = lowByte(getCameraControlGainB(input));
				_packetBuffer[12+_cBBO+4+4+22] = highByte(getCameraControlGainY(input));
				_packetBuffer[12+_cBBO+4+4+23] = lowByte(getCameraControlGainY(input));

				_finishCommandPacket();
			}

			/**
			 * Set Camera Control; Lum Mix
			 * input 	0-7: Camera
			 * lumMix 	0-2048: 0-100%
			 */
			void ATEMmax::setCameraControlLumMix(uint8_t input, int16_t lumMix) {

		  		_prepareCommandPacket(PSTR("CCmd"),24);

					// Preset values:
				_packetBuffer[12+_cBBO+4+4+1] = 8;
				_packetBuffer[12+_cBBO+4+4+2] = 5;

				_packetBuffer[12+_cBBO+4+4+4] = 0x80;
				_packetBuffer[12+_cBBO+4+4+9] = 0x01;

				_packetBuffer[12+_cBBO+4+4+0] = input;

				_packetBuffer[12+_cBBO+4+4+16] = highByte(lumMix);
				_packetBuffer[12+_cBBO+4+4+17] = lowByte(lumMix);

				_finishCommandPacket();

			}

			/**
			 * Set Camera Control; Hue
			 * input 	0-7: Camera
			 * hue 	-2048-2048: 0-360 degrees
			 */
			void ATEMmax::setCameraControlHue(uint8_t input, int16_t hue) {

		  		_prepareCommandPacket(PSTR("CCmd"),24);

					// Preset values:
				_packetBuffer[12+_cBBO+4+4+1] = 8;
				_packetBuffer[12+_cBBO+4+4+2] = 6;

				_packetBuffer[12+_cBBO+4+4+4] = 0x80;
				_packetBuffer[12+_cBBO+4+4+9] = 0x02;

				_packetBuffer[12+_cBBO+4+4+0] = input;

				_packetBuffer[12+_cBBO+4+4+16] = highByte(hue);
				_packetBuffer[12+_cBBO+4+4+17] = lowByte(hue);

				_packetBuffer[12+_cBBO+4+4+18] = highByte(getCameraControlSaturation(input));
				_packetBuffer[12+_cBBO+4+4+19] = lowByte(getCameraControlSaturation(input));

				_finishCommandPacket();

			}

			/**
			 * Set Camera Control; Shutter
			 * input 	0-7: Camera
			 * shutter 	20000: 1/50, 16667: 1/60, 13333: 1/75, 11111: 1/90, 10000: 1/100, 8333: 1/120, 6667: 1/150, 5556: 1/180, 4000: 1/250, 2778: 1/360, 2000: 1/500, 1379: 1/750, 1000: 1/1000, 690: 1/1450, 500: 1/2000
			 */
			void ATEMmax::setCameraControlShutter(uint8_t input, int16_t shutter) {

		  		_prepareCommandPacket(PSTR("CCmd"),24);

					// Preset values:
				_packetBuffer[12+_cBBO+4+4+1] = 1;
				_packetBuffer[12+_cBBO+4+4+2] = 5;

				_packetBuffer[12+_cBBO+4+4+4] = 0x03;
				_packetBuffer[12+_cBBO+4+4+11] = 0x01;

				_packetBuffer[12+_cBBO+4+4+0] = input;

				_packetBuffer[12+_cBBO+4+4+18] = highByte(shutter);
				_packetBuffer[12+_cBBO+4+4+19] = lowByte(shutter);

				_finishCommandPacket();

			}

			/**
			 * Set Camera Control; Lift G
			 * input 	0-7: Camera
			 * liftG 	-4096-4096: -1.00-1.00
			 */
			void ATEMmax::setCameraControlLiftG(uint8_t input, int16_t liftG) {

		  		_prepareCommandPacket(PSTR("CCmd"),24);

					// Preset values:
				_packetBuffer[12+_cBBO+4+4+1] = 8;
				_packetBuffer[12+_cBBO+4+4+2] = 0;

				_packetBuffer[12+_cBBO+4+4+4] = 0x80;
				_packetBuffer[12+_cBBO+4+4+9] = 0x04;

				_packetBuffer[12+_cBBO+4+4+0] = input;

				_packetBuffer[12+_cBBO+4+4+16] = highByte(getCameraControlLiftR(input));
				_packetBuffer[12+_cBBO+4+4+17] = lowByte(getCameraControlLiftR(input));

				_packetBuffer[12+_cBBO+4+4+18] = highByte(liftG);
				_packetBuffer[12+_cBBO+4+4+19] = lowByte(liftG);

				_packetBuffer[12+_cBBO+4+4+20] = highByte(getCameraControlLiftB(input));
				_packetBuffer[12+_cBBO+4+4+21] = lowByte(getCameraControlLiftB(input));
				_packetBuffer[12+_cBBO+4+4+22] = highByte(getCameraControlLiftY(input));
				_packetBuffer[12+_cBBO+4+4+23] = lowByte(getCameraControlLiftY(input));


				_finishCommandPacket();

			}

			/**
			 * Set Camera Control; Gamma G
			 * input 	0-7: Camera
			 * gammaG 	-4096-4096: -1.00-1.00
			 */
			void ATEMmax::setCameraControlGammaG(uint8_t input, int16_t gammaG) {

		  		_prepareCommandPacket(PSTR("CCmd"),24);

					// Preset values:
				_packetBuffer[12+_cBBO+4+4+1] = 8;
				_packetBuffer[12+_cBBO+4+4+2] = 1;

				_packetBuffer[12+_cBBO+4+4+4] = 0x80;
				_packetBuffer[12+_cBBO+4+4+9] = 0x04;

				_packetBuffer[12+_cBBO+4+4+0] = input;

				_packetBuffer[12+_cBBO+4+4+16] = highByte(getCameraControlGammaR(input));
				_packetBuffer[12+_cBBO+4+4+17] = lowByte(getCameraControlGammaR(input));

				_packetBuffer[12+_cBBO+4+4+18] = highByte(gammaG);
				_packetBuffer[12+_cBBO+4+4+19] = lowByte(gammaG);

				_packetBuffer[12+_cBBO+4+4+20] = highByte(getCameraControlGammaB(input));
				_packetBuffer[12+_cBBO+4+4+21] = lowByte(getCameraControlGammaB(input));
				_packetBuffer[12+_cBBO+4+4+22] = highByte(getCameraControlGammaY(input));
				_packetBuffer[12+_cBBO+4+4+23] = lowByte(getCameraControlGammaY(input));

				_finishCommandPacket();

			}

			/**
			 * Set Camera Control; Gain G
			 * input 	0-7: Camera
			 * gainG 	-4096-4096: -1.00-1.00
			 */
			void ATEMmax::setCameraControlGainG(uint8_t input, int16_t gainG) {

		  		_prepareCommandPacket(PSTR("CCmd"),24);

					// Preset values:
				_packetBuffer[12+_cBBO+4+4+1] = 8;
				_packetBuffer[12+_cBBO+4+4+2] = 2;

				_packetBuffer[12+_cBBO+4+4+4] = 0x80;
				_packetBuffer[12+_cBBO+4+4+9] = 0x04;

				_packetBuffer[12+_cBBO+4+4+0] = input;

				_packetBuffer[12+_cBBO+4+4+16] = highByte(getCameraControlGainR(input));
				_packetBuffer[12+_cBBO+4+4+17] = lowByte(getCameraControlGainR(input));

				_packetBuffer[12+_cBBO+4+4+18] = highByte(gainG);
				_packetBuffer[12+_cBBO+4+4+19] = lowByte(gainG);

				_packetBuffer[12+_cBBO+4+4+20] = highByte(getCameraControlGainB(input));
				_packetBuffer[12+_cBBO+4+4+21] = lowByte(getCameraControlGainB(input));
				_packetBuffer[12+_cBBO+4+4+22] = highByte(getCameraControlGainY(input));
				_packetBuffer[12+_cBBO+4+4+23] = lowByte(getCameraControlGainY(input));

				_finishCommandPacket();

			}

			/**
			 * Set Camera Control; Contrast
			 * input 	0-7: Camera
			 * contrast 	0-4096: 0-100%
			 */
			void ATEMmax::setCameraControlContrast(uint8_t input, int16_t contrast) {

		  		_prepareCommandPacket(PSTR("CCmd"),24);

					// Preset values:
				_packetBuffer[12+_cBBO+4+4+1] = 8;
				_packetBuffer[12+_cBBO+4+4+2] = 4;

				_packetBuffer[12+_cBBO+4+4+4] = 0x80;
				_packetBuffer[12+_cBBO+4+4+9] = 0x02;

				_packetBuffer[12+_cBBO+4+4+0] = input;

				// Pivot = 0.5 (Fixed16 1024)
				_packetBuffer[12+_cBBO+4+4+16] = 4;
				_packetBuffer[12+_cBBO+4+4+17] = 0;

				_packetBuffer[12+_cBBO+4+4+18] = highByte(contrast);
				_packetBuffer[12+_cBBO+4+4+19] = lowByte(contrast);

				_finishCommandPacket();

			}

			/**
			 * Set Camera Control; Saturation
			 * input 	0-7: Camera
			 * saturation 	0-4096: 0-100%
			 */
			void ATEMmax::setCameraControlSaturation(uint8_t input, int16_t saturation) {

		  		_prepareCommandPacket(PSTR("CCmd"),24);

					// Preset values:
				_packetBuffer[12+_cBBO+4+4+1] = 8;
				_packetBuffer[12+_cBBO+4+4+2] = 6;

				_packetBuffer[12+_cBBO+4+4+4] = 0x80;
				_packetBuffer[12+_cBBO+4+4+9] = 0x02;

				_packetBuffer[12+_cBBO+4+4+0] = input;

				_packetBuffer[12+_cBBO+4+4+16] = highByte(getCameraControlHue(input));
				_packetBuffer[12+_cBBO+4+4+17] = lowByte(getCameraControlHue(input));

				_packetBuffer[12+_cBBO+4+4+18] = highByte(saturation);
				_packetBuffer[12+_cBBO+4+4+19] = lowByte(saturation);

				_finishCommandPacket();

			}

			/**
			 * Set Camera Control; Lift B
			 * input 	0-7: Camera
			 * liftB 	-4096-4096: -1.00-1.00
			 */
			void ATEMmax::setCameraControlLiftB(uint8_t input, int16_t liftB) {

		  		_prepareCommandPacket(PSTR("CCmd"),24);

					// Preset values:
				_packetBuffer[12+_cBBO+4+4+1] = 8;
				_packetBuffer[12+_cBBO+4+4+2] = 0;

				_packetBuffer[12+_cBBO+4+4+4] = 0x80;
				_packetBuffer[12+_cBBO+4+4+9] = 0x04;

				_packetBuffer[12+_cBBO+4+4+0] = input;

				_packetBuffer[12+_cBBO+4+4+16] = highByte(getCameraControlLiftR(input));
				_packetBuffer[12+_cBBO+4+4+17] = lowByte(getCameraControlLiftR(input));
				_packetBuffer[12+_cBBO+4+4+18] = highByte(getCameraControlLiftG(input));
				_packetBuffer[12+_cBBO+4+4+19] = lowByte(getCameraControlLiftG(input));

				_packetBuffer[12+_cBBO+4+4+20] = highByte(liftB);
				_packetBuffer[12+_cBBO+4+4+21] = lowByte(liftB);

				_packetBuffer[12+_cBBO+4+4+22] = highByte(getCameraControlLiftY(input));
				_packetBuffer[12+_cBBO+4+4+23] = lowByte(getCameraControlLiftY(input));

				_finishCommandPacket();

			}

			/**
			 * Set Camera Control; Gamma B
			 * input 	0-7: Camera
			 * gammaB 	-4096-4096: -1.00-1.00
			 */
			void ATEMmax::setCameraControlGammaB(uint8_t input, int16_t gammaB) {

		  		_prepareCommandPacket(PSTR("CCmd"),24);

					// Preset values:
				_packetBuffer[12+_cBBO+4+4+1] = 8;
				_packetBuffer[12+_cBBO+4+4+2] = 1;

				_packetBuffer[12+_cBBO+4+4+4] = 0x80;
				_packetBuffer[12+_cBBO+4+4+9] = 0x04;

				_packetBuffer[12+_cBBO+4+4+0] = input;

				_packetBuffer[12+_cBBO+4+4+16] = highByte(getCameraControlGammaR(input));
				_packetBuffer[12+_cBBO+4+4+17] = lowByte(getCameraControlGammaR(input));
				_packetBuffer[12+_cBBO+4+4+18] = highByte(getCameraControlGammaG(input));
				_packetBuffer[12+_cBBO+4+4+19] = lowByte(getCameraControlGammaG(input));

				_packetBuffer[12+_cBBO+4+4+20] = highByte(gammaB);
				_packetBuffer[12+_cBBO+4+4+21] = lowByte(gammaB);

				_packetBuffer[12+_cBBO+4+4+22] = highByte(getCameraControlGammaY(input));
				_packetBuffer[12+_cBBO+4+4+23] = lowByte(getCameraControlGammaY(input));

				_finishCommandPacket();

			}

			/**
			 * Set Camera Control; Gain B
			 * input 	0-7: Camera
			 * gainB 	-4096-4096: -1.00-1.00
			 */
			void ATEMmax::setCameraControlGainB(uint8_t input, int16_t gainB) {

		  		_prepareCommandPacket(PSTR("CCmd"),24);

					// Preset values:
				_packetBuffer[12+_cBBO+4+4+1] = 8;
				_packetBuffer[12+_cBBO+4+4+2] = 2;

				_packetBuffer[12+_cBBO+4+4+4] = 0x80;
				_packetBuffer[12+_cBBO+4+4+9] = 0x04;

				_packetBuffer[12+_cBBO+4+4+0] = input;

				_packetBuffer[12+_cBBO+4+4+16] = highByte(getCameraControlGainR(input));
				_packetBuffer[12+_cBBO+4+4+17] = lowByte(getCameraControlGainR(input));
				_packetBuffer[12+_cBBO+4+4+18] = highByte(getCameraControlGainG(input));
				_packetBuffer[12+_cBBO+4+4+19] = lowByte(getCameraControlGainG(input));

				_packetBuffer[12+_cBBO+4+4+20] = highByte(gainB);
				_packetBuffer[12+_cBBO+4+4+21] = lowByte(gainB);

				_packetBuffer[12+_cBBO+4+4+22] = highByte(getCameraControlGainY(input));
				_packetBuffer[12+_cBBO+4+4+23] = lowByte(getCameraControlGainY(input));

				_finishCommandPacket();

			}

			/**
			 * Set Camera Control; Lift Y
			 * input 	0-7: Camera
			 * liftY 	-4096-4096: -1.00-1.00
			 */
			void ATEMmax::setCameraControlLiftY(uint8_t input, int16_t liftY) {

		  		_prepareCommandPacket(PSTR("CCmd"),24);

					// Preset values:
				_packetBuffer[12+_cBBO+4+4+1] = 8;
				_packetBuffer[12+_cBBO+4+4+2] = 0;

				_packetBuffer[12+_cBBO+4+4+4] = 0x80;
				_packetBuffer[12+_cBBO+4+4+9] = 0x04;

				_packetBuffer[12+_cBBO+4+4+0] = input;

				_packetBuffer[12+_cBBO+4+4+16] = highByte(getCameraControlLiftR(input));
				_packetBuffer[12+_cBBO+4+4+17] = lowByte(getCameraControlLiftR(input));
				_packetBuffer[12+_cBBO+4+4+18] = highByte(getCameraControlLiftG(input));
				_packetBuffer[12+_cBBO+4+4+19] = lowByte(getCameraControlLiftG(input));
				_packetBuffer[12+_cBBO+4+4+20] = highByte(getCameraControlLiftB(input));
				_packetBuffer[12+_cBBO+4+4+21] = lowByte(getCameraControlLiftB(input));

				_packetBuffer[12+_cBBO+4+4+22] = highByte(liftY);
				_packetBuffer[12+_cBBO+4+4+23] = lowByte(liftY);

				_finishCommandPacket();

			}

			/**
			 * Set Camera Control; Gamma Y
			 * input 	0-7: Camera
			 * gammaY 	-4096-4096: -1.00-1.00
			 */
			void ATEMmax::setCameraControlGammaY(uint8_t input, int16_t gammaY) {

		  		_prepareCommandPacket(PSTR("CCmd"),24);

					// Preset values:
				_packetBuffer[12+_cBBO+4+4+1] = 8;
				_packetBuffer[12+_cBBO+4+4+2] = 1;

				_packetBuffer[12+_cBBO+4+4+4] = 0x80;
				_packetBuffer[12+_cBBO+4+4+9] = 0x04;

				_packetBuffer[12+_cBBO+4+4+0] = input;

				_packetBuffer[12+_cBBO+4+4+16] = highByte(getCameraControlGammaR(input));
				_packetBuffer[12+_cBBO+4+4+17] = lowByte(getCameraControlGammaR(input));
				_packetBuffer[12+_cBBO+4+4+18] = highByte(getCameraControlGammaG(input));
				_packetBuffer[12+_cBBO+4+4+19] = lowByte(getCameraControlGammaG(input));
				_packetBuffer[12+_cBBO+4+4+20] = highByte(getCameraControlGammaB(input));
				_packetBuffer[12+_cBBO+4+4+21] = lowByte(getCameraControlGammaB(input));

				_packetBuffer[12+_cBBO+4+4+22] = highByte(gammaY);
				_packetBuffer[12+_cBBO+4+4+23] = lowByte(gammaY);


				_finishCommandPacket();

			}

			/**
			 * Set Camera Control; Gain Y
			 * input 	0-7: Camera
			 * gainY 	-4096-4096: -1.00-1.00
			 */
			void ATEMmax::setCameraControlGainY(uint8_t input, int16_t gainY) {

		  		_prepareCommandPacket(PSTR("CCmd"),24);

					// Preset values:
				_packetBuffer[12+_cBBO+4+4+1] = 8;
				_packetBuffer[12+_cBBO+4+4+2] = 2;

				_packetBuffer[12+_cBBO+4+4+4] = 0x80;
				_packetBuffer[12+_cBBO+4+4+9] = 0x04;

				_packetBuffer[12+_cBBO+4+4+0] = input;

				_packetBuffer[12+_cBBO+4+4+16] = highByte(getCameraControlGainR(input));
				_packetBuffer[12+_cBBO+4+4+17] = lowByte(getCameraControlGainR(input));
				_packetBuffer[12+_cBBO+4+4+18] = highByte(getCameraControlGainG(input));
				_packetBuffer[12+_cBBO+4+4+19] = lowByte(getCameraControlGainG(input));
				_packetBuffer[12+_cBBO+4+4+20] = highByte(getCameraControlGainB(input));
				_packetBuffer[12+_cBBO+4+4+21] = lowByte(getCameraControlGainB(input));

				_packetBuffer[12+_cBBO+4+4+22] = highByte(gainY);
				_packetBuffer[12+_cBBO+4+4+23] = lowByte(gainY);


				_finishCommandPacket();

			}
			
			/**
			 * Get Clip Player; Playing
			 * mediaPlayer 	0: Media Player 1, 1: Media Player 2
			 */
			bool ATEMmax::getClipPlayerPlaying(uint8_t mediaPlayer) {
				return atemClipPlayerPlaying[mediaPlayer];
			}
			
			/**
			 * Get Clip Player; Loop
			 * mediaPlayer 	0: Media Player 1, 1: Media Player 2
			 */
			bool ATEMmax::getClipPlayerLoop(uint8_t mediaPlayer) {
				return atemClipPlayerLoop[mediaPlayer];
			}
			
			/**
			 * Get Clip Player; At Beginning
			 * mediaPlayer 	0: Media Player 1, 1: Media Player 2
			 */
			bool ATEMmax::getClipPlayerAtBeginning(uint8_t mediaPlayer) {
				return atemClipPlayerAtBeginning[mediaPlayer];
			}
			
			/**
			 * Get Clip Player; Clip Frame
			 * mediaPlayer 	0: Media Player 1, 1: Media Player 2
			 */
			uint16_t ATEMmax::getClipPlayerClipFrame(uint8_t mediaPlayer) {
				return atemClipPlayerClipFrame[mediaPlayer];
			}
			
			/**
			 * Set Clip Player; Playing
			 * mediaPlayer 	0: Media Player 1, 1: Media Player 2
			 * playing 	Bit 0: On/Off
			 */
			void ATEMmax::setClipPlayerPlaying(uint8_t mediaPlayer, bool playing) {
			
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
			void ATEMmax::setClipPlayerLoop(uint8_t mediaPlayer, bool loop) {
			
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
			void ATEMmax::setClipPlayerAtBeginning(uint8_t mediaPlayer, bool atBeginning) {
			
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
			void ATEMmax::setClipPlayerClipFrame(uint8_t mediaPlayer, uint16_t clipFrame) {
			
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
			uint8_t ATEMmax::getMediaPlayerSourceType(uint8_t mediaPlayer) {
				return atemMediaPlayerSourceType[mediaPlayer];
			}
			
			/**
			 * Get Media Player Source; Still Index
			 * mediaPlayer 	0: Media Player 1, 1: Media Player 2
			 */
			uint8_t ATEMmax::getMediaPlayerSourceStillIndex(uint8_t mediaPlayer) {
				return atemMediaPlayerSourceStillIndex[mediaPlayer];
			}
			
			/**
			 * Get Media Player Source; Clip Index
			 * mediaPlayer 	0: Media Player 1, 1: Media Player 2
			 */
			uint8_t ATEMmax::getMediaPlayerSourceClipIndex(uint8_t mediaPlayer) {
				return atemMediaPlayerSourceClipIndex[mediaPlayer];
			}
			
			/**
			 * Set Media Player Source; Type
			 * mediaPlayer 	0: Media Player 1, 1: Media Player 2
			 * type 	1: Still, 2: Clip
			 */
			void ATEMmax::setMediaPlayerSourceType(uint8_t mediaPlayer, uint8_t type) {
			
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
			void ATEMmax::setMediaPlayerSourceStillIndex(uint8_t mediaPlayer, uint8_t stillIndex) {
			
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
			void ATEMmax::setMediaPlayerSourceClipIndex(uint8_t mediaPlayer, uint8_t clipIndex) {
			
	  	  		_prepareCommandPacket(PSTR("MPSS"),8,(_packetBuffer[12+_cBBO+4+4+1]==mediaPlayer));
		
					// Set Mask: 4
				_packetBuffer[12+_cBBO+4+4+0] |= 4;
						
				_packetBuffer[12+_cBBO+4+4+1] = mediaPlayer;
				
				_packetBuffer[12+_cBBO+4+4+4] = clipIndex;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Get Media Pool Storage; Clip 1 Max Length
			 */
			uint16_t ATEMmax::getMediaPoolStorageClip1MaxLength() {
				return atemMediaPoolStorageClip1MaxLength;
			}
			
			/**
			 * Get Media Pool Storage; Clip 2 Max Length
			 */
			uint16_t ATEMmax::getMediaPoolStorageClip2MaxLength() {
				return atemMediaPoolStorageClip2MaxLength;
			}
			
			/**
			 * Set Media Pool Storage; Clip 1 Max Length
			 * clip1MaxLength 	Frames
			 */
			void ATEMmax::setMediaPoolStorageClip1MaxLength(uint16_t clip1MaxLength) {
			
	  	  		_prepareCommandPacket(PSTR("CMPS"),4);
		
				_packetBuffer[12+_cBBO+4+4+0] = highByte(clip1MaxLength);
				_packetBuffer[12+_cBBO+4+4+1] = lowByte(clip1MaxLength);
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Get Media Player Clip Source; Is Used
			 * clipBank 	0-1: Clip Bank
			 */
			bool ATEMmax::getMediaPlayerClipSourceIsUsed(uint8_t clipBank) {
				return atemMediaPlayerClipSourceIsUsed[clipBank];
			}
			
			/**
			 * Get Media Player Clip Source; File Name
			 * clipBank 	0-1: Clip Bank
			 */
			char *  ATEMmax::getMediaPlayerClipSourceFileName(uint8_t clipBank) {
				return atemMediaPlayerClipSourceFileName[clipBank];
			}
			
			/**
			 * Get Media Player Clip Source; Frames
			 * clipBank 	0-1: Clip Bank
			 */
			uint16_t ATEMmax::getMediaPlayerClipSourceFrames(uint8_t clipBank) {
				return atemMediaPlayerClipSourceFrames[clipBank];
			}
			
			/**
			 * Get Media Player Audio Source; Is Used
			 * clipBank 	1-2: Clip Bank
			 */
			bool ATEMmax::getMediaPlayerAudioSourceIsUsed(uint8_t clipBank) {
				return atemMediaPlayerAudioSourceIsUsed[clipBank];
			}
			
			/**
			 * Get Media Player Audio Source; File Name
			 * clipBank 	1-2: Clip Bank
			 */
			char *  ATEMmax::getMediaPlayerAudioSourceFileName(uint8_t clipBank) {
				return atemMediaPlayerAudioSourceFileName[clipBank];
			}
			
			/**
			 * Get Media Player Still Files; Is Used
			 * stillBank 	0-31: Still Bank
			 */
			bool ATEMmax::getMediaPlayerStillFilesIsUsed(uint8_t stillBank) {
				return atemMediaPlayerStillFilesIsUsed[stillBank];
			}
			
			/**
			 * Get Media Player Still Files; File Name
			 * stillBank 	0-31: Still Bank
			 */
			char *  ATEMmax::getMediaPlayerStillFilesFileName(uint8_t stillBank) {
				return atemMediaPlayerStillFilesFileName[stillBank];
			}
			
			/**
			 * Get Macro Run Status; State
			 */
			uint8_t ATEMmax::getMacroRunStatusState() {
				return atemMacroRunStatusState;
			}
			
			/**
			 * Get Macro Run Status; Is Looping
			 */
			bool ATEMmax::getMacroRunStatusIsLooping() {
				return atemMacroRunStatusIsLooping;
			}
			
			/**
			 * Get Macro Run Status; Index
			 */
			uint16_t ATEMmax::getMacroRunStatusIndex() {
				return atemMacroRunStatusIndex;
			}
			
			/**
			 * Set Macro Action; Action
			 * index 	0-99: Macro Index Number. 0xFFFF: stop
			 * action 	0: Run Macro, 1: Stop (w/Index 0xFFFF), 2: Stop Recording (w/Index 0xFFFF), 3: Insert Wait for User (w/Index 0xFFFF), 4: Continue (w/Index 0xFFFF), 5: Delete Macro
			 */
			void ATEMmax::setMacroAction(uint16_t index, uint8_t action) {
			
	  	  		_prepareCommandPacket(PSTR("MAct"),4,(_packetBuffer[12+_cBBO+4+4+0]==highByte(index)) && (_packetBuffer[12+_cBBO+4+4+1]==lowByte(index)));
		
				_packetBuffer[12+_cBBO+4+4+0] = highByte(index);
				_packetBuffer[12+_cBBO+4+4+1] = lowByte(index);
				
				_packetBuffer[12+_cBBO+4+4+2] = action;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Macro Run Change Properties; Looping
			 * looping 	Bit 0: On/Off
			 */
			void ATEMmax::setMacroRunChangePropertiesLooping(bool looping) {
			
	  	  		_prepareCommandPacket(PSTR("MRCP"),4);
		
					// Set Mask: 1
				_packetBuffer[12+_cBBO+4+4+0] |= 1;
						
				_packetBuffer[12+_cBBO+4+4+1] = looping;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Get Macro Properties; Is Used
			 * macroIndex 	0-9: Macro Index Number
			 */
			bool ATEMmax::getMacroPropertiesIsUsed(uint8_t macroIndex) {
				return atemMacroPropertiesIsUsed[macroIndex];
			}
			
			/**
			 * Get Macro Properties; Name
			 * macroIndex 	0-9: Macro Index Number
			 */
			char *  ATEMmax::getMacroPropertiesName(uint8_t macroIndex) {
				return atemMacroPropertiesName[macroIndex];
			}
			
			/**
			 * Set Macro Start Recording; Index
			 * index 	0-99: Macro Index Number
			 */
			void ATEMmax::setMacroStartRecordingIndex(uint8_t index) {
			
	  	  		_prepareCommandPacket(PSTR("MSRc"),8);
		
				_packetBuffer[12+_cBBO+4+4+1] = index;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Macro Add Pause; Frames
			 * frames 	Number of
			 */
			void ATEMmax::setMacroAddPauseFrames(uint16_t frames) {
			
	  	  		_prepareCommandPacket(PSTR("MSlp"),4);
		
				_packetBuffer[12+_cBBO+4+4+2] = highByte(frames);
				_packetBuffer[12+_cBBO+4+4+3] = lowByte(frames);
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Get Macro Recording Status; Is Recording
			 */
			bool ATEMmax::getMacroRecordingStatusIsRecording() {
				return atemMacroRecordingStatusIsRecording;
			}
			
			/**
			 * Get Macro Recording Status; Index
			 */
			uint16_t ATEMmax::getMacroRecordingStatusIndex() {
				return atemMacroRecordingStatusIndex;
			}
			
			/**
			 * Get Super Source; Fill Source
			 */
			uint16_t ATEMmax::getSuperSourceFillSource() {
				return atemSuperSourceFillSource;
			}
			
			/**
			 * Get Super Source; Key Source
			 */
			uint16_t ATEMmax::getSuperSourceKeySource() {
				return atemSuperSourceKeySource;
			}
			
			/**
			 * Get Super Source; Foreground
			 */
			bool ATEMmax::getSuperSourceForeground() {
				return atemSuperSourceForeground;
			}
			
			/**
			 * Get Super Source; Pre Multiplied
			 */
			bool ATEMmax::getSuperSourcePreMultiplied() {
				return atemSuperSourcePreMultiplied;
			}
			
			/**
			 * Get Super Source; Clip
			 */
			uint16_t ATEMmax::getSuperSourceClip() {
				return atemSuperSourceClip;
			}
			
			/**
			 * Get Super Source; Gain
			 */
			uint16_t ATEMmax::getSuperSourceGain() {
				return atemSuperSourceGain;
			}
			
			/**
			 * Get Super Source; Invert Key
			 */
			bool ATEMmax::getSuperSourceInvertKey() {
				return atemSuperSourceInvertKey;
			}
			
			/**
			 * Get Super Source; Border Enabled
			 */
			bool ATEMmax::getSuperSourceBorderEnabled() {
				return atemSuperSourceBorderEnabled;
			}
			
			/**
			 * Get Super Source; Border Bevel
			 */
			uint8_t ATEMmax::getSuperSourceBorderBevel() {
				return atemSuperSourceBorderBevel;
			}
			
			/**
			 * Get Super Source; Border Outer Width
			 */
			uint16_t ATEMmax::getSuperSourceBorderOuterWidth() {
				return atemSuperSourceBorderOuterWidth;
			}
			
			/**
			 * Get Super Source; Border Inner Width
			 */
			uint16_t ATEMmax::getSuperSourceBorderInnerWidth() {
				return atemSuperSourceBorderInnerWidth;
			}
			
			/**
			 * Get Super Source; Border Outer Softness
			 */
			uint8_t ATEMmax::getSuperSourceBorderOuterSoftness() {
				return atemSuperSourceBorderOuterSoftness;
			}
			
			/**
			 * Get Super Source; Border Inner Softness
			 */
			uint8_t ATEMmax::getSuperSourceBorderInnerSoftness() {
				return atemSuperSourceBorderInnerSoftness;
			}
			
			/**
			 * Get Super Source; Border Bevel Softness
			 */
			uint8_t ATEMmax::getSuperSourceBorderBevelSoftness() {
				return atemSuperSourceBorderBevelSoftness;
			}
			
			/**
			 * Get Super Source; Border Bevel Position
			 */
			uint8_t ATEMmax::getSuperSourceBorderBevelPosition() {
				return atemSuperSourceBorderBevelPosition;
			}
			
			/**
			 * Get Super Source; Border Hue
			 */
			uint16_t ATEMmax::getSuperSourceBorderHue() {
				return atemSuperSourceBorderHue;
			}
			
			/**
			 * Get Super Source; Border Saturation
			 */
			uint16_t ATEMmax::getSuperSourceBorderSaturation() {
				return atemSuperSourceBorderSaturation;
			}
			
			/**
			 * Get Super Source; Border Luma
			 */
			uint16_t ATEMmax::getSuperSourceBorderLuma() {
				return atemSuperSourceBorderLuma;
			}
			
			/**
			 * Get Super Source; Light Source Direction
			 */
			uint16_t ATEMmax::getSuperSourceLightSourceDirection() {
				return atemSuperSourceLightSourceDirection;
			}
			
			/**
			 * Get Super Source; Light Source Altitude
			 */
			uint8_t ATEMmax::getSuperSourceLightSourceAltitude() {
				return atemSuperSourceLightSourceAltitude;
			}
			
			/**
			 * Set Super Source; Fill Source
			 * fillSource 	(See video source list)
			 */
			void ATEMmax::setSuperSourceFillSource(uint16_t fillSource) {
			
	  	  		_prepareCommandPacket(PSTR("CSSc"),36);
		
					// Set Mask: 1
				_packetBuffer[12+_cBBO+4+4+3] |= 1;
						
				_packetBuffer[12+_cBBO+4+4+4] = highByte(fillSource);
				_packetBuffer[12+_cBBO+4+4+5] = lowByte(fillSource);
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Super Source; Key Source
			 * keySource 	(See video source list)
			 */
			void ATEMmax::setSuperSourceKeySource(uint16_t keySource) {
			
	  	  		_prepareCommandPacket(PSTR("CSSc"),36);
		
					// Set Mask: 2
				_packetBuffer[12+_cBBO+4+4+3] |= 2;
						
				_packetBuffer[12+_cBBO+4+4+6] = highByte(keySource);
				_packetBuffer[12+_cBBO+4+4+7] = lowByte(keySource);
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Super Source; Foreground
			 * foreground 	Bit 0: On/Off
			 */
			void ATEMmax::setSuperSourceForeground(bool foreground) {
			
	  	  		_prepareCommandPacket(PSTR("CSSc"),36);
		
					// Set Mask: 4
				_packetBuffer[12+_cBBO+4+4+3] |= 4;
						
				_packetBuffer[12+_cBBO+4+4+8] = foreground;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Super Source; Pre Multiplied
			 * preMultiplied 	Bit 0: On/Off
			 */
			void ATEMmax::setSuperSourcePreMultiplied(bool preMultiplied) {
			
	  	  		_prepareCommandPacket(PSTR("CSSc"),36);
		
					// Set Mask: 8
				_packetBuffer[12+_cBBO+4+4+3] |= 8;
						
				_packetBuffer[12+_cBBO+4+4+9] = preMultiplied;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Super Source; Clip
			 * clip 	0-1000: 0-100%
			 */
			void ATEMmax::setSuperSourceClip(uint16_t clip) {
			
	  	  		_prepareCommandPacket(PSTR("CSSc"),36);
		
					// Set Mask: 16
				_packetBuffer[12+_cBBO+4+4+3] |= 16;
						
				_packetBuffer[12+_cBBO+4+4+10] = highByte(clip);
				_packetBuffer[12+_cBBO+4+4+11] = lowByte(clip);
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Super Source; Gain
			 * gain 	0-1000: 0-100%
			 */
			void ATEMmax::setSuperSourceGain(uint16_t gain) {
			
	  	  		_prepareCommandPacket(PSTR("CSSc"),36);
		
					// Set Mask: 32
				_packetBuffer[12+_cBBO+4+4+3] |= 32;
						
				_packetBuffer[12+_cBBO+4+4+12] = highByte(gain);
				_packetBuffer[12+_cBBO+4+4+13] = lowByte(gain);
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Super Source; Invert Key
			 * invertKey 	Bit 0: On/Off
			 */
			void ATEMmax::setSuperSourceInvertKey(bool invertKey) {
			
	  	  		_prepareCommandPacket(PSTR("CSSc"),36);
		
					// Set Mask: 64
				_packetBuffer[12+_cBBO+4+4+3] |= 64;
						
				_packetBuffer[12+_cBBO+4+4+14] = invertKey;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Super Source; Border Enabled
			 * borderEnabled 	Bit 0: On/Off
			 */
			void ATEMmax::setSuperSourceBorderEnabled(bool borderEnabled) {
			
	  	  		_prepareCommandPacket(PSTR("CSSc"),36);
		
					// Set Mask: 128
				_packetBuffer[12+_cBBO+4+4+3] |= 128;
						
				_packetBuffer[12+_cBBO+4+4+15] = borderEnabled;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Super Source; Border Bevel
			 * borderBevel 	0: No, 1: In/Out, 2: In, 3: Out
			 */
			void ATEMmax::setSuperSourceBorderBevel(uint8_t borderBevel) {
			
	  	  		_prepareCommandPacket(PSTR("CSSc"),36);
		
					// Set Mask: 256
				_packetBuffer[12+_cBBO+4+4+2] |= 1;
						
				_packetBuffer[12+_cBBO+4+4+16] = borderBevel;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Super Source; Border Outer Width
			 * borderOuterWidth 	0-1600: 0-16.00
			 */
			void ATEMmax::setSuperSourceBorderOuterWidth(uint16_t borderOuterWidth) {
			
	  	  		_prepareCommandPacket(PSTR("CSSc"),36);
		
					// Set Mask: 512
				_packetBuffer[12+_cBBO+4+4+2] |= 2;
						
				_packetBuffer[12+_cBBO+4+4+18] = highByte(borderOuterWidth);
				_packetBuffer[12+_cBBO+4+4+19] = lowByte(borderOuterWidth);
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Super Source; Border Inner Width
			 * borderInnerWidth 	0-1600: 0-16.00
			 */
			void ATEMmax::setSuperSourceBorderInnerWidth(uint16_t borderInnerWidth) {
			
	  	  		_prepareCommandPacket(PSTR("CSSc"),36);
		
					// Set Mask: 1024
				_packetBuffer[12+_cBBO+4+4+2] |= 4;
						
				_packetBuffer[12+_cBBO+4+4+20] = highByte(borderInnerWidth);
				_packetBuffer[12+_cBBO+4+4+21] = lowByte(borderInnerWidth);
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Super Source; Border Outer Softness
			 * borderOuterSoftness 	0-100: 0-100%
			 */
			void ATEMmax::setSuperSourceBorderOuterSoftness(uint8_t borderOuterSoftness) {
			
	  	  		_prepareCommandPacket(PSTR("CSSc"),36);
		
					// Set Mask: 2048
				_packetBuffer[12+_cBBO+4+4+2] |= 8;
						
				_packetBuffer[12+_cBBO+4+4+22] = borderOuterSoftness;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Super Source; Border Inner Softness
			 * borderInnerSoftness 	0-100: 0-100%
			 */
			void ATEMmax::setSuperSourceBorderInnerSoftness(uint8_t borderInnerSoftness) {
			
	  	  		_prepareCommandPacket(PSTR("CSSc"),36);
		
					// Set Mask: 4096
				_packetBuffer[12+_cBBO+4+4+2] |= 16;
						
				_packetBuffer[12+_cBBO+4+4+23] = borderInnerSoftness;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Super Source; Border Bevel Softness
			 * borderBevelSoftness 	0-100: 0.0-1.0
			 */
			void ATEMmax::setSuperSourceBorderBevelSoftness(uint8_t borderBevelSoftness) {
			
	  	  		_prepareCommandPacket(PSTR("CSSc"),36);
		
					// Set Mask: 8192
				_packetBuffer[12+_cBBO+4+4+2] |= 32;
						
				_packetBuffer[12+_cBBO+4+4+24] = borderBevelSoftness;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Super Source; Border Bevel Position
			 * borderBevelPosition 	0-100: 0.0-1.0
			 */
			void ATEMmax::setSuperSourceBorderBevelPosition(uint8_t borderBevelPosition) {
			
	  	  		_prepareCommandPacket(PSTR("CSSc"),36);
		
					// Set Mask: 16384
				_packetBuffer[12+_cBBO+4+4+2] |= 64;
						
				_packetBuffer[12+_cBBO+4+4+25] = borderBevelPosition;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Super Source; Border Hue
			 * borderHue 	0-3599: 0-359.9 Degrees
			 */
			void ATEMmax::setSuperSourceBorderHue(uint16_t borderHue) {
			
	  	  		_prepareCommandPacket(PSTR("CSSc"),36);
		
					// Set Mask: 32768
				_packetBuffer[12+_cBBO+4+4+2] |= 128;
						
				_packetBuffer[12+_cBBO+4+4+26] = highByte(borderHue);
				_packetBuffer[12+_cBBO+4+4+27] = lowByte(borderHue);
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Super Source; Border Saturation
			 * borderSaturation 	0-1000: 0-100%
			 */
			void ATEMmax::setSuperSourceBorderSaturation(uint16_t borderSaturation) {
			
	  	  		_prepareCommandPacket(PSTR("CSSc"),36);
		
					// Set Mask: 65536
				_packetBuffer[12+_cBBO+4+4+1] |= 1;
						
				_packetBuffer[12+_cBBO+4+4+28] = highByte(borderSaturation);
				_packetBuffer[12+_cBBO+4+4+29] = lowByte(borderSaturation);
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Super Source; Border Luma
			 * borderLuma 	0-1000: 0-100%
			 */
			void ATEMmax::setSuperSourceBorderLuma(uint16_t borderLuma) {
			
	  	  		_prepareCommandPacket(PSTR("CSSc"),36);
		
					// Set Mask: 131072
				_packetBuffer[12+_cBBO+4+4+1] |= 2;
						
				_packetBuffer[12+_cBBO+4+4+30] = highByte(borderLuma);
				_packetBuffer[12+_cBBO+4+4+31] = lowByte(borderLuma);
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Super Source; Light Source Direction
			 * lightSourceDirection 	0-3590: 0-359 Degrees
			 */
			void ATEMmax::setSuperSourceLightSourceDirection(uint16_t lightSourceDirection) {
			
	  	  		_prepareCommandPacket(PSTR("CSSc"),36);
		
					// Set Mask: 262144
				_packetBuffer[12+_cBBO+4+4+1] |= 4;
						
				_packetBuffer[12+_cBBO+4+4+32] = highByte(lightSourceDirection);
				_packetBuffer[12+_cBBO+4+4+33] = lowByte(lightSourceDirection);
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Super Source; Light Source Altitude
			 * lightSourceAltitude 	10-100: 10-100
			 */
			void ATEMmax::setSuperSourceLightSourceAltitude(uint8_t lightSourceAltitude) {
			
	  	  		_prepareCommandPacket(PSTR("CSSc"),36);
		
					// Set Mask: 524288
				_packetBuffer[12+_cBBO+4+4+1] |= 8;
						
				_packetBuffer[12+_cBBO+4+4+34] = lightSourceAltitude;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Get Super Source Box Parameters; Enabled
			 * box 	0-3: Box 1-4
			 */
			bool ATEMmax::getSuperSourceBoxParametersEnabled(uint8_t box) {
				return atemSuperSourceBoxParametersEnabled[box];
			}
			
			/**
			 * Get Super Source Box Parameters; Input Source
			 * box 	0-3: Box 1-4
			 */
			uint16_t ATEMmax::getSuperSourceBoxParametersInputSource(uint8_t box) {
				return atemSuperSourceBoxParametersInputSource[box];
			}
			
			/**
			 * Get Super Source Box Parameters; Position X
			 * box 	0-3: Box 1-4
			 */
			int16_t ATEMmax::getSuperSourceBoxParametersPositionX(uint8_t box) {
				return atemSuperSourceBoxParametersPositionX[box];
			}
			
			/**
			 * Get Super Source Box Parameters; Position Y
			 * box 	0-3: Box 1-4
			 */
			int16_t ATEMmax::getSuperSourceBoxParametersPositionY(uint8_t box) {
				return atemSuperSourceBoxParametersPositionY[box];
			}
			
			/**
			 * Get Super Source Box Parameters; Size
			 * box 	0-3: Box 1-4
			 */
			uint16_t ATEMmax::getSuperSourceBoxParametersSize(uint8_t box) {
				return atemSuperSourceBoxParametersSize[box];
			}
			
			/**
			 * Get Super Source Box Parameters; Cropped
			 * box 	0-3: Box 1-4
			 */
			bool ATEMmax::getSuperSourceBoxParametersCropped(uint8_t box) {
				return atemSuperSourceBoxParametersCropped[box];
			}
			
			/**
			 * Get Super Source Box Parameters; Crop Top
			 * box 	0-3: Box 1-4
			 */
			uint16_t ATEMmax::getSuperSourceBoxParametersCropTop(uint8_t box) {
				return atemSuperSourceBoxParametersCropTop[box];
			}
			
			/**
			 * Get Super Source Box Parameters; Crop Bottom
			 * box 	0-3: Box 1-4
			 */
			uint16_t ATEMmax::getSuperSourceBoxParametersCropBottom(uint8_t box) {
				return atemSuperSourceBoxParametersCropBottom[box];
			}
			
			/**
			 * Get Super Source Box Parameters; Crop Left
			 * box 	0-3: Box 1-4
			 */
			uint16_t ATEMmax::getSuperSourceBoxParametersCropLeft(uint8_t box) {
				return atemSuperSourceBoxParametersCropLeft[box];
			}
			
			/**
			 * Get Super Source Box Parameters; Crop Right
			 * box 	0-3: Box 1-4
			 */
			uint16_t ATEMmax::getSuperSourceBoxParametersCropRight(uint8_t box) {
				return atemSuperSourceBoxParametersCropRight[box];
			}
			
			/**
			 * Set Super Source Box Parameters; Enabled
			 * box 	0-3: Box 1-4
			 * enabled 	Bit 0: On/Off
			 */
			void ATEMmax::setSuperSourceBoxParametersEnabled(uint8_t box, bool enabled) {
			
	  	  		_prepareCommandPacket(PSTR("CSBP"),24,(_packetBuffer[12+_cBBO+4+4+2]==box));
		
					// Set Mask: 1
				_packetBuffer[12+_cBBO+4+4+1] |= 1;
						
				_packetBuffer[12+_cBBO+4+4+2] = box;
				
				_packetBuffer[12+_cBBO+4+4+3] = enabled;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Super Source Box Parameters; Input Source
			 * box 	0-3: Box 1-4
			 * inputSource 	(See video source list)
			 */
			void ATEMmax::setSuperSourceBoxParametersInputSource(uint8_t box, uint16_t inputSource) {
			
	  	  		_prepareCommandPacket(PSTR("CSBP"),24,(_packetBuffer[12+_cBBO+4+4+2]==box));
		
					// Set Mask: 2
				_packetBuffer[12+_cBBO+4+4+1] |= 2;
						
				_packetBuffer[12+_cBBO+4+4+2] = box;
				
				_packetBuffer[12+_cBBO+4+4+4] = highByte(inputSource);
				_packetBuffer[12+_cBBO+4+4+5] = lowByte(inputSource);
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Super Source Box Parameters; Position X
			 * box 	0-3: Box 1-4
			 * positionX 	-4800-4800: -48.00-48.00
			 */
			void ATEMmax::setSuperSourceBoxParametersPositionX(uint8_t box, int16_t positionX) {
			
	  	  		_prepareCommandPacket(PSTR("CSBP"),24,(_packetBuffer[12+_cBBO+4+4+2]==box));
		
					// Set Mask: 4
				_packetBuffer[12+_cBBO+4+4+1] |= 4;
						
				_packetBuffer[12+_cBBO+4+4+2] = box;
				
				_packetBuffer[12+_cBBO+4+4+6] = highByte(positionX);
				_packetBuffer[12+_cBBO+4+4+7] = lowByte(positionX);
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Super Source Box Parameters; Position Y
			 * box 	0-3: Box 1-4
			 * positionY 	-2700-2700: -27.00-27.00
			 */
			void ATEMmax::setSuperSourceBoxParametersPositionY(uint8_t box, int16_t positionY) {
			
	  	  		_prepareCommandPacket(PSTR("CSBP"),24,(_packetBuffer[12+_cBBO+4+4+2]==box));
		
					// Set Mask: 8
				_packetBuffer[12+_cBBO+4+4+1] |= 8;
						
				_packetBuffer[12+_cBBO+4+4+2] = box;
				
				_packetBuffer[12+_cBBO+4+4+8] = highByte(positionY);
				_packetBuffer[12+_cBBO+4+4+9] = lowByte(positionY);
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Super Source Box Parameters; Size
			 * box 	0-3: Box 1-4
			 * size 	70-1000: 0.07-1.00
			 */
			void ATEMmax::setSuperSourceBoxParametersSize(uint8_t box, uint16_t size) {
			
	  	  		_prepareCommandPacket(PSTR("CSBP"),24,(_packetBuffer[12+_cBBO+4+4+2]==box));
		
					// Set Mask: 16
				_packetBuffer[12+_cBBO+4+4+1] |= 16;
						
				_packetBuffer[12+_cBBO+4+4+2] = box;
				
				_packetBuffer[12+_cBBO+4+4+10] = highByte(size);
				_packetBuffer[12+_cBBO+4+4+11] = lowByte(size);
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Super Source Box Parameters; Cropped
			 * box 	0-3: Box 1-4
			 * cropped 	Bit 0: On/Off
			 */
			void ATEMmax::setSuperSourceBoxParametersCropped(uint8_t box, bool cropped) {
			
	  	  		_prepareCommandPacket(PSTR("CSBP"),24,(_packetBuffer[12+_cBBO+4+4+2]==box));
		
					// Set Mask: 32
				_packetBuffer[12+_cBBO+4+4+1] |= 32;
						
				_packetBuffer[12+_cBBO+4+4+2] = box;
				
				_packetBuffer[12+_cBBO+4+4+12] = cropped;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Super Source Box Parameters; Crop Top
			 * box 	0-3: Box 1-4
			 * cropTop 	0-18000: 0.0-18.0
			 */
			void ATEMmax::setSuperSourceBoxParametersCropTop(uint8_t box, uint16_t cropTop) {
			
	  	  		_prepareCommandPacket(PSTR("CSBP"),24,(_packetBuffer[12+_cBBO+4+4+2]==box));
		
					// Set Mask: 64
				_packetBuffer[12+_cBBO+4+4+1] |= 64;
						
				_packetBuffer[12+_cBBO+4+4+2] = box;
				
				_packetBuffer[12+_cBBO+4+4+14] = highByte(cropTop);
				_packetBuffer[12+_cBBO+4+4+15] = lowByte(cropTop);
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Super Source Box Parameters; Crop Bottom
			 * box 	0-3: Box 1-4
			 * cropBottom 	0-18000: 0.0-18.0
			 */
			void ATEMmax::setSuperSourceBoxParametersCropBottom(uint8_t box, uint16_t cropBottom) {
			
	  	  		_prepareCommandPacket(PSTR("CSBP"),24,(_packetBuffer[12+_cBBO+4+4+2]==box));
		
					// Set Mask: 128
				_packetBuffer[12+_cBBO+4+4+1] |= 128;
						
				_packetBuffer[12+_cBBO+4+4+2] = box;
				
				_packetBuffer[12+_cBBO+4+4+16] = highByte(cropBottom);
				_packetBuffer[12+_cBBO+4+4+17] = lowByte(cropBottom);
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Super Source Box Parameters; Crop Left
			 * box 	0-3: Box 1-4
			 * cropLeft 	0-32000: 0.0-32.0
			 */
			void ATEMmax::setSuperSourceBoxParametersCropLeft(uint8_t box, uint16_t cropLeft) {
			
	  	  		_prepareCommandPacket(PSTR("CSBP"),24,(_packetBuffer[12+_cBBO+4+4+2]==box));
		
					// Set Mask: 256
				_packetBuffer[12+_cBBO+4+4+0] |= 1;
						
				_packetBuffer[12+_cBBO+4+4+2] = box;
				
				_packetBuffer[12+_cBBO+4+4+18] = highByte(cropLeft);
				_packetBuffer[12+_cBBO+4+4+19] = lowByte(cropLeft);
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Super Source Box Parameters; Crop Right
			 * box 	0-3: Box 1-4
			 * cropRight 	0-32000: 0.0-32.0
			 */
			void ATEMmax::setSuperSourceBoxParametersCropRight(uint8_t box, uint16_t cropRight) {
			
	  	  		_prepareCommandPacket(PSTR("CSBP"),24,(_packetBuffer[12+_cBBO+4+4+2]==box));
		
					// Set Mask: 512
				_packetBuffer[12+_cBBO+4+4+0] |= 2;
						
				_packetBuffer[12+_cBBO+4+4+2] = box;
				
				_packetBuffer[12+_cBBO+4+4+20] = highByte(cropRight);
				_packetBuffer[12+_cBBO+4+4+21] = lowByte(cropRight);
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Get Audio Mixer Input; Type
			 * audioSource 	(See audio source list)
			 */
			uint8_t ATEMmax::getAudioMixerInputType(uint16_t audioSource) {
				return atemAudioMixerInputType[getAudioSrcIndex(audioSource)];
			}
			
			/**
			 * Get Audio Mixer Input; From Media Player
			 * audioSource 	(See audio source list)
			 */
			bool ATEMmax::getAudioMixerInputFromMediaPlayer(uint16_t audioSource) {
				return atemAudioMixerInputFromMediaPlayer[getAudioSrcIndex(audioSource)];
			}
			
			/**
			 * Get Audio Mixer Input; Plug type
			 * audioSource 	(See audio source list)
			 */
			uint8_t ATEMmax::getAudioMixerInputPlugtype(uint16_t audioSource) {
				return atemAudioMixerInputPlugtype[getAudioSrcIndex(audioSource)];
			}
			
			/**
			 * Get Audio Mixer Input; Mix Option
			 * audioSource 	(See audio source list)
			 */
			uint8_t ATEMmax::getAudioMixerInputMixOption(uint16_t audioSource) {
				return atemAudioMixerInputMixOption[getAudioSrcIndex(audioSource)];
			}
			
			/**
			 * Get Audio Mixer Input; Volume
			 * audioSource 	(See audio source list)
			 */
			uint16_t ATEMmax::getAudioMixerInputVolume(uint16_t audioSource) {
				return atemAudioMixerInputVolume[getAudioSrcIndex(audioSource)];
			}
			
			/**
			 * Get Audio Mixer Input; Balance
			 * audioSource 	(See audio source list)
			 */
			int16_t ATEMmax::getAudioMixerInputBalance(uint16_t audioSource) {
				return atemAudioMixerInputBalance[getAudioSrcIndex(audioSource)];
			}
			
			/**
			 * Set Audio Mixer Input; Mix Option
			 * audioSource 	(See audio source list)
			 * mixOption 	0: Off, 1: On, 2: AFV
			 */
			void ATEMmax::setAudioMixerInputMixOption(uint16_t audioSource, uint8_t mixOption) {
			
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
			void ATEMmax::setAudioMixerInputVolume(uint16_t audioSource, uint16_t volume) {
			
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
			void ATEMmax::setAudioMixerInputBalance(uint16_t audioSource, int16_t balance) {
			
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
			 * Get Audio Mixer Master; Volume
			 */
			uint16_t ATEMmax::getAudioMixerMasterVolume() {
				return atemAudioMixerMasterVolume;
			}
			
			/**
			 * Set Audio Mixer Master; Volume
			 * volume 	0-65381: (DB)
			 */
			void ATEMmax::setAudioMixerMasterVolume(uint16_t volume) {
			
	  	  		_prepareCommandPacket(PSTR("CAMM"),8);
		
					// Set Mask: 1
				_packetBuffer[12+_cBBO+4+4+0] |= 1;
						
				_packetBuffer[12+_cBBO+4+4+2] = highByte(volume);
				_packetBuffer[12+_cBBO+4+4+3] = lowByte(volume);
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Get Audio Mixer Monitor; Monitor Audio
			 */
			bool ATEMmax::getAudioMixerMonitorMonitorAudio() {
				return atemAudioMixerMonitorMonitorAudio;
			}
			
			/**
			 * Get Audio Mixer Monitor; Volume
			 */
			uint16_t ATEMmax::getAudioMixerMonitorVolume() {
				return atemAudioMixerMonitorVolume;
			}
			
			/**
			 * Get Audio Mixer Monitor; Mute
			 */
			bool ATEMmax::getAudioMixerMonitorMute() {
				return atemAudioMixerMonitorMute;
			}
			
			/**
			 * Get Audio Mixer Monitor; Solo
			 */
			bool ATEMmax::getAudioMixerMonitorSolo() {
				return atemAudioMixerMonitorSolo;
			}
			
			/**
			 * Get Audio Mixer Monitor; Solo Input
			 */
			uint16_t ATEMmax::getAudioMixerMonitorSoloInput() {
				return atemAudioMixerMonitorSoloInput;
			}
			
			/**
			 * Get Audio Mixer Monitor; Dim
			 */
			bool ATEMmax::getAudioMixerMonitorDim() {
				return atemAudioMixerMonitorDim;
			}
			
			/**
			 * Set Audio Mixer Monitor; Monitor Audio
			 * monitorAudio 	Bit 0: On/Off
			 */
			void ATEMmax::setAudioMixerMonitorMonitorAudio(bool monitorAudio) {
			
	  	  		_prepareCommandPacket(PSTR("CAMm"),12);
		
					// Set Mask: 1
				_packetBuffer[12+_cBBO+4+4+0] |= 1;
						
				_packetBuffer[12+_cBBO+4+4+1] = monitorAudio;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Audio Mixer Monitor; Volume
			 * volume 	0-65381: (DB)
			 */
			void ATEMmax::setAudioMixerMonitorVolume(uint16_t volume) {
			
	  	  		_prepareCommandPacket(PSTR("CAMm"),12);
		
					// Set Mask: 2
				_packetBuffer[12+_cBBO+4+4+0] |= 2;
						
				_packetBuffer[12+_cBBO+4+4+2] = highByte(volume);
				_packetBuffer[12+_cBBO+4+4+3] = lowByte(volume);
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Audio Mixer Monitor; Mute
			 * mute 	Bit 0: On/Off
			 */
			void ATEMmax::setAudioMixerMonitorMute(bool mute) {
			
	  	  		_prepareCommandPacket(PSTR("CAMm"),12);
		
					// Set Mask: 4
				_packetBuffer[12+_cBBO+4+4+0] |= 4;
						
				_packetBuffer[12+_cBBO+4+4+4] = mute;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Audio Mixer Monitor; Solo
			 * solo 	Bit 0: On/Off
			 */
			void ATEMmax::setAudioMixerMonitorSolo(bool solo) {
			
	  	  		_prepareCommandPacket(PSTR("CAMm"),12);
		
					// Set Mask: 8
				_packetBuffer[12+_cBBO+4+4+0] |= 8;
						
				_packetBuffer[12+_cBBO+4+4+5] = solo;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Audio Mixer Monitor; Solo Input
			 * soloInput 	(See audio source list)
			 */
			void ATEMmax::setAudioMixerMonitorSoloInput(uint16_t soloInput) {
			
	  	  		_prepareCommandPacket(PSTR("CAMm"),12);
		
					// Set Mask: 16
				_packetBuffer[12+_cBBO+4+4+0] |= 16;
						
				_packetBuffer[12+_cBBO+4+4+6] = highByte(soloInput);
				_packetBuffer[12+_cBBO+4+4+7] = lowByte(soloInput);
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Audio Mixer Monitor; Dim
			 * dim 	Bit 0: On/Off
			 */
			void ATEMmax::setAudioMixerMonitorDim(bool dim) {
			
	  	  		_prepareCommandPacket(PSTR("CAMm"),12);
		
					// Set Mask: 32
				_packetBuffer[12+_cBBO+4+4+0] |= 32;
						
				_packetBuffer[12+_cBBO+4+4+8] = dim;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Audio Levels; Enable
			 * enable 	Bit 0: On/Off
			 */
			void ATEMmax::setAudioLevelsEnable(bool enable) {
			
	  	  		_prepareCommandPacket(PSTR("SALN"),4);
		
				_packetBuffer[12+_cBBO+4+4+0] = enable;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Get Audio Mixer Levels; Sources
			 */
			uint16_t ATEMmax::getAudioMixerLevelsSources() {
				return atemAudioMixerLevelsSources;
			}
			
			/**
			 * Get Audio Mixer Levels; Master Left
			 */
			int32_t ATEMmax::getAudioMixerLevelsMasterLeft() {
				return atemAudioMixerLevelsMasterLeft;
			}
			
			/**
			 * Get Audio Mixer Levels; Master Right
			 */
			int32_t ATEMmax::getAudioMixerLevelsMasterRight() {
				return atemAudioMixerLevelsMasterRight;
			}
			
			/**
			 * Get Audio Mixer Levels; Master Peak Left
			 */
			int32_t ATEMmax::getAudioMixerLevelsMasterPeakLeft() {
				return atemAudioMixerLevelsMasterPeakLeft;
			}
			
			/**
			 * Get Audio Mixer Levels; Master Peak Right
			 */
			int32_t ATEMmax::getAudioMixerLevelsMasterPeakRight() {
				return atemAudioMixerLevelsMasterPeakRight;
			}
			
			/**
			 * Get Audio Mixer Levels; Monitor
			 */
			int32_t ATEMmax::getAudioMixerLevelsMonitor() {
				return atemAudioMixerLevelsMonitor;
			}
			
			/**
			 * Get Audio Mixer Levels; Source Order
			 * sources 	0-24: Number of
			 */
			uint16_t ATEMmax::getAudioMixerLevelsSourceOrder(uint16_t sources) {
				return atemAudioMixerLevelsSourceOrder[sources];
			}
			
			/**
			 * Get Audio Mixer Levels; Source Left
			 * sources 	0-24: Number of
			 */
			int32_t ATEMmax::getAudioMixerLevelsSourceLeft(uint16_t sources) {
				return atemAudioMixerLevelsSourceLeft[sources];
			}
			
			/**
			 * Get Audio Mixer Levels; Source Right
			 * sources 	0-24: Number of
			 */
			int32_t ATEMmax::getAudioMixerLevelsSourceRight(uint16_t sources) {
				return atemAudioMixerLevelsSourceRight[sources];
			}
			
			/**
			 * Get Audio Mixer Levels; Source Peak Left
			 * sources 	0-24: Number of
			 */
			int32_t ATEMmax::getAudioMixerLevelsSourcePeakLeft(uint16_t sources) {
				return atemAudioMixerLevelsSourcePeakLeft[sources];
			}
			
			/**
			 * Get Audio Mixer Levels; Source Peak Right
			 * sources 	0-24: Number of
			 */
			int32_t ATEMmax::getAudioMixerLevelsSourcePeakRight(uint16_t sources) {
				return atemAudioMixerLevelsSourcePeakRight[sources];
			}
			
			/**
			 * Set Reset Audio Mixer Peaks; Input Source
			 * inputSource 	(See audio source list)
			 */
			void ATEMmax::setResetAudioMixerPeaksInputSource(uint16_t inputSource) {
			
	  	  		_prepareCommandPacket(PSTR("RAMP"),8);
		
					// Set Mask: 2
				_packetBuffer[12+_cBBO+4+4+0] |= 2;
						
				_packetBuffer[12+_cBBO+4+4+2] = highByte(inputSource);
				_packetBuffer[12+_cBBO+4+4+3] = lowByte(inputSource);
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Reset Audio Mixer Peaks; Master
			 * master 	Bit 0: Yes/No
			 */
			void ATEMmax::setResetAudioMixerPeaksMaster(bool master) {
			
	  	  		_prepareCommandPacket(PSTR("RAMP"),8);
		
					// Set Mask: 4
				_packetBuffer[12+_cBBO+4+4+0] |= 4;
						
				_packetBuffer[12+_cBBO+4+4+4] = master;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Get Audio Mixer Tally; Sources
			 */
			uint16_t ATEMmax::getAudioMixerTallySources() {
				return atemAudioMixerTallySources;
			}
			
			/**
			 * Get Audio Mixer Tally; Audio Source
			 * sources 	0-24: Number of
			 */
			uint16_t ATEMmax::getAudioMixerTallyAudioSource(uint16_t sources) {
				return atemAudioMixerTallyAudioSource[sources];
			}
			
			/**
			 * Get Audio Mixer Tally; IsMixedIn
			 * sources 	0-24: Number of
			 */
			bool ATEMmax::getAudioMixerTallyIsMixedIn(uint16_t sources) {
				return atemAudioMixerTallyIsMixedIn[sources];
			}
			
			/**
			 * Get Tally By Index; Sources
			 */
			uint16_t ATEMmax::getTallyByIndexSources() {
				return atemTallyByIndexSources;
			}
			
			/**
			 * Get Tally By Index; Tally Flags
			 * sources 	0-20: Number of
			 */
			uint8_t ATEMmax::getTallyByIndexTallyFlags(uint16_t sources) {
				return atemTallyByIndexTallyFlags[sources];
			}
			
			/**
			 * Get Tally By Source; Sources
			 */
			uint16_t ATEMmax::getTallyBySourceSources() {
				return atemTallyBySourceSources;
			}
			
			/**
			 * Get Tally By Source; Video Source
			 * sources 	0-46: Number of
			 */
			uint16_t ATEMmax::getTallyBySourceVideoSource(uint16_t sources) {
				return atemTallyBySourceVideoSource[sources];
			}
			
			/**
			 * Get Tally By Source; Tally Flags
			 * sources 	0-46: Number of
			 */
			uint8_t ATEMmax::getTallyBySourceTallyFlags(uint16_t sources) {
				return atemTallyBySourceTallyFlags[sources];
			}
			
			/**
			 * Get Last State Change Time Code; Hour
			 */
			uint8_t ATEMmax::getLastStateChangeTimeCodeHour() {
				return atemLastStateChangeTimeCodeHour;
			}
			
			/**
			 * Get Last State Change Time Code; Minute
			 */
			uint8_t ATEMmax::getLastStateChangeTimeCodeMinute() {
				return atemLastStateChangeTimeCodeMinute;
			}
			
			/**
			 * Get Last State Change Time Code; Second
			 */
			uint8_t ATEMmax::getLastStateChangeTimeCodeSecond() {
				return atemLastStateChangeTimeCodeSecond;
			}
			
			/**
			 * Get Last State Change Time Code; Frame
			 */
			uint8_t ATEMmax::getLastStateChangeTimeCodeFrame() {
				return atemLastStateChangeTimeCodeFrame;
			}
			

	