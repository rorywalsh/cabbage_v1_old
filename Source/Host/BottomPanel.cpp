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

#include "../JuceLibraryCode/JuceHeader.h"
#include "FilterGraph.h"
#include "BottomPanel.h"
#include "GraphEditorPanel.h"
#include "../Plugin/CabbageGenericAudioProcessorEditor.h"
#include "../CabbageGUIClass.h"


//==============================================================================
BottomPanel::BottomPanel(FilterGraph* graph):
canResize(false),
viewport(),
container(),
numberOfComponents(0),
indexOfCompToScrollTo(0),
indexOfCurrentComp(0),
animateIndex(10),
topBorder("topBorder")
{
	addAndMakeVisible(&topBorder);
	this->setName("bottomPanel");
    setOpaque(true);
	topBorder.addMouseListener(this, true);
	topBorder.setAlwaysOnTop(true);
	addAndMakeVisible(&viewport);
	viewport.setViewedComponent(&container, true);
	viewport.setScrollBarsShown(true, false);
	
}
//=================================================================
BottomPanel::~BottomPanel()
{	

}
//=================================================================
void BottomPanel::resized()
{
	const int numChildren = container.getNumChildComponents();	
	viewport.setBounds(0, 5, getWidth()-5, getHeight()-10);
	topBorder.setBounds(0, 0, getWidth(), 5);

	cUtils::debug(getWidth(), getHeight());	
	if(container.getHeight()>viewport.getHeight())
	{
		viewport.setScrollBarsShown(true, false);
		container.setBounds(0, 0, getWidth()-24, numChildren*190);
	}
	else
		container.setBounds(0, 0, getWidth()-5, numChildren*190);
	
	for(int i=0;i<numChildren;i++)
	{
		container.getChildComponent(i)->setBounds(0, i*190, container.getWidth(), 190);
	}	
	
}
//=================================================================
void BottomPanel::addComponentToPanel(Component* comp)
{
	int numChildren=0;
	if(container.getIndexOfChildComponent(comp)==-1)
	{
		container.addAndMakeVisible(comp);
		numChildren = container.getNumChildComponents();	
		
		container.setSize(container.getWidth(), numChildren*190);	
		
		for(int i=0;i<numChildren;i++)
		{
			container.getChildComponent(i)->setBounds(0, i*190, viewport.getWidth(), 190);
		}

	}
	resized();

	viewport.setViewPosition(0, (numChildren-1)*190);
}

void BottomPanel::removeAllComponentsFromPanel()
{
	container.removeAllChildren();
	setVisible(false);
}

void BottomPanel::showComponentInPanel(String compName)
{
	int numChildren = container.getNumChildComponents();

	for(int i=0;i<numChildren;i++)
	{
		if(container.getChildComponent(i)->getComponentID()==compName)
		{
			viewport.setViewPosition(0, i*190);	
		}
	}	

}

void BottomPanel::removeComponentFromPanel(String compName)
{
	int numChildren = container.getNumChildComponents();

	for(int i=0;i<numChildren;i++)
	{
		if(container.getChildComponent(i)->getComponentID()==compName)
		{
			container.removeChildComponent(i);
			i=numChildren+1;
		}
	}		
	
	//when we remove one we need to rearrange the remaining ones...
	numChildren = container.getNumChildComponents();
	for(int i=0;i<numChildren;i++)
	{
		container.getChildComponent(i)->setBounds(0, i*190, container.getWidth(), 190);
	}
	
	if(container.getNumChildComponents()>0)
	{
		viewport.setViewPosition(0, 0);	
	}
	else
		this->setVisible(false);
		
		
		
}
//=================================================================
void BottomPanel::actionListenerCallback (const String &message)
{
	if(message.contains("SidebarWidth:"))
	{
		GraphDocumentComponent* parent =findParentComponentOfClass<GraphDocumentComponent>();
		if(parent)
		{
			String subStr = message.substring(message.indexOf(":")+1);
			setBounds(subStr.getIntValue(), getPosition().getY(), parent->getWidth()-subStr.getIntValue(), getHeight());		
			parent->getKeyboardComponent()->setBounds(subStr.getIntValue(), parent->getKeyboardComponent()->getPosition().getY(), parent->getWidth()-subStr.getIntValue()-200, parent->getKeyboardComponent()->getHeight());
			//parent->getKeyboardComponent()->setLowestVisibleKey(40);
		}		
	}
}
//=================================================================
void BottomPanel::timerCallback()
{
	
	//cUtils::debug("yPosOfCurrentComp", currentYPos);
	//cUtils::debug("indexOfCompToScrollTo", indexOfCompToScrollTo*190);
	
	const float move = (indexOfCurrentComp-indexOfCompToScrollTo)*190;
	const float finalPosition = -indexOfCompToScrollTo*190;
	float moveFrom = (indexOfCurrentComp*190)*(animateIndex/10.f);
	
	container.setTopLeftPosition(container.getPosition().getX(), finalPosition+5);
			
	stopTimer();	
	resized();
			
	animateIndex--;
	
	if(animateIndex<0)
	{
		animateIndex=10;
		stopTimer();
		indexOfCurrentComp = indexOfCompToScrollTo;
	}
}

//=================================================================
void BottomPanel::paint(Graphics &g)
{
	g.fillAll(Colour::greyLevel (0.2f));
	g.setColour(Colour(30, 30, 30));
	g.fillRect(0, 5, getWidth()-5, getHeight()-10);	
}
//=================================================================
void BottomPanel::mouseEnter(const MouseEvent& event)
{
	const int yPos = event.getPosition().getY();
	startingYPos = getPosition().getY();
	cUtils::debug(event.eventComponent->getName());
	if(event.eventComponent->getName()=="topBorder")
	{
		cUtils::debug("Should show cursor");
		event.eventComponent->setMouseCursor (MouseCursor::UpDownResizeCursor);
		canResize=true;
	}
	else
		setMouseCursor (MouseCursor::NormalCursor);
}
//=================================================================
void BottomPanel::mouseDrag(const MouseEvent& event)
{
	const int keysHeight = 60;
	if(canResize)
	{
		GraphDocumentComponent* parent =findParentComponentOfClass<GraphDocumentComponent>();
		if(parent)
		{
		//cUtils::debug(parent->getHeight()-(startingYPos+event.getDistanceFromDragStartY()));
			const int newHeight = parent->getHeight()-(startingYPos+event.getDistanceFromDragStartY())-keysHeight;
			setBounds(getPosition().getX(), startingYPos+event.getDistanceFromDragStartY(), getWidth(), newHeight);
		}
	}
}
