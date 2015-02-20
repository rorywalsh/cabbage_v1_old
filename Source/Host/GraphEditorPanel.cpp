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
#include "GraphEditorPanel.h"
#include "InternalFilters.h"
#include "MainHostWindow.h"
#include "PluginWrapperProcessor.h"
#include "AudioFilePlaybackEditor.h"
#include "../Plugin/CabbageGenericAudioProcessorEditor.h"
#include "SidebarPanel.h"

//==============================================================================
class PluginWindow;
static Array <PluginWindow*> activePluginWindows;

PluginWindow::PluginWindow (Component* const pluginEditor,
                            AudioProcessorGraph::Node* const o,
                            WindowFormatType t)
    : DocumentWindow (pluginEditor->getName(), Colours::black,
                      DocumentWindow::minimiseButton | DocumentWindow::closeButton),
    owner (o),
    type (t),
	editor(pluginEditor),	
	basicLookAndFeel(new CabbageLookAndFeelBasic())
{
    setSize (400, 300);	
	this->setTitleBarHeight(18);
	//setLookAndFeel(basicLookAndFeel);
    setContentOwned (pluginEditor, true);

    setTopLeftPosition (owner->properties.getWithDefault ("uiLastX", Random::getSystemRandom().nextInt (500)),
                        owner->properties.getWithDefault ("uiLastY", Random::getSystemRandom().nextInt (500)));
    setVisible (true);
	setAlwaysOnTop(true);
    activePluginWindows.add (this);
	startTimer(100);
}

void PluginWindow::timerCallback()
{
	this->getContentComponent()->setTopLeftPosition(0, 18);
	stopTimer();
}

void PluginWindow::closeCurrentlyOpenWindowsFor (const uint32 nodeId)
{
    for (int i = activePluginWindows.size(); --i >= 0;)
        if (activePluginWindows.getUnchecked(i)->owner->nodeId == nodeId)
            delete activePluginWindows.getUnchecked (i);
}

void PluginWindow::closeAllCurrentlyOpenWindows()
{
    if (activePluginWindows.size() > 0)
    {
        for (int i = activePluginWindows.size(); --i >= 0;)
            delete activePluginWindows.getUnchecked (i);

        Component dummyModalComp;
        dummyModalComp.enterModalState();
        MessageManager::getInstance()->runDispatchLoopUntil (50);
    }
}

void PluginWindow::updateWindow(AudioProcessorGraph::Node* node, int nodeId)
{
    for (int i = activePluginWindows.size(); --i >= 0;)
        if (activePluginWindows.getUnchecked(i)->owner->nodeId == nodeId){
			 AudioProcessorEditor* ui = nullptr;
			 activePluginWindows.getUnchecked(i)->setContentOwned(nullptr, false);
			 ui = node->getProcessor()->createEditor();
			 if(ui)
			 activePluginWindows.getUnchecked(i)->setContentOwned(ui, true);
			 activePluginWindows.getUnchecked(i)->repaint();


		}
}
//==============================================================================
class ProcessorProgramPropertyComp : public PropertyComponent,
    private AudioProcessorListener
{
public:
    ProcessorProgramPropertyComp (const String& name, AudioProcessor& p, int index_)
        : PropertyComponent (name),
          owner (p),
          index (index_)
    {
        owner.addListener (this);
    }

    ~ProcessorProgramPropertyComp()
    {
        owner.removeListener (this);
    }

    void refresh() { }
    void audioProcessorChanged (AudioProcessor*) { }
    void audioProcessorParameterChanged (AudioProcessor*, int, float) { }

private:
    AudioProcessor& owner;
    const int index;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ProcessorProgramPropertyComp)
};

class ProgramAudioProcessorEditor : public AudioProcessorEditor
{
public:
    ProgramAudioProcessorEditor (AudioProcessor* const p)
        : AudioProcessorEditor (p)
    {
        jassert (p != nullptr);
        setOpaque (true);

        addAndMakeVisible (panel);

        Array<PropertyComponent*> programs;

        const int numPrograms = p->getNumPrograms();
        int totalHeight = 0;

        for (int i = 0; i < numPrograms; ++i)
        {
            String name (p->getProgramName (i).trim());

            if (name.isEmpty())
                name = "Unnamed";

            ProcessorProgramPropertyComp* const pc = new ProcessorProgramPropertyComp (name, *p, i);
            programs.add (pc);
            totalHeight += pc->getPreferredHeight();
        }

        panel.addProperties (programs);

        setSize (400, jlimit (25, 400, totalHeight));
    }

    void paint (Graphics& g)
    {
        g.fillAll (Colours::grey);
    }

    void resized()
    {
        panel.setBounds (getLocalBounds());
    }

private:
    PropertyPanel panel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ProgramAudioProcessorEditor)
};

