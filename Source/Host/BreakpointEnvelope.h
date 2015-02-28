/*
  Copyright (c) 2014 - Rory Walsh

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

#ifndef __ENVELOPE_H_266C4137__
#define __ENVELOPE_H_266C4137__


#include "../JuceLibraryCode/JuceHeader.h"
#include "../CabbageUtils.h"

class EnvelopHandle;

class BreakpointEnvelope : public Component,
						   public ChangeBroadcaster
{			
public:
	BreakpointEnvelope(Colour col);
	
	~BreakpointEnvelope()
	{
		handles.clear();
	}
	
	void resized();		
	void paint (Graphics& g);
	void mouseDown(const MouseEvent& e);
	EnvelopHandle* getPreviousHandle(EnvelopHandle* thisHandle);
	int getHandleIndex(EnvelopHandle* thisHandle);
	void removeHandle(EnvelopHandle* handle);
	EnvelopHandle* getLastHandle();
	EnvelopHandle* getFirstHandle();
	EnvelopHandle* getNextHandle(EnvelopHandle* thisHandle);
	void createEnvStartEndPoint(int amp = 1);
	void showBubble(EnvelopHandle* handle);
	void addHandle(Point<double> pos, bool resize=true);
	Array<Point<double>> getHandlePoints();
	
	
private:
	OwnedArray<EnvelopHandle> handles;
	BubbleMessageComponent popupBubble;
	Colour colour;
	
};


//handle class
class EnvelopHandle : public Component
{
public:
	EnvelopHandle(BreakpointEnvelope* env, Colour col):
	owner(env),
	compY(0),
	compX(0),
	shape(1)
	{
	setSize(8, 8);	
	}
	
	void paint (Graphics& g)
	{
		g.fillAll(Colours::transparentBlack);
		g.setColour(Colours::white);
		g.fillEllipse(0, 0, 8, 8);
	}

	void setRelativePosition(Point<double> pos, double width, double height)
	{
		//convert position so that it's scaled between 0 and 1
		xPosRelative = jlimit(0.0, 1.0, pos.getX()/width);
		yPosRelative = jlimit(0.0, 1.0, pos.getY()/height);
	} 
	
	Point<double> getRelativePosition()
	{
		return Point<double>(xPosRelative, yPosRelative);
	} 

	void mouseDown(const MouseEvent &e)
	{
		if(e.mods.isPopupMenu() && (this!=owner->getFirstHandle() || this!=owner->getLastHandle()))
		{
			PopupMenu pop;
			pop.addItem(99, "Delete");
			
			if(pop.show()==99)
			{
				owner->removeHandle(this);
			}
		}
		
		compX = getX();
		compY = getY();
		setMouseCursor (MouseCursor::DraggingHandCursor);
		dragger.startDraggingComponent (this, e);				
		constrainer.setMinimumWidth(200);
	}

	void mouseEnter(const MouseEvent &e)
	{
		constrainer.setMinimumOnscreenAmounts(owner->getHeight(), 
											  owner->getWidth(), 
											  owner->getHeight(), 
											  1);
		setMouseCursor (MouseCursor::DraggingHandCursor);	
		owner->showBubble(this);
	}

	void mouseDrag(const MouseEvent &e)
	{
		
		//when a handle is dragged, we update its position and send a message
		//to Cabbage to update the Csound function table(CabbagePluginEditor.cpp)
		const EnvelopHandle* previousHandle = owner->getPreviousHandle(this);
		const EnvelopHandle* nextHandle = owner->getNextHandle(this);
		setMouseCursor (MouseCursor::DraggingHandCursor);
		double xPos = compX+e.getDistanceFromDragStartX();
		double yPos = jlimit(-4.0, owner->getHeight()-4.0, compY+e.getDistanceFromDragStartY());		
		
		
		if(this==owner->getFirstHandle() || this==owner->getLastHandle())
		{			
			setRelativePosition(Point<double>((this==owner->getLastHandle() ? owner->getWidth() : 0), yPos), owner->getWidth(), owner->getHeight());
			dragger.dragComponent (this, e.withNewPosition(Point<float>(compX, e.getPosition().getY())), &constrainer);
		}
		else
		{
			if((xPos>(previousHandle->getX()-4)) && (xPos<nextHandle->getX()))
			{
				setRelativePosition(Point<double>(xPos, yPos), owner->getWidth(), owner->getHeight());
				dragger.dragComponent (this, e, &constrainer);
			}
		}
		
		owner->showBubble(this);
		
		this->setSize(8, 8);
		owner->repaint();
	}
	
	void mouseUp(const MouseEvent &e)
	{
		owner->resized();
		owner->repaint();
	}
	
	double xPosRelative, yPosRelative, compX, compY;
	ComponentDragger dragger;
	Colour colour;
	int shape;
	int uid;
	ComponentBoundsConstrainer constrainer;
	BreakpointEnvelope* owner;

};
#endif
