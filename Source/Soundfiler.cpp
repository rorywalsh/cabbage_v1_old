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
// soundfiler display  component
//==============================================================================

Soundfiler::Soundfiler(int sr, Colour col, Colour fcol):	thumbnailCache (5), 
														colour(col),														sampleRate(sr),
														currentPlayPosition(0),
														mouseDownX(0),
														mouseUpX(0),
														drawWaveform(false),
														regionWidth(1),
														loopLength(0),
														scrubberPosition(0),
														fontcolour(fcol),
														currentPositionMarker(new DrawableRectangle())
{	
    formatManager.registerBasicFormats();  
	thumbnail = new AudioThumbnail(2, formatManager, thumbnailCache); 
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
Soundfiler::~Soundfiler()
{
	scrollbar->removeListener (this);
	thumbnail->removeChangeListener (this);
}
//==============================================================================	
void Soundfiler::changeListenerCallback(ChangeBroadcaster *source)
{
	repaint();
	Logger::writeToLog("Change listener");
}
//==============================================================================
void Soundfiler::resized()
{
	if(scrollbar)
		scrollbar->setBounds (getLocalBounds().removeFromBottom (20).reduced (2));
}
//==============================================================================	    
void Soundfiler::scrollBarMoved (ScrollBar* scrollBarThatHasMoved, double newRangeStart)
{
	if (scrollBarThatHasMoved == scrollbar)
		setRange (visibleRange.movedToStartAt (newRangeStart));
}
	
void Soundfiler::setFile (const File& file)
{
   if (! file.isDirectory())
     {
            thumbnail->setSource (new FileInputSource (file));
            const Range<double> newRange (0.0, thumbnail->getTotalLength());
            scrollbar->setRangeLimits (newRange);
            setRange (newRange);
      }
	repaint(0, 0, getWidth(), getHeight());
}

//==============================================================================
void Soundfiler::setWaveform(AudioSampleBuffer buffer)
{            
	thumbnail->clear();	
	repaint();
	thumbnail->reset(2, 44100, buffer.getNumSamples());	
	//thumbnail->clear();
	thumbnail->addBlock(0, buffer, 0, buffer.getNumSamples());
	const Range<double> newRange (0.0, thumbnail->getTotalLength());
	scrollbar->setRangeLimits (newRange);
	setRange (newRange);
	setZoomFactor(zoom);
	repaint();
	Logger::writeToLog("updating waveform");
}

//==============================================================================
void Soundfiler::setZoomFactor (double amount)
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
void Soundfiler::setRange(Range<double> newRange)
{
	visibleRange = newRange;
	scrollbar->setCurrentRange (visibleRange);
	repaint();
}
//==============================================================================
void Soundfiler::paint (Graphics& g)
{
		g.fillAll (Colours::black);
		g.setColour (colour);	
        if (thumbnail->getTotalLength() > 0.01)
        {			
            Rectangle<int> thumbArea (getLocalBounds());
            thumbArea.setHeight(getHeight()-14);
			thumbArea.setTop(10.f);
            thumbnail->drawChannels(g, thumbArea.reduced (2),
                                    visibleRange.getStart(), visibleRange.getEnd(), .8f);
			
			/*
			for(int i=0, offset=0;i<getWidth();i+=(getWidth()/visibleRange.getLength()/4), offset++){
			g.setColour (fontcolour);
			//float pos = visibleRange.getStart()+i;	
			//float xPos = (jmax(1, i)/visibleRange.getStart()) / (visibleRange.getLength() * getWidth());
			if(offset%2==0)
			{
				String test = CabbageUtils::setDecimalPlaces(xToTime(i), 1);
				g.setFont(8);
				g.drawFittedText(test, i, 0, 20, 5, Justification::left, 1);
				g.drawVerticalLine(i+1, 5, 10);
			}	
			
			g.drawVerticalLine(i, 5, 10);
			}
			*/
        
	
		if(regionWidth>1){
		g.setColour(colour.contrasting(.5f).withAlpha(.7f));
		float zoomFactor = thumbnail->getTotalLength()/visibleRange.getLength();
		//regionWidth = (regionWidth=2 ? 2 : regionWidth*zoomFactor)
		g.fillRect(timeToX(currentPlayPosition), 10.f, (regionWidth==2 ? 2 : regionWidth*zoomFactor), (float)getHeight()-26.f);	
		}

		}
	else
		{
			g.setColour(Colours::whitesmoke);
			g.setFont (14.0f);
			g.drawFittedText ("(No audio file loaded)", getLocalBounds(), Justification::centred, 2);
		}
}

//==============================================================================
void Soundfiler::mouseWheelMove (const MouseEvent&, const MouseWheelDetails& wheel)
{
	if (thumbnail->getTotalLength() > 0.5)
	{
		double newStart = visibleRange.getStart() - wheel.deltaX * (visibleRange.getLength()) / 10.0;
		newStart = jlimit (0.0, jmax (0.0, thumbnail->getTotalLength() - (visibleRange.getLength())), newStart);

		setRange (Range<double> (newStart, newStart + visibleRange.getLength()));

		repaint();
	}
}
//==============================================================================
void Soundfiler::mouseDown (const MouseEvent& e)
{
	Logger::writeToLog("mouseDown soundfiler");
	if(e.mods.isLeftButtonDown())
		{
		regionWidth = 2;	
		currentPlayPosition = jmax (0.0, xToTime ((float) e.x));
		loopStart = e.x;
		loopLength =  0;
		repaint();
		sendChangeMessage();
		}
}
//==============================================================================
void Soundfiler::mouseDrag(const MouseEvent& e)
{
	if(this->getLocalBounds().contains(e.getPosition()))
	{
	if(e.mods.isLeftButtonDown())
		{
		double zoomFactor = visibleRange.getLength()/thumbnail->getTotalLength();
		regionWidth = abs(e.getDistanceFromDragStartX())*zoomFactor;
		Logger::writeToLog(String(e.getDistanceFromDragStartX()));
		if(e.getDistanceFromDragStartX()<0)
		currentPlayPosition = jmax (0.0, xToTime (loopStart+(float)e.getDistanceFromDragStartX()));
		float widthInTime = ((float)e.getDistanceFromDragStartX() / (float)getWidth()) * (float)thumbnail->getTotalLength();
		loopLength = jmax (0.0, widthInTime*zoomFactor);	
		}	
	repaint();	
	}
}
//==============================================================================
void Soundfiler::setScrubberPos(double pos){
currentPositionMarker->setVisible (true);
pos = (pos/(thumbnail->getTotalLength()*sampleRate))*thumbnail->getTotalLength();
currentPositionMarker->setRectangle (Rectangle<float> (timeToX (pos) - 0.75f, 10,
                                                              1.5f, (float) (getHeight() - scrollbar->getHeight()-10)));
}
//==============================================================================
void Soundfiler::mouseUp(const MouseEvent& e)
{
	sendChangeMessage();
}