//==============================================================================
PluginWindow* PluginWindow::getWindowFor (AudioProcessorGraph::Node* const node,
        WindowFormatType type)
{
    jassert (node != nullptr);

    for (int i = activePluginWindows.size(); --i >= 0;)
        if (activePluginWindows.getUnchecked(i)->owner == node
                && activePluginWindows.getUnchecked(i)->type == type)
            return activePluginWindows.getUnchecked(i);

    AudioProcessor* processor = node->getProcessor();	
	//PluginWrapper* wrapperPlug = dynamic_cast<PluginWrapper*>(processor);
	
    AudioProcessorEditor* ui = nullptr;

    if (type == Normal)
    {
        ui = processor->createEditorIfNeeded();
        if (ui == nullptr)
            type = Generic;
    }

    if (ui == nullptr)
    {
        if (type == Generic || type == Parameters)
		{
			if(processor)
			{
				ui = new CabbageGenericAudioProcessorEditor (processor, type == midiLearn ? true : false);
			}
		}
        else if (type == Programs)
            ui = new ProgramAudioProcessorEditor (processor);
    }

    if (ui != nullptr)
    {
        if (AudioPluginInstance* const plugin = dynamic_cast<AudioPluginInstance*> (processor))
            ui->setName (plugin->getName());

        return new PluginWindow (ui, node, type);
    }

    return nullptr;
}

PluginWindow::~PluginWindow()
{
    activePluginWindows.removeFirstMatchingValue (this);
    clearContentComponent();
}

void PluginWindow::moved()
{
    owner->properties.set ("uiLastX", getX());
    owner->properties.set ("uiLastY", getY());
}

void PluginWindow::closeButtonPressed()
{
    delete this;
}


//==============================================================================
GraphEditorPanel::GraphEditorPanel (FilterGraph& graph_)
    : graph (graph_), wildcardFilter("*.*", "*", "File Filter")
{
    InternalPluginFormat internalFormat;

    graph.addFilter (internalFormat.getDescriptionFor (InternalPluginFormat::audioInputFilter),
               0.55f, 0.5f);

    graph.addFilter (internalFormat.getDescriptionFor (InternalPluginFormat::midiInputFilter),
               0.53f, 0.5f);

    graph.addFilter (internalFormat.getDescriptionFor (InternalPluginFormat::audioOutputFilter),
               0.55f, 0.56f);

				
//	//each patch has an automation track
//	PluginDescription descript;
//	descript.descriptiveName = "Automation Track";
//	descript.name = "AutomationTrack";
//	descript.pluginFormatName = "AutomationTrack";
//
//	graph.addFilter (&descript, 0.0f, 0.0f);
	
	
    graph.addChangeListener (this);
    setOpaque (true);
	//setSize(10000, 10000);
}

GraphEditorPanel::~GraphEditorPanel()
{
    graph.removeChangeListener (this);
    draggingConnector = nullptr;
    deleteAllChildren();
}

void GraphEditorPanel::paint (Graphics& g)
{
    g.fillAll (Colour(20, 20, 20));
}

void GraphEditorPanel::mouseDown (const MouseEvent& e)
{
	myDragger.startDraggingComponent (this, e);
	
	Array<File> cabbageFiles;
	int numNonNativePlugins;
	
    if (e.mods.isPopupMenu())
    {
        PopupMenu m;
		PopupMenu subMenu;
		subMenu.addItem(801, "Instrument");
		subMenu.addItem(802, "Effect");
		m.addSubMenu("New", subMenu);
		m.addSeparator();
	
        if (MainHostWindow* const mainWindow = findParentComponentOfClass<MainHostWindow>())
        {
	
			numNonNativePlugins = m.getNumItems();
			m.addSeparator();
            
			mainWindow->addCabbageNativePluginsToMenu(m, cabbageFiles);
	
			mainWindow->addPluginsToMenu (m);
			
            const int r = m.show();
            
			bool showNative = cUtils::getPreference(appProperties, "ShowNativeFileDialogues");
			wildcardFilter = WildcardFileFilter("*.csd", "*", ".csd Files");
			
            
			//file new instrument
			if(r==801)
			{
				Array<File> selectedFile = cUtils::launchFileBrowser("Save file as...", wildcardFilter, "*.csd", 0, File(""), showNative, &getLookAndFeel());
				if(selectedFile.size()>0)
				{				
					File csoundFile(selectedFile[0].withFileExtension(String(".csd")));
					csoundFile.replaceWithText(newFile("instrument", csoundFile.getFileNameWithoutExtension()));
					createNewPlugin (mainWindow->getChosenType (r), e.x, e.y, AUDIO_PLUGIN, csoundFile.getFullPathName());
				}
				return;
			}
			
			//file new effect
			if(r==802)
			{
				Array<File> selectedFile = cUtils::launchFileBrowser("Save file as...", wildcardFilter, "*.csd", 0, File(""), showNative, &getLookAndFeel());
				if(selectedFile.size()>0)
				{				
					File csoundFile(selectedFile[0].withFileExtension(String(".csd")));
					csoundFile.replaceWithText(newFile("effect", csoundFile.getFileNameWithoutExtension()));
					createNewPlugin (mainWindow->getChosenType (r), e.x, e.y, AUDIO_PLUGIN, csoundFile.getFullPathName());
				}
				return;
			}

			if(r>0) //make sure we have a valid item index....
			{
				if(r<cabbageFiles.size()+1)
				{
					createNewPlugin (mainWindow->getChosenType (r), e.x, e.y, true, cabbageFiles[r-1].getFullPathName());
					findParentComponentOfClass<GraphDocumentComponent>()->updatePluginsInSidebarPanel();
					return;
				}
				else 
				{
					if(r==10000)
					{
						createNewPlugin (mainWindow->getChosenType (r), e.x, e.y, true, "soundfile player");
					}
					if(r==10001)
					{
						createNewPlugin (mainWindow->getChosenType (r), e.x, e.y, true, "automation track");
					}
					else
					{
						createNewPlugin (mainWindow->getChosenType (r), e.x, e.y, false, "");
						findParentComponentOfClass<GraphDocumentComponent>()->updatePluginsInSidebarPanel();
						return;						
					}
				}
			}

        }
    }
    else
    {
		//deselect all grouped filters and set alpha to normal
		selectedFilters.deselectAll();
		for(int i=0; i<getNumChildComponents(); i++)
		{
			getChildComponent(i)->getProperties().set("selected", 0);
			getChildComponent(i)->repaint();
		}

        if((e.mods.getCurrentModifiers().isCtrlDown()))
		{
			addChildComponent (&lassoComp);
			lassoComp.beginLasso (e, this);
		}
    }


}

