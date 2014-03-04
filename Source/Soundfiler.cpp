/*
  Copyright (C) 2009 Rory Walsh

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

#include "Soundfiler.h"

//==============================================================================
// waveform display  component
//==============================================================================

WaveformDisplay::WaveformDisplay(int sr, Colour col):	thumbnailCache (5), 
														colour(col),														sampleRate(sr),
														currentPlayPosition(0),
														mouseDownX(0),
														mouseUpX(0),
														drawWaveform(false),
														regionWidth(1),
														loopEnd(0),
														scrubberPosition(0)
{	
    formatManager.registerBasicFormats();  
	thumbnail = new AudioThumbnail(512, formatManager, thumbnailCache); 
	thumbnail->addChangeListener (this);
	setSize(400, 200);
	sampleRate = sr;
	addAndMakeVisible(scrollbar = new ScrollBar(false));
	scrollbar->setRangeLimits (visibleRange);
	scrollbar->setAutoHide (false);
	scrollbar->addListener(this);
}
//==============================================================================	
WaveformDisplay::~WaveformDisplay()
{
	scrollbar->removeListener (this);
	thumbnail->removeChangeListener (this);
}
//==============================================================================	
void WaveformDisplay::changeListenerCallback(ChangeBroadcaster *source)
{
	repaint();
}
//==============================================================================
void WaveformDisplay::resized()
{
	if(scrollbar)
		scrollbar->setBounds (getLocalBounds().removeFromBottom (20).reduced (2));
}
//==============================================================================	    
void WaveformDisplay::scrollBarMoved (ScrollBar* scrollBarThatHasMoved, double newRangeStart)
{
	if (scrollBarThatHasMoved == scrollbar)
		setRange (visibleRange.movedToStartAt (newRangeStart));
}
	
void WaveformDisplay::setFile (const File& file, bool firstTime)
{
	if(file.existsAsFile()){
		if(firstTime){
		thumbnail->setSource (new FileInputSource(file));
		}
		else{
		//FileInputStream* inFileStream = new FileInputStream(file); // jatFile is the saved thumbnail file
		thumbnail = nullptr;
		thumbnail = new AudioThumbnail(512, formatManager, thumbnailCache); 
		thumbnail->setSource (new FileInputSource(file));
		const Range<double> newRange (0.0, thumbnail->getTotalLength());
		scrollbar->setRangeLimits (newRange);
		setRange (newRange);
		}
		drawWaveform=true;
	}
	else
		drawWaveform = false;

	startTime = 0;
	endTime = thumbnail->getTotalLength();
	repaint();
}
//==============================================================================
void WaveformDisplay::setZoomFactor (double amount)
{
        if (thumbnail->getTotalLength() > 0)
        {
            const double newScale = jmax (0.001, thumbnail->getTotalLength() * (1.0 - jlimit (0.0, 0.99, amount)));
            const double timeAtCentre = xToTime (getWidth() / 2.0f);
            setRange (Range<double> (timeAtCentre - newScale * 0.5, timeAtCentre + newScale * 0.5));
        }
		repaint();
}
//==============================================================================
void WaveformDisplay::setRange(Range<double> newRange)
{
	visibleRange = newRange;
	scrollbar->setCurrentRange (visibleRange);
	repaint();
}
//==============================================================================
void WaveformDisplay::paint (Graphics& g)
{
	g.fillAll (Colours::black);
	if(drawWaveform)
		{
		
		g.setColour (colour);
        if (thumbnail->getTotalLength() > 0.0)
        {
            Rectangle<int> thumbArea (getLocalBounds());
            thumbArea.removeFromBottom (scrollbar->getHeight() + 4);
            thumbnail->drawChannels (g, thumbArea.reduced (2),
                                    visibleRange.getStart(), visibleRange.getEnd(), 1.0f);
        }
	
		if(regionWidth>1){
		g.setColour(colour.contrasting(.5f).withAlpha(.7f));
		g.fillRect(timeToX(currentPlayPosition), 0.f, regionWidth, (float)getHeight());	
		}

		}
	else
		{
			g.setColour(Colours::whitesmoke);
			g.setFont (14.0f);
			g.drawFittedText ("(No audio file selected)", getLocalBounds(), Justification::centred, 2);
		}
}

//==============================================================================
void WaveformDisplay::mouseDown (const MouseEvent& e)
{
	if(e.mods.isCommandDown()){
	if(e.mods.isLeftButtonDown())
		this->setSize(jmin(1000, this->getWidth()+100), getHeight());
	else
		this->setSize(jmax(400, this->getWidth()-100), getHeight());
		Logger::writeToLog("command down");
	}
	regionWidth = 2;
	
	currentPlayPosition = jmax (0.0, xToTime ((float) e.x));
	loopEnd =  0;//getCurrentPlayPosInSamples();
	mouseDownX = e.x;
	reDraw = true;
	repaint();
	sendChangeMessage();
}
//==============================================================================
void WaveformDisplay::mouseDrag(const MouseEvent& e)
{
	//mouseUpX = e.x;
	//currentPlayPosition = jmax (0.0, xToTime ((float) e.x));
	if(e.mods.isRightButtonDown())
		{
		regionWidth = e.getDistanceFromDragStartX();
		loopEnd = jmax (0.0, xToTime ((float) e.getDistanceFromDragStartX()));
		}	
	reDraw = true;
	repaint();
}

//==============================================================================
void WaveformDisplay::mouseUp(const MouseEvent& e)
{
	sendChangeMessage();
}


//==============================================================================
// soundfiler component
//==============================================================================
Soundfiler::Soundfiler(String fileName, int sr, Colour colour, Colour fontColour, int zoom)
{
	Logger::writeToLog("sample Rate:"+String(sr));
	waveformDisplay = new WaveformDisplay(sr, colour);
	waveformDisplay->addChangeListener(this);
	addAndMakeVisible(waveformDisplay);
    setSize (400, 300);
	waveformDisplay->setFile(File(fileName), true);
	waveformDisplay->setZoomFactor(zoom);
}

//==============================================================================
void Soundfiler::changeListenerCallback(ChangeBroadcaster *source)
{
	position = waveformDisplay->getCurrentPlayPosInSamples();
	endPosition = waveformDisplay->getLoopEndInSamples();
	sendChangeMessage();
}

//==============================================================================
void Soundfiler::resized()
{
waveformDisplay->setSize(getWidth(), getHeight());
}

//==============================================================================
void Soundfiler::setScrubberPosition(int pos)
{
waveformDisplay->setScrubberPos(pos);
}
//==============================================================================
void Soundfiler::paint (Graphics& g)
{
//g.fillAll(Colour(20, 20, 20));
}

