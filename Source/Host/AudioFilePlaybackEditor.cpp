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

#include "AudioFilePlaybackProcessor.h"
#include "AudioFilePlaybackEditor.h"


#define BUTTON_SIZE 25
WaveformDisplay::WaveformDisplay(AudioFormatManager& formatManager, BufferingAudioSource *source, int sr, Colour col):
thumbnailCache(15),
thumbnail (16, formatManager, thumbnailCache),
source(source),
tableColour(col),
sampleRate(sr),
scrollbar(false),
currentPlayPosition(0)
{
    thumbnail.addChangeListener(this);
	addAndMakeVisible(gainEnvelope);
    currentPositionMarker.setFill (Colours::lime);
    addAndMakeVisible(currentPositionMarker);
    addAndMakeVisible (scrollbar);
    scrollbar.setRangeLimits (visibleRange);
    scrollbar.setAutoHide (false);
    scrollbar.addListener (this);
	setOpaque(true);
	resized();
}

WaveformDisplay::~WaveformDisplay()
{
    stopTimer();
}

void WaveformDisplay::resized()
{
    scrollbar.setBounds (getLocalBounds().removeFromBottom (14).reduced (2));
	gainEnvelope.setSize(getWidth(), getHeight()-14);
}

void WaveformDisplay::setScrubberPos(double pos)
{
    currentPositionMarker.setVisible (true);
    //pos = (pos/(thumbnail.getTotalLength()*sampleRate))*thumbnail.getTotalLength();
    currentPositionMarker.setRectangle (Rectangle<float> (timeToX (pos) - 0.75f, 0,
                                                          1.5f, (float) (getHeight() - (scrollbar.getHeight()+5))));
	this->getEditor()->setCurrentPosition(String(pos, 4));
}

void WaveformDisplay::changeListenerCallback (ChangeBroadcaster*)
{
    repaint();
}

void WaveformDisplay::showGainEnvelope(bool show)
{
	gainEnvelope.setVisible(show);
}


void WaveformDisplay::setFile (const File& file)
{
    AudioFormatManager format;
    format.registerBasicFormats();
    AudioFormatReader* reader = format.createReaderFor(file);
    
    if(reader)
    {
        AudioSampleBuffer buffer(reader->numChannels, reader->lengthInSamples);
        buffer.clear();
        buffer.setSize(reader->numChannels, reader->lengthInSamples);
        reader->read(&buffer,0, buffer.getNumSamples(), 0, true, true);
        setWaveform(buffer, reader->numChannels);
    }
    
    delete reader;
}

void WaveformDisplay::setWaveform(AudioSampleBuffer buffer, int channels)
{
    thumbnail.clear();
    thumbnail.reset(channels, 44100, buffer.getNumSamples());
    thumbnail.addBlock(0, buffer, 0, buffer.getNumSamples());
    const Range<double> newRange (0.0, thumbnail.getTotalLength());
    scrollbar.setRangeLimits (newRange);
    setRange (newRange);
}

void WaveformDisplay::setZoomFactor (double amount)
{
    if (thumbnail.getTotalLength() > 0)
    {
        const double newScale = jmax (0.001, thumbnail.getTotalLength() * (1.0 - jlimit (0.0, 0.99, amount)));
        const double timeAtCentre = xToTime (getWidth() / 2.0f);
        setRange (Range<double> (timeAtCentre - newScale * 0.5, timeAtCentre + newScale * 0.5));
    }
}

void WaveformDisplay::setRange (Range<double> newRange)
{
    visibleRange = newRange;
    scrollbar.setCurrentRange (visibleRange);
	
	const double visibleStart = visibleRange.getStart();
	const double visibleEnd = visibleRange.getEnd();
	const double visibleLength = visibleRange.getLength();
	
	const double newWidth = double(getWidth())*(double(thumbnail.getTotalLength())/visibleLength);
	const double leftOffset = newWidth*(visibleStart/(double)thumbnail.getTotalLength());
	gainEnvelope.setSize(newWidth, gainEnvelope.getHeight());
	gainEnvelope.setTopLeftPosition(-leftOffset, 0);	
	
	
    repaint();
}

