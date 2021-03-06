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

#include "FilterComponent.h"
#include "MainHostWindow.h"
#include "GraphEditorPanel.h"

//==============================================================================
// Pin Component.
//==============================================================================
PinComponent::PinComponent(FilterGraph& graph_,
                           const uint32 filterID_, const int index_, const bool isInput_)
    : filterID (filterID_),
      index (index_),
      isInput (isInput_),
      graph (graph_)
{
    if (const AudioProcessorGraph::Node::Ptr node = graph.getNodeForId (filterID_))
    {
        String tip;

        if (index_ == FilterGraph::midiChannelNumber)
        {
            tip = isInput ? "MIDI Input" : "MIDI Output";
        }
        else
        {
            if (isInput)
                tip = node->getProcessor()->getInputChannelName (index_);
            else
                tip = node->getProcessor()->getOutputChannelName (index_);

            if (tip.isEmpty())
                tip = (isInput ? "Input " : "Output ") + String (index_ + 1);
        }

        setTooltip (tip);
    }

    setSize (16, 16);
}

void PinComponent::paint (Graphics& g)
{
    const float w = (float) getWidth();
    const float h = (float) getHeight();

    Path p;
    p.addEllipse (w * 0.25f, h * 0.25f, w * 0.5f, h * 0.5f);

    p.addRectangle (w * 0.4f, isInput ? (0.5f * h) : 0.0f, w * 0.2f, h * 0.5f);

    g.setColour (index == FilterGraph::midiChannelNumber ? Colours::cornflowerblue : Colours::green);
    g.fillPath (p);
}

void PinComponent::mouseDown (const MouseEvent& e)
{
    getGraphPanel()->beginConnectorDrag (isInput ? 0 : filterID,
                                         index,
                                         isInput ? filterID : 0,
                                         index,
                                         e);
}

void PinComponent::mouseDrag (const MouseEvent& e)
{
    getGraphPanel()->dragConnector (e);
}

void PinComponent::mouseUp (const MouseEvent& e)
{
    getGraphPanel()->endDraggingConnector (e);
}

//======================================================================
// Mute and bypass button components set up with tooltips
//======================================================================
class MiniButton : public Component,
    public TooltipClient
{
public:
    MiniButton(FilterComponent* _parent, String _name): parent(_parent), name(_name), isMuted(false), isBypassed(false)
    {
        this->setInterceptsMouseClicks(true, true);
    }

    ~MiniButton() {}

    void mouseDown(const MouseEvent& e)
    {
        if(name=="Toggle mute")
        {
            parent->isMuted=!parent->isMuted;
        }
        else
            parent->isBypassed=!parent->isBypassed;

        parent->repaint();
    }

    String getTooltip()
    {
        return name;
    }

private:
    String name;
    FilterComponent* parent;
    bool isBypassed, isMuted;
};
//================================================================================
// Main component used to represent a filter in our graph
//================================================================================
FilterComponent::FilterComponent (FilterGraph& graph_, const uint32 filterID_)
    : graph (graph_),
      filterID (filterID_),
      numInputs (0),
      numOutputs (0),
      pinSize (16),
      rmsLeft(0),
      font (13.0f, Font::bold),

      rmsRight(0),
      numIns (0),
      numOuts (0),
      isMuted(false),
      filterIsPartofSelectedGroup(false),
      isBypassed(false),
      muteButton(0.f, 10.f, 8.f, 8.f),
      bypassButton(0.f, 10.f, 8.f, 8.f)
{
    shadow.setShadowProperties (DropShadow (Colours::black.withAlpha (0.5f), 3, Point<int> (0, 1)));
    setComponentEffect (&shadow);
    //addAndMakeVisible(mute);
    //addAndMakeVisible(bypass);

}
//================================================================================
FilterComponent::~FilterComponent()
{
    mute = nullptr;
    bypass = nullptr;
    deleteAllChildren();
}

