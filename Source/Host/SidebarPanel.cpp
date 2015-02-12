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
#include "SidebarPanel.h"
#include "../Plugin/PluginGenericAudioProcessorEditor.h"


//==============================================================================
SidebarPanel::SidebarPanel(FilterGraph* graph) : 
filterGraph(graph),
previousFilterNodeId(-99),
thread ("file preview"),
directoryList (nullptr, thread),
fileTreeComp ("", directoryList, 400)
{
	setOpaque (true);
	addAndMakeVisible (concertinaPanel);	
	
	directoryList.setDirectory (File::getSpecialLocation (File::userHomeDirectory), true, true);
	thread.startThread (3);

	fileTreeComp.setText(File::getSpecialLocation (File::userHomeDirectory).getFileNameWithoutExtension());

	fileTreeComp.fileComp.addListener (this);	
	
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


	addPluginPanel (panel);

	
	PropertyPanel* filePanel = new PropertyPanel ("File Browser");
	Array <PropertyComponent*> singleParams;
	singleParams.add(&fileTreeComp);
	filePanel->addProperties(singleParams);	
	
	concertinaPanel.addPanel(1, filePanel, false);
	
	
}

SidebarPanel::~SidebarPanel()
{	
	fileTreeComp.fileComp.removeListener(this);
}

void SidebarPanel::updatePluginParameters()
{	
	PropertyPanel* panel = (PropertyPanel*)concertinaPanel.getPanel(0);

	Array <PropertyComponent*> params;
	
	const int numPanels = concertinaPanel.getNumPanels();

	int i = filterGraph->getNumFilters()-1;
	
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

			panel->addSection (filterGraph->getNode(i)->getProcessor()->getName(), params, false);
		}
	}

	for(int y=0;y<panel->getSectionNames().size()-1;y++)
		panel->setSectionOpen(y, false, true);
		
	panel->setSectionOpen(panel->getSectionNames().size()-1, true, true);

	concertinaPanel.expandPanelFully(concertinaPanel.getPanel(0), true);

}


void SidebarPanel::showParametersForNode(int nodeId)
{	
	const int numPanels = concertinaPanel.getNumPanels();
	int indexOfNode = 0;
	int pluginFilters=0;

		//filterGraph->getNumFilters() returns all filters including internal ones..
		for(int i=0;i<filterGraph->getNumFilters();i++)
		{	
			if(filterGraph->getNode(i)->properties.getWithDefault("pluginType", "")!="Internal")
			{
				if(filterGraph->getNode(i)->nodeId==nodeId)
				{
					indexOfNode=pluginFilters;
					previousFilterNodeId=nodeId;
				}			
	
			pluginFilters++;
			
			}
		}
	
	if(PropertyPanel* propPanel = (PropertyPanel*)concertinaPanel.getPanel(0))
	{
		for(int i=0;i<propPanel->getSectionNames().size();i++)
			propPanel->setSectionOpen(i, false, false);
					
		propPanel->setSectionOpen(indexOfNode, true, true);
		
	}
			
}


void SidebarPanel::paint (Graphics& g)
{
	g.fillAll (Colour::greyLevel (0.2f));
}

void SidebarPanel::resized()
{
	concertinaPanel.setBounds (getLocalBounds().reduced (4));
}

void SidebarPanel::timerCallback()
{

}

void SidebarPanel::addPluginPanel (PropertyPanel* panel)
{
	concertinaPanel.addPanel(0, panel, false);
	//concertinaPanel.setMaximumPanelSize (panel, panel->getTotalContentHeight());
	concertinaPanel.expandPanelFully(concertinaPanel.getPanel(0), true);
}

void SidebarPanel::selectionChanged()
{
	//cUtils::debug(fileTreeComp.fileComp.getSelectedFile().getFullPathName());
}
