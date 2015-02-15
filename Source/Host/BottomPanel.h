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

#ifndef __BOTTOM_PANEL_JUCEHEADER__
#define __BOTTOM_PANEL_JUCEHEADER__

#include "../JuceLibraryCode/JuceHeader.h"
#include "FilterGraph.h"
#include "../CabbageLookAndFeel.h"


//==============================================================================
class BottomPanel   : public Component,
					  public ActionListener
{
public:
    BottomPanel(FilterGraph* graph);
	~BottomPanel();
	void paint(Graphics &g);
	void actionListenerCallback (const String &message);
	void mouseDrag(const MouseEvent& event);
	void mouseEnter(const MouseEvent& event);
	
private:
	bool canResize;
	int startingYPos;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BottomPanel);
};

#endif