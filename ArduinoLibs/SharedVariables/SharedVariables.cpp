/*
Copyright 2015 Kasper Skårhøj, KS Engineering, kasperskaarhoj@gmail.com
Developed for LearningLab DTU, Technical University of Denmark

The SharedVariables library is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published by the 
Free Software Foundation, either version 3 of the License, or (at your 
option) any later version.

The SharedVariables library is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
or FITNESS FOR A PARTICULAR PURPOSE. 
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along 
with the SharedVariables library. If not, see http://www.gnu.org/licenses/.


IMPORTANT: If you want to use this library in your own projects and/or products,
please play a fair game and heed the license rules! See our web page for a Q&A so
you can keep a clear conscience: http://skaarhoj.com/about/licenses/

*/



#include "SharedVariables.h"


/**
 * Constructor
 * (every registered variable consumes 12 bytes of memory)
 */
SharedVariables::SharedVariables(uint8_t numberOfVariables, uint8_t numberOfWidgets){	
	_numberOfVars = numberOfVariables;
	
	_varReferences = (void **) malloc (sizeof(void **) * _numberOfVars);
	_varType = (uint8_t *) malloc (sizeof(uint8_t) * _numberOfVars);
	_varSize = (uint8_t *) malloc (sizeof(uint8_t) * _numberOfVars);
	_varMin = (int *) malloc (sizeof(int) * _numberOfVars);
	_varMax = (int *) malloc (sizeof(int) * _numberOfVars);
	_varName = (const char **) malloc (sizeof(char **) * _numberOfVars);
	_varDescr = (const char **) malloc (sizeof(char **) * _numberOfVars);
	
	for(uint8_t i=0; i<_numberOfVars; i++)	{
		_varType[i]=0;
	}
		
	_numberOfWidgets = numberOfWidgets;
	_widgetPointer = 0;
	if (_numberOfWidgets)	{
		_widgetIdx = (uint8_t *) malloc (sizeof(uint8_t) * _numberOfWidgets);
		_widgetLabel = (const char **) malloc (sizeof(char **) * _numberOfWidgets);
		_widgetCategory = (const char **) malloc (sizeof(char **) * _numberOfWidgets);
		_widgetSortOrder = (uint8_t *) malloc (sizeof(uint8_t) * _numberOfWidgets);
		_widgetType = (uint8_t *) malloc (sizeof(uint8_t) * _numberOfWidgets);
		_widgetFlags = (uint8_t *) malloc (sizeof(uint8_t) * _numberOfWidgets);
		_widgetParams = (const char **) malloc (sizeof(char **) * _numberOfWidgets);
		_widgetPin = (int *) malloc (sizeof(int) * _numberOfWidgets);
		_widgetCmdVariable = (uint8_t *) malloc (sizeof(uint8_t) * _numberOfWidgets);
	}

	_externalChangeOfVariableCallback = 0;
	_externalReadResponseCallback = 0;
	
	_currentBundleAddr = 0;
	_attachPointer = 0;
}

/**
 * Associate a variable for sharing, integers
 * NOTE: Names for sharing variables is the string by which the variable is referenced via the buffer interface - make sure not to name two variables the same or substrings of each other, for instance dont use names like "My Var" and "My Var2" because "My Var" is a substring of "My Var2". This may lead to unexpected behaviours
 */
void SharedVariables::shareLocalVariable(uint8_t idx, int16_t &variableRef, uint8_t rw, const char *name, const char *descr, int16_t min, int16_t max) {
	if (idx < _numberOfVars)	{
//		Serial.println((int)variableRef);	// Value
//		Serial.println((int)&variableRef);	// Memory Address

		shareLocalVariable(idx, (void *)&variableRef, name, descr, min, max);
		_varType[idx] = 0 | (rw << 6);

/*		Serial.println("-------");
		Serial.println((int)_varReferences[idx]);	// Value (which is the memory address)
		Serial.println((int)&_varReferences[idx]);	// Memory Address of the pointer
		Serial.println((int)*static_cast<int*>(_varReferences[idx]));	// Value of the pointers pointer
		

		Serial.println((int)_rw[idx]);	// Value
		Serial.println((int)&_rw[idx]);	// Memory address
		Serial.println("-------");
*/	}
}

/**
 * Associate a variable for sharing, booleans
 */
void SharedVariables::shareLocalVariable(uint8_t idx, bool &variableRef, uint8_t rw, const char *name, const char *descr) {
	if (idx < _numberOfVars)	{
		shareLocalVariable(idx, (void *)&variableRef, name, descr);
		_varType[idx] = 1 | (rw << 6);
	}
}

/**
 * Associate a variable for sharing, uint8_t
 */
void SharedVariables::shareLocalVariable(uint8_t idx, uint8_t &variableRef, uint8_t rw, const char *name, const char *descr, uint8_t min, uint8_t max) {
	if (idx < _numberOfVars)	{
		shareLocalVariable(idx, (void *)&variableRef, name, descr, min, max);
		_varType[idx] = 2 | (rw << 6);
	}
}

/**
 * Associate a variable for sharing, uint16_t
 */
void SharedVariables::shareLocalVariable(uint8_t idx, uint16_t &variableRef, uint8_t rw, const char *name, const char *descr, uint16_t min, uint16_t max) {
	if (idx < _numberOfVars)	{
		shareLocalVariable(idx, (void *)&variableRef, name, descr, min, max);
		_varType[idx] = 3 | (rw << 6);
	}
}

/**
 * Associate a variable for sharing, long
 */
void SharedVariables::shareLocalVariable(uint8_t idx, int32_t &variableRef, uint8_t rw, const char *name, const char *descr) {
	if (idx < _numberOfVars)	{
		shareLocalVariable(idx, (void *)&variableRef, name, descr);
		_varType[idx] = 4 | (rw << 6);
	}
}

/**
 * Associate a variable for sharing, unsigned long
 */
void SharedVariables::shareLocalVariable(uint8_t idx, uint32_t &variableRef, uint8_t rw, const char *name, const char *descr) {
	if (idx < _numberOfVars)	{
		shareLocalVariable(idx, (void *)&variableRef, name, descr);
		_varType[idx] = 5 | (rw << 6);
	}
}

/**
 * Associate a variable for sharing, float (32 bit only atm.)
 */
void SharedVariables::shareLocalVariable(uint8_t idx, float &variableRef, uint8_t rw, const char *name, const char *descr, int min, int max) {
	if (idx < _numberOfVars)	{
		shareLocalVariable(idx, (void *)&variableRef, name, descr, min, max);
		_varType[idx] = 6 | (rw << 6);
	}
}

/**
 * Associate a variable for sharing, char (single character)
 */
