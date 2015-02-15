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

#include "../JuceLibraryCode/JuceHeader.h"
#include "FilterGraph.h"
#include "SidebarPanel.h"
#include "../Plugin/CabbageGenericAudioProcessorEditor.h"
#include "../CabbageGUIClass.h"


//==============================================================================
SidebarPanel::SidebarPanel(FilterGraph* graph) : 
filterGraph(graph),
previousFilterNodeId(-99),
thread ("file preview"),
directoryList (nullptr, thread),
fileTreeComp (*this, "", directoryList),
canResize(false),
transportControls(*this, ""),
midiBubble(250)
{
	setOpaque (true);
	addAndMakeVisible (concertinaPanel);	
	
    midiBubble.setColour(BubbleComponent::backgroundColourId, Colours::white);
    midiBubble.setBounds(0, 0, 50, 20);
    addChildComponent(midiBubble);
    midiBubble.setAlwaysOnTop(true);	
	
	directoryList.setDirectory (File::getSpecialLocation (File::userHomeDirectory), true, true);
	thread.startThread (3);

	fileTreeComp.setText(File::getSpecialLocation (File::userHomeDirectory).getFileNameWithoutExtension());

	fileTreeComp.fileComp.addListener (this);	
	
	filterGraph->addActionListener(this);
	
	PropertyPanel* transportPanel = new PropertyPanel ("Transport Controls");
	Array <PropertyComponent*> transport;
	transport.add(&transportControls);
	transportPanel->addProperties(transport);		
	concertinaPanel.addPanel(TRANSPORT_CONTROLS, transportPanel, false);
	concertinaPanel.setMaximumPanelSize(concertinaPanel.getPanel(TRANSPORT_CONTROLS), 70);
	
	PropertyPanel* panel = new PropertyPanel ("Plugins");

	Array <PropertyComponent*> params;

	addPluginPanel (panel);

	
	PropertyPanel* filePanel = new PropertyPanel ("File Browser");
	Array <PropertyComponent*> singleParams;
	singleParams.add(&fileTreeComp);
	filePanel->addProperties(singleParams);	
	
	concertinaPanel.addPanel(FILE_BROWSER, filePanel, false);
	concertinaPanel.expandPanelFully(concertinaPanel.getPanel(TRANSPORT_CONTROLS), true);
	
}
//=================================================================
SidebarPanel::~SidebarPanel()
{	
	fileTreeComp.fileComp.removeListener(this);
}

//=================================================================
void SidebarPanel::updatePluginParameters()
{	
	PropertyPanel* panel = (PropertyPanel*)concertinaPanel.getPanel(PLUGIN_PARAMS);
	//panel->clear();

	Array <PropertyComponent*> params;
	
	int index = filterGraph->getNumFilters()-1;

	int numParams = filterGraph->getNode(index)->getProcessor()->getNumParameters();
	params.clear();
	for (int i = 0; i < numParams; ++i)
	{
		String name (filterGraph->getNode(index)->getProcessor()->getParameterName (i));
		if (name.trim().isEmpty())
			name = "Unnamed";

		ProcessorParameterPropertyComp* const pc = new ProcessorParameterPropertyComp(name, 
																					  *filterGraph->getNode(index)->getProcessor(),
																					  i,
																					  filterGraph->getNode(index)->nodeId);
		pc->addChangeListener(this);
		params.add (pc);
	}

	panel->addSection(filterGraph->getNode(index)->getProcessor()->getName(), params, false);


	for(int y=0;y<panel->getSectionNames().size()-1;y++)
		panel->setSectionOpen(y, false, true);
		
	panel->setSectionOpen(panel->getSectionNames().size()-1, true, true);

	concertinaPanel.expandPanelFully(concertinaPanel.getPanel(PLUGIN_PARAMS), true);

}

