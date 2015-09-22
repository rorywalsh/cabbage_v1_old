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

#include "AutomationProcessor.h"
#include "AutomationEditor.h"

#define BUTTON_SIZE 25
AutomationDisplay::AutomationDisplay(AutomationEditor* editor):
    scrollbar(false),
    currentPlayPosition(0),
    owner(editor)
{
    totalLength = 10.f;
    tableColour = Colours::lime;
    sampleRate = 44100;

    currentPositionMarker.setFill (Colours::lime);
    addAndMakeVisible(currentPositionMarker);
    addAndMakeVisible (scrollbar);

    scrollbar.setAutoHide (false);
    scrollbar.addListener (this);
    setOpaque(true);
    resized();
    setSize(500, 200);

    const Range<double> newRange (0.0, totalLength);
    scrollbar.setRangeLimits (newRange);
    setRange (newRange);

}

AutomationDisplay::~AutomationDisplay()
{
    stopTimer();
    for(int i=0; i<automationEnvelopes.size(); i++)
        automationEnvelopes[i]->removeAllChangeListeners();
}


void AutomationDisplay::addNewAutomationEnvelope(Colour envColour, int tableNumber)
{

    automationEnvelopes.add(new BreakpointEnvelope(envColour, automationEnvelopes.size(), tableNumber));
    automationEnvelopes[automationEnvelopes.size()-1]->createEnvStartEndPoint(0);
    automationEnvelopes[automationEnvelopes.size()-1]->addChangeListener(owner->getFilter());
    addAndMakeVisible(automationEnvelopes[automationEnvelopes.size()-1]);
    setZoomFactor(0);
    resized();
}

void AutomationDisplay::bringEnvelopeToFront(int index)
{
    for(int i=0; i<automationEnvelopes.size(); i++)
        automationEnvelopes[i]->setAlpha(.3f);

    automationEnvelopes[index]->setAlpha(1.f);
    automationEnvelopes[index]->toFront(true);
}

void AutomationDisplay::resized()
{
    scrollbar.setBounds (getLocalBounds().removeFromBottom (18).reduced (2));
    for(int i=0; i<automationEnvelopes.size(); i++)
        automationEnvelopes[i]->setSize(getWidth(), getHeight()-18);
}

void AutomationDisplay::setScrubberPos(double pos)
{
    currentPositionMarker.setVisible (true);
    //pos = (pos/(thumbnail.getTotalLength()*sampleRate))*thumbnail.getTotalLength();
    currentPositionMarker.setRectangle (Rectangle<float> (timeToX (pos) - 0.75f, 0,
                                        1.5f, (float) (getHeight() - (scrollbar.getHeight()+5))));
}

void AutomationDisplay::changeListenerCallback (ChangeBroadcaster*)
{
    repaint();
}

void AutomationDisplay::setZoomFactor (double amount)
{
    if (totalLength > 0)
    {
        const double newScale = jmax (0.001, totalLength * (1.0 - jlimit (0.0, 0.99, amount)));
        const double timeAtCentre = xToTime (getWidth() / 2.0f);
        setRange (Range<double> (timeAtCentre - newScale * 0.5, timeAtCentre + newScale * 0.5));
    }
}

void AutomationDisplay::setRange (Range<double> newRange)
{
    visibleRange = newRange;
    scrollbar.setCurrentRange (visibleRange);

    const double visibleStart = visibleRange.getStart();
    const double visibleEnd = visibleRange.getEnd();
    const double visibleLength = visibleRange.getLength();

    const double newWidth = double(getWidth())*(double(totalLength)/visibleLength);
    const double leftOffset = newWidth*(visibleStart/(double)totalLength);


    for(int i=0; i<automationEnvelopes.size(); i++)
    {

        automationEnvelopes[i]->setSize(newWidth, automationEnvelopes[i]->getHeight());
        automationEnvelopes[i]->setTopLeftPosition(-leftOffset, 0);

    }


    repaint();
}