void SharedVariables::shareLocalVariable(uint8_t idx, char &variableRef, uint8_t rw, const char *name, const char *descr) {
	if (idx < _numberOfVars)	{
		shareLocalVariable(idx, (void *)&variableRef, name, descr);
		_varType[idx] = 7 | (rw << 6);
	}
}

/**
 * Associate a variable for sharing, strings (arrays of chars)
 */
void SharedVariables::shareLocalVariable(uint8_t idx, const char * variableRef, uint8_t theSize, uint8_t rw, const char *name, const char *descr) {
	if (idx < _numberOfVars)	{
		shareLocalVariable(idx, (void *)variableRef, name, descr);
		_varType[idx] = 8 | (rw << 6);
		_varSize[idx] = theSize;
	}
}

/**
 * Associate a variable for sharing, arrays of uint8_t
 */
void SharedVariables::shareLocalVariable(uint8_t idx, const uint8_t * variableRef, uint8_t theSize, uint8_t rw, const char *name, const char *descr) {
	if (idx < _numberOfVars)	{
		shareLocalVariable(idx, (void *)variableRef, name, descr);
		_varType[idx] = 9 | (rw << 6);
		_varSize[idx] = theSize;
	}
}

/**
 * Associate a variable for sharing, arrays of integers
 */
void SharedVariables::shareLocalVariable(uint8_t idx, const int16_t * variableRef, uint16_t theSize, uint8_t rw, const char *name, const char *descr) {
	if (idx < _numberOfVars)	{
		shareLocalVariable(idx, (void *)variableRef, name, descr);
		_varType[idx] = 10 | (rw << 6);
		_varSize[idx] = theSize/2;	// two bytes per integer
	}
}

/**
 * Associate a variable for sharing, arrays of uint16_t
 */
void SharedVariables::shareLocalVariable(uint8_t idx, const uint16_t * variableRef, uint16_t theSize, uint8_t rw, const char *name, const char *descr) {
	if (idx < _numberOfVars)	{
		shareLocalVariable(idx, (void *)variableRef, name, descr);
		_varType[idx] = 11 | (rw << 6);
		_varSize[idx] = theSize/2;	// two bytes per 16-bit word
	}
}

/**
 * Associate a variable for sharing, arrays of longs
 */
void SharedVariables::shareLocalVariable(uint8_t idx, const int32_t * variableRef, uint16_t theSize, uint8_t rw, const char *name, const char *descr) {
	if (idx < _numberOfVars)	{
		shareLocalVariable(idx, (void *)variableRef, name, descr);
		_varType[idx] = 12 | (rw << 6);
		_varSize[idx] = theSize/4;	// four bytes per long
	}
}


/**
 * Associate a variable for sharing: For internal use, does the hard work for all of the above functions.
 */
void SharedVariables::shareLocalVariable(uint8_t idx, void *voidPointer, const char *name, const char *descr, int min, int max) {
	_varReferences[idx] = voidPointer;
	_varMin[idx] = min;
	_varMax[idx] = max;
	_varName[idx] = name;
	_varDescr[idx] = descr;
	_varSize[idx] = 0;	
}


/**
 * 
 * idx: Reference to a variable index
 * label: PSTR() label for the value (used in the interface)
 * category: PSTR() label for the category (tab) in which to place the value. This string value is used to group the values in tabs or otherwise.
 * sortOrder: used to sort the values in the particular group they belong to
 * widgetType: 0 = Display of value, 1 = Button/Switch (Boolean), 2 = Slider, 3 = Combobox / Radio buttons
 * flags, bit 0-1: Layouts 0-3 (actual rendering and detail level depends the widget type)
	* Display: Layout 0: Simple text string in small font, 1: Normal font size [Label] [Value] rendering, 2: [Label] on line one and [Value] in big on line two, 3: Like (2) but with a live graph depicting the value development over time.
	* Button: Layout 0: Regular button; Layout 1: Big button; Layout 2: [Label] [Switch]
	* Slider: ?
	* Combo: Layout 0: Regular combo; Layout 1: Option buttons (radio buttons)
 * flags, bit 2-6: Various flags, depends on widget type
	* Display: Bit 2: Add Reset-to-zero button; Bit 3: Add Edit-value button
	* Button: Bit 2-3: 0=Toggling boolean, 1=True on press, 2=True on press, false on release
	* Slider: Bit 2: Display value; Bit 3: Edit value by tapping value field; Bit 4: Spring back to center upon release
	* Combo: Bit 4: Spring back to zero upon release (for radio buttons)
 * flags, bit 7: For arrays; if set, tries to organize the widgets horizontally rather than vertically
 * parameters: Text string, semicolon separated list of []=[] pairs.  Depends on widget type
	* Display: #Unit=[m3, kWh, sec etc.]; [value]=[label]{color as #xxxxxx}
	* Button: #True=[Value of true if not "1"]; [value]=[label]{color as #xxxxxx}
	* Combobox: [value]=[label]{color as #xxxxxx}
	* [For arrays]: also: #Label[0]=[label]; ... etc. will rendering labels for each value.
 * pinCode: If > 0, this pincode (or master pin code) must be set to render the field; If =-1, master pin code must be set.
 * cmdVariable: If != 255, this idx-value (must be boolean) will be sent as well right after, assumed to trigger an action related to the changed value
 */
void SharedVariables::addWidget(uint8_t idx, const char *label, const char *category, uint8_t sortOrder, uint8_t widgetType, uint8_t flags, const char *parameters, int pinCode, uint8_t cmdVariable) {
  	if (_widgetPointer < _numberOfWidgets)	{
  		_widgetIdx[_widgetPointer] = idx;
		_widgetLabel[_widgetPointer] = label;
		_widgetCategory[_widgetPointer] = category;
		_widgetSortOrder[_widgetPointer] = sortOrder;
		_widgetType[_widgetPointer] = widgetType;
		_widgetFlags[_widgetPointer] = flags;
		_widgetParams[_widgetPointer] = parameters;
		_widgetPin[_widgetPointer] = pinCode;
		_widgetCmdVariable[_widgetPointer] = cmdVariable;

		_widgetPointer++;
  	} else {
  		Serial << F("ERROR: Widget couldn't be added, no more space\n");
  	}
}



/**
 * Testing the reference of an index by changing the value. ONLY for testing!
 */
