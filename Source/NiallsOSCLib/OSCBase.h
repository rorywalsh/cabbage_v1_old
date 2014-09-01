//	OSCBase.h - Base class for any OSC data packets etc.
//------------------------------------------------------------------------------
//	This file is part of Niall's OSC Lib - a simple library providing the basic
//	operations necessary for communicating via Open Sound Control.
//	Copyright (c) 2005 Niall Moody.
//
//	This program is free software; you can redistribute it and/or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation; either version 2 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program; if not, write to the Free Software
//	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//------------------------------------------------------------------------------

#ifndef OSCBASE_H_
#define OSCBASE_H_

#include "../JuceLibraryCode/JuceHeader.h"

namespace OSC
{

//------------------------------------------------------------------------------
///	OSC's int32 type (this may be different on other machines).
typedef long Int32;
///	OSC's float32 type (this may be different on other machines).
typedef float Float32;
///	OSC's OSC-string type.
typedef char * OscString;
///	A padding byte.
typedef char PaddingByte;

//------------------------------------------------------------------------------
///	The base class for any OSC message/bundle packets.
class OscBase
{
  public:
	///	Constructor.
	OscBase() {};
	///	Destructor.
	virtual ~OscBase() {};

	///	Returns the total size of the OSC data to be sent.
	virtual Int32 getSize() = 0;
	///	Returns the data to be sent as a contiguous unsigned char array.
	virtual char *getData() = 0;

	juce_UseDebuggingNewOperator
};

///	Used to easily access the individual bytes in Int32 and Float32s.
typedef struct
{
	unsigned char a;
	unsigned char b;
	unsigned char c;
	unsigned char d;
} GetTheBytes;

}

#endif