GraphDocumentComponent* GraphEditorPanel::getGraphDocument()
{
		return findParentComponentOfClass<GraphDocumentComponent>();
}
	
void GraphEditorPanel::mouseDrag (const MouseEvent& e)
{
	if(!e.mods.isCommandDown())
		myDragger.dragComponent (this, e, nullptr);
	else
	{
		lassoComp.toFront (false);
		lassoComp.dragLasso (e);
	}
}

void GraphEditorPanel::mouseUp (const MouseEvent& e)
{
    //if a selection has been made update the selected groups alpha setting to highlight selection
    for(int i=0; i<selectedFilters.getNumSelected(); i++) {
        selectedFilters.getSelectedItem(i)->getProperties().set("selected", 1);
        selectedFilters.getSelectedItem(i)->repaint();
    }

    lassoComp.endLasso();
    removeChildComponent (&lassoComp);
}

void GraphEditorPanel::findLassoItemsInArea (Array <FilterComponent*>& results, const Rectangle<int>& area)
{
    const Rectangle<int> lasso (area);

    for (int i = 0; i < getNumChildComponents()-1; i++)
    {
        FilterComponent* c = (FilterComponent*)getChildComponent(i);
        if (c->getBounds().intersects (lasso)) {
            results.addIfNotAlreadyThere(c);
            selectedFilters.addToSelection(c);
        }
        else
            selectedFilters.deselect(c);
    }
}

void GraphEditorPanel::createNewPlugin (const PluginDescription* desc, int x, int y, bool isNative, String fileName)
{
	if(isNative)
	{
		PluginDescription descript;
		if(fileName=="soundfile player")
		{
			descript.descriptiveName = "Soundfile player";
			descript.name = "SoundfilePlayer";
			descript.pluginFormatName = "SoundfilePlayer";
		}
		else if(fileName=="automation track")
		{
			descript.descriptiveName = "Automation track";
			descript.name = "AutomationTrack";
			descript.pluginFormatName = "AutomationTrack";
		}
		else
		{
			descript.fileOrIdentifier = fileName;
			descript.descriptiveName = "Cabbage Plugin "+File(fileName).getFileNameWithoutExtension();
			descript.name = File(fileName).getFileNameWithoutExtension();
			descript.manufacturerName = "CabbageAudio";
			descript.pluginFormatName = "Cabbage";		
		}

		descript.numInputChannels = 2;		
		descript.numOutputChannels = 2;
		graph.addFilter (&descript, x / (double) getWidth(), y / (double) getHeight());
	}
	else
	{
		graph.addFilter (desc, x / (double) getWidth(), y / (double) getHeight());
	}
}

FilterComponent* GraphEditorPanel::getComponentForFilter (const uint32 filterID) const
{
    for (int i = getNumChildComponents(); --i >= 0;)
    {
        if (FilterComponent* const fc = dynamic_cast <FilterComponent*> (getChildComponent (i)))
            if (fc->filterID == filterID)
                return fc;
    }

    return nullptr;
}

ConnectorComponent* GraphEditorPanel::getComponentForConnection (const AudioProcessorGraph::Connection& conn) const
{
    for (int i = getNumChildComponents(); --i >= 0;)
    {
        if (ConnectorComponent* const c = dynamic_cast <ConnectorComponent*> (getChildComponent (i)))
            if (c->sourceFilterID == conn.sourceNodeId
                    && c->destFilterID == conn.destNodeId
                    && c->sourceFilterChannel == conn.sourceChannelIndex
                    && c->destFilterChannel == conn.destChannelIndex)
                return c;
    }

    return nullptr;
}