void SharedVariables::test(uint8_t idx) {
	if (idx < _numberOfVars && _varType[idx]>0)	{
		uint8_t tmp[] = {9,8,7,6};

		switch(_varType[idx]&0xF)	{
			case 0: 	// int
				*static_cast<int*>(_varReferences[idx]) += 10;
				break;
			case 1: 	// bool
				*static_cast<bool*>(_varReferences[idx]) = !(*static_cast<bool*>(_varReferences[idx]));
				break;
			case 2: 	// uint8_t
				*static_cast<uint8_t*>(_varReferences[idx]) += 10;
				break;
			case 3: 	// uint16_t
				*static_cast<uint16_t*>(_varReferences[idx]) += 10;
				break;
			case 4: 	// long
				*static_cast<long*>(_varReferences[idx]) += 10;
				break;
			case 5: 	// unsigned long
				*static_cast<unsigned long*>(_varReferences[idx]) += 10;
				break;
			case 6: 	// float
				*static_cast<float*>(_varReferences[idx]) += 10;
				break;
			case 7: 	// char
				*static_cast<char*>(_varReferences[idx]) = 'C';
				break;
			case 8: 	// char array
				//Serial.println((char *)_varReferences[idx]);
				strncpy_P((char *)_varReferences[idx], PSTR("QRSTasdf"), _varSize[idx]-1);
				break;
			case 9: 	// uint8_t array
			    memcpy((uint8_t *)_varReferences[idx], tmp, _varSize[idx]);
				break;
			case 10: 	// int array
				for(uint8_t i=0; i<_varSize[idx]; i++)	{
					((int *)_varReferences[idx])[i] += i+1;
				}
				break;
			case 11: 	// uint16_t array
				for(uint8_t i=0; i<_varSize[idx]; i++)	{
					((uint16_t *)_varReferences[idx])[i] += i+1;
				}
				break;
			case 12: 	// long array
				for(uint8_t i=0; i<_varSize[idx]; i++)	{
					((long *)_varReferences[idx])[i] += i+1;
				}
				break;
			default:
				break;
		}
	}
}

/**
 * Print overview to &output
 */
void SharedVariables::printOverview(Print &output)	{
	output << F("IDX ");
	output << F("TYPE       ");
	output << F("RW ");
	output << F("RANGE:        ");
	output << F("NAME: DESCRIPTION");
	output << F("\n");
	for(uint8_t idx=0; idx<_numberOfVars; idx++)	{
		if (_varType[idx] > 0)	{
			output << _DECPADL(idx,2," ") << F("  ");

			switch(_varType[idx]&0xF)	{
				case 0: 	// int
					output << F("int        ");
					break;
				case 1: 	// bool
					output << F("bool       ");
					break;
				case 2: 	// uint8_t
					output << F("uint8_t    ");
					break;
				case 3: 	// uint16_t
					output << F("uint16_t   ");
					break;
				case 4: 	// long
					output << F("long       ");
					break;
				case 5: 	// unsigned long
					output << F("u-long     ");
					break;
				case 6: 	// float
					output << F("float      ");
					break;
				case 7: 	// char
					output << F("char       ");
					break;
				case 8: 	// char array
					output << F("char array ");
					break;
				case 9: 	// uint8_t array
					output << F("uint8_t ar ");
					break;
				case 10: 	// int array
					output << F("int array  ");
					break;
				case 11: 	// uint16_t array
					output << F("uint16_t ar");
					break;
				case 12: 	// long array
					output << F("long array ");
					break;
				default:
					break;
			}
			
			output << (readEnabled(idx) ? F("R"):F(" ")) << (writeEnabled(idx) ? F("W"):F(" ")) << F(" ");
			
			if (hasCustomRange(idx))	{
				switch(_varType[idx]&0xF)	{
					case 3:
						output << _DECPADL((uint16_t)_varMin[idx],6," ") << F(" ") << _DECPADL((uint16_t)_varMax[idx],6," ") << F(" ");
						break;
					default:
						output << _DECPADL(_varMin[idx],6," ") << F(" ") << _DECPADL(_varMax[idx],6," ") << F(" ");
						break;
				}
			} else if (_varSize[idx]>0) {
				output << F("  Length: ") << _DECPADL(_varSize[idx]-((_varType[idx]&0xF)==8?1:0),3," ") << F(" ");
			} else {
				output << F("              ");
			}
			
			output << ((__FlashStringHelper*) _varName[idx]) << F(": ");
			output << ((__FlashStringHelper*) _varDescr[idx]);
			
			output << F("\n");
		}
	}
}

/**
 * Print values to &output
 */
void SharedVariables::printValues(Print &output, bool readOnlyCheck)	{
	output << F("IDX ");
	output << F("NAME=VALUE");
	output << F("\n");
	for(uint8_t idx=0; idx<_numberOfVars; idx++)	{
		if (_varType[idx] > 0)	{
			if (!readOnlyCheck || readEnabled(idx))	{
				output << _DECPADL(idx,2," ") << F("  ");
			
				printSingleValue(output, idx);
			
				output << F("\n");
			}
		}
	}
}

/**
 * Returns true if the variable is read enabled
 */
bool SharedVariables::readEnabled(uint8_t idx)	{
	return (_varType[idx]>>6)&B10;
}

/**
 * Returns true if the variable is write enabled
 */
bool SharedVariables::writeEnabled(uint8_t idx)	{
	return (_varType[idx]>>6)&B01;
}

/**
 * Returns true if the variable has a custom range
 */
bool SharedVariables::hasCustomRange(uint8_t idx)	{
	return _varMin[idx]!=0 || _varMax[idx]!=0;
}

/**
 * Print single value to &output
 */
void SharedVariables::printSingleValue(Print &output, uint8_t idx)	{
	output << ((__FlashStringHelper*) _varName[idx]) << F("=");

	switch(_varType[idx]&0xF)	{
		case 0: 	// int
			output << *static_cast<int*>(_varReferences[idx]);
			break;
		case 1: 	// bool
			output << *static_cast<bool*>(_varReferences[idx]);
			break;
		case 2: 	// uint8_t
			output << *static_cast<uint8_t*>(_varReferences[idx]);
			break;
		case 3: 	// uint16_t
			output << *static_cast<uint16_t*>(_varReferences[idx]);
			break;
		case 4: 	// long
			output << *static_cast<long*>(_varReferences[idx]);
			break;
		case 5: 	// unsigned long
			output << *static_cast<unsigned long*>(_varReferences[idx]);
			break;
		case 6: 	// float
			output << *static_cast<float*>(_varReferences[idx]);
			break;
		case 7: 	// char
			output << *static_cast<char*>(_varReferences[idx]);
			break;
		case 8: 	// char array
			output << (char *)_varReferences[idx];
			break;
		case 9: 	// uint8_t array
			output << F("{");
			for(uint8_t i=0; i<_varSize[idx]; i++)	{
				output << ((uint8_t *)_varReferences[idx])[i] << (_varSize[idx]==i+1?F(""):F(","));
			}
			output << F("}");
			break;
		case 10: 	// int array
			output << F("{");
			for(uint8_t i=0; i<_varSize[idx]; i++)	{
				output << ((int *)_varReferences[idx])[i] << (_varSize[idx]==i+1?F(""):F(","));
			}
			output << F("}");
			break;
		case 11: 	// uint16_t array
			output << F("{");
			for(uint8_t i=0; i<_varSize[idx]; i++)	{
				output << ((uint16_t *)_varReferences[idx])[i] << (_varSize[idx]==i+1?F(""):F(","));
			}
			output << F("}");
			break;
		case 12: 	// long array
			output << F("{");
			for(uint8_t i=0; i<_varSize[idx]; i++)	{
				output << ((long *)_varReferences[idx])[i] << (_varSize[idx]==i+1?F(""):F(","));
			}
			output << F("}");
			break;
		default:
			break;
	}
}