void WaveformDisplay::paint (Graphics& g)
{
    g.fillAll (Colour(10, 10, 10));
    g.setColour (tableColour);
    
    
    
    if (thumbnail.getTotalLength() > 0)
    {
        Rectangle<int> thumbArea (getLocalBounds());
        thumbArea.removeFromBottom (scrollbar.getHeight() + 4);
        thumbnail.drawChannels (g, thumbArea.reduced (2),
                                visibleRange.getStart(), visibleRange.getEnd(), 1.0f);
    }
    else
    {
        g.setFont (14.0f);
        g.drawFittedText ("(No audio file selected)", getLocalBounds(), Justification::centred, 2);
    }
    
}

void WaveformDisplay::timerCallback()
{
    if(thumbnail.getTotalLength()>0 && getEditor()->getFilter()->isSourcePlaying==true)
    {
        currentPlayPosition = source->getNextReadPosition()/sampleRate;
        setScrubberPos(currentPlayPosition);
    }
}

void WaveformDisplay::mouseDown (const MouseEvent& e)
{
    if(thumbnail.getTotalLength()>0)
    {
        source->setNextReadPosition (jmax (0.0, xToTime ((float) e.x)*sampleRate));
        currentPlayPosition = jmax (0.0, xToTime ((float) e.x));
        setScrubberPos(currentPlayPosition);
    }
}

void WaveformDisplay::mouseDrag (const MouseEvent& e)
{
    if(thumbnail.getTotalLength()>0)
    {
        source->setNextReadPosition (jmax (0.0, xToTime ((float) e.x)*sampleRate));
        currentPlayPosition = jmax (0.0, xToTime ((float) e.x));
        setScrubberPos(currentPlayPosition);
    }
}

void WaveformDisplay::resetPlaybackPosition()
{
	setScrubberPos(0);
}

float WaveformDisplay::timeToX (const double time) const
{
    return getWidth() * (float) ((time - visibleRange.getStart()) / (visibleRange.getLength()));
}

double WaveformDisplay::xToTime (const float x) const
{
    return (x / getWidth()) * (visibleRange.getLength()) + visibleRange.getStart();
}

void WaveformDisplay::scrollBarMoved (ScrollBar* scrollBarThatHasMoved, double newRangeStart)
{
    if (scrollBarThatHasMoved == &scrollbar)
        setRange (visibleRange.movedToStartAt (newRangeStart));
}

//------------------------------------------------------------------------------
WaveformDisplay::GainEnvelope::GainEnvelope()
{

}

void WaveformDisplay::GainEnvelope::createGainEnvStartEndPoint()
{
	Handle* leftMostHandle = new Handle(this);
	addAndMakeVisible(leftMostHandle);
	leftMostHandle->setRelativePosition(Point<double>(0, 0), 1, 1);
	Handle* rightMostHandle = new Handle(this);
	addAndMakeVisible(rightMostHandle);
	rightMostHandle->setRelativePosition(Point<double>(1, 0), 1, 1);
	handles.add(leftMostHandle);
	handles.add(rightMostHandle);		
}

void WaveformDisplay::GainEnvelope::mouseDown(const MouseEvent& e)
{
	int indx;
	const int x = e.getPosition().getX();
	for (int i=1; i<handles.size(); i++)
	{
		cUtils::debug("handle0X", handles[i-1]->getX());
		cUtils::debug("handle1X", handles[i]->getX());
		cUtils::debug("mouseX", x);
		if (x >= handles[i-1]->getX() && x < handles[i]->getX())
		{
			indx = i;
		}
	}	
	
	Handle* handle = new Handle(this);		
	addAndMakeVisible(handle);
	handle->setTopLeftPosition(e.getPosition().getX(), e.getPosition().getY());
	handle->setRelativePosition(e.getPosition().toDouble(), getWidth(), getHeight());
	handles.insert(indx, handle);
	repaint();
	resized();
}

void WaveformDisplay::GainEnvelope::addHandle(Point<double> pos, bool resize)
{
	Handle* handle = new Handle(this);		
	addAndMakeVisible(handle);
	handle->setRelativePosition(Point<double>(pos.getX(), pos.getY()), 1.0, 1.0);
	handles.add(handle);
	repaint();
	if(resize)
		resized();	
} 

int WaveformDisplay::GainEnvelope::getHandleIndex(Handle* thisHandle)
{
    return handles.indexOf(thisHandle);
}