void AutomationDisplay::paint (Graphics& g)
{
    g.fillAll (Colour(10, 10, 10));
    g.setColour (tableColour);
}

void AutomationDisplay::timerCallback()
{
    if(totalLength>0)
    {
        currentPlayPosition = owner->getFilter()->getScrubberPosition();
        //setScrubberPos(currentPlayPosition);
    }
}

void AutomationDisplay::mouseDown (const MouseEvent& e)
{
    if(totalLength>0)
    {
        currentPlayPosition = jmax (0.0, xToTime ((float) e.x));
        setScrubberPos(currentPlayPosition);
    }
}

void AutomationDisplay::mouseDrag (const MouseEvent& e)
{
    if(totalLength>0)
    {
        currentPlayPosition = jmax (0.0, xToTime ((float) e.x));
        setScrubberPos(currentPlayPosition);
    }
}

void AutomationDisplay::resetPlaybackPosition()
{
    setScrubberPos(0);
}

float AutomationDisplay::timeToX (const double time) const
{
    return getWidth() * (float) ((time - visibleRange.getStart()) / (visibleRange.getLength()));
}

double AutomationDisplay::xToTime (const float x) const
{
    return (x / getWidth()) * (visibleRange.getLength()) + visibleRange.getStart();
}

void AutomationDisplay::scrollBarMoved (ScrollBar* scrollBarThatHasMoved, double newRangeStart)
{
    if (scrollBarThatHasMoved == &scrollbar)
        setRange (visibleRange.movedToStartAt (newRangeStart));
}