/**
 * Processes an incoming buffer line inside the passed &bufferObj. Prints to &output
 */
void SharedVariables::incomingASCIILine(SkaarhojBufferTools &bufferObj, Print &output)	{
	
	bool commandOK = false;  // This must be set when a command is accepted, otherwise we will output a NACK as well.
	
	if (bufferObj.isBufferEqualTo_P(PSTR("help")))	{
		output << F("Below is the complete list of variables you can set.\n");
		output << F("Set a variable value by sending [NAME]=[VALUE]<CR>\n");
		output << F("Get a variable value by sending [NAME]?<CR>\n");
		output << F("Get all values by sending ALL?<CR>\n");
		printOverview(output);
		commandOK = true;
	} else
	if (bufferObj.isBufferEqualTo_P(PSTR("ALL?")))	{
		printValues(output, true);
		commandOK = true;
	} else {
		for(uint8_t idx=0; idx<_numberOfVars; idx++)	{
			if (_varType[idx] > 0)	{
				if (bufferObj.isNextPartOfBuffer_P(_varName[idx]))  {
					if (bufferObj.isNextPartOfBuffer_P(PSTR("=")))	{
						commandOK = true;

						if (writeEnabled(idx))	{
							bool customValueRange = hasCustomRange(idx);
							long valueBuffer;
							float valueBufferFloat;

							switch(_varType[idx]&0xF)	{
								case 0: 	// int
									valueBuffer = atol(bufferObj.getRemainingBuffer());
									if (valueBuffer >= (customValueRange?_varMin[idx]:-32768) && valueBuffer <= (customValueRange?_varMax[idx]:32767))	{
										*static_cast<int*>(_varReferences[idx]) = valueBuffer;
									} else {
										output << F("Out of range\n");
									}
									break;
								case 1: 	// bool
									valueBuffer = atol(bufferObj.getRemainingBuffer());
									if (valueBuffer == 0 || valueBuffer == 1)	{
										*static_cast<bool*>(_varReferences[idx]) = valueBuffer;
									} else {
										output << F("Out of range\n");
									}						
									break;
								case 2: 	// uint8_t
									valueBuffer = atol(bufferObj.getRemainingBuffer());
									if (valueBuffer >= (customValueRange?_varMin[idx]:0) && valueBuffer <= (customValueRange?_varMax[idx]:255))	{
										*static_cast<uint8_t*>(_varReferences[idx]) = valueBuffer;
									} else {
										output << F("Out of range\n");
									}
						
									break;
								case 3: 	// uint16_t
									valueBuffer = atol(bufferObj.getRemainingBuffer());
									if (valueBuffer >= (customValueRange?(uint16_t)_varMin[idx]:0) && valueBuffer <= (customValueRange?(uint16_t)_varMax[idx]:65535))	{
										*static_cast<uint16_t*>(_varReferences[idx]) = valueBuffer;
									} else {
										output << F("Out of range\n");
									}
						
									break;
								case 4: 	// long
									*static_cast<long*>(_varReferences[idx]) = atol(bufferObj.getRemainingBuffer());
									break;
								case 5: 	// unsigned long
									*static_cast<unsigned long*>(_varReferences[idx]) = atol(bufferObj.getRemainingBuffer());
									break;
								case 6: 	// float
									valueBufferFloat = atof(bufferObj.getRemainingBuffer());
									if (!customValueRange || (valueBufferFloat >= _varMin[idx] && valueBufferFloat <= _varMax[idx]))	{
										*static_cast<float*>(_varReferences[idx]) =  valueBufferFloat;
									} else {
										output << F("Out of range\n");
									}
									break;
								case 7: 	// char
									*static_cast<char*>(_varReferences[idx]) = (bufferObj.getRemainingBuffer())[0];
									break;
								case 8: 	// char array
									strncpy((char *)_varReferences[idx], bufferObj.getRemainingBuffer(), _varSize[idx]-1);
									break;
								case 9: 	// uint8_t array
								case 10: 	// int array
								case 11: 	// int array
								case 12: 	// int array
									if (bufferObj.isNextPartOfBuffer_P(PSTR("{")))	{
										for(uint8_t i=0; i<_varSize[idx]; i++)	{
											if ((_varType[idx]&0xF)==9)	{
												((uint8_t *)_varReferences[idx])[i] = bufferObj.parseInt();
											} else
											if ((_varType[idx]&0xF)==10)	{
												((int *)_varReferences[idx])[i] = bufferObj.parseInt();
											} else
											if ((_varType[idx]&0xF)==11)	{
												((uint16_t *)_varReferences[idx])[i] = bufferObj.parseInt();
											} else
											if ((_varType[idx]&0xF)==12)	{
												((long *)_varReferences[idx])[i] = bufferObj.parseInt();
											}
											if (!bufferObj.isNextPartOfBuffer_P(PSTR(",")))	{
												if (!bufferObj.isNextPartOfBuffer_P(PSTR("}")))	{
													output << F("No closing brace, but still accepted.\n");
												} else if (i+1<_varSize[idx]) {
													output << F("Not all values were passed, but still accepted.\n");
												}
												break;
											} 
										}
									} else {
										output << F("No opening brace found!\n");
									}
									break;
								default:
									break;
							}
							
							if (_externalChangeOfVariableCallback != 0)	_externalChangeOfVariableCallback(idx);
						} else {
							output << F("Not write enabled\n");
						}
					}
					if (commandOK || bufferObj.isNextPartOfBuffer_P(PSTR("?")))	{
						if (readEnabled(idx))	{
							printSingleValue(output, idx);
							output << F("\n");
						} else if (!commandOK) {
							output << F("Not read enabled\n");
						}
						commandOK = true;
					}

					break;	// No more values.
				}
			}
		}		
	}

    if (!commandOK) {  // NACK + hint is returned if we didn't recognize command
      output << F("NACK (type \"help\" for commands)\r\n");
    }
}
	  
	  
	  
	  
	  