void WaveformDisplay::GainEnvelope::removeHandle(Handle* handle)
{
    if (handles.size() > 0)
    {
        handles.removeObject(handle, true);
    }	
	
	repaint();
	resized();
}
	
Handle* WaveformDisplay::GainEnvelope::getLastHandle()
{
    return handles.getUnchecked(handles.size()-1);
}

Handle* WaveformDisplay::GainEnvelope::getFirstHandle()
{
    return handles.getUnchecked(0);
}

Handle* WaveformDisplay::GainEnvelope::getPreviousHandle(Handle* thisHandle)
{
    int thisHandleIndex = handles.indexOf(thisHandle);

    if(thisHandleIndex <= 0)
        return 0;
    else
        return handles.getUnchecked(thisHandleIndex-1);
}

Handle* WaveformDisplay::GainEnvelope::getNextHandle(Handle* thisHandle)
{
    int thisHandleIndex = handles.indexOf(thisHandle);

    if(thisHandleIndex == -1 || thisHandleIndex >= handles.size()-1)
        return 0;
    else
        return handles.getUnchecked(thisHandleIndex+1);
}

void WaveformDisplay::GainEnvelope::resized()
{
	getEditor()->getFilter()->clearEnvDataPoint();
	for(int i=0;i<handles.size();i++)
	{
		const double xPos = handles[i]->getRelativePosition().getX()*getWidth()-((i==0 || i==handles.size()-1) ? 4 : 0);
		const double yPos = handles[i]->getRelativePosition().getY()*getHeight();
		handles[i]->setTopLeftPosition(xPos, yPos);
		getEditor()->getFilter()->addEnvDataPoint(handles[i]->getRelativePosition());
	}
}
 
void WaveformDisplay::GainEnvelope::paint(Graphics& g)
{			
	g.fillAll(Colours::transparentBlack);
	Path path;
	g.setColour(Colours::cornflowerblue);
	path.startNewSubPath(handles[0]->getPosition().translated(2.5, 2.5).toFloat());
	for(int i=0;i<handles.size()-1;i++)
	{
		path.lineTo(handles[i]->getPosition().translated(4, 4).toFloat());
	}
	path.lineTo(handles[handles.size()-1]->getPosition().translated(4, 4).toFloat());
	g.strokePath(path, PathStrokeType(2));
}
	
