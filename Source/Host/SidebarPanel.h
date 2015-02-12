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

class SidebarPanel;

class FileTreePropertyComponent : public PropertyComponent,
								  public Button::Listener
{
public:
	FileTreePropertyComponent(SidebarPanel &ownerPanel, String name, DirectoryContentsList &_listToShow);
	
	~FileTreePropertyComponent()
	{
	}
	
	
	void buttonClicked (Button* button);
	
	void setText(String text)
	{
		currentDir.setText(text, dontSendNotification);
	}
	
	void resized()
	{
		fileComp.setBounds(getLocalBounds().withTop(30));
		upButton.setBounds(getWidth()-40, 5, 40, 20); 
		currentDir.setBounds(5, 5, getWidth()-50, 20);
	}
	
	void refresh(){}
	
	void paint(Graphics& g)
	{
		g.fillAll(Colour(20, 20, 20));
	}
	
	FileTreeComponent fileComp;
	//DirectoryContentsList directory;
	
private:
	DrawableButton upButton;
	Label currentDir;
	ScopedPointer<LookAndFeel_V2> standardLookAndFeel;
	SidebarPanel &owner;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FileTreePropertyComponent);
	
};
//==============================================================================
class SidebarPanel   : public Component,
								private Timer,
								public FileBrowserListener
{
public:
    SidebarPanel(FilterGraph* graph);
	~SidebarPanel();
	

    void paint (Graphics& g) override;
    void resized() override;
    void timerCallback() override;
	void showParametersForNode(int nodeID=-1);
	void updatePluginParameters();
    void selectionChanged() override;
    void fileClicked (const File&, const MouseEvent&) override          {}
    void fileDoubleClicked (const File&) override                       {}
    void browserRootChanged (const File&) override                      {}
	void mouseDrag(const MouseEvent& event);
	void mouseEnter(const MouseEvent& event);
	void mouseUp(const MouseEvent& event);
	void upButtonPressed();
	
	
private:
    ConcertinaPanel concertinaPanel;
	DirectoryContentsList directoryList;
	FileTreePropertyComponent fileTreeComp;
	TimeSliceThread thread;
	FilterGraph* filterGraph;
	int previousFilterNodeId;
    void addPluginPanel (PropertyPanel* panel);
	bool canResize;
	
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SidebarPanel);
};

#endif