/**
 * Parses incoming UDP messages and acts accordingly.
 */
void SharedVariables::incomingBinBuffer(UDPmessenger &UDPmessengerObj, const uint8_t cmd, const uint8_t from, const uint8_t dataLength, const uint8_t *dataArray)	{
	/*
	Serial << F("Incoming segment: ");
	Serial << F("from=") << from << F(", ");
	Serial << F("cmd=0x") << _HEXPADL(cmd,2,"0") << F(", ");
	Serial << F("dataLength=") << dataLength << F(": ");
	for(uint8_t i=0; i<dataLength; i++)	{
		Serial << _HEXPADL(dataArray[i],2,"0") << F(" ");
	}
	Serial << F("\n");
*/
	
	if (cmd==0x7F)	{	// Deliver PING response:
		UDPmessengerObj.addValueToDataBuffer(UDPmessengerObj.ownAddress(), 0);	// My address
		UDPmessengerObj.addValueToDataBuffer(_numberOfVars, 1);	// My number of shared variables
		UDPmessengerObj.addValueToDataBuffer(_numberOfWidgets, 2);	// My number of widgets
		UDPmessengerObj.addCommand(cmd | 0x80, 3);
	} else if (cmd==0x7E)	{	// Deliver INFO response:
		for(uint8_t i=0; i<dataLength; i++)  {
			sendBinaryInfo(UDPmessengerObj, dataArray[i] & 0x7F);
		}
	} else if (cmd==0x7D) {	// Deliver READ MULTIPLE VALUES response
		for(uint8_t i=0; i<dataLength; i++)  {	// Process any additional as read indexes also:
			sendBinaryReadResponse(UDPmessengerObj, dataArray[i] & 0x7F);
		}
	} else if (cmd==0x7C) {	// Deliver WIDGET response:
		for(uint8_t i=0; i<_numberOfWidgets; i++)  {	// Process any additional as read indexes also:
			sendBinaryWidgetSetup(UDPmessengerObj, i);
		}
	} else if (cmd < 0x78) {	// Regular values read <120
		if (dataLength>0)	{

			for(uint8_t i=0; i<_attachPointer; i++)	{
				if (_attachments[i][3]==0 && _attachments[i][1] == from && _attachments[i][2] == cmd)	{	// If !push and address/idx is right...
					storeBinaryValue(UDPmessengerObj, _attachments[i][0], dataLength, dataArray);
//										Serial << F("Stored attachment ") << i << F("-") << _attachments[i][0] << F("-") << validIdx(_attachments[i][0]) << F("-") << writeEnabled(_attachments[i][0]) << F("\n");
				}
			}
			if (_externalReadResponseCallback != NULL)	{
				_externalReadResponseCallback(cmd, from, dataLength, dataArray);
			}
		} else {
			sendBinaryReadResponse(UDPmessengerObj, cmd);	// Process for read
		}
	} else if ((cmd & 0x80) && (cmd & 0x7F) < 0x78)  {	// Regular values write
		storeBinaryValue(UDPmessengerObj, cmd & 0x7F, dataLength, dataArray);
	}
}
	  
/**
 * Sends binary ping
 */
void SharedVariables::sendUDPPing(const uint8_t slaveAddress)	{
	(*_messenger).resetTXBuffer();
	(*_messenger).addCommand(0x7E, 0);
	(*_messenger).send(slaveAddress, 8765);
}
	  
/**
 * Sends binary information
 */
void SharedVariables::sendBinaryInfo(UDPmessenger &UDPmessengerObj, const uint8_t idx)	{
	if (validIdx(idx))	{
	
		uint8_t nameLen = strlen_P((char *)_varName[idx]);
		uint8_t dLen = 7+nameLen;
		if (!UDPmessengerObj.fitCommand(dLen))	{	// If there is not enough space for this variable, just send the current buffer and reset it:
			UDPmessengerObj.send();
		}

		UDPmessengerObj.addValueToDataBuffer(idx & 0x7F, 0);	// bit7=0 - main info
		UDPmessengerObj.addValueToDataBuffer(_varType[idx], 1);
		UDPmessengerObj.addValueToDataBuffer(_varSize[idx], 2);
		UDPmessengerObj.addValueToDataBuffer(_varMin[idx], 3);
		UDPmessengerObj.addValueToDataBuffer(_varMin[idx]>>8, 4);
		UDPmessengerObj.addValueToDataBuffer(_varMax[idx], 5);
		UDPmessengerObj.addValueToDataBuffer(_varMax[idx]>>8, 6);

		for(uint8_t i=0; i<nameLen; i++)	{
			UDPmessengerObj.addValueToDataBuffer(pgm_read_byte_near(((char *)_varName[idx])+i), 7+i);
		}
		
		UDPmessengerObj.addCommand(0x7E | 0x80, dLen);


		// Sending description:
		uint8_t descrLen = strlen_P((char *)_varDescr[idx]);
		dLen = 2+descrLen;
		if (!UDPmessengerObj.fitCommand(dLen))	{	// If there is not enough space for this variable, just send the current buffer and reset it:
			UDPmessengerObj.send();
		}
		
		uint8_t maxStringLen = (UDPmessenger_BUFFER_SIZE-2-2-2);
		for(uint8_t j=0; j<descrLen/maxStringLen+1;j++)	{
			UDPmessengerObj.addValueToDataBuffer(idx | 0x80, 0);	// bit7=1 - descr info
			UDPmessengerObj.addValueToDataBuffer(j, 1);	// Index...

			uint8_t stringLen = j==(descrLen/maxStringLen) ? (descrLen%maxStringLen) : maxStringLen;
			for(uint8_t i=0; i<stringLen; i++)	{
				UDPmessengerObj.addValueToDataBuffer(pgm_read_byte_near(((char *)_varDescr[idx])+i+j*maxStringLen), 2+i);
			}
			UDPmessengerObj.addCommand(0x7E | 0x80, stringLen+2);
			UDPmessengerObj.send();
		}
	}
}
	  
/**
 * Sends binary widget set up
 */
