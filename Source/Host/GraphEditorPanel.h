/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2013 - Raw Material Software Ltd.

   Permission is granted to use this software under the terms of either:
   a) the GPL v2 (or any later version)
   b) the Affero GPL v3

   Details of these licenses can be found at: www.gnu.org/licenses

   JUCE is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

   ------------------------------------------------------------------------------

   To release a closed-source product which uses JUCE, commercial licenses are
   available: visit www.juce.com for more information.

  ==============================================================================
*/

#ifndef __GRAPHEDITORPANEL_JUCEHEADER__
#define __GRAPHEDITORPANEL_JUCEHEADER__

#include "FilterGraph.h"

class GraphEditorPanel;
class ConnectorComponent;
class PinComponent;
#define INTERNAL 1
#define CABBAGE 2
#define THIRDPARTY 3

//======================================================================
// Filter Component, GUI component that represents a processing node
//======================================================================
class FilterComponent    : public Component, 
						   public ActionListener
{
public:
    FilterComponent (FilterGraph& graph_, const uint32 filterID_);
	~FilterComponent();

    FilterGraph& graph;
    const uint32 filterID;
    int numInputs, numOutputs;
	void mouseDown (const MouseEvent& e);
	void mouseDrag (const MouseEvent& e);
	void mouseUp (const MouseEvent& e);
	bool hitTest (int x, int y);
	void actionListenerCallback (const String &message);
	void paint (Graphics& g);
	void getPinPos (const int index, const bool isInput, float& x, float& y);
	void update();
	void drawLevelMeter (Graphics& g, float x, float y, int width, int height, float level);
	void drawMuteIcon(Graphics& g, Rectangle<float> rect, bool state);
	void drawBypassIcon(Graphics& g, Rectangle<float> rect, bool isActive);
	

private:
	int pluginType;
    int pinSize;
    Colour filterColour;
    bool filterIsPartofSelectedGroup;
    Point<int> originalPos;
	float rmsLeft, rmsRight;
	void resized();
    Font font;
    int numIns, numOuts;
    DropShadowEffect shadow;
	bool isMuted, isBypassed;
	Rectangle<float> muteButton;
	Rectangle<float> bypassButton;

    GraphEditorPanel* getGraphPanel() const noexcept
    {
        return findParentComponentOfClass<GraphEditorPanel>();
    }

    FilterComponent (const FilterComponent&);
    FilterComponent& operator= (const FilterComponent&);
};

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
    public LassoSource <FilterComponent*>
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
    void updateComponents();
    void findLassoItemsInArea (Array <FilterComponent*>& results, const Rectangle<int>& area);

    //==============================================================================
    void beginConnectorDrag (uint32 sourceFilterID, int sourceFilterChannel,
                             uint32 destFilterID, int destFilterChannel,
                             const MouseEvent& e);
    void dragConnector (const MouseEvent& e);
    void endDraggingConnector (const MouseEvent& e);
    SelectedItemSet <FilterComponent*>& getLassoSelection() {
        return selectedFilters;
    }
    //array holding positions of selected filters
    Array<Point<int>> selectedFilterCoordinates;
	
    //==============================================================================
private:

    FilterGraph& graph;
    ScopedPointer<ConnectorComponent> draggingConnector;
    ComponentDragger myDragger;
    LassoComponent <FilterComponent*> lassoComp;
    SelectedComponents selectedFilters;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GraphEditorPanel)
};


//==============================================================================
//    A panel that embeds a GraphEditorPanel with a midi keyboard at the bottom.
//    It also manages the graph itself, and plays it.
//==============================================================================
class GraphDocumentComponent  : public Component
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

    //==============================================================================
    void resized();

private:
    //==============================================================================
    AudioDeviceManager* deviceManager;
    AudioProcessorPlayer graphPlayer;
    MidiKeyboardState keyState;

    GraphEditorPanel* graphPanel;
    Component* keyboardComp;
    Component* statusBar;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GraphDocumentComponent)
};

//==============================================================================
// A desktop window containing a plugin's UI. 
//==============================================================================
class PluginWindow  : public DocumentWindow
{
public:
    enum WindowFormatType
    {
        Normal = 0,
        Generic,
        Programs,
        Parameters
    };

    PluginWindow (Component* pluginEditor, AudioProcessorGraph::Node*, WindowFormatType);
    ~PluginWindow();

    static PluginWindow* getWindowFor (AudioProcessorGraph::Node*, WindowFormatType);

    static void closeCurrentlyOpenWindowsFor (const uint32 nodeId);
    static void closeAllCurrentlyOpenWindows();

    void moved() override;
    void closeButtonPressed() override;


private:
    AudioProcessorGraph::Node* owner;
    WindowFormatType type;

    float getDesktopScaleFactor() const override     {
        return 1.0f;
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginWindow)
};

#endif   // __GRAPHEDITORPANEL_JUCEHEADER__
