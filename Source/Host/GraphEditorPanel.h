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

#ifndef __GRAPHEDITORPANEL_JUCEHEADER__
#define __GRAPHEDITORPANEL_JUCEHEADER__

#include "FilterGraph.h"
#include "MixerStrip.h"
#include "../Editor/CodeWindow.h"
#include "../CabbageLookAndFeel.h"
#include "FilterComponent.h"
#include "SidebarPanel.h"
#include "BottomPanel.h"


class GraphAudioProcessorPlayer;


//==============================================================================
//this get populated whenever we select multiple objects..
//==============================================================================
class SelectedComponents   : public SelectedItemSet<FilterComponent*>
{
public:
    void itemSelected (Component* item) {
        item->repaint ();
    }
    void itemDeselected (Component* item) {
        item->repaint ();
    }
};
//==============================================================================
//   A panel that displays and edits a FilterGraph.
//==============================================================================
class GraphEditorPanel   : public Component,
    public ChangeListener,
	public ActionListener,
    public LassoSource <FilterComponent*>,
	public DragAndDropTarget
{
public:
    GraphEditorPanel (FilterGraph& graph);
    ~GraphEditorPanel();

    void paint (Graphics& g);
    void mouseDown (const MouseEvent& e);
    void mouseDrag(const MouseEvent& e);
    void mouseUp(const MouseEvent& e);

    void createNewPlugin (const PluginDescription* desc, int x, int y, bool isNative, String fileName);

    FilterComponent* getComponentForFilter (uint32 filterID) const;
    ConnectorComponent* getComponentForConnection (const AudioProcessorGraph::Connection& conn) const;
    PinComponent* findPinAt (int x, int y) const;

    void resized();
    void changeListenerCallback (ChangeBroadcaster*);
	void actionListenerCallback (const String &message);
	void updateNode (const int nodeID, const int inChannels, const int outChannels);
	
	void enabledMIDILearn(bool val)
	{
		midiLearn=val;
	}

    bool isInterestedInDragSource (const SourceDetails &dragSourceDetails) override
    {
        return true;
    }

	
	void itemDropped (const DragAndDropTarget::SourceDetails& dragSourceDetails)
	{
		if(FileTreeComponent* fileComp = dynamic_cast<FileTreeComponent*>(dragSourceDetails.sourceComponent.get()))			
			cUtils::showMessage(fileComp->getSelectedFile().getFullPathName());	
	}
	
    void updateComponents();
    void findLassoItemsInArea (Array <FilterComponent*>& results, const Rectangle<int>& area);

    //==============================================================================
    void beginConnectorDrag (uint32 sourceFilterID, int sourceFilterChannel,
                             uint32 destFilterID, int destFilterChannel,
                             const MouseEvent& e);
    void dragConnector (const MouseEvent& e);
    void endDraggingConnector (const MouseEvent& e);
	String newFile(String type, String caption);
	WildcardFileFilter wildcardFilter;
	
    SelectedItemSet <FilterComponent*>& getLassoSelection() {
        return selectedFilters;
    }
    //array holding positions of selected filters
    Array<Point<int>> selectedFilterCoordinates;
	
	
    //==============================================================================
private:

    FilterGraph& graph;
	bool midiLearn;
    ScopedPointer<ConnectorComponent> draggingConnector;
    ComponentDragger myDragger;
    LassoComponent <FilterComponent*> lassoComp;
    SelectedComponents selectedFilters;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GraphEditorPanel)
};


//==============================================================================
//   This is the AudioProcessorPlayer that plays our grpah
//==============================================================================
class GraphAudioProcessorPlayer  :  public AudioProcessorPlayer,
									public ChangeListener,
								    public ChangeBroadcaster
								   
{
public:
	GraphAudioProcessorPlayer();
	~GraphAudioProcessorPlayer();

    void setProcessor (AudioProcessor* processorToPlay);
    AudioProcessor* getCurrentProcessor() const noexcept            { return processor; }
    MidiMessageCollector& getMidiMessageCollector() noexcept        { return messageCollector; }
    void audioDeviceIOCallback (const float**, int, float**, int, int) override;
    void audioDeviceAboutToStart (AudioIODevice*) override;
    void audioDeviceStopped() override;

	void changeListenerCallback (ChangeBroadcaster*);
	void actionListenerCallback (const String &message);
	
	Array<float> getOutputChannelRMS()
	{  
		return outputChannelRMS; 
	}

	Array<float> getInputChannelRMS()
	{
		return inputChannelRMS; 
	}
	
private:
    //==============================================================================
    AudioProcessor* processor;
    CriticalSection lock;
    double sampleRate;
    int blockSize;
    bool isPrepared;
	float inputGainLevel, outputGainLevel;

    int numInputChans, numOutputChans;
	Array<float> inputChannelRMS;
	Array<float> outputChannelRMS;
	int actionCounter;
    HeapBlock<float*> channels;
    AudioSampleBuffer tempBuffer;

    MidiBuffer incomingMidi;
    MidiMessageCollector messageCollector;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GraphAudioProcessorPlayer)
};

