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
#include "../Plugin/CabbageGenericAudioProcessorEditor.h"


//==============================================================================
SidebarPanel::SidebarPanel(FilterGraph* graph) : 
filterGraph(graph),
previousFilterNodeId(-99),
thread ("file preview"),
directoryList (nullptr, thread),
fileTreeComp (*this, "", directoryList),
canResize(false)
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

	int index = filterGraph->getNumFilters()-1;
	
	
	//----------------------------------------------------
	PluginWrapper* wrapper = dynamic_cast<PluginWrapper*>(filterGraph->getNode(index)->getProcessor());
	if(wrapper)
	{
		int numParams = wrapper->getNumParameters();
		params.clear();
		for (int i = 0; i < numParams; ++i)
		{
			String name (wrapper->getParameterName (i));
			if (name.trim().isEmpty())
				name = "Unnamed";

			PluginProcessorParameterPropertyComp* const pc = new PluginProcessorParameterPropertyComp(name, *wrapper, i);
			params.add (pc);
		}

		panel->addSection (filterGraph->getNode(index)->getProcessor()->getName(), params, false);
	}

	//----------------------------------------------------
	CabbagePluginAudioProcessor* internalCabbage = dynamic_cast<CabbagePluginAudioProcessor*>(filterGraph->getNode(index)->getProcessor());
	if(internalCabbage)
	{
		int numParams = internalCabbage->getNumParameters();
		params.clear();
		for (int i = 0; i < numParams; ++i)
		{
			String name (internalCabbage->getParameterName (i));
			if (name.trim().isEmpty())
				name = "Unnamed";

			ProcessorParameterPropertyComp* const pc = new ProcessorParameterPropertyComp(name, *internalCabbage, i);
			params.add (pc);
		}

		panel->addSection (filterGraph->getNode(index)->getProcessor()->getName(), params, false);
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

void SidebarPanel::upButtonPressed()
{
	directoryList.setDirectory(directoryList.getDirectory().getParentDirectory(), true, true);
	fileTreeComp.setText(directoryList.getDirectory().getFileNameWithoutExtension());
}


void SidebarPanel::paint (Graphics& g)
{
	g.fillAll (Colour::greyLevel (0.2f));
}

void SidebarPanel::resized()
{
	fileTreeComp.setPreferredHeight(getLocalBounds().reduced(4).getHeight()-40);
	concertinaPanel.setBounds (getLocalBounds().reduced (4));
}

void SidebarPanel::timerCallback()
{

}

void SidebarPanel::mouseEnter(const MouseEvent& event)
{
	const int xPos = event.getPosition().getX();
	
	if(!event.mods.isLeftButtonDown() && xPos>getWidth()-10)
	{
		setMouseCursor (MouseCursor::LeftRightResizeCursor);
		canResize=true;
	}
	else
		setMouseCursor (MouseCursor::NormalCursor);
}

void SidebarPanel::mouseDrag(const MouseEvent& event)
{
	if(canResize)
		this->setSize(event.getPosition().getX(), this->getHeight());
}

void SidebarPanel::mouseUp(const MouseEvent& event)
{
	canResize = false;
}

void SidebarPanel::addPluginPanel (PropertyPanel* panel)
{
	concertinaPanel.addPanel(0, panel, false);
	concertinaPanel.setPanelHeaderSize (panel, 20);
	concertinaPanel.expandPanelFully(concertinaPanel.getPanel(0), true);
}

void SidebarPanel::selectionChanged()
{
	//cUtils::debug(fileTreeComp.fileComp.getSelectedFile().getFullPathName());
}

//==============================================================================
FileTreePropertyComponent::FileTreePropertyComponent(SidebarPanel &ownerPanel, String name, DirectoryContentsList &_listToShow):
PropertyComponent(name, 1000),
fileComp(_listToShow), 
upButton (String::empty, DrawableButton::ImageOnButtonBackground),
currentDir(""),
owner(ownerPanel),
standardLookAndFeel(new LookAndFeel_V2())
{

	addAndMakeVisible(fileComp);
	addAndMakeVisible(currentDir);
	currentDir.setLookAndFeel(standardLookAndFeel);
	
	currentDir.setColour(Label::backgroundColourId, Colours::white);
	
	fileComp.setDragAndDropDescription("FileBrowser");
	
	addAndMakeVisible (upButton);
	upButton.addListener (this);

	Path arrowPath;
	arrowPath.addArrow (Line<float> (50.0f, 100.0f, 50.0f, 0.0f), 40.0f, 100.0f, 50.0f);
	DrawablePath arrowImage;
	arrowImage.setFill (Colours::white);
	arrowImage.setPath (arrowPath);
	upButton.setImages (&arrowImage);
	fileComp.setColour (FileTreeComponent::backgroundColourId, Colours::black);	
	fileComp.setColour (TreeView::linesColourId, Colours::white); 
}

void FileTreePropertyComponent::buttonClicked (Button* button)
{
	owner.upButtonPressed();
}