//==============================================================================
AutomationEditor::AutomationEditor (AutomationProcessor& p):
    AudioProcessorEditor(&p),
    processor(p),
    autoCombo(),
    zoom(0),
    basicLook(),
    playButton("playButton", DrawableButton::ImageOnButtonBackground),
    stopButton("stopButton", DrawableButton::ImageOnButtonBackground),
    zoomInButton("zoomInButton", DrawableButton::ImageOnButtonBackground),
    loopButton("loopButton", DrawableButton::ImageOnButtonBackground),
    zoomOutButton("zoomOutButton", DrawableButton::ImageOnButtonBackground),
    linkToTransport("linkToTransportButton", DrawableButton::ImageOnButtonBackground),
    automationDisplay(this)
{

    addAndMakeVisible(automationDisplay);
    //automationDisplay.envelope.createGainEnvStartEndPoint();

//	if(processor.getEnvPointSize()>0)
//	{
//		for(int i=0;i<processor.getEnvPointSize();i++)
//			waveformDisplay->gainEnvelope.addHandle(getFilter()->getEnvPoint(i), false);
//	}
//	else
    addAndMakeVisible(autoCombo);
    autoCombo.addListener(this);
    setOpaque(false);

    playButton.addListener(this);
    addAndMakeVisible(playButton);
    stopButton.addListener(this);
    addAndMakeVisible(stopButton);
    zoomInButton.addListener(this);
    addAndMakeVisible(zoomInButton);
    zoomOutButton.addListener(this);
    addAndMakeVisible(zoomOutButton);
    linkToTransport.addListener(this);
    addAndMakeVisible(linkToTransport);
    loopButton.addListener(this);
    addAndMakeVisible(loopButton);

    playButton.setLookAndFeel(&basicLook);
    stopButton.setLookAndFeel(&basicLook);
    zoomOutButton.setLookAndFeel(&basicLook);
    zoomInButton.setLookAndFeel(&basicLook);
    linkToTransport.setLookAndFeel(&basicLook);
    loopButton.setLookAndFeel(&basicLook);

    zoomOutButton.getProperties().set("isRounded", true);
    zoomInButton.getProperties().set("isRounded", true);

    playButton.setColour(TextButton::buttonColourId, Colours::white);
    playButton.setColour(TextButton::buttonColourId, Colours::white);
    loopButton.setColour(TextButton::buttonColourId, Colours::white);
    loopButton.setColour(TextButton::buttonOnColourId, Colours::cornflowerblue);

    playButton.setColour(TextButton::buttonOnColourId, Colours::yellow);
    zoomOutButton.setColour(TextButton::buttonColourId, Colours::white);
    zoomInButton.setColour(TextButton::buttonColourId, Colours::white);
    linkToTransport.setColour(TextButton::buttonColourId, Colours::green.darker(.9f));
    linkToTransport.setColour(TextButton::buttonOnColourId, Colours::green.darker(.9f));

    playButton.setClickingTogglesState(true);
    loopButton.setClickingTogglesState(true);

    linkToTransport.setColour(TextButton::buttonOnColourId, Colours::cornflowerblue);



    stopButton.setColour(TextButton::buttonColourId, Colours::white);

    linkToTransport.setImages(cUtils::createPlayButtonPath(25, Colours::white));

    playButton.setImages(cUtils::createPlayButtonPath(25, Colours::green.darker(.9f)),
                         cUtils::createPlayButtonPath(25, Colours::green.darker(.9f)),
                         cUtils::createPauseButtonPath(25),
                         cUtils::createPlayButtonPath(25, Colours::green.darker(.9f)),
                         cUtils::createPauseButtonPath(25),
                         cUtils::createPauseButtonPath(25));


    stopButton.setImages(cUtils::createStopButtonPath(25, Colours::green.darker(.9f)),
                         cUtils::createStopButtonPath(25, Colours::green.darker(.9f)),
                         cUtils::createStopButtonPath(25, Colours::green.darker(.9f)),
                         cUtils::createStopButtonPath(25, Colours::green.darker(.9f)),
                         cUtils::createStopButtonPath(25, Colours::green.darker(.9f)),
                         cUtils::createStopButtonPath(25, Colours::green.darker(.9f)),
                         cUtils::createStopButtonPath(25, Colours::green.darker(.9f))
                        );


    loopButton.setImages(cUtils::createLoopButtonPath());


    zoomInButton.setImages(cUtils::createZoomInButtonPath(25));
    zoomOutButton.setImages(cUtils::createZoomOutButtonPath(25));

    ampRange.add(0);
    ampRange.add(1);
    ampRange.add(-1);

    updateComboBoxItems();
    addTablesOnStartup();

    setSize (200, 400);
}

AutomationEditor::~AutomationEditor()
{
    processor.editorBeingDeleted(this);
}

//==============================================================================
void AutomationEditor::paint (Graphics& g)
{
    g.fillAll (Colours::black);
}

void AutomationEditor::resized()
{
    automationDisplay.setBounds(BUTTON_SIZE+7, 5, getWidth()-(BUTTON_SIZE+12), getHeight()-40);
    //tableManager.setBounds(10, 10, getWidth()-5-, getHeight()-50);
    //viewport->setBounds(BUTTON_SIZE+7, 5, getWidth()-20, getHeight()-10);

    playButton.setBounds(3, 5, BUTTON_SIZE, BUTTON_SIZE);
    stopButton.setBounds(3, ((BUTTON_SIZE)*1)+5, BUTTON_SIZE, BUTTON_SIZE);
    loopButton.setBounds(3, ((BUTTON_SIZE)*2)+5, BUTTON_SIZE, BUTTON_SIZE);
    linkToTransport.setBounds(3, ((BUTTON_SIZE)*3)+5, BUTTON_SIZE, BUTTON_SIZE);
    zoomInButton.setBounds(3, ((BUTTON_SIZE)*4)+5, BUTTON_SIZE, BUTTON_SIZE);
    zoomOutButton.setBounds(3, ((BUTTON_SIZE)*5)+5, BUTTON_SIZE, BUTTON_SIZE);

    autoCombo.setBounds(BUTTON_SIZE+7, getHeight()-35, BUTTON_SIZE*12, BUTTON_SIZE);
}
//==============================================================================
void AutomationEditor::updateComboBoxItems()
{
    autoCombo.clear();
    for(int i=0; i<processor.getNumberOfAutomatableNodes(); i++)
    {
        String info = processor.getAutomatableNode(i).nodeName+": "+processor.getAutomatableNode(i).parametername;
        autoCombo.addItem(info, i+1);
        autoCombo.setSelectedId(i+1);
    }
}