//==============================================================================
//    A lightweight component to handle scrolling of graph panel
//==============================================================================
class GraphPanelContainer : public Component
{
public:
	GraphPanelContainer(String name):Component(name)
	{

	}

	~GraphPanelContainer(){}

	void mouseWheelMove(const MouseEvent &event, const MouseWheelDetails &wheel)
	{
		const int posOffset = 100;
		const int sizeOffset = posOffset*2;
			
		if(wheel.deltaY>0)
		{
			getChildComponent(0)->setBounds(getChildComponent(0)->getPosition().x+posOffset, 
												getChildComponent(0)->getPosition().y+posOffset,
												getChildComponent(0)->getWidth()-sizeOffset, 
												getChildComponent(0)->getHeight()-sizeOffset);

		}
		else
		{ 
			getChildComponent(0)->setBounds(getChildComponent(0)->getPosition().x-posOffset, 
												getChildComponent(0)->getPosition().y-posOffset,
												getChildComponent(0)->getWidth()+sizeOffset, 
												getChildComponent(0)->getHeight()+sizeOffset);
		}
	}
};
//==============================================================================
//    A panel that embeds a GraphEditorPanel with a midi keyboard at the bottom.
//    It also manages the graph itself, and plays it.
//==============================================================================
class GraphDocumentComponent  : public Component,
								public MidiInputCallback
{
public:
    //==============================================================================
    GraphDocumentComponent (AudioPluginFormatManager& formatManager,
                            AudioDeviceManager* deviceManager);
    ~GraphDocumentComponent();

    //==============================================================================
    void createNewPlugin (const PluginDescription* desc, int x, int y, bool isNative, String filename);

    //==============================================================================
    FilterGraph graph;
	void handleIncomingMidiMessage (MidiInput*, const MidiMessage&) override;
	bool doMidiMappingsMatch(int i, int channel, int controller);
	
	void showSidebarPanel(bool show);
	
	bool isSidebarPanelShowing()
	{
		return sidebarPanel->isVisible();
	}
	
	void expandParametersInPluginsPanel(int nodeId=-1)
	{
		sidebarPanel->showParametersForNode(nodeId);
	}
	
	void refreshPluginsInSidebarPanel()
	{
		sidebarPanel->refreshPluginParameters();
	}
	
	void updatePluginsInSidebarPanel()
	{
		sidebarPanel->updatePluginParameters();
	}
	
	void toggleMIDILearn()
	{
		midiLearnEnabled=!midiLearnEnabled;
		sidebarPanel->toggleMIDILearn();
	}
	
    //==============================================================================
    void resized();

private:
    //==============================================================================
    AudioDeviceManager* deviceManager;
	GraphPanelContainer* graphPanelContainer;
    GraphAudioProcessorPlayer graphPlayer;
    MidiKeyboardState keyState;
	InternalMixerStrip* inputStrip;
	InternalMixerStrip* outputStrip; 
	SidebarPanel* sidebarPanel;
	BottomPanel* bottomPanel;
    GraphEditorPanel* graphPanel;
    Component* keyboardComp;
    Component* statusBar;
	bool midiLearnEnabled;
	bool addNewMapping;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GraphDocumentComponent)
};

//==============================================================================
// A desktop window containing a plugin's UI. 
//==============================================================================
class PluginWindow  : public DocumentWindow, public ActionBroadcaster, private Timer
{
public:
    enum WindowFormatType
    {
        Normal = 0,
        Generic,
        Programs,
        Parameters,
		midiLearn
    };

    PluginWindow (Component* pluginEditor, AudioProcessorGraph::Node*, WindowFormatType);
    ~PluginWindow();

    static PluginWindow* getWindowFor (AudioProcessorGraph::Node*, WindowFormatType);

    static void closeCurrentlyOpenWindowsFor (const uint32 nodeId);
    static void closeAllCurrentlyOpenWindows();
	static void updateWindow(AudioProcessorGraph::Node* node, int nodeId);

    void moved() override;
    void closeButtonPressed() override;

	void timerCallback();

private:
    AudioProcessorGraph::Node* owner;
	Component* editor;
    WindowFormatType type;
	ScopedPointer<CabbageLookAndFeelBasic> basicLookAndFeel;

    float getDesktopScaleFactor() const override     {
        return 1.0f;
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginWindow)
};

#endif   // __GRAPHEDITORPANEL_JUCEHEADER__
