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
#include "../Plugin/CabbageGenericAudioProcessorEditor.h"
#include "../CabbageGUIClass.h"


//==============================================================================
BottomPanel::BottomPanel(FilterGraph* graph)
{

	
}
//=================================================================
BottomPanel::~BottomPanel()
{	

}

void BottomPanel::paint(Graphics &g)
{
	g.fillAll(Colour::greyLevel (0.2f));
	g.setColour(Colour(30, 30, 30));
	g.fillRect(5, 5, getWidth()-10, getHeight()-10);	
}
