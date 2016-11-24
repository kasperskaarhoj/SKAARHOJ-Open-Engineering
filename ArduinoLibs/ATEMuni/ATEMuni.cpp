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



#include "Arduino.h"
#include "ATEMuni.h"


/**
 * Constructor (using arguments is deprecated! Use begin() instead)
 */
ATEMuni::ATEMuni(){}



uint8_t ATEMuni::getTallyFlags(uint16_t videoSource)  {
  for (uint8_t a = 0; a < getTallyBySourceSources(); a++)  {
    if (getTallyBySourceVideoSource(a) == videoSource)  {
    	return getTallyBySourceTallyFlags(a);
    }
  }
  return 0;
}
uint8_t ATEMuni::getAudioTallyFlags(uint16_t audioSource)  {
  for (uint8_t a = 0; a < getAudioMixerTallySources(); a++)  {
    if (getAudioMixerTallyAudioSource(a) == audioSource)  {
    	return getAudioMixerTallyIsMixedIn(a);
    }
  }
  return 0;
}

void ATEMuni::setCameraControlVideomode(uint8_t input, uint8_t fps, uint8_t resolution, uint8_t interlaced) {
		_prepareCommandPacket(PSTR("CCmd"), 24);

		_packetBuffer[12+_cBBO+4+4+0] = input;

		_packetBuffer[12+_cBBO+4+4+1] = 1;
		_packetBuffer[12+_cBBO+4+4+2] = 0;

		_packetBuffer[12+_cBBO+4+4+4] = 0x01; // Data type: int8

		_packetBuffer[12+_cBBO+4+4+7] = 0x05; // 5 Byte array

		_packetBuffer[12+_cBBO+4+4+16] = fps;
		_packetBuffer[12+_cBBO+4+4+17] = 0x00; // Regular M-rate
		_packetBuffer[12+_cBBO+4+4+18] = resolution;
		_packetBuffer[12+_cBBO+4+4+19] = interlaced;
		_packetBuffer[12+_cBBO+4+4+20] = 0x00; // YUV

		_finishCommandPacket();
}


	void ATEMuni::setCameraControlLift(uint8_t input, int liftR, int liftG, int liftB, int liftY) {
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

	void ATEMuni::setCameraControlGamma(uint8_t input, int gammaR, int gammaG, int gammaB, int gammaY) {
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

	void ATEMuni::setCameraControlGain(uint8_t input, int gainR, int gainG, int gainB, int gainY) {
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

	void ATEMuni::setCameraControlHueSaturation(uint8_t input, int hue, int saturation) {
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
		// ** Implementations in ATEMuni.c:
		// **
		// *********************************

		void ATEMuni::_parseGetCommands(const char *cmdStr)	{
			uint8_t multiViewer,windowIndex,mE,keyer,aUXChannel,input,mediaPlayer,stillBank,macroIndex;
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
			if(!strcmp_P(cmdStr, PSTR("KeDV"))) {
				
				mE = _packetBuffer[0];
				keyer = _packetBuffer[1];
				if (mE<=1 && keyer<=3) {
					#if ATEM_debug
					temp = atemKeyDVESizeX[mE][keyer];
					#endif
					atemKeyDVESizeX[mE][keyer] = (unsigned long)_packetBuffer[4]<<24 | (unsigned long)_packetBuffer[5]<<16 | (unsigned long)_packetBuffer[6]<<8 | (unsigned long)_packetBuffer[7];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyDVESizeX[mE][keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyDVESizeX[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("] = "));
						Serial.println(atemKeyDVESizeX[mE][keyer]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyDVESizeY[mE][keyer];
					#endif
					atemKeyDVESizeY[mE][keyer] = (unsigned long)_packetBuffer[8]<<24 | (unsigned long)_packetBuffer[9]<<16 | (unsigned long)_packetBuffer[10]<<8 | (unsigned long)_packetBuffer[11];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyDVESizeY[mE][keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyDVESizeY[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("] = "));
						Serial.println(atemKeyDVESizeY[mE][keyer]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyDVEPositionX[mE][keyer];
					#endif
					atemKeyDVEPositionX[mE][keyer] = (unsigned long)_packetBuffer[12]<<24 | (unsigned long)_packetBuffer[13]<<16 | (unsigned long)_packetBuffer[14]<<8 | (unsigned long)_packetBuffer[15];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyDVEPositionX[mE][keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyDVEPositionX[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("] = "));
						Serial.println(atemKeyDVEPositionX[mE][keyer]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyDVEPositionY[mE][keyer];
					#endif
					atemKeyDVEPositionY[mE][keyer] = (unsigned long)_packetBuffer[16]<<24 | (unsigned long)_packetBuffer[17]<<16 | (unsigned long)_packetBuffer[18]<<8 | (unsigned long)_packetBuffer[19];
					#if ATEM_debug
					if ((_serialOutput==0x80 && atemKeyDVEPositionY[mE][keyer]!=temp) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemKeyDVEPositionY[mE=")); Serial.print(mE); Serial.print(F("][keyer=")); Serial.print(keyer); Serial.print(F("] = "));
						Serial.println(atemKeyDVEPositionY[mE][keyer]);
					}
					#endif
					
					#if ATEM_debug
					temp = atemKeyDVERotation[mE][keyer];
					#endif
					atemKeyDVERotation[mE][keyer] = (unsigned long)_packetBuffer[20]<<24 | (unsigned long)_packetBuffer[21]<<16 | (unsigned long)_packetBuffer[22]<<8 | (unsigned long)_packetBuffer[23];
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
				if (input<=8) {
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
					strncpy(atemMacroPropertiesName[macroIndex], (char *)(_packetBuffer+8), 10);
					#if ATEM_debug
					if ((_serialOutput==0x80 && hasInitialized()) || (_serialOutput==0x81 && !hasInitialized()))	{
						Serial.print(F("atemMacroPropertiesName[macroIndex=")); Serial.print(macroIndex); Serial.print(F("] = "));
						Serial.println(atemMacroPropertiesName[macroIndex]);
					}
					#endif
					
				}
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
			if(!strcmp_P(cmdStr, PSTR("TlSr"))) {
				
				sources = word(_packetBuffer[0],_packetBuffer[1]);
				if (sources<=41) {
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
			{}
		}





			/**
			 * Get Input Properties; Long Name
			 * videoSource 	(See video source list)
			 */
			char *  ATEMuni::getInputLongName(uint16_t videoSource) {
				return atemInputLongName[getVideoSrcIndex(videoSource)];
			}
			
			/**
			 * Get Input Properties; Short Name
			 * videoSource 	(See video source list)
			 */
			char *  ATEMuni::getInputShortName(uint16_t videoSource) {
				return atemInputShortName[getVideoSrcIndex(videoSource)];
			}
			
			/**
			 * Get Input Properties; Availability
			 * videoSource 	(See video source list)
			 */
			uint8_t ATEMuni::getInputAvailability(uint16_t videoSource) {
				return atemInputAvailability[getVideoSrcIndex(videoSource)];
			}
			
			/**
			 * Get Input Properties; ME Availability
			 * videoSource 	(See video source list)
			 */
			uint8_t ATEMuni::getInputMEAvailability(uint16_t videoSource) {
				return atemInputMEAvailability[getVideoSrcIndex(videoSource)];
			}
			
			/**
			 * Get Multi Viewer Input; Video Source
			 * multiViewer 	0: 1, 1: 2
			 * windowIndex 	0-9
			 */
			uint16_t ATEMuni::getMultiViewerInputVideoSource(uint8_t multiViewer, uint8_t windowIndex) {
				return atemMultiViewerInputVideoSource[multiViewer][windowIndex];
			}
			
			/**
			 * Get Program Input; Video Source
			 * mE 	0: ME1, 1: ME2
			 */
			uint16_t ATEMuni::getProgramInputVideoSource(uint8_t mE) {
				return atemProgramInputVideoSource[mE];
			}
			
			/**
			 * Set Program Input; Video Source
			 * mE 	0: ME1, 1: ME2
			 * videoSource 	(See video source list)
			 */
			void ATEMuni::setProgramInputVideoSource(uint8_t mE, uint16_t videoSource) {
			
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
			uint16_t ATEMuni::getPreviewInputVideoSource(uint8_t mE) {
				return atemPreviewInputVideoSource[mE];
			}
			
			/**
			 * Set Preview Input; Video Source
			 * mE 	0: ME1, 1: ME2
			 * videoSource 	(See video source list)
			 */
			void ATEMuni::setPreviewInputVideoSource(uint8_t mE, uint16_t videoSource) {
			
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
			void ATEMuni::performCutME(uint8_t mE) {
			
	  	  		_prepareCommandPacket(PSTR("DCut"),4);
		
				_packetBuffer[12+_cBBO+4+4+0] = mE;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Auto; M/E
			 * mE 	0: ME1, 1: ME2
			 */
			void ATEMuni::performAutoME(uint8_t mE) {
			
	  	  		_prepareCommandPacket(PSTR("DAut"),4);
		
				_packetBuffer[12+_cBBO+4+4+0] = mE;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Get Transition; Style
			 * mE 	0: ME1, 1: ME2
			 */
			uint8_t ATEMuni::getTransitionStyle(uint8_t mE) {
				return atemTransitionStyle[mE];
			}
			
			/**
			 * Get Transition; Next Transition
			 * mE 	0: ME1, 1: ME2
			 */
			uint8_t ATEMuni::getTransitionNextTransition(uint8_t mE) {
				return atemTransitionNextTransition[mE];
			}
			
			/**
			 * Get Transition; Style Next
			 * mE 	0: ME1, 1: ME2
			 */
			uint8_t ATEMuni::getTransitionStyleNext(uint8_t mE) {
				return atemTransitionStyleNext[mE];
			}
			
			/**
			 * Get Transition; Next Transition Next
			 * mE 	0: ME1, 1: ME2
			 */
			uint8_t ATEMuni::getTransitionNextTransitionNext(uint8_t mE) {
				return atemTransitionNextTransitionNext[mE];
			}
			
			/**
			 * Set Transition; Style
			 * mE 	0: ME1, 1: ME2
			 * style 	0: Mix, 1: Dip, 2: Wipe, 3: DVE, 4: Sting
			 */
			void ATEMuni::setTransitionStyle(uint8_t mE, uint8_t style) {
			
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
			void ATEMuni::setTransitionNextTransition(uint8_t mE, uint8_t nextTransition) {
			
	  	  		_prepareCommandPacket(PSTR("CTTp"),4,(_packetBuffer[12+_cBBO+4+4+1]==mE));
		
					// Set Mask: 2
				_packetBuffer[12+_cBBO+4+4+0] |= 2;
						
				_packetBuffer[12+_cBBO+4+4+1] = mE;
				
				_packetBuffer[12+_cBBO+4+4+3] = nextTransition;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Get Transition Position; In Transition
			 * mE 	0: ME1, 1: ME2
			 */
			bool ATEMuni::getTransitionInTransition(uint8_t mE) {
				return atemTransitionInTransition[mE];
			}
			
			/**
			 * Get Transition Position; Frames Remaining
			 * mE 	0: ME1, 1: ME2
			 */
			uint8_t ATEMuni::getTransitionFramesRemaining(uint8_t mE) {
				return atemTransitionFramesRemaining[mE];
			}
			
			/**
			 * Get Transition Position; Position
			 * mE 	0: ME1, 1: ME2
			 */
			uint16_t ATEMuni::getTransitionPosition(uint8_t mE) {
				return atemTransitionPosition[mE];
			}
			
			/**
			 * Set Transition Position; Position
			 * mE 	0: ME1, 1: ME2
			 * position 	0-9999
			 */
			void ATEMuni::setTransitionPosition(uint8_t mE, uint16_t position) {
			
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
			uint8_t ATEMuni::getTransitionMixRate(uint8_t mE) {
				return atemTransitionMixRate[mE];
			}
			
			/**
			 * Set Transition Mix; Rate
			 * mE 	0: ME1, 1: ME2
			 * rate 	1-250: Frames
			 */
			void ATEMuni::setTransitionMixRate(uint8_t mE, uint8_t rate) {
			
	  	  		_prepareCommandPacket(PSTR("CTMx"),4,(_packetBuffer[12+_cBBO+4+4+0]==mE));
		
				_packetBuffer[12+_cBBO+4+4+0] = mE;
				
				_packetBuffer[12+_cBBO+4+4+1] = rate;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Get Transition Dip; Rate
			 * mE 	0: ME1, 1: ME2
			 */
			uint8_t ATEMuni::getTransitionDipRate(uint8_t mE) {
				return atemTransitionDipRate[mE];
			}
			
			/**
			 * Get Transition Dip; Input
			 * mE 	0: ME1, 1: ME2
			 */
			uint16_t ATEMuni::getTransitionDipInput(uint8_t mE) {
				return atemTransitionDipInput[mE];
			}
			
			/**
			 * Set Transition Dip; Rate
			 * mE 	0: ME1, 1: ME2
			 * rate 	1-250: Frames
			 */
			void ATEMuni::setTransitionDipRate(uint8_t mE, uint8_t rate) {
			
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
			void ATEMuni::setTransitionDipInput(uint8_t mE, uint16_t input) {
			
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
			uint8_t ATEMuni::getTransitionWipeRate(uint8_t mE) {
				return atemTransitionWipeRate[mE];
			}
			
			/**
			 * Get Transition Wipe; Pattern
			 * mE 	0: ME1, 1: ME2
			 */
			uint8_t ATEMuni::getTransitionWipePattern(uint8_t mE) {
				return atemTransitionWipePattern[mE];
			}
			
			/**
			 * Get Transition Wipe; Width
			 * mE 	0: ME1, 1: ME2
			 */
			uint16_t ATEMuni::getTransitionWipeWidth(uint8_t mE) {
				return atemTransitionWipeWidth[mE];
			}
			
			/**
			 * Get Transition Wipe; Fill Source
			 * mE 	0: ME1, 1: ME2
			 */
			uint16_t ATEMuni::getTransitionWipeFillSource(uint8_t mE) {
				return atemTransitionWipeFillSource[mE];
			}
			
			/**
			 * Get Transition Wipe; Symmetry
			 * mE 	0: ME1, 1: ME2
			 */
			uint16_t ATEMuni::getTransitionWipeSymmetry(uint8_t mE) {
				return atemTransitionWipeSymmetry[mE];
			}
			
			/**
			 * Get Transition Wipe; Softness
			 * mE 	0: ME1, 1: ME2
			 */
			uint16_t ATEMuni::getTransitionWipeSoftness(uint8_t mE) {
				return atemTransitionWipeSoftness[mE];
			}
			
			/**
			 * Get Transition Wipe; Position X
			 * mE 	0: ME1, 1: ME2
			 */
			uint16_t ATEMuni::getTransitionWipePositionX(uint8_t mE) {
				return atemTransitionWipePositionX[mE];
			}
			
			/**
			 * Get Transition Wipe; Position Y
			 * mE 	0: ME1, 1: ME2
			 */
			uint16_t ATEMuni::getTransitionWipePositionY(uint8_t mE) {
				return atemTransitionWipePositionY[mE];
			}
			
			/**
			 * Get Transition Wipe; Reverse
			 * mE 	0: ME1, 1: ME2
			 */
			bool ATEMuni::getTransitionWipeReverse(uint8_t mE) {
				return atemTransitionWipeReverse[mE];
			}
			
			/**
			 * Get Transition Wipe; FlipFlop
			 * mE 	0: ME1, 1: ME2
			 */
			bool ATEMuni::getTransitionWipeFlipFlop(uint8_t mE) {
				return atemTransitionWipeFlipFlop[mE];
			}
			
			/**
			 * Set Transition Wipe; Rate
			 * mE 	0: ME1, 1: ME2
			 * rate 	1-250: Frames
			 */
			void ATEMuni::setTransitionWipeRate(uint8_t mE, uint8_t rate) {
			
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
			void ATEMuni::setTransitionWipePattern(uint8_t mE, uint8_t pattern) {
			
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
			void ATEMuni::setTransitionWipeWidth(uint8_t mE, uint16_t width) {
			
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
			void ATEMuni::setTransitionWipeFillSource(uint8_t mE, uint16_t fillSource) {
			
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
			void ATEMuni::setTransitionWipeSymmetry(uint8_t mE, uint16_t symmetry) {
			
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
			void ATEMuni::setTransitionWipeSoftness(uint8_t mE, uint16_t softness) {
			
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
			void ATEMuni::setTransitionWipePositionX(uint8_t mE, uint16_t positionX) {
			
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
			void ATEMuni::setTransitionWipePositionY(uint8_t mE, uint16_t positionY) {
			
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
			void ATEMuni::setTransitionWipeReverse(uint8_t mE, bool reverse) {
			
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
			void ATEMuni::setTransitionWipeFlipFlop(uint8_t mE, bool flipFlop) {
			
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
			uint8_t ATEMuni::getTransitionDVERate(uint8_t mE) {
				return atemTransitionDVERate[mE];
			}
			
			/**
			 * Get Transition DVE; Style
			 * mE 	0: ME1, 1: ME2
			 */
			uint8_t ATEMuni::getTransitionDVEStyle(uint8_t mE) {
				return atemTransitionDVEStyle[mE];
			}
			
			/**
			 * Get Transition DVE; Fill Source
			 * mE 	0: ME1, 1: ME2
			 */
			uint16_t ATEMuni::getTransitionDVEFillSource(uint8_t mE) {
				return atemTransitionDVEFillSource[mE];
			}
			
			/**
			 * Get Transition DVE; Key Source
			 * mE 	0: ME1, 1: ME2
			 */
			uint16_t ATEMuni::getTransitionDVEKeySource(uint8_t mE) {
				return atemTransitionDVEKeySource[mE];
			}
			
			/**
			 * Get Transition DVE; Enable Key
			 * mE 	0: ME1, 1: ME2
			 */
			bool ATEMuni::getTransitionDVEEnableKey(uint8_t mE) {
				return atemTransitionDVEEnableKey[mE];
			}
			
			/**
			 * Get Transition DVE; Pre Multiplied
			 * mE 	0: ME1, 1: ME2
			 */
			bool ATEMuni::getTransitionDVEPreMultiplied(uint8_t mE) {
				return atemTransitionDVEPreMultiplied[mE];
			}
			
			/**
			 * Get Transition DVE; Clip
			 * mE 	0: ME1, 1: ME2
			 */
			uint16_t ATEMuni::getTransitionDVEClip(uint8_t mE) {
				return atemTransitionDVEClip[mE];
			}
			
			/**
			 * Get Transition DVE; Gain
			 * mE 	0: ME1, 1: ME2
			 */
			uint16_t ATEMuni::getTransitionDVEGain(uint8_t mE) {
				return atemTransitionDVEGain[mE];
			}
			
			/**
			 * Get Transition DVE; Invert Key
			 * mE 	0: ME1, 1: ME2
			 */
			bool ATEMuni::getTransitionDVEInvertKey(uint8_t mE) {
				return atemTransitionDVEInvertKey[mE];
			}
			
			/**
			 * Get Transition DVE; Reverse
			 * mE 	0: ME1, 1: ME2
			 */
			bool ATEMuni::getTransitionDVEReverse(uint8_t mE) {
				return atemTransitionDVEReverse[mE];
			}
			
			/**
			 * Get Transition DVE; FlipFlop
			 * mE 	0: ME1, 1: ME2
			 */
			bool ATEMuni::getTransitionDVEFlipFlop(uint8_t mE) {
				return atemTransitionDVEFlipFlop[mE];
			}
			
			/**
			 * Set Transition DVE; Rate
			 * mE 	0: ME1, 1: ME2
			 * rate 	1-250: Frames
			 */
			void ATEMuni::setTransitionDVERate(uint8_t mE, uint8_t rate) {
			
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
			void ATEMuni::setTransitionDVEStyle(uint8_t mE, uint8_t style) {
			
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
			void ATEMuni::setTransitionDVEFillSource(uint8_t mE, uint16_t fillSource) {
			
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
			void ATEMuni::setTransitionDVEKeySource(uint8_t mE, uint16_t keySource) {
			
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
			void ATEMuni::setTransitionDVEEnableKey(uint8_t mE, bool enableKey) {
			
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
			void ATEMuni::setTransitionDVEPreMultiplied(uint8_t mE, bool preMultiplied) {
			
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
			void ATEMuni::setTransitionDVEClip(uint8_t mE, uint16_t clip) {
			
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
			void ATEMuni::setTransitionDVEGain(uint8_t mE, uint16_t gain) {
			
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
			void ATEMuni::setTransitionDVEInvertKey(uint8_t mE, bool invertKey) {
			
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
			void ATEMuni::setTransitionDVEReverse(uint8_t mE, bool reverse) {
			
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
			void ATEMuni::setTransitionDVEFlipFlop(uint8_t mE, bool flipFlop) {
			
	  	  		_prepareCommandPacket(PSTR("CTDv"),20,(_packetBuffer[12+_cBBO+4+4+2]==mE));
		
					// Set Mask: 2048
				_packetBuffer[12+_cBBO+4+4+0] |= 8;
						
				_packetBuffer[12+_cBBO+4+4+2] = mE;
				
				_packetBuffer[12+_cBBO+4+4+18] = flipFlop;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Get Keyer On Air; Enabled
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			bool ATEMuni::getKeyerOnAirEnabled(uint8_t mE, uint8_t keyer) {
				return atemKeyerOnAirEnabled[mE][keyer];
			}
			
			/**
			 * Set Keyer On Air; Enabled
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 * enabled 	Bit 0: On/Off
			 */
			void ATEMuni::setKeyerOnAirEnabled(uint8_t mE, uint8_t keyer, bool enabled) {
			
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
			uint8_t ATEMuni::getKeyerType(uint8_t mE, uint8_t keyer) {
				return atemKeyerType[mE][keyer];
			}
			
			/**
			 * Get Keyer Base; Fly Enabled
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			bool ATEMuni::getKeyerFlyEnabled(uint8_t mE, uint8_t keyer) {
				return atemKeyerFlyEnabled[mE][keyer];
			}
			
			/**
			 * Get Keyer Base; Fill Source
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			uint16_t ATEMuni::getKeyerFillSource(uint8_t mE, uint8_t keyer) {
				return atemKeyerFillSource[mE][keyer];
			}
			
			/**
			 * Get Keyer Base; Key Source
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			uint16_t ATEMuni::getKeyerKeySource(uint8_t mE, uint8_t keyer) {
				return atemKeyerKeySource[mE][keyer];
			}
			
			/**
			 * Get Keyer Base; Masked
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			bool ATEMuni::getKeyerMasked(uint8_t mE, uint8_t keyer) {
				return atemKeyerMasked[mE][keyer];
			}
			
			/**
			 * Get Keyer Base; Top
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			int ATEMuni::getKeyerTop(uint8_t mE, uint8_t keyer) {
				return atemKeyerTop[mE][keyer];
			}
			
			/**
			 * Get Keyer Base; Bottom
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			int ATEMuni::getKeyerBottom(uint8_t mE, uint8_t keyer) {
				return atemKeyerBottom[mE][keyer];
			}
			
			/**
			 * Get Keyer Base; Left
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			int ATEMuni::getKeyerLeft(uint8_t mE, uint8_t keyer) {
				return atemKeyerLeft[mE][keyer];
			}
			
			/**
			 * Get Keyer Base; Right
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			int ATEMuni::getKeyerRight(uint8_t mE, uint8_t keyer) {
				return atemKeyerRight[mE][keyer];
			}
			
			/**
			 * Set Key Fill; Fill Source
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 * fillSource 	(See video source list)
			 */
			void ATEMuni::setKeyerFillSource(uint8_t mE, uint8_t keyer, uint16_t fillSource) {
			
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
			void ATEMuni::setKeyerKeySource(uint8_t mE, uint8_t keyer, uint16_t keySource) {
			
	  	  		_prepareCommandPacket(PSTR("CKeC"),4,(_packetBuffer[12+_cBBO+4+4+0]==mE) && (_packetBuffer[12+_cBBO+4+4+1]==keyer));
		
				_packetBuffer[12+_cBBO+4+4+0] = mE;
				
				_packetBuffer[12+_cBBO+4+4+1] = keyer;
				
				_packetBuffer[12+_cBBO+4+4+2] = highByte(keySource);
				_packetBuffer[12+_cBBO+4+4+3] = lowByte(keySource);
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Get Key DVE; Size X
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			long ATEMuni::getKeyDVESizeX(uint8_t mE, uint8_t keyer) {
				return atemKeyDVESizeX[mE][keyer];
			}
			
			/**
			 * Get Key DVE; Size Y
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			long ATEMuni::getKeyDVESizeY(uint8_t mE, uint8_t keyer) {
				return atemKeyDVESizeY[mE][keyer];
			}
			
			/**
			 * Get Key DVE; Position X
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			long ATEMuni::getKeyDVEPositionX(uint8_t mE, uint8_t keyer) {
				return atemKeyDVEPositionX[mE][keyer];
			}
			
			/**
			 * Get Key DVE; Position Y
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			long ATEMuni::getKeyDVEPositionY(uint8_t mE, uint8_t keyer) {
				return atemKeyDVEPositionY[mE][keyer];
			}
			
			/**
			 * Get Key DVE; Rotation
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			long ATEMuni::getKeyDVERotation(uint8_t mE, uint8_t keyer) {
				return atemKeyDVERotation[mE][keyer];
			}
			
			/**
			 * Get Key DVE; Border Enabled
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			bool ATEMuni::getKeyDVEBorderEnabled(uint8_t mE, uint8_t keyer) {
				return atemKeyDVEBorderEnabled[mE][keyer];
			}
			
			/**
			 * Get Key DVE; Shadow
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			bool ATEMuni::getKeyDVEShadow(uint8_t mE, uint8_t keyer) {
				return atemKeyDVEShadow[mE][keyer];
			}
			
			/**
			 * Get Key DVE; Border Bevel
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			uint8_t ATEMuni::getKeyDVEBorderBevel(uint8_t mE, uint8_t keyer) {
				return atemKeyDVEBorderBevel[mE][keyer];
			}
			
			/**
			 * Get Key DVE; Border Outer Width
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			uint16_t ATEMuni::getKeyDVEBorderOuterWidth(uint8_t mE, uint8_t keyer) {
				return atemKeyDVEBorderOuterWidth[mE][keyer];
			}
			
			/**
			 * Get Key DVE; Border Inner Width
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			uint16_t ATEMuni::getKeyDVEBorderInnerWidth(uint8_t mE, uint8_t keyer) {
				return atemKeyDVEBorderInnerWidth[mE][keyer];
			}
			
			/**
			 * Get Key DVE; Border Outer Softness
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			uint8_t ATEMuni::getKeyDVEBorderOuterSoftness(uint8_t mE, uint8_t keyer) {
				return atemKeyDVEBorderOuterSoftness[mE][keyer];
			}
			
			/**
			 * Get Key DVE; Border Inner Softness
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			uint8_t ATEMuni::getKeyDVEBorderInnerSoftness(uint8_t mE, uint8_t keyer) {
				return atemKeyDVEBorderInnerSoftness[mE][keyer];
			}
			
			/**
			 * Get Key DVE; Border Bevel Softness
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			uint8_t ATEMuni::getKeyDVEBorderBevelSoftness(uint8_t mE, uint8_t keyer) {
				return atemKeyDVEBorderBevelSoftness[mE][keyer];
			}
			
			/**
			 * Get Key DVE; Border Bevel Position
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			uint8_t ATEMuni::getKeyDVEBorderBevelPosition(uint8_t mE, uint8_t keyer) {
				return atemKeyDVEBorderBevelPosition[mE][keyer];
			}
			
			/**
			 * Get Key DVE; Border Opacity
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			uint8_t ATEMuni::getKeyDVEBorderOpacity(uint8_t mE, uint8_t keyer) {
				return atemKeyDVEBorderOpacity[mE][keyer];
			}
			
			/**
			 * Get Key DVE; Border Hue
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			uint16_t ATEMuni::getKeyDVEBorderHue(uint8_t mE, uint8_t keyer) {
				return atemKeyDVEBorderHue[mE][keyer];
			}
			
			/**
			 * Get Key DVE; Border Saturation
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			uint16_t ATEMuni::getKeyDVEBorderSaturation(uint8_t mE, uint8_t keyer) {
				return atemKeyDVEBorderSaturation[mE][keyer];
			}
			
			/**
			 * Get Key DVE; Border Luma
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			uint16_t ATEMuni::getKeyDVEBorderLuma(uint8_t mE, uint8_t keyer) {
				return atemKeyDVEBorderLuma[mE][keyer];
			}
			
			/**
			 * Get Key DVE; Light Source Direction
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			uint16_t ATEMuni::getKeyDVELightSourceDirection(uint8_t mE, uint8_t keyer) {
				return atemKeyDVELightSourceDirection[mE][keyer];
			}
			
			/**
			 * Get Key DVE; Light Source Altitude
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			uint8_t ATEMuni::getKeyDVELightSourceAltitude(uint8_t mE, uint8_t keyer) {
				return atemKeyDVELightSourceAltitude[mE][keyer];
			}
			
			/**
			 * Get Key DVE; Masked
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			bool ATEMuni::getKeyDVEMasked(uint8_t mE, uint8_t keyer) {
				return atemKeyDVEMasked[mE][keyer];
			}
			
			/**
			 * Get Key DVE; Top
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			int ATEMuni::getKeyDVETop(uint8_t mE, uint8_t keyer) {
				return atemKeyDVETop[mE][keyer];
			}
			
			/**
			 * Get Key DVE; Bottom
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			int ATEMuni::getKeyDVEBottom(uint8_t mE, uint8_t keyer) {
				return atemKeyDVEBottom[mE][keyer];
			}
			
			/**
			 * Get Key DVE; Left
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			int ATEMuni::getKeyDVELeft(uint8_t mE, uint8_t keyer) {
				return atemKeyDVELeft[mE][keyer];
			}
			
			/**
			 * Get Key DVE; Right
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			int ATEMuni::getKeyDVERight(uint8_t mE, uint8_t keyer) {
				return atemKeyDVERight[mE][keyer];
			}
			
			/**
			 * Get Key DVE; Rate
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			uint8_t ATEMuni::getKeyDVERate(uint8_t mE, uint8_t keyer) {
				return atemKeyDVERate[mE][keyer];
			}
			
			/**
			 * Set Key DVE; Size X
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 * sizeX 	Example: 1000: 1.000
			 */
			void ATEMuni::setKeyDVESizeX(uint8_t mE, uint8_t keyer, long sizeX) {
			
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
			void ATEMuni::setKeyDVESizeY(uint8_t mE, uint8_t keyer, long sizeY) {
			
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
			void ATEMuni::setKeyDVEPositionX(uint8_t mE, uint8_t keyer, long positionX) {
			
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
			void ATEMuni::setKeyDVEPositionY(uint8_t mE, uint8_t keyer, long positionY) {
			
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
			void ATEMuni::setKeyDVERotation(uint8_t mE, uint8_t keyer, long rotation) {
			
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
			void ATEMuni::setKeyDVEBorderEnabled(uint8_t mE, uint8_t keyer, bool borderEnabled) {
			
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
			void ATEMuni::setKeyDVEShadow(uint8_t mE, uint8_t keyer, bool shadow) {
			
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
			void ATEMuni::setKeyDVEBorderBevel(uint8_t mE, uint8_t keyer, uint8_t borderBevel) {
			
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
			void ATEMuni::setKeyDVEBorderOuterWidth(uint8_t mE, uint8_t keyer, uint16_t borderOuterWidth) {
			
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
			void ATEMuni::setKeyDVEBorderInnerWidth(uint8_t mE, uint8_t keyer, uint16_t borderInnerWidth) {
			
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
			void ATEMuni::setKeyDVEBorderOuterSoftness(uint8_t mE, uint8_t keyer, uint8_t borderOuterSoftness) {
			
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
			void ATEMuni::setKeyDVEBorderInnerSoftness(uint8_t mE, uint8_t keyer, uint8_t borderInnerSoftness) {
			
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
			void ATEMuni::setKeyDVEBorderBevelSoftness(uint8_t mE, uint8_t keyer, uint8_t borderBevelSoftness) {
			
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
			void ATEMuni::setKeyDVEBorderBevelPosition(uint8_t mE, uint8_t keyer, uint8_t borderBevelPosition) {
			
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
			void ATEMuni::setKeyDVEBorderOpacity(uint8_t mE, uint8_t keyer, uint8_t borderOpacity) {
			
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
			void ATEMuni::setKeyDVEBorderHue(uint8_t mE, uint8_t keyer, uint16_t borderHue) {
			
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
			void ATEMuni::setKeyDVEBorderSaturation(uint8_t mE, uint8_t keyer, uint16_t borderSaturation) {
			
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
			void ATEMuni::setKeyDVEBorderLuma(uint8_t mE, uint8_t keyer, uint16_t borderLuma) {
			
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
			void ATEMuni::setKeyDVELightSourceDirection(uint8_t mE, uint8_t keyer, uint16_t lightSourceDirection) {
			
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
			void ATEMuni::setKeyDVELightSourceAltitude(uint8_t mE, uint8_t keyer, uint8_t lightSourceAltitude) {
			
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
			void ATEMuni::setKeyDVEMasked(uint8_t mE, uint8_t keyer, bool masked) {
			
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
			void ATEMuni::setKeyDVETop(uint8_t mE, uint8_t keyer, int top) {
			
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
			void ATEMuni::setKeyDVEBottom(uint8_t mE, uint8_t keyer, int bottom) {
			
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
			void ATEMuni::setKeyDVELeft(uint8_t mE, uint8_t keyer, int left) {
			
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
			void ATEMuni::setKeyDVERight(uint8_t mE, uint8_t keyer, int right) {
			
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
			void ATEMuni::setKeyDVERate(uint8_t mE, uint8_t keyer, uint8_t rate) {
			
	  	  		_prepareCommandPacket(PSTR("CKDV"),64,(_packetBuffer[12+_cBBO+4+4+4]==mE) && (_packetBuffer[12+_cBBO+4+4+5]==keyer));
		
					// Set Mask: 33554432
				_packetBuffer[12+_cBBO+4+4+0] |= 2;
						
				_packetBuffer[12+_cBBO+4+4+4] = mE;
				
				_packetBuffer[12+_cBBO+4+4+5] = keyer;
				
				_packetBuffer[12+_cBBO+4+4+60] = rate;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Get Downstream Keyer; Fill Source
			 * keyer 	0: DSK1, 1: DSK2
			 */
			uint16_t ATEMuni::getDownstreamKeyerFillSource(uint8_t keyer) {
				return atemDownstreamKeyerFillSource[keyer];
			}
			
			/**
			 * Get Downstream Keyer; Key Source
			 * keyer 	0: DSK1, 1: DSK2
			 */
			uint16_t ATEMuni::getDownstreamKeyerKeySource(uint8_t keyer) {
				return atemDownstreamKeyerKeySource[keyer];
			}
			
			/**
			 * Set Downstream Keyer; Fill Source
			 * keyer 	0-3: Keyer 1-4
			 * fillSource 	(See video source list)
			 */
			void ATEMuni::setDownstreamKeyerFillSource(uint8_t keyer, uint16_t fillSource) {
			
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
			void ATEMuni::setDownstreamKeyerKeySource(uint8_t keyer, uint16_t keySource) {
			
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
			bool ATEMuni::getDownstreamKeyerTie(uint8_t keyer) {
				return atemDownstreamKeyerTie[keyer];
			}
			
			/**
			 * Get Downstream Keyer; Rate
			 * keyer 	0: DSK1, 1: DSK2
			 */
			uint8_t ATEMuni::getDownstreamKeyerRate(uint8_t keyer) {
				return atemDownstreamKeyerRate[keyer];
			}
			
			/**
			 * Get Downstream Keyer; Pre Multiplied
			 * keyer 	0: DSK1, 1: DSK2
			 */
			bool ATEMuni::getDownstreamKeyerPreMultiplied(uint8_t keyer) {
				return atemDownstreamKeyerPreMultiplied[keyer];
			}
			
			/**
			 * Get Downstream Keyer; Clip
			 * keyer 	0: DSK1, 1: DSK2
			 */
			uint16_t ATEMuni::getDownstreamKeyerClip(uint8_t keyer) {
				return atemDownstreamKeyerClip[keyer];
			}
			
			/**
			 * Get Downstream Keyer; Gain
			 * keyer 	0: DSK1, 1: DSK2
			 */
			uint16_t ATEMuni::getDownstreamKeyerGain(uint8_t keyer) {
				return atemDownstreamKeyerGain[keyer];
			}
			
			/**
			 * Get Downstream Keyer; Invert Key
			 * keyer 	0: DSK1, 1: DSK2
			 */
			bool ATEMuni::getDownstreamKeyerInvertKey(uint8_t keyer) {
				return atemDownstreamKeyerInvertKey[keyer];
			}
			
			/**
			 * Get Downstream Keyer; Masked
			 * keyer 	0: DSK1, 1: DSK2
			 */
			bool ATEMuni::getDownstreamKeyerMasked(uint8_t keyer) {
				return atemDownstreamKeyerMasked[keyer];
			}
			
			/**
			 * Get Downstream Keyer; Top
			 * keyer 	0: DSK1, 1: DSK2
			 */
			int ATEMuni::getDownstreamKeyerTop(uint8_t keyer) {
				return atemDownstreamKeyerTop[keyer];
			}
			
			/**
			 * Get Downstream Keyer; Bottom
			 * keyer 	0: DSK1, 1: DSK2
			 */
			int ATEMuni::getDownstreamKeyerBottom(uint8_t keyer) {
				return atemDownstreamKeyerBottom[keyer];
			}
			
			/**
			 * Get Downstream Keyer; Left
			 * keyer 	0: DSK1, 1: DSK2
			 */
			int ATEMuni::getDownstreamKeyerLeft(uint8_t keyer) {
				return atemDownstreamKeyerLeft[keyer];
			}
			
			/**
			 * Get Downstream Keyer; Right
			 * keyer 	0: DSK1, 1: DSK2
			 */
			int ATEMuni::getDownstreamKeyerRight(uint8_t keyer) {
				return atemDownstreamKeyerRight[keyer];
			}
			
			/**
			 * Set Downstream Keyer; Rate
			 * keyer 	0: DSK1, 1: DSK2
			 * rate 	1-250: Frames
			 */
			void ATEMuni::setDownstreamKeyerRate(uint8_t keyer, uint8_t rate) {
			
	  	  		_prepareCommandPacket(PSTR("CDsR"),4,(_packetBuffer[12+_cBBO+4+4+0]==keyer));
		
				_packetBuffer[12+_cBBO+4+4+0] = keyer;
				
				_packetBuffer[12+_cBBO+4+4+1] = rate;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Downstream Keyer Auto; Keyer
			 * keyer 	0: DSK1, 1: DSK2
			 */
			void ATEMuni::performDownstreamKeyerAutoKeyer(uint8_t keyer) {
			
	  	  		_prepareCommandPacket(PSTR("DDsA"),4);
		
				_packetBuffer[12+_cBBO+4+4+0] = keyer;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Get Downstream Keyer; On Air
			 * keyer 	0: DSK1, 1: DSK2
			 */
			bool ATEMuni::getDownstreamKeyerOnAir(uint8_t keyer) {
				return atemDownstreamKeyerOnAir[keyer];
			}
			
			/**
			 * Get Downstream Keyer; In Transition
			 * keyer 	0: DSK1, 1: DSK2
			 */
			bool ATEMuni::getDownstreamKeyerInTransition(uint8_t keyer) {
				return atemDownstreamKeyerInTransition[keyer];
			}
			
			/**
			 * Get Downstream Keyer; Is Auto Transitioning
			 * keyer 	0: DSK1, 1: DSK2
			 */
			bool ATEMuni::getDownstreamKeyerIsAutoTransitioning(uint8_t keyer) {
				return atemDownstreamKeyerIsAutoTransitioning[keyer];
			}
			
			/**
			 * Get Downstream Keyer; Frames Remaining
			 * keyer 	0: DSK1, 1: DSK2
			 */
			uint8_t ATEMuni::getDownstreamKeyerFramesRemaining(uint8_t keyer) {
				return atemDownstreamKeyerFramesRemaining[keyer];
			}
			
			/**
			 * Set Downstream Keyer; On Air
			 * keyer 	0: DSK1, 1: DSK2
			 * onAir 	Bit 0: On/Off
			 */
			void ATEMuni::setDownstreamKeyerOnAir(uint8_t keyer, bool onAir) {
			
	  	  		_prepareCommandPacket(PSTR("CDsL"),4,(_packetBuffer[12+_cBBO+4+4+0]==keyer));
		
				_packetBuffer[12+_cBBO+4+4+0] = keyer;
				
				_packetBuffer[12+_cBBO+4+4+1] = onAir;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Get Fade-To-Black; Rate
			 * mE 	0: ME1, 1: ME2
			 */
			uint8_t ATEMuni::getFadeToBlackRate(uint8_t mE) {
				return atemFadeToBlackRate[mE];
			}
			
			/**
			 * Set Fade-To-Black; Rate
			 * mE 	0: ME1, 1: ME2
			 * rate 	1-250: Frames
			 */
			void ATEMuni::setFadeToBlackRate(uint8_t mE, uint8_t rate) {
			
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
			bool ATEMuni::getFadeToBlackStateFullyBlack(uint8_t mE) {
				return atemFadeToBlackStateFullyBlack[mE];
			}
			
			/**
			 * Get Fade-To-Black State; In Transition
			 * mE 	0: ME1, 1: ME2
			 */
			bool ATEMuni::getFadeToBlackStateInTransition(uint8_t mE) {
				return atemFadeToBlackStateInTransition[mE];
			}
			
			/**
			 * Get Fade-To-Black State; Frames Remaining
			 * mE 	0: ME1, 1: ME2
			 */
			uint8_t ATEMuni::getFadeToBlackStateFramesRemaining(uint8_t mE) {
				return atemFadeToBlackStateFramesRemaining[mE];
			}
			


				/**
				 * Set Fade-To-Black; M/E
				 * mE 	0: ME1, 1: ME2
				 */
				void ATEMuni::performFadeToBlackME(uint8_t mE) {

		  	  		_prepareCommandPacket(PSTR("FtbA"),4);

					_packetBuffer[12+_cBBO+4+4+0] = mE;
					_packetBuffer[12+_cBBO+4+4+1] = 0x02;

					_finishCommandPacket();

				}
			
			/**
			 * Get Aux Source; Input
			 * aUXChannel 	0-5: Aux 1-6
			 */
			uint16_t ATEMuni::getAuxSourceInput(uint8_t aUXChannel) {
				return atemAuxSourceInput[aUXChannel];
			}
			
			/**
			 * Set Aux Source; Input
			 * aUXChannel 	0-5: Aux 1-6
			 * input 	(See video source list)
			 */
			void ATEMuni::setAuxSourceInput(uint8_t aUXChannel, uint16_t input) {
			
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
			 * input 	1-8: Camera
			 */
			int ATEMuni::getCameraControlIris(uint8_t input) {
				return atemCameraControlIris[input];
			}
			
			/**
			 * Get Camera Control; Focus
			 * input 	1-8: Camera
			 */
			int ATEMuni::getCameraControlFocus(uint8_t input) {
				return atemCameraControlFocus[input];
			}
			
			/**
			 * Get Camera Control; Gain
			 * input 	1-8: Camera
			 */
			int ATEMuni::getCameraControlGain(uint8_t input) {
				return atemCameraControlGain[input];
			}
			
			/**
			 * Get Camera Control; White Balance
			 * input 	1-8: Camera
			 */
			int ATEMuni::getCameraControlWhiteBalance(uint8_t input) {
				return atemCameraControlWhiteBalance[input];
			}
			
			/**
			 * Get Camera Control; Sharpening Level
			 * input 	1-8: Camera
			 */
			int ATEMuni::getCameraControlSharpeningLevel(uint8_t input) {
				return atemCameraControlSharpeningLevel[input];
			}
			
			/**
			 * Get Camera Control; Zoom Normalized
			 * input 	1-8: Camera
			 */
			int ATEMuni::getCameraControlZoomNormalized(uint8_t input) {
				return atemCameraControlZoomNormalized[input];
			}
			
			/**
			 * Get Camera Control; Zoom Speed
			 * input 	1-8: Camera
			 */
			int ATEMuni::getCameraControlZoomSpeed(uint8_t input) {
				return atemCameraControlZoomSpeed[input];
			}
			
			/**
			 * Get Camera Control; Colorbars
			 * input 	1-8: Camera
			 */
			int ATEMuni::getCameraControlColorbars(uint8_t input) {
				return atemCameraControlColorbars[input];
			}
			
			/**
			 * Get Camera Control; Lift R
			 * input 	1-8: Camera
			 */
			int ATEMuni::getCameraControlLiftR(uint8_t input) {
				return atemCameraControlLiftR[input];
			}
			
			/**
			 * Get Camera Control; Gamma R
			 * input 	1-8: Camera
			 */
			int ATEMuni::getCameraControlGammaR(uint8_t input) {
				return atemCameraControlGammaR[input];
			}
			
			/**
			 * Get Camera Control; Gain R
			 * input 	1-8: Camera
			 */
			int ATEMuni::getCameraControlGainR(uint8_t input) {
				return atemCameraControlGainR[input];
			}
			
			/**
			 * Get Camera Control; Lum Mix
			 * input 	1-8: Camera
			 */
			int ATEMuni::getCameraControlLumMix(uint8_t input) {
				return atemCameraControlLumMix[input];
			}
			
			/**
			 * Get Camera Control; Hue
			 * input 	1-8: Camera
			 */
			int ATEMuni::getCameraControlHue(uint8_t input) {
				return atemCameraControlHue[input];
			}
			
			/**
			 * Get Camera Control; Shutter
			 * input 	1-8: Camera
			 */
			int ATEMuni::getCameraControlShutter(uint8_t input) {
				return atemCameraControlShutter[input];
			}
			
			/**
			 * Get Camera Control; Lift G
			 * input 	1-8: Camera
			 */
			int ATEMuni::getCameraControlLiftG(uint8_t input) {
				return atemCameraControlLiftG[input];
			}
			
			/**
			 * Get Camera Control; Gamma G
			 * input 	1-8: Camera
			 */
			int ATEMuni::getCameraControlGammaG(uint8_t input) {
				return atemCameraControlGammaG[input];
			}
			
			/**
			 * Get Camera Control; Gain G
			 * input 	1-8: Camera
			 */
			int ATEMuni::getCameraControlGainG(uint8_t input) {
				return atemCameraControlGainG[input];
			}
			
			/**
			 * Get Camera Control; Contrast
			 * input 	1-8: Camera
			 */
			int ATEMuni::getCameraControlContrast(uint8_t input) {
				return atemCameraControlContrast[input];
			}
			
			/**
			 * Get Camera Control; Saturation
			 * input 	1-8: Camera
			 */
			int ATEMuni::getCameraControlSaturation(uint8_t input) {
				return atemCameraControlSaturation[input];
			}
			
			/**
			 * Get Camera Control; Lift B
			 * input 	1-8: Camera
			 */
			int ATEMuni::getCameraControlLiftB(uint8_t input) {
				return atemCameraControlLiftB[input];
			}
			
			/**
			 * Get Camera Control; Gamma B
			 * input 	1-8: Camera
			 */
			int ATEMuni::getCameraControlGammaB(uint8_t input) {
				return atemCameraControlGammaB[input];
			}
			
			/**
			 * Get Camera Control; Gain B
			 * input 	1-8: Camera
			 */
			int ATEMuni::getCameraControlGainB(uint8_t input) {
				return atemCameraControlGainB[input];
			}
			
			/**
			 * Get Camera Control; Lift Y
			 * input 	1-8: Camera
			 */
			int ATEMuni::getCameraControlLiftY(uint8_t input) {
				return atemCameraControlLiftY[input];
			}
			
			/**
			 * Get Camera Control; Gamma Y
			 * input 	1-8: Camera
			 */
			int ATEMuni::getCameraControlGammaY(uint8_t input) {
				return atemCameraControlGammaY[input];
			}
			
			/**
			 * Get Camera Control; Gain Y
			 * input 	1-8: Camera
			 */
			int ATEMuni::getCameraControlGainY(uint8_t input) {
				return atemCameraControlGainY[input];
			}
			

			/**
				* Set Camera Control; Auto iris
				* Command takes no input
				*/

			void ATEMuni::setCameraControlAutoIris(uint8_t input, int autoiris) {
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

			void ATEMuni::setCameraControlSharpeningLevel(uint8_t input, int detail) {
					_prepareCommandPacket(PSTR("CCmd"), 20);

					_packetBuffer[12+_cBBO+4+4+0] = input;

					_packetBuffer[12+_cBBO+4+4+1] = 1;
					_packetBuffer[12+_cBBO+4+4+2] = 8;

					_packetBuffer[12+_cBBO+4+4+4] = 0x01; // Data type: int8
					_packetBuffer[12+_cBBO+4+4+7] = 0x01;

					_packetBuffer[12+_cBBO+4+4+16] = detail & 0xFF;

					_finishCommandPacket();
			}

			/**
				* Set Camera Control; Auto focus
				* Command takes no input
				*/

			void ATEMuni::setCameraControlAutoFocus(uint8_t input, int autoiris) {
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

			void ATEMuni::setCameraControlResetAll(uint8_t input, int reset) {
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
			void ATEMuni::setCameraControlIris(uint8_t input, int iris) {

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
			void ATEMuni::setCameraControlColorbars(uint8_t input, int colorbars) {

		  		_prepareCommandPacket(PSTR("CCmd"), 20);

				_packetBuffer[12+_cBBO+4+4+0] = input;

					// Preset values:
				_packetBuffer[12+_cBBO+4+4+1] = 4;
				_packetBuffer[12+_cBBO+4+4+2] = 4;

				_packetBuffer[12+_cBBO+4+4+4] = 0x01;	// Data type: int8
				_packetBuffer[12+_cBBO+4+4+7] = 0x01;	// ?


				_packetBuffer[12+_cBBO+4+4+16] = (colorbars & 0xFF);

				_finishCommandPacket();

			}

			/**
			 * Set Camera Control; Focus
			 * input 	0-7: Camera
			 * focus 	0-65535
			 */
			void ATEMuni::setCameraControlFocus(uint8_t input, int focus) {

		  		_prepareCommandPacket(PSTR("CCmd"),24);

					// Preset values:
				_packetBuffer[12+_cBBO+4+4+1] = 0;
				_packetBuffer[12+_cBBO+4+4+2] = 0;

				_packetBuffer[12+_cBBO+4+4+3] = 0x01;	// Relative setting
				_packetBuffer[12+_cBBO+4+4+4] = 0x80;	// Data type: 5.11 floating point
				_packetBuffer[12+_cBBO+4+4+9] = 0x01;	// One byte

				_packetBuffer[12+_cBBO+4+4+0] = input;

				_packetBuffer[12+_cBBO+4+4+16] = highByte(focus);		// Relative values...?
				_packetBuffer[12+_cBBO+4+4+17] = lowByte(focus);

		   		_finishCommandPacket();

			}

			/**
			 * Set Camera Control; Gain
			 * input 	0-7: Camera
			 * gain 	512: 0db, 1024: 6db, 2048: 12db, 4096: 18db
			 */
			void ATEMuni::setCameraControlGain(uint8_t input, int gain) {

		  		_prepareCommandPacket(PSTR("CCmd"),24);

					// Preset values:
				_packetBuffer[12+_cBBO+4+4+1] = 1;
				_packetBuffer[12+_cBBO+4+4+2] = 1;

				_packetBuffer[12+_cBBO+4+4+4] = 0x01;
				_packetBuffer[12+_cBBO+4+4+7] = 0x01;

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
			void ATEMuni::setCameraControlWhiteBalance(uint8_t input, int whiteBalance) {

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
			void ATEMuni::setCameraControlZoomNormalized(uint8_t input, int zoomNormalized) {

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
			void ATEMuni::setCameraControlZoomSpeed(uint8_t input, int zoomSpeed) {

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
			void ATEMuni::setCameraControlLiftR(uint8_t input, int liftR) {

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
			void ATEMuni::setCameraControlGammaR(uint8_t input, int gammaR) {

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
			void ATEMuni::setCameraControlGainR(uint8_t input, int gainR) {

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
			void ATEMuni::setCameraControlLumMix(uint8_t input, int lumMix) {

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
			void ATEMuni::setCameraControlHue(uint8_t input, int hue) {

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
			void ATEMuni::setCameraControlShutter(uint8_t input, int shutter) {

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
			void ATEMuni::setCameraControlLiftG(uint8_t input, int liftG) {

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
			void ATEMuni::setCameraControlGammaG(uint8_t input, int gammaG) {

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
			void ATEMuni::setCameraControlGainG(uint8_t input, int gainG) {

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
			void ATEMuni::setCameraControlContrast(uint8_t input, int contrast) {

		  		_prepareCommandPacket(PSTR("CCmd"),24);

					// Preset values:
				_packetBuffer[12+_cBBO+4+4+1] = 8;
				_packetBuffer[12+_cBBO+4+4+2] = 4;

				_packetBuffer[12+_cBBO+4+4+4] = 0x80;
				_packetBuffer[12+_cBBO+4+4+9] = 0x02;

				_packetBuffer[12+_cBBO+4+4+0] = input;

				_packetBuffer[12+_cBBO+4+4+18] = highByte(contrast);
				_packetBuffer[12+_cBBO+4+4+19] = lowByte(contrast);

				_finishCommandPacket();

			}

			/**
			 * Set Camera Control; Saturation
			 * input 	0-7: Camera
			 * saturation 	0-4096: 0-100%
			 */
			void ATEMuni::setCameraControlSaturation(uint8_t input, int saturation) {

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
			void ATEMuni::setCameraControlLiftB(uint8_t input, int liftB) {

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
			void ATEMuni::setCameraControlGammaB(uint8_t input, int gammaB) {

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
			void ATEMuni::setCameraControlGainB(uint8_t input, int gainB) {

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
			void ATEMuni::setCameraControlLiftY(uint8_t input, int liftY) {

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
			void ATEMuni::setCameraControlGammaY(uint8_t input, int gammaY) {

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
			void ATEMuni::setCameraControlGainY(uint8_t input, int gainY) {

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
			 * Get Media Player Source; Type
			 * mediaPlayer 	0: Media Player 1, 1: Media Player 2
			 */
			uint8_t ATEMuni::getMediaPlayerSourceType(uint8_t mediaPlayer) {
				return atemMediaPlayerSourceType[mediaPlayer];
			}
			
			/**
			 * Get Media Player Source; Still Index
			 * mediaPlayer 	0: Media Player 1, 1: Media Player 2
			 */
			uint8_t ATEMuni::getMediaPlayerSourceStillIndex(uint8_t mediaPlayer) {
				return atemMediaPlayerSourceStillIndex[mediaPlayer];
			}
			
			/**
			 * Get Media Player Source; Clip Index
			 * mediaPlayer 	0: Media Player 1, 1: Media Player 2
			 */
			uint8_t ATEMuni::getMediaPlayerSourceClipIndex(uint8_t mediaPlayer) {
				return atemMediaPlayerSourceClipIndex[mediaPlayer];
			}
			
			/**
			 * Set Media Player Source; Type
			 * mediaPlayer 	0: Media Player 1, 1: Media Player 2
			 * type 	1: Still, 2: Clip
			 */
			void ATEMuni::setMediaPlayerSourceType(uint8_t mediaPlayer, uint8_t type) {
			
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
			void ATEMuni::setMediaPlayerSourceStillIndex(uint8_t mediaPlayer, uint8_t stillIndex) {
			
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
			void ATEMuni::setMediaPlayerSourceClipIndex(uint8_t mediaPlayer, uint8_t clipIndex) {
			
	  	  		_prepareCommandPacket(PSTR("MPSS"),8,(_packetBuffer[12+_cBBO+4+4+1]==mediaPlayer));
		
					// Set Mask: 4
				_packetBuffer[12+_cBBO+4+4+0] |= 4;
						
				_packetBuffer[12+_cBBO+4+4+1] = mediaPlayer;
				
				_packetBuffer[12+_cBBO+4+4+4] = clipIndex;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Get Media Player Still Files; Is Used
			 * stillBank 	0-31: Still Bank
			 */
			bool ATEMuni::getMediaPlayerStillFilesIsUsed(uint8_t stillBank) {
				return atemMediaPlayerStillFilesIsUsed[stillBank];
			}
			
			/**
			 * Get Media Player Still Files; File Name
			 * stillBank 	0-31: Still Bank
			 */
			char *  ATEMuni::getMediaPlayerStillFilesFileName(uint8_t stillBank) {
				return atemMediaPlayerStillFilesFileName[stillBank];
			}
			
			/**
			 * Get Macro Run Status; State
			 */
			uint8_t ATEMuni::getMacroRunStatusState() {
				return atemMacroRunStatusState;
			}
			
			/**
			 * Get Macro Run Status; Is Looping
			 */
			bool ATEMuni::getMacroRunStatusIsLooping() {
				return atemMacroRunStatusIsLooping;
			}
			
			/**
			 * Get Macro Run Status; Index
			 */
			uint16_t ATEMuni::getMacroRunStatusIndex() {
				return atemMacroRunStatusIndex;
			}
			
			/**
			 * Set Macro Action; Action
			 * index 	0-99: Macro Index Number. 0xFFFF: stop
			 * action 	0: Run Macro, 1: Stop (w/Index 0xFFFF), 2: Stop Recording (w/Index 0xFFFF), 3: Insert Wait for User (w/Index 0xFFFF), 4: Continue (w/Index 0xFFFF), 5: Delete Macro
			 */
			void ATEMuni::setMacroAction(uint16_t index, uint8_t action) {
			
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
			void ATEMuni::setMacroRunChangePropertiesLooping(bool looping) {
			
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
			bool ATEMuni::getMacroPropertiesIsUsed(uint8_t macroIndex) {
				return atemMacroPropertiesIsUsed[macroIndex];
			}
			
			/**
			 * Get Macro Properties; Name
			 * macroIndex 	0-9: Macro Index Number
			 */
			char *  ATEMuni::getMacroPropertiesName(uint8_t macroIndex) {
				return atemMacroPropertiesName[macroIndex];
			}
			
			/**
			 * Get Audio Mixer Input; Mix Option
			 * audioSource 	(See audio source list)
			 */
			uint8_t ATEMuni::getAudioMixerInputMixOption(uint16_t audioSource) {
				return atemAudioMixerInputMixOption[getAudioSrcIndex(audioSource)];
			}
			
			/**
			 * Get Audio Mixer Input; Volume
			 * audioSource 	(See audio source list)
			 */
			uint16_t ATEMuni::getAudioMixerInputVolume(uint16_t audioSource) {
				return atemAudioMixerInputVolume[getAudioSrcIndex(audioSource)];
			}
			
			/**
			 * Get Audio Mixer Input; Balance
			 * audioSource 	(See audio source list)
			 */
			int ATEMuni::getAudioMixerInputBalance(uint16_t audioSource) {
				return atemAudioMixerInputBalance[getAudioSrcIndex(audioSource)];
			}
			
			/**
			 * Set Audio Mixer Input; Mix Option
			 * audioSource 	(See audio source list)
			 * mixOption 	0: Off, 1: On, 2: AFV
			 */
			void ATEMuni::setAudioMixerInputMixOption(uint16_t audioSource, uint8_t mixOption) {
			
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
			void ATEMuni::setAudioMixerInputVolume(uint16_t audioSource, uint16_t volume) {
			
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
			void ATEMuni::setAudioMixerInputBalance(uint16_t audioSource, int balance) {
			
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
			uint16_t ATEMuni::getAudioMixerMasterVolume() {
				return atemAudioMixerMasterVolume;
			}
			
			/**
			 * Set Audio Mixer Master; Volume
			 * volume 	0-65381: (DB)
			 */
			void ATEMuni::setAudioMixerMasterVolume(uint16_t volume) {
			
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
			bool ATEMuni::getAudioMixerMonitorMonitorAudio() {
				return atemAudioMixerMonitorMonitorAudio;
			}
			
			/**
			 * Get Audio Mixer Monitor; Volume
			 */
			uint16_t ATEMuni::getAudioMixerMonitorVolume() {
				return atemAudioMixerMonitorVolume;
			}
			
			/**
			 * Get Audio Mixer Monitor; Mute
			 */
			bool ATEMuni::getAudioMixerMonitorMute() {
				return atemAudioMixerMonitorMute;
			}
			
			/**
			 * Get Audio Mixer Monitor; Solo
			 */
			bool ATEMuni::getAudioMixerMonitorSolo() {
				return atemAudioMixerMonitorSolo;
			}
			
			/**
			 * Get Audio Mixer Monitor; Solo Input
			 */
			uint16_t ATEMuni::getAudioMixerMonitorSoloInput() {
				return atemAudioMixerMonitorSoloInput;
			}
			
			/**
			 * Get Audio Mixer Monitor; Dim
			 */
			bool ATEMuni::getAudioMixerMonitorDim() {
				return atemAudioMixerMonitorDim;
			}
			
			/**
			 * Set Audio Mixer Monitor; Monitor Audio
			 * monitorAudio 	Bit 0: On/Off
			 */
			void ATEMuni::setAudioMixerMonitorMonitorAudio(bool monitorAudio) {
			
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
			void ATEMuni::setAudioMixerMonitorVolume(uint16_t volume) {
			
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
			void ATEMuni::setAudioMixerMonitorMute(bool mute) {
			
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
			void ATEMuni::setAudioMixerMonitorSolo(bool solo) {
			
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
			void ATEMuni::setAudioMixerMonitorSoloInput(uint16_t soloInput) {
			
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
			void ATEMuni::setAudioMixerMonitorDim(bool dim) {
			
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
			void ATEMuni::setAudioLevelsEnable(bool enable) {
			
	  	  		_prepareCommandPacket(PSTR("SALN"),4);
		
				_packetBuffer[12+_cBBO+4+4+0] = enable;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Get Audio Mixer Levels; Sources
			 */
			uint16_t ATEMuni::getAudioMixerLevelsSources() {
				return atemAudioMixerLevelsSources;
			}
			
			/**
			 * Get Audio Mixer Levels; Master Left
			 */
			long ATEMuni::getAudioMixerLevelsMasterLeft() {
				return atemAudioMixerLevelsMasterLeft;
			}
			
			/**
			 * Get Audio Mixer Levels; Master Right
			 */
			long ATEMuni::getAudioMixerLevelsMasterRight() {
				return atemAudioMixerLevelsMasterRight;
			}
			
			/**
			 * Get Audio Mixer Levels; Master Peak Left
			 */
			long ATEMuni::getAudioMixerLevelsMasterPeakLeft() {
				return atemAudioMixerLevelsMasterPeakLeft;
			}
			
			/**
			 * Get Audio Mixer Levels; Master Peak Right
			 */
			long ATEMuni::getAudioMixerLevelsMasterPeakRight() {
				return atemAudioMixerLevelsMasterPeakRight;
			}
			
			/**
			 * Get Audio Mixer Levels; Monitor
			 */
			long ATEMuni::getAudioMixerLevelsMonitor() {
				return atemAudioMixerLevelsMonitor;
			}
			
			/**
			 * Get Audio Mixer Levels; Source Order
			 * sources 	0-24: Number of
			 */
			uint16_t ATEMuni::getAudioMixerLevelsSourceOrder(uint16_t sources) {
				return atemAudioMixerLevelsSourceOrder[sources];
			}
			
			/**
			 * Get Audio Mixer Levels; Source Left
			 * sources 	0-24: Number of
			 */
			long ATEMuni::getAudioMixerLevelsSourceLeft(uint16_t sources) {
				return atemAudioMixerLevelsSourceLeft[sources];
			}
			
			/**
			 * Get Audio Mixer Levels; Source Right
			 * sources 	0-24: Number of
			 */
			long ATEMuni::getAudioMixerLevelsSourceRight(uint16_t sources) {
				return atemAudioMixerLevelsSourceRight[sources];
			}
			
			/**
			 * Get Audio Mixer Levels; Source Peak Left
			 * sources 	0-24: Number of
			 */
			long ATEMuni::getAudioMixerLevelsSourcePeakLeft(uint16_t sources) {
				return atemAudioMixerLevelsSourcePeakLeft[sources];
			}
			
			/**
			 * Get Audio Mixer Levels; Source Peak Right
			 * sources 	0-24: Number of
			 */
			long ATEMuni::getAudioMixerLevelsSourcePeakRight(uint16_t sources) {
				return atemAudioMixerLevelsSourcePeakRight[sources];
			}
			
			/**
			 * Get Audio Mixer Tally; Sources
			 */
			uint16_t ATEMuni::getAudioMixerTallySources() {
				return atemAudioMixerTallySources;
			}
			
			/**
			 * Get Audio Mixer Tally; Audio Source
			 * sources 	0-24: Number of
			 */
			uint16_t ATEMuni::getAudioMixerTallyAudioSource(uint16_t sources) {
				return atemAudioMixerTallyAudioSource[sources];
			}
			
			/**
			 * Get Audio Mixer Tally; IsMixedIn
			 * sources 	0-24: Number of
			 */
			bool ATEMuni::getAudioMixerTallyIsMixedIn(uint16_t sources) {
				return atemAudioMixerTallyIsMixedIn[sources];
			}
			
			/**
			 * Get Tally By Source; Sources
			 */
			uint16_t ATEMuni::getTallyBySourceSources() {
				return atemTallyBySourceSources;
			}
			
			/**
			 * Get Tally By Source; Video Source
			 * sources 	0-41: Number of
			 */
			uint16_t ATEMuni::getTallyBySourceVideoSource(uint16_t sources) {
				return atemTallyBySourceVideoSource[sources];
			}
			
			/**
			 * Get Tally By Source; Tally Flags
			 * sources 	0-41: Number of
			 */
			uint8_t ATEMuni::getTallyBySourceTallyFlags(uint16_t sources) {
				return atemTallyBySourceTallyFlags[sources];
			}
			

	