void SharedVariables::sendBinaryWidgetSetup(UDPmessenger &UDPmessengerObj, const uint8_t widgetPointer)	{

	uint8_t labelLenWNull = strlen_P((char *)_widgetLabel[widgetPointer])+1;
	uint8_t categoryLenWNull = strlen_P((char *)_widgetCategory[widgetPointer])+1;
	
	uint8_t dLen = 8+labelLenWNull+categoryLenWNull;
	if (!UDPmessengerObj.fitCommand(dLen))	{	// If there is not enough space for this variable, just send the current buffer and reset it:
		UDPmessengerObj.send();
	}

	UDPmessengerObj.addValueToDataBuffer(widgetPointer , 0);	// Widget pointer
	UDPmessengerObj.addValueToDataBuffer(_widgetIdx[widgetPointer], 1);
	UDPmessengerObj.addValueToDataBuffer(_widgetSortOrder[widgetPointer], 2);
	UDPmessengerObj.addValueToDataBuffer(_widgetType[widgetPointer], 3);
	UDPmessengerObj.addValueToDataBuffer(_widgetFlags[widgetPointer], 4);
	UDPmessengerObj.addValueToDataBuffer(_widgetPin[widgetPointer], 5);
	UDPmessengerObj.addValueToDataBuffer(_widgetPin[widgetPointer]>>8, 6);
	UDPmessengerObj.addValueToDataBuffer(_widgetCmdVariable[widgetPointer], 7);

	for(uint8_t i=0; i<labelLenWNull; i++)	{
		UDPmessengerObj.addValueToDataBuffer(pgm_read_byte_near(((char *)_widgetLabel[widgetPointer])+i), 8+i);
	}
	for(uint8_t i=0; i<categoryLenWNull; i++)	{
		UDPmessengerObj.addValueToDataBuffer(pgm_read_byte_near(((char *)_widgetCategory[widgetPointer])+i), 8+labelLenWNull+i);
	}
	UDPmessengerObj.addCommand(0x7C | 0x80, dLen);



	// Sending description:
	uint8_t descrLen = strlen_P((char *)_widgetParams[widgetPointer]);
	dLen = 2+descrLen;
	if (!UDPmessengerObj.fitCommand(dLen))	{	// If there is not enough space for this variable, just send the current buffer and reset it:
		UDPmessengerObj.send();
	}
	
	uint8_t maxStringLen = (UDPmessenger_BUFFER_SIZE-2-2-2);
	for(uint8_t j=0; j<descrLen/maxStringLen+1;j++)	{
		UDPmessengerObj.addValueToDataBuffer(widgetPointer | 0x80, 0);	// bit7=1 - descr info
		UDPmessengerObj.addValueToDataBuffer(j, 1);	// Index...

		uint8_t stringLen = j==(descrLen/maxStringLen) ? (descrLen%maxStringLen) : maxStringLen;
		for(uint8_t i=0; i<stringLen; i++)	{
			UDPmessengerObj.addValueToDataBuffer(pgm_read_byte_near(((char *)_widgetParams[widgetPointer])+i+j*maxStringLen), 2+i);
		}
		UDPmessengerObj.addCommand(0x7C | 0x80, stringLen+2);
		UDPmessengerObj.send();
	}
}
	  
/**
 * Sends a local shared variable (if read enabled) as a read-response
 */
void SharedVariables::sendBinaryReadResponse(UDPmessenger &UDPmessengerObj, const uint8_t idx, bool noErrorIfReadEnabled)	{
	if (validIdx(idx))	{
		uint8_t dLen = 0;

		if (readEnabled(idx))	{
			switch(_varType[idx] & 0xF)	{
				case 0: 	// int
					dLen = 2;
					break;
				case 1: 	// bool
					dLen = 1;
					break;
				case 2: 	// uint8_t
					dLen = 1;
					break;
				case 3: 	// uint16_t
					dLen = 2;
					break;
				case 4: 	// int16_t
					dLen = 4;
					break;
				case 5: 	//  uint32_t
					dLen = 4;
					break;
				case 6: 	// float
					dLen = 4;
					break;
				case 7: 	// char
					dLen = 1;
					break;
				case 8: 	// char array
					dLen = _varSize[idx];
					break;
				case 9: 	// uint8_t array
					dLen = _varSize[idx];
					break;
				case 10: 	// int16_t array
					dLen = _varSize[idx]*2;
					break;
				case 11: 	// uint16_t array
					dLen = _varSize[idx]*2;
					break;
				case 12: 	// int32_t array
					dLen = _varSize[idx]*4;
					break;
				default:
				break;
			}
			if (dLen > 0)	{
				if (!UDPmessengerObj.fitCommand(dLen))	{	// If there is not enough space for this variable, just send the current buffer and reset it:
					UDPmessengerObj.send();
				}
			
				for(uint8_t i=0; i<dLen; i++)	{
					#if defined(__arm__) || defined(ESP8266)	/* Arduino DUE */
						/*if ((_varType[idx] & 0xF) == 12)	{	 	// integer array, is 32 bit on Due, LSB first, 4 bytes
							UDPmessengerObj.addValueToDataBuffer(((uint8_t *)_varReferences[idx])[i+4*(i>>2)], i);
						} else*/
						if ((_varType[idx] & 0xF) == 10)	{	 	// integer array, is 32 bit on Due, LSB first, 4 bytes
							UDPmessengerObj.addValueToDataBuffer(((uint8_t *)_varReferences[idx])[i+2*(i>>1)], i);
						} else {
							UDPmessengerObj.addValueToDataBuffer(((uint8_t *)_varReferences[idx])[i], i);
						}
					#else
						UDPmessengerObj.addValueToDataBuffer(((uint8_t *)_varReferences[idx])[i], i);
					#endif
				}
				UDPmessengerObj.addCommand(idx, dLen);
			}
		} else if (!noErrorIfReadEnabled) {
			Serial << F("ERROR idx:") << idx << F(" - Not read enabled\n");
		}
	}
}
	  
/**
 * Stores an incoming write operation in shared variables (if write enabled)
 */
