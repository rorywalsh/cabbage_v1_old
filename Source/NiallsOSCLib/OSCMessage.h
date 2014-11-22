//	OSCMessage.h - Class representing an OSC Message.
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

#ifndef OSCMESSAGE_H_
#define OSCMESSAGE_H_

#include <vector>
#include <string>
#include "OSCBase.h"
#include "../CabbageUtils.h"

namespace OSC
{

///	Class representing an OSC message.
class Message : public OscBase
{
public:
    ///	Default constructor.
    /*!
    	Typically used when you're constructing a Message to send out.
     */
    Message();
    ///	Constructor to create a Message from a block of char data.
    /*!
    	\param data The data to construct the Message from.
    	\param size The size of the data.

    	You'll typically use this to construct a Message from a block of data
    	you've received.  To be safe, make sure you check the data is a Message
    	with the isMessage() static method first.
     */
    Message(char *data, const int size);
    ///	Destructor.
    ~Message();

    ///	Returns the total size of the message.
    /*!
    	You must call this before you call getData(), or the memory for
    	getData()'s return buffer won't be allocated.
     */
    Int32 getSize();
    ///	Returns the message as a contiguous block of memory.
    char *getData();

    ///	Clears the message data (erases the various vectors).
    void clearMessage();
    ///	Sets the message's destination address.
    void setAddress(const std::string& val);
    ///	Adds an Int32 value to the message.
    void addInt32(const Int32 val);
    ///	Adds an Float32 value to the message.
    void addFloat32(const Float32 val);
    ///	Adds an OscString value to the message.
    void addOscString(const std::string& val);

    ///	Returns the address of the Message.
    std::string getAddress() const
    {
        return address;
    };
    ///	Returns the type tag of the Message.
    std::string getTypeTag() const
    {
        return typeTag;
    };
    ///	Returns the number of floats in this Message.
    int getNumFloats() const
    {
        return floatArray.size();
    };
    ///	Returns the indexed float.
    Float32 getFloat(Int32 index) const
    {
        return floatArray[index];
    };
    ///	Returns the number of ints in this Message.
    int getNumInts() const
    {
        return intArray.size();
    };
    ///	Returns the indexed int.
    Int32 getInt(Int32 index) const
    {
        return intArray[index];
    };
    ///	Returns the number of strings in this Message.
    int getNumStrings() const
    {
        return stringArray.size();
    };
    ///	Returns the indexed string.
    std::string getString(Int32 index) const
    {
        return stringArray[index];
    };

    ///	Method to check whether a block of data is an OSC Message.
    /*!
    	This is a very simplistic check, and could be greatly improved.  For
    	now, there's a very good chance it'll generate false positives if given
    	packets which start with '/' and have a ',' somewhere in them.
     */
    static bool isMessage(char *data, const int size);

    juce_UseDebuggingNewOperator
private:
    ///	The destination address (OSC namespace) this message is going to.
    std::string address;
    ///	Our OSC type tag string.
    /*!
    	Stored as a (safer) std::string, gets converted to an OscString later.
     */
    std::string typeTag;

    ///	Our array of Int32s.
    std::vector<Int32> intArray;
    ///	Our array of Float32s.
    std::vector<Float32> floatArray;
    ///	Our array of OscStrings.
    /*!
    	(well, std::strings for safety - they'll get converted back in getData).
     */
    std::vector<std::string> stringArray;

    ///	The current size of our output buffer.
    Int32 bufferSize;
    ///	The size of the section of our buffer we're sending out.
    Int32 outgoingSize;
    ///	The buffer we copy everything into for getData().
    char *buffer;
};

}

#endif
