//	OSCBundle.h - Class representing an OSC Bundle.
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

#ifndef OSCBUNDLE_H_
#define OSCBUNDLE_H_

#include "OSCBase.h"
//#include "OSCMessage.h"
#include "OSCTimeTag.h"
#include <vector>

namespace OSC
{

class Message;

///	Class representing an OSC Bundle.
/*!
	Remember, bundles can be recursive, so a bundle may contain other bundles,
	as well as messages.
 */
class Bundle : public OscBase
{
public:
    ///	Default constructor.
    /*!
    	Typically used when you're constructing a bundle to send out.
     */
    Bundle();
    ///	Constructor used to create a bundle and it's contents from a block of char data.
    /*!
    	\param data A block of data which should contain an OSC bundle.
    	\param size The size of the data.

    	You'd typically use this when you've received a block of data, and want
    	to extract all the bundle data from it.  To be safe, make sure you check
    	the data is actually a bundle with the isBundle() static method before
    	you call this.
     */
    Bundle(char *data, const int size);
    ///	Destructor.
    /*!
    	All elements in the messageArray and bundleArray are deleted here.
     */
    ~Bundle();

    ///	Returns the total size of the message.
    /*!
    	You must call this beforeyou call getData(), or the memory for
    	getData()'s return buffer won't be allocated.
     */
    ///	Returns the bundle (including all it's elements) as a contiguous block of memory.
    Int32 getSize();
    char *getData();

    ///	Adds an element to the bundle's array of contents.
    /*!
    	The element may be either a Message or a Bundle.  The bundle (this
    	bundle) will call Message::isMessage() and Bundle::isBundle() on the
    	element's data to determine what kind of entry it is.  This is
    	necessary so we can easily manipulate the data with the various
    	getMessage() and getBundle() methods.

    	Once an element has been added to a Bundle, it is the Bundle's
    	responsibility to delete it, not the caller's.

    	Note that the order you add elements is not preserved for the final
    	bundle sent off via getData() - the final order will always be:
    	1st->last Messages, 1st->last Bundles.
     */
    void addElement(OscBase *entry);
    ///	Clears all elements from the bundle (deletes them).
    void clearElements();
    ///	Sets the time tag for this bundle.
    void setTimeTag(const TimeTag& val);
    ///	Returns the time tag for this bundle.
    TimeTag getTimeTag() const
    {
        return timeTag;
    };

    ///	Returns the number of elements in this bundle.
    long getNumElements() const;
    ///	Returns the number of Messages in this bundle.
    long getNumMessages() const
    {
        return messageArray.size();
    };
    ///	Returns the number of Bundles in this bundle.
    long getNumBundles() const
    {
        return bundleArray.size();
    };
    ///	Returns the indexed Message.
    /*!
    	\return 0 if we're outside the array's bounds.
     */
    Message *getMessage(const long index);
    ///	Returns the indexed Bundle.
    /*!
    	\return 0 if we're outside the array's bounds.
     */
    Bundle *getBundle(const long index);

    ///	Method to check whether a block of data is an OSC Message.
    /*!
    	Basically just checks for the presence of the initial '#bundle' string
    	that all OSC bundles should have as their first bytes.
     */
    static bool isBundle(char *data, const int size);

    juce_UseDebuggingNewOperator
private:
    ///	The TimeTag for this bundle.
    TimeTag timeTag;

    ///	Dynamic array of Messages.
    std::vector<Message *> messageArray;
    ///	Dynamic array of bundles.
    std::vector<Bundle *> bundleArray;

    ///	Current size of the data buffer we're holding.
    Int32 bufferSize;
    ///	Size of the data buffer when we're sending data out via getData().
    /*!
    	We keep track of two sizes because we don't want to keep allocating
    	memory - if we need to allocate a new buffer which is smaller than the
    	current one, we just set outgoingSize to the new size, and don't bother
    	re-allocating.
     */
    Int32 outgoingSize;
    ///	The data buffer we pass out when we're sending data out via getData().
    char *dataBuffer;
};

}

#endif
