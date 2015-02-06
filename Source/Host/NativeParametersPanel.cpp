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

#include "../JuceLibraryCode/JuceHeader.h"
#include "FilterGraph.h"
#include "NativeParametersPanel.h"
#include "../Plugin/PluginGenericAudioProcessorEditor.h"


//==============================================================================
NativeParametersPanel::NativeParametersPanel(FilterGraph* graph) : filterGraph(graph)
{
	setOpaque (true);
	addAndMakeVisible (concertinaPanel);
	
	PropertyPanel* panel = new PropertyPanel ("Plugins");

	Array <PropertyComponent*> params;

	for(int i=0;i<graph->getNumFilters();i++)
	{	
		PluginWrapper* plugin = dynamic_cast<PluginWrapper*>(graph->getNode(i)->getProcessor());
		
		if(plugin)
		{
			int numParams = plugin->getNumParameters();
			params.clear();
			for (int i = 0; i < numParams; ++i)
			{
				String name (plugin->getParameterName (i));
				if (name.trim().isEmpty())
					name = "Unnamed";

				PluginProcessorParameterPropertyComp* const pc = new PluginProcessorParameterPropertyComp (name, *plugin, i);
				params.add (pc);
			}
			panel->addSection (graph->getNode(i)->getProcessor()->getName(), params, true);
		}
	}
	//panel->addSection ("Section 1", createSliders (4), true);
	//panel->addSection ("Section 2", createSliders (3), true);
	addPanel (panel);

	startTimer (300);
}

void NativeParametersPanel::update()
{
	
	for(int i=0;i<concertinaPanel.getNumPanels();i++)
		concertinaPanel.removePanel(concertinaPanel.getPanel(i));
	
	PropertyPanel* panel = new PropertyPanel ("Plugins");

	Array <PropertyComponent*> params;

	for(int i=0;i<filterGraph->getNumFilters();i++)
	{	
		PluginWrapper* plugin = dynamic_cast<PluginWrapper*>(filterGraph->getNode(i)->getProcessor());
		
		if(plugin)
		{
			int numParams = plugin->getNumParameters();
			params.clear();
			for (int i = 0; i < numParams; ++i)
			{
				String name (plugin->getParameterName (i));
				if (name.trim().isEmpty())
					name = "Unnamed";

				PluginProcessorParameterPropertyComp* const pc = new PluginProcessorParameterPropertyComp (name, *plugin, i);
				params.add (pc);
			}
			panel->addSection (filterGraph->getNode(i)->getProcessor()->getName(), params, true);
		}
	}
	//panel->addSection ("Section 1", createSliders (4), true);
	//panel->addSection ("Section 2", createSliders (3), true);
	addPanel (panel);
}


void NativeParametersPanel::paint (Graphics& g)
{
	g.fillAll (Colour::greyLevel (0.2f));
}

void NativeParametersPanel::resized()
{
	concertinaPanel.setBounds (getLocalBounds().reduced (4));
}

void NativeParametersPanel::timerCallback()
{
	stopTimer();
	concertinaPanel.expandPanelFully (concertinaPanel.getPanel(0), true);
}

void NativeParametersPanel::addPanel (PropertyPanel* panel)
{
	concertinaPanel.addPanel (-1, panel, true);
	concertinaPanel.setMaximumPanelSize (panel, panel->getTotalContentHeight());
}
