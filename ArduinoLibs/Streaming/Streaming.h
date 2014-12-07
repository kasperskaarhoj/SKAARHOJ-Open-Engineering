/*
Streaming.h - Arduino library for supporting the << streaming operator
Copyright (c) 2010-2012 Mikal Hart.  All rights reserved.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef ARDUINO_STREAMING
#define ARDUINO_STREAMING

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define STREAMING_LIBRARY_VERSION 5

// Generic template
template<class T> 
inline Print &operator <<(Print &stream, T arg) 
{ stream.print(arg); return stream; }

struct _STRPAD 
{ 
  const char* val;
  uint8_t pad;
  const char* padChar;
  bool right;
  _STRPAD(const char* v, uint8_t p, const char* pc, bool r): val(v), pad(p), padChar(pc), right(r) 
  {}
};

struct _BASED 
{ 
  long val; 
  int base;
  _BASED(long v, int b): val(v), base(b) 
  {}
};

struct _BASEDPADL 
{ 
  long val; 
  uint8_t pad;
  const char* padChar;
  int base;
  _BASEDPADL(long v, uint8_t p, const char* pc, int b): val(v), pad(p), padChar(pc), base(b) 
  {}
};

#if ARDUINO >= 100

struct _BYTE_CODE
{
	byte val;
	_BYTE_CODE(byte v) : val(v)
	{}
};
#define _BYTE(a)    _BYTE_CODE(a)

inline Print &operator <<(Print &obj, const _BYTE_CODE &arg)
{ obj.write(arg.val); return obj; } 

#else

#define _BYTE(a)    _BASED(a, BYTE)

#endif

#define _HEX(a)     _BASED(a, HEX)
#define _DEC(a)     _BASED(a, DEC)
#define _OCT(a)     _BASED(a, OCT)
#define _BIN(a)     _BASED(a, BIN)
#define _HEXPADL(a,pad,padChar)     _BASEDPADL(a, pad, padChar, HEX)
#define _DECPADL(a,pad,padChar)     _BASEDPADL(a, pad, padChar, DEC)
#define _BINPADL(a,pad,padChar)     _BASEDPADL(a, pad, padChar, BIN)
#define _STRPADL(a,pad,padChar)     _STRPAD(a, pad, padChar, false)
#define _STRPADR(a,pad,padChar)     _STRPAD(a, pad, padChar, true)

// Specialization for class _BASED
// Thanks to Arduino forum user Ben Combee who suggested this 
// clever technique to allow for expressions like
//   Serial << _HEX(a);

inline Print &operator <<(Print &obj, const _STRPAD &arg)
{ 
	if (!arg.right)	{
		for(uint8_t a=0; a<arg.pad-strlen(arg.val);a++)	{
			obj.print(arg.padChar[0]);
		}
	}
	obj.print(arg.val); 
	if (arg.right)	{
		for(uint8_t a=0; a<arg.pad-strlen(arg.val);a++)	{
			obj.print(arg.padChar[0]);
		}
	}

	return obj; 
} 

inline Print &operator <<(Print &obj, const _BASED &arg)
{ obj.print(arg.val, arg.base); return obj; } 

inline Print &operator <<(Print &obj, const _BASEDPADL &arg) {
	if (arg.pad>1)	{
		int i;
		if (arg.base==HEX)	{
			for(i=arg.pad-1; i>0; i--)	{
				if (!(arg.val >> (i<<2)))	{
					obj.print(arg.padChar[0]);
				} else break;
			}
		}
		if (arg.base==BIN)	{
			for(i=arg.pad-1; i>0; i--)	{
				if (!(arg.val >> i))	{
					obj.print(arg.padChar[0]);
				} else break;
			}
		}
		if (arg.base==DEC)	{
			bool isPadding = false;
			long cmpVal;
			if (arg.val>=0)	{
				cmpVal = 10;
				for(i=arg.pad-1; i>0; i--)	{
					if (isPadding || arg.val < cmpVal)	{
						obj.print(arg.padChar[0]);
						isPadding = true;
					} else {
						cmpVal*=10;
					}
				}
			} else {
				cmpVal = -10;
				for(i=arg.pad-2; i>0; i--)	{
					if (isPadding || arg.val > cmpVal)	{
						obj.print(arg.padChar[0]);
						isPadding = true;
					} else {
						cmpVal*=10;
					}
				}
			}
		}
	}
	obj.print(arg.val, arg.base); 
	return obj; 
} 


#if ARDUINO >= 18
// Specialization for class _FLOAT
// Thanks to Michael Margolis for suggesting a way
// to accommodate Arduino 0018's floating point precision
// feature like this:
//   Serial << _FLOAT(gps_latitude, 6); // 6 digits of precision

struct _FLOAT
{
  float val;
  int digits;
  _FLOAT(double v, int d): val(v), digits(d)
  {}
};

inline Print &operator <<(Print &obj, const _FLOAT &arg)
{ obj.print(arg.val, arg.digits); return obj; }
#endif

// Specialization for enum _EndLineCode
// Thanks to Arduino forum user Paul V. who suggested this
// clever technique to allow for expressions like
//   Serial << "Hello!" << endl;

enum _EndLineCode { endl };

inline Print &operator <<(Print &obj, _EndLineCode arg) 
{ obj.println(); return obj; }

#endif