//================================================================================
void FilterComponent::mouseDown (const MouseEvent& e)
{
    filterIsPartofSelectedGroup=false;

    if(muteButton.contains(e.getPosition().toFloat()))
    {
        isMuted=!isMuted;
        if(PluginWrapper* tmpPtr = dynamic_cast <PluginWrapper*> (graph.getNodeForId (filterID)->getProcessor()))
        {
            //tmpPtr
            tmpPtr->shouldMute(isMuted);
        }
    }

    if(bypassButton.contains(e.getPosition().toFloat()))
    {
        isBypassed=!isBypassed;
        if(PluginWrapper* tmpPtr = dynamic_cast <PluginWrapper*> (graph.getNodeForId (filterID)->getProcessor()))
        {
            //tmpPtr
            tmpPtr->shouldBypass(isBypassed);
        }
    }

    getGraphPanel()->selectedFilterCoordinates.clear();

    int numSelected = getGraphPanel()->getLassoSelection().getNumSelected();
    for(int i=0; i<numSelected; i++)
    {
        //check if current filter is part of a group or not, if so add position to coor array
        if(filterID==getGraphPanel()->getLassoSelection().getSelectedItem(i)->filterID)
            filterIsPartofSelectedGroup = true;
        getGraphPanel()->selectedFilterCoordinates.add(getGraphPanel()->getLassoSelection().getSelectedItem(i)->getPosition());
    }


    if(!filterIsPartofSelectedGroup)
    {
        for(int i=0; i<getGraphPanel()->getNumChildComponents(); i++)
        {
            //if not part of a group reset colours back to normal
            getGraphPanel()->getChildComponent(i)->getProperties().set("selected", 0);
            getGraphPanel()->getChildComponent(i)->repaint();
        }
        getGraphPanel()->getLassoSelection().deselectAll();
    }

    originalPos = localPointToGlobal (Point<int>());

    toFront (true);

    CabbagePluginAudioProcessor* nativeCabbagePlugin = dynamic_cast<CabbagePluginAudioProcessor*>(graph.getNodeForId (filterID)->getProcessor());

    if (e.mods.isPopupMenu())
    {

        PopupMenu m;
        m.addItem (1, "Delete this filter");
        m.addItem (2, "Disconnect all pins");
        m.addSeparator();

        if(pluginType!=SOUNDFILER)
        {
            m.addItem (3, "Show plugin UI");
            m.addItem (5, "Show all parameters");
        }
        //m.addItem (4, "Show all programs");


        //m.addItem (8, "Assign MIDI");
        if(pluginType == CABBAGE)
        {
            m.addItem (7, "Show source code");
            m.addItem (8, "Export as VST");
            m.addItem (9, "Export as VSTi");
        }

        //m.addItem (6, "Test state save/load");

        const int r = m.show();

        //cUtils::debug(r);

        if (r == 1)
        {
            graph.removeFilter (filterID);
            getGraphDocument()->refreshPluginsInSidebarPanel();
            getGraphDocument()->removeComponentFromBottomPanel("Soundfile Player:"+String(filterID));
            return;
        }
        else if (r == 2)
        {
            graph.disconnectFilter (filterID);
        }
        else if (r==7)
        {
            if(codeWindow==nullptr)
            {
                File csdFile = nativeCabbagePlugin->getCsoundInputFile();
                codeWindow = new CodeWindow(csdFile.getFileNameWithoutExtension());
                codeWindow->addActionListener(this);
                codeWindow->splitBottomWindow->SetSplitBarPosition(codeWindow->getWidth()/3);
                codeWindow->textEditor->setShowTabButtons(false);
                codeWindow->setSize(500, 400);
                codeWindow->setTopLeftPosition(e.getScreenX()-100, e.getScreenY()+100);
                codeWindow->setVisible(true);
                codeWindow->toFront(true);
                codeWindow->setAlwaysOnTop(true);
                codeWindow->setText(csdFile.loadFileAsString(), csdFile.getFullPathName());
                codeWindow->textEditor->setAllText(csdFile.loadFileAsString());

                CabbagePluginAudioProcessor* const processor = (CabbagePluginAudioProcessor*)graph.getNodeForId (filterID)->getProcessor();
                processor->codeEditor = codeWindow->textEditor;

                startTimer(100);
            }
        }
        else if(r == 0)
        {
            return;
        }
        else if(r==8 ||r==9)
        {
            exportPlugin(r==8 ? String("VST") : String("VSTi"), false);
        }

        else
        {
            if (AudioProcessorGraph::Node::Ptr f = graph.getNodeForId (filterID))
            {
                AudioProcessor* const processor = f->getProcessor();
                jassert (processor != nullptr);

                if (r == 6)
                {
                    MemoryBlock state;
                    processor->getStateInformation (state);
                    processor->setStateInformation (state.getData(), (int) state.getSize());
                }
                else
                {
                    PluginWindow::WindowFormatType type = processor->hasEditor() ? PluginWindow::Normal
                                                          : PluginWindow::Generic;

                    switch (r)
                    {
                    case 4:
                        type = PluginWindow::Programs;
                        break;
                    case 5:
                        type = PluginWindow::Parameters;
                        break;

                    default:
                        break;
                    };

                    if (PluginWindow* const w = PluginWindow::getWindowFor (f, type))
                        w->toFront (true);

                }
            }
        }
    }

    if (graph.getNodeForId(filterID)->properties.getWithDefault("pluginType", "")!="Internal")
        getGraphDocument()->expandParametersInPluginsPanel(filterID);

    if (pluginType==SOUNDFILER)
        getGraphDocument()->showComponentInBottomPanel("Soundfile Player:"+String(filterID));

}
//================================================================================
void FilterComponent::mouseDrag (const MouseEvent& e)
{
    if (! e.mods.isPopupMenu())
    {
        Point<int> pos (originalPos + Point<int> (e.getDistanceFromDragStartX(), e.getDistanceFromDragStartY()));

        if (getParentComponent() != nullptr)
            pos = getParentComponent()->getLocalPoint (nullptr, pos);

        int numSelected = getGraphPanel()->getLassoSelection().getNumSelected();

        for(int i=0; i<numSelected; i++)
            if(filterID==getGraphPanel()->getLassoSelection().getSelectedItem(i)->filterID)
                filterIsPartofSelectedGroup = true;

        if(filterIsPartofSelectedGroup)
        {
            for(int i=0; i<numSelected; i++)
            {

                const int fltID = getGraphPanel()->getLassoSelection().getSelectedItem(i)->filterID;
                const int width =  getGraphPanel()->getLassoSelection().getSelectedItem(i)->getWidth();
                const int height =  getGraphPanel()->getLassoSelection().getSelectedItem(i)->getHeight();
                int filterPosX = getGraphPanel()->selectedFilterCoordinates[i].getX();
                int filterPosY = getGraphPanel()->selectedFilterCoordinates[i].getY();

                //Logger::writeToLog("FilterID from Filter Component MouseDrag:"+String(fltID));
                graph.setNodePosition (fltID,
                                       (filterPosX+e.getDistanceFromDragStartX() + width / 2) / (double) getParentWidth(),
                                       (filterPosY+e.getDistanceFromDragStartY() + height / 2) / (double) getParentHeight());
            }
        }

        else
        {
            graph.setNodePosition (filterID,
                                   (pos.getX() + getWidth() / 2) / (double) getParentWidth(),
                                   (pos.getY() + getHeight() / 2) / (double) getParentHeight());
        }

        getGraphPanel()->updateComponents();
    }
    repaint();
}
//================================================================================
void FilterComponent::mouseUp (const MouseEvent& e)
{
    if (e.mouseWasClicked() && e.getNumberOfClicks() == 2)
    {
        if (const AudioProcessorGraph::Node::Ptr f = graph.getNodeForId (filterID))
        {
            if(f->properties.getWithDefault("pluginType", "")!="Internal" &&
                    (f->properties.getWithDefault("pluginType", "")!="SoundfilePlayer"))
                if (PluginWindow* const w = PluginWindow::getWindowFor (f, PluginWindow::Normal))
                    w->toFront (true);
        }
    }
    else if (! e.mouseWasClicked())
    {
        graph.setChangedFlag (true);
    }
}
//================================================================================
bool FilterComponent::hitTest (int x, int y)
{
    for (int i = getNumChildComponents(); --i >= 0;)
        if (getChildComponent(i)->getBounds().contains (x, y))
            return true;

    return x >= 3 && x < getWidth() - 6 && y >= pinSize && y < getHeight() - pinSize;
}