PinComponent* GraphEditorPanel::findPinAt (const int x, const int y) const
{
    for (int i = getNumChildComponents(); --i >= 0;)
    {
        if (FilterComponent* fc = dynamic_cast <FilterComponent*> (getChildComponent (i)))
        {
            if (PinComponent* pin = dynamic_cast <PinComponent*> (fc->getComponentAt (x - fc->getX(),
                                    y - fc->getY())))
                return pin;
        }
    }

    return nullptr;
}

void GraphEditorPanel::resized()
{
    updateComponents();
}

void GraphEditorPanel::changeListenerCallback (ChangeBroadcaster*)
{
    updateComponents();
}

void GraphEditorPanel::actionListenerCallback (const String &message)
{

}

void GraphEditorPanel::updateNode (const int nodeID, const int inChannels, const int outChannels)
{
	
	ScopedPointer<XmlElement> temp(graph.createXml());
	const XmlElement tempXml (*temp);
	
	graph.removeFilter(nodeID);
	
	forEachXmlChildElementWithTagName (tempXml, e, "FILTER")
	{			
		if(nodeID==e->getIntAttribute("uid"))
		{
			double origX = e->getDoubleAttribute("x");
			double origY = e->getDoubleAttribute("y");
			e->setAttribute("x", origX);
			e->setAttribute("y", origY);
			cUtils::debug(e->getIntAttribute("uid"));
			e->setAttribute("numInputs", inChannels);
			e->setAttribute("numOutputs", outChannels); 
			graph.createNodeFromXml(*e);
			graph.changed();
		}
	}	

	forEachXmlChildElementWithTagName (tempXml, e, "CONNECTION")
	{
		if(e->getIntAttribute ("srcFilter")==nodeID || e->getIntAttribute ("dstFilter")==nodeID)
		{
			graph.addConnection((uint32) e->getIntAttribute ("srcFilter"),
								e->getIntAttribute ("srcChannel"),
								(uint32) e->getIntAttribute ("dstFilter"),
								e->getIntAttribute ("dstChannel"));
		}
	}	
	
}

void GraphEditorPanel::updateComponents()
{
    for (int i = getNumChildComponents(); --i >= 0;)
    {
        if (FilterComponent* const fc = dynamic_cast <FilterComponent*> (getChildComponent (i)))
            fc->update();
    }

    for (int i = getNumChildComponents(); --i >= 0;)
    {
        ConnectorComponent* const cc = dynamic_cast <ConnectorComponent*> (getChildComponent (i));

        if (cc != nullptr && cc != draggingConnector)
        {
            if (graph.getConnectionBetween (cc->sourceFilterID, cc->sourceFilterChannel,
                                            cc->destFilterID, cc->destFilterChannel) == nullptr)
            {
                delete cc;
            }
            else
            {
                cc->update();
            }
        }
    }

    for (int i = graph.getNumFilters(); --i >= 0;)
    {
        const AudioProcessorGraph::Node::Ptr f (graph.getNode (i));

        if (getComponentForFilter (f->nodeId) == 0)
        {
            FilterComponent* const comp = new FilterComponent (graph, f->nodeId);
            addAndMakeVisible (comp);
            comp->update();
        }
    }

    for (int i = graph.getNumConnections(); --i >= 0;)
    {
        const AudioProcessorGraph::Connection* const c = graph.getConnection (i);

        if (getComponentForConnection (*c) == 0)
        {
            ConnectorComponent* const comp = new ConnectorComponent (graph);
            addAndMakeVisible (comp);

            comp->setInput (c->sourceNodeId, c->sourceChannelIndex);
            comp->setOutput (c->destNodeId, c->destChannelIndex);
        }
    }
}


String GraphEditorPanel::newFile(String type, String caption)
{
	String csdText;
	if(type=="effect")
	{
		csdText= 
		"<Cabbage>\n"
		"form size(400, 300), caption(\"";
		csdText = csdText+caption+"\"), pluginID(\"plu1\")\n"
		"\n"
		"</Cabbage>\n"
		"<CsoundSynthesizer>\n"
		"<CsOptions>\n"
		"-n -d\n"
		"</CsOptions>\n"
		"<CsInstruments>\n"
		"sr = 44100\n"
		"ksmps = 64\n"
		"nchnls = 2\n"
		"0dbfs=1\n"
		"\n"
		"instr 1\n"
		"a1 inch 1\n"
		"a2 inch 2\n"
		"\n"
		"\n"
		"outs a1, a2\n"
		"endin\n"
		"\n"
		"</CsInstruments>  \n"
		"<CsScore>\n"
		"f1 0 1024 10 1\n"
		"i1 0 3600\n"
		"</CsScore>\n"
		"</CsoundSynthesizer>";
	}
	
	else if(type=="instrument")
	{
		csdText= 
		"<Cabbage>\n"
		"form size(400, 300), caption(\"";
		csdText = csdText+caption+"\"), pluginID(\"plu1\")\n"
		"keyboard bounds(10, 200, 390, 100)\n"
		"\n"

		"</Cabbage>\n"
		"<CsoundSynthesizer>\n"
		"<CsOptions>\n"
		"-n -d -+rtmidi=NULL -M0 --midi-key-cps=4 --midi-velocity-amp=5\n" 
		"</CsOptions>\n"
		"<CsInstruments>\n"
		"sr = 44100\n"
		"ksmps = 64\n"
		"nchnls = 2\n"
		"0dbfs=1\n"
		"\n"
		"instr 1\n"
		"a1 oscili p5, p4, 1\n"
		"outs a1, a1"
		"\n"
		"endin\n"
		"\n"
		"</CsInstruments>  \n"
		"<CsScore>\n"
		"f1 0 1024 10 1\n"
		"f0 3600\n"
		"</CsScore>\n"
		"</CsoundSynthesizer>";
	}
	return csdText;
}

