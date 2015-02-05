/*
  Copyright (c) 2013 - Raw Material Software Ltd, Rory Walsh

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

#ifndef __NativeParametersPanel_JUCEHEADER__
#define __NativeParametersPanel_JUCEHEADER__

#include "../JuceLibraryCode/JuceHeader.h"
#include "FilterGraph.h"


//==============================================================================
class NativeParametersPanel   : public Component,
								private Timer
{
public:
    NativeParametersPanel(FilterGraph* graph);

    void paint (Graphics& g) override;
    void resized() override;
    void timerCallback() override;
	
private:
    ConcertinaPanel concertinaPanel;
	ScopedPointer<FilterGraph> filterGraph;

    void addPanel (PropertyPanel* panel);
	
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NativeParametersPanel);
};

#endif