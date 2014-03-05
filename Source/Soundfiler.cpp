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
														loopLength(0),
														scrubberPosition(0),
														currentPositionMarker(new DrawableRectangle())
{	
    formatManager.registerBasicFormats();  
	thumbnail = new AudioThumbnail(512, formatManager, thumbnailCache); 
	thumbnail->addChangeListener (this);
	//setSize(400, 200);
	sampleRate = sr;
	addAndMakeVisible(scrollbar = new ScrollBar(false));
	scrollbar->setRangeLimits (visibleRange);
	scrollbar->setAutoHide (false);
	scrollbar->addListener(this);
	currentPositionMarker->setFill (Colours::white.withAlpha (0.85f));
	addAndMakeVisible(currentPositionMarker);
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
	
void WaveformDisplay::setFile (const File& file)
{
   if (! file.isDirectory())
     {
            thumbnail->setSource (new FileInputSource (file));
            const Range<double> newRange (0.0, thumbnail->getTotalLength());
            scrollbar->setRangeLimits (newRange);
            setRange (newRange);
      }

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
		zoom = amount;
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
        if (thumbnail->getTotalLength() > 0.0)
        {
			g.fillAll (Colours::black);
			g.setColour (colour);			
            Rectangle<int> thumbArea (getLocalBounds());
            thumbArea.removeFromBottom (scrollbar->getHeight() + 4);
            thumbnail->drawChannels(g, thumbArea.reduced (2),
                                    visibleRange.getStart(), visibleRange.getEnd(), .8f);
			//Logger::writeToLog("drawing waveform");
			//Logger::writeToLog(CabbageUtils::getBoundsString(getLocalBounds()));
        
	
		if(regionWidth>1){
		g.setColour(colour.contrasting(.5f).withAlpha(.7f));
		float zoomFactor = thumbnail->getTotalLength()/visibleRange.getLength();
		//regionWidth = (regionWidth=2 ? 2 : regionWidth*zoomFactor)
		g.fillRect(timeToX(currentPlayPosition), 0.f, (regionWidth==2 ? 2 : regionWidth*zoomFactor), (float)getHeight()-16.f);	
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
void WaveformDisplay::mouseWheelMove (const MouseEvent&, const MouseWheelDetails& wheel)
{
	if (thumbnail->getTotalLength() > 0.0)
	{
		double newStart = visibleRange.getStart() - wheel.deltaX * (visibleRange.getLength()) / 10.0;
		newStart = jlimit (0.0, jmax (0.0, thumbnail->getTotalLength() - (visibleRange.getLength())), newStart);

		setRange (Range<double> (newStart, newStart + visibleRange.getLength()));

		repaint();
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
	loopLength =  0;//getCurrentPlayPosInSamples();
	mouseDownX = e.x;
	reDraw = true;
	repaint();
	sendChangeMessage();
}
//==============================================================================
void WaveformDisplay::mouseDrag(const MouseEvent& e)
{
	if(e.mods.isRightButtonDown())
		{
		double zoomFactor = visibleRange.getLength()/thumbnail->getTotalLength();
		regionWidth = e.getDistanceFromDragStartX()*zoomFactor;
		float widthInTime = ((float)e.getDistanceFromDragStartX() / (float)getWidth()) * (float)thumbnail->getTotalLength();
		loopLength = jmax (0.0, widthInTime*zoomFactor);	
		}	
	reDraw = true;
	repaint();
}
//==============================================================================
void WaveformDisplay::setScrubberPos(double pos){
currentPositionMarker->setVisible (true);
pos = (pos/(thumbnail->getTotalLength()*sampleRate))*thumbnail->getTotalLength();
currentPositionMarker->setRectangle (Rectangle<float> (timeToX (pos) - 0.75f, 0,
                                                              1.5f, (float) (getHeight() - scrollbar->getHeight())));
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
	waveformDisplay->setFile(File(fileName));
	waveformDisplay->setZoomFactor(zoom);
}

//==============================================================================
void Soundfiler::changeListenerCallback(ChangeBroadcaster *source)
{
	position = waveformDisplay->getCurrentPlayPosInSamples();
	endPosition = waveformDisplay->getLoopLengthInSamples();
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