//==============================================================================
AudioFilePlaybackEditor::AudioFilePlaybackEditor (AudioFilePlaybackProcessor* ownerFilter):
AudioProcessorEditor (ownerFilter),
playButton("playButton", DrawableButton::ImageOnButtonBackground),
stopButton("stopButton", DrawableButton::ImageOnButtonBackground),
openButton("openButton", DrawableButton::ImageOnButtonBackground),
zoomInButton("zoomInButton", DrawableButton::ImageOnButtonBackground),
loopButton("loopButton", DrawableButton::ImageOnButtonBackground),
zoomOutButton("zoomOutButton", DrawableButton::ImageOnButtonBackground),
linkToTransport("linkToTransportButton", DrawableButton::ImageOnButtonBackground),
basicLook(),
beatOffset("beatOffet"),
fileNameLabel(""),
currentPositionLabel(""),
beatOffsetLabel("beatOffsetLabel"),
gainEnvelopeButton("gainEnvelopeButton", DrawableButton::ImageOnButtonBackground),
zoom(0)
{
    AudioFormatManager formatManager;
    formatManager.registerBasicFormats();
    
    tableColour = Colour(Random::getSystemRandom().nextInt(255),
                         Random::getSystemRandom().nextInt(255),
                         Random::getSystemRandom().nextInt(255));
    
	waveformDisplay = new WaveformDisplay(formatManager, getFilter()->bufferingAudioFileSource, getFilter()->sourceSampleRate, tableColour);
	setOpaque(true);
	playButton.addListener(this);
	addAndMakeVisible(playButton);
	stopButton.addListener(this);
	addAndMakeVisible(stopButton);
	openButton.addListener(this);
	addAndMakeVisible(openButton);
	zoomInButton.addListener(this);
	addAndMakeVisible(zoomInButton);
	zoomOutButton.addListener(this);
	addAndMakeVisible(zoomOutButton);	
	linkToTransport.addListener(this);
	addAndMakeVisible(linkToTransport);	
	gainEnvelopeButton.addListener(this);
	addAndMakeVisible(gainEnvelopeButton);	
	loopButton.addListener(this);
	addAndMakeVisible(loopButton);	
	
	playButton.setLookAndFeel(&basicLook);
	stopButton.setLookAndFeel(&basicLook);
	openButton.setLookAndFeel(&basicLook);
	zoomOutButton.setLookAndFeel(&basicLook);
	zoomInButton.setLookAndFeel(&basicLook);
	linkToTransport.setLookAndFeel(&basicLook);
	gainEnvelopeButton.setLookAndFeel(&basicLook);
	loopButton.setLookAndFeel(&basicLook);
	
	
	addAndMakeVisible(beatOffset);
	beatOffset.setSliderStyle(Slider::SliderStyle::LinearBarVertical);
	beatOffset.setRange(0, 99000, 1);
	beatOffset.setValue(0, dontSendNotification);
	beatOffset.addListener(this);
	beatOffset.setVelocityBasedMode(true);
	beatOffset.setColour(Slider::ColourIds::thumbColourId, Colours::black);
	beatOffset.setColour(Slider::ColourIds::textBoxTextColourId, Colours::white);
	beatOffset.setColour(Slider::ColourIds::textBoxBackgroundColourId, Colours::black);	
	
	beatOffsetLabel.setText("Cue:", dontSendNotification);
	zoomOutButton.getProperties().set("isRounded", true);
	zoomInButton.getProperties().set("isRounded", true);
	
	openButton.setColour(TextButton::buttonColourId, Colours::white);	
	playButton.setColour(TextButton::buttonColourId, Colours::white);
	playButton.setColour(TextButton::buttonColourId, Colours::white);
	gainEnvelopeButton.setColour(TextButton::buttonColourId, Colours::white);
	loopButton.setColour(TextButton::buttonColourId, Colours::white);
	loopButton.setColour(TextButton::buttonOnColourId, Colours::cornflowerblue);
	gainEnvelopeButton.setColour(TextButton::buttonOnColourId, Colours::cornflowerblue);
	
	playButton.setColour(TextButton::buttonOnColourId, Colours::yellow);
	zoomOutButton.setColour(TextButton::buttonColourId, Colours::white);
	zoomInButton.setColour(TextButton::buttonColourId, Colours::white);
	linkToTransport.setColour(TextButton::buttonColourId, Colours::green.darker(.9f));
	linkToTransport.setColour(TextButton::buttonOnColourId, Colours::green.darker(.9f));
	
	playButton.setClickingTogglesState(true);	
	loopButton.setClickingTogglesState(true);	
	gainEnvelopeButton.setClickingTogglesState(true);	
	
	linkToTransport.setColour(TextButton::buttonOnColourId, Colours::cornflowerblue);
	
	addAndMakeVisible(fileNameLabel);
	addAndMakeVisible(beatOffsetLabel);
	addAndMakeVisible(currentPositionLabel);
	currentPositionLabel.setJustificationType(Justification::centred);
	beatOffsetLabel.setJustificationType(Justification::right);
	
	fileNameLabel.setInterceptsMouseClicks(false, false);
	fileNameLabel.setJustificationType(Justification::right);
	
	stopButton.setColour(TextButton::buttonColourId, Colours::white);
	
	linkToTransport.setImages(cUtils::createPlayButtonPath(25, Colours::white));
	
	playButton.setImages(cUtils::createPlayButtonPath(25, Colours::green.darker(.9f)), 
						 cUtils::createPlayButtonPath(25, Colours::green.darker(.9f)), 
						 cUtils::createPauseButtonPath(25), 
						 cUtils::createPlayButtonPath(25, Colours::green.darker(.9f)), 
						 cUtils::createPauseButtonPath(25),
						 cUtils::createPauseButtonPath(25));

	openButton.setImages(cUtils::createOpenButtonPath(25));		
	stopButton.setImages(cUtils::createStopButtonPath(25, Colours::green.darker(.9f)),
						 cUtils::createStopButtonPath(25, Colours::green.darker(.9f)),
						 cUtils::createStopButtonPath(25, Colours::green.darker(.9f)),
						 cUtils::createStopButtonPath(25, Colours::green.darker(.9f)),
						 cUtils::createStopButtonPath(25, Colours::green.darker(.9f)),
						 cUtils::createStopButtonPath(25, Colours::green.darker(.9f)),
						 cUtils::createStopButtonPath(25, Colours::green.darker(.9f))
						 );

	
	loopButton.setImages(cUtils::createLoopButtonPath());
	
	gainEnvelopeButton.setImages(cUtils::createEnvelopeButtonPath(Colours::green.darker(.9f)));
	gainEnvelopeButton.setClickingTogglesState(true);
	
	zoomInButton.setImages(cUtils::createZoomInButtonPath(25));
	zoomOutButton.setImages(cUtils::createZoomOutButtonPath(25));
	//waveformDisplay->setBounds(10, 10, 500, 200);
	
	
	if(getFilter()->getEnvPointSize()>0)
	{
		for(int i=0;i<getFilter()->getEnvPointSize();i++)
			waveformDisplay->gainEnvelope.addHandle(getFilter()->getEnvPoint(i), false);
	}
	else
		waveformDisplay->gainEnvelope.createGainEnvStartEndPoint();
		
	addAndMakeVisible(waveformDisplay);
	waveformDisplay->showGainEnvelope(false);
	
	
	

    setSize (500, 250);

	beatOffset.toFront(true);

	if(File(getFilter()->getCurrentFile()).existsAsFile())
	{
		waveformDisplay->setFile(File(getFilter()->getCurrentFile()));
		fileNameLabel.setText(getFilter()->getCurrentFile(), dontSendNotification);
		fileNameLabel.toFront(true);
	}

	if(getFilter()->linkedToMasterTransport()==true)
	{
		linkToTransport.setToggleState(true, dontSendNotification);
		getFilter()->linkToMasterTransport(true);
		getFilter()->isSourcePlaying=true;	
		waveformDisplay->startTimer(50);
		playButton.setEnabled(false);
		stopButton.setEnabled(false);
	}
		
	if(getFilter()->getLooping()==true)			
		loopButton.setToggleState(true, sendNotification);	

	if(getFilter()->shouldShowGainEnv()==true)
		gainEnvelopeButton.setToggleState(true, sendNotification);	
	
}