//=================================================================
void SidebarPanel::refreshPluginParameters()
{	
	PropertyPanel* panel = (PropertyPanel*)concertinaPanel.getPanel(PLUGIN_PARAMS);
	panel->clear();

	Array <PropertyComponent*> params;
	
	const int numPanels = concertinaPanel.getNumPanels();

	for(int index=0;index<filterGraph->getNumFilters();index++)
	{ 
		int numParams = filterGraph->getNode(index)->getProcessor()->getNumParameters();
		const AudioProcessorGraph::Node::Ptr f = filterGraph->getNode(index);
		params.clear();
		
		for (int i = 0; i < numParams; ++i)
		{
			String name (filterGraph->getNode(index)->getProcessor()->getParameterName (i));
			if (name.trim().isEmpty())
				name = "Unnamed";

			ProcessorParameterPropertyComp* const pc = new ProcessorParameterPropertyComp(name, 
																						  *filterGraph->getNode(index)->getProcessor(),
																						  i,
																						  filterGraph->getNode(index)->nodeId);
			pc->addChangeListener(this);
			params.add (pc);
		}
		
		if(f->properties.getWithDefault("pluginType", "")!="Internal")
			panel->addSection(filterGraph->getNode(index)->getProcessor()->getName(), params, false);	
	}

	
	concertinaPanel.expandPanelFully(concertinaPanel.getPanel(PLUGIN_PARAMS), true);	
}
//=================================================================
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
	
	if(PropertyPanel* propPanel = (PropertyPanel*)concertinaPanel.getPanel(PLUGIN_PARAMS))
	{
		for(int i=0;i<propPanel->getSectionNames().size();i++)
			propPanel->setSectionOpen(i, false, false);
					
		propPanel->setSectionOpen(indexOfNode, true, true);
		
	}
			
}

//--------------------------------------------------------------------
//move the file browser root to a parent directory
//--------------------------------------------------------------------
void SidebarPanel::upButtonPressed()
{
	directoryList.setDirectory(directoryList.getDirectory().getParentDirectory(), true, true);
	fileTreeComp.setText(directoryList.getDirectory().getFileNameWithoutExtension());
}

//--------------------------------------------------------------------
// transport methods
//--------------------------------------------------------------------
void SidebarPanel::stopButtonPressed()
{
	filterGraph->setIsPlaying(false, true);
	transportControls.setTimeIsRunning(false);
	transportControls.setTimeLabel("00 : 00 : 00");
	transportControls.setBeatsLabel("Bar 1 : 1");
	repaint();
	stopTimer();
}

void SidebarPanel::pauseButtonPressed()
{
	filterGraph->setIsPlaying(false);
	transportControls.setTimeIsRunning(false);
	transportControls.repaint();
	stopTimer();
}

void SidebarPanel::playButtonPressed()
{
	filterGraph->setIsPlaying(true);
	transportControls.setTimeIsRunning(true);
	startTimer(10);
}


void SidebarPanel::setCurrentBPM(int bpm)
{
	filterGraph->setBPM(bpm);
}
//--------------------------------------------------------------------
void SidebarPanel::paint (Graphics& g)
{
	g.fillAll (Colour::greyLevel (0.2f));
	
	if(midiLearn)
	{
		g.setColour(Colours::yellow);
		g.drawRect(getLocalBounds().reduced(.8), 4.f);		
	}		
}

//--------------------------------------------------------------------
//called when a users presses the play button
//--------------------------------------------------------------------
void SidebarPanel::toggleMIDILearn()
{
	midiLearn=!midiLearn;
	
	if(midiLearn)
		cUtils::showMessage("To assign MIDI controllers to plugin parameters\nfirst select the parameter, then move the MIDI controller.");
	
	repaint();
}
//--------------------------------------------------------------------
void SidebarPanel::changeListenerCallback (ChangeBroadcaster* source)
{
	if(midiLearn)
	{
		ProcessorParameterPropertyComp* comp = (ProcessorParameterPropertyComp*)source;
		String text = filterGraph->findControllerForparameter(comp->getNodeId(), comp->getParamIndex());
		if(text.isEmpty())
			text = "Unassigned";
			
		midiBubble.showAt(comp, AttributedString(text), 1000);
	}
}

void SidebarPanel::actionListenerCallback (const String &message)
{
	if(concertinaPanel.getPanel(WIDGET_PROPS))
	{
		((CabbagePropertiesPanel*)concertinaPanel.getPanel(WIDGET_PROPS))->removeAllChangeListeners();
		concertinaPanel.removePanel(concertinaPanel.getPanel(WIDGET_PROPS));
	}
				
	CabbageGUIClass cAttr(message, -1);
	CabbagePropertiesPanel* panel = new CabbagePropertiesPanel("Widget Properties");
	panel->updateProperties(cAttr);	
	panel->addChangeListener(filterGraph);
	
	concertinaPanel.addPanel(WIDGET_PROPS, panel, true);
	concertinaPanel.expandPanelFully(concertinaPanel.getPanel(WIDGET_PROPS), true);
}
//--------------------------------------------------------------------
void SidebarPanel::resized()
{
	fileTreeComp.setPreferredHeight(getLocalBounds().reduced(4).getHeight()-40);
	concertinaPanel.setBounds (getLocalBounds().reduced (4));
}