//================================================================================
void FilterComponent::enableEditMode(bool enable)
{
    const AudioProcessorGraph::Node::Ptr f = graph.getNodeForId (filterID);
    if(f->properties.getWithDefault("pluginType","")=="Cabbage")
    {
        if(enable==true)
        {
            if (PluginWindow* const w = PluginWindow::getWindowFor (f, PluginWindow::Normal))
                w->toFront (true);

            if(f->getProcessor()->getActiveEditor())
            {
                ((CabbagePluginAudioProcessorEditor*)f->getProcessor()->getActiveEditor())->addActionListener(&graph);
                ((CabbagePluginAudioProcessorEditor*)f->getProcessor()->getActiveEditor())->setEditMode(true);
                ((CabbagePluginAudioProcessor*)f->getProcessor())->setGuiEnabled(true);
            }
        }
        else
        {
            if(f->getProcessor()->getActiveEditor())
            {
                ((CabbagePluginAudioProcessorEditor*)f->getProcessor()->getActiveEditor())->setEditMode(false);
                ((CabbagePluginAudioProcessor*)f->getProcessor())->setGuiEnabled(false);
            }
        }
    }
}
void FilterComponent::changeListenerCallback(ChangeBroadcaster* source)
{
    //so far only automation tracks call this change method so it's ok to old-style cast.
    //cUtils::debug(((AutomationProcessor*)source)->getAutomationValue());

}
//================================================================================
void FilterComponent::actionListenerCallback (const String &message)
{
    if(message.contains("rmsValues"))
    {
        String newMessage = message.replace("rmsValues ", "");
        rmsLeft = newMessage.substring(0, newMessage.indexOf(" ")).getFloatValue();
        rmsRight = newMessage.substring(newMessage.indexOf(" ")+1).getFloatValue();
        repaint(16, getHeight() - pinSize * 2, getWidth()-32.f, 20);
    }
    else if(message == "closing editor")
    {
        enableEditMode(false);
        getGraphDocument()->disableWidetPropertiesInSidebarPanel();
        codeWindow = nullptr;
        stopTimer();
    }
    else if(message == "enableEditMode")
    {
        enableEditMode(true);
        graph.setEditedNodeId(filterID);
    }
    else if(message == "disableEditMode")
    {
        enableEditMode(false);
        getGraphDocument()->disableWidetPropertiesInSidebarPanel();
    }
    else if(message.contains("fileSave"))
    {
        CabbagePluginAudioProcessor* instance = (CabbagePluginAudioProcessor*)(graph.getNodeForId (filterID)->getProcessor());
        enableEditMode(false);
        String xmlPluginDescriptor = graph.getNodeForId (filterID)->properties.getWithDefault("pluginDesc", "").toString();
        File file(instance->getCsoundInputFile());
        file.replaceWithText(codeWindow->csoundDoc.getAllContent());
        graph.setEditedNodeId(filterID);

        if(instance)
        {
            const int currentChannelCount = graph.getNodeForId(filterID)->getProcessor()->getNumOutputChannels();

            const int newChannelCount = cUtils::getNchnlsFromFile(file.loadFileAsString());

            if((getGraphPanel()!=nullptr) && (currentChannelCount!=newChannelCount))
            {
                codeWindow = nullptr;
                enableEditMode(false);
                getGraphPanel()->updateNode(filterID, newChannelCount, newChannelCount);
            }
            else
            {

                instance->suspendProcessing(true);
                instance->recompileCsound(file);
                instance->setPlayConfigDetails(newChannelCount,
                                               newChannelCount,
                                               instance->getCsoundSamplingRate(),
                                               instance->getCsoundKsmpsSize());

                numIns = instance->getNumberCsoundOutChannels();
                numOuts = instance->getNumberCsoundOutChannels();
                instance->initialiseWidgets(file.loadFileAsString(), true);
                instance->addWidgetsToEditor(true);


                if(instance->getActiveEditor() != nullptr)
                    PluginWindow::getWindowFor (graph.getNodeForId(filterID), PluginWindow::Normal);

                if(codeWindow)
                {
                    codeWindow->setName(file.getFileName());
                    codeWindow->setText(file.loadFileAsString(), file.getFullPathName());

                    codeWindow->textEditor->textChanged = false;
                    instance->codeEditor = codeWindow->textEditor;
                }

                update();
                graph.changed();
            }
            getGraphDocument()->refreshPluginsInSidebarPanel();
        }

        if(message == "fileSaveAndClose")
        {
            enableEditMode(false);
            codeWindow = nullptr;
            stopTimer();
        }

    }
}

