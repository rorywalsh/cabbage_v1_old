/*
  Copyright (c) 2015 - Rory Walsh

  Cabbage is free software; you can redistribute it
  and/or modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  Cabbage is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with Csound; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
  02111-1307 USA
*/

#include "BreakpointEnvelope.h"

BreakpointEnvelope::BreakpointEnvelope()
{

}

void BreakpointEnvelope::createGainEnvStartEndPoint()
{
	EnvelopHandle* leftMostHandle = new EnvelopHandle(this);
	addAndMakeVisible(leftMostHandle);
	leftMostHandle->setRelativePosition(Point<double>(0, 0), 1, 1);
	EnvelopHandle* rightMostHandle = new EnvelopHandle(this);
	addAndMakeVisible(rightMostHandle);
	rightMostHandle->setRelativePosition(Point<double>(1, 0), 1, 1);
	handles.add(leftMostHandle);
	handles.add(rightMostHandle);		
}

void BreakpointEnvelope::mouseDown(const MouseEvent& e)
{
	int indx;
	const int x = e.getPosition().getX();
	for (int i=1; i<handles.size(); i++)
	{
		cUtils::debug("handle0X", handles[i-1]->getX());
		cUtils::debug("handle1X", handles[i]->getX());
		cUtils::debug("mouseX", x);
		if (x >= handles[i-1]->getX() && x < handles[i]->getX())
		{
			indx = i;
		}
	}	
	
	EnvelopHandle* handle = new EnvelopHandle(this);
	addAndMakeVisible(handle);
	handle->setTopLeftPosition(e.getPosition().getX(), e.getPosition().getY());
	handle->setRelativePosition(e.getPosition().toDouble(), getWidth(), getHeight());
	handles.insert(indx, handle);
	repaint();
	resized();
}

void BreakpointEnvelope::addHandle(Point<double> pos, bool resize)
{
	EnvelopHandle* handle = new EnvelopHandle(this);
	addAndMakeVisible(handle);
	handle->setRelativePosition(Point<double>(pos.getX(), pos.getY()), 1.0, 1.0);
	handles.add(handle);
	repaint();
	if(resize)
		resized();	
} 

int BreakpointEnvelope::getHandleIndex(EnvelopHandle* thisHandle)
{
    return handles.indexOf(thisHandle);
}

void BreakpointEnvelope::removeHandle(EnvelopHandle* handle)
{
    if (handles.size() > 0)
    {
        handles.removeObject(handle, true);
    }	
	
	repaint();
	resized();
}
	
EnvelopHandle* BreakpointEnvelope::getLastHandle()
{
    return handles.getUnchecked(handles.size()-1);
}

EnvelopHandle* BreakpointEnvelope::getFirstHandle()
{
    return handles.getUnchecked(0);
}

EnvelopHandle* BreakpointEnvelope::getPreviousHandle(EnvelopHandle* thisHandle)
{
    int thisHandleIndex = handles.indexOf(thisHandle);

    if(thisHandleIndex <= 0)
        return 0;
    else
        return handles.getUnchecked(thisHandleIndex-1);
}

EnvelopHandle* BreakpointEnvelope::getNextHandle(EnvelopHandle* thisHandle)
{
    int thisHandleIndex = handles.indexOf(thisHandle);

    if(thisHandleIndex == -1 || thisHandleIndex >= handles.size()-1)
        return 0;
    else
        return handles.getUnchecked(thisHandleIndex+1);
}

Array<Point<double>> BreakpointEnvelope::getHandlePoints()
{
		Array<Point<double>> points;
		for(int i=0;i<handles.size();i++)
			points.add(handles[i]->getRelativePosition());
			
		return points;	
}

void BreakpointEnvelope::resized()
{
	//sendActionMessage instead....
	//getEditor()->getFilter()->clearEnvDataPoint();
	for(int i=0;i<handles.size();i++)
	{
		const double xPos = handles[i]->getRelativePosition().getX()*getWidth()-((i==0 || i==handles.size()-1) ? 4 : 0);
		const double yPos = handles[i]->getRelativePosition().getY()*getHeight();
		handles[i]->setTopLeftPosition(xPos, yPos);
		//sendActionMessage instead....
		//getEditor()->getFilter()->addEnvDataPoint(handles[i]->getRelativePosition());
	}
	sendChangeMessage();
}
 
void BreakpointEnvelope::paint(Graphics& g)
{			
	g.fillAll(Colours::transparentBlack);
	Path path;
	g.setColour(Colours::cornflowerblue);
	path.startNewSubPath(handles[0]->getPosition().translated(2.5, 2.5).toFloat());
	for(int i=0;i<handles.size()-1;i++)
	{
		path.lineTo(handles[i]->getPosition().translated(4, 4).toFloat());
	}
	path.lineTo(handles[handles.size()-1]->getPosition().translated(4, 4).toFloat());
	g.strokePath(path, PathStrokeType(2));
}