void SharedVariables::storeBinaryValue(UDPmessenger &UDPmessengerObj, const uint8_t idx, const uint8_t dataLength, const uint8_t *dataArray)	{
	if (validIdx(idx))	{
		uint8_t dLen = 0;
		bool varLen = false;
		
		if (writeEnabled(idx))	{		
			switch(_varType[idx] & 0xF)	{
				case 0: 	// int16_t
					dLen = 2;
					break;
				case 1: 	// bool
					dLen = 1;
					break;
				case 2: 	// uint8_t
					dLen = 1;
					break;
				case 3: 	// uint16_t
					dLen = 2;
					break;
				case 4: 	// int32_t
					dLen = 4;
					break;
				case 5: 	// uint32_t
					dLen = 4;
					break;
				case 6: 	// float
					dLen = 4;
					break;
				case 7: 	// char
					dLen = 1;
					break;
				case 8: 	// char array
					dLen = _varSize[idx];
					dLen = dataLength < dLen ? dataLength : dLen;
					varLen = true;
					break;
				case 9: 	// uint8_t array
					dLen = _varSize[idx];
					dLen = dataLength < dLen ? dataLength : dLen;
					varLen = true;
					break;
				case 10: 	// int16_t array
				case 11: 	// uint16_t array
					dLen = _varSize[idx]*2;
					dLen = dataLength < dLen ? dataLength : dLen;
					varLen = true;
					break;
				case 12: 	// int32_T array
					dLen = _varSize[idx]*4;
					dLen = dataLength < dLen ? dataLength : dLen;
					varLen = true;
					break;
				default:
				break;
			}
			if ((dLen > 0 && dLen==dataLength) || varLen)	{
				for(uint8_t i=0; i<dLen; i++)	{
					#if defined(__arm__) || defined(ESP8266)	/* 32 bit microcontroller */
					/*	if ((_varType[idx] & 0xF) == 12)	{	 	// integer array, is 32 bit on Due, LSB first, 4 bytes
							((uint8_t *)_varReferences[idx])[i+4*(i>>2)] = dataArray[i];
							((uint8_t *)_varReferences[idx])[i+4+4*(i>>2)] = dataArray[4*(i>>2)+3] & 0x80 ? 0xFF : 0x00;
						} else */
						if ((_varType[idx] & 0xF) == 10)	{	 	// integer array, is 32 bit on Due, LSB first, 4 bytes
							((uint8_t *)_varReferences[idx])[i+2*(i>>1)] = dataArray[i];
							((uint8_t *)_varReferences[idx])[i+2+2*(i>>1)] = dataArray[2*(i>>1)+1] & 0x80 ? 0xFF : 0x00;
						} else {
							((uint8_t *)_varReferences[idx])[i] = dataArray[i];
						}
					#else
						((uint8_t *)_varReferences[idx])[i] = dataArray[i];
					#endif
				}
				if ((_varType[idx] & 0xF) == 8)	{	 	// char array
					((uint8_t *)_varReferences[idx])[dLen-1] = 0;	// Make sure its nul terminated if string
				}

				sendBinaryReadResponse(UDPmessengerObj, idx, true);	// Return...

				if (_externalChangeOfVariableCallback != 0)	_externalChangeOfVariableCallback(idx); 
			} else {
				Serial << F("ERROR idx:") << idx << F(" dLen=") << dLen << F(" != dataLength=") << dataLength << F("\n");
			}
		} else {
			Serial << F("ERROR idx:") << idx << F(" - Not write enabled\n");
		}
	}	
}
	  
	  
/**
 * Sets a remote variable on a UDP slave over UDP
 * When using these functions, remember to cast the variable to the same type it has on the receiving slave!
 */
void SharedVariables::setRemoteVariableOverUDP(int variableRef, const uint8_t slaveAddress, const uint8_t slaveIdx) {
	//Serial << F("int\n");
	setRemoteVariableOverUDP((void *)&variableRef, 2, slaveAddress, slaveIdx);
}	  
void SharedVariables::setRemoteVariableOverUDP(bool variableRef, const uint8_t slaveAddress, const uint8_t slaveIdx) {
	//Serial << F("bool\n");
	setRemoteVariableOverUDP((void *)&variableRef, 1, slaveAddress, slaveIdx);
}	  
void SharedVariables::setRemoteVariableOverUDP(uint8_t variableRef, const uint8_t slaveAddress, const uint8_t slaveIdx) {
	//Serial << F("uint8_t\n");
	setRemoteVariableOverUDP((void *)&variableRef, 1, slaveAddress, slaveIdx);
}	  
void SharedVariables::setRemoteVariableOverUDP(uint16_t variableRef, const uint8_t slaveAddress, const uint8_t slaveIdx) {
	//Serial << F("uint16_t\n");
	setRemoteVariableOverUDP((void *)&variableRef, 2, slaveAddress, slaveIdx);
}	  
void SharedVariables::setRemoteVariableOverUDP(long variableRef, const uint8_t slaveAddress, const uint8_t slaveIdx) {
	//Serial << F("long\n");
	setRemoteVariableOverUDP((void *)&variableRef, 4, slaveAddress, slaveIdx);
}	  
void SharedVariables::setRemoteVariableOverUDP(unsigned long variableRef, const uint8_t slaveAddress, const uint8_t slaveIdx) {
	//Serial << F("unsigned long\n");
	setRemoteVariableOverUDP((void *)&variableRef, 4, slaveAddress, slaveIdx);
}	  
void SharedVariables::setRemoteVariableOverUDP(float variableRef, const uint8_t slaveAddress, const uint8_t slaveIdx) {
	//Serial << F("float\n");
	setRemoteVariableOverUDP((void *)&variableRef, 4, slaveAddress, slaveIdx);
}	  
void SharedVariables::setRemoteVariableOverUDP(double variableRef, const uint8_t slaveAddress, const uint8_t slaveIdx) {
	//Serial << F("double\n");
	setRemoteVariableOverUDP((void *)&variableRef, 4, slaveAddress, slaveIdx);
}	  
void SharedVariables::setRemoteVariableOverUDP(char variableRef, const uint8_t slaveAddress, const uint8_t slaveIdx) {
	//Serial << F("char\n");
	setRemoteVariableOverUDP((void *)&variableRef, 1, slaveAddress, slaveIdx);
}	  
void SharedVariables::setRemoteVariableOverUDP(const char * variableRef, uint8_t theSize, const uint8_t slaveAddress, const uint8_t slaveIdx) {
	//Serial << F("const char *\n");
	setRemoteVariableOverUDP((void *)variableRef, theSize, slaveAddress, slaveIdx);
}	  
void SharedVariables::setRemoteVariableOverUDP(const uint8_t * variableRef, uint8_t theSize, const uint8_t slaveAddress, const uint8_t slaveIdx) {
	//Serial << F("const uint8 *\n");
	setRemoteVariableOverUDP((void *)variableRef, theSize, slaveAddress, slaveIdx);
}	  
void SharedVariables::setRemoteVariableOverUDP(const int * variableRef, uint8_t theSize, const uint8_t slaveAddress, const uint8_t slaveIdx) {
	//Serial << F("const int *\n");
	setRemoteVariableOverUDP((void *)variableRef, theSize, slaveAddress, slaveIdx);
}	  

/**
 * Sets a remote variable on a UDP slave over UDP (void pointer)
 */