void SidebarPanel::updateWidgetProperties()
{
	CabbagePropertiesPanel* panel = (CabbagePropertiesPanel*)concertinaPanel.getPanel(WIDGET_PROPS);
	panel->sendChangeMessage();
}
//--------------------------------------------------------------------
void SidebarPanel::timerCallback()
{
	const int ellapsedTime = filterGraph->getTimeInSeconds();	
	const int hours = (ellapsedTime / 60 / 60) % 24;
	const int minutes = (ellapsedTime / 60) % 60;
	const int seconds = ellapsedTime % 60;		
	String time = String::formatted("%02d", hours)+" : "+String::formatted("%02d", minutes)+" : "+String::formatted("%02d", seconds);
	transportControls.setTimeLabel(time);

	const int elaspsedQNs = filterGraph->getPPQPosition();
	const int bars = (elaspsedQNs/4);
	const int beats = elaspsedQNs%4;
	
	String ppqPos = "Bar "+String(bars+1)+":"+String(beats+1);	
	transportControls.setBeatsLabel(String(ppqPos));

}

//--------------------------------------------------------------------
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
//--------------------------------------------------------------------
void SidebarPanel::mouseDrag(const MouseEvent& event)
{
	if(canResize)
	{
		this->setSize(event.getPosition().getX(), this->getHeight());
		sendActionMessage("SidebarWidth:"+String(event.getPosition().getX()));
	}
}
//--------------------------------------------------------------------
void SidebarPanel::mouseUp(const MouseEvent& event)
{
	canResize = false;
}
//--------------------------------------------------------------------
void SidebarPanel::addPluginPanel (PropertyPanel* panel)
{
	concertinaPanel.addPanel(PLUGIN_PARAMS, panel, false);
	concertinaPanel.setPanelHeaderSize (panel, 20);
	concertinaPanel.expandPanelFully(concertinaPanel.getPanel(PLUGIN_PARAMS), true);
}
//--------------------------------------------------------------------
void SidebarPanel::selectionChanged()
{
	//cUtils::debug(fileTreeComp.fileComp.getSelectedFile().getFullPathName());
}

//==============================================================================
//File browser property
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