void GraphEditorPanel::beginConnectorDrag (const uint32 sourceFilterID, const int sourceFilterChannel,
        const uint32 destFilterID, const int destFilterChannel,
        const MouseEvent& e)
{
    draggingConnector = dynamic_cast <ConnectorComponent*> (e.originalComponent);

    if (draggingConnector == nullptr)
        draggingConnector = new ConnectorComponent (graph);

    draggingConnector->setInput (sourceFilterID, sourceFilterChannel);
    draggingConnector->setOutput (destFilterID, destFilterChannel);

    addAndMakeVisible (draggingConnector);
    draggingConnector->toFront (false);

    dragConnector (e);
}

void GraphEditorPanel::dragConnector (const MouseEvent& e)
{
    const MouseEvent e2 (e.getEventRelativeTo (this));

    if (draggingConnector != nullptr)
    {
        draggingConnector->setTooltip (String::empty);

        int x = e2.x;
        int y = e2.y;

        if (PinComponent* const pin = findPinAt (x, y))
        {
            uint32 srcFilter = draggingConnector->sourceFilterID;
            int srcChannel   = draggingConnector->sourceFilterChannel;
            uint32 dstFilter = draggingConnector->destFilterID;
            int dstChannel   = draggingConnector->destFilterChannel;

            if (srcFilter == 0 && ! pin->isInput)
            {
                srcFilter = pin->filterID;
                srcChannel = pin->index;
            }
            else if (dstFilter == 0 && pin->isInput)
            {
                dstFilter = pin->filterID;
                dstChannel = pin->index;
            }

            if (graph.canConnect (srcFilter, srcChannel, dstFilter, dstChannel))
            {
                x = pin->getParentComponent()->getX() + pin->getX() + pin->getWidth() / 2;
                y = pin->getParentComponent()->getY() + pin->getY() + pin->getHeight() / 2;

                draggingConnector->setTooltip (pin->getTooltip());
            }
        }

        if (draggingConnector->sourceFilterID == 0)
            draggingConnector->dragStart (x, y);
        else
            draggingConnector->dragEnd (x, y);
    }
}

void GraphEditorPanel::endDraggingConnector (const MouseEvent& e)
{
    if (draggingConnector == nullptr)
        return;

    draggingConnector->setTooltip (String::empty);

    const MouseEvent e2 (e.getEventRelativeTo (this));

    uint32 srcFilter = draggingConnector->sourceFilterID;
    int srcChannel   = draggingConnector->sourceFilterChannel;
    uint32 dstFilter = draggingConnector->destFilterID;
    int dstChannel   = draggingConnector->destFilterChannel;

    draggingConnector = nullptr;

    if (PinComponent* const pin = findPinAt (e2.x, e2.y))
    {
        if (srcFilter == 0)
        {
            if (pin->isInput)
                return;

            srcFilter = pin->filterID;
            srcChannel = pin->index;
        }
        else
        {
            if (! pin->isInput)
                return;

            dstFilter = pin->filterID;
            dstChannel = pin->index;
        }

        graph.addConnection (srcFilter, srcChannel, dstFilter, dstChannel);
    }
}


//==============================================================================
class TooltipBar   : public Component,
    private Timer
{
public:
    TooltipBar()
    {
        startTimer (100);
    }

    void paint (Graphics& g)
    {
        g.setFont (Font (getHeight() * 0.7f, Font::bold));
        g.setColour (Colours::black);
        g.drawFittedText (tip, 10, 0, getWidth() - 12, getHeight(), Justification::centredLeft, 1);
    }

    void timerCallback()
    {
        Component* const underMouse = Desktop::getInstance().getMainMouseSource().getComponentUnderMouse();
        TooltipClient* const ttc = dynamic_cast <TooltipClient*> (underMouse);

        String newTip;

        if (ttc != nullptr && ! (underMouse->isMouseButtonDown() || underMouse->isCurrentlyBlockedByAnotherModalComponent()))
            newTip = ttc->getTooltip();

        if (newTip != tip)
        {
            tip = newTip;
            repaint();
        }
    }

private:
    String tip;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TooltipBar)
};