AudioFilePlaybackEditor::~AudioFilePlaybackEditor()
{
    getFilter()->editorBeingDeleted(this);
	waveformDisplay->stopTimer();
}

//==============================================================================
void AudioFilePlaybackEditor::resized()
{
	waveformDisplay->setBounds(BUTTON_SIZE*2+7, 5, getWidth()-(BUTTON_SIZE*2+12), getHeight()-14);
	//viewport->setBounds(BUTTON_SIZE+7, 5, getWidth()-20, getHeight()-10);	
	stopButton.setBounds(3, 5, BUTTON_SIZE, BUTTON_SIZE);
	playButton.setBounds(3+BUTTON_SIZE, 5, BUTTON_SIZE, BUTTON_SIZE);
	loopButton.setBounds(3, ((BUTTON_SIZE)*1)+5, BUTTON_SIZE, BUTTON_SIZE);
	openButton.setBounds(3+BUTTON_SIZE, ((BUTTON_SIZE)*1)+5, BUTTON_SIZE, BUTTON_SIZE);
	zoomInButton.setBounds(3, ((BUTTON_SIZE)*2)+5, BUTTON_SIZE, BUTTON_SIZE);
	zoomOutButton.setBounds(3+BUTTON_SIZE, ((BUTTON_SIZE)*2)+5, BUTTON_SIZE, BUTTON_SIZE);
	linkToTransport.setBounds(3, ((BUTTON_SIZE)*3)+5, BUTTON_SIZE, BUTTON_SIZE);
	gainEnvelopeButton.setBounds(3+BUTTON_SIZE, ((BUTTON_SIZE)*3)+5, BUTTON_SIZE, BUTTON_SIZE);
	currentPositionLabel.setBounds(0, ((BUTTON_SIZE)*4)+5, (BUTTON_SIZE*2)+7, 12);
	beatOffsetLabel.setBounds(0, getHeight()-45, BUTTON_SIZE*2, 12);
	beatOffset.setBounds(3, getHeight()-30, BUTTON_SIZE*2, 20);
	fileNameLabel.setBounds((getWidth()/2), 7, (getWidth()/2)-5, 14);
}
//==============================================================================
void AudioFilePlaybackEditor::paint (Graphics& g)
{
	g.fillAll(Colours::black);
    g.setColour(tableColour);
    g.drawRect(0, 0, getWidth(), getHeight());
}
//==============================================================================
void AudioFilePlaybackEditor::itemDropped (const DragAndDropTarget::SourceDetails& dragSourceDetails)
{
	if(FileTreeComponent* fileComp = dynamic_cast<FileTreeComponent*>(dragSourceDetails.sourceComponent.get()))			
	{
		getFilter()->setupAudioFile(fileComp->getSelectedFile());
		getFilter()->prepareToPlay(0, 512);
		if(getFilter()->bufferingAudioFileSource)
		{
			waveformDisplay->source = getFilter()->bufferingAudioFileSource;
			waveformDisplay->setFile(fileComp->getSelectedFile());	
		}	
	}
}