void SharedVariables::setRemoteVariableOverUDP(void * voidPointer, uint8_t dLen, const uint8_t slaveAddress, const uint8_t slaveIdx)	{

	if (_currentBundleAddr > 0 && _currentBundleAddr!=slaveAddress)	return;

	/*
	Serial << F("-- To slave ") << slaveAddress << F(" - idx ") << slaveIdx << F("\n");
	for(uint8_t i=0; i<dLen; i++)	{
		Serial << _HEXPADL((uint8_t)(((uint8_t *)voidPointer)[i]),2,"0") << F("\n");
	}
	Serial << F("--\n");
*/

	if (_currentBundleAddr > 0)	{
		if (!(*_messenger).fitCommand(dLen))	{	// If there is not enough space for this variable, just send the current buffer and reset it:
			(*_messenger).send();
		}
	} else {
		(*_messenger).resetTXBuffer();
	}

	for(uint8_t i=0; i<dLen; i++)	{
		(*_messenger).addValueToDataBuffer(((uint8_t *)voidPointer)[i], i);
	}

	(*_messenger).addCommand(slaveIdx | 0x80, dLen);

	if (_currentBundleAddr == 0)	{
		(*_messenger).send(slaveAddress, 8765);
	}
}

/**
 * Gets a remote variable on a UDP slave over UDP (delivered through call back function)
 */
void SharedVariables::getRemoteVariableOverUDP(const uint8_t slaveAddress, const uint8_t slaveIdx)	{

	if (_currentBundleAddr > 0 && _currentBundleAddr!=slaveAddress)	return;

	if (_currentBundleAddr == 0)	{
		(*_messenger).resetTXBuffer();
	}

	(*_messenger).addCommand(slaveIdx & 0x7F, 0);

	if (_currentBundleAddr == 0)	{
		(*_messenger).send(slaveAddress, 8765);
	}
}

int16_t SharedVariables::read_int(const uint8_t *dataArray)	{
	return *((int16_t *)dataArray);
}
bool SharedVariables::read_bool(const uint8_t *dataArray)	{
	return dataArray[0]?true:false;
}
uint8_t SharedVariables::read_uint8_t(const uint8_t *dataArray)	{
	return dataArray[0];
}
uint16_t SharedVariables::read_uint16_t(const uint8_t *dataArray)	{
	return *((uint16_t *)dataArray);
}
long SharedVariables::read_long(const uint8_t *dataArray)	{
	return *((long *)dataArray);
}
unsigned long SharedVariables::read_unsigned_long(const uint8_t *dataArray)	{
	return *((unsigned long *)dataArray);
}
float SharedVariables::read_float(const uint8_t *dataArray)	{
	return *((float *)dataArray);
}
char SharedVariables::read_char(const uint8_t *dataArray)	{
	return *((char *)dataArray);
}
void SharedVariables::read_string(const uint8_t *dataArray, char * variableRef, uint8_t theSize) {
	strncpy(variableRef, (char *)dataArray, theSize);
	variableRef[theSize-1] = 0;
}
void SharedVariables::read_uint8_array(const uint8_t *dataArray, uint8_t * variableRef, uint8_t theSize) {
	memcpy(variableRef, dataArray, theSize);
}
void SharedVariables::read_int_array(const uint8_t *dataArray, int * variableRef, uint8_t theSize) {
	memcpy(variableRef, (uint8_t *)dataArray, theSize);
}
	
	
	
	
	

void SharedVariables::startBundle(const uint8_t slaveAddress) {
	_currentBundleAddr = slaveAddress;
	(*_messenger).resetTXBuffer();
}
void SharedVariables::endBundle()	{
	(*_messenger).send(_currentBundleAddr, 8765);
	_currentBundleAddr = 0;
}






bool SharedVariables::attachLocalVariableToRemoteVariable(const uint8_t localIdx, const uint8_t slaveAddress, const uint8_t slaveIdx, bool push)	{
	if (_attachPointer<SharedVariables_MAX_ATTACHMENTS)	{
		_attachments[_attachPointer][0] = localIdx;
		_attachments[_attachPointer][1] = slaveAddress;
		_attachments[_attachPointer][2] = slaveIdx;
		_attachments[_attachPointer][3] = push;	 // 1 = output variable from this device to the slave, otherwise incoming value (we pull in that case)
		_attachPointer++;
		return true;
	} else return false;
}
void SharedVariables::UDPautoExchange(uint16_t period)	{
	static unsigned long timer = 0;
	if (hasTimedOut(timer, period))	{
		timer = millis();
//	Serial << F("UDPautoExchange:\n");
		bool positionsProcessed[SharedVariables_MAX_ATTACHMENTS];
		memset(positionsProcessed,0,SharedVariables_MAX_ATTACHMENTS);
		uint8_t currentAddress = 0;
		do	{
			currentAddress = 0;
			for(uint8_t i=0; i<_attachPointer; i++)	{
				if (!positionsProcessed[i] && !currentAddress)	{
					currentAddress = _attachments[i][1];
					startBundle(currentAddress);
				}
				if (!positionsProcessed[i] && currentAddress == _attachments[i][1])	{
					if (_attachments[i][3])	{	// Push value
						if (validIdx(_attachments[i][0]))	{
							setRemoteVariableOverUDP((void *)_varReferences[_attachments[i][0]], _varSize[_attachments[i][0]]*bytesPerElement(_varType[_attachments[i][0]] & 0xF), _attachments[i][1], _attachments[i][2]);
						}
					} else {	// Pull value
						getRemoteVariableOverUDP(_attachments[i][1], _attachments[i][2]);
					}
					positionsProcessed[i]=true;
				}
			}
			if (currentAddress)	{
				endBundle();
			}
		} while(currentAddress>0);
//		Serial << F("UDPautoExchange end\n");
	}
}
uint8_t SharedVariables::bytesPerElement(uint8_t type)	{
	switch(type)	{
		case 10:
		case 11:
			return 2;
		case 12:
			return 4;
		default:
			return 1;
	}
}


	  
/**
 * Returns true if idx is valid
 */
bool SharedVariables::validIdx(uint8_t idx)	{
	return (idx < _numberOfVars && _varType[idx] > 0);
}
	  
/**
 * Setting call-back function for when variables are changed from this class.
 */
void SharedVariables::setExternalChangeOfVariableCallback(void (*fptr)(uint8_t idx))	{
	_externalChangeOfVariableCallback = fptr;
}
void SharedVariables::setExternalReadResponseCallback(void (*fptr)(const uint8_t slaveIdx, const uint8_t slaveAddress, const uint8_t dataLength, const uint8_t *dataArray))	{
	_externalReadResponseCallback = fptr;
}


void SharedVariables::messengerObject(UDPmessenger &UDPmessengerObj)	{
	_messenger = &UDPmessengerObj;
}


/**
 * Timer-funtion. Input a timestamp (ms) and a timeout time (ms) and if the current time is "timeout time" longer than the input timestamp it returns true.
 */
bool SharedVariables::hasTimedOut(unsigned long time, unsigned long timeout)  {
  if ((unsigned long)(time + timeout) < (unsigned long)millis())  {  // This should "wrap around" if time+timout is larger than the size of unsigned-longs, right?
    return true; 
  } 
  else {
    return false;
  }
}