//==============================================================================
// graph audio player
//==============================================================================
GraphAudioProcessorPlayer::GraphAudioProcessorPlayer()
    : processor (nullptr),
      sampleRate (0),
      blockSize (0),
      isPrepared (false),
      numInputChans (0),
      numOutputChans (0),
	  actionCounter(0),
	  inputGainLevel(1.f),
	  outputGainLevel(1.f)
{
}

GraphAudioProcessorPlayer::~GraphAudioProcessorPlayer()
{
    setProcessor (nullptr);
}

//==============================================================================
void GraphAudioProcessorPlayer::setProcessor (AudioProcessor* const processorToPlay)
{
    if (processor != processorToPlay)
    {
        if (processorToPlay != nullptr && sampleRate > 0 && blockSize > 0)
        {
            processorToPlay->setPlayConfigDetails (numInputChans, numOutputChans, sampleRate, blockSize);
            processorToPlay->prepareToPlay (sampleRate, blockSize);
			
			for(int i=0;i<numInputChans;i++)
				inputChannelRMS.add(0.f);
						
			for(int i=0;i<numOutputChans;i++)
				outputChannelRMS.add(0.f);
        }

        AudioProcessor* oldOne;

        {
            const ScopedLock sl (lock);
            oldOne = isPrepared ? processor : nullptr;
            processor = processorToPlay;
            isPrepared = true;
        }

        if (oldOne != nullptr)
            oldOne->releaseResources();
    }
}

//simple and safe method to listen for gain changes...
void GraphAudioProcessorPlayer::changeListenerCallback (ChangeBroadcaster* source)
{
	const ScopedLock sl(getCurrentProcessor()->getCallbackLock());
	if(((InternalMixerStrip*)source)->isInput())
		inputGainLevel = ((InternalMixerStrip*)source)->currentGainLevel;
	else
		outputGainLevel = ((InternalMixerStrip*)source)->currentGainLevel;	
}
//==============================================================================
void GraphAudioProcessorPlayer::audioDeviceIOCallback (const float** const inputChannelData,
                                                  const int numInputChannels,
                                                  float** const outputChannelData,
                                                  const int numOutputChannels,
                                                  const int numSamples)
{
    // these should have been prepared by audioDeviceAboutToStart()...
    jassert (sampleRate > 0 && blockSize > 0);
	actionCounter++;
    incomingMidi.clear();
	float inputBuffer[numSamples];
	float outputBuffer[numSamples];
    messageCollector.removeNextBlockOfMessages (incomingMidi, numSamples);
    int totalNumChans = 0;

    if (numInputChannels > numOutputChannels)
    {
        // if there aren't enough output channels for the number of
        // inputs, we need to create some temporary extra ones (can't
        // use the input data in case it gets written to)
        tempBuffer.setSize (numInputChannels - numOutputChannels, numSamples,
                            false, false, true);

        for (int i = 0; i < numOutputChannels; ++i)
        {
            channels[totalNumChans] = outputChannelData[i];
            memcpy (channels[totalNumChans], inputChannelData[i], sizeof (float) * (size_t) numSamples);
            ++totalNumChans;
        }

        for (int i = numOutputChannels; i < numInputChannels; ++i)
        {
            channels[totalNumChans] = tempBuffer.getWritePointer (i - numOutputChannels);
            memcpy (channels[totalNumChans], inputChannelData[i], sizeof (float) * (size_t) numSamples);
            ++totalNumChans;
        }
    }
    else
    {
				
        for (int i = 0; i < numInputChannels; ++i)
        {
			//apply gain control on input
			for(int y=0;y<numSamples;y++)
				inputBuffer[y] = inputChannelData[i][y]*inputGainLevel;
			
            channels[totalNumChans] = outputChannelData[i];
			
            memcpy (channels[totalNumChans], inputBuffer, sizeof (float) * (size_t) numSamples);
			inputChannelRMS.getReference(i) = abs(inputBuffer[0]);
			
			for(int y=0;y<numSamples;y++)
			{
				channels[totalNumChans][y] = channels[totalNumChans][y]*.0;
			}
			
            ++totalNumChans;
        }

        for (int i = numInputChannels; i < numOutputChannels; ++i)
        {
            channels[totalNumChans] = outputChannelData[i];
            zeromem (channels[totalNumChans], sizeof (float) * (size_t) numSamples);
            ++totalNumChans;
        }
    }

	//put some breaks on the VU update speed. 
	if(actionCounter==5){
		actionCounter=0;
		sendChangeMessage();
	}
		
		
    AudioSampleBuffer buffer (channels, totalNumChans, numSamples);

    {
        const ScopedLock sl (lock);

        if (processor != nullptr)
        {
            const ScopedLock sl2 (processor->getCallbackLock());

            if (! processor->isSuspended())
            {
                processor->processBlock (buffer, incomingMidi);
				//apply gain control on output
				buffer.applyGain(outputGainLevel);
				for(int i=0;i<totalNumChans;i++)
					outputChannelRMS.getReference(i) = buffer.getRMSLevel(i, 0, numSamples);				
                return;
            }
        }
    }

    for (int i = 0; i < numOutputChannels; ++i)
        FloatVectorOperations::clear (outputChannelData[i], numSamples);
}