//================================================================================
void FilterComponent::timerCallback()
{
    if(pluginType==CABBAGE)
    {
        CabbagePluginAudioProcessor* instance = (CabbagePluginAudioProcessor*)(graph.getNodeForId (filterID)->getProcessor());


        if(codeWindow)
        {
            if(codeWindow->csoundOutputComponent->getText()!=instance->getCsoundOutput())
                codeWindow->csoundOutputComponent->setText(instance->getCsoundOutput());

            if(codeWindow->csoundDebuggerComponent->getText()!=instance->getDebuggerOutput())
                codeWindow->csoundDebuggerComponent->setText(instance->getDebuggerOutput());
                
            instance->clearDebugMessage();
        }
    }

}
//================================================================================
void FilterComponent::paint (Graphics& g)
{
    int selected = getProperties().getWithDefault("selected", 0);

    const int x = 4;
    const int y = pinSize;
    const int w = getWidth() - x * 2;
    const int h = getHeight() - pinSize * 2;

    // g.setColour(cUtils::getComponentSkin());
    g.setColour(Colour(20, 20, 20));
    g.fillRoundedRectangle(x, y, w, h, 5);

    g.drawRoundedRectangle(x, y, w, h, 5, 1.f);
    g.setColour (cUtils::getComponentFontColour());
    g.setFont (cUtils::getComponentFont());
    g.drawFittedText (getName(),
                      x + 4, y - 2, w - 8, h - 4,
                      Justification::centred, 2);

    g.setOpacity(0.2);
    g.setColour(selected==1 ? Colours::yellow : cUtils::getComponentFontColour().withAlpha(.3f));
    g.drawRoundedRectangle(x+0.5, y+0.5, w-1, h-1, 5, 1.0f);
    if(pluginType!=INTERNAL && pluginType!=AUTOMATION)
    {
        if(!isMuted)
        {
            drawLevelMeter(g, 16, h+4, getWidth()-32.f, 7,
                           (float) exp (log (rmsLeft) / 3.0)); // (add a bit of a skew to make the level more obvious)
            drawLevelMeter(g, 16, h+9, getWidth()-32.f, 7,
                           (float) exp (log (rmsRight) / 3.0)); // (add a bit of a skew to make the level more obvious)

        }
        drawBypassIcon(g, bypassButton, isBypassed);
        drawMuteIcon(g, muteButton, isMuted);
    }
}

void FilterComponent::drawBypassIcon(Graphics& g, Rectangle<float> rect, bool isActive)
{
    const float x = rect.getX();
    const float y = rect.getY();
    const float w = rect.getWidth()-5.f;
    const float h = rect.getHeight();
    const float d = 5;
    g.setColour(isActive ? Colours::cornflowerblue : Colours::lime);
    Path p;
    p.startNewSubPath(x+5, y+h/2.f+d/2.f);
    g.drawEllipse(x, y+h/2.f, d, d, 1);
    g.drawEllipse(x+w, y+h/2.f, d, d, 1.f);

    if(!isActive)
    {
        p.lineTo(x+w, y+h/2.f+d/2.f);
    }
    else
    {
        p.addArc(x+w, y+h/2.f+d/2.f, 5, 5, 3.14, 3.14);
    }

    p.closeSubPath();
    g.strokePath(p, PathStrokeType(1));
}

void FilterComponent::drawMuteIcon(Graphics& g, Rectangle<float> rect, bool muted)
{
    const float x = rect.getX();
    const float y = rect.getY();
    const float w = rect.getWidth()-5.f;
    const float h = rect.getHeight();
    g.setColour(muted ? Colours::lime : Colours::cornflowerblue);
    Path p;
    p.startNewSubPath(x, y+h*.33);
    p.lineTo(x+w/2.f, y+h*.33);
    p.lineTo(x+w, y+0.f);
    p.lineTo(x+w, y+h);
    p.lineTo(x+w/2.f, y+h*.66);
    p.lineTo(x, y+h*.66);
    p.lineTo(x, y+h*.33);
    p.closeSubPath();
    g.strokePath(p, PathStrokeType(1));

    if(muted)
    {
        g.setColour(Colours::whitesmoke);
        g.drawLine(x+0, y+0, x+w, y+h, 2);
        g.drawLine(x+0, y+h, x+w, y+0, 2);
    }
    else
        g.fillPath(p);
}

void FilterComponent::drawLevelMeter (Graphics& g, float x, float y, int width, int height, float level)
{
    g.fillAll(filterColour);
    const int totalBlocks = 20;
    const int numBlocks = roundToInt (totalBlocks * level)-1;
    const float w = (width) / (float) totalBlocks;

    for (int i = 0; i < totalBlocks; ++i)
    {
        if (i >= numBlocks)
            g.setColour (Colours::lightblue.withAlpha (0.1f));
        else
            g.setColour (i < totalBlocks - 1 ? (isBypassed ? Colours::cornflowerblue : Colours::lime.withAlpha (0.5f))
                             : Colours::red);

        g.fillRoundedRectangle (x + i * w + w * 0.1f,
                                y+1,
                                w,
                                height/2,
                                .5f);
    }
}

//================================================================================
void FilterComponent::resized()
{
    for (int i = 0; i < getNumChildComponents(); ++i)
    {
        if (PinComponent* const pc = dynamic_cast <PinComponent*> (getChildComponent(i)))
        {
            const int total = pc->isInput ? numIns : numOuts;
            const int index = pc->index == FilterGraph::midiChannelNumber ? (total - 1) : pc->index;

            pc->setBounds (proportionOfWidth ((1 + index) / (total + 1.0f)) - pinSize / 2,
                           pc->isInput ? 0 : (getHeight() - pinSize),
                           pinSize, pinSize);
        }
    }

    if(mute && bypass)
    {
        mute->setBounds(getWidth()-20, 20.f, 15.f, 15.f);
        bypass->setBounds(10, 16.f, 15.f, 15.f);
    }
}
//================================================================================
void FilterComponent::getPinPos (const int index, const bool isInput, float& x, float& y)
{
    for (int i = 0; i < getNumChildComponents(); ++i)
    {
        if (PinComponent* const pc = dynamic_cast <PinComponent*> (getChildComponent(i)))
        {
            if (pc->index == index && isInput == pc->isInput)
            {
                x = getX() + pc->getX() + pc->getWidth() * 0.5f;
                y = getY() + pc->getY() + pc->getHeight() * 0.5f;
                break;
            }
        }
    }
}