//==============================================================================
void AutomationEditor::addTablesOnStartup()
{

    cUtils::debug(processor.getNumberOfAutomatableNodes());
    cUtils::debug(processor.getNumberOfEnvelopes());

    for(int i=0; i<processor.getNumberOfAutomatableNodes(); i++)
    {
        int tableNumber = processor.getAutomatableNode(i).fTableNumber;
        automationDisplay.addNewAutomationEnvelope(cUtils::getRandomColour(), tableNumber);
        const int index = automationDisplay.getNumberOfEnvelopes()-1;


        if(processor.getEnvelope(index).envPoints.size()>0)
        {
            for(int y=0; y<processor.getEnvelope(index).envPoints.size(); y+=2)
            {
                Point<double> point(processor.getEnvelope(index).envPoints[y], 1.0-processor.getEnvelope(index).envPoints[y+1]);
                automationDisplay.getAutomationEnvelope(index)->addHandle(point, false);

                //automationDisplay.automationEnvelopes[i].addHandle(, false);
            }

        }
        //else
        //	automationDisplay.getAutomationEnvelope(index)->createEnvStartEndPoint();
    }
}
//==============================================================================
void AutomationEditor::changeListenerCallback(ChangeBroadcaster* source)
{

}
//==============================================================================
void AutomationEditor::addTable(Colour tableColour, int tableNumber)
{

    automationDisplay.addNewAutomationEnvelope(tableColour, tableNumber);

}

//==============================================================================
void AutomationEditor::timerCallback()
{
    automationDisplay.setScrubberPos(processor.getScrubberPosition());
}
//==============================================================================
void AutomationEditor::buttonClicked(Button* button)
{
    if(button->getName()=="playButton")
    {
        if(button->getToggleState()==true)
        {
            processor.suspendProcessing(false);
            startTimer(100);
            getFilter()->messageQueue.addOutgoingChannelMessageToQueue("isPlaying", 1.0, "");
        }
        else
        {
            getFilter()->messageQueue.addOutgoingChannelMessageToQueue("isPlaying", 0.0, "");
            //processor.suspendProcessing(true);
            stopTimer();

        }

        getFilter()->isSourcePlaying=!getFilter()->isSourcePlaying;
    }

    else if(button->getName()=="stopButton")
    {
        stopTimer();
        if(playButton.getToggleState()==true)
            playButton.setToggleState(false, dontSendNotification);

        getFilter()->messageQueue.addOutgoingChannelMessageToQueue("isPlaying", 0.0, "");
        getFilter()->isSourcePlaying=false;

    }

    else if(button->getName()=="zoomInButton")
    {
        zoom=jmin(1.0, zoom+.1);
        automationDisplay.setZoomFactor(zoom);
    }

    else if(button->getName()=="zoomOutButton")
    {
        zoom=jmin(0.0, zoom-.1);
        automationDisplay.setZoomFactor(zoom);
    }

}

void AutomationEditor::comboBoxChanged (ComboBox* combo)
{
    if(combo->getSelectedItemIndex()>-1 && combo->getSelectedItemIndex()>0<=automationDisplay.getNumberOfEnvelopes())
        automationDisplay.bringEnvelopeToFront(combo->getSelectedItemIndex());
}

//==============================================================================
void AutomationEditor::updateScrubberPosition(double position)
{
    //tableManager.setScrubberPos(position, tableNumber);
}
//================================================================================