void GraphAudioProcessorPlayer::audioDeviceAboutToStart (AudioIODevice* const device)
{
    const double newSampleRate = device->getCurrentSampleRate();
    const int newBlockSize     = device->getCurrentBufferSizeSamples();
    const int numChansIn       = device->getActiveInputChannels().countNumberOfSetBits();
    const int numChansOut      = device->getActiveOutputChannels().countNumberOfSetBits();

    const ScopedLock sl (lock);

    sampleRate = newSampleRate;
    blockSize  = newBlockSize;
    numInputChans  = numChansIn;
    numOutputChans = numChansOut;

    messageCollector.reset (sampleRate);
    channels.calloc ((size_t) jmax (numChansIn, numChansOut) + 2);

    if (processor != nullptr)
    {
        if (isPrepared)
            processor->releaseResources();

        AudioProcessor* const oldProcessor = processor;
        setProcessor (nullptr);
        setProcessor (oldProcessor);
    }
}

void GraphAudioProcessorPlayer::audioDeviceStopped()
{
    const ScopedLock sl (lock);

    if (processor != nullptr && isPrepared)
        processor->releaseResources();

    sampleRate = 0.0;
    blockSize = 0;
    isPrepared = false;
    tempBuffer.setSize (1, 1);
}


//==============================================================================
// graphDocumentComponent. Holds out main GUI objects
GraphDocumentComponent::GraphDocumentComponent (AudioPluginFormatManager& formatManager,
        AudioDeviceManager* deviceManager_)
: graph (formatManager), 
deviceManager (deviceManager_),
midiLearnEnabled(false)
{
    addAndMakeVisible (graphPanel = new GraphEditorPanel (graph));

	addAndMakeVisible(sidebarPanel = new SidebarPanel(&graph));
	addAndMakeVisible(bottomPanel = new BottomPanel(&graph));

	sidebarPanel->addActionListener(bottomPanel);

    deviceManager->addChangeListener (graphPanel);

    graphPlayer.setProcessor (&graph.getGraph());
	
	graphPanel->setSize(6000, 6000);
	graphPanel->setTopLeftPosition(-2600,-2900);

    keyState.addListener (&graphPlayer.getMidiMessageCollector());

    addAndMakeVisible (keyboardComp = new MidiKeyboardComponent (keyState,
            MidiKeyboardComponent::horizontalKeyboard));

	keyboardComp->setColour(MidiKeyboardComponent::ColourIds::whiteNoteColourId, Colours::white.darker(.3f));
	keyboardComp->setColour(MidiKeyboardComponent::ColourIds::blackNoteColourId, Colours::green.darker(.9f));
	keyboardComp->setColour(MidiKeyboardComponent::ColourIds::upDownButtonArrowColourId, Colours::lime);
	keyboardComp->setColour(MidiKeyboardComponent::ColourIds::upDownButtonBackgroundColourId, Colour(30,30,30));
	
    addAndMakeVisible (statusBar = new TooltipBar());
    deviceManager->addAudioCallback (&graphPlayer);
    deviceManager->addMidiInputCallback (String::empty, &graphPlayer.getMidiMessageCollector());
	deviceManager->addMidiInputCallback (String::empty, this);

	//setup channel strips for inputs and outputs
	addAndMakeVisible (inputStrip = new InternalMixerStrip("Inputs", deviceManager->getCurrentAudioDevice()->getInputChannelNames().size()));
	addAndMakeVisible (outputStrip = new InternalMixerStrip("Outputs", deviceManager->getCurrentAudioDevice()->getOutputChannelNames().size()));
	
	graphPlayer.addChangeListener(inputStrip);
	graphPlayer.addChangeListener(outputStrip);
	inputStrip->addChangeListener(&graphPlayer);
	outputStrip->addChangeListener(&graphPlayer);
	
	showBottomPanel(false);
	
    graphPanel->updateComponents();
}

GraphDocumentComponent::~GraphDocumentComponent()
{
    deviceManager->removeAudioCallback (&graphPlayer);
    deviceManager->removeMidiInputCallback (String::empty, &graphPlayer.getMidiMessageCollector());
	deviceManager->removeMidiInputCallback (String::empty, this);
    deviceManager->removeChangeListener (graphPanel);

    deleteAllChildren();

    graphPlayer.setProcessor (nullptr);
    keyState.removeListener (&graphPlayer.getMidiMessageCollector());

    graph.clear();
}