//================================================================================
void FilterComponent::update()
{
    const AudioProcessorGraph::Node::Ptr f (graph.getNodeForId (filterID));

    if (f == nullptr)
    {
        delete this;
        return;
    }

    if(f->properties.getWithDefault("pluginType","")=="Internal")
        pluginType = INTERNAL;
    else if(f->properties.getWithDefault("pluginType","")=="Cabbage")
        pluginType = CABBAGE;
    else if(f->properties.getWithDefault("pluginType","")=="AutomationTrack")
    {
        pluginType = AUTOMATION;
        ((AutomationProcessor*)f->getProcessor())->addChangeListener(&graph);
        AudioProcessorEditor* ui = nullptr;
        ui = f->getProcessor()->createEditorIfNeeded();
        ui->setName("AutomationTrack");
        getGraphDocument()->addComponentToBottomPanel(ui);
    }
    else if(f->properties.getWithDefault("pluginType","")=="SoundfilePlayer")
    {
        pluginType = SOUNDFILER;
        AudioProcessorEditor* ui = nullptr;
        ui = f->getProcessor()->createEditorIfNeeded();
        ui->setName("Soundfile Player:"+String( f->nodeId));
        getGraphDocument()->addComponentToBottomPanel(ui);
    }
    else
        pluginType = THIRDPARTY;

    numIns = f->getProcessor()->getNumInputChannels();
    if (f->getProcessor()->acceptsMidi())
        ++numIns;

    numOuts = f->getProcessor()->getNumOutputChannels();
    if (f->getProcessor()->producesMidi())
        ++numOuts;

    int w = 100;
    int h = 60;

    w = jmax (w, (jmax (numIns, numOuts) + 1) * 20);

    const String pluginName = f->properties.getWithDefault("pluginName", "").toString();
    const int textWidth = font.getStringWidth (pluginName);

    w = jmax (w, jmin (textWidth+50, 300));

    if (textWidth > 300)
        h = 160;

    setSize (w, h);
    muteButton = Rectangle<float>(w-20, 20.f, 15.f, 15.f);
    bypassButton = Rectangle<float>(10, 16.f, 15.f, 15.f);


    if(PluginWrapper* tmpPlug = dynamic_cast <PluginWrapper*> (f->getProcessor()))
    {
        setName (tmpPlug->getPluginName());
        tmpPlug->addActionListener(this);
    }

    else if(CabbagePluginAudioProcessor* tmpPlug = dynamic_cast <CabbagePluginAudioProcessor*> (f->getProcessor()))
    {
        setName (tmpPlug->getPluginName());
        tmpPlug->addActionListener(this);
    }

    else if(AudioFilePlaybackProcessor* tmpPlug = dynamic_cast <AudioFilePlaybackProcessor*> (f->getProcessor()))
    {
        setName (pluginName);
        tmpPlug->addActionListener(this);
    }

    setName(pluginName);


    {
        double x, y;
        graph.getNodePosition (filterID, x, y);
        setCentreRelative ((float) x, (float) y);
    }


    if (numIns != numInputs || numOuts != numOutputs)
    {
        numInputs = numIns;
        numOutputs = numOuts;

        deleteAllChildren();

        int i;
        for (i = 0; i < f->getProcessor()->getNumInputChannels(); ++i)
            if(pluginType!=SOUNDFILER &&pluginType!=AUTOMATION)
                addAndMakeVisible (new PinComponent (graph, filterID, i, true));

        if (f->getProcessor()->acceptsMidi())
            if(pluginType!=SOUNDFILER && pluginType!=AUTOMATION)
                addAndMakeVisible (new PinComponent (graph, filterID, FilterGraph::midiChannelNumber, true));

        for (i = 0; i < f->getProcessor()->getNumOutputChannels(); ++i)
            if(pluginType!=AUTOMATION)
                addAndMakeVisible (new PinComponent (graph, filterID, i, false));

        if (f->getProcessor()->producesMidi())
            if(pluginType!=SOUNDFILER && pluginType!=AUTOMATION)
                addAndMakeVisible (new PinComponent (graph, filterID, FilterGraph::midiChannelNumber, false));

        if(pluginType!=INTERNAL && pluginType!=AUTOMATION)
        {
//            addAndMakeVisible(mute = new MiniButton(this, "Toggle mute"));
//            addAndMakeVisible(bypass = new MiniButton(this, "Toggle bypass"));
        }

        resized();
    }
}

//==============================================================================
// connecting patch lead component
//==============================================================================
ConnectorComponent::ConnectorComponent (FilterGraph& graph_)
    : sourceFilterID (0),
      destFilterID (0),
      sourceFilterChannel (0),
      destFilterChannel (0),
      graph (graph_),
      lastInputX (0),
      lastInputY (0),
      lastOutputX (0),
      lastOutputY (0)
{
    setAlwaysOnTop (true);
}

void ConnectorComponent::setInput (const uint32 sourceFilterID_, const int sourceFilterChannel_)
{
    if (sourceFilterID != sourceFilterID_ || sourceFilterChannel != sourceFilterChannel_)
    {
        sourceFilterID = sourceFilterID_;
        sourceFilterChannel = sourceFilterChannel_;
        update();
    }
}

void ConnectorComponent::setOutput (const uint32 destFilterID_, const int destFilterChannel_)
{
    if (destFilterID != destFilterID_ || destFilterChannel != destFilterChannel_)
    {
        destFilterID = destFilterID_;
        destFilterChannel = destFilterChannel_;
        update();
    }
}

void ConnectorComponent::dragStart (int x, int y)
{
    lastInputX = (float) x;
    lastInputY = (float) y;
    resizeToFit();
}

void ConnectorComponent::dragEnd (int x, int y)
{
    lastOutputX = (float) x;
    lastOutputY = (float) y;
    resizeToFit();
}

void ConnectorComponent::update()
{
    float x1, y1, x2, y2;
    getPoints (x1, y1, x2, y2);

    if (lastInputX != x1
            || lastInputY != y1
            || lastOutputX != x2
            || lastOutputY != y2)
    {
        resizeToFit();
    }
}

void ConnectorComponent::resizeToFit()
{
    float x1, y1, x2, y2;
    getPoints (x1, y1, x2, y2);

    const Rectangle<int> newBounds ((int) jmin (x1, x2) - 4,
                                    (int) jmin (y1, y2) - 4,
                                    (int) std::abs (x1 - x2) + 8,
                                    (int) std::abs (y1 - y2) + 8);

    if (newBounds != getBounds())
        setBounds (newBounds);
    else
        resized();

    repaint();
}