void AudioFilePlaybackEditor::sliderValueChanged (Slider* slider)
{
	getFilter()->setBeatOffset(slider->getValue());
}	

//==============================================================================
void AudioFilePlaybackEditor::buttonClicked(Button *button)
{
	
	if(button->getName()=="playButton")
	{
		if(getFilter()->bufferingAudioFileSource)
		{
			if(!getFilter()->isSourcePlaying)
				waveformDisplay->startTimer(50);
			else
				waveformDisplay->stopTimer();

			getFilter()->isSourcePlaying=!getFilter()->isSourcePlaying;			
		}
	}

    else if(button->getName()=="zoomInButton")
    {
        zoom=jmin(1.0, zoom+.1);
        
        waveformDisplay->setZoomFactor(zoom);
    }
    
    else if(button->getName()=="zoomOutButton")
    {
        zoom=jmin(0.0, zoom-.1);
        waveformDisplay->setZoomFactor(zoom);
    }
    
    else if(button->getName()=="gainEnvelopeButton")
    {
		waveformDisplay->showGainEnvelope(button->getToggleState());
		getFilter()->setShowGainEnv(button->getToggleState());
    }
	
    else if(button->getName()=="loopButton")
    {
		getFilter()->setLooping(button->getToggleState());
    }
	
	else if(button->getName()=="stopButton")
	{
		if(getFilter()->bufferingAudioFileSource)
		{
			playButton.setToggleState(false, dontSendNotification);
			waveformDisplay->stopTimer();
			getFilter()->isSourcePlaying=false;
			getFilter()->resetGainEnv();
			waveformDisplay->resetPlaybackPosition();
			getFilter()->setBufferingReadPosition(0);
		}	
	}
	
	else if(button->getName()=="openButton")
	{
	    FileChooser fc ("Open file");

	    if (fc.browseForFileToOpen())
		{
			getFilter()->setupAudioFile(fc.getResult());
			getFilter()->prepareToPlay(0, 512);
			if(getFilter()->bufferingAudioFileSource)
			{
				waveformDisplay->source = getFilter()->bufferingAudioFileSource;
				waveformDisplay->setFile(fc.getResult());	
			}

		}	
	}
			
	else if(button->getName()=="linkToTransportButton")
	{
		if(button->getToggleState()==true)
		{
			waveformDisplay->resetPlaybackPosition();
			getFilter()->setBufferingReadPosition(0);	
			getFilter()->resetGainEnv();
			playButton.setToggleState(false, dontSendNotification);
			button->setToggleState(false, dontSendNotification);
			getFilter()->linkToMasterTransport(false);
			getFilter()->isSourcePlaying=false;	
			playButton.setEnabled(true);
			stopButton.setEnabled(true);
		}
		else
		{
			waveformDisplay->resetPlaybackPosition();
			getFilter()->resetGainEnv();
			waveformDisplay->startTimer(50);
			getFilter()->setBufferingReadPosition(0);	
			playButton.setToggleState(false, dontSendNotification);
			button->setToggleState(true, dontSendNotification);
			getFilter()->linkToMasterTransport(true);
			getFilter()->isSourcePlaying=true;	
			playButton.setEnabled(false);
			stopButton.setEnabled(false);
		}
	}
}
