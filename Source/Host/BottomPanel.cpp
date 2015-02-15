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
canResize(false)
{

	
}
//=================================================================
BottomPanel::~BottomPanel()
{	

}

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

void BottomPanel::paint(Graphics &g)
{
	g.fillAll(Colour::greyLevel (0.2f));
	g.setColour(Colour(30, 30, 30));
	g.fillRect(5, 5, getWidth()-10, getHeight()-10);	
}

//--------------------------------------------------------------------
void BottomPanel::mouseEnter(const MouseEvent& event)
{
	const int yPos = event.getPosition().getY();
	startingYPos = getPosition().getY();
	
	if(!event.mods.isLeftButtonDown() && yPos<10)
	{
		setMouseCursor (MouseCursor::UpDownResizeCursor);
		canResize=true;
	}
	else
		setMouseCursor (MouseCursor::NormalCursor);
}
//--------------------------------------------------------------------
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