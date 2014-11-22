//	OSCTimeTag.h - Class representing an OSC Time Tag, used in Bundles.
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

#ifndef OSCTIMETAG_H_
#define OSCTIMETAG_H_

#include <string>
#include "../JuceLibraryCode/JuceHeader.h"

namespace OSC
{

///	Class representing an OSC TimeTag, used by Bundle.
class TimeTag
{
  public:
	///	Copy constructor.
	/*!
		Necessary?
	 */
	TimeTag(const TimeTag& copy);
	///	Default Constructor.
	/*!
		Initialises the time to the 'immediately' - i.e. 0x0000000000000001.
	 */
	TimeTag();
	///	Constructor to initialise the time to a particular time.
	TimeTag(const unsigned long seconds, const unsigned long fraction);
	///	Constructor to initialise the time from a char block of 8 bytes.
	/*!
		Note: the data passed in must be at least 8 bytes long!

		This is most useful when you've just received a block of char data and
		need to determine it's time tag.
	 */
	TimeTag(char *data);

	///	Returns the seconds value for this time tag.
	unsigned long getSeconds() const {return seconds;};
	///	Returnes the fractional part for this time tag.
	unsigned long getFraction() const {return fraction;};
	///	Returns the time represented as a string.
	std::string getTimeAsText();

	///	Sets the seconds value for this time tag.
	void setSeconds(const unsigned long val);
	///	Sets the fractional value for this time tag.
	void setFraction(const unsigned long val);
	///	Sets the seconds and fractional values for this time tag.
	void setTime(const unsigned long seconds, const unsigned long fraction);

	///	Static method to create a new TimeTag with the current time.
	static TimeTag getCurrentTime();

	///	So we can test TimeTags against each other.
	bool operator==(const TimeTag& other);
	///	So we can test TimeTags against each other.
	bool operator!=(const TimeTag& other);
	///	So we can test TimeTags against each other.
	bool operator<(const TimeTag& other);
	///	So we can test TimeTags against each other.
	bool operator>(const TimeTag& other);
	///	So we can test TimeTags against each other.
	bool operator<=(const TimeTag& other);
	///	So we can test TimeTags against each other.
	bool operator>=(const TimeTag& other);

	///	So we can assign one TimeTag's values to another.
	TimeTag operator=(const TimeTag& other);

	juce_UseDebuggingNewOperator
  private:
	///	The seconds value for this time tag.
	unsigned long seconds;
	///	The fractional part of this time tag.
	unsigned long fraction;
};

}

#endif