//==============================================================================
//transport controls
//==============================================================================
TransportComponent::TransportComponent(SidebarPanel &ownerPanel, String name):
owner(ownerPanel),
PropertyComponent(name, 70),
lookAndFeel(),
playButton("playButton", DrawableButton::ImageOnButtonBackground),
stopButton("stopButton", DrawableButton::ImageOnButtonBackground),
bpmSlider("bpmSlider"),
timeLabel("TimeLabel"),
beatsLabel("beatsLabel"),
timingInfoBox(),
standardLookAndFeel(new LookAndFeel_V2())
{
	bpmSlider.setSliderStyle(Slider::SliderStyle::LinearBarVertical);
	bpmSlider.setRange(1, 500, 1);
	bpmSlider.setValue(60, dontSendNotification);
	bpmSlider.addListener(this);
	bpmSlider.setVelocityBasedMode(true);
	bpmSlider.setColour(Slider::ColourIds::thumbColourId, Colours::black);
	bpmSlider.setColour(Slider::ColourIds::textBoxTextColourId, Colours::white);
	bpmSlider.setColour(Slider::ColourIds::textBoxBackgroundColourId, Colours::black);
	
	timeSigNum.setSliderStyle(Slider::SliderStyle::LinearBarVertical);
	timeSigNum.setRange(1, 24, 1);
	timeSigNum.setValue(4, dontSendNotification);
	timeSigNum.addListener(this);
	timeSigNum.setColour(Slider::ColourIds::thumbColourId, Colours::black);
	timeSigNum.setColour(Slider::ColourIds::textBoxTextColourId, Colours::white);
	timeSigNum.setColour(Slider::ColourIds::textBoxBackgroundColourId, Colours::black);
		
	timeSigDen.setSliderStyle(Slider::SliderStyle::LinearBarVertical);
	timeSigDen.setRange(1, 24, 1);
	timeSigDen.setValue(4, dontSendNotification);
	timeSigDen.addListener(this);
	timeSigDen.setColour(Slider::ColourIds::thumbColourId, Colours::black);
	timeSigDen.setColour(Slider::ColourIds::textBoxTextColourId, Colours::white);
	timeSigDen.setColour(Slider::ColourIds::textBoxBackgroundColourId, Colours::black);
	
	bpmSlider.setVelocityModeParameters(0.9);
	bpmSlider.setTextValueSuffix(" BPM");
	
	timeLabel.setJustificationType(Justification::left);
	timeLabel.setFont(Font(24, 1));
	timeLabel.setLookAndFeel(standardLookAndFeel);
	timeLabel.setColour(Label::backgroundColourId, Colours::black);
	timeLabel.setColour(Label::textColourId, Colours::cornflowerblue);
	timeLabel.setText("00 : 00 : 00", dontSendNotification);
	
	beatsLabel.setJustificationType(Justification::right);
	beatsLabel.setFont(Font(18, 1));
	beatsLabel.setLookAndFeel(standardLookAndFeel);
	beatsLabel.setColour(Label::backgroundColourId, Colours::black);
	beatsLabel.setColour(Label::textColourId, Colours::cornflowerblue);
	beatsLabel.setText("Bar 1:1", dontSendNotification);
		
	
	playButton.setLookAndFeel(&lookAndFeel);	
	playButton.setColour(TextButton::buttonColourId, Colours::white);
	playButton.setColour(TextButton::buttonOnColourId, Colours::yellow);
	playButton.setClickingTogglesState(true);
	
	stopButton.setLookAndFeel(&lookAndFeel);	
	stopButton.setColour(TextButton::buttonColourId, Colours::white);	
	
	Path playPath;
	playPath.addTriangle(0, 0, BUTTON_SIZE, BUTTON_SIZE/2, 0, BUTTON_SIZE);
	DrawablePath playImage;
	playImage.setFill(Colours::green.darker(.9f));
	playImage.setPath(playPath);
	
	Path pausePath;
	pausePath.addRectangle(0, 0, BUTTON_SIZE*.4, BUTTON_SIZE);
	pausePath.addRectangle(BUTTON_SIZE*.5, 0, BUTTON_SIZE*.4, BUTTON_SIZE);
	DrawablePath pauseImage;
	pauseImage.setFill(Colours::green.darker(.9f));
	pauseImage.setPath(pausePath);
	
	playButton.setImages(&playImage, &playImage, &pauseImage, &playImage, &pauseImage);

	Path stopPath;
	stopPath.addRectangle(0, 0, BUTTON_SIZE, BUTTON_SIZE);
	DrawablePath stopImage;
	stopImage.setFill(Colours::green.darker(.9f));
	stopImage.setPath(stopPath);
	stopButton.setImages(&stopImage);	
	
	addAndMakeVisible (timingInfoBox);
	addAndMakeVisible (playButton);
	addAndMakeVisible (stopButton);
	addAndMakeVisible (bpmSlider);
	addAndMakeVisible (bpmLabel);
	addAndMakeVisible (timeLabel);
	addAndMakeVisible (beatsLabel);
	addAndMakeVisible (timeSigNum);
	addAndMakeVisible (timeSigDen);
	
	
	playButton.addListener (this);	
	stopButton.addListener (this);	
}

void TransportComponent::sliderValueChanged(Slider* slider)
{
	//owner.setCurrentBPM(slider->getValue());
}

void TransportComponent::sliderDragEnded(Slider* slider)
{
	owner.setCurrentBPM(slider->getValue());
}

void TransportComponent::resized()
{
	stopButton.setBounds(5, 5, BUTTON_SIZE, BUTTON_SIZE);
	playButton.setBounds(BUTTON_SIZE+10, 5, BUTTON_SIZE, BUTTON_SIZE);
	timeLabel.setBounds(BUTTON_SIZE*2+20, 5, getWidth()-(BUTTON_SIZE*2+45), 30);
	beatsLabel.setBounds(BUTTON_SIZE*2+75, 30, getWidth()-(BUTTON_SIZE*2+100), 20);
	timeSigNum.setBounds(BUTTON_SIZE*2+25, 35, 25, 15);
	timeSigDen.setBounds(BUTTON_SIZE*2+50, 35, 25, 15);
	
	bpmSlider.setBounds(8, BUTTON_SIZE+8, BUTTON_SIZE*2, 20);
	
	Path timeBox;
	timeBox.addRoundedRectangle(BUTTON_SIZE*2+15, 5, getWidth()-(BUTTON_SIZE*2+30), 50, 5);
	timingInfoBox.setPath(timeBox);
	timingInfoBox.setFill(Colours::black);
	
	timingInfoBox.setBounds(BUTTON_SIZE*2+15, 5, getWidth()-(BUTTON_SIZE*2+30), 50);
	
}

void TransportComponent::buttonClicked (Button* button)
{
	if(button->getName()=="stopButton")
	{
		playButton.setToggleState(false, sendNotification);
		owner.stopButtonPressed();
	}
	else if(button->getName()=="playButton")
	{
		if(button->getToggleState())
			owner.playButtonPressed();
		else
			owner.pauseButtonPressed();
	}
}