//	OSCMessage.cpp - Class representing an OSC Message.
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

#include"OSCMessage.h"

using namespace std;

namespace OSC
{

//------------------------------------------------------------------------------
Message::Message():
OscBase(),
bufferSize(0),
outgoingSize(0),
buffer(0)
{
	typeTag = ',';
}

//------------------------------------------------------------------------------
Message::Message(char *data, const int size):
OscBase(),
bufferSize(0),
outgoingSize(0),
buffer(0)
{
	int i;
	GetTheBytes tempBytes;
	Int32 tempint;
	Float32 *tempf;
	string tempstr;
	//This represents which part of the Message we're currently reading from.
	int currentSection = 0;
	int currentTag = 1; //Because Type Tags start with ','.

	for(i=0;i<size;++i)
	{
		//Address.
		if(currentSection == 0)
		{
			if((data[i] != 0) && (data[i] != ','))
				address += data[i];
			else if(data[i] == ',')
			{
				typeTag = ',';
				++currentSection;
			}
		}
		//Type Tag.
		else if(currentSection == 1)
		{
			if(data[i] != 0)
				typeTag += data[i];
			else if((i%4) == 3) //i.e. we're on the last byte of a 4 byte boundary.
				++currentSection;
		}
		//The actual data.
		else
		{
			if(currentTag >= static_cast<int>(typeTag.length()))
				break;
			else if(typeTag[currentTag] == 'f')
			{
				tempBytes.a = data[i];
				++i;
				tempBytes.b = data[i];
				++i;
				tempBytes.c = data[i];
				++i;
				tempBytes.d = data[i];

				tempint = ntohl(*(reinterpret_cast<Int32 *>(&tempBytes)));
				tempf = reinterpret_cast<Float32 *>(&tempint);
				floatArray.push_back(*tempf);

				++currentTag;
			}
			else if(typeTag[currentTag] == 'i')
			{
				tempBytes.a = data[i];
				++i;
				tempBytes.b = data[i];
				++i;
				tempBytes.c = data[i];
				++i;
				tempBytes.d = data[i];

				tempint = ntohl(*(reinterpret_cast<Int32 *>(&tempBytes)));
				intArray.push_back(tempint);

				++currentTag;
			}
			else if(typeTag[currentTag] == 's')
			{
				tempstr = "";
				while(data[i] != 0)
				{
					tempstr += data[i];

					++i;
				}
				tempstr[tempstr.length()] = static_cast<char>(NULL); //Terminator.
				stringArray.push_back(tempstr);

				//Handle any padding bytes.
				while((i%4) != 0)
					++i;

				++currentTag;
			}
		}
	}
}

//------------------------------------------------------------------------------
Message::~Message()
{
	if(buffer)
	{
		delete [] buffer;
		buffer = 0;
	}
}

//------------------------------------------------------------------------------
Int32 Message::getSize()
{
	Int32 i;
	Int32 tempint;

	outgoingSize = 0;
	if((address.length()+1)%4)
	{
		//Because we need space for the padding bytes.
		tempint = (4 - ((address.length()+1)%4));
		outgoingSize += address.length() + 1;
		outgoingSize += tempint;
	}
	else
		outgoingSize += address.length() + 1; //+1 = Null terminator.
	if((typeTag.length()+1)%4)
	{
		//Because we need space for the padding bytes.
		tempint = (4 - ((typeTag.length()+1)%4));
		outgoingSize += typeTag.length() + 1;
		outgoingSize += tempint;
	}
	else
		outgoingSize += typeTag.length() + 1; //+1 = Null terminator.
	outgoingSize += intArray.size() * sizeof(Int32);
	outgoingSize += floatArray.size() * sizeof(Float32);
	for(i=0;i<static_cast<Int32>(stringArray.size());++i)
	{
		if((stringArray[i].length()+1)%4)
		{
			//Because we need space for the padding bytes.
			tempint = (4 - ((stringArray[i].length()+1)%4));
			outgoingSize += stringArray[i].length() + 1;
			outgoingSize += tempint;
		}
		else
			outgoingSize += stringArray[i].length() + 1; //+1 = Null terminator.
	}

	if(outgoingSize > bufferSize)
	{
		bufferSize = outgoingSize;
		if(buffer)
		{
			delete [] buffer;
			buffer = 0;
		}

		buffer = new char[bufferSize];
	}

	return outgoingSize;
}

//------------------------------------------------------------------------------
char *Message::getData()
{
	Int32 addressCount = 0;
	Int32 typeTagCount = 0;
	Int32 intCount = 0;
	Int32 floatCount = 0;
	Int32 stringCount = 0;
	Int32 currentTypeTag = 1;
	Int32 i, j;
	GetTheBytes *tempBytes;
	Int32 tempInt;

	if(!buffer)
		return 0;

	for(i=0;i<outgoingSize;++i)
	{
		//If we're currently writing the address to the buffer.
		if(addressCount < static_cast<Int32>(address.length() + 1))
		{
			buffer[i] = address[addressCount];
			++addressCount;

			if(addressCount == static_cast<Int32>(address.length()))
			{
				++i;
				buffer[i] = static_cast<char>(NULL);
				++addressCount;
				//Add any packing bytes.
				while(addressCount%4)
				{
					++i;
					buffer[i] = static_cast<char>(NULL);
					++addressCount;
				}
			}
		}
		//If we're currently writing the type tag to the buffer.
		else if(typeTagCount < static_cast<Int32>(typeTag.length() + 1))
		{
			buffer[i] = typeTag[typeTagCount];
			++typeTagCount;

			if(typeTagCount == static_cast<Int32>(typeTag.length()))
			{
				++i;
				buffer[i] = static_cast<char>(NULL);
				++typeTagCount;
				//Add any packing bytes.
				while(typeTagCount%4)
				{
					++i;
					buffer[i] = static_cast<char>(NULL);
					++typeTagCount;
				}
			}
		}
		//If we're currently writing part of the data to the buffer.
		else if(currentTypeTag < static_cast<Int32>(typeTag.length()))
		{
			//Determine what kind of data we should be writing to the buffer.
			switch(typeTag[currentTypeTag])
			{
				//If we're writing an integer to the buffer.
				case 'i':
					tempInt = htonl(intArray[intCount]);
					tempBytes = reinterpret_cast<GetTheBytes *>(&tempInt);
					buffer[i] = tempBytes->a;
					++i;
					buffer[i] = tempBytes->b;
					++i;
					buffer[i] = tempBytes->c;
					++i;
					buffer[i] = tempBytes->d;

					++intCount;
					break;
				//If we're writing a float to the buffer.
				case 'f':
					tempInt = htonl(*(Int32 *)(&(floatArray[floatCount])));
					tempBytes = reinterpret_cast<GetTheBytes *>(&tempInt);
					buffer[i] = tempBytes->a;
					++i;
					buffer[i] = tempBytes->b;
					++i;
					buffer[i] = tempBytes->c;
					++i;
					buffer[i] = tempBytes->d;

					++floatCount;
					break;
				//If we're writing an OSC String to the buffer.
				case 's':
					for(j=1;
						j<static_cast<Int32>(stringArray[stringCount].length());
						++j)
					{
						buffer[i] = stringArray[stringCount][j-1];
						++i;
					}
					buffer[i] = static_cast<char>(NULL);

					//Add any packing bytes.
					while(j%4)
					{
						++i;
						buffer[i] = static_cast<char>(NULL);
						++j;
					}

					++stringCount;
					break;
			}
			//Move on to the next tag in the message's type tag list.
			++currentTypeTag;
		}
	}

	i = i;

	return buffer;
}

//------------------------------------------------------------------------------
void Message::clearMessage()
{
	outgoingSize = 0;

	typeTag = ",";
	intArray.clear();
	floatArray.clear();
	stringArray.clear();
}

//------------------------------------------------------------------------------
void Message::setAddress(const std::string& val)
{
	address = val;
}

//------------------------------------------------------------------------------
void Message::addInt32(const Int32 val)
{
	typeTag += 'i';
	intArray.push_back(val);
}

//------------------------------------------------------------------------------
void Message::addFloat32(const Float32 val)
{
	typeTag += 'f';
	floatArray.push_back(val);
}

//------------------------------------------------------------------------------
void Message::addOscString(const std::string& val)
{
	typeTag += 's';
	stringArray.push_back(val);
}

//------------------------------------------------------------------------------
bool Message::isMessage(char *data, const int size)
{
	int i;
	bool retval = false;

	//All OSC Messages start with the OSC address, which always starts with '/'.
	if(data[0] == '/')
	{
		for(i=1;i<size;++i)
		{
			//The comma marks the start of the Type Tag section of the message.
			//It'll always come at the start of a 4-byte boundary.
			if((data[i] == ',')&&((i%4) == 0))
			{
				retval = true;
				break;
			}
		}
	}

	return retval;
}

}
