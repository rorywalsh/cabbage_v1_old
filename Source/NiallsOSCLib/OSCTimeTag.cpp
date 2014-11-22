//	OSCTimeTag.cpp - Class representing an OSC Time Tag, used in Bundles.
//	----------------------------------------------------------------------------
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
//	----------------------------------------------------------------------------

#ifdef WIN32
//Needed so we can use ntohl().
#include <winsock.h>
//Needed so we can use _ftime.
#include <sys/timeb.h>
#else
#include <netinet/in.h>
#endif

#include "OSCTimeTag.h"
#include <sstream>
#include <cmath>
#include <ctime>

using namespace std;

namespace OSC
{

//------------------------------------------------------------------------------
TimeTag::TimeTag(const TimeTag& copy)
{
    seconds = copy.seconds;
    fraction = copy.fraction;
}

//------------------------------------------------------------------------------
TimeTag::TimeTag()
{
    seconds = 0;
    fraction = 1;
}

//------------------------------------------------------------------------------
TimeTag::TimeTag(const unsigned long seconds, const unsigned long fraction)
{
    this->seconds = seconds;
    this->fraction = fraction;
}

//------------------------------------------------------------------------------
TimeTag::TimeTag(char *data)
{
    unsigned long *tempBytes;

    //Get the seconds.
    tempBytes = reinterpret_cast<unsigned long *>(data);
    seconds = ntohl(*tempBytes);

    //Get the fractional part.
    tempBytes = reinterpret_cast<unsigned long *>(data+4);
    fraction = ntohl(*tempBytes);
}

//------------------------------------------------------------------------------
string TimeTag::getTimeAsText()
{
    ostringstream out(ostringstream::out);

    //*********************************************************************
    //****TODO: Make sure fraction displays the correct number of zeros****
    //*********************************************************************
    out << seconds << "." << fraction;

    return out.str();
}

//------------------------------------------------------------------------------
void TimeTag::setSeconds(const unsigned long val)
{
    seconds = val;
}

//------------------------------------------------------------------------------
void TimeTag::setFraction(const unsigned long val)
{
    fraction = val;
}

//------------------------------------------------------------------------------
void TimeTag::setTime(const unsigned long seconds, const unsigned long fraction)
{
    this->seconds = seconds;
    this->fraction = fraction;
}

//------------------------------------------------------------------------------
TimeTag TimeTag::getCurrentTime()
{
    unsigned long retSeconds;
    unsigned long retFraction;

#ifdef WIN32
    _timeb currentTime;

    _ftime(&currentTime);

    //2208988800 = Num. seconds from 1900 to 1970, where _ftime starts from.
    retSeconds = 2208988800 + static_cast<unsigned long>(currentTime.time);
    //Correct for timezone.
    retSeconds -= static_cast<unsigned long>(60 * currentTime.timezone);
    //Correct for daylight savings time.
    if(currentTime.dstflag)
        retSeconds += static_cast<unsigned long>(3600);

    retFraction = static_cast<unsigned long>(currentTime.millitm);
    //Fill up all 32 bits...
    retFraction *= static_cast<unsigned long>(static_cast<float>(1<<32)/1000000.0f);
#else
    retSeconds = 0;
    retFraction = 0;
#endif

    return TimeTag(retSeconds, retFraction);
}

//------------------------------------------------------------------------------
bool TimeTag::operator==(const TimeTag& other)
{
    bool retval = false;

    if(seconds == other.seconds)
    {
        if(fraction == other.fraction)
            retval = true;
    }

    return retval;
}

//------------------------------------------------------------------------------
bool TimeTag::operator!=(const TimeTag& other)
{
    bool retval = false;

    if(seconds != other.seconds)
        retval = true;
    else if(fraction != other.fraction)
        retval = true;

    return retval;
}

//------------------------------------------------------------------------------
bool TimeTag::operator<(const TimeTag& other)
{
    bool retval = false;

    if(fraction < other.fraction)
    {
        if(seconds <= other.seconds)
            retval = true;
    }
    else if(seconds < other.seconds)
        retval = true;

    return retval;
}

//------------------------------------------------------------------------------
bool TimeTag::operator>(const TimeTag& other)
{
    bool retval = false;

    if(fraction > other.fraction)
    {
        if(seconds >= other.seconds)
            retval = true;
    }
    else if(seconds > other.seconds)
        retval = true;

    return retval;
}

//------------------------------------------------------------------------------
bool TimeTag::operator<=(const TimeTag& other)
{
    bool retval = false;

    if(fraction <= other.fraction)
    {
        if(seconds <= other.seconds)
            retval = true;
    }
    else if(seconds <= other.seconds)
        retval = true;

    return retval;
}

//------------------------------------------------------------------------------
bool TimeTag::operator>=(const TimeTag& other)
{
    bool retval = false;

    if(fraction > other.fraction)
    {
        if(seconds >= other.seconds)
            retval = true;
    }
    else if(seconds > other.seconds)
        retval = true;

    return retval;
}

//------------------------------------------------------------------------------
TimeTag TimeTag::operator=(const TimeTag& other)
{
    seconds = other.seconds;
    fraction = other.fraction;

    return *this;
}

}
