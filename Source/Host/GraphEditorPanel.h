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
class GraphDocumentComponent;

//==============================================================================
//this get populated whenever we select multiple objects..
//==============================================================================
class SelectedComponents   : public SelectedItemSet<FilterComponent*>
{
public:
    void itemSelected (Component* item)
    {
        item->repaint ();
    }
    void itemDeselected (Component* item)
    {
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

    GraphDocumentComponent* getGraphDocument();

    void enabledMIDILearn(bool val)
    {
        midiLearn=val;
    }

    bool isInterestedInDragSource (const SourceDetails &dragSourceDetails) override
    {
        return true;
    }


    void itemDropped (const DragAndDropTarget::SourceDetails& dragSourceDetails);
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

    SelectedItemSet <FilterComponent*>& getLassoSelection()
    {
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
    AudioProcessor* getCurrentProcessor() const noexcept
    {
        return processor;
    }
    MidiMessageCollector& getMidiMessageCollector() noexcept
    {
        return messageCollector;
    }
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

    void suspendProcessing(bool suspend)
    {
        processor->suspendProcessing(suspend);
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

    ~GraphPanelContainer() {}

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

class MidiMappingsComponent    : public Component,
    public TableListBoxModel
{
public:
    MidiMappingsComponent(XmlElement* xml)   : font (14.0f)
    {

        rowData = xml;//->getChildByName ("MIDIMAPPINGS");
        numRows = rowData->getNumChildElements();
        // Create our table component and add it to this component..
        addAndMakeVisible (table);
        table.setModel (this);

        // give it a border
        table.setColour (ListBox::outlineColourId, Colours::grey);
        table.setOutlineThickness (1);

        table.getHeader().addColumn("Channel", 1,
                                    60,
                                    50, 400,
                                    TableHeaderComponent::defaultFlags);

        table.getHeader().addColumn("Controller", 2,
                                    60,
                                    50, 400,
                                    TableHeaderComponent::defaultFlags);

        table.getHeader().addColumn("Plugin (NodeId)", 3,
                                    140,
                                    50, 400,
                                    TableHeaderComponent::defaultFlags);

        table.getHeader().addColumn("Parameter (Index)", 4,
                                    140,
                                    50, 400,
                                    TableHeaderComponent::defaultFlags);

    }

    // This is overloaded from TableListBoxModel, and must return the total number of rows in our table
    int getNumRows() override
    {
        return numRows;
    }

    // This is overloaded from TableListBoxModel, and must paint any cells that aren't using custom
    // components.
    void paintCell (Graphics& g, int rowNumber, int columnId,
                    int width, int height, bool /*rowIsSelected*/) override
    {
        g.setColour (Colours::black);
        g.setFont (font);
        g.setColour(Colours::whitesmoke);

        const XmlElement* rowElement = rowData->getChildElement (rowNumber);

        if (rowElement != 0)
        {
            const String text (rowElement->getStringAttribute (getAttributeNameForColumnId (columnId)));

            g.drawText (text, 2, 0, width - 4, height, Justification::centredLeft, true);
        }

        g.setColour (Colours::black.withAlpha (0.2f));
        g.fillRect (width - 1, 0, 1, height);
    }

    String getAttributeNameForColumnId (const int columnId) const
    {
        if(columnId==1)
            return "Channel";
        else if(columnId==2)
            return "Controller";
        else if(columnId==3)
            return "NodeId";
        else if(columnId==4)
            return "ParameterIndex";
    }

    void paintRowBackground (Graphics& g, int rowNumber, int width, int height, bool rowIsSelected)
    {
        g.fillAll (cUtils::getBackgroundSkin());
        g.setFont (font);
        //g.setColour(Colours::white);
        //g.drawText (rowData[rowNumber], 2, 0, width - 4, height, Justification::centredLeft, true);

        if (rowIsSelected)
            g.fillAll (Colours::lime.withAlpha(.4f));
    }

    // This is overloaded from TableListBoxModel, and must update any custom components that we're using
    Component* refreshComponentForCell (int rowNumber, int columnId, bool /*isRowSelected*/,
                                        Component* existingComponentToUpdate) override
    {

        if(columnId==1 || columnId==2)
        {
            EditableTextCustomComponent* textLabel = (EditableTextCustomComponent*) existingComponentToUpdate;

            // same as above...
            if (textLabel == 0)
                textLabel = new EditableTextCustomComponent (*this);

            textLabel->setRowAndColumn (rowNumber, columnId);

            return textLabel;
        }
        else
            return 0;
    }

    String getText (const int columnNumber, const int rowNumber) const
    {
        //cUtils::showMessage(rowData->getStringAttribute (getAttributeNameForColumnId (columnNumber)));
        return rowData->getChildElement (rowNumber)->getStringAttribute ( getAttributeNameForColumnId(columnNumber));

        //return "";
    }

    void setText (const int columnNumber, const int rowNumber, const String& newText)
    {
        const String& columnName = getAttributeNameForColumnId(columnNumber);//table.getHeader().getColumnName (columnNumber);
        rowData->getChildElement (rowNumber)->setAttribute (columnName, newText);
    }

    void removeRow(const int rowNumber)
    {
        rowData->removeChildElement(rowData->getChildElement (rowNumber), true);
        numRows = rowData->getNumChildElements();
    }

    XmlElement* getMidiMapXml()
    {

        return rowData;
    }

    //==============================================================================
    void resized() override
    {
        // position our table with a gap around its edge
        table.setBoundsInset (BorderSize<int> (8));
    }

    //determine actions to take place when users click on a cell...
    void cellClicked (int rowNumber, int columnId, const MouseEvent &e)
    {

    }

private:
    TableListBox table;     // the table component itself
    Font font;
    int numRows;            // The number of rows of data we've got
    XmlElement* rowData;

    //=========================================================
    // editable text box for tweaking midi parameters
    //=========================================================
    class EditableTextCustomComponent : public Label
    {
    public:
        EditableTextCustomComponent (MidiMappingsComponent& owner_)
            : owner (owner_)
        {
            // double click to edit the label text; single click handled below
            setEditable (false, true, false);
            setColour (textColourId, Colours::white);
        }

        void mouseDown (const MouseEvent& e) override
        {
            // single click on the label should simply select the row
            owner.table.selectRowsBasedOnModifierKeys (row, e.mods, false);

            if(e.mods.isRightButtonDown())
            {
                PopupMenu m;
                CabbageLookAndFeel cLAK;
                m.setLookAndFeel(&cLAK);
                m.addItem(1, "Delete selected");
                int c = m.show();
                if(c==1)
                {
                    owner.removeRow(row);
                    owner.table.updateContent();

                }
            }

            Label::mouseDown (e);
        }

        void textWasEdited() override
        {
            owner.setText (columnId, row, getText());
        }

        // Our demo code will call this when we may need to update our contents
        void setRowAndColumn (const int newRow, const int newColumn)
        {
            row = newRow;
            columnId = newColumn;
            setText (owner.getText(columnId, row), dontSendNotification);
        }

    private:
        MidiMappingsComponent& owner;
        int row, columnId;
    };
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
    void showMidiMappings();

    void showSidebarPanel(bool show);
    void showBottomPanel(bool show);

    bool isSidebarPanelShowing()
    {
        return sidebarPanel->isVisible();
    }

    bool isBottomPanelShowing()
    {
        return bottomPanel->isVisible();
    }

    void addComponentToBottomPanel(Component* component);
    void removeComponentFromBottomPanel(String component);
    void showComponentInBottomPanel(String component);
    void removeAllComponentsFromBottomPanel();

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

    void disableWidetPropertiesInSidebarPanel()
    {
        sidebarPanel->disablePropertiesPanel();
    }

    void toggleMIDILearn()
    {
        midiLearnEnabled=!midiLearnEnabled;
        sidebarPanel->toggleMIDILearn();
    }

    MidiKeyboardComponent* getKeyboardComponent()
    {
        return keyboardComp;
    }

    void suspendProcessing(bool suspend)
    {
        graphPlayer.suspendProcessing(suspend);
    }

    //==============================================================================
    void resized();

private:
    //==============================================================================
    AudioDeviceManager* deviceManager;
    GraphPanelContainer* graphPanelContainer;
    GraphAudioProcessorPlayer graphPlayer;
    // AudioProcessorPlayer graphPlayer;
    MidiKeyboardState keyState;
    InternalMixerStrip* inputStrip;
    InternalMixerStrip* outputStrip;
    SidebarPanel* sidebarPanel;
    BottomPanel* bottomPanel;
    GraphEditorPanel* graphPanel;
    MidiKeyboardComponent* keyboardComp;
    Component* statusBar;
    bool midiLearnEnabled;
    bool addNewMapping;
    bool audioDeviceOk;

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

    float getDesktopScaleFactor() const override
    {
        return 1.0f;
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginWindow)
};

#endif   // __GRAPHEDITORPANEL_JUCEHEADER__