void ConnectorComponent::getPoints (float& x1, float& y1, float& x2, float& y2) const
{
    x1 = lastInputX;
    y1 = lastInputY;
    x2 = lastOutputX;
    y2 = lastOutputY;

    if (GraphEditorPanel* const hostPanel = getGraphPanel())
    {
        if (FilterComponent* srcFilterComp = hostPanel->getComponentForFilter (sourceFilterID))
            srcFilterComp->getPinPos (sourceFilterChannel, false, x1, y1);

        if (FilterComponent* dstFilterComp = hostPanel->getComponentForFilter (destFilterID))
            dstFilterComp->getPinPos (destFilterChannel, true, x2, y2);
    }
}

void ConnectorComponent::paint (Graphics& g)
{
    if (sourceFilterChannel == FilterGraph::midiChannelNumber
            || destFilterChannel == FilterGraph::midiChannelNumber)
    {
        g.setColour (Colours::cornflowerblue);
    }
    else
    {
        g.setColour (Colours::green);
    }

    g.fillPath (linePath);
}

bool ConnectorComponent::hitTest (int x, int y)
{
    if (hitPath.contains ((float) x, (float) y))
    {
        double distanceFromStart, distanceFromEnd;
        getDistancesFromEnds (x, y, distanceFromStart, distanceFromEnd);

        // avoid clicking the connector when over a pin
        return distanceFromStart > 7.0 && distanceFromEnd > 7.0;
    }

    return false;
}

void ConnectorComponent::mouseDown (const MouseEvent&)
{
    dragging = false;
}

void ConnectorComponent::mouseDrag (const MouseEvent& e)
{
    if ((! dragging) && ! e.mouseWasClicked())
    {
        dragging = true;

        graph.removeConnection (sourceFilterID, sourceFilterChannel, destFilterID, destFilterChannel);

        double distanceFromStart, distanceFromEnd;
        getDistancesFromEnds (e.x, e.y, distanceFromStart, distanceFromEnd);
        const bool isNearerSource = (distanceFromStart < distanceFromEnd);

        getGraphPanel()->beginConnectorDrag (isNearerSource ? 0 : sourceFilterID,
                                             sourceFilterChannel,
                                             isNearerSource ? destFilterID : 0,
                                             destFilterChannel,
                                             e);
    }
    else if (dragging)
    {
        getGraphPanel()->dragConnector (e);
    }
}

void ConnectorComponent::mouseUp (const MouseEvent& e)
{
    if (dragging)
        getGraphPanel()->endDraggingConnector (e);
}

void ConnectorComponent::resized()
{
    float x1, y1, x2, y2;
    getPoints (x1, y1, x2, y2);

    lastInputX = x1;
    lastInputY = y1;
    lastOutputX = x2;
    lastOutputY = y2;

    x1 -= getX();
    y1 -= getY();
    x2 -= getX();
    y2 -= getY();

    linePath.clear();
    linePath.startNewSubPath (x1, y1);
    linePath.cubicTo (x1, y1 + (y2 - y1) * 0.33f,
                      x2, y1 + (y2 - y1) * 0.66f,
                      x2, y2);

    PathStrokeType wideStroke (8.0f);
    wideStroke.createStrokedPath (hitPath, linePath);

    PathStrokeType stroke (2.5f);
    stroke.createStrokedPath (linePath, linePath);

    const float arrowW = 5.0f;
    const float arrowL = 4.0f;

    Path arrow;
    arrow.addTriangle (-arrowL, arrowW,
                       -arrowL, -arrowW,
                       arrowL, 0.0f);

    arrow.applyTransform (AffineTransform::identity
                          .rotated (float_Pi * 0.5f - (float) atan2 (x2 - x1, y2 - y1))
                          .translated ((x1 + x2) * 0.5f,
                                       (y1 + y2) * 0.5f));

    linePath.addPath (arrow);
    linePath.setUsingNonZeroWinding (true);
}


void ConnectorComponent::getDistancesFromEnds (int x, int y, double& distanceFromStart, double& distanceFromEnd) const
{
    float x1, y1, x2, y2;
    getPoints (x1, y1, x2, y2);

    distanceFromStart = juce_hypot (x - (x1 - getX()), y - (y1 - getY()));
    distanceFromEnd = juce_hypot (x - (x2 - getX()), y - (y2 - getY()));
}

