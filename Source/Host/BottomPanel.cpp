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
bottomBorder("bottomBorder"),
topBorder("topBorder")
{
//    listBox.setModel (&listBoxModel);
//    listBox.setMultipleSelectionEnabled (false);
//    listBox.setColour(ListBox::ColourIds::backgroundColourId, Colour(30, 30, 30));
//    addAndMakeVisible (listBox);
	listBoxModel.addActionListener(this);
//	listBox.selectRow(0);
	
	addAndMakeVisible(&bottomBorder);
	addAndMakeVisible(&topBorder);
	topBorder.addMouseListener(this, true);
	topBorder.setAlwaysOnTop(true);
	bottomBorder.setAlwaysOnTop(true);
	
	
	addAndMakeVisible(&viewport);
	viewport.setViewedComponent(&container, true);
	viewport.setScrollBarsShown(false, false);
	
}
//=================================================================
BottomPanel::~BottomPanel()
{	

}
//=================================================================
void BottomPanel::resized()
{
	//listBox.setBounds(5, 10, 110, getHeight()-40);
	const int numChildren = container.getNumChildComponents();
	
	viewport.setBounds(0, 5, getWidth()-5, getHeight()-10);
	bottomBorder.setBounds(0, getHeight()-5, getWidth(), 5);
	topBorder.setBounds(0, 0, getWidth(), 5);
	
	if(container.getHeight()>viewport.getHeight())
	{
		viewport.setScrollBarsShown(true, false);
		container.setBounds(0, 0, getWidth()-24, numChildren*190);
	}
	else
		container.setBounds(0, 0, getWidth()-5, numChildren*190);
		
	//if(viewport.isHorizontalScrollBarShown())
	
	for(int i=0;i<numChildren;i++)
	{
			container.getChildComponent(i)->setBounds(0, i*190, container.getWidth(), 190);
	}	
	
}
//=================================================================
void BottomPanel::addComponentToPanel(Component* comp)
{
	if(container.getIndexOfChildComponent(comp)==-1)
	{
		//comp->setBounds(0, 0, viewport.getWidth(), 190);
		container.addAndMakeVisible(comp);

		const int numChildren = container.getNumChildComponents();
		container.setSize(viewport.getWidth(), numChildren*190);
		
		//cUtils::showMessage(viewport.getWidth());		
		
		for(int i=0;i<numChildren;i++)
		{
			container.getChildComponent(i)->setBounds(0, i*190, viewport.getWidth(), 190);
		}

	}
	resized();
}

void BottomPanel::removeAllComponentsFromPanel()
{
	this->removeAllChildren();
	setVisible(false);
}

void BottomPanel::showComponentInPanel(String compName)
{
	int numChildren = container.getNumChildComponents();

	for(int i=0;i<numChildren;i++)
	{
		if(container.getChildComponent(i)->getComponentID()==compName)
		{
			indexOfCompToScrollTo = i;
			//listBox.selectRow(i);	
			//startTimer(10);
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
//			listBoxModel.removeRow(compName);
//			listBox.updateContent();
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
		indexOfCompToScrollTo=0;
//		listBox.selectRow(0);	
//		startTimer(10);
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

//====================== ListBox Component ===================================
// when a user clicks on an item in the listbox, the CabbagePreferencesComp will update
// what the users see
BottomPanel::ListboxContents::ListboxContents()
{
	contents.clear();	
}

void BottomPanel::ListboxContents::addRow(String rowName)
{
	contents.add(rowName);
}

void BottomPanel::ListboxContents::removeRow(String rowName)
{
	contents.removeString(rowName);
}

int BottomPanel::ListboxContents::getNumRows()
{
	return contents.size();
}

void BottomPanel::ListboxContents::paintListBoxItem (int rowNumber, Graphics& g,
					   int width, int height, bool rowIsSelected)
{
	if (rowIsSelected)
		g.fillAll (Colours::cornflowerblue);

	g.setColour (rowIsSelected ? Colours::black : Colours::green);
	g.setFont (cUtils::getComponentFont());

	g.drawText (contents[rowNumber],
				5, 0, width, height,
				Justification::centredLeft, true);
}

void BottomPanel::ListboxContents::listBoxItemClicked(int row, const MouseEvent &)
{
	sendActionMessage("ListBox:"+String(row));
}		
