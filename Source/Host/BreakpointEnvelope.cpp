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

BreakpointEnvelope::BreakpointEnvelope(Colour col, int Id):
colour(col),
uid(Id),
popupBubble(500)
{
    popupBubble.setColour(BubbleComponent::backgroundColourId, Colours::white);
	popupBubble.setAllowedPlacement(BubbleComponent::above | BubbleComponent::below);
    popupBubble.setBounds(0, 0, 50, 20);
	popupBubble.setAlwaysOnTop(true);
    addChildComponent(popupBubble);
	setSize(600, 300);
}

void BreakpointEnvelope::createEnvStartEndPoint(int amp)
{
	EnvelopHandle* leftMostHandle = new EnvelopHandle(this, colour);
	addAndMakeVisible(leftMostHandle);
	double yOffset = 8.f/(double)getHeight();
	leftMostHandle->setRelativePosition(Point<double>(0, 1-amp-yOffset), 1, 1);
	EnvelopHandle* rightMostHandle = new EnvelopHandle(this, colour);
	addAndMakeVisible(rightMostHandle);
	rightMostHandle->setRelativePosition(Point<double>(1, 1-amp-yOffset), 1, 1);
	handles.add(leftMostHandle);
	handles.add(rightMostHandle);		
	//resized();
}


void BreakpointEnvelope::showBubble(EnvelopHandle* handle)
{
//	float x = cUtils::roundToPrec(handle->getRelativePosition().getX(), 2)*100.f;
//	float y = (1.f-cUtils::roundToPrec(handle->getRelativePosition().getY(), 2))*100.f;
//	String message = String(x)+"%, "+String(y)+"%";
//	popupBubble.showAt(handle, AttributedString(message), 1050); 	
}

//==============================================================================
float BreakpointEnvelope::ampToPixel(int height, Range<float> minMax, float sampleVal)
{
    //caluclate amp value based on pixel...
    float amp =  (sampleVal-minMax.getStart()) / minMax.getLength();
    return jmax(0.f, ((1-amp) * height));
}

float BreakpointEnvelope::pixelToAmp(int height, Range<float> minMax, float pixelY)
{
    //caluclate pixel value based on amp...
    float amp =  ((1-(pixelY/height))*minMax.getLength())+minMax.getStart();
    return amp;
}

Array<double> BreakpointEnvelope::getEnvelopeAsPfields()
{
    Array<double> values;
    double prevXPos=0, currXPos=0, currYPos=0;
	int genRoutine = 7;
	Range<float> minMax;
	minMax.setStart(0);
	minMax.setEnd(1);
    
	for(int i=0; i<handles.size(); i++)
    {
        currYPos = handles[i]->getRelativePosition().getY()*getHeight();
        if(genRoutine==7)
        {
            currXPos = handles[i]->getRelativePosition().getX()*4096;

            //add x position
            values.add(jmax(0.0, ceil(currXPos-prevXPos)));
            //hack to prevent csound from bawking with a 0 in gen05
            float amp = pixelToAmp(getHeight(), minMax, currYPos);
            if(genRoutine==5)
                amp = jmax(0.001f, amp);
            else
                amp = jmax(0.f, amp);
            //add y position
            values.add(amp);
            prevXPos = roundToIntAccurate(handles[i]->getRelativePosition().getX()*4096);
        }
    }

    return values;
}

void BreakpointEnvelope::mouseDown(const MouseEvent& e)
{
	int indx;
	const int x = e.getPosition().getX();
	for (int i=1; i<handles.size(); i++)
	{
//		cUtils::debug("handle0X", handles[i-1]->getX());
//		cUtils::debug("handle1X", handles[i]->getX());
//		cUtils::debug("mouseX", x);
		if (x >= handles[i-1]->getX() && x < handles[i]->getX())
		{
			indx = i;
		}
	}	
	
	EnvelopHandle* handle = new EnvelopHandle(this, colour);
	addAndMakeVisible(handle);
	handle->setTopLeftPosition(e.getPosition().getX(), e.getPosition().getY());
	handle->setRelativePosition(e.getPosition().toDouble(), getWidth(), getHeight());
	handles.insert(indx, handle);
	repaint();
	resized();
}

void BreakpointEnvelope::addHandle(Point<double> pos, bool resize)
{
	EnvelopHandle* handle = new EnvelopHandle(this, colour);
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
		handles[i]->setSize(8, 8);
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
	g.setColour(colour);
	path.startNewSubPath(handles[0]->getPosition().translated(2.5, 2.5).toFloat());
	for(int i=0;i<handles.size()-1;i++)
	{
		path.lineTo(handles[i]->getPosition().translated(4, 4).toFloat());
	}
	path.lineTo(handles[handles.size()-1]->getPosition().translated(4, 4).toFloat());
	g.strokePath(path, PathStrokeType(2));
}