//===================== EXPORT FUNCTIONS FOR CABBAGE PLUGINS ==========================
int FilterComponent::exportPlugin(String type, bool saveAs, String fileName)
{
    File dll;
    File loc_csdFile;
#ifndef LINUX
    File thisFile(File::getSpecialLocation(File::currentApplicationFile));
#else
    File thisFile(File::getSpecialLocation(File::currentExecutableFile));
#endif

    CabbagePluginAudioProcessor* nativeCabbagePlugin = dynamic_cast<CabbagePluginAudioProcessor*>(graph.getNodeForId (filterID)->getProcessor());

    File csdFile = nativeCabbagePlugin->getCsoundInputFile();

    String currentApplicationDirectory =thisFile.getParentDirectory().getFullPathName();

    if(!csdFile.exists())
    {
        cUtils::showMessage("You need to open a Cabbage instrument before you can export one as a plugin!");
        return 0;
    }
#ifdef LINUX
    //FileChooser saveFC(String("Save as..."), File::nonexistent, String(""), UseNativeDialogue);
    bool showNative = cUtils::getPreference(appProperties, "ShowNativeFileDialogues");
    WildcardFileFilter wildcardFilter ("*.*", String::empty, "Foo files");
    Array<File> selectedFile = cUtils::launchFileBrowser("Save a file..", wildcardFilter, "*.*", 0, File("*"), showNative, &getLookAndFeel());

    String VST;

    if (selectedFile.size()>0)
    {
        if(type.contains("VSTi"))
            VST = currentApplicationDirectory + String("/CabbagePluginSynth.so");
        else if(type.contains(String("VST")))
            VST = currentApplicationDirectory + String("/CabbagePluginEffect.so");
        else if(type.contains(String("LV2-ins")))
            VST = currentApplicationDirectory + String("/CabbagePluginSynthLV2.so");
        else if(type.contains(String("LV2-fx")))
            VST = currentApplicationDirectory + String("/CabbagePluginEffectLV2.so");
        else if(type.contains(String("AU")))
        {
            cUtils::showMessage("This feature only works on computers running OSX");
        }
        //Logger::writeToLog(VST);

        File VSTData(VST);
        if(!VSTData.exists())
        {
            cUtils::showMessage(VST+" cannot be found?");
        }

        else
        {
            if (type.contains("LV2"))
            {
                String filename(selectedFile[0].getFileNameWithoutExtension());
                File bundle(selectedFile[0].withFileExtension(".lv2").getFullPathName());
                bundle.createDirectory();
                File dll(bundle.getChildFile(filename+".so"));
                Logger::writeToLog(bundle.getFullPathName());
                Logger::writeToLog(dll.getFullPathName());
                if(!VSTData.copyFileTo(dll))
                    cUtils::showMessage("Can not move lib");

                File loc_csdFile(bundle.getChildFile(filename+".csd").getFullPathName());
                loc_csdFile.replaceWithText(csdFile.loadFileAsString());

                // this generates the ttl data
                typedef void (*TTL_Generator_Function)(const char* basename);
                DynamicLibrary lib(dll.getFullPathName());
                TTL_Generator_Function genFunc = (TTL_Generator_Function)lib.getFunction("lv2_generate_ttl");
                if(!genFunc)
                {
                    cUtils::showMessage("Can not generate LV2 data");
                    return 1;
                }

                // change CWD for ttl generation
                File oldCWD(File::getCurrentWorkingDirectory());
                bundle.setAsCurrentWorkingDirectory();
                (genFunc)(filename.toRawUTF8());
                oldCWD.setAsCurrentWorkingDirectory();
            }
            else
            {
                File dll(selectedFile[0].withFileExtension(".so").getFullPathName());
                if(!VSTData.copyFileTo(dll))
                    cUtils::showMessage("Can not move lib");

                File loc_csdFile(selectedFile[0].withFileExtension(".csd").getFullPathName());
                loc_csdFile.replaceWithText(csdFile.loadFileAsString());
                setUniquePluginID(dll, loc_csdFile, false);
            }
        }
    }
#elif WIN32
    FileChooser saveFC(String("Save plugin as..."), File::nonexistent, String("*.dll"), UseNativeDialogue);
    String VST;
    if(type.contains("VSTi"))
        VST = thisFile.getParentDirectory().getFullPathName() + String("\\CabbagePluginSynth.dat");
    else if(type.contains(String("VST")))
        VST = thisFile.getParentDirectory().getFullPathName() + String("\\CabbagePluginEffect.dat");

    File VSTData(VST);

    if(!VSTData.exists())
    {
        cUtils::showMessage("Cabbage cannot find the plugin libraries. Make sure that Cabbage is situated in the same directory as CabbagePluginSynth.dat and CabbagePluginEffect.dat");
        return 0;
    }
    else
    {
        if(saveAs)
        {
            if (saveFC.browseForFileToSave(true))
            {
                dll = saveFC.getResult().withFileExtension(".dll").getFullPathName();
                loc_csdFile = saveFC.getResult().withFileExtension(".csd").getFullPathName();
            }
            else
                return 0;
        }
        else
        {
            dll = csdFile.withFileExtension(".dll").getFullPathName();
            loc_csdFile = csdFile.withFileExtension(".csd").getFullPathName();

        }
        //showMessage(dll.getFullPathName());
        if(!VSTData.copyFileTo(dll))
            cUtils::showMessage("Problem moving plugin lib, make sure it's not currently open in your plugin host!");

        loc_csdFile.replaceWithText(csdFile.loadFileAsString());
        setUniquePluginID(dll, loc_csdFile, false);
        String info;
        info = String("Your plugin has been created. It's called:\n\n")+dll.getFullPathName()+String("\n\nIn order to modify this plugin you only have to edit the associated .csd file. You do not need to export every time you make changes.\n\nTo turn off this notice visit 'Preferences' in the main 'options' menu");

        //int val = getPreference(appProperties, "DisablePluginInfo");
        //if(!val)
        //    cUtils::showMessage(info);
    }

#endif

#if MACOSX

    FileChooser saveFC(String("Save as..."), File::nonexistent, String("*.vst"), UseNativeDialogue);
    String VST;
    if (saveFC.browseForFileToSave(true))
    {
        //showMessage("name of file is:");
        //showMessage(saveFC.getResult().withFileExtension(".vst").getFullPathName());

        if(type.contains("VSTi"))
            VST = thisFile.getFullPathName()+"/Contents/CabbagePluginSynth.component";
        else if(type.contains(String("VST")))
            VST = thisFile.getFullPathName()+"/Contents/CabbagePluginEffect.component";
        else if(type.contains("CsoundVSTi"))
            VST = thisFile.getFullPathName()+"/Contents/CabbageCsoundPluginSynth.component";
        else if(type.contains(String("CsoundVST")))
            VST = thisFile.getFullPathName()+"/Contents/CabbageCsoundPluginEffect.component";
        else if(type.contains(String("AU")))
        {
            cUtils::showMessage("this feature is coming soon");
            //VST = thisFile.getParentDirectory().getFullPathName() + String("\\CabbageVSTfx.component");
        }

        //create a copy of the data package and write it to the new location given by user
        File VSTData(VST);
        if(!VSTData.exists())
        {
            cUtils::showMessage("Cabbage cannot find the plugin libraries. Make sure that Cabbage is situated in the same directory as CabbagePluginSynth.dat and CabbagePluginEffect.dat");
            return 0;
        }


        String plugType = ".vst";

        File dll(saveFC.getResult().withFileExtension(plugType).getFullPathName());
        VSTData.copyFileTo(dll);

        File pl(dll.getFullPathName()+String("/Contents/Info.plist"));
        String newPList = pl.loadFileAsString();
        //write our identifiers to the plist file
        newPList = newPList.replace("CabbagePlugin", saveFC.getResult().getFileNameWithoutExtension());
        newPList = newPList.replace("NewProject", saveFC.getResult().getFileNameWithoutExtension());
        //write plist file
        pl.replaceWithText(newPList);



        File loc_csdFile(dll.getFullPathName()+String("/Contents/")+saveFC.getResult().getFileNameWithoutExtension()+String(".csd"));
        loc_csdFile.replaceWithText(csdFile.loadFileAsString());
        //showMessage(loc_csdFile.getFullPathName());
        //showMessage(csdFile.loadFileAsString());
        csdFile = loc_csdFile;

        //showMessage(VST+String("/Contents/MacOS/")+saveFC.getResult().getFileNameWithoutExtension());
        //if plugin already exists there is no point in rewriting the binaries
        if(!File(saveFC.getResult().withFileExtension(".vst").getFullPathName()+("/Contents/MacOS/")+saveFC.getResult().getFileNameWithoutExtension()).exists())
        {
            File bin(dll.getFullPathName()+String("/Contents/MacOS/CabbagePlugin"));
            File pluginBinary(dll.getFullPathName()+String("/Contents/MacOS/")+saveFC.getResult().getFileNameWithoutExtension());
            bin.moveFileTo(pluginBinary);
            //else
            //showMessage("could not copy library binary file");

            setUniquePluginID(pluginBinary, loc_csdFile, true);
        }

        String info;
        info = String("Your plugin has been created. It's called:\n\n")+dll.getFullPathName()+String("\n\nIn order to modify this plugin you only have to edit the current .csd file. You do not need to export every time you make changes. To open the current csd file with Cabbage in another session, go to 'Open Cabbage Instrument' and select the .vst file. Cabbage will the load the associated .csd file. \n\nTo turn off this notice visit 'Preferences' in the main 'options' menu");

        int val = appProperties->getUserSettings()->getValue("DisablePluginInfo", var(0)).getFloatValue();
        if(!val)
            cUtils::showMessage(info);


    }

#endif

    return 0;
}