void GraphDocumentComponent::resized()
{
    const int keysHeight = 60;
    const int statusHeight = 20;

   // statusBar->setBounds (0, getHeight() - keysHeight - statusHeight, getWidth(), statusHeight);
    keyboardComp->setBounds (250, getHeight() - keysHeight, getWidth()-450, keysHeight);
	inputStrip->setBounds(getWidth()-200, getHeight() - keysHeight, 200, 30);
	outputStrip->setBounds(getWidth()-200, getHeight() - keysHeight/2, 200, 30);
	sidebarPanel->setLookAndFeel(&getLookAndFeel());
	sidebarPanel->setBounds(0, 0, 250, getHeight());	
	bottomPanel->setBounds(250, getHeight()-260, getWidth()-250, 200);
}

void GraphDocumentComponent::createNewPlugin (const PluginDescription* desc, int x, int y, bool isNative, String filename)
{
    graphPanel->createNewPlugin (desc, x, y, isNative, filename);
}

void GraphDocumentComponent::addComponentToBottomPanel(Component* component)
{
	component->setSize(bottomPanel->getWidth(), bottomPanel->getHeight());
	component->setComponentID(component->getName());
	bottomPanel->addComponentToPanel(component);	
	showBottomPanel(true);
}

void GraphDocumentComponent::removeComponentFromBottomPanel(String compName)
{
	bottomPanel->removeComponentFromPanel(compName);	
}

void GraphDocumentComponent::showComponentInBottomPanel(String compName)
{
	bottomPanel->showComponentInPanel(compName);	
}

void GraphDocumentComponent::removeAllComponentsFromBottomPanel()
{
	bottomPanel->removeAllComponentsFromPanel();
}
void GraphDocumentComponent::showSidebarPanel(bool show)
{
	if(show)
	{
		sidebarPanel->setVisible(true);
		if(bottomPanel->isVisible())
			bottomPanel->setBounds(sidebarPanel->getWidth(), bottomPanel->getPosition().getY(), getWidth()-sidebarPanel->getWidth(), bottomPanel->getHeight());
		if(keyboardComp->isVisible())
			keyboardComp->setBounds(sidebarPanel->getWidth()+5, keyboardComp->getPosition().getY(), getWidth()-sidebarPanel->getWidth()-10, keyboardComp->getHeight());		
	}
	else
	{
		sidebarPanel->setVisible(false);
		if(bottomPanel->isVisible())
			bottomPanel->setBounds(0, bottomPanel->getPosition().getY(), getWidth(), bottomPanel->getHeight());
		if(keyboardComp->isVisible())
			keyboardComp->setBounds(0, keyboardComp->getPosition().getY(), getWidth(), keyboardComp->getHeight());
	}
}

void GraphDocumentComponent::showBottomPanel(bool show)
{
	if(show)
		bottomPanel->setVisible(true);
	else
		bottomPanel->setVisible(false);
}

void GraphDocumentComponent::handleIncomingMidiMessage (MidiInput *source, const MidiMessage &message)
{
	if(message.isController()){
		//if MIDI learn is turned on
		if(midiLearnEnabled){
			int nodeId = graph.getLastMovedNodeId();
			int parameterIndex = graph.getLastMovedNodeParameterIndex();
			
			for(int i=0;i<graph.midiMappings.size();i++)
			{
				if(doMidiMappingsMatch(i, message.getChannel(), message.getControllerNumber()))
				{
					graph.midiMappings.getReference(i).nodeId = nodeId;
					graph.midiMappings.getReference(i).parameterIndex = parameterIndex;
					
					if(graph.getGraph().getNodeForId(nodeId))
					{
						graph.getGraph().getNodeForId(nodeId)->getProcessor()->setParameterNotifyingHost(parameterIndex, message.getControllerValue()/127.f);
						graph.getGraph().getNodeForId(nodeId)->getProcessor()->setParameter(parameterIndex, message.getControllerValue()/127.f);
						//Logger::writeToLog("exists");
						addNewMapping=false;
					}
				}
			}		
			
			if(addNewMapping)
			{
				if(nodeId>0)
					graph.midiMappings.add(CabbageMidiMapping(nodeId, parameterIndex, message.getChannel(), message.getControllerNumber()));
			}
		}
		else
		{ //mid learn disabled
			for(int i=0;i<graph.midiMappings.size();i++)
			{
				if(doMidiMappingsMatch(i, message.getChannel(), message.getControllerNumber()))
				{
					if(graph.getGraph().getNodeForId(graph.midiMappings.getReference(i).nodeId))
					{
						graph.getGraph().getNodeForId(graph.midiMappings.getReference(i).nodeId)->getProcessor()->setParameterNotifyingHost(graph.midiMappings.getReference(i).parameterIndex, message.getControllerValue()/127.f);
						graph.getGraph().getNodeForId(graph.midiMappings.getReference(i).nodeId)->getProcessor()->setParameter(graph.midiMappings.getReference(i).parameterIndex, message.getControllerValue()/127.f);
					}
				}		
					
			}
		}
		
	}
addNewMapping=true;
}

bool GraphDocumentComponent::doMidiMappingsMatch(int i, int channel, int controller)
{
	if((graph.midiMappings.getReference(i).channel==channel)&&(graph.midiMappings.getReference(i).controller==controller))
		return true;
	else 
		return false;
}