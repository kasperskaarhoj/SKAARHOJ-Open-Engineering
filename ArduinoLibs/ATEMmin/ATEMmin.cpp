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
#include "ATEMmin.h"


/**
 * Constructor (using arguments is deprecated! Use begin() instead)
 */
ATEMmin::ATEMmin(){}




		// *********************************
		// **
		// ** Implementations in ATEMmin.c:
		// **
		// *********************************


		// *********************************
		// **
		// ** Implementations in ATEMmin.c:
		// **
		// *********************************

		void ATEMmin::_parseGetCommands(const char *cmdStr)	{
			uint8_t mE,keyer,aUXChannel;
			uint16_t sources;
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
			 * Get Program Input; Video Source
			 * mE 	0: ME1, 1: ME2
			 */
			uint16_t ATEMmin::getProgramInputVideoSource(uint8_t mE) {
				return atemProgramInputVideoSource[mE];
			}
			
			/**
			 * Set Program Input; Video Source
			 * mE 	0: ME1, 1: ME2
			 * videoSource 	(See video source list)
			 */
			void ATEMmin::setProgramInputVideoSource(uint8_t mE, uint16_t videoSource) {
			
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
			uint16_t ATEMmin::getPreviewInputVideoSource(uint8_t mE) {
				return atemPreviewInputVideoSource[mE];
			}
			
			/**
			 * Set Preview Input; Video Source
			 * mE 	0: ME1, 1: ME2
			 * videoSource 	(See video source list)
			 */
			void ATEMmin::setPreviewInputVideoSource(uint8_t mE, uint16_t videoSource) {
			
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
			void ATEMmin::performCutME(uint8_t mE) {
			
	  	  		_prepareCommandPacket(PSTR("DCut"),4);
		
				_packetBuffer[12+_cBBO+4+4+0] = mE;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Set Auto; M/E
			 * mE 	0: ME1, 1: ME2
			 */
			void ATEMmin::performAutoME(uint8_t mE) {
			
	  	  		_prepareCommandPacket(PSTR("DAut"),4);
		
				_packetBuffer[12+_cBBO+4+4+0] = mE;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Get Transition Position; In Transition
			 * mE 	0: ME1, 1: ME2
			 */
			bool ATEMmin::getTransitionInTransition(uint8_t mE) {
				return atemTransitionInTransition[mE];
			}
			
			/**
			 * Get Transition Position; Frames Remaining
			 * mE 	0: ME1, 1: ME2
			 */
			uint8_t ATEMmin::getTransitionFramesRemaining(uint8_t mE) {
				return atemTransitionFramesRemaining[mE];
			}
			
			/**
			 * Get Transition Position; Position
			 * mE 	0: ME1, 1: ME2
			 */
			uint16_t ATEMmin::getTransitionPosition(uint8_t mE) {
				return atemTransitionPosition[mE];
			}
			
			/**
			 * Set Transition Position; Position
			 * mE 	0: ME1, 1: ME2
			 * position 	0-9999
			 */
			void ATEMmin::setTransitionPosition(uint8_t mE, uint16_t position) {
			
	  	  		_prepareCommandPacket(PSTR("CTPs"),4,(_packetBuffer[12+_cBBO+4+4+0]==mE));
		
				_packetBuffer[12+_cBBO+4+4+0] = mE;
				
				_packetBuffer[12+_cBBO+4+4+2] = highByte(position);
				_packetBuffer[12+_cBBO+4+4+3] = lowByte(position);
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Get Keyer On Air; Enabled
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 */
			bool ATEMmin::getKeyerOnAirEnabled(uint8_t mE, uint8_t keyer) {
				return atemKeyerOnAirEnabled[mE][keyer];
			}
			
			/**
			 * Set Keyer On Air; Enabled
			 * mE 	0: ME1, 1: ME2
			 * keyer 	0-3: Keyer 1-4
			 * enabled 	Bit 0: On/Off
			 */
			void ATEMmin::setKeyerOnAirEnabled(uint8_t mE, uint8_t keyer, bool enabled) {
			
	  	  		_prepareCommandPacket(PSTR("CKOn"),4,(_packetBuffer[12+_cBBO+4+4+0]==mE) && (_packetBuffer[12+_cBBO+4+4+1]==keyer));
		
				_packetBuffer[12+_cBBO+4+4+0] = mE;
				
				_packetBuffer[12+_cBBO+4+4+1] = keyer;
				
				_packetBuffer[12+_cBBO+4+4+2] = enabled;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Get Downstream Keyer; On Air
			 * keyer 	0: DSK1, 1: DSK2
			 */
			bool ATEMmin::getDownstreamKeyerOnAir(uint8_t keyer) {
				return atemDownstreamKeyerOnAir[keyer];
			}
			
			/**
			 * Get Downstream Keyer; In Transition
			 * keyer 	0: DSK1, 1: DSK2
			 */
			bool ATEMmin::getDownstreamKeyerInTransition(uint8_t keyer) {
				return atemDownstreamKeyerInTransition[keyer];
			}
			
			/**
			 * Get Downstream Keyer; Is Auto Transitioning
			 * keyer 	0: DSK1, 1: DSK2
			 */
			bool ATEMmin::getDownstreamKeyerIsAutoTransitioning(uint8_t keyer) {
				return atemDownstreamKeyerIsAutoTransitioning[keyer];
			}
			
			/**
			 * Get Downstream Keyer; Frames Remaining
			 * keyer 	0: DSK1, 1: DSK2
			 */
			uint8_t ATEMmin::getDownstreamKeyerFramesRemaining(uint8_t keyer) {
				return atemDownstreamKeyerFramesRemaining[keyer];
			}
			
			/**
			 * Set Downstream Keyer; On Air
			 * keyer 	0: DSK1, 1: DSK2
			 * onAir 	Bit 0: On/Off
			 */
			void ATEMmin::setDownstreamKeyerOnAir(uint8_t keyer, bool onAir) {
			
	  	  		_prepareCommandPacket(PSTR("CDsL"),4,(_packetBuffer[12+_cBBO+4+4+0]==keyer));
		
				_packetBuffer[12+_cBBO+4+4+0] = keyer;
				
				_packetBuffer[12+_cBBO+4+4+1] = onAir;
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Get Fade-To-Black State; Fully Black
			 * mE 	0: ME1, 1: ME2
			 */
			bool ATEMmin::getFadeToBlackStateFullyBlack(uint8_t mE) {
				return atemFadeToBlackStateFullyBlack[mE];
			}
			
			/**
			 * Get Fade-To-Black State; In Transition
			 * mE 	0: ME1, 1: ME2
			 */
			bool ATEMmin::getFadeToBlackStateInTransition(uint8_t mE) {
				return atemFadeToBlackStateInTransition[mE];
			}
			
			/**
			 * Get Fade-To-Black State; Frames Remaining
			 * mE 	0: ME1, 1: ME2
			 */
			uint8_t ATEMmin::getFadeToBlackStateFramesRemaining(uint8_t mE) {
				return atemFadeToBlackStateFramesRemaining[mE];
			}
			


				/**
				 * Set Fade-To-Black; M/E
				 * mE 	0: ME1, 1: ME2
				 */
				void ATEMmin::performFadeToBlackME(uint8_t mE) {

		  	  		_prepareCommandPacket(PSTR("FtbA"),4);

					_packetBuffer[12+_cBBO+4+4+0] = mE;
					_packetBuffer[12+_cBBO+4+4+1] = 0x02;

					_finishCommandPacket();

				}
			
			/**
			 * Get Aux Source; Input
			 * aUXChannel 	0-5: Aux 1-6
			 */
			uint16_t ATEMmin::getAuxSourceInput(uint8_t aUXChannel) {
				return atemAuxSourceInput[aUXChannel];
			}
			
			/**
			 * Set Aux Source; Input
			 * aUXChannel 	0-5: Aux 1-6
			 * input 	(See video source list)
			 */
			void ATEMmin::setAuxSourceInput(uint8_t aUXChannel, uint16_t input) {
			
	  	  		_prepareCommandPacket(PSTR("CAuS"),4,(_packetBuffer[12+_cBBO+4+4+1]==aUXChannel));
		
					// Set Mask: 1
				_packetBuffer[12+_cBBO+4+4+0] |= 1;
						
				_packetBuffer[12+_cBBO+4+4+1] = aUXChannel;
				
				_packetBuffer[12+_cBBO+4+4+2] = highByte(input);
				_packetBuffer[12+_cBBO+4+4+3] = lowByte(input);
				
	 	   		_finishCommandPacket();
		
			}
			
			/**
			 * Get Tally By Index; Sources
			 */
			uint16_t ATEMmin::getTallyByIndexSources() {
				return atemTallyByIndexSources;
			}
			
			/**
			 * Get Tally By Index; Tally Flags
			 * sources 	0-20: Number of
			 */
			uint8_t ATEMmin::getTallyByIndexTallyFlags(uint16_t sources) {
				return atemTallyByIndexTallyFlags[sources];
			}