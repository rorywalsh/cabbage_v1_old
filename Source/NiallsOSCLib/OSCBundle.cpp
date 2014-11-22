//	OSCBundle.cpp - Class representing an OSC Bundle.
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
#include <winsock.h>
#else
#include <netinet/in.h>
#endif

#include "OSCBundle.h"
#include "OSCMessage.h"

using namespace std;

namespace OSC
{

//------------------------------------------------------------------------------
Bundle::Bundle():
    OscBase(),
    bufferSize(0),
    outgoingSize(0),
    dataBuffer(0)
{

}

//------------------------------------------------------------------------------
Bundle::Bundle(char *data, const int size):
    OscBase(),
    bufferSize(0),
    outgoingSize(0),
    dataBuffer(0)
{
    int i;
    Int32 tempSize;
    TimeTag *tempTag;
    Message *newMessage;
    Bundle *newBundle;

    //Start at 8, because we don't care about the first '#bundle' string.
    for(i=8; i<size; ++i)
    {
        if(i < 16)
        {
            //Get timeTag.
            tempTag = new TimeTag(data+i);
            timeTag = *tempTag;
            delete tempTag;
            i += 7;
        }
        else
        {
            tempSize = ntohl(*(reinterpret_cast<Int32 *>(data+i)));
            i += 4;

            if(isBundle((data+i), tempSize))
            {
                newBundle = new Bundle((data+i), tempSize);
                addElement(newBundle);
                i += (tempSize-1); //-1 because it gets incremented at the end of the loop.
            }
            else if(Message::isMessage((data+i), tempSize))
            {
                newMessage = new Message((data+i), tempSize);
                addElement(newMessage);
                i += (tempSize-1); //-1 because it gets incremented at the end of the loop.
            }
        }
    }
}

//------------------------------------------------------------------------------
Bundle::~Bundle()
{
    clearElements();

    if(dataBuffer)
    {
        delete [] dataBuffer;
        dataBuffer = 0;
    }
}

//------------------------------------------------------------------------------
Int32 Bundle::getSize()
{
    vector<Message *>::iterator itM;
    vector<Bundle *>::iterator itB;
    Int32 retval = 0;

    retval += 8; //'#bundle'
    retval += 8; //timeTag

    for(itM=messageArray.begin(); itM!=messageArray.end(); ++itM)
    {
        retval += 4; //For the size of the message.
        retval += (*itM)->getSize();
    }

    for(itB=bundleArray.begin(); itB!=bundleArray.end(); ++itB)
    {
        retval += 4; //For the size of the bundle.
        retval += (*itB)->getSize();
    }

    outgoingSize = retval;
    if(outgoingSize > bufferSize)
    {
        //This shouldn't ever happen, but just in case...
        if(outgoingSize < 16)
            outgoingSize = 16;

        bufferSize = outgoingSize;
        if(dataBuffer)
        {
            delete [] dataBuffer;
            dataBuffer = 0;
        }

        dataBuffer = new char[bufferSize];
    }

    return retval;
}

//------------------------------------------------------------------------------
char *Bundle::getData()
{
    Int32 i, j;
    Int32 tempInt;
    unsigned long tempL;
    GetTheBytes *tempBytes;
    char *tempBuffer;
    Int32 messageCount = 0;
    Int32 bundleCount = 0;

    if(!dataBuffer)
        return 0;

    for(i=0; i<outgoingSize; ++i)
    {
        //'#bundle'
        if(i < 8)
        {
            //dataBuffer = "#bundle";
            dataBuffer[0] = '#';
            dataBuffer[1] = 'b';
            dataBuffer[2] = 'u';
            dataBuffer[3] = 'n';
            dataBuffer[4] = 'd';
            dataBuffer[5] = 'l';
            dataBuffer[6] = 'e';
            dataBuffer[7] = static_cast<char>(NULL);
            i += 7;
        }
        //timeTag
        else if(i < 16)
        {
            tempL = timeTag.getSeconds();
            tempInt = htonl(tempL);
            tempBytes = reinterpret_cast<GetTheBytes *>(&tempInt);
            dataBuffer[i] = tempBytes->a;
            ++i;
            dataBuffer[i] = tempBytes->b;
            ++i;
            dataBuffer[i] = tempBytes->c;
            ++i;
            dataBuffer[i] = tempBytes->d;
            ++i;

            tempL = timeTag.getFraction();
            tempInt = htonl(tempL);
            tempBytes = reinterpret_cast<GetTheBytes *>(&tempInt);
            dataBuffer[i] = tempBytes->a;
            ++i;
            dataBuffer[i] = tempBytes->b;
            ++i;
            dataBuffer[i] = tempBytes->c;
            ++i;
            dataBuffer[i] = tempBytes->d;
        }
        //Do the messages first.
        else if(messageCount < static_cast<Int32>(messageArray.size()))
        {
            tempInt = htonl(messageArray[messageCount]->getSize());
            tempBytes = reinterpret_cast<GetTheBytes *>(&tempInt);
            dataBuffer[i] = tempBytes->a;
            ++i;
            dataBuffer[i] = tempBytes->b;
            ++i;
            dataBuffer[i] = tempBytes->c;
            ++i;
            dataBuffer[i] = tempBytes->d;
            ++i;

            tempBuffer = messageArray[messageCount]->getData();
            for(j=0; j<static_cast<Int32>(ntohl(tempInt)); ++j)
                dataBuffer[i+j] = tempBuffer[j];
            i += (j-1);

            ++messageCount;
        }
        //Now the bundles.
        else if(bundleCount < static_cast<Int32>(bundleArray.size()))
        {
            tempInt = htonl(bundleArray[bundleCount]->getSize());
            tempBytes = reinterpret_cast<GetTheBytes *>(&tempInt);
            dataBuffer[i] = tempBytes->a;
            ++i;
            dataBuffer[i] = tempBytes->b;
            ++i;
            dataBuffer[i] = tempBytes->c;
            ++i;
            dataBuffer[i] = tempBytes->d;
            ++i;

            tempBuffer = bundleArray[bundleCount]->getData();
            for(j=0; j<static_cast<Int32>(ntohl(tempInt)); ++j)
                dataBuffer[i+j] = tempBuffer[j];
            i += (j-1);

            ++bundleCount;
        }
    }

    return dataBuffer;
}

//------------------------------------------------------------------------------
void Bundle::addElement(OscBase *entry)
{
    Int32 size;

    size = entry->getSize();
    if(isBundle(entry->getData(), size))
        bundleArray.push_back(static_cast<Bundle *>(entry));
    else if(Message::isMessage(entry->getData(), size))
        messageArray.push_back(static_cast<Message *>(entry));
}

//------------------------------------------------------------------------------
void Bundle::clearElements()
{
    vector<Message *>::iterator itM;
    vector<Bundle *>::iterator itB;

    for(itM=messageArray.begin(); itM!=messageArray.end(); ++itM)
        delete (*itM);
    for(itB=bundleArray.begin(); itB!=bundleArray.end(); ++itB)
        delete (*itB);

    messageArray.clear();
    bundleArray.clear();
}

//------------------------------------------------------------------------------
void Bundle::setTimeTag(const TimeTag& val)
{
    timeTag = val;
}

//------------------------------------------------------------------------------
long Bundle::getNumElements() const
{
    return messageArray.size() + bundleArray.size();
}

//------------------------------------------------------------------------------
Message *Bundle::getMessage(const long index)
{
    Message *retval = 0;

    if(index < static_cast<Int32>(messageArray.size()))
        retval = messageArray[index];

    return retval;
}

//------------------------------------------------------------------------------
Bundle *Bundle::getBundle(const long index)
{
    Bundle *retval = 0;

    if(index < static_cast<Int32>(bundleArray.size()))
        retval = bundleArray[index];

    return retval;
}

//------------------------------------------------------------------------------
bool Bundle::isBundle(char *data, const int size)
{
    bool retval = false;

    if(size > 8)
    {
        //Should only need to check for the '#bundle' string...
        if((data[0] == '#') &&
                (data[1] == 'b') &&
                (data[2] == 'u') &&
                (data[3] == 'n') &&
                (data[4] == 'd') &&
                (data[5] == 'l') &&
                (data[6] == 'e'))
            retval = true;
    }

    return retval;
}

}