//==============================================================================
// Set unique plugin ID for each plugin based on the file name
//==============================================================================
int FilterComponent::setUniquePluginID(File binFile, File csdFile, bool AU)
{
    String newID;
    StringArray csdText;
    csdText.addLines(csdFile.loadFileAsString());
    //read contents of csd file to find pluginID
    for(int i=0; i<csdText.size(); i++)
    {
        StringArray tokes;
        tokes.addTokens(csdText[i].trimEnd(), ", ", "\"");
        if(tokes[0].equalsIgnoreCase(String("form")))
        {
            CabbageGUIType cAttr(csdText[i].trimEnd(), 0);
            if(cAttr.getStringProp(CabbageIDs::pluginid).length()!=4)
            {
                cUtils::showMessage("Your plugin ID is not the right size. It MUST be 4 characters long. Some hosts may not be able to load your plugin");
                //return 0;
            }
            else
            {
                newID = cAttr.getStringProp(CabbageIDs::pluginid);
                i = csdText.size();
            }
        }
    }

    size_t file_size;
    const char *pluginID;
    //if(!AU)
    pluginID = "YROR";
    //else
    //	pluginID = "RORY";


    long loc;
    //showMessage(binFile.getFullPathName(), lookAndFeel);
    fstream mFile(binFile.getFullPathName().toUTF8(), ios_base::in | ios_base::out | ios_base::binary);

    if(mFile.is_open())
    {
        mFile.seekg (0, ios::end);
        file_size = mFile.tellg();
        unsigned char* buffer = (unsigned char*)malloc(sizeof(unsigned char)*file_size);
        //set plugin ID, do this a few times in case the plugin ID appear in more than one place.
        for(int r=0; r<10; r++)
        {
            mFile.seekg (0, ios::beg);
            mFile.read((char*)&buffer[0], file_size);
            loc = cabbageFindPluginID(buffer, file_size, pluginID);
            if (loc < 0)
            {
                //showMessage(String("Internel Cabbage Error: The pluginID was not found"));
                break;
            }
            else
            {
                //showMessage(newID);
                mFile.seekg (loc, ios::beg);
                mFile.write(newID.toUTF8(), 4);
            }
        }
        //set plugin name based on .csd file
        const char *pluginName = "CabbageEffectNam";
        String plugLibName = csdFile.getFileNameWithoutExtension();
        if(plugLibName.length()<16)
            for(int y=plugLibName.length(); y<16; y++)
                plugLibName.append(String(" "), 1);

        mFile.seekg (0, ios::beg);
        buffer = (unsigned char*)malloc(sizeof(unsigned char)*file_size);
        mFile.read((char*)&buffer[0], file_size);
        loc = cabbageFindPluginID(buffer, file_size, pluginName);
        if (loc < 0)
            cUtils::showMessage("Plugin name could not be set?!?");
        else
        {
            //showMessage("plugin name set!");
            mFile.seekg (loc, ios::beg);
            mFile.write(csdFile.getFileNameWithoutExtension().toUTF8(), 16);
        }
        //#endif

    }
    else
        cUtils::showMessage("File could not be opened");

    mFile.close();
    return 1;
}

long FilterComponent::cabbageFindPluginID(unsigned char *buf, size_t len, const char *s)
{
    long i, j;
    int slen = strlen(s);
    long imax = len - slen - 1;
    long ret = -1;
    int match;

    for(i=0; i<imax; i++)
    {
        match = 1;
        for (j=0; j<slen; j++)
        {
            if (buf[i+j] != s[j])
            {
                match = 0;
                break;
            }
        }
        if (match)
        {
            ret = i;
            break;
        }
    }
    //return position of plugin ID
    return ret;
}
