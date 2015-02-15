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

#ifndef __NativeParametersPanel_JUCEHEADER__
#define __NativeParametersPanel_JUCEHEADER__

#include "../JuceLibraryCode/JuceHeader.h"
#include "FilterGraph.h"
#include "../CabbageLookAndFeel.h"

#define TRANSPORT_CONTROLS 0
#define PLUGIN_PARAMS 1
#define FILE_BROWSER 2
#define WIDGET_PROPS 3


#define BUTTON_SIZE 30

class SidebarPanel;

//==============================================================================
// transport prop
//==============================================================================
class TransportComponent : public PropertyComponent,
							public Button::Listener,
							public Slider::Listener
{
public:
	TransportComponent(SidebarPanel &ownerPanel, String name);
	
	~TransportComponent()
	{
	}
	
	void resized();
	void refresh(){}
	void buttonClicked (Button* button);
	void sliderValueChanged (Slider* sliderThatWasMoved);
	void sliderDragEnded (Slider *);
	
	void setTimeLabel(String value){ 	timeLabel.setText(value, dontSendNotification);	}
	void setBeatsLabel(String value){ 	beatsLabel.setText(value, dontSendNotification);	}
	
	void setTimeIsRunning(bool value)
	{
		if(value)
			timeLabel.setColour(Label::textColourId, Colours::yellow);
		else
			timeLabel.setColour(Label::textColourId, Colours::cornflowerblue);
	}
	
private:
	Slider bpmSlider, timeSigNum, timeSigDen;
	DrawableButton playButton;
	DrawableButton stopButton;
	SidebarPanel &owner;
	Label bpmLabel;
	Label timeLabel;
	Label beatsLabel;
	CabbageLookAndFeelBasic lookAndFeel;
	DrawablePath timingInfoBox;
	ScopedPointer<LookAndFeel_V2> standardLookAndFeel;
};

//==============================================================================
// file browser comp
//==============================================================================
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
								public FileBrowserListener,
								public ChangeListener,
								public ActionListener
{
public:
    SidebarPanel(FilterGraph* graph);
	~SidebarPanel();
	

    void paint (Graphics& g) override;
    void resized() override;
    void timerCallback() override;
	void showParametersForNode(int nodeID=-1);
	void refreshPluginParameters();
	void updatePluginParameters();
	void updateWidgetProperties();
    void selectionChanged() override;
    void fileClicked (const File&, const MouseEvent&) override          {}
    void fileDoubleClicked (const File&) override                       {}
    void browserRootChanged (const File&) override                      {}
	void mouseDrag(const MouseEvent& event);
	void mouseEnter(const MouseEvent& event);
	void mouseUp(const MouseEvent& event);
	
	void upButtonPressed();	
	void toggleMIDILearn();
	void stopButtonPressed();
	void playButtonPressed();
	void pauseButtonPressed();
	void setCurrentBPM(int bpm);
	void changeListenerCallback (ChangeBroadcaster*);
	void actionListenerCallback (const String &message);
	
	
private:
    ConcertinaPanel concertinaPanel;
	BubbleMessageComponent midiBubble;
	DirectoryContentsList directoryList;
	FileTreePropertyComponent fileTreeComp;
	
	TransportComponent transportControls;
	TimeSliceThread thread;
	FilterGraph* filterGraph;
	int previousFilterNodeId;
    void addPluginPanel (PropertyPanel* panel);
	bool canResize;
	bool midiLearn;
	
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SidebarPanel);
};